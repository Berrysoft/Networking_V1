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
		TEST_METHOD(TraitsTest)
		{
			Assert::IsTrue(is_mutable_buffer_sequence_v<mutable_buffer>);
			Assert::IsTrue(is_mutable_buffer_sequence_v<const mutable_buffer>);
			Assert::IsTrue(is_mutable_buffer_sequence_v<vector<mutable_buffer>>);
			Assert::IsTrue(is_mutable_buffer_sequence_v<const vector<mutable_buffer>>);
			Assert::IsFalse(is_mutable_buffer_sequence_v<const_buffer>);
			Assert::IsFalse(is_mutable_buffer_sequence_v<vector<const_buffer>>);

			Assert::IsTrue(is_const_buffer_sequence_v<const_buffer>);
			Assert::IsTrue(is_const_buffer_sequence_v<const const_buffer>);
			Assert::IsTrue(is_const_buffer_sequence_v<vector<const_buffer>>);
			Assert::IsTrue(is_const_buffer_sequence_v<const vector<const_buffer>>);
			Assert::IsTrue(is_const_buffer_sequence_v<mutable_buffer>);
			Assert::IsTrue(is_const_buffer_sequence_v<const mutable_buffer>);
			Assert::IsTrue(is_const_buffer_sequence_v<vector<mutable_buffer>>);
			Assert::IsTrue(is_const_buffer_sequence_v<const vector<mutable_buffer>>);

			Assert::IsTrue(is_dynamic_buffer_v<dynamic_vector_buffer<int, allocator<int>>>);
			Assert::IsTrue(is_dynamic_buffer_v<dynamic_string_buffer<char, char_traits<char>, allocator<char>>>);
		}

		TEST_METHOD(TestMutable1)
		{
			const mutable_buffer b;

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
			Assert::AreEqual((void*)nullptr, b.data());
			Assert::AreEqual(size_t(0), b.size());

			b = mutable_buffer{ c, sizeof(c) };
			Assert::AreEqual((void*)c, b.data());
			Assert::AreEqual(sizeof(c), b.size());

			b = mutable_buffer{};
			Assert::AreEqual((void*)nullptr, b.data());
			Assert::AreEqual(size_t(0), b.size());
		}

		TEST_METHOD(TestConst1)
		{
			const const_buffer b;

			Assert::IsTrue(is_nothrow_default_constructible_v<const_buffer>);
			Assert::IsTrue(is_copy_assignable_v<const_buffer>);
			Assert::IsTrue(is_nothrow_constructible_v<const_buffer, const void*, size_t>);
			Assert::IsTrue(is_same_v<decltype(b.data()), const void*>);
			Assert::IsTrue(noexcept(b.data()));
			Assert::IsTrue(is_same_v<decltype(b.size()), size_t>);
			Assert::IsTrue(noexcept(b.size()));
		}

		TEST_METHOD(TestConst2)
		{
			char c[4];

			const_buffer b;
			Assert::AreEqual((const void*)nullptr, b.data());
			Assert::AreEqual(size_t(0), b.size());

			b = const_buffer{ c, sizeof(c) };
			Assert::AreEqual((const void*)c, b.data());
			Assert::AreEqual(sizeof(c), b.size());

			b = const_buffer{};
			Assert::AreEqual((const void*)nullptr, b.data());
			Assert::AreEqual(size_t(0), b.size());
		}

		TEST_METHOD(SizeTestMutable)
		{
			char c[4];

			mutable_buffer mb;
			Assert::AreEqual(size_t(0), buffer_size(mb));

			mb = mutable_buffer{ c, sizeof(c) };
			Assert::AreEqual(mb.size(), buffer_size(mb));
		}

		TEST_METHOD(SizeTestConst)
		{
			char c[4];

			const_buffer mb;
			Assert::AreEqual(size_t(0), buffer_size(mb));

			mb = const_buffer{ c, sizeof(c) };
			Assert::AreEqual(mb.size(), buffer_size(mb));
		}

		TEST_METHOD(SequenceSizeTestMutable)
		{
			char c[32];
			vector<mutable_buffer> vec{ {c, 0}, {c, 32}, {c, 16}, {c, 3}, {c, 0} };
			Assert::AreEqual(size_t(0 + 32 + 16 + 3 + 0), buffer_size(vec));
		}

		TEST_METHOD(SequenceSizeTestConst)
		{
			char c[32];
			vector<const_buffer> vec{ {c, 0}, {c, 32}, {c, 16}, {c, 3}, {c, 0} };
			Assert::AreEqual(size_t(0 + 32 + 16 + 3 + 0), buffer_size(vec));
		}

		TEST_METHOD(ArithemicTestMutable)
		{
			char c[4];

			mutable_buffer mb;
			mb = mb + 0;
			Assert::AreEqual((void*)nullptr, mb.data());
			Assert::AreEqual(size_t(0), mb.size());

			mb = 0 + mb;
			Assert::AreEqual((void*)nullptr, mb.data());
			Assert::AreEqual(size_t(0), mb.size());

			mb = mutable_buffer{ c, sizeof(c) };
			mb += 1;
			Assert::AreEqual((void*)(c + 1), mb.data());
			Assert::AreEqual(size_t(3), mb.size());

			mb = mb + 2;
			Assert::AreEqual((void*)(c + 3), mb.data());
			Assert::AreEqual(size_t(1), mb.size());

			mb = mb + 2;
			Assert::AreEqual((void*)(c + 4), mb.data());
			Assert::AreEqual(size_t(0), mb.size());

			mb = mutable_buffer(c, sizeof(c));
			mb = 3 + mb;
			Assert::AreEqual((void*)(c + 3), mb.data());
			Assert::AreEqual(size_t(1), mb.size());

			mb = 2 + mb;
			Assert::AreEqual((void*)(c + 4), mb.data());
			Assert::AreEqual(size_t(0), mb.size());
		}

		TEST_METHOD(ArithemicTestConst)
		{
			char c[4];

			const_buffer mb;
			mb = mb + 0;
			Assert::AreEqual((const void*)nullptr, mb.data());
			Assert::AreEqual(size_t(0), mb.size());

			mb = 0 + mb;
			Assert::AreEqual((const void*)nullptr, mb.data());
			Assert::AreEqual(size_t(0), mb.size());

			mb = const_buffer{ c, sizeof(c) };
			mb += 1;
			Assert::AreEqual((const void*)(c + 1), mb.data());
			Assert::AreEqual(size_t(3), mb.size());

			mb = mb + 2;
			Assert::AreEqual((const void*)(c + 3), mb.data());
			Assert::AreEqual(size_t(1), mb.size());

			mb = mb + 2;
			Assert::AreEqual((const void*)(c + 4), mb.data());
			Assert::AreEqual(size_t(0), mb.size());

			mb = const_buffer(c, sizeof(c));
			mb = 3 + mb;
			Assert::AreEqual((const void*)(c + 3), mb.data());
			Assert::AreEqual(size_t(1), mb.size());

			mb = 2 + mb;
			Assert::AreEqual((const void*)(c + 4), mb.data());
			Assert::AreEqual(size_t(0), mb.size());
		}
	};
}
