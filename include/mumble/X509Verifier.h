// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_X509_VERIFIER_H_
#define MUMBLE_X509_VERIFIER_H_

#include <memory>
#include <string>
#include <vector>
#include <ctime>

namespace mumble {

/// X509VerifierOptions specifies options for an X509Verifier.
struct X509VerifierOptions {
	/// The DNS name to verify against.
	/// If this field is not empty, the verifier will
	/// treat the certificate chain as being an SSL server
	/// chain, and verify it accordingly.
	std::string   dns_name;
	/// The time that the verifier should use when checking
	/// the validity period of the certificate chain. If this
	/// field is 0, the system time is used.
	std::time_t   time;
};

class X509Certificate;
class X509VerifierPrivate;

/// X509Verifier implements objects that can verify X509 certificate chains.
class X509Verifier {
public:
	/// SystemVerifier returns an X509Verifier that calls
	/// the operating system's native X509 verification routines.
	static X509Verifier &SystemVerifier();

	/// VerifyChain verifies a chain of X509Certificates.
	///
	/// @param   chain  The certificate chain to verify.
	/// @param   opts   Options to use during the verification process.
	///
	/// @return  Returns true if the verifier deemed the certificate
	///          valid for the use given in opts.
	bool VerifyChain(std::vector<X509Certificate> chain, const X509VerifierOptions &opts);

private:
	X509Verifier();
	X509Verifier(const X509Verifier &verifier);
	X509Verifier& operator=(X509Verifier verifier);
	~X509Verifier();

	friend class X509VerifierPrivate;
	std::unique_ptr<X509VerifierPrivate> dptr_;
};

}

#endif