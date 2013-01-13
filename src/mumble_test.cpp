// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <gtest/gtest.h>

#include "mumble_test.h"

#include <mumble/ByteArray.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>

#include <iostream>
#include <fstream>

// The Android generator for gyp is very keen on
// all sources of a target having the same file
// extension. This means that we can't include
// gtest's test-all.cc file in our sources.
//
// Let's just include it here instead.
#include "src/gtest-all.cc"

mumble::ByteArray LoadFile(const std::string &path) {
	mumble::ByteArray ba;

	std::ifstream ifs;
	ifs.open(path, std::ios::binary);

	while (ifs.good()) {
		mumble::ByteArray chunk(256);
		char *buf = chunk.Data();
		ifs.read(buf, chunk.Length());
		chunk.Truncate(ifs.gcount());
		if (chunk.Length() > 0) {
			ba.Append(chunk);
		}
	}

	ifs.close();

	return ba;
}

bool SaveFile(const std::string &path, const mumble::ByteArray &ba) {
	bool ok;
	std::ofstream ofs;
	ofs.open(path, std::ios::binary|std::ios::out|std::ios::trunc);
	if (ofs.is_open()) {
		ofs.write(ba.ConstData(), ba.Length());
		ofs.flush();
		ok = ofs.good();
		ofs.close();
	}
	return ok;
}

int main(int argc, char **argv) {
	// Ensure OpenSSL is properly initialized
	// during testing.
	//
	// fixme(mkrautz): This should happen
	// automatically inside the library itself,
	// but this will do for now.
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	ERR_load_crypto_strings();
	SSL_load_error_strings();

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
