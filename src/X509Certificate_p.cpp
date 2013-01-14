// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/X509Certificate.h>
#include "X509Certificate_p.h"

#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pkcs12.h>

#include <ctime>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>
#include <assert.h>

#ifdef LIBMUMBLE_OS_ANDROID
# include "Compat_android.h"
#endif

namespace mumble {

X509CertificatePrivate::X509CertificatePrivate() {
}

void X509CertificatePrivate::ExtractCertificateInfo() {
	X509 *x509 = nullptr;
	const unsigned char *p = nullptr;

	p = reinterpret_cast<const unsigned char *>(cert_der_.Data());
	x509 = d2i_X509(nullptr, &p, cert_der_.Length());

	if (x509) {
		// Extract subject information
		X509_NAME *subject = X509_get_subject_name(x509);
		subject_items_ = X509CertificatePrivate::ParseX509Name(subject);

		// Extract issuer information
		X509_NAME *issuer = X509_get_issuer_name(x509);
		issuer_items_ = X509CertificatePrivate::ParseX509Name(issuer);

		// Extract notBefore and notAfter
		ASN1_TIME *notBefore = X509_get_notBefore(x509);
		if (notBefore) {
			not_before_ = X509CertificatePrivate::ParseASN1Time(notBefore);
		}
		ASN1_TIME *notAfter = X509_get_notAfter(x509);
		if (notAfter) {
			not_after_ = X509CertificatePrivate::ParseASN1Time(notAfter);
		}

		// Extract Subject Alt Names
		STACK_OF(GENERAL_NAME) *subjAltNames = static_cast<STACK_OF(GENERAL_NAME) *>(X509_get_ext_d2i(x509, NID_subject_alt_name, nullptr, nullptr));
		int num = sk_GENERAL_NAME_num(subjAltNames);
		for (int i = 0; i < num; i++) {
			GENERAL_NAME *name = sk_GENERAL_NAME_value(subjAltNames, i);
			switch (name->type) {
			case GEN_DNS: {
				std::string dns = X509CertificatePrivate::StringFromASN1String(name->d.ia5);
				if (!dns.empty()) {
					dns_names_.push_back(dns);
				}
				break;
			}
			case GEN_EMAIL: {
				std::string email = X509CertificatePrivate::StringFromASN1String(name->d.ia5);
				if (!email.empty()) {
					email_addrs_.push_back(email);
				}
				break;
			}
			// fixme(mkrautz): There's an URI alt name as well.
			default:
				break;
			}
		}

		sk_pop_free((_STACK *) subjAltNames, (void (*)(void *)) sk_free);
		X509_free(x509);
	}
}

ByteArray X509CertificatePrivate::Digest(const std::string &name) const {
	const EVP_MD *md = EVP_get_digestbyname(name.c_str());
	if (md == nullptr) {
		return ByteArray();
	}

	int size = EVP_MD_size(md);
	ByteArray ba(size);

	EVP_MD_CTX *ctx = EVP_MD_CTX_create();
	if (ctx == nullptr) {
		return ByteArray();
	}

	EVP_DigestInit_ex(ctx, md, nullptr);
	EVP_DigestUpdate(ctx, cert_der_.ConstData(), cert_der_.Length());
	EVP_DigestFinal_ex(ctx, reinterpret_cast<unsigned char *>(ba.Data()), nullptr);
	EVP_MD_CTX_destroy(ctx);

	return ba;
}

std::string X509CertificatePrivate::SubjectName() const {
	// If the subject has a CN, use that.
	auto cn = CommonName();
	if (!cn.empty()) {
		return cn;
	}
	// If not, check the DNS entries next. There's a
	// large chance that a server certificate will
	// have an email set alongside a DNS entry, but
	// there's only a very small chance that a client
	// certificate will have a DNS entry, so this is
	// (hopefully) a no-op for client certificates.
	if (dns_names_.size() > 0) {
		return dns_names_.front();
	}
	// OK, no DNS entries found. Try to use an email
	// address.
	if (email_addrs_.size() > 0) {
		return email_addrs_.front();
	}
	// We couldn't find anything sensible. Return
	// the empty string to signal that.
	return std::string();
}

std::string X509CertificatePrivate::CommonName() const {
	auto cn = subject_items_.find(std::string("CN"));
	if (cn != subject_items_.end()) {
		return (*cn).second;
	}
	return std::string();
}

std::string X509CertificatePrivate::EmailAddress() const {
	if (email_addrs_.size() > 0) {
		return email_addrs_.front();
	}
	return std::string();
}

std::vector<std::string> X509CertificatePrivate::DNSNames() const {
	return std::vector<std::string>(dns_names_.begin(), dns_names_.end());
}

std::string X509CertificatePrivate::IssuerName() const {
	auto cn = issuer_items_.find(std::string("CN"));
	if (cn != issuer_items_.end()) {
		return (*cn).second;
	}
	return std::string();
}

std::string X509CertificatePrivate::LookupIssuerItem(const std::string &item) const {
	auto found = issuer_items_.find(std::string(item));
	if (found != issuer_items_.end()) {
		return (*found).second;
	}
	return std::string();
}

std::string X509CertificatePrivate::LookupSubjectItem(const std::string &item) const {
	auto found = subject_items_.find(std::string(item));
	if (found != subject_items_.end()) {
		return (*found).second;
	}
	return std::string();
}

// AsOpenSSLX509 returns the certificate
// as an OpenSSL X509 pointer. It is the
// responsibility of the caller to ensure
// that the returned X509 pointer is freed
// by a call to X509_free().
X509 *X509CertificatePrivate::AsOpenSSLX509() const {
	const char *p = cert_der_.ConstData();
	return d2i_X509(nullptr, reinterpret_cast<const unsigned char **>(&p), cert_der_.Length());
}

std::time_t X509CertificatePrivate::NotBeforeTime() const {
	return not_before_;
}

std::time_t X509CertificatePrivate::NotAfterTime() const {
	return not_after_;
}

bool X509CertificatePrivate::IsSignedBy(const X509Certificate &parent) const {
	X509 *us = AsOpenSSLX509();
	X509 *it = parent.dptr_->AsOpenSSLX509();
	bool result = false;

	if (us != nullptr && it != nullptr)
		result = X509_verify(us, X509_get_pubkey(it)) == 1;

	if (us)
		X509_free(us);
	if (it)
		X509_free(it);

	return result;
}

static int add_ext(X509 *crt, int nid, const char *value) {
	X509_EXTENSION *ex;
	X509V3_CTX ctx;
	X509V3_set_ctx_nodb(&ctx);
	X509V3_set_ctx(&ctx, crt, crt, NULL, NULL, 0);
	ex = X509V3_EXT_conf_nid(NULL, &ctx, nid, const_cast<char *>(value));
	if (!ex)
		return 0;

	X509_add_ext(crt, ex, -1);
	X509_EXTENSION_free(ex);
	return 1;
}

// Generate self-signed certificate with the given name and email address as a X509Certificate
// object.
X509Certificate X509CertificatePrivate::GenerateSelfSignedCertificate(const std::string &name, const std::string &email) {
	CRYPTO_mem_ctrl(CRYPTO_MEM_CHECK_ON);

	X509 *x509 = X509_new();
	EVP_PKEY *keypair = EVP_PKEY_new();

	// Generate the public/private keypair.
	RSA *rsa = RSA_generate_key(2048, RSA_F4, nullptr, nullptr);
	EVP_PKEY_assign_RSA(keypair, rsa);

	// Setup general parameters: X509v3, unique serial number,
	// 1 year validity peroid.
	X509_set_version(x509, 2);
	ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);
	X509_gmtime_adj(X509_get_notBefore(x509), 0);
	X509_gmtime_adj(X509_get_notAfter(x509), 60*60*24*365*20);
	X509_set_pubkey(x509, keypair);

