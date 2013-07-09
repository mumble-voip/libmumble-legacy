#!/bin/bash
# Copyright (c) 2012 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

# Generate a self-signed server certificate valid for one year, with CN=mumble.example.com
# as well as the usual server key usages, and mumble.example.com as a DNS alt name.
openssl genrsa -out /dev/stdout 2048 | openssl rsa -inform PEM -outform DER > selfsign-server.key
openssl req -new -keyform DER -key selfsign-server.key -subj "/CN=mumble.example.com" -out selfsign-server.csr -batch
openssl x509 -req -extfile selfsign-server.cnf -extensions v3_req -days 365 -in selfsign-server.csr -keyform DER -signkey selfsign-server.key -out selfsign-server.crt -outform DER
rm selfsign-server.csr
