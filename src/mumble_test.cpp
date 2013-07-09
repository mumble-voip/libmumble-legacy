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

#if LIBMUMBLE_OS_IOS == 1
#import <Foundation/Foundation.h>
#endif

mumble::ByteArray LoadFile(const std::string &path) {
#if LIBMUMBLE_OS_IOS == 1
	std::string basename = path.substr(path.rfind("/")+1);
	NSString *absFn = [[NSBundle mainBundle] pathForResource:@(basename.c_str()) ofType:nil];
	NSData *data = [NSData dataWithContentsOfFile:absFn];
	return mumble::ByteArray(const_cast<char *>(reinterpret_cast<const char *>([data bytes])),
							 static_cast<int>([data length]));
#else
	mumble::ByteArray ba;

	std::ifstream ifs;
	ifs.open(path, std::ifstream::binary);

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
#endif
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
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
