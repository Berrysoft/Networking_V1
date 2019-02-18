#include "pch.h"

#include <experimental/buffer>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace std::experimental::net;

namespace NetworkingTest
{
	TEST_CLASS(BufferTest)
	{
	public:
		TEST_METHOD(TestMutable1)
		{
			const mutable_buffer b;

			Assert::IsTrue(is_mutable_buffer_sequence_v<mutable_buffer>);
			Assert::IsTrue(is_nothrow_default_constructible_v<mutable_buffer>);
			Assert::IsTrue(is_copy_assignable_v<mutable_buffer>);
			Assert::IsTrue(is_nothrow_constructible_v<mutable_buffer, void*, size_t>);
			Assert::IsTrue(is_same_v<decltype(b.data()), void*>);
			Assert::IsTrue(noexcept(b.data()));
			Assert::IsTrue(is_same_v<decltype(b.size()), size_t>);
			Assert::IsTrue(noexcept(b.size()));
		}

		TEST_METHOD(TestMutable2)
		{
			char c[4];

			mutable_buffer b;
			Assert::AreEqual(b.data(), (void*)nullptr);
			Assert::AreEqual(b.size(), size_t(0));

			b = mutable_buffer{ c, sizeof(c) };
			Assert::AreEqual(b.data(), (void*)c);
			Assert::AreEqual(b.size(), sizeof(c));

			b = mutable_buffer{};
			Assert::AreEqual(b.data(), (void*)nullptr);
			Assert::AreEqual(b.size(), size_t(0));
		}

		TEST_METHOD(SizeTestMutable)
		{
			char c[4];

			mutable_buffer mb;
			Assert::AreEqual(buffer_size(mb), size_t(0));

			mb = mutable_buffer{ c, sizeof(c) };
			Assert::AreEqual(buffer_size(mb), mb.size());
		}

		TEST_METHOD(SizeTestConst)
		{
			char c[4];

			const_buffer mb;
			Assert::AreEqual(buffer_size(mb), size_t(0));

			mb = const_buffer{ c, sizeof(c) };
			Assert::AreEqual(buffer_size(mb), mb.size());
		}

		TEST_METHOD(SequenceSizeTestMutable)
		{
			char c[32];
			vector<mutable_buffer> vec{ {c, 0}, {c, 32}, {c, 16}, {c, 3}, {c, 0} };
			Assert::AreEqual(buffer_size(vec), size_t(0 + 32 + 16 + 3 + 0));
		}

		TEST_METHOD(SequenceSizeTestConst)
		{
			char c[32];
			vector<const_buffer> vec{ {c, 0}, {c, 32}, {c, 16}, {c, 3}, {c, 0} };
			Assert::AreEqual(buffer_size(vec), size_t(0 + 32 + 16 + 3 + 0));
		}
	};
}
