#!/bin/bash
# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

# Check that the basenames of files in the testdata
# directory are unique. On iOS, we copy them directly
# into the app bundle, and non-unique filenames would
# cause weird test failures.
TEST_FILES=$(find testdata -type f | grep -v \.bash$ | xargs basename | sort | openssl dgst -sha1)
UNIQUE_TEST_FILES=$(find testdata -type f | grep -v \.bash$ | xargs basename | sort | uniq | openssl dgst -sha1)
if [ "${TEST_FILES}" != "${UNIQUE_TEST_FILES}" ]; then
	exit 1
fi