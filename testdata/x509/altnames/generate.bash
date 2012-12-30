#!/bin/bash
# Copyright (c) 2012 The libmumble Developers
# The use of this source code is goverened by a BSD-style
# license that can be found in the LICENSE-file.

# Generate a certifiacte with DNS subject alt names
openssl genrsa -out /dev/stdout 2048 | openssl rsa -inform PEM -outform DER > dns.key
openssl req -new -keyform DER -key dns.key -subj "/O=libmumble" -out dns.csr -batch
openssl x509 -extfile dns.cnf -extensions v3_req -req -days 365 -in dns.csr -keyform DER -signkey dns.key -out dns.crt -outform DER
rm dns.csr

# Generate a certificate with email subject alt names
openssl genrsa -out /dev/stdout 2048 | openssl rsa -inform PEM -outform DER > email.key
openssl req -new -keyform DER -key email.key -subj "/O=libmumble" -out email.csr -batch
openssl x509 -extfile email.cnf -extensions v3_req -req -days 365 -in email.csr -keyform DER -signkey email.key -out email.crt -outform DER
rm email.csr
