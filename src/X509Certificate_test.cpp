// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <gtest/gtest.h>
#include "mumble_test.h"

#include <mumble/ByteArray.h>
#include <mumble/X509Certificate.h>

#include <cstdlib>
#include <cstring>
#include <vector>

TEST(X509CertificateTest, TestLoadDERNoPrivateKey) {
	mumble::ByteArray der = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-leaf.crt"));
	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(der);
	EXPECT_TRUE(cert.HasCertificate());
	EXPECT_FALSE(cert.HasPrivateKey());
}

TEST(X509CertificateTest, TestLoadDERWithPrivateKey) {
	mumble::ByteArray cert_der = LoadFile(std::string("testdata/x509/selfsign/self.crt"));
	mumble::ByteArray priv_der = LoadFile(std::string("testdata/x509/selfsign/self.key"));
	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(cert_der, priv_der);
	EXPECT_TRUE(cert.HasCertificate());
	EXPECT_TRUE(cert.HasPrivateKey());
}

TEST(X509CertificateTest, TestSelfSignedGeneration) {
	std::string name("John Doe");
	std::string email("john@example.com");
	mumble::X509Certificate cert = mumble::X509Certificate::GenerateSelfSignedCertificate(name, email);
	EXPECT_EQ(name, cert.CommonName());
	EXPECT_EQ(email, cert.EmailAddress());
	EXPECT_TRUE(cert.IsSignedBy(cert));
}

TEST(X509CertificateTest, TestSelfSignedDefaultArgs) {
	mumble::X509Certificate cert = mumble::X509Certificate::GenerateSelfSignedCertificate();
	EXPECT_EQ(std::string("Mumble User"), cert.CommonName());
	EXPECT_EQ(std::string(), cert.EmailAddress());
	EXPECT_TRUE(cert.IsSignedBy(cert));
}

TEST(X509CertificateTest, TestLoadPKCS12NoPassordNoPrivateKey) {
	mumble::ByteArray pk12 = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-chain.p12"));
	ASSERT_FALSE(pk12.IsNull());

	std::vector<mumble::X509Certificate> chain = mumble::X509Certificate::FromPKCS12(pk12, std::string());
	ASSERT_EQ(3, chain.size());

	EXPECT_EQ(std::string("*.google.dk"), chain.at(0).CommonName());
	EXPECT_EQ(std::string("Google Internet Authority"), chain.at(1).CommonName());
	EXPECT_EQ(std::string("Equifax Secure Certificate Authority"), chain.at(2).LookupIssuerItem(std::string("OU")));
}

TEST(X509CertificateTest, TestLoadPKCS12WithPassordNoPrivateKey) {
	mumble::ByteArray pk12 = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-chain-password.p12"));
	ASSERT_FALSE(pk12.IsNull());

	std::vector<mumble::X509Certificate> chain = mumble::X509Certificate::FromPKCS12(pk12, std::string("password"));
	ASSERT_EQ(3, chain.size());

	EXPECT_EQ(std::string("*.google.dk"), chain.at(0).CommonName());
	EXPECT_EQ(std::string("Google Internet Authority"), chain.at(1).CommonName());
	EXPECT_EQ(std::string("Equifax Secure Certificate Authority"), chain.at(2).LookupIssuerItem(std::string("OU")));
}

TEST(X509CertificateTest, TestExportPKCS12NoPassword) {
	std::string name("John Doe");
	std::string email("john@example.com");

	mumble::X509Certificate leaf = mumble::X509Certificate::GenerateSelfSignedCertificate(name, email);
	std::vector<mumble::X509Certificate> export_chain;
	export_chain.push_back(leaf);

	mumble::ByteArray pkcs12 = mumble::X509Certificate::ExportCertificateChainAsPKCS12(export_chain, std::string());
	ASSERT_TRUE(pkcs12.Length() > 0);

	std::vector<mumble::X509Certificate> import_chain = mumble::X509Certificate::FromPKCS12(pkcs12, std::string());
	ASSERT_EQ(1, import_chain.size());

	EXPECT_EQ(name, import_chain.at(0).CommonName());
	EXPECT_EQ(email, import_chain.at(0).EmailAddress());
	EXPECT_TRUE(import_chain.at(0).IsSignedBy(import_chain.at(0)));
}

