// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <gtest/gtest.h>

#include <mumble/ByteArray.h>

#include <cstdlib>
#include <cstring>

TEST(ByteArrayTest, TakeOwnershipNoCap) {
	char *buf = static_cast<char *>(malloc(50));
	memset(buf, 'x', 10);
	mumble::ByteArray b(buf, 10);

	EXPECT_EQ(10, b.Length());
	EXPECT_EQ(10, b.Capacity());
}

TEST(ByteArrayTest, TakeOwnershipCap) {
	char *buf = static_cast<char *>(malloc(50));
	memset(buf, 'x', 10);
	mumble::ByteArray b(buf, 10, 50);

	EXPECT_EQ(10, b.Length());
	EXPECT_EQ(50, b.Capacity());
}

TEST(ByteArrayTest, Null) {
	mumble::ByteArray b;
	EXPECT_TRUE(b.IsNull());
	EXPECT_EQ(0, b.Capacity());
	EXPECT_EQ(0, b.Length());
}

TEST(ByteArrayTest, Assignment) {
	char *buf = static_cast<char *>(malloc(50));
	memset(buf, 'x', 10);
	mumble::ByteArray b(buf, 10, 50);

	mumble::ByteArray tmp;
	tmp = b;
}

TEST(ByteArrayTest, Slicing) {
	char *buf = static_cast<char *>(malloc(50));
	memset(buf, 'x', 10);
	mumble::ByteArray b(buf, 10, 50);

	mumble::ByteArray lastfive = b.Slice(5);
	EXPECT_EQ(5, lastfive.Length());
	EXPECT_EQ(5, lastfive.Capacity());

	bool ok = true;
	const char *tmp = lastfive.Data();
	for (int i = 0; i < 5; i++) {
		if (tmp[i] != 'x') {
			ok = false;
			break;
		}
	}
	EXPECT_TRUE(ok);
}

TEST(ByteArrayTest, SlicingContent) {
	char *buf = static_cast<char *>(malloc(50));
	memset(buf, 'x', 10);
	mumble::ByteArray b(buf, 10, 50);

	mumble::ByteArray lastfive = b.Slice(5);
	EXPECT_EQ(5, lastfive.Length());
	EXPECT_EQ(5, lastfive.Capacity());
}

TEST(ByteArrayTest, TestEmptyAppend) {
	mumble::ByteArray b; // empty

	mumble::ByteArray a(10); // 'aaaaaaaaaa'
	memset(a.Data(), 'a', 10);

	b.Append(a);

	EXPECT_EQ(a.Length(), b.Length());
	EXPECT_TRUE(memcmp(b.Data(), a.Data(), a.Length()) == 0);
}

TEST(ByteArrayTest, TestTruncate) {
	mumble::ByteArray b(10);
	b.Truncate(0);

	EXPECT_EQ(0, b.Length());
	EXPECT_EQ(10, b.Capacity());	
}

TEST(ByteArrayTest, TestNoAllocAppend) {
	mumble::ByteArray b(50);
	b.Truncate(0); // set length to 0, keep cap at 50
	char *buf = b.Data(); // note the buffer ptr

	mumble::ByteArray a(10); // 'aaaaaaaaaa'
	memset(a.Data(), 'a', 10);

	for (int i = 0; i < 5; i++) {
		b.Append(a);
	}

	// Check for pointer equality.
	// ByteArray b should not have realloc'd it's buffer.
	EXPECT_EQ(buf, b.Data());
}

TEST(ByteArrayTest, TestEqual) {
	mumble::ByteArray a(10);
	mumble::ByteArray b(10);
	mumble::ByteArray c(10);

	memset(a.Data(), 'a', a.Length());
	memset(b.Data(), 'a', b.Length());
	memset(c.Data(), 'c', c.Length());

	EXPECT_TRUE(a.Equal(b));
	EXPECT_TRUE(b.Equal(a));

	EXPECT_FALSE(a.Equal(c));
	EXPECT_FALSE(c.Equal(a));
}

TEST(ByteArrayTest, TestOperatorEqual) {
	mumble::ByteArray a(10);
	mumble::ByteArray b(10);

	memset(a.Data(), 'a', a.Length());
	memset(b.Data(), 'a', b.Length());

	EXPECT_EQ(a, b);
	EXPECT_EQ(b, a);
	EXPECT_TRUE(a == b);
	EXPECT_TRUE(b == a);
}

TEST(ByteArrayTest, TestNullEqual) {
	mumble::ByteArray null1;
	mumble::ByteArray null2;

	EXPECT_EQ(null1, null2);
}

TEST(ByteArrayTest, TestEmptyEqual) {
	mumble::ByteArray empty1(1);
	mumble::ByteArray empty2(1);

	empty1.Truncate(0);
	empty2.Truncate(0);

	EXPECT_EQ(empty1, empty2);
}

TEST(ByteArrayTest, TestEqualNullEmpty) {
	mumble::ByteArray null1;
	mumble::ByteArray empty1(10);

	empty1.Truncate(0); // make empty

	EXPECT_NE(null1, empty1);
}

TEST(ByteArrayTest, TestEqualCapIndifference) {
	mumble::ByteArray a(15);
	mumble::ByteArray b(10);

	a.Truncate(10);
	memset(a.Data(), 'a', a.Length());
	memset(b.Data(), 'a', b.Length());

	EXPECT_EQ(a, b);
}