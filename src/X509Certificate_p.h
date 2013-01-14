// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_X509_CERTIFICATE_P_H_
#define MUMBLE_X509_CERTIFICATE_P_H_

#include <mumble/ByteArray.h>

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pkcs12.h>

#include <string>
#include <list>
#include <vector>
#include <map>
#include <ctime>

namespace mumble {

class X509CertificatePrivate {
public:
	X509CertificatePrivate();
	void ExtractCertificateInfo();

	static std::time_t ParseASN1Time(ASN1_TIME *time);
	static std::string StringFromDNAttributeType(ASN1_OBJECT *obj);
	static std::string StringFromASN1String(ASN1_STRING *str);
	static std::map<std::string, std::string> ParseX509Name(X509_NAME *name);

	std::map<std::string, std::string> subject_items_;
	std::map<std::string, std::string> issuer_items_;

	std::list<std::string> email_addrs_;
	std::list<std::string> dns_names_;

	ByteArray cert_der_;
	ByteArray priv_der_;

	std::time_t not_before_;
	std::time_t not_after_;

	static X509Certificate GenerateSelfSignedCertificate(const std::string &name, const std::string &email);
	// ExportCertificateChainAsPKCs12 exports the certificate chain in chain as a PKCS12-encoded ByteArray.
	static ByteArray ExportCertificateChainAsPKCS12(std::vector<X509Certificate>chain, const std::string &password);
	static std::vector<X509Certificate> FromPKCS12(const ByteArray &pkcs12, const std::string &password);

	X509 *AsOpenSSLX509() const;

	ByteArray Digest(const std::string &name) const;

	std::time_t NotBeforeTime() const;
	std::time_t NotAfterTime() const;

	bool IsSignedBy(const X509Certificate &parent) const;

	std::string SubjectName() const;
	std::string CommonName() const;
	std::string EmailAddress() const;
	std::vector<std::string> DNSNames() const;
	std::string IssuerName() const;

	std::string LookupIssuerItem(const std::string &item) const;
	std::string LookupSubjectItem(const std::string &item) const;
};

}

#endif