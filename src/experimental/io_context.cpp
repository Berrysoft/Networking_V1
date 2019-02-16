#include <experimental/io_context>

#include <WinSock2.h>

namespace std::experimental::net
{
inline namespace v1
{
size_t io_context::_Do_one(DWORD msec)
{
    DWORD n;
    ULONG_PTR key{ 0 };
    _Io_operation* p;
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
}

void io_context::_Start()
{
    port_ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, concurrency_hint_);
}
} // namespace v1
} // namespace std::experimental::net
