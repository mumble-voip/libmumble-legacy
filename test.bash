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

GYP=./gyp
GYPFLAGS="-I common.gypi"

function xcode_build {
	${GYP} libmumble.gyp ${GYPFLAGS} -f xcode --depth . -Dlibrary=static_library -Dopenssl_asm= -Dtarget_arch=x64 --generator-out=test
	xcodebuild -project test/libmumble.xcodeproj/ -target ${EXECUTABLE} -configuration Default CONFIGURATION_BUILD_DIR=test/build || exit 1
	./test/build/${EXECUTABLE}
}

function msvs_build {
	cmd /k test.bat
}

function ninja_build {
	${GYP} libmumble.gyp ${GYPFLAGS} -f ninja --depth . -Dlibrary=static_library -Dopenssl_asm= --generator-out=test
	ninja -C test/out/Default || exit 1
	./test/out/Default/${EXECUTABLE}
}

function make_build {
	${GYP} libmumble.gyp ${GYPFLAGS} -f make --depth . -Dlibrary=static_library -Dopenssl_asm= --generator-out=test
	make -C test/ || exit 1
	./test/out/Default/${EXECUTABLE}
}

function android_build {
	./build/android/generate.bash
	# Ensure the generated Android.mk has the same timestamp
	# as our libmumble.gyp file. This is a simplistic remedy
	# for ndk-build/make rebuilding the whole project when
	# the build files are regenerated. This will possibly
	# result in weird behavior if new gyp files are added
	# to a project without touching the libmumble.gyp file,
	# but who knows.
	touch -r libmumble.gyp ./build/android/jni/Android.mk

	cd ./build/android/jni
	ndk-build V=1 || exit 1
	adb shell mkdir -p /sdcard/libmumble
	adb shell su -c "umount /sdcard/libmumble"
	adb shell su -c "mount -t tmpfs -o size=25m none /sdcard/libmumble"
	adb push "../libs/armeabi/${EXECUTABLE}" "/sdcard/libmumble/${EXECUTABLE}"
	adb push ../../../testdata /sdcard/libmumble/testdata
	adb shell "cd /sdcard/libmumble && ./${EXECUTABLE}"
}

trap exit SIGINT SIGTERM

EXECUTABLE="${1}"
if [ "${EXECUTABLE}" == "" ]; then
	EXECUTABLE="libmumble-test"
fi

if [ "${2}" == "android" ]; then
	android_build
	exit
fi

system=$(uname -s)
case "$system" in
	Darwin)
		xcode_build
		exit
		;;
	MINGW*)
		msvs_build
		exit
		;;
	CYGWIN*)
		msvs_build
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