TEST(X509CertificateTest, TestExportPKCS12WithPassword) {
	std::string name("John Doe");
	std::string email("john@example.com");
	std::string password("SomeObscureSecret");

	mumble::X509Certificate leaf = mumble::X509Certificate::GenerateSelfSignedCertificate(name, email);
	std::vector<mumble::X509Certificate> export_chain;
	export_chain.push_back(leaf);

	mumble::ByteArray pkcs12 = mumble::X509Certificate::ExportCertificateChainAsPKCS12(export_chain, password);
	ASSERT_TRUE(pkcs12.Length() > 0);

	std::vector<mumble::X509Certificate> import_chain = mumble::X509Certificate::FromPKCS12(pkcs12, password);
	ASSERT_EQ(1, import_chain.size());

	EXPECT_EQ(name, import_chain.at(0).CommonName());
	EXPECT_EQ(email, import_chain.at(0).EmailAddress());
	EXPECT_TRUE(import_chain.at(0).IsSignedBy(import_chain.at(0)));
}

TEST(X509CertificateTest, TestExportPKCS12ChainNoPrivateKey) {
	mumble::ByteArray der = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-leaf.crt"));
	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(der);
	std::vector<mumble::X509Certificate> export_chain;
	export_chain.push_back(cert);

	mumble::ByteArray pkcs12 = mumble::X509Certificate::ExportCertificateChainAsPKCS12(export_chain, std::string());
	ASSERT_TRUE(pkcs12.Length() > 0);

	std::vector<mumble::X509Certificate> import_chain = mumble::X509Certificate::FromPKCS12(pkcs12, std::string());
	ASSERT_EQ(1, import_chain.size());

	EXPECT_EQ(std::string("*.google.dk"), import_chain.at(0).CommonName());
}

TEST(X509CertificateTest, TestExportPKCS12GoogleDKChain) {
	mumble::ByteArray pk12 = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-chain.p12"));
	ASSERT_FALSE(pk12.IsNull());

	std::vector<mumble::X509Certificate> export_chain = mumble::X509Certificate::FromPKCS12(pk12, std::string());
	ASSERT_EQ(3, export_chain.size());

	mumble::ByteArray pkcs12 = mumble::X509Certificate::ExportCertificateChainAsPKCS12(export_chain, std::string());
	ASSERT_TRUE(pkcs12.Length() > 0);

	std::vector<mumble::X509Certificate> import_chain = mumble::X509Certificate::FromPKCS12(pkcs12, std::string());
	ASSERT_EQ(3, import_chain.size());

	EXPECT_EQ(std::string("*.google.dk"), import_chain.at(0).CommonName());
	EXPECT_EQ(std::string("Google Internet Authority"), import_chain.at(1).CommonName());
	EXPECT_EQ(std::string("Equifax Secure Certificate Authority"), import_chain.at(2).LookupIssuerItem(std::string("OU")));
}

TEST(X509CertificateTest, TestSHA1Digest) {
	mumble::ByteArray der = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-leaf.crt"));
	mumble::ByteArray expected_sha1 = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-leaf.sha1"));
	ASSERT_EQ(20, expected_sha1.Length());

	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(der);
	mumble::ByteArray sha1 = cert.SHA1Digest();
	ASSERT_EQ(20, expected_sha1.Length());

	EXPECT_EQ(expected_sha1, sha1);
}

