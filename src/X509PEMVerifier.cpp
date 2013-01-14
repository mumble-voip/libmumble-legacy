// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include "X509PEMVerifier.h"
#include "X509HostnameVerifier.h"
#include <mumble/X509Certificate.h>
#include "X509Certificate_p.h"
#include <mumble/ByteArray.h>

#include <iostream>
#include <vector>
#include <string>

#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include "uv.h"

namespace mumble {

X509PEMVerifier::X509PEMVerifier() {
	store_ = X509_STORE_new();
}

// Adds a buffer containing one or more PEM-encoded
// root certificates to the X509PEMVerifier.
//
// If the certificate (or one of the certificates) could not be
// parsed AddPEM will return immediately, resulting in all of the
// certificates up to the bad certicate being added to the verifier.
bool X509PEMVerifier::AddPEM(const ByteArray &buf) {
	BIO *mem = BIO_new_mem_buf(static_cast<void *>(const_cast<char *>(buf.ConstData())), buf.Length());
	(void) BIO_set_close(mem, BIO_NOCLOSE);

	int ncerts = 0;
	while (1) {
		X509 *x = PEM_read_bio_X509_AUX(mem, nullptr, nullptr, nullptr);
		if (x == nullptr) {
			return false;
		}
		X509_STORE_add_cert(store_, x);
		X509_free(x);
		ncerts++;
	}

	return true;
}

// VerifyChain verifies the certificate chain in chain
// according to the verification options given as opts.
bool X509PEMVerifier::VerifyChain(const std::vector<X509Certificate> &chain, const X509VerifierOptions &opts) {
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
			std::cerr << "X509PEMVerifier - hostname verification failed" << std::endl;
			goto out;
		}
	}

	// Extract our chain into the format that OpenSSL
	// expects for verification.
	for (const X509Certificate &cert : chain) {
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
		std::cerr << "X509PEMVerifier - verification error: " << X509_verify_cert_error_string(ctx->error) << std::endl;
	}

out:
	sk_X509_pop_free(untrusted, X509_free);
	X509_STORE_CTX_free(ctx);
	return status;
}

}