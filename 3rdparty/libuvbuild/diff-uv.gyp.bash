#!/bin/bash
# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

trap exit SIGINT SIGTERM

FN=${BASH_SOURCE}
type -P cygpath 2>&1 >/dev/null
if [ $? -eq 0 ]; then
	FN=$(cygpath -u "${FN}")
fi
SCRIPT_DIR=$(dirname ${FN})

# Show a diff between our modified uv.gyp and the
# uv.gyp in 3rdparty/libuv without taking the path
# changes that occur from living in 3rdparty/libuvbuild
# instead of 3rdparty/libuv into consideration.
cat ${SCRIPT_DIR}/uv.gyp | sed 's,../libuv/,,' | diff -urNp ${SCRIPT_DIR}/../libuv/uv.gyp /dev/stdin