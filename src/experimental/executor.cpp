#include <experimental/executor>

namespace std::experimental::net
{
inline namespace v1
{
execution_context::execution_context() : service_registry_(*this)
{
}

execution_context::~execution_context()
{
    shutdown();
    destroy();
}

execution_context::_Service_registry::_Service_registry(execution_context& owner) : owner_(owner) {}

void execution_context::_Service_registry::shutdown_services()
{
    for (auto& svc : services_)
    {
        svc.second->shutdown();
    }
}

void execution_context::_Service_registry::destroy_services()
{
    services_.clear();
}

void execution_context::_Service_registry::notify_fork(fork_event fork_ev)
{
    if (fork_ev == fork_event::prepare)
    {
        auto begin{ services_.rbegin() };
        auto end{ services_.rend() };
        for (; begin != end; ++begin)
        {
            (*begin).second->notify_fork(fork_ev);
        }
    }
    else
    {
        auto begin{ services_.begin() };
        auto end{ services_.end() };
        for (; begin != end; ++begin)
        {
            (*begin).second->notify_fork(fork_ev);
        }
    }
}

_Thread_pool::_Thread_pool() : stopped_(true)
{
    threads_ = deque<thread>(thread::hardware_concurrency());
}

void _Thread_pool::start()
{
    for (thread& t : threads_)
    {
        t = thread([this]() { do_job(); });
    }
}

void _Thread_pool::post(function<void()>&& f)
{
    {
        lock_guard<mutex> lock{ jobs_mutex_ };
        jobs_.emplace_back(move(f));
    }
    cond_.notify_one();
}

void _Thread_pool::stop()
{
    if (!stopped_)
    {
        stopped_ = true;
        cond_.notify_all();
    }
}

void _Thread_pool::join()
{
    if (!stopped_)
    {
        for (thread& t : threads_)
        {
            t.join();
        }
    }
}

void _Thread_pool::do_job()
{
    while (true)
    {
        if (jobs_.empty())
        {
            unique_lock<mutex> lock{ cond_mutex_ };
            cond_.wait(lock);
        }
        if (stopped_)
            break;
        function<void()> f;
        {
            lock_guard<mutex> lock{ jobs_mutex_ };
            if (!jobs_.empty())
            {
                f = move(jobs_.front());
                jobs_.pop_front();
            }
        }
        if (f)
            f();
    }
}

system_context::system_context()
{
    pool_.start();
}

system_context::~system_context()
{
    pool_.stop();
    pool_.join();
}

system_context& system_executor::context() const noexcept
{
    static system_context instance{};
    return instance;
}
} // namespace v1
} // namespace std::experimental::net
