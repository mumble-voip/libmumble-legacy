// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <gtest/gtest.h>
#include "mumble_test.h"

#include <mumble/X509Verifier.h>
#include <mumble/X509Certificate.h>

#include <cstdlib>
#include <cstring>
#include <vector>

TEST(X509Verifier, TestGoogleDKFullChain) {
	mumble::X509Verifier &verifier = mumble::X509Verifier::SystemVerifier();
	
	mumble::X509VerifierOptions opts;
	opts.dns_name = std::string("www.google.dk");
	opts.time = 1356880850; // Sun, 30 Dec 2012 15:20:50 GMT

	mumble::ByteArray pk12 = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-chain.p12"));
	ASSERT_FALSE(pk12.IsNull());

	std::vector<mumble::X509Certificate> chain = mumble::X509Certificate::FromPKCS12(pk12, std::string());
	ASSERT_EQ(3, chain.size());

	EXPECT_TRUE(verifier.VerifyChain(chain, opts));
}

TEST(X509Verifier, TestGoogleDKLeafAndIntermediates) {
	mumble::X509Verifier &verifier = mumble::X509Verifier::SystemVerifier();
	
	mumble::X509VerifierOptions opts;
	opts.dns_name = std::string("www.google.dk");
	opts.time = 1356880850; // Sun, 30 Dec 2012 15:20:50 GMT

	mumble::ByteArray pk12 = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-chain.p12"));
	ASSERT_FALSE(pk12.IsNull());

	std::vector<mumble::X509Certificate> chain = mumble::X509Certificate::FromPKCS12(pk12, std::string());
	ASSERT_EQ(3, chain.size());

	// Chop away the root certificate.
	chain.resize(2);
	ASSERT_EQ(2, chain.size());

	EXPECT_TRUE(verifier.VerifyChain(chain, opts));
}

TEST(X509Verifier, TestGoogleDKLeafAndIntermediatesTimeValidity) {
	mumble::X509Verifier &verifier = mumble::X509Verifier::SystemVerifier();

	mumble::ByteArray pk12 = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-chain.p12"));
	ASSERT_FALSE(pk12.IsNull());

	std::vector<mumble::X509Certificate> chain = mumble::X509Certificate::FromPKCS12(pk12, std::string());
	ASSERT_EQ(3, chain.size());

	// Chop away the root certificate.
	chain.resize(2);
	ASSERT_EQ(2, chain.size());

	mumble::X509VerifierOptions opts;
	opts.dns_name = std::string("www.google.dk");

	// Trigger a Not Before trap
	opts.time = 1324771931; // Sun, 25 Dec 2011 00:12:11 GMT
	EXPECT_FALSE(verifier.VerifyChain(chain, opts));

	// Trigger a Not After trap
	opts.time = 1451002331; // Fri, 25 Dec 2015 00:12:11 GMT
	EXPECT_FALSE(verifier.VerifyChain(chain, opts));
}

TEST(X509Verifier, TestGoogleDKLeafAndIntermediatesBadHostname) {
	mumble::X509Verifier &verifier = mumble::X509Verifier::SystemVerifier();
	
	mumble::X509VerifierOptions opts;
	opts.dns_name = std::string("www.facebook.com");
	opts.time = 1356880850; // Sun, 30 Dec 2012 15:20:50 GMT

	mumble::ByteArray pk12 = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-chain.p12"));
	ASSERT_FALSE(pk12.IsNull());

	std::vector<mumble::X509Certificate> chain = mumble::X509Certificate::FromPKCS12(pk12, std::string());
	ASSERT_EQ(3, chain.size());

	// Chop away the root certificate.
	chain.resize(2);
	ASSERT_EQ(2, chain.size());

	// Should trigger the hostname veriifer, or possibly
	// the system verifier.
	EXPECT_FALSE(verifier.VerifyChain(chain, opts));
}

TEST(X509Verifier, TestSelfSignedClientCert) {
	mumble::X509Verifier &verifier = mumble::X509Verifier::SystemVerifier();
	
	mumble::X509VerifierOptions opts;
	opts.dns_name = std::string("mumble.example.com");
	opts.time = 1356880850; // Sun, 30 Dec 2012 15:20:50 GMT

	mumble::ByteArray cert_der = LoadFile(std::string("testdata/x509/selfsign/self.crt"));
	ASSERT_FALSE(cert_der.IsNull());

	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(cert_der);
	std::vector<mumble::X509Certificate> chain;
	chain.push_back(cert);
	ASSERT_EQ(1, chain.size());

	// This should generate a hostname failure, since we're trying with a client cert.
	EXPECT_FALSE(verifier.VerifyChain(chain, opts));
}

TEST(X509Verifier, TestSelfSignedServerCert) {
	mumble::X509Verifier &verifier = mumble::X509Verifier::SystemVerifier();
	
	mumble::X509VerifierOptions opts;
	opts.dns_name = std::string("mumble.example.com");
	opts.time = 1356882805; // Sun, 30 Dec 2012 15:53:25 GMT

	mumble::ByteArray cert_der = LoadFile(std::string("testdata/x509/selfsign-server/self.crt"));
	ASSERT_FALSE(cert_der.IsNull());

	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(cert_der);
	std::vector<mumble::X509Certificate> chain;
	chain.push_back(cert);
	ASSERT_EQ(1, chain.size());

	// The hostname verifier (or system verifier) should
	// let this one pass, triggering an actual X509 verification.
	EXPECT_FALSE(verifier.VerifyChain(chain, opts));
}

TEST(X509Verifier, TestEmptyChain) {
	mumble::X509Verifier &verifier = mumble::X509Verifier::SystemVerifier();
	mumble::X509VerifierOptions opts;
	EXPECT_FALSE(verifier.VerifyChain(std::vector<mumble::X509Certificate>(), opts));
}
