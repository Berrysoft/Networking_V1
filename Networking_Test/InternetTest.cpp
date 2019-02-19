#include "pch.h"

#include "InternetTest.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace std::experimental::net;
using namespace std::experimental::net::ip;

namespace NetworkingTest
{
	TEST_CLASS(InternetTest)
	{
	public:
		TEST_METHOD(ComparisonsTestV4)
		{
			address_v4 addrs[]{ address_v4::any(), address_v4::loopback(), address_v4::broadcast(), address_v4{0x11001100}, address_v4{0xEFEFEFEF} };
			auto b{ begin(addrs) };
			auto e{ end(addrs) };
			for (auto it{ b }; it != e; ++it)
			{
				auto& a{ *it };
				Assert::AreEqual(a, a);
				Assert::IsTrue(a == a);
				Assert::IsTrue(a <= a);
				Assert::IsTrue(a >= a);
				Assert::IsFalse(a != a);
				Assert::IsFalse(a < a);
				Assert::IsFalse(a > a);
			}

			sort(b, e);

			for (auto it{ b + 1 }; it != e; ++it)
			{
				auto& x{ *it };
				auto& y{ *b };
				Assert::IsTrue(x != y);
				Assert::AreNotEqual(x, y);
				Assert::IsTrue(y < x);
				Assert::IsTrue(y <= x);
				Assert::IsTrue(x > y);
				Assert::IsTrue(x >= y);
			}
		}

		TEST_METHOD(ConsTestV4)
		{
			address_v4 a;
			Assert::AreEqual(uint_least32_t(0), a.to_uint());
			Assert::IsTrue(a.to_bytes() == address_v4::bytes_type{});

			a = address_v4{ address_v4::bytes_type{} };
			Assert::AreEqual(uint_least32_t(0), a.to_uint());
			Assert::IsTrue(a.to_bytes() == address_v4::bytes_type{});

			address_v4::bytes_type b{ 1, 2, 3, 4 };
			a = address_v4{ b };
			Assert::AreEqual(uint_least32_t(ntohl((4 << 24) | (3 << 16) | (2 << 8) | 1)), a.to_uint());
			Assert::IsTrue(a.to_bytes() == b);

			a = address_v4{ 0 };
			Assert::AreEqual(uint_least32_t(0), a.to_uint());
			Assert::IsTrue(a.to_bytes() == address_v4::bytes_type{});

			uint_least32_t u{ ntohl((8 << 24) | (7 << 16) | (6 << 8) | 5) };
			a = address_v4{ u };
			Assert::AreEqual(u, a.to_uint());
			Assert::IsTrue(a.to_bytes() == address_v4::bytes_type{ 5, 6, 7, 8 });
		}

		TEST_METHOD(CreationTestV4)
		{
			address_v4 a{ make_address_v4("127.0.0.1") };
			Assert::IsTrue(a.is_loopback());
			a = make_address_v4("127.0.0.2"s);
			Assert::IsTrue(a.is_loopback());
			a = make_address_v4("127.0.0.3"sv);
			Assert::IsTrue(a.is_loopback());

			error_code ec{};
			a = make_address_v4("127...1", ec);
			Assert::IsTrue(ec == errc::invalid_argument);

			make_address_v4("127.0.0.1", ec);
			Assert::IsTrue(!ec);

			a = make_address_v4("256.0.0.1"s, ec);
			Assert::IsTrue(ec == errc::invalid_argument);

			make_address_v4("127.0.0.1"s, ec);
			Assert::IsTrue(!ec);

			a = make_address_v4(""sv, ec);
			Assert::IsTrue(ec == errc::invalid_argument);
		}

		TEST_METHOD(MemberTestV4)
		{
			address_v4 a;
			Assert::IsTrue(a.is_unspecified());

			a = address_v4::any();
			Assert::IsTrue(a.is_unspecified());

			a = address_v4::loopback();
			Assert::IsFalse(a.is_unspecified());

			a = address_v4::broadcast();
			Assert::IsFalse(a.is_unspecified());

			a = address_v4::loopback();
			Assert::IsTrue(a.is_loopback());

			a = address_v4{ 0x7F000001 };
			Assert::IsTrue(a.is_loopback());

			a = address_v4{ 0x7F010203 };
			Assert::IsTrue(a.is_loopback());

			a = address_v4{ 0x7FFFFFFF };
			Assert::IsTrue(a.is_loopback());

			a = address_v4::any();
			Assert::IsFalse(a.is_loopback());

			a = address_v4::broadcast();
			Assert::IsFalse(a.is_loopback());

			a = address_v4{ 0xE0000001 };
			Assert::IsTrue(a.is_multicast());

			a = address_v4{ 0xE0010203 };
			Assert::IsTrue(a.is_multicast());

			a = address_v4{ 0xE0FFFFFF };
			Assert::IsTrue(a.is_multicast());

			a = address_v4{ 0xF0000000 };
			Assert::IsFalse(a.is_multicast());

			a = address_v4{ 0xDFFFFFFF };
			Assert::IsFalse(a.is_multicast());

			Assert::AreEqual("0.0.0.0", address_v4::any().to_string().c_str());
			Assert::AreEqual("127.0.0.1", address_v4::loopback().to_string().c_str());
			Assert::AreEqual("255.255.255.255", address_v4::broadcast().to_string().c_str());

			ostringstream oss;
			oss << address_v4::any() << ' ' << address_v4::loopback() << ' ' << address_v4::broadcast();
			Assert::AreEqual("0.0.0.0 127.0.0.1 255.255.255.255", oss.str().c_str());
		}

		TEST_METHOD(ResolveTestTCP)
		{
			error_code ec{};
			io_context ctx;
			tcp::resolver res{ ctx };
			auto addrs{ res.resolve("localhost", "http", ec) };
			Assert::IsTrue(!ec);
			Assert::IsTrue(addrs.size() > 0);
			Assert::IsTrue(addrs.begin() != addrs.end());
			Assert::IsFalse(addrs.empty());

			auto addrs2{ res.resolve("localhost","http") };
			Assert::IsTrue(addrs == addrs2);

			for (auto& addr : addrs)
			{
				Logger::WriteMessage(addr.endpoint().address().to_string().c_str());
			}
		}
	};
}
