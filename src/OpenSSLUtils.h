// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_OPENSSL_UTILS_H_
#define MUMBLE_OPENSSL_UTILS_H_

#include <mumble/Error.h>

namespace mumble {

class OpenSSLUtils {
public:
	static void EnsureInitialized();

	/// ErrorFromOpenSSLErrorCode translates a libssl
	/// error code into a mumble::Error.
	static Error ErrorFromOpenSSLErrorCode(int SSLerr);

	/// ErrorFromLastCryptoError calls libcrypto's
	/// ERR_get_error to translate the last error
	/// that happened in the calling thread to a
	/// mumble::Error.
	static Error ErrorFromLastCryptoError();
};

}

#endif
