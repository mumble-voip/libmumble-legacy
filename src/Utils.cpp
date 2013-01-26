// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include "Utils.h"

#include <stdlib.h>

void NotReached() {
#ifdef __EXCEPTIONS
	throw "not reached";
#else
	abort();
#endif
}
