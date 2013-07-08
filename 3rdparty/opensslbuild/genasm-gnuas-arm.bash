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

mkdir -p ${SCRIPT_DIR}/asm/gnuas-arm-elf/crypto/aes/asm
mkdir -p ${SCRIPT_DIR}/asm/gnuas-arm-elf/crypto/bn/asm
mkdir -p ${SCRIPT_DIR}/asm/gnuas-arm-elf/crypto/sha/asm

perl ${SCRIPT_DIR}/../openssl/crypto/aes/asm/aes-armv4.pl > ${SCRIPT_DIR}/asm/gnuas-arm-elf/crypto/aes/asm/aes-armv4.S
perl ${SCRIPT_DIR}/../openssl/crypto/bn/asm/armv4-mont.pl > ${SCRIPT_DIR}/asm/gnuas-arm-elf/crypto/bn/asm/armv4-mont.S
perl ${SCRIPT_DIR}/../openssl/crypto/sha/asm/sha1-armv4-large.pl > ${SCRIPT_DIR}/asm/gnuas-arm-elf/crypto/sha/asm/sha1-armv4-large.S
perl ${SCRIPT_DIR}/../openssl/crypto/sha/asm/sha256-armv4.pl > ${SCRIPT_DIR}/asm/gnuas-arm-elf/crypto/sha/asm/sha256-armv4.S
perl ${SCRIPT_DIR}/../openssl/crypto/sha/asm/sha512-armv4.pl > ${SCRIPT_DIR}/asm/gnuas-arm-elf/crypto/sha/asm/sha512-armv4.S