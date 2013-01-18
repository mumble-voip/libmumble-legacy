// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/X509Verifier.h>
#include "X509Verifier_win.h"
#include "X509HostnameVerifier.h"
#include <mumble/X509Certificate.h>
#include "X509Certificate_p.h"

#include <vector>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>

#include "uv.h"

namespace mumble {

static uv_once_t    system_verifier_once_ = UV_ONCE_INIT;
static X509Verifier *system_verifier_ptr_;

void X509VerifierPrivate::InitializeSystemVerifier() {
	system_verifier_ptr_ = new X509Verifier;
}

X509Verifier &X509Verifier::SystemVerifier() {
	uv_once(&system_verifier_once_, X509VerifierPrivate::InitializeSystemVerifier);
	return *system_verifier_ptr_;
}

X509Verifier::X509Verifier() : dptr_(new X509VerifierPrivate) {
}

X509Verifier::~X509Verifier() {
}

bool X509Verifier::VerifyChain(std::vector<X509Certificate> chain, const mumble::X509VerifierOptions &opts) {
	return dptr_->VerifyChain(chain, opts);
}

X509VerifierPrivate::X509VerifierPrivate() {
}

// AddCertToStore creates a CERT_CONTEXT of the certificate passed in as cert and adds it to the certificate store represented
// by store. If ctx_out points to a non-nullptr CERT_CONTEXT, ctx_out will be updated to point to a copy of the CERT_CONTEXT that
// is added to store.
bool X509VerifierPrivate::AddCertToStore(HCERTSTORE store, const X509Certificate &cert, const CERT_CONTEXT **ctx_out) const {
	const ByteArray &buf = cert.dptr_->cert_der_;
	const CERT_CONTEXT *ctx = CertCreateCertificateContext(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, reinterpret_cast<const BYTE*>(buf.ConstData()), buf.Length());
	if (ctx == nullptr) {
		return false;
	}

	bool ok = CertAddCertificateContextToStore(store, ctx, CERT_STORE_ADD_ALWAYS, ctx_out);
	if (!ok) {
		CertFreeCertificateContext(ctx);
		return false;
	}

	CertFreeCertificateContext(ctx);
	return true;
}

// CreateCertContextFromChain creates a CERT_CONTEXT representing the leaf certificate (index 0 of the chain)
// in an in-memory certificate store which will also contains any intermediate certificates found in the chain.
//
// While the method returns a CERT_CONTEXT that represents the leaf certificate, this CERT_CONTEXT contains a pointer
// to the in-memory store that holds the whole certificate chain. This store can be accessed via the hCertStore field
// of the returned CERT_CONTEXT. The store is automatically closed when the CERT_CONTEXT that this function returns
// is freed using CertFreeCertificateContext (it holds the only reference to the store).
const CERT_CONTEXT *X509VerifierPrivate::CreateCertContextFromChain(const std::vector<X509Certificate> &chain) const {
	const CERT_CONTEXT *store_ctx = nullptr;

	HCERTSTORE store = CertOpenStore(CERT_STORE_PROV_MEMORY, 0, 0, CERT_STORE_DEFER_CLOSE_UNTIL_LAST_FREE_FLAG, nullptr);
	if (store == nullptr) {
		goto err;
	}

	if (!this->AddCertToStore(store, chain.at(0), &store_ctx)) {
		goto err;
	}

	for (int i = 1; i < chain.size(); i++) {
		if (!this->AddCertToStore(store, chain.at(i))) {
			goto err;
		}
	}

	return store_ctx;

err:
	if (store != nullptr) {
		CertCloseStore(store, 0);
	}
	if (store_ctx != nullptr) {
		CertFreeCertificateContext(store_ctx);
	}
	return nullptr;
}

// FileTimeForStdTimeT converts a std::time_t to a FILETIME.
FILETIME X509VerifierPrivate::FileTimeFromStdTimeT(std::time_t time) const {
	FILETIME ft;
	ULARGE_INTEGER lint;

	// 1 Jan 1601 00:00:00 UTC is -11644473600 in Unix epoch representation.
	const ULONGLONG epoch_1601 = 11644473600ULL;
	// Factor that converts from 1 sec (Unix epoch) increments to 100 nsec (FILETIME) increments.
	const ULONGLONG secs_to_100ns =  10000000ULL;

	lint.QuadPart = (epoch_1601 + time) * secs_to_100ns;
	ft.dwLowDateTime = lint.LowPart;
	ft.dwHighDateTime = lint.HighPart;

	return ft;
}

// UTF16StringFromStdString converts a UTF-8 encoded std::string to a UTF-16 WCHAR *.
// The returned pointer must be freed with free().
WCHAR *X509VerifierPrivate::UTF16StringFromStdString(const std::string &str) const {
	WCHAR *buf;
	int sz = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), -1, nullptr, 0);
	if (sz == 0) {
		return nullptr;
	}

	buf = reinterpret_cast<WCHAR *>(malloc(sz*sizeof(WCHAR)));
	sz = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), -1, buf, sz);
	if (sz == 0) {
		return nullptr;
	}

	return buf;
}

