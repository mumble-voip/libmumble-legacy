// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_X509_VERIFIER_ANDROID_H_
#define MUMBLE_X509_VERIFIER_ANDROID_H_

#include <mumble/X509Verifier.h>
#include "X509PEMVerifier.h"

#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include <vector>
#include <string>

namespace mumble {

class X509VerifierPrivate {
public:
	X509VerifierPrivate();
	static void InitializeSystemVerifier();
	void ReadSystemRoots();
	X509PEMVerifier pem_verifier_;
};

}

#endif