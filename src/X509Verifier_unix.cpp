// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/X509Verifier.h>
#include <mumble/X509Certificate.h>
#include "X509Verifier_unix.h"
#include "X509PEMVerifier.h"

#include <vector>
#include <string>
#include <fstream>

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

// read_caroot_pem reads a CA certificate named fn from dir.
ByteArray read_pem_bundle(const std::string &fn) {
	mumble::ByteArray ba;
	std::ifstream ifs;
	ifs.open(fn, std::ios::binary);
	while (ifs.good()) {
		mumble::ByteArray chunk(256);
		char *buf = chunk.Data();
		ifs.read(buf, chunk.Length());
		chunk.Truncate(ifs.gcount());
		if (chunk.Length() > 0) {
			ba.Append(chunk);
		}
	}
	ifs.close();
	return ba;
}

X509VerifierPrivate::X509VerifierPrivate() {
#if defined(LIBMUMBLE_SYSTEM_OPENSSL)
	X509_STORE_set_default_paths(pem_verifier_.store_);
#else
	const char *bundles[] = {
		// Debian and Ubuntu
		"/etc/ssl/certs/ca-certificates.crt",
	};

	int nbundles = sizeof(bundles)/sizeof(*bundles);
	for (int i = 0; i < nbundles; i++) {
		ByteArray bundle = read_pem_bundle(std::string(bundles[i]));
		if (bundle.Length() > 0) {
			pem_verifier_.AddPEM(bundle);
			break;
		}
	}
#endif
}

}