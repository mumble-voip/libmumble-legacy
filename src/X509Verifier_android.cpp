// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/X509Verifier.h>
#include "X509Verifier_android.h"
#include "X509PEMVerifier.h"

#include <vector>
#include <string>
#include <set>
#include <fstream>
#include <algorithm>

#include <dirent.h>

#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include "uv.h"

namespace mumble {

static uv_once_t     system_verifier_once_ = UV_ONCE_INIT;
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

bool X509Verifier::VerifyChain(std::vector<X509Certificate> chain, const X509VerifierOptions &opts) {
	return dptr_->pem_verifier_.VerifyChain(chain, opts);
}

X509VerifierPrivate::X509VerifierPrivate() {
	ReadSystemRoots();
}

// read_caroot_dir returns set set of root certificate hash-names for the caroot at path.
std::set<std::string> read_caroot_dir(const std::string &path) {
	std::set<std::string> roots;
	DIR *dir = opendir(path.c_str());
	if (dir != nullptr)  {
		struct dirent *dent = nullptr;
		while (dent = readdir(dir)) {
			if (dent->d_name[0] == '.')
				continue;
			std::string name(const_cast<const char *>(&dent->d_name[0]));
			roots.insert(name);
		}
		closedir(dir);
	}
	return roots;
}

// read_caroot_pem reads a CA certificate named fn from dir.
ByteArray read_caroot_pem(const std::string &dir, const std::string &fn) {
	mumble::ByteArray ba;
	std::ifstream ifs;
	ifs.open(dir + std::string("/") + fn, std::ios::binary);
	while (ifs.good()) {
		mumble::ByteArray chunk(256);
		char *buf = chunk.Data();
		ifs.read(buf, chunk.Length());
		chunk.Truncate(ifs.gcount());
		if (chunk.Length() > 0) {
			ba.Append(chunk);
		}
	}
	ifs.close();
	return ba;
}

// ReadSystemRoots reads system CA store on Android 4.0 and greater.
// It's not public API, so it's prone to fail in the future, but
// it's better than the alternative (JNI) at the moment.
void X509VerifierPrivate::ReadSystemRoots() {
	const std::string cacerts_path(std::string("/system/etc/security/cacerts"));
	const std::string cacerts_removed_path(std::string("/data/misc/keychain/cacerts-removed"));
	const std::string cacerts_added_path(std::string("/data/misc/keychain/cacerts-added"));

	std::set<std::string> cacerts = read_caroot_dir(cacerts_path);
	std::set<std::string> cacerts_removed = read_caroot_dir(cacerts_removed_path);
	std::set<std::string> cacerts_added = read_caroot_dir(cacerts_added_path);

	std::set<std::string> cacerts_filtered;
	std::set_difference(cacerts.begin(), cacerts.end(),
	                    cacerts_removed.begin(), cacerts_removed.end(),
	                    std::inserter(cacerts_filtered, cacerts_filtered.end()));

	for (const std::string &fn : cacerts_filtered) {
		ByteArray pem_bytes = read_caroot_pem(cacerts_path, fn);
		pem_verifier_.AddPEM(pem_bytes);
	}

	for (const std::string &fn : cacerts_added) {
		ByteArray pem_bytes = read_caroot_pem(cacerts_added_path, fn);
		pem_verifier_.AddPEM(pem_bytes);
	}
}

}