TEST(X509CertificateTest, TestSHA256Digest) {
	mumble::ByteArray der = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-leaf.crt"));
	mumble::ByteArray expected_sha256 = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-leaf.sha256"));
	ASSERT_EQ(32, expected_sha256.Length());

	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(der);
	mumble::ByteArray sha256 = cert.SHA256Digest();
	ASSERT_EQ(32, sha256.Length());

	EXPECT_EQ(expected_sha256, sha256);
}

TEST(X509CertificateTest, TestValidityTimeParsing) {
	mumble::ByteArray der = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-leaf.crt"));
	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(der);
	mumble::ByteArray not_before_ba = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-leaf.notBefore"));
	mumble::ByteArray not_after_ba = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-leaf.notAfter"));

	std::string not_before_string(not_before_ba.Data(), not_before_ba.Length());
	std::string not_after_string(not_after_ba.Data(), not_after_ba.Length());

	std::time_t expected_not_after = strtoul(not_after_string.c_str(), nullptr, 10);
	std::time_t expected_not_before = strtoul(not_before_string.c_str(), nullptr, 10);

	std::time_t not_after = cert.NotAfterTime();
	std::time_t not_before = cert.NotBeforeTime();

	EXPECT_EQ(expected_not_after, not_after);
	EXPECT_EQ(expected_not_before, not_before);
}

TEST(X509CertificateTest, SignedByTestGoogleChain) {
	mumble::ByteArray pk12 = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-chain.p12"));
	ASSERT_FALSE(pk12.IsNull());

	std::vector<mumble::X509Certificate> chain = mumble::X509Certificate::FromPKCS12(pk12, std::string());
	ASSERT_EQ(3, chain.size());

	EXPECT_TRUE(chain.at(0).IsSignedBy(chain.at(1)));
	EXPECT_TRUE(chain.at(1).IsSignedBy(chain.at(2)));
	EXPECT_TRUE(chain.at(2).IsSignedBy(chain.at(2)));
	EXPECT_FALSE(chain.at(1).IsSignedBy(chain.at(0)));
	EXPECT_FALSE(chain.at(2).IsSignedBy(chain.at(0)));
}

TEST(X509CertificateTest, SignedBySelfSigned) {
	mumble::ByteArray cert_der = LoadFile(std::string("testdata/x509/selfsign/self.crt"));
	mumble::ByteArray priv_der = LoadFile(std::string("testdata/x509/selfsign/self.key"));
	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(cert_der, priv_der);

	EXPECT_TRUE(cert.IsSignedBy(cert));
}

TEST(X509CertificateTest, ValidAtTimeGoogleDK) {
	mumble::ByteArray der = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-leaf.crt"));

	// Not before: Dec 6 08:56:04 2012 UTC
	// Not after:  Jun 7 19:43:27 2013 UTC
	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(der);

	// Not before
	EXPECT_FALSE(cert.IsValidAtTime(1354784163));  // Dec 6 08:56:03 2012 UTC
	EXPECT_TRUE(cert.IsValidAtTime(1354784164));   // Dec 6 08:56:04 2012 UTC
	EXPECT_TRUE(cert.IsValidAtTime(1354784165));   // Dec 6 08:56:05 2012 UTC

	// In between
	EXPECT_TRUE(cert.IsValidAtTime(1355314332));   // Dec 12 12:12:12 2012 UTC
	EXPECT_TRUE(cert.IsValidAtTime(1356827520));   // Dec 30 00:32:00 2012 UTC

	// Not after
	EXPECT_TRUE(cert.IsValidAtTime(1370634206));   // Jun 7 19:43:26 2013 UTC
	EXPECT_TRUE(cert.IsValidAtTime(1370634207));   // Jun 7 19:43:27 2013 UTC
	EXPECT_FALSE(cert.IsValidAtTime(1370634208));  // Jun 7 19:43:28 2013 UTC
}

