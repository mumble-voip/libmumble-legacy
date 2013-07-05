#!/bin/bash
# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

trap exit SIGINT SIGTERM

SCRIPT_DIR=$(dirname ${BASH_SOURCE})
cat ${1} > ${SCRIPT_DIR}/../openssl/crypto/opensslconf.h
touch -r ${1} ${SCRIPT_DIR}/../openssl/crypto/opensslconf.h 
