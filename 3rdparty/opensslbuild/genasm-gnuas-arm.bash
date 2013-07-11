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

KIND="$1"

mkdir -p ${SCRIPT_DIR}/asm/gnuas-arm-${KIND}/crypto/aes/asm
mkdir -p ${SCRIPT_DIR}/asm/gnuas-arm-${KIND}/crypto/bn/asm
mkdir -p ${SCRIPT_DIR}/asm/gnuas-arm-${KIND}/crypto/sha/asm

case "$KIND" in
	elf )
		perl ${SCRIPT_DIR}/../openssl/crypto/aes/asm/aes-armv4.pl > ${SCRIPT_DIR}/asm/gnuas-arm-${KIND}/crypto/aes/asm/aes-armv4.S
		perl ${SCRIPT_DIR}/../openssl/crypto/bn/asm/armv4-mont.pl > ${SCRIPT_DIR}/asm/gnuas-arm-${KIND}/crypto/bn/asm/armv4-mont.S
		perl ${SCRIPT_DIR}/../openssl/crypto/sha/asm/sha1-armv4-large.pl > ${SCRIPT_DIR}/asm/gnuas-arm-${KIND}/crypto/sha/asm/sha1-armv4-large.S
		perl ${SCRIPT_DIR}/../openssl/crypto/sha/asm/sha256-armv4.pl > ${SCRIPT_DIR}/asm/gnuas-arm-${KIND}/crypto/sha/asm/sha256-armv4.S
		perl ${SCRIPT_DIR}/../openssl/crypto/sha/asm/sha512-armv4.pl > ${SCRIPT_DIR}/asm/gnuas-arm-${KIND}/crypto/sha/asm/sha512-armv4.S
		;;
	macosx )
		perl ${SCRIPT_DIR}/../openssl/crypto/aes/asm/aes-armv4.pl | grep -v "^\.[type|size].*" > ${SCRIPT_DIR}/asm/gnuas-arm-${KIND}/crypto/aes/asm/aes-armv4.S
		perl ${SCRIPT_DIR}/../openssl/crypto/bn/asm/armv4-mont.pl | grep -v "^\.[type|size].*" | sed 's,bn_mul_mont,_bn_mul_mont,' > ${SCRIPT_DIR}/asm/gnuas-arm-${KIND}/crypto/bn/asm/armv4-mont.S
		perl ${SCRIPT_DIR}/../openssl/crypto/sha/asm/sha1-armv4-large.pl | grep -v "^\.[type|size].*" | sed 's,sha1_block_data_order,_sha1_block_data_order,'> ${SCRIPT_DIR}/asm/gnuas-arm-${KIND}/crypto/sha/asm/sha1-armv4-large.S
		perl ${SCRIPT_DIR}/../openssl/crypto/sha/asm/sha256-armv4.pl | grep -v "^\.[type|size].*" | sed 's,sha256_block_data_order,_sha256_block_data_order,' > ${SCRIPT_DIR}/asm/gnuas-arm-${KIND}/crypto/sha/asm/sha256-armv4.S
		perl ${SCRIPT_DIR}/../openssl/crypto/sha/asm/sha512-armv4.pl | grep -v "^\.[type|size].*" | sed 's,sha512_block_data_order,_sha512_block_data_order,' > ${SCRIPT_DIR}/asm/gnuas-arm-${KIND}/crypto/sha/asm/sha512-armv4.S
		;;
esac
