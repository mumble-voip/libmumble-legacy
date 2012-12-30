// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_X509_VERIFIER_OPENSSL_H_
#define MUMBLE_X509_VERIFIER_OPENSSL_H_

#include <mumble/X509Verifier.h>

#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include <vector>
#include <string>

namespace mumble {

class X509VerifierPrivate {
public:
	X509VerifierPrivate();
	void ReadSystemCAs();
	bool VerifyChain(std::vector<X509Certificate> chain, const X509VerifierOptions &opts);
	static void InitializeSystemVerifier();

	X509_STORE *store_;
};

}

#endif