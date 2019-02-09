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
    int scope_id{ 0 };
    bool is_link_local = (bytes[0] == 0xfe) && ((bytes[1] & 0xc0) == 0x80);
    bool is_multicast_link_local = (bytes[0] == 0xff) && ((bytes[1] & 0x0f) == 0x02);
    if (is_link_local || is_multicast_link_local)
        scope_id = ::if_nametoindex(if_name + 1);
    if (!scope_id)
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

basic_address_iterator<address_v4>& basic_address_iterator<address_v4>::operator++() noexcept
{
    address_ = address_v4{ (address_.to_uint() + 1) & 0xFFFFFFFF };
    return *this;
}
basic_address_iterator<address_v4>& basic_address_iterator<address_v4>::operator--() noexcept
{
    address_ = address_v4{ (address_.to_uint() - 1) & 0xFFFFFFFF };
    return *this;
}

basic_address_iterator<address_v6>& basic_address_iterator<address_v6>::operator++() noexcept
{
    for (int i = 15; i >= 0; --i)
    {
        if (address_.addr_.s6_addr[i] < 0xFF)
        {
            ++address_.addr_.s6_addr[i];
            break;
        }
        address_.addr_.s6_addr[i] = 0;
    }
    return *this;
}
basic_address_iterator<address_v6>& basic_address_iterator<address_v6>::operator--() noexcept
{
    for (int i = 15; i >= 0; --i)
    {
        if (address_.addr_.s6_addr[i] > 0)
        {
            --address_.addr_.s6_addr[i];
            break;
        }
        address_.addr_.s6_addr[i] = 0xFF;
    }
    return *this;
}

network_v4 make_network_v4(const string& str, error_code& ec) noexcept
{
    auto pos{ str.find_first_of('/') };
    if (pos == string::npos)
    {
        ec = make_error_code(errc::invalid_argument);
        return network_v4{};
    }
    if (pos == str.size() - 1)
    {
        ec = make_error_code(errc::invalid_argument);
        return network_v4{};
    }
    auto end{ str.find_first_not_of("0123456789", pos + 1) };
    if (end != string::npos)
    {
        ec = make_error_code(errc::invalid_argument);
        return network_v4{};
    }
    const address_v4 addr{ make_address_v4(str.substr(0, pos), ec) };
    if (ec)
        return network_v4{};
    long prefix_len{ strtol(str.substr(pos + 1).c_str(), nullptr, 10) };
    if (prefix_len < 0 || prefix_len > 32)
    {
        ec = make_error_code(errc::invalid_argument);
        return network_v4{};
    }
    return network_v4{ addr, (int)prefix_len };
}

network_v6 make_network_v6(const string& str, error_code& ec) noexcept
{
    auto pos{ str.find_first_of('/') };
    if (pos == string::npos)
    {
        ec = make_error_code(errc::invalid_argument);
        return network_v6{};
    }
    if (pos == str.size() - 1)
    {
        ec = make_error_code(errc::invalid_argument);
        return network_v6{};
    }
    auto end{ str.find_first_not_of("0123456789", pos + 1) };
    if (end != string::npos)
    {
        ec = make_error_code(errc::invalid_argument);
        return network_v6{};
    }
    const address_v6 addr{ make_address_v6(str.substr(0, pos), ec) };
    if (ec)
        return network_v6{};
    long prefix_len{ strtol(str.substr(pos + 1).c_str(), nullptr, 10) };
    if (prefix_len < 0 || prefix_len > 128)
    {
        ec = make_error_code(errc::invalid_argument);
        return network_v6{};
    }
    return network_v6{ addr, (int)prefix_len };
}
} // namespace ip
} // namespace v1
} // namespace std::experimental::net