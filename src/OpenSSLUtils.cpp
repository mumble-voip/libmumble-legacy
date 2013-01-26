// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include "OpenSSLUtils.h"
#include <mumble/Error.h>

#include <uv.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/x509.h>

#include <sstream>

static uv_once_t sslinit = UV_ONCE_INIT;

static void InitializeOpenSSL() {
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();
	SSL_load_error_strings();
}

namespace mumble {

void OpenSSLUtils::EnsureInitialized() {
	uv_once(&sslinit, InitializeOpenSSL);
}

static std::string SSLErrorToString(int SSLerr) {
	switch (SSLerr) {
		case SSL_ERROR_NONE:
			return std::string("SSL_ERROR_NONE - operation completed successfully");
		case SSL_ERROR_ZERO_RETURN:
			return std::string("SSL_ERROR_ZERO_RETURN - connection closed by peer");
		case SSL_ERROR_WANT_READ:
			return std::string("SSL_ERROR_WANT_READ - requires a read operation to continue");
		case SSL_ERROR_WANT_WRITE:
			return std::string("SSL_ERROR_WANT_WRITE - requires a write operation to continue");
		case SSL_ERROR_WANT_CONNECT:
			return std::string("SSL_ERROR_WANT_CONNECT - operation did not complete, call SSL_connect again later");
		case SSL_ERROR_WANT_ACCEPT:
			return std::string("SSL_ERROR_WANT_ACCEPT - operation did not complete, call SSL_accept again laster");
		case SSL_ERROR_WANT_X509_LOOKUP:
			return std::string("SSL_ERROR_WANT_X509_LOOKUP - operation did not complete, call the TLS I/O function again later");
		case SSL_ERROR_SYSCALL:
			return std::string("SSL_ERROR_SYSCALL - system I/O error");
		case SSL_ERROR_SSL:
			return std::string("SSL_ERROR_SSL - SSL library error, probably a protocol error");
	}
	return std::string("(none)");
}

Error OpenSSLUtils::ErrorFromOpenSSLErrorCode(int SSLerr) {
	return Error::ErrorFromDescription(
		std::string("libssl"),
		static_cast<long>(SSLerr),
		SSLErrorToString(SSLerr)
	);
}

Error OpenSSLUtils::ErrorFromLastCryptoError() {
	unsigned long cryptocode = ERR_get_error();
	const char *error = ERR_lib_error_string(cryptocode);
	const char *func = ERR_func_error_string(cryptocode);
	const char *reason = ERR_func_error_string(cryptocode);

	std::stringstream ss;
	ss << error << "; " << reason << "; " << func;

	return Error::ErrorFromDescription(
		std::string("libcrypto"),
		static_cast<long>(cryptocode),
		ss.str()
	);
}

}
