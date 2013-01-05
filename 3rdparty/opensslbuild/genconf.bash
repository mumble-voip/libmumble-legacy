#!/bin/bash
# Copyright (c) 2013 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

trap exit SIGINT SIGTERM

unlink ../openssl/crypto/opensslconf.h
cat $* > ../openssl/crypto/opensslconf.h
