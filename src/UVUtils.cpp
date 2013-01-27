// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include "UVUtils.h"
#include <mumble/Error.h>

#include <uv.h>

namespace mumble {

Error UVUtils::ErrorFromLastUVError(uv_loop_t *loop) {
	uv_err_t last = uv_last_error(loop);
	return UVUtils::ErrorFromUVError(last);
}

Error UVUtils::ErrorFromUVError(uv_err_t err) {
	return Error::ErrorFromDescription(
		std::string("uv"),
		static_cast<long>(err.code),
		std::string(uv_strerror(err))
	);
}

}
