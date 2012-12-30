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

struct X509VerifierOptions {
	std::string   dns_name;
	std::time_t   time;
};

class X509Certificate;
class X509VerifierPrivate;

class X509Verifier {
public:
	static X509Verifier &SystemVerifier();
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