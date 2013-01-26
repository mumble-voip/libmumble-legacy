// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_UV_UTILS_H_
#define MUMBLE_UV_UTILS_H_

#include <mumble/Error.h>

#include <uv.h>

namespace mumble {

class UVUtils {
public:
	static Error ErrorFromLastUVError(uv_loop_t *loop);
	static Error ErrorFromUVError(uv_err_t err);
};

}

#endif