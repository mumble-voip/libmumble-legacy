#!/bin/bash
# Copyright (c) 2012 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

# Generate a self-signed certificate valid for one year, with CN=SelfSignedTest
openssl genrsa -out /dev/stdout 2048 | openssl rsa -inform PEM -outform DER > tricky.key
openssl req -new -keyform DER -key tricky.key -subj "/CN=Name With A Comma, \"and another one, haha\"/O=ACME" -out tricky.csr -batch
openssl x509 -req -days 365 -in tricky.csr -keyform DER -signkey tricky.key -out tricky.crt -outform DER
rm tricky.csr
