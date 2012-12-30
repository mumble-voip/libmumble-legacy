// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_X509_CERTIFICATE_H_
#define MUMBLE_X509_CERTIFICATE_H_

#include <mumble/ByteArray.h>

#include <memory>
#include <vector>
#include <ctime>
#include <string>

namespace mumble {

class X509VerifierPrivate;
class X509CertificatePrivate;

class X509Certificate {
public:
	static X509Certificate FromRawDERData(const ByteArray &cert, const ByteArray &pkey = ByteArray());
	static X509Certificate GenerateSelfSignedCertificate(const std::string &name = std::string(), const std::string &email = std::string());
	static std::vector<X509Certificate> FromPKCS12(const ByteArray &pkcs12, const std::string &password);
	static ByteArray ExportCertificateChainAsPKCS12(std::vector<X509Certificate> chain, const std::string &password);

	X509Certificate();
	~X509Certificate();
	X509Certificate(const X509Certificate &cert);
	X509Certificate& operator=(X509Certificate cert);

	bool HasCertificate() const;
	bool HasPrivateKey() const;

	ByteArray SHA1Digest() const ;
	ByteArray SHA256Digest() const;

	std::time_t NotBeforeTime();
	std::time_t NotAfterTime();

	bool IsSignedBy(const X509Certificate &parent);
	bool IsValidAtTime(std::time_t time);

	std::string SubjectName();
	std::string CommonName();
	std::string EmailAddress();
	std::vector<std::string> DNSNames();
	std::string IssuerName();

	std::string LookupIssuerItem(const std::string &item);
	std::string LookupSubjectItem(const std::string &item);

private:
	friend class X509VerifierPrivate;
	friend class X509CertificatePrivate;
	std::unique_ptr<X509CertificatePrivate> dptr_;
};

}

#endif
