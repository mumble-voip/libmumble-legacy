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

/// X509Certificate represents an X.509 certificate for use in the Transport Layer Security protocol.
class X509Certificate {
public:
	/// FromRawDERData constructs an X509Certificate using the certificate and public key
	/// given as *cert* and the private key given as *pkey*.
	///
	/// @param   cert   ByteArray containing a DER-encoded X.509 certificate.
	/// @param   pkey   ByteArray containing *cert*'s private key. May be nullptr.
	///
	/// @return  Returns an X509Certificate holding the certificate and public key of *cert*,
	///          as well as the private key of *pkey*, if given.
	static X509Certificate FromRawDERData(const ByteArray &cert, const ByteArray &pkey = ByteArray());

	/// GenerateSelfSignedCertificate creates a self-signed X.509 client certificate.
	/// The generated certificate uses RSA keys of 2048 bits. The certificate will be
	/// generated with a common name of *name*, with *email* being represented by a
	/// subject alternative email.
	///
	/// @param   name   The Common Name of the generated client certificate.
	/// @param   email  Email address to embed in the certificate (as a subject alternative name).
	///
	/// @return  Returns a self-signed certificate containing the certificate
	///          created by using the template arguments *name* and *email*, along
	///          with a unique 2048 bit RSA key pair (public and private key).
	static X509Certificate GenerateSelfSignedCertificate(const std::string &name = std::string(), const std::string &email = std::string());

	/// FromPKCS12 parses the *pkcs12* ByteArray containing a PKCS #12 file. The PKCS #12
	/// file will be attempted to be decrypted using *password*. If unsuccessful, FromPKCS12
	/// returns an empty vector.
	///
	/// @param   pkcs12     A ByteArray containing the contents of a PKCS# 12 file.
	/// @param   password   The password that FromPKCS12 shall use to decrypt *pkcs12*.
	///
	/// @return  Retruns a std::vector of X509Certifcates representing the content of
	///          the PKCS #12 file if successful. If unsuccessful, FromPKCS12 returns
	///          an empty vector.
	static std::vector<X509Certificate> FromPKCS12(const ByteArray &pkcs12, const std::string &password);

	/// ExportCertificateChainAsPKCS12 exports a certificate chain as a PKCS #12 file
	/// output in a ByteArray. The returned ByteArray will be encrypted using the given
	/// *password*.
	///
	/// @param   chain     The certificate chain to export.
	/// @param   password  The password to encrypt the exported PKCS #12 file with.
	///
	/// @return  Returns a ByteArray containing a PKCS #12 file containing *chain*.
	///          If the operation fails, a null ByteArray may be returned.
	static ByteArray ExportCertificateChainAsPKCS12(std::vector<X509Certificate> chain, const std::string &password);

	/// Constructs an X509Certificate containing no *certificate* or *private key*.
	X509Certificate();

	/// Destroys the X509Certificate.
	~X509Certificate();

	/// Constructs a copy of *cert*.
	X509Certificate(const X509Certificate &cert);

	/// Assigns *cert* to this X509Certificate.
	X509Certificate& operator=(X509Certificate cert);

	/// HasCertificate checks whether the X509Certificate has a *certificate*
	/// and therefore also a *public key*.
	///
	/// @return   Returns true if the X509Certificate contains a certificate part.
	bool HasCertificate() const;

	/// HasPrivateKey checks whether the X509Certificate has a *private key*.
	///
	/// @return   Returns true if the X509Certificate contains a *private key* part.
	bool HasPrivateKey() const;

	/// SHA1Digest returns the SHA1 digest of the X509Certificate. The SHA1 digest
	/// is often shown to users as a fingerprint of the certificate. This
	/// calculation does not touch the private key in any way.
	///
	/// @return   Returns a ByteArray containing the result of applying the SHA1 hash
	///           to the raw DER representtion of the X509Certificate.
	ByteArray SHA1Digest() const ;

	/// SHA256Digest returns the SHA256 digest of the X509Certificate. The SHA256
	/// digest is often shown to users as a fingerprint of the certificate. This
	/// calculation does not touch the private key in any way.
	///
	/// @return   Returns a ByteArray containing the result of applying the SHA256
	///           hash to the raw DER representation of the X509Certificate.
	ByteArray SHA256Digest() const;

