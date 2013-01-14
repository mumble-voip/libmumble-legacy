// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_X509_PEM_VERIFIER_H_
#define MUMBLE_X509_PEM_VERIFIER_H_

#include <mumble/X509Verifier.h>
#include <mumble/ByteArray.h>

#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include <vector>
#include <string>

namespace mumble {

class X509PEMVerifier {
public:
	X509PEMVerifier();
	bool AddPEM(const ByteArray &buf);
	bool VerifyChain(const std::vector<X509Certificate> &chain, const X509VerifierOptions &opts);
private:
	friend class X509VerifierPrivate;
	X509_STORE *store_;
};

}

#endif