	X509_NAME *subject_name = X509_get_subject_name(x509);

	std::string common_name;
	if (name.empty()) {
		common_name = std::string("Mumble User");
	} else {
		common_name = name;
	}

	std::string alt_name_email;
	if (!email.empty()) {
		alt_name_email.append(std::string("email:"));
		alt_name_email.append(email);
	}

	X509_NAME_add_entry_by_txt(subject_name, "CN", MBSTRING_UTF8, reinterpret_cast<const unsigned char *>(common_name.c_str()), -1, -1, 0);
	X509_set_issuer_name(x509, subject_name);
	add_ext(x509, NID_basic_constraints, "critical,CA:FALSE");
	add_ext(x509, NID_ext_key_usage, "clientAuth");
	add_ext(x509, NID_subject_key_identifier, "hash");
	add_ext(x509, NID_netscape_comment, "Generated by Mumble");
	if (!alt_name_email.empty()) {
		add_ext(x509, NID_subject_alt_name, alt_name_email.c_str());
	}

	X509_sign(x509, keypair, EVP_sha1());

	int len = i2d_X509(x509, nullptr);
	ByteArray cert_der(len);
	char *ptr = cert_der.Data();
	i2d_X509(x509, reinterpret_cast<unsigned char **>(&ptr));
	X509_free(x509);

