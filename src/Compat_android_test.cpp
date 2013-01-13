// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <gtest/gtest.h>

#include "Compat_android.h"
#include <time64.h>
#include <cstdint>

TEST(CompatAndroidTest, Before1970TimeGM64) {
	struct tm tm;
	memset(&tm, 0, sizeof(tm));

	// 00:00:00
	tm.tm_sec = 0;
	tm.tm_min = 0;
	tm.tm_hour = 0;
	// 1st Jan 1969 00:00:00 UTC
	tm.tm_mday = 1;
	tm.tm_mon = 0;
	tm.tm_year = 69;

	time64_t ts = timegm64(&tm);
	EXPECT_EQ(-31536000, ts);
}

TEST(CompatAndroidTest, InvalidMonthTimeGM64) {
	struct tm tm;
	memset(&tm, 0, sizeof(tm));

	tm.tm_sec = 0;
	tm.tm_min = 0;
	tm.tm_hour = 0;
	tm.tm_mday = 1;
	tm.tm_mon = 12; // An invalid month field is treated as 0.
	tm.tm_year = 69;

	time64_t ts = timegm64(&tm);
	EXPECT_EQ(-31536000, ts);
}

TEST(CompatAndroidTest, TestCompatTimeGM) {
	struct tm tm;
	memset(&tm, 0, sizeof(tm));

	// 1st Jan 1970 00:00:00 UTC
	tm.tm_mday = 1;
	tm.tm_mon = 0;
	tm.tm_year = 70;
	tm.tm_hour = 0;
	tm.tm_min = 0;
	tm.tm_sec = 0;
	std::time_t ts = timegm(&tm);
	EXPECT_EQ(0, ts);

	tm.tm_year = 80; // 1 Jan 1980 00:00:00 UTC
	ts = timegm(&tm);
	EXPECT_EQ(315532800, ts);

	tm.tm_year = 90; // 1 Jan 1990 00:00:00 UTC
	ts = timegm(&tm);
	EXPECT_EQ(631152000, ts);

	tm.tm_year = 100; // 1 Jan 2000 00:00:00 UTC
	ts = timegm(&tm);
	EXPECT_EQ(946684800, ts);

	// 19 Jan 2038 03:14:07 UTC
	tm.tm_mday = 19;
	tm.tm_mon = 0;
	tm.tm_year = 138;
	tm.tm_hour = 3;
	tm.tm_min = 14;
	tm.tm_sec = 7;
	ts = timegm(&tm);
	EXPECT_EQ(2147483647, ts);

	// 19 Jan 2038 03:14:08 UTC
	// Error. Timestamp doesn't fit in signed 32-bit integer.
	tm.tm_sec++;
	ts = timegm(&tm);
	EXPECT_EQ(INT32_MIN, ts);
	tm.tm_sec++;
	ts = timegm(&tm);
	EXPECT_EQ(INT32_MIN, ts);

	// 13 December 1901 20:45:53 UTC
	tm.tm_mday = 13;
	tm.tm_mon = 11;
	tm.tm_year = 1;
	tm.tm_hour = 20;
	tm.tm_min = 45;
	tm.tm_sec = 53;
	ts = timegm(&tm);
	EXPECT_EQ(-2147483647, ts);

	// 13 December 1901 20:45:52 UTC
	tm.tm_sec--;
	ts = timegm(&tm);
	EXPECT_EQ(INT32_MIN, ts); // -2147483648

	// 13 December 1901 20:45:51 UTC
	// Error. Timestamp doesn't fit in signed 32-bit integer.
	tm.tm_sec--;
	ts = timegm(&tm);
	EXPECT_EQ(INT32_MIN, ts);

	// 13 December 1901 20:45:50 UTC
	// Error. Timestamp doesn't fit in signed 32-bit integer.
	tm.tm_sec--;
	ts = timegm(&tm);
	EXPECT_EQ(INT32_MIN, ts);
}