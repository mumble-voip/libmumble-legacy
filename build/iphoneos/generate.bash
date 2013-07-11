#!/bin/bash
# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

# This script re-generates the Xcode project files for iOS.

# Check for bad files in testdata.
./testdata/vet.bash
if [ $? -ne 0 ]; then
	echo "There seems to be non-unique files in the testdata directory."
	echo "This is not supported by the iOS test runner, so generate.bash"
	echo "will not be able to continue."
	echo
	echo "Unable to generate Xcode project files."
	exit 1
fi

GYP=./gyp
GYPFLAGS="-I common.gypi"
${GYP} libmumble.gyp ${GYPFLAGS} -f xcode --depth . -Dlibrary=static_library -Dopenssl_asm=gnuas-arm -DOS=ios --generator-out=build/iphoneos/xcode
