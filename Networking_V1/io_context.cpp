#include <experimental/io_context>

namespace std::experimental::net
{
inline namespace v1
{
struct _Io_context_monitor
{
    _Io_context_monitor(io_context& ctx) : ctx_(&ctx)
    {
        lock_guard<mutex> lock{ ctx_->mtx_ };
        ctx_->call_stack_.push_back(this_thread::get_id());
    }
    ~_Io_context_monitor()
    {
        lock_guard<mutex> lock{ ctx_->mtx_ };
        ctx_->call_stack_.erase(find(ctx_->call_stack_.begin(), ctx_->call_stack_.end(), this_thread::get_id()));
    }

private:
    io_context* ctx_;
};

size_t io_context::_Do_one(DWORD msec)
{
    DWORD n;
    ULONG_PTR key{ 0 };
    _Io_operation* p;
    _Io_context_monitor mon{ *this };
    if (::GetQueuedCompletionStatus(port_, &n, &key, (LPOVERLAPPED*)&p, msec))
    {
        p->operation(p, n);
        ::WSACloseEvent(p->overlapped.hEvent);
        delete p;
        return 1;
    }
    else
    {
        return 0;
    }
}

size_t io_context::poll_one()
{
    if (_Do_one(0))
        return 1;
    else
    {
        stop();
        return 0;
    }
}

void io_context::stop()
{
    ::PostQueuedCompletionStatus(port_, 0, 0, nullptr);
    ::CloseHandle(port_);
    port_ = nullptr;
    ::WSACleanup();
}

void io_context::_Start()
{
    WSAData data;
    ::WSAStartup(WINSOCK_VERSION, &data);
    port_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, concurrency_hint_);
}
} // namespace v1
} // namespace std::experimental::net
