#!/bin/bash
# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

# Re-generates the Android NDK build files.
# Must be run from the root of the source tree.
GYP=./gyp
${GYP} libmumble.gyp -f android -G android_ndk_version=1 --depth . -Dlibrary=static_library -Dopenssl_asm= --generator-out=build/android/jni
