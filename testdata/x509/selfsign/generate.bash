#!/bin/bash
# Copyright (c) 2012 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

# Generate a self-signed certificate valid for one year, with CN=SelfSignedTest
openssl genrsa -out /dev/stdout 2048 | openssl rsa -inform PEM -outform DER > self.key
openssl req -new -keyform DER -key self.key -subj "/CN=SelfSignedTest" -out self.csr -batch
openssl x509 -req -days 365 -in self.csr -keyform DER -signkey self.key -out self.crt -outform DER
rm self.csr
