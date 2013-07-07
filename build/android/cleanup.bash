#!/bin/bash
# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

# Cleans up the Android NDK build files.
# Must be run from the root of the source tree.

rm -rf build/android/libs
rm -rf build/android/obj
rm -rf build/android/jni/3rdparty
rm -rf build/android/jni/Android.mk
rm -rf build/android/jni/*.target.mk