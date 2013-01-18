// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_X509_VERIFIER_WIN_H_
#define MUMBLE_X509_VERIFIER_WIN_H_

#include <mumble/X509Verifier.h>

#include <vector>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>

namespace mumble {

class X509VerifierPrivate {
public:
	X509VerifierPrivate();
	bool VerifyChain(std::vector<X509Certificate> chain, const X509VerifierOptions &opts);
	static void InitializeSystemVerifier();

private:
	bool AddCertToStore(HCERTSTORE store, const X509Certificate &cert, const CERT_CONTEXT **ctx_out = nullptr) const;
	const CERT_CONTEXT *CreateCertContextFromChain(const std::vector<X509Certificate> &chain) const;

	WCHAR *X509VerifierPrivate::UTF16StringFromStdString(const std::string &str) const;
	FILETIME FileTimeFromStdTimeT(std::time_t time) const;
};

}

#endif