// VerifyChain verifies the certificate chain in chain
// according to the verification options given as opts.
bool X509VerifierPrivate::VerifyChain(std::vector<X509Certificate> chain, const X509VerifierOptions &opts) {
	bool status = false;
	FILETIME verify_time;
	WCHAR *wide_dns_name = nullptr;

	if (chain.empty()) {
		goto out;
	}

	const CERT_CONTEXT *store_ctx = this->CreateCertContextFromChain(chain);
	if (store_ctx == nullptr) {
		goto out;
	}

	CERT_CHAIN_PARA para;
	memset(&para, 0, sizeof(para));
	para.cbSize = sizeof(CERT_CHAIN_PARA);

	LPSTR ssl_oids[] = {
		szOID_PKIX_KP_SERVER_AUTH,
		szOID_SERVER_GATED_CRYPTO,
		szOID_SGC_NETSCAPE,
	};

	if (!opts.dns_name.empty()) {
		para.RequestedUsage.dwType = USAGE_MATCH_TYPE_OR;
		para.RequestedUsage.Usage.cUsageIdentifier = sizeof(ssl_oids)/sizeof(ssl_oids[0]);
		para.RequestedUsage.Usage.rgpszUsageIdentifier = ssl_oids;
	} else {
		para.RequestedUsage.dwType = USAGE_MATCH_TYPE_AND;
		para.RequestedUsage.Usage.cUsageIdentifier = 0;
		para.RequestedUsage.Usage.rgpszUsageIdentifier = nullptr;
	}

	const CERT_CHAIN_CONTEXT *chain_ctx = nullptr;
	verify_time = this->FileTimeFromStdTimeT(opts.time);
	bool ok = CertGetCertificateChain(nullptr, store_ctx, (opts.time != 0) ? &verify_time : nullptr, store_ctx->hCertStore, &para, 0, nullptr, &chain_ctx);
	if (!ok) {
		goto out;
	}

	if (chain_ctx->TrustStatus.dwErrorStatus != CERT_TRUST_NO_ERROR) {
		goto out;
	}

	if (!opts.dns_name.empty()) {
		wide_dns_name = this->UTF16StringFromStdString(opts.dns_name);

		SSL_EXTRA_CERT_CHAIN_POLICY_PARA sslpara;
		memset(&sslpara, 0, sizeof(sslpara));
		sslpara.cbSize = sizeof(sslpara);
		sslpara.dwAuthType = AUTHTYPE_SERVER;
		sslpara.pwszServerName = wide_dns_name;

		CERT_CHAIN_POLICY_PARA chainpara;
		memset(&chainpara, 0, sizeof(chainpara));
		chainpara.pvExtraPolicyPara = &sslpara;
		chainpara.dwFlags = 0;
		chainpara.cbSize = sizeof(chainpara);

		CERT_CHAIN_POLICY_STATUS chain_status;
		memset(&chain_status, 0, sizeof(chain_status));
		ok = CertVerifyCertificateChainPolicy(CERT_CHAIN_POLICY_SSL, chain_ctx, &chainpara, &chain_status);
		if (!ok) {
			goto out;
		}
		if (chain_status.dwError == 0) {
			status = true;
		}
	} else {
		status = true;
	}

out:
	if (store_ctx != nullptr) {
		CertFreeCertificateContext(store_ctx);
	}
	if (chain_ctx != nullptr) {
		CertFreeCertificateChain(chain_ctx);
	}
	if (wide_dns_name != nullptr) {
		free(wide_dns_name);
	}
	return status;
}

}
