#!/bin/bash
# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

trap exit SIGINT SIGTERM

mkdir -p asm/gnuas/crypto/aes/asm
mkdir -p asm/gnuas/crypto/cast/asm
mkdir -p asm/gnuas/crypto/sha/asm
mkdir -p asm/gnuas/crypto/md5/asm
mkdir -p asm/gnuas/crypto/ripemd/asm
mkdir -p asm/gnuas/crypto/bf/asm
mkdir -p asm/gnuas/crypto/des/asm
mkdir -p asm/gnuas/crypto/rc4/asm
mkdir -p asm/gnuas/crypto/bn/asm

perl ../openssl/crypto/x86cpuid.pl elf > asm/gnuas/crypto/x86cpuid.S # -DOPENSSL_IA32_SSE2 for SSE2
perl ../openssl/crypto/aes/asm/aes-586.pl elf 386 > asm/gnuas/crypto/bn/asm/aes-586.S # remove 386 for SSE
perl ../openssl/crypto/cast/asm/cast-586.pl elf > asm/gnuas/crypto/cast/asm/cast-586.S
perl ../openssl/crypto/sha/asm/sha1-586.pl elf > asm/gnuas/crypto/sha/asm/sha1-586.S
perl ../openssl/crypto/sha/asm/sha256-586.pl elf > asm/gnuas/crypto/sha/asm/sha256-586.S
perl ../openssl/crypto/sha/asm/sha512-586.pl elf > asm/gnuas/crypto/sha/asm/sha512-586.S # -DOPENSSL_IA32_SSE2 for SSE2
perl ../openssl/crypto/md5/asm/md5-586.pl elf > asm/gnuas/crypto/md5/asm/md5-586.S
perl ../openssl/crypto/ripemd/asm/rmd-586.pl elf > asm/gnuas/crypto/md5/asm/rmd-586.S
perl ../openssl/crypto/bf/asm/bf-586.pl elf > asm/gnuas/crypto/bf/asm/bf-586.S
perl ../openssl/crypto/des/asm/des-586.pl elf > asm/gnuas/crypto/des/asm/des-586.S
perl ../openssl/crypto/des/asm/crypt586.pl elf > asm/gnuas/crypto/des/asm/crypt586.S
perl ../openssl/crypto/rc4/asm/rc4-586.pl elf > asm/gnuas/crypto/rc4/asm/rc4-586.S
perl ../openssl/crypto/bn/asm/bn-586.pl elf > asm/gnuas/crypto/bn/asm/bn-586.S # -DOPENSSL_IA32_SSE2 for SSE2
perl ../openssl/crypto/bn/asm/co-586.pl elf > asm/gnuas/crypto/bn/asm/co-586.S
perl ../openssl/crypto/bn/asm/x86-mont.pl elf > asm/gnuas/crypto/bn/asm/x86-mont.S # -DOPENSSL_IA32_SSE2 for SSE2
