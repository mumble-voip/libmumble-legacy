#!/bin/bash
# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

# This script generates the assembly source files
# that are needed to build OpenSSL.

trap exit SIGINT SIGTERM

# Generate gnuas-x86-elf
./genasm-gnuas-x86.bash elf

# Generate gnuas-x86-macosx
./genasm-gnuas-x86.bash macosx

# Generate gnuas-x86_64-elf
./genasm-gnuas-x86_64.bash elf

# Generate gnuas-x86_64-mingw64
./genasm-gnuas-x86_64.bash mingw64

# Generate gnuas-x86_64-macosx
./genasm-gnuas-x86_64.bash macosx