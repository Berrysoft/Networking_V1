#include <experimental/io_context>

namespace std::experimental::net
{
inline namespace v1
{
io_context::io_context(int concurrency_hint)
    : outstanding_work_(0), stopped_(0), stop_event_posted_(0), shutdown_(0), dispatch_required_(0), concurrency_hint_(concurrency_hint)
{
    HANDLE h{ ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, (DWORD)(concurrency_hint >= 0 ? concurrency_hint : ~0)) };
    if (!h)
    {
        _Check_error_code(error_code{ ::GetLastError(), generic_category() }, __func__);
    }
    iocp_ = unique_handle{ h };
}

size_t io_context::_Do_one(DWORD msec)
{
    while (true)
    {
        if (_InterlockedCompareExchange(&dispatch_required_, 0, 1) == 1)
        {
            lock_guard lock{ dispatch_mutex_ };
            queue<_Io_operation> ops = completed_ops_;
            timer_queues_.get_ready_timers(ops);
            _Post_deferred_completions(ops);
            _Update_timeout();
        }
        DWORD bytes_transferred{ 0 };
        DWORD_PTR completion_key{ 0 };
        LPOVERLAPPED overlapped{ nullptr };
        ::SetLastError(0);
        BOOL ok{ ::GetQueuedCompletionStatus(iocp_.get(), &bytes_transferred, &completion_key, &overlapped, msec < gqcs_timeout_ ? msec : gqcs_timeout_) };
        DWORD last_error{ ::GetLastError() };
        if (overlapped)
        {
            _Io_operation* op = static_cast<_Io_operation*>(overlapped);
            error_code result_ec{ last_error, generic_category() };
            if (completion_key == overlapped_contains_result)
            {
                result_ec = error_code{ static_cast<int>(op->Offset), *reinterpret_cast<std::error_category*>(op->Internal) };
                bytes_transferred = op->OffsetHigh;
            }
            else
            {
                op->Internal = reinterpret_cast<ULONG_PTR>(&result_ec.category());
                op->Offset = result_ec.value();
                op->OffsetHigh = bytes_transferred;
            }
            if (_InterlockedCompareExchange(&op->ready_, 1, 0) == 1)
            {
                op->complete(this, result_ec, bytes_transferred);
                _Work_finished();
                return 1;
            }
        }
        else if (!ok)
        {
            if (last_error != WAIT_TIMEOUT)
            {
                _Check_error_code(error_code{ last_error, generic_category() }, __func__);
                return 0;
            }
            if (msec == INFINITE)
                continue;
            return 0;
        }
        else if (completion_key == wake_for_dispatch)
        {
        }
        else
        {
            _InterlockedExchange(&stop_event_posted_, 0);
            if (_InterlockedExchangeAdd(&stopped_, 0) != 0)
            {
                if (_InterlockedExchange(&stop_event_posted_, 1) == 0)
                {
                    if (!::PostQueuedCompletionStatus(iocp_.get(), 0, 0, nullptr))
                    {
                        _Check_error_code(error_code{ ::GetLastError(), generic_category() }, __func__);
                        return 0;
                    }
                }
                return 0;
            }
        }
    }
}

void io_context::_Work_started() { _InterlockedIncrement(&outstanding_work_); }

void io_context::_Work_finished()
{
    if (_InterlockedDecrement(&outstanding_work_) == 0)
        stop();
}

size_t io_context::run_one()
{
    if (_InterlockedExchangeAdd(&outstanding_work_, 0) == 0)
    {
        stop();
        return 0;
    }
    return _Do_one(INFINITE);
}

size_t io_context::poll_one()
{
    if (_InterlockedExchangeAdd(&outstanding_work_, 0) == 0)
    {
        stop();
        return 0;
    }
    return _Do_one(0);
}

void io_context::stop()
{
    if (_InterlockedExchange(&stopped_, 1) == 0)
    {
        if (_InterlockedExchange(&stop_event_posted_, 1) == 0)
        {
            if (!::PostQueuedCompletionStatus(iocp_.get(), 0, 0, nullptr))
            {
                _Check_error_code(error_code{ ::GetLastError(), generic_category() }, __func__);
            }
        }
    }
}

bool io_context::stopped() const noexcept { return _InterlockedExchangeAdd(&stopped_, 0) != 0; }

void io_context::restart() { _InterlockedExchange(&stopped_, 0); }
} // namespace v1
} // namespace std::experimental::net
