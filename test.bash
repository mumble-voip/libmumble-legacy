#!/bin/bash
# Copyright (c) 2012-2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

# This script builds libmumble and runs its test suite.
# It's primarily meant to be used during development,
# such that a developer can run ./test.bash from a
# terminal while hacking on a piece of code, or tests.
#
# The script will use Xcode to build on Mac OS X, and
# on other Unix-like systems it will attempt to use
# ninja or make (if ninja is not available on the
# system).

function xcode_build {
	gyp libmumble.gyp -f xcode --depth . -Dlibrary=static_library -Dopenssl_asm= --generator-out=test
	xcodebuild -project test/libmumble.xcodeproj/ -target libmumble-test -configuration Default CONFIGURATION_BUILD_DIR=test/build
	./test/build/libmumble-test
}

function ninja_build {
	gyp libmumble.gyp -f ninja --depth . -Dlibrary=static_library -Dopenssl_asm= --generator-out=test
	ninja -C test/out/Default
	./test/out/Default/libmumble-test
}

function make_build {
	gyp libmumble.gyp -f make --depth . -Dlibrary=static_library -Dopenssl_asm= --generator-out=test
	make -C test/
	./test/out/Default/libmumble-test
}

trap exit SIGINT SIGTERM

system=$(uname -s)
case "$system" in
	"Darwin")
		xcode_build
		exit
		;;
	*)
		type -P ninja 2>&1 >/dev/null
		has_ninja=$?
		if [ $has_ninja -eq 0 ]; then
			ninja_build
			exit
		fi
		type -P gmake 2>&1 >/dev/null
		has_gmake=$?
		if [ $has_gmake -eq 0 ]; then
			make_build gmake
			exit
		fi
		type -P make 2>&1 >/dev/null
		has_make=$?
		if [ $has_make -eq 0 ]; then
			make_build make
			exit
		fi
		;;
esac

echo "No supported build system found. Aborting."
