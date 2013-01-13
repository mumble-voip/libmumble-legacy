// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include "Compat_android.h"

#include <ctime>
#include <cstdint>
#include <time64.h>

std::time_t timegm(struct tm *tm) {
	// Android's time_t is a 32-bit signed integer, but Android also provides a 64-bit time type
	// called time64_t. The timegm function in Android is only defined for the time64_t type,
	// and is called timegm64. Since our API is defined as returning a time_t, we'll stick to
	// that even though it means we'll only span Unix timestamps from INT32_MIN through INT32_MAX.
	static_assert(sizeof(std::time_t) == 4, "expecting 32-bit time_t on Android");
	time64_t ts = timegm64(tm);
	if (ts < INT32_MIN || ts > INT32_MAX) {
		return INT32_MIN;
	}
	return static_cast<std::time_t>(ts);
}