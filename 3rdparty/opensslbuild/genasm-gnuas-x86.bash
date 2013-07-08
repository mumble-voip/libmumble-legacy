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

mkdir -p ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/aes/asm
mkdir -p ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/cast/asm
mkdir -p ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/sha/asm
mkdir -p ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/md5/asm
mkdir -p ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/ripemd/asm
mkdir -p ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/bf/asm
mkdir -p ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/des/asm
mkdir -p ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/rc4/asm
mkdir -p ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/bn/asm

perl ${SCRIPT_DIR}/../openssl/crypto/x86cpuid.pl ${KIND} > ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/x86cpuid.S # -DOPENSSL_IA32_SSE2 for SSE2
perl ${SCRIPT_DIR}/../openssl/crypto/aes/asm/aes-586.pl ${KIND} 386 >  ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/bn/asm/aes-586.S # remove 386 for SSE
perl ${SCRIPT_DIR}/../openssl/crypto/cast/asm/cast-586.pl ${KIND} > ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/cast/asm/cast-586.S
perl ${SCRIPT_DIR}/../openssl/crypto/sha/asm/sha1-586.pl ${KIND} > ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/sha/asm/sha1-586.S
perl ${SCRIPT_DIR}/../openssl/crypto/sha/asm/sha256-586.pl ${KIND} > ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/sha/asm/sha256-586.S
perl ${SCRIPT_DIR}/../openssl/crypto/sha/asm/sha512-586.pl ${KIND} > ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/sha/asm/sha512-586.S # -DOPENSSL_IA32_SSE2 for SSE2
perl ${SCRIPT_DIR}/../openssl/crypto/md5/asm/md5-586.pl ${KIND} > ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/md5/asm/md5-586.S
perl ${SCRIPT_DIR}/../openssl/crypto/ripemd/asm/rmd-586.pl ${KIND} > ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/md5/asm/rmd-586.S
perl ${SCRIPT_DIR}/../openssl/crypto/bf/asm/bf-586.pl ${KIND} > ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/bf/asm/bf-586.S
perl ${SCRIPT_DIR}/../openssl/crypto/des/asm/des-586.pl ${KIND} > ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/des/asm/des-586.S
perl ${SCRIPT_DIR}/../openssl/crypto/des/asm/crypt586.pl ${KIND} > ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/des/asm/crypt586.S
perl ${SCRIPT_DIR}/../openssl/crypto/rc4/asm/rc4-586.pl ${KIND} > ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/rc4/asm/rc4-586.S
perl ${SCRIPT_DIR}/../openssl/crypto/bn/asm/bn-586.pl ${KIND} > ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/bn/asm/bn-586.S # -DOPENSSL_IA32_SSE2 for SSE2
perl ${SCRIPT_DIR}/../openssl/crypto/bn/asm/co-586.pl ${KIND} > ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/bn/asm/co-586.S
perl ${SCRIPT_DIR}/../openssl/crypto/bn/asm/x86-mont.pl ${KIND} > ${SCRIPT_DIR}/asm/gnuas-x86-${KIND}/crypto/bn/asm/x86-mont.S # -DOPENSSL_IA32_SSE2 for SSE2
