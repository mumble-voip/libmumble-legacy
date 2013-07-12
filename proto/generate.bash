#!/bin/bash
# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

# Generate an updated proto description from Mumble.proto.
# Remember to run 'git clean -dfx' in 3rdparty/protobuf once
# you are done re-generating the .pb.* files.

FN=${BASH_SOURCE}
type -P cygpath 2>&1 >/dev/null
if [ $? -eq 0 ]; then
	FN=$(cygpath -u "${FN}")
fi
SCRIPT_DIR=$(dirname ${FN})

cd ${SCRIPT_DIR}/../3rdparty/protobuf

if [ ! -f src/protoc ]; then
	echo "No existing protoc binary found in 3rdparty/protobuf"
	echo "Re-building protoc binary in 3rdparty/protobuf"
	./autogen.sh
	./configure
	make -j4
else
	echo "Using existing 3rdparty/protobuf protoc compiler"
fi

src/protoc ../../proto/Mumble.proto \
	--proto_path=../../proto/ \
	--cpp_out=../../proto/

# There seems to be a bug when generating for protobuf-lite? Remove
# it with some magic sed powder for now.
sed -i '' -e 's,::google::protobuf::::,::,' ../../proto/Mumble.pb.cc