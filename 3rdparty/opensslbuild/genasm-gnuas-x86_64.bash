#!/bin/bash
# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

trap exit SIGINT SIGTERM

mkdir -p asm/gnuas/crypto/aes/asm
mkdir -p asm/gnuas/crypto/sha/asm
mkdir -p asm/gnuas/crypto/camellia/asm
mkdir -p asm/gnuas/crypto/md5/asm
mkdir -p asm/gnuas/crypto/whrlpool/asm
mkdir -p asm/gnuas/crypto/rc4/asm
mkdir -p asm/gnuas/crypto/bn/asm

perl ../openssl/crypto/x86_64cpuid.pl elf > asm/gnuas/crypto/x86_64cpuid.S
perl ../openssl/crypto/aes/asm/aes-x86_64.pl elf > asm/gnuas/crypto/aes/asm/aes-x86_64.S
perl ../openssl/crypto/sha/asm/sha1-x86_64.pl elf > asm/gnuas/crypto/sha/asm/sha1-x86_64.S
perl ../openssl/crypto/sha/asm/sha512-x86_64.pl elf asm/gnuas/crypto/sha/asm/sha256-x86_64.S
perl ../openssl/crypto/sha/asm/sha512-x86_64.pl elf asm/gnuas/crypto/sha/asm/sha512-x86_64.S
perl ../openssl/crypto/camellia/asm/cmll-x86_64.pl elf > asm/gnuas/crypto/camellia/asm/cmll-x86_64.S
perl ../openssl/crypto/md5/asm/md5-x86_64.pl elf > asm/gnuas/crypto/md5/asm/md5-x86_64.S
perl ../openssl/crypto/whrlpool/asm/wp-x86_64.pl elf > asm/gnuas/crypto/whrlpool/asm/wp-x86_64.S
perl ../openssl/crypto/rc4/asm/rc4-x86_64.pl elf > asm/gnuas/crypto/rc4/asm/rc4-x86_64.S
perl ../openssl/crypto/bn/asm/x86_64-mont.pl elf > asm/gnuas/crypto/bn/asm/x86_64-mont.S
