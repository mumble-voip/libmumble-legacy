// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_COMPAT_WIN_H_
#define MUMBLE_COMPAT_WIN_H_

#include <ctime>

std::time_t timegm(struct tm *tm);
char *strptime(const char *buf, const char *fmt, struct tm *tm);

#endif