// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/X509Verifier.h>
#include "X509Verifier_openssl.h"
#include "X509PEMVerifier.h"

#include <vector>
#include <string>

#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include "uv.h"

namespace mumble {

static uv_once_t     system_verifier_once_ = UV_ONCE_INIT;
static X509Verifier *system_verifier_ptr_;

void X509VerifierPrivate::InitializeSystemVerifier() {
	system_verifier_ptr_ = new X509Verifier;
}

X509Verifier &X509Verifier::SystemVerifier() {
	uv_once(&system_verifier_once_, X509VerifierPrivate::InitializeSystemVerifier);
	return *system_verifier_ptr_;
}

X509Verifier::X509Verifier() : dptr_(new X509VerifierPrivate) {
}

X509Verifier::~X509Verifier() {
}

bool X509Verifier::VerifyChain(std::vector<X509Certificate> chain, const X509VerifierOptions &opts) {
	return dptr_->pem_verifier_.VerifyChain(chain, opts);
}

X509VerifierPrivate::X509VerifierPrivate() {
#if 1
	X509_STORE_set_default_paths(pem_verifier_.store_);
#else
# error libmumble does not have a list of CA paths to search
#endif
}

}