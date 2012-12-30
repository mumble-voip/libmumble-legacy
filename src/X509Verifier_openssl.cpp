// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/X509Verifier.h>
#include "X509Verifier_openssl.h"
#include "X509HostnameVerifier.h"
#include <mumble/X509Certificate.h>
#include "X509Certificate_p.h"

#include <iostream>
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
	return dptr_->VerifyChain(chain, opts);
}

X509VerifierPrivate::X509VerifierPrivate() {
	store_ = X509_STORE_new();
	ReadSystemCAs();
}

// ReadSystemCAs ensures we've read our system CAs
// from disk. The current implementation only works
// for operating systems where OpenSSL has prior
// knowledge of where the system stores its CA
// certificates.
//
// If libmumble is built using a standalone
// OpenSSL library, and not linked against the
// system library, we should traverse a list of
// known CA locations.
void X509VerifierPrivate::ReadSystemCAs() {
#if 1
	X509_STORE_set_default_paths(store_);
#else
# error libmumble does not have a list of CA paths to search
#endif
}

// VerifyChain verifies the certificate chain in chain
// according to the verification options given as opts.
bool X509VerifierPrivate::VerifyChain(std::vector<X509Certificate> chain, const X509VerifierOptions &opts) {
	bool status = false;
	X509_STORE_CTX *ctx = X509_STORE_CTX_new();
	STACK_OF(X509) *untrusted = sk_X509_new_null();

	// Ensure that we have a chain to check on.
	if (chain.empty()) {
		goto out;
	}

	// If we've been passed a DNS name in opts,
	// we should check whether the leaf certificate
	// matches that before doing the more expensive
	// checks.
	if (!opts.dns_name.empty()) {
		if (!X509HostnameVerifier::VerifyHostname(chain.at(0), opts.dns_name)) {
			std::cerr << "X509VerifierPrivate - hostname verification failed" << std::endl;
			goto out;
		}
	}

	// Extract our chain into the format that OpenSSL
	// expects for verification.
	for (X509Certificate &cert : chain) {
		X509 *cur = cert.dptr_->AsOpenSSLX509();
		sk_X509_push(untrusted, cur);
	}

	// Set up the X509_STORE_CTX to verify according to opts.
	X509_STORE_CTX_init(ctx, store_, sk_X509_value(untrusted, 0), untrusted);
	// If a time is not specified in opts, use the current system time.
	if (opts.time == 0) {
		X509_STORE_CTX_set_time(ctx, 0, std::time(nullptr));
	} else {
		X509_STORE_CTX_set_time(ctx, 0, opts.time);
	}
	// If a dns_name is specified in opts, use the SSL server policy.
	if (!opts.dns_name.empty()) {
		X509_STORE_CTX_set_purpose(ctx, X509_PURPOSE_SSL_SERVER);
		X509_STORE_CTX_set_trust(ctx, X509_TRUST_SSL_SERVER);
	}

	if (X509_verify_cert(ctx) == 1) {
		status = true;
	} else {
		std::cerr << "X509VerifierPrivate - verification error: " << X509_verify_cert_error_string(ctx->error) << std::endl;
	}

out:
	sk_X509_pop_free(untrusted, X509_free);
	X509_STORE_CTX_free(ctx);
	return status;
}

}