	len = i2d_PrivateKey(keypair, nullptr);
	ByteArray priv_der(len);
	ptr = priv_der.Data();
	i2d_PrivateKey(keypair, reinterpret_cast<unsigned char **>(&ptr));
	EVP_PKEY_free(keypair);

	return X509Certificate::FromRawDERData(cert_der, priv_der);
}

// Export a chain of certificates to a PKCS12-encoded data blob. In most cases, the leaf
// certificate is expected to contain a private key, but this is not required.
ByteArray X509CertificatePrivate::ExportCertificateChainAsPKCS12(std::vector<X509Certificate>chain, const std::string &password) {
	X509 *x509 = nullptr;
	EVP_PKEY *pkey = nullptr;
	PKCS12 *pkcs = nullptr;
	BIO *mem = nullptr;
	STACK_OF(X509) *certs = sk_X509_new_null();
	const unsigned char *p;
	ByteArray ba;

	if (chain.empty()) {
		return ByteArray();
	}

	X509Certificate leaf = chain.front();
	if (!leaf.HasCertificate())
		return ByteArray();

	p = reinterpret_cast<const unsigned char *>(leaf.dptr_->priv_der_.Data());
	if (p)
		pkey = d2i_AutoPrivateKey(nullptr, &p, leaf.dptr_->priv_der_.Length());
	x509 = leaf.dptr_->AsOpenSSLX509();

	if (x509 && (pkey ? X509_check_private_key(x509, pkey) : true)) {
		X509_keyid_set1(x509, nullptr, 0);
		X509_alias_set1(x509, nullptr, 0);
		for (int i = 1; i < chain.size(); i++) {
			X509Certificate cert = chain.at(i);
			if (!cert.HasCertificate()) {
				continue;
			}
			X509 *c = cert.dptr_->AsOpenSSLX509();
			sk_X509_push(certs, c);
		}
		char *password_cstr = nullptr;
		if (!password.empty()) {
			password_cstr = const_cast<char *>(password.c_str());
		}
		pkcs = PKCS12_create(password_cstr, const_cast<char *>("Mumble Identity"), pkey, x509, certs, 0, 0, 0, 0, 0);
		if (pkcs) {
			mem = BIO_new(BIO_s_mem());
			i2d_PKCS12_bio(mem, pkcs);
			(void) BIO_flush(mem);

			char *buf;
			int len = BIO_get_mem_data(mem, reinterpret_cast<unsigned char **>(&buf));
			ba = ByteArray(buf, len);
		}
	}

	if (pkey)
		EVP_PKEY_free(pkey);
	if (x509)
		X509_free(x509);
	if (pkcs)
		PKCS12_free(pkcs);
	if (mem)
		BIO_free(mem);
	if (certs)
		sk_X509_pop_free(certs, X509_free);

	return ba;
}

std::vector<X509Certificate> X509CertificatePrivate::FromPKCS12(const ByteArray &pkcs12, const std::string &password) {
	std::vector<X509Certificate> out_certs;
	X509 *x509 = nullptr;
	EVP_PKEY *pkey = nullptr;
	PKCS12 *pkcs = nullptr;
	BIO *mem = nullptr;
	STACK_OF(X509) *certs = nullptr;
	int ret = 0;

	if (pkcs12.IsNull()) {
		return std::vector<X509Certificate>();
	}

	void *buf = reinterpret_cast<void *>(const_cast<char *>(pkcs12.ConstData()));
	int len = pkcs12.Length();

	mem = BIO_new_mem_buf(buf, len);
	(void) BIO_set_close(mem, BIO_NOCLOSE);
	pkcs = d2i_PKCS12_bio(mem, nullptr);

	if (pkcs) {
		ret = PKCS12_parse(pkcs, password.c_str(), &pkey, &x509, &certs);
		// If we're using an empty password, try with nullptr as the
		// password argument. Some software might have botched this,
		// an we need to stay compatible.
		if (ret == 0 && password.empty()) {
			ret = PKCS12_parse(pkcs, nullptr, &pkey, &x509, &certs);
		}
		// We got a certs stack. This means that we're extracting a
		// PKCS12 blob that didn't include a private key.
		if (ret == 1 && certs != nullptr && x509 == nullptr && pkey == nullptr) {
			// Fallthrough to extraction of the chain stack.

		// PKCS12_parse filled out both our x509 and or pkey. This means
		// we're extracting a PKCS12 with a leaf certificate, a private key,
		// and possibly a certificate chain following the leaf.
		} else if (ret == 1 && x509 != nullptr && pkey != nullptr) {
			// Ensure that the leaf and private key match.
			if (X509_check_private_key(x509, pkey) > 0) {
				int len = i2d_PrivateKey(pkey, nullptr);
				ByteArray pkey_der = ByteArray(len);
				char *buf = pkey_der.Data();
				i2d_PrivateKey(pkey, reinterpret_cast<unsigned char **>(&buf));
				EVP_PKEY_free(pkey);

				len = i2d_X509(x509, nullptr);
				ByteArray leaf_der(len);
				buf = leaf_der.Data();
				i2d_X509(x509, reinterpret_cast<unsigned char **>(&buf));
				X509_free(x509);

				out_certs.push_back(X509Certificate::FromRawDERData(leaf_der, pkey_der));
			}
		} else {
			assert(ret == 0);
			assert(x509 == nullptr);
			assert(pkey == nullptr);
			assert(certs == nullptr);
		}
		// If we found a leaf, and there are extra certs, attempt to extract
		// those as well.
		if (certs != nullptr) {
			// If we have no leaf certificate, don't expect there to be any certificates in out_certs
			// at this point.
			// If we do have a leaf certificate, having no certificates in out_certs is an error (there
			// should be exactly one in there).
			if ((out_certs.size() == 0 && x509 == nullptr) || (out_certs.size() == 1 && x509 != nullptr)) {
				int ncerts = sk_X509_num(certs);
				for (int i = 0; i < ncerts; i++) {
					x509 = sk_X509_pop(certs);

					int len = i2d_X509(x509, nullptr);
					ByteArray leaf_der(len);
					char *buf = leaf_der.Data();
					i2d_X509(x509, reinterpret_cast<unsigned char **>(&buf));

					out_certs.push_back(X509Certificate::FromRawDERData(leaf_der, ByteArray()));
					X509_free(x509);
				}
			}
		}
	}

	if (certs)
		sk_X509_pop_free(certs, X509_free);
	if (pkcs)
		PKCS12_free(pkcs);
	if (mem)
		BIO_free(mem);

	return out_certs;
}

// StringFromDNAttributeType converts an ASN1_OBJECT representing a DN
// attribute type to a std::string. If the conversion in unsuccessful,
// the function returns the empty string.
//
// The function handles all attribute types that an implementation MUST
// handle according to RFC 4514.
std::string X509CertificatePrivate::StringFromDNAttributeType(ASN1_OBJECT *obj) {
	assert(obj != nullptr);

	// Call OBJ_obj2nid to get the object's numeric
	// object ID. This ensures the obeject exists
	// in OpenSSL's internal tables. For objects
	// not already in OpenSSL's tables, obj->nid
	// is set to NID_undef, so using the OBJ_obj2nid
	// accessor is important.
	int nid = OBJ_obj2nid(obj);
	if (nid != NID_undef) {
		switch (nid) {
		// CN	   commonName (2.5.4.3)
		case NID_commonName:                      return std::string(SN_commonName);
		// L	   localityName (2.5.4.7)
		case NID_localityName:                    return std::string(SN_localityName);
		// ST	   stateOrProvinceName (2.5.4.8)
		case NID_stateOrProvinceName:             return std::string(SN_stateOrProvinceName);
		// O	   organizationName (2.5.4.10)
		case NID_organizationName:                return std::string(SN_organizationName);
		// OU	   organizationalUnitName (2.5.4.11)
		case NID_organizationalUnitName:          return std::string(SN_organizationalUnitName);
		// C	   countryName (2.5.4.6)
		case NID_countryName:                     return std::string(SN_countryName);
		// STREET  streetAddress (2.5.4.9)
		case NID_streetAddress:                   return std::string(SN_streetAddress);
		// DC	   domainComponent (0.9.2342.19200300.100.1.25)
		case NID_domainComponent:                 return std::string(SN_domainComponent);
		// UID	   userId (0.9.2342.19200300.100.1.1)
		case NID_userId:                          return std::string(SN_userId);
		}
	}
	return std::string();
}

