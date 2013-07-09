// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <stdlib.h>

static char **fakeenviron = NULL;

// _NSGetEnviron will trigger a false positive if submitted
// to Apple for review, so watch out that this stub only gets
// included in the test runner.
char ***_NSGetEnviron() {
	return &fakeenviron;
}