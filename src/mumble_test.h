// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_TEST_H_
#define MUMBLE_TEST_H_

#include <mumble/ByteArray.h>

mumble::ByteArray LoadFile(const std::string &path);
bool SaveFile(const std::string &path, const mumble::ByteArray &ba);

#endif