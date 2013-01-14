// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_X509_HOSTNAME_VERIFIER_P_H_
#define MUMBLE_X509_HOSTNAME_VERIFIER_P_H_

#include <string>
#include <vector>

namespace mumble {

class X509Certificate;

class X509HostnameVerifier {
public:
	static std::vector<std::string> LowerCaseHostnameVector(const std::string &hostname);
	static bool HostnameExprMatch(const std::string &hostexpr, const std::string &hostname);
	static bool VerifyHostname(const X509Certificate &cert, const std::string &hostname);
};

}

#endif