	/// NotBeforeTime returns the *Not Before* validity time of the X509Certificate
	/// in a std::time_t. The certificate is invalid if used before this date.
	///
	/// @return   Returns the *Not Before* validity time of the X509Certificate.
	std::time_t NotBeforeTime() const;

	/// NotAfterTime returns the *Not After* validity time of the X509Certificate
	/// in a std::time_t. The certificate is invalid if used after this date.
	///
	/// @return   Returns the *Not AFter* validity time of the X509Certificate.
	std::time_t NotAfterTime() const;

	/// IsSignedBy checks whether the X509Certificate is signed by *parent*.
	///
	/// @param    parent   The certificate that the signature of this
	///                    X509Certificate shall be matched against.
	///
	/// @return   Returns true if this certificate is signed by *parent.
	bool IsSignedBy(const X509Certificate &parent) const;

	/// IsValidAtTime checks whether this X509Certificate is valid at *time*.
	/// This method uses the *Not Before* and *Not After* validity times of
	/// the X509Certificate. These are also available via the *NotBeforeTime()*
	/// and *NotAfterTime()* methods of the X509Certificate.
	///
	/// @param   time   The absolute time to check against.
	///
	/// @return  Returns true if the X509Certificate is valid for use at *time*.
	bool IsValidAtTime(std::time_t time) const;

	/// SubjectName returns a string that can be used for identifying the subject
	/// of the X509Certificate. It attempts to use the subject's common name,
	/// first found email address or first found DNS name in that order depending
	/// on availabilty.
	///
	/// @return  Returns a human readable description of the X509Certificate's subject.
	///          May be empty if no viable subject name could be found.
	std::string SubjectName() const;

	/// CommonName returns the common name of the X509Certificate's subject.
	///
	/// @return  Returns the common name of the X509Certificate's subject.
	///          May return an empty std::string if no such name is available.
	std::string CommonName() const;

	/// EmailAddress returns the first email address of the X509Certificate's
	/// subject alternative names.
	///
	/// @return   Returns the first email address encountered in the
	///           X509Certificate's subject alternative names. May be
	///           empty if no such email addresses could be found.
	std::string EmailAddress() const;

	/// DNSNames returns all DNS names found in the X509Certificate's subject
	/// alternative names.
	///
	/// @return   A std::vector containing all DNS names in the X509Certificate's
	///           subject alternative names. May be empty if no DNS names are present.
	std::vector<std::string> DNSNames() const;

	/// IssuerName returns the common name of the X509Certificate's issuer.
	///
	/// @return  Common name of this X509Certificate's issuer. May be empty
	///          if no common name was found for the issuer.
	std::string IssuerName() const;

	/// LookupIssuerItem looks up an item of the X509Certificate's
	/// issuer Distinguished Name. For example, to look up the country
	/// or organization of the issuer, one could use:
	///
	///    LookupIssuerItem("C") // Country
	///    LookupIssuerItem("O") // Organization
	///
	///
	/// @param   item   The Distinguished Name attribute type to look up.
	///                 See RFC 4514 for more information on these attributes.
	///
	/// @return  Returns the value for the given attribute type. Returns an
	///          empty string if the value is not present for the issuer name.
	std::string LookupIssuerItem(const std::string &item) const;

	/// LookupSubjectItem looks up an item of the X509Certificate's
	/// subject Distinguished Name. For example, to look up the
	/// common name of the subject, one could use:
	///
	///    LookupIssuerItem("CN") // Common Name
	///
	/// @param   item   The Distinguished Name attribute type to look up.
	///                 See RFC 4514 for more informationo on these attributes.
	///
	/// @return  Returns the value for the given attribute type. Returns an
	///          empty string if the value is not present for the subject name.
	std::string LookupSubjectItem(const std::string &item) const;

private:
	friend class X509PEMVerifier;
	friend class X509VerifierPrivate;
	friend class X509CertificatePrivate;
	std::unique_ptr<X509CertificatePrivate> dptr_;
};

}

#endif
