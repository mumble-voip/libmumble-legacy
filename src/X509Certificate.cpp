// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/X509Certificate.h>
#include "X509Certificate_p.h"
#include <mumble/ByteArray.h>

#include <string>
#include <vector>
#include <iostream>

namespace mumble {

X509Certificate X509Certificate::FromRawDERData(const ByteArray &cert, const ByteArray &pkey) {
	X509Certificate tmp;
	tmp.dptr_->cert_der_ = cert;
	tmp.dptr_->priv_der_ = pkey;
	tmp.dptr_->ExtractCertificateInfo();
	return tmp;
}

X509Certificate X509Certificate::GenerateSelfSignedCertificate(const std::string &name, const std::string &email) {
	return X509CertificatePrivate::GenerateSelfSignedCertificate(name, email);
}

std::vector<X509Certificate> X509Certificate::FromPKCS12(const ByteArray &pkcs12, const std::string &password) {
	return X509CertificatePrivate::FromPKCS12(pkcs12, password);
}

ByteArray X509Certificate::ExportCertificateChainAsPKCS12(std::vector<X509Certificate> chain, const std::string &password) {
	return X509CertificatePrivate::ExportCertificateChainAsPKCS12(chain, password);
}

X509Certificate::X509Certificate() : dptr_(new X509CertificatePrivate) {
}

X509Certificate::~X509Certificate() {
}

X509Certificate::X509Certificate(const X509Certificate &cert) : dptr_(new X509CertificatePrivate) {
	dptr_->cert_der_ = cert.dptr_->cert_der_;
	dptr_->priv_der_ = cert.dptr_->priv_der_;
	dptr_->ExtractCertificateInfo();
}

X509Certificate& X509Certificate::operator=(X509Certificate cert) {
	std::swap((*this).dptr_, cert.dptr_);
	return *this;
}

bool X509Certificate::HasCertificate() const {
	return !dptr_->cert_der_.IsNull();
}

bool X509Certificate::HasPrivateKey() const {
	return !dptr_->priv_der_.IsNull();
}

ByteArray X509Certificate::SHA1Digest() const {
	return dptr_->Digest(std::string("sha1"));
}

ByteArray X509Certificate::SHA256Digest() const {
	return dptr_->Digest(std::string("sha256"));
}

std::time_t X509Certificate::NotBeforeTime() const {
	return dptr_->NotBeforeTime();
}

std::time_t X509Certificate::NotAfterTime() const {
	return dptr_->NotAfterTime();
}

bool X509Certificate::IsSignedBy(const X509Certificate &parent) const {
	return dptr_->IsSignedBy(parent);
}

bool X509Certificate::IsValidAtTime(std::time_t time) const {
	return time >= NotBeforeTime() && time <= NotAfterTime();
}

std::string X509Certificate::SubjectName() const {
	return dptr_->SubjectName();
}

std::string X509Certificate::CommonName() const {
	return dptr_->CommonName();
}

std::string X509Certificate::EmailAddress() const {
	return dptr_->EmailAddress();
}

std::vector<std::string> X509Certificate::DNSNames() const {
	return dptr_->DNSNames();
}

std::string X509Certificate::IssuerName() const {
	return dptr_->IssuerName();
}

std::string X509Certificate::LookupIssuerItem(const std::string &item) const{
	return dptr_->LookupIssuerItem(item);
}

std::string X509Certificate::LookupSubjectItem(const std::string &item) const {
	return dptr_->LookupSubjectItem(item);
}

}
