// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <gtest/gtest.h>

#include "Compat_win.h"
#include <cstdint>

TEST(CompatWindowsTest, InvalidStrptimeFormat) {
	struct tm tm;
	memset(&tm, 0, sizeof(tm));

	const char *buf = "20121224";
	EXPECT_EQ(nullptr, strptime(buf, "%Y%m%d", &tm));
}

TEST(CompatWindowsTest, ValidStrptimeFormat) {
	struct tm tm;
	memset(&tm, 0, sizeof(tm));

	const char *buf = "20130117180000";
	char *ret = strptime(buf, "%Y%m%d%H%M%S", &tm);
	ASSERT_FALSE(ret == nullptr);

	EXPECT_EQ(2013-1900, tm.tm_year);
	EXPECT_EQ(0, tm.tm_mon);
	EXPECT_EQ(17, tm.tm_mday);
	EXPECT_EQ(18, tm.tm_hour);
	EXPECT_EQ(0, tm.tm_min);
	EXPECT_EQ(0, tm.tm_sec);

	EXPECT_EQ(0, tm.tm_isdst);
	EXPECT_EQ(0, tm.tm_wday);
	EXPECT_EQ(0, tm.tm_yday);
}

TEST(CompatWindowsTest, TimeGmConversion) {
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

	// 13 December 1901 20:45:53 UTC
	// Error: Windows's _mkgmtime can't convert
	// to dates earlier than the Unix epoch.
	tm.tm_mday = 13;
	tm.tm_mon = 11;
	tm.tm_year = 1;
	tm.tm_hour = 20;
	tm.tm_min = 45;
	tm.tm_sec = 53;
	ts = timegm(&tm);
	EXPECT_EQ(-1, ts);

	// 13 December 1901 20:45:52 UTC
	// Error: Windows's _mkgmtime can't convert
	// to dates earlier than the Unix epoch.
	tm.tm_sec--;
	ts = timegm(&tm);
	EXPECT_EQ(-1, ts);
}