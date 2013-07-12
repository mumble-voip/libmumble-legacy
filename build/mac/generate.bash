#!/bin/bash
# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

# This script re-generates the Xcode project files for OS X.

GYP=./gyp
GYPFLAGS="-I common.gypi"
${GYP} libmumble.gyp ${GYPFLAGS} -f xcode --depth . -Dlibrary=static_library -Dopenssl_asm=gnuas-x86_64 -Dtarget_arch=x64 --generator-out=build/mac/xcode