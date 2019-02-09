#include <experimental/internet>
#include <net/if.h>

namespace std::experimental::net
{
inline namespace v1
{
namespace ip
{
address_v4 make_address_v4(const char* str, error_code& ec) noexcept
{
    address_v4::bytes_type bytes;
    int r{ ::inet_pton(AF_INET, str, bytes.begin()) };
    if (r != 1)
        ec = make_error_code(errc::invalid_argument);
    return address_v4{ bytes };
}

address_v6 make_address_v6(const char* str, error_code& ec) noexcept
{
    const char* if_name = strchr(str, '%');
    char src_buf[257];
    const char* src_ptr = str;
    if (if_name - str > 256)
    {
        ec = make_error_code(errc::invalid_argument);
        return {};
    }
    memcpy(src_buf, str, if_name - str);
    src_buf[if_name - str] = 0;
    src_ptr = src_buf;
    address_v6::bytes_type bytes;
    int r{ ::inet_pton(AF_INET6, str, bytes.begin()) };
    if (r != 1)
        ec = make_error_code(errc::invalid_argument);
    int scope_id;
    bool is_link_local = ((bytes[0] == 0xfe) && ((bytes[1] & 0xc0) == 0x80));
    bool is_multicast_link_local = ((bytes[0] == 0xff) && ((bytes[1] & 0x0f) == 0x02));
    if (is_link_local || is_multicast_link_local)
        scope_id = ::if_nametoindex(if_name + 1);
    if (scope_id == 0)
        scope_id = atoi(if_name + 1);
    return address_v6{ bytes, scope_id };
}

address make_address(const char* str, error_code& ec) noexcept
{
    address a{};
    address_v6 v6a{ make_address_v6(str, ec) };
    if (!ec)
        a = v6a;
    else
    {
        address_v4 v4a{ make_address_v4(str, ec) };
        if (!ec)
            a = v4a;
    }
    return a;
}
address make_address(const string& str, error_code& ec) noexcept
{
    address a{};
    address_v6 v6a{ make_address_v6(str, ec) };
    if (!ec)
        a = v6a;
    else
    {
        address_v4 v4a{ make_address_v4(str, ec) };
        if (!ec)
            a = v4a;
    }
    return a;
}
address make_address(string_view str, error_code& ec) noexcept
{
    address a{};
    address_v6 v6a{ make_address_v6(str, ec) };
    if (!ec)
        a = v6a;
    else
    {
        address_v4 v4a{ make_address_v4(str, ec) };
        if (!ec)
            a = v4a;
    }
    return a;
}
} // namespace ip
} // namespace v1
} // namespace std::experimental::net