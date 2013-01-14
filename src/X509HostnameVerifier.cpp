// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include "X509HostnameVerifier.h"
#include <mumble/X509Certificate.h>

namespace mumble {

// VerifyHostname verifies that the certificate at cert is valid
// for connections to the passed-in hostname.
bool X509HostnameVerifier::VerifyHostname(const X509Certificate &cert, const std::string &hostname) {
	std::vector<std::string> dns_names = cert.DNSNames();
	if (dns_names.size() > 0) {
		for (auto dns_name : dns_names) {
			if (X509HostnameVerifier::HostnameExprMatch(dns_name, hostname))
				return true;
		}
	} else {
		std::string cn = cert.CommonName();
		if (X509HostnameVerifier::HostnameExprMatch(cn, hostname))
			return true;
	}

	return false;
}

// HostnameExprMatch checks whether hostname matches hostexpr. While
// hostname must be a valid DNS name, hostexpr may contain wildcards,
// such as *.example.com. A wild card character matches a single
// domain component. That is, a.b.example.com cannot match
// *.example.com.
//
// The matching is done in a case-insensitive manner. The strings
// in hostexpr and hostname must be ASCII strings. If IDNs are used,
// this means they must be ACE-encoded.
bool X509HostnameVerifier::HostnameExprMatch(const std::string &hostexpr, const std::string &hostname) {
	std::vector<std::string> vhostexpr = X509HostnameVerifier::LowerCaseHostnameVector(hostexpr);
	std::vector<std::string> vhostname = X509HostnameVerifier::LowerCaseHostnameVector(hostname);

	if (vhostexpr.size() == 0 || vhostname.size() == 0) {
		return false;
	}

	if (vhostexpr.size() != vhostname.size()) {
		return false;
	}

	for (int i = 0; i < vhostname.size(); i++) {
		const std::string &cexpr = vhostexpr.at(i);
		const std::string &chost = vhostname.at(i);

		if (cexpr.compare("*") == 0) {
			continue;
		} else {
			if (cexpr.compare(chost) != 0) {
				return false;
			}
		}
	}

	return true;
}

// LowerCaseHostnameVector splits the hostname into components separated by '.'
// while ensuring that each component is represented as lower-case ASCII.
std::vector<std::string> X509HostnameVerifier::LowerCaseHostnameVector(const std::string &hostname) {
	std::vector<std::string> components;
	std::string lower;

	for (int i = 0; i < hostname.size(); i++) {
		char c = hostname.at(i);
		if (c == '.') {
			components.push_back(lower);
			lower.erase();
			continue;
		}
		if (c >= 'A' && c <= 'Z') {
			lower.append(1, c - 'A' + 'a');
		} else {
			lower.append(1, c);
		}
	}
	components.push_back(lower);

	return components;
}

}