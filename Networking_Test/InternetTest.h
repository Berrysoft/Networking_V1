#pragma once

#include <experimental/internet>

namespace Microsoft::VisualStudio::CppUnitTestFramework
{
	template<> inline std::wstring ToString<std::experimental::net::ip::address_v4>(const std::experimental::net::ip::address_v4& t) { return ToString(t.to_string()); }
}