// StringFromASN1String converts an ASN1_STRING to a UTF-8 encoded std::string.
// If an error occurs, or the passed-in ASN1_STRING is empty, the empty string
// is returned.
std::string X509CertificatePrivate::StringFromASN1String(ASN1_STRING *str) {
	unsigned char *buf;
	int len;

	len = ASN1_STRING_to_UTF8(&buf, str);
	if (len < 0) {
		return std::string();
	}

	std::string out(reinterpret_cast<char *>(buf), static_cast<size_t>(len));
	OPENSSL_free(buf);

	return out;
}

// Parse an X509 Distinguished Name into map of name-value pairs.
std::map<std::string, std::string> X509CertificatePrivate::ParseX509Name(X509_NAME *name) {
	std::map<std::string, std::string> items;
	int n = X509_NAME_entry_count(name);
	for (int i = 0; i < n; i++) {
		X509_NAME_ENTRY *ne = X509_NAME_get_entry(name, i);
		if (ne == nullptr) {
			return std::map<std::string, std::string>();
		}
		ASN1_OBJECT *obj = X509_NAME_ENTRY_get_object(ne);
		if (obj == nullptr) {
			return std::map<std::string, std::string>();
		}
		ASN1_STRING *val = X509_NAME_ENTRY_get_data(ne);
		if (val == nullptr) {
			return std::map<std::string, std::string>();
		}
		std::string attributeName = X509CertificatePrivate::StringFromDNAttributeType(obj);
		if (attributeName.empty()) {
			return std::map<std::string, std::string>();
		}
		std::string attributeValue = X509CertificatePrivate::StringFromASN1String(val);
		if (attributeValue.empty()) {
			return std::map<std::string, std::string>();
		}
		items.insert(std::make_pair(attributeName, attributeValue));
	}

	return items;
}

// Parse an ASN1 string representing time from an X509 PKIX certificate.
std::time_t X509CertificatePrivate::ParseASN1Time(ASN1_TIME *time) {
	struct tm tm;
	char buf[15];

	// RFC 5280, 4.1.2.5.1 UTCTime
	// For the purposes of this profile, UTCTime values MUST be expressed in
	// Greenwich Mean Time (Zulu) and MUST include seconds (i.e., times are
	// YYMMDDHHMMSSZ), even where the number of seconds is zero.  Conforming
	// systems MUST interpret the year field (YY) as follows:
	//
	// Where YY is greater than or equal to 50, the year SHALL be
	// interpreted as 19YY; and
	//
	// Where YY is less than 50, the year SHALL be interpreted as 20YY.
	if (time->type == V_ASN1_UTCTIME && time->length == 13 && time->data[12] == 'Z') {
		memcpy(buf+2, time->data, time->length-1);
		if (time->data[0] >= '5') {
			buf[0] = '1';
			buf[1] = '9';
		} else {
			buf[0] = '2';
			buf[1] = '0';
		}
	// RFC 5280, 4.1.2.5.2. GeneralizedTime
	//
	// GeneralizedTime values MUST be expressed in Greenwich Mean Time (Zulu)
	// and MUST include seconds (i.e., times are YYYYMMDDHHMMSSZ), even where
	// the number of seconds is zero.  GeneralizedTime values MUST NOT include
	// fractional seconds.
	} else if (time->type == V_ASN1_GENERALIZEDTIME && time->length == 15 && time->data[14] == 'Z') {
		memcpy(buf, time->data, time->length-1);
	} else {
		std::cerr << "X509CertificatePrivate: Invalid ASN.1 date for PKIX purposes encountered." << std::endl;
		return 0;
	}

	// We used to append +0000 to the string at this point, but we dropped it.
	// The reason is that the strptime function doesn't support the %z modifier
	// on all systems, and on the systems that do support it, using it results
	// in the returned struct tm being translated to the local system's timezone.
	//
	// Since a struct tm does not include time zone information, we can simply
	// treat the result of parsing a timezone-less date string with strptime as
	// being UTC, and use timegm to convert it to a Unix epoch timestamp.
	buf[14] = 0;

	memset(&tm, 0, sizeof(tm));
	if (strptime(buf, "%Y%m%d%H%M%S", &tm) == nullptr) {
		std::cerr << "X509CertificatePrivate: Invalid ASN.1 date for PKIX purposes encountered." << std::endl;
		return 0;
	}

	return timegm(&tm);
}

}