TEST(X509CertificateTest, TestDNSAltNames) {
	mumble::ByteArray der = LoadFile(std::string("testdata/x509/altnames/dns.crt"));
	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(der);

	std::vector<std::string> dns_names = cert.DNSNames();
	ASSERT_EQ(3, dns_names.size());
	EXPECT_EQ(std::string("www1.example.com"), dns_names.at(0));
	EXPECT_EQ(std::string("www2.example.com"), dns_names.at(1));
	EXPECT_EQ(std::string("www3.example.com"), dns_names.at(2));
}

TEST(X509CertificateTest, TestEmailAltNames) {
	mumble::ByteArray der = LoadFile(std::string("testdata/x509/altnames/email.crt"));
	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(der);

	// Right now, we only extract a single email address.
	EXPECT_EQ(std::string("noreply@example.com"), cert.EmailAddress());
}

TEST(X509CertificateTest, TestSubjectName) {
	mumble::ByteArray der = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-leaf.crt"));
	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(der);

	EXPECT_EQ(std::string("*.google.dk"), cert.SubjectName());
}

TEST(X509CertificateTest, TestSubjectCommonName) {
	mumble::ByteArray der = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-leaf.crt"));
	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(der);

	EXPECT_EQ(std::string("*.google.dk"), cert.CommonName());
}

TEST(X509CertificateTest, TestSubjectNameFallback) {
	mumble::ByteArray email_der = LoadFile(std::string("testdata/x509/altnames/email.crt"));
	mumble::X509Certificate email_cert = mumble::X509Certificate::FromRawDERData(email_der);

	mumble::ByteArray dns_der = LoadFile(std::string("testdata/x509/altnames/dns.crt"));
	mumble::X509Certificate dns_cert = mumble::X509Certificate::FromRawDERData(dns_der);

	EXPECT_EQ(std::string("noreply@example.com"), email_cert.SubjectName());
	EXPECT_EQ(std::string("www1.example.com"), dns_cert.SubjectName());
}

TEST(X509CertificateTest, TestIssuerName) {
	mumble::ByteArray der = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-leaf.crt"));
	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(der);

	EXPECT_EQ(std::string("Google Internet Authority"), cert.IssuerName());
}

TEST(X509CertificateTest, SubjectAndIssuerItemLookup) {
	mumble::ByteArray der = LoadFile(std::string("testdata/x509/google.dk/*.google.dk-leaf.crt"));

	// As per openssl x509 -in \*.google.dk-leaf.crt -inform der -noout -text:
	// Issuer: C=US, O=Google Inc, CN=Google Internet Authority
	// Subject: C=US, ST=California, L=Mountain View, O=Google Inc, CN=*.google.dk
	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(der);

	EXPECT_EQ(std::string("US"), cert.LookupIssuerItem(std::string("C")));
	EXPECT_EQ(std::string("Google Inc"), cert.LookupIssuerItem(std::string("O")));
	EXPECT_EQ(std::string("Google Internet Authority"), cert.LookupIssuerItem(std::string("CN")));
	EXPECT_EQ(std::string(), cert.LookupIssuerItem(std::string("OU"))); // bad item

	EXPECT_EQ(std::string("US"), cert.LookupSubjectItem(std::string("C")));
	EXPECT_EQ(std::string("California"), cert.LookupSubjectItem(std::string("ST")));
	EXPECT_EQ(std::string("Mountain View"), cert.LookupSubjectItem(std::string("L")));
	EXPECT_EQ(std::string("Google Inc"), cert.LookupSubjectItem(std::string("O")));
	EXPECT_EQ(std::string("*.google.dk"), cert.LookupSubjectItem(std::string("CN")));
	EXPECT_EQ(std::string(), cert.LookupSubjectItem(std::string("OU"))); // bad item
}

TEST(X509CertificateTest, TestNaiveDNParsing) {
	mumble::ByteArray der = LoadFile(std::string("testdata/x509/trickydn/tricky.crt"));
	mumble::X509Certificate cert = mumble::X509Certificate::FromRawDERData(der);

	std::string expected_cn("Name With A Comma, \"and another one, haha\"");
	EXPECT_EQ(expected_cn, cert.CommonName());
}