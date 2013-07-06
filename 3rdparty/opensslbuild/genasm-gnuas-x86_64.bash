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

KIND=elf
if [ "$1" == "\$win" ]; then
	KIND=mingw64
fi

mkdir -p "${SCRIPT_DIR}/asm/gnuas/crypto/aes/asm"
mkdir -p "${SCRIPT_DIR}/asm/gnuas/crypto/sha/asm"
mkdir -p "${SCRIPT_DIR}/asm/gnuas/crypto/camellia/asm"
mkdir -p "${SCRIPT_DIR}/asm/gnuas/crypto/md5/asm"
mkdir -p "${SCRIPT_DIR}/asm/gnuas/crypto/whrlpool/asm"
mkdir -p "${SCRIPT_DIR}/asm/gnuas/crypto/rc4/asm"
mkdir -p "${SCRIPT_DIR}/asm/gnuas/crypto/bn/asm"

perl "${SCRIPT_DIR}/../openssl/crypto/x86_64cpuid.pl" "${KIND}" "${SCRIPT_DIR}/asm/gnuas/crypto/x86_64cpuid.S"
perl "${SCRIPT_DIR}/../openssl/crypto/aes/asm/aes-x86_64.pl" "${KIND}" "${SCRIPT_DIR}/asm/gnuas/crypto/aes/asm/aes-x86_64.S"
perl "${SCRIPT_DIR}/../openssl/crypto/sha/asm/sha1-x86_64.pl" "${KIND}" "${SCRIPT_DIR}/asm/gnuas/crypto/sha/asm/sha1-x86_64.S"
perl "${SCRIPT_DIR}/../openssl/crypto/sha/asm/sha512-x86_64.pl" "${KIND}" "${SCRIPT_DIR}/asm/gnuas/crypto/sha/asm/sha256-x86_64.S"
perl "${SCRIPT_DIR}/../openssl/crypto/sha/asm/sha512-x86_64.pl" "${KIND}" "${SCRIPT_DIR}/asm/gnuas/crypto/sha/asm/sha512-x86_64.S"
perl "${SCRIPT_DIR}/../openssl/crypto/camellia/asm/cmll-x86_64.pl" "${KIND}" "${SCRIPT_DIR}/asm/gnuas/crypto/camellia/asm/cmll-x86_64.S"
perl "${SCRIPT_DIR}/../openssl/crypto/md5/asm/md5-x86_64.pl" "${KIND}" "${SCRIPT_DIR}/asm/gnuas/crypto/md5/asm/md5-x86_64.S"
perl "${SCRIPT_DIR}/../openssl/crypto/whrlpool/asm/wp-x86_64.pl" "${KIND}" "${SCRIPT_DIR}/asm/gnuas/crypto/whrlpool/asm/wp-x86_64.S"
perl "${SCRIPT_DIR}/../openssl/crypto/rc4/asm/rc4-x86_64.pl" "${KIND}" "${SCRIPT_DIR}/asm/gnuas/crypto/rc4/asm/rc4-x86_64.S"
perl "${SCRIPT_DIR}/../openssl/crypto/bn/asm/x86_64-mont.pl" "${KIND}" "${SCRIPT_DIR}/asm/gnuas/crypto/bn/asm/x86_64-mont.S"
