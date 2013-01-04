// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/X509Verifier.h>
#include "X509Verifier_mac.h"
#include "X509HostnameVerifier.h"
#include <mumble/X509Certificate.h>
#include "X509Certificate_p.h"

#include <iostream>
#include <vector>
#include <string>

#include "uv.h"

#include <Security/Security.h>
#import <Foundation/Foundation.h>

namespace mumble {

static uv_once_t	 system_verifier_once_ = UV_ONCE_INIT;
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
	@autoreleasepool {
		return dptr_->VerifyChain(chain, opts);
	}
}

X509VerifierPrivate::X509VerifierPrivate() {
}

// VerifyChain verifies the certificate chain in chain
// according to the verification options given as opts.
bool X509VerifierPrivate::VerifyChain(std::vector<X509Certificate> chain, const X509VerifierOptions &opts) {
	NSMutableArray *sec_chain = [NSMutableArray arrayWithCapacity:chain.size()];
	SecTrustResultType trust_result = kSecTrustResultInvalid;
	SecPolicyRef policy = nullptr;
	SecTrustRef trust = nullptr;
	OSStatus err = noErr;
	bool status = false;

	// Check for the no-op condition. There's no knowing
	// how the Apple APIs treat an empty certificate chain.
	if (chain.empty()) {
		return false;
	}
	
	// If we have a DNS name set, verify the chain for
	// TLS server usage.
	if (!opts.dns_name.empty()) {
		NSString *hostname = [[NSString alloc] initWithBytesNoCopy:const_cast<char *>(opts.dns_name.data())
															length:opts.dns_name.length()
														  encoding:NSUTF8StringEncoding
													  freeWhenDone:NO];
		if (hostname == nullptr) {
			return false;
		}
		policy = SecPolicyCreateSSL(true, reinterpret_cast<CFStringRef>(hostname));
		[hostname release];
	} else {
		policy = SecPolicyCreateBasicX509();
	}
	// Abort early if policy creation fails. The functions are
	// not documented as being able to return nullptr, but if
	// they do, we'll probably crash in SecTrustEvaluate.
	if (policy == nullptr) {
		return false;
	}
	[reinterpret_cast<id>(policy) autorelease];

	// Convert our X509Certificate objects in SecCertificateRefs
	// that are usable by the SecTrustRef APIs.
	for (auto cert : chain) {
		const ByteArray &ba = cert.dptr_->cert_der_;
		NSData *cert_der = [NSData dataWithBytesNoCopy:const_cast<char *>(ba.ConstData())
												length:static_cast<NSUInteger>(ba.Length())
										  freeWhenDone:NO];
		if (cert_der == nullptr) {
			return false;
		}
		SecCertificateRef sec_cert = SecCertificateCreateWithData(kCFAllocatorDefault, reinterpret_cast<CFDataRef>(cert_der));
		if (sec_cert == nullptr) {
			// Unable to read one of the certificates
			// in the chain. Abort.
			return false;
		}
		[sec_chain addObject:reinterpret_cast<id>(sec_cert)];
		CFRelease(sec_cert);
	}

	err = SecTrustCreateWithCertificates(reinterpret_cast<CFArrayRef>(sec_chain), policy, &trust);
	if (err != noErr || trust == nullptr) {
		return false;
	}
	[reinterpret_cast<id>(trust) autorelease];

	// If a time was specified in opts, use that for the verification.
	// If not time is set in the SecTrustRef, it will automatically
	// use the current sytem time.
	if (opts.time > 0) {
		NSTimeInterval ti = static_cast<NSTimeInterval>(opts.time);
		err = SecTrustSetVerifyDate(trust, reinterpret_cast<CFDateRef>([NSDate dateWithTimeIntervalSince1970:ti]));
		if (err != noErr) {
			return false;
		}
	}

	err = SecTrustEvaluate(trust, &trust_result);
	if (err != noErr) {
		return false;
	}

	switch (trust_result) {
		case kSecTrustResultProceed:	 // User trusts it.
		case kSecTrustResultUnspecified: // System trusts it.
			return true;
	}

	return false;
}

}
