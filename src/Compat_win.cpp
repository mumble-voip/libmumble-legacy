// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include "Compat_win.h"

#include <ctime>
#include <string>
#include <cstdint>
#include <assert.h>

#include <iostream>

std::time_t timegm(struct tm *tm) {
	return _mkgmtime(tm);
}

char *strptime(const char *buf, const char *fmt, struct tm *tm) {
	const std::string x509fmt("%Y%m%d%H%M%S");

	if (tm == nullptr) {
		return nullptr;
	}

	std::string infmt(fmt);
	if (infmt != x509fmt) {
		return nullptr;
	}

	if (strlen(buf) < 14) {
		return nullptr;
	}

	int year = (buf[0]-'0') * 1000 + (buf[1]-'0') * 100 + (buf[2]-'0') * 10 + (buf[3]-'0');
	tm->tm_year = year - 1900;

	int month = (buf[4]-'0') * 10 + (buf[5]-'0');
	tm->tm_mon = month - 1;

	int day = (buf[6]-'0') * 10 + (buf[7]-'0');
	tm->tm_mday = day;

	int hour = (buf[8]-'0') * 10 + (buf[9]-'0');
	tm->tm_hour = hour;

	int minute = (buf[10]-'0') * 10 + (buf[11]-'0');
	tm->tm_min = minute;

	int seconds = (buf[12]-'0') * 10 + (buf[13]-'0');
	tm->tm_sec = seconds;

	tm->tm_wday = 0;
	tm->tm_yday = 0;
	tm->tm_isdst = 0;

	return const_cast<char *>(&buf[14]);
}