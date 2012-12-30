// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/Connection.h>
#include "Connection_p.h"
#include <mumble/X509Certificate.h>
#include "X509Certificate_p.h"
#include <mumble/X509Verifier.h>

#include "UVBio.h"

#include <string>
#include <iostream>
#include <assert.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/x509.h>

namespace mumble {

static uv_once_t sslinit = UV_ONCE_INIT;

ConnectionPrivate::ConnectionPrivate() {
	uv_once(&sslinit, ConnectionPrivate::InitializeSSL);
}

// fixme(mkrautz): There are also other consumers of OpenSSL in libmumble.
// Move this somewhere else.
void ConnectionPrivate::InitializeSSL() {
	// SSL_library_init() always returns ``1'', so it is safe to discard the return value.
	SSL_library_init();
	// OpenSSL 0.9.8o and 1.0.0a and later added SHA2 algorithms to SSL_library_init().
	// Applications which need to use SHA2 in earlier versions of OpenSSL should call
	// OpenSSL_add_all_algorithms() as well.
	OpenSSL_add_all_algorithms();

	ERR_load_crypto_strings();
}

int ConnectionPrivate::Connect(std::string hostname) {
	int err;

	loop_ = uv_loop_new();

	err = uv_thread_create(&thread_, ConnectionPrivate::ConnectionThread, this);
	if (err != UV_OK) {
		std::cerr << "uv_thread_create: " << err << std::endl;
		return -1;
	}

	struct sockaddr_in addr = uv_ip4_addr("173.194.41.168", 443);

	err = uv_tcp_init(loop_, &tcpsock_);
	if (err != UV_OK) {
		std::cerr << "uv_tcp_init: " << uv_strerror(uv_last_error(loop_)) << std::endl;
		return -1;
	}

	tcpconn_.data = static_cast<void *>(this);
	tcpsock_.data = static_cast<void *>(this);
	err = uv_tcp_connect(&tcpconn_, &tcpsock_, addr, ConnectionPrivate::OnConnect);
	if (err != UV_OK) {
		std::cerr << "uv_tcp_connect failed: " << err << std::endl;
		return -1;
	}

	state_ = CONNECTION_STATE_PRE_CONNECT;

	return 0;
}

void ConnectionPrivate::ConnectionThread(void *udata) {
	ConnectionPrivate *cp = static_cast<ConnectionPrivate *>(udata);
	uv_run(cp->loop_);
}

static const char *SSLErrorLookup(int sslErr) {
	switch (sslErr) {
	case SSL_ERROR_NONE:             return "SSL_ERROR_NONE";
	case SSL_ERROR_ZERO_RETURN:      return "SSL_ERROR_ZERO_RETURN";
	case SSL_ERROR_WANT_READ:        return "SSL_ERRROR_WANT_READ";
	case SSL_ERROR_WANT_WRITE:       return "SSL_ERROR_WANT_WRITE";
	case SSL_ERROR_WANT_CONNECT:     return "SSL_ERROR_WANT_CONNECT";
	case SSL_ERROR_WANT_ACCEPT:      return "SSL_ERROR_WANT_ACCEPT";
	case SSL_ERROR_WANT_X509_LOOKUP: return "SSL_ERROR_WANT_X509_LOOKUP";
	case SSL_ERROR_SYSCALL:          return "SSL_ERROR_SYSCALL";
	case SSL_ERROR_SSL:              return "SSL_ERROR_SSL";
	}
	return "(unknown error)";
}

uv_buf_t ConnectionPrivate::AllocCallback(uv_handle_t *handle, size_t suggested_size) {
	char *buf = static_cast<char *>(calloc(suggested_size, 1));
	return uv_buf_init(buf, suggested_size);
}

void ConnectionPrivate::OnRead(uv_stream_t *stream, ssize_t nread, uv_buf_t buf) {
	ConnectionPrivate *cp = static_cast<ConnectionPrivate *>(stream->data);
	assert(cp != nullptr);

	std::cerr << "connection READ! " << nread << std::endl;

	if (cp->state_ == CONNECTION_STATE_STARVED_SSL_CONNECT) {
		ByteArray b(buf.base, nread, buf.len);
		cp->biostate_->PutNewBuffer(b);

		int err = SSL_connect(cp->ssl_);
		if (err == -1) {
			int SSLerr = SSL_get_error(cp->ssl_, err);
			std::cerr << "TLS fatal error or non-blocking signal [" << SSLerr << "] (" << SSLErrorLookup(SSLerr) << ")" << std::endl;
		} else {
			std::cerr << "SSL_Connect returned " << err << std::endl;
		}

		return;
	}

	return;
}

void ConnectionPrivate::OnConnect(uv_connect_t *connect, int status) {
	ConnectionPrivate *cp = static_cast<ConnectionPrivate *>(connect->data);
	assert(cp->state_ == CONNECTION_STATE_PRE_CONNECT);

	// Unable to connect.
	if (status == -1) {
		uv_err_t last = uv_last_error(cp->loop_);
		std::cerr << "connection failed: " << uv_strerror(last) << std::endl;
		return;
	}

	// Begin reading the incoming stream of data.
	int err = uv_read_start(connect->handle, ConnectionPrivate::AllocCallback, ConnectionPrivate::OnRead);
	if (err != UV_OK) {
		std::cerr << "uv_read_start failed!" << std::endl;
		return;
	}

	// Set up the OpenSSL context
	const SSL_METHOD *meth = TLSv1_client_method();
	cp->ctx_ = SSL_CTX_new(meth);

	SSL_CTX_set_verify(cp->ctx_, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
	SSL_CTX_set_cert_verify_callback(cp->ctx_, ConnectionPrivate::SSLVerifyCallback, cp);

	cp->ssl_ = SSL_new(cp->ctx_);
	SSL_set_connect_state(cp->ssl_);
	cp->bio_ = BIO_new(UVBioState::GetMethod());
	cp->biostate_ = new UVBioState(connect);
	cp->bio_->ptr = cp->biostate_;
	SSL_set_bio(cp->ssl_, cp->bio_, cp->bio_);

	// Set an empty X509_STORE as our SSL_CTX cert store.
	// The default store should be empty as well, but let's
	// make sure.
	X509_STORE *store = X509_STORE_new();
	SSL_CTX_set_cert_store(cp->ctx_, store);

	// Attempt to do a TLS handshake as-is. This will initiate
	// an initial write, but will (most likely) fail with an
	// SSL_ERROR_WANT_READ because we haven't yet read anything.
	err = SSL_connect(cp->ssl_);

	// Success
	if (err == 1) {
		// todo(mkrautz): This is an error state at this point
		// in time. We should tear down the connection at this
		// point.
		std::cerr << "TLS handshake successful" << std::endl;
		return;

	// Controlled shutdown
	} else if (err == 0) {
		// todo(mkrautz): Tear down connection and tell the delegate.
		std::cerr << "TLS shutdown" << std::endl;
		return;

	// Error
	} else if (err < 0) {
		int SSLerr = SSL_get_error(cp->ssl_, err);
		if (SSLerr != SSL_ERROR_WANT_READ) {
			// todo(mkrautz): Tear down connection and tell the delegate.
			std::cerr << "TLS fatal error or non-blocking signal [" << SSLerr << "] (" << SSLErrorLookup(SSLerr) << ")" << std::endl;
			return;
		} else {
			// OpenSSL wants more data from the server to be able to continue the handshake.
			cp->state_ = CONNECTION_STATE_STARVED_SSL_CONNECT;
		}
	}

	std::cerr << "connect successful!" << std::endl;
}

int ConnectionPrivate::SSLVerifyCallback(X509_STORE_CTX *ctx, void *udata) {
	ConnectionPrivate *cp = static_cast<ConnectionPrivate *>(udata);

	X509 *cert = ctx->cert;
	STACK_OF(X509) *chain = ctx->untrusted;

	std::list<X509Certificate> verification_chain;
	int ncerts = sk_X509_num(chain);
	for (int i = 0; i < ncerts; i++) {
		cert = sk_X509_pop(chain);

		int buflen = i2d_X509(cert, NULL);
		ByteArray cert_der(buflen);
		char *tmp = cert_der.Data();
		int err = i2d_X509(cert, reinterpret_cast<unsigned char **>(&tmp));
		if (err == 0) {
			// todo(mkrautz): make this an error. tear down and tell the delegate.
			std::cerr << "unable to convert X509 to DER" << std::endl;
			return 0;
		}

		X509Certificate parsed_cert = X509Certificate::FromRawDERData(cert_der);
		verification_chain.push_front(parsed_cert);
	}

	for (X509Certificate cert : verification_chain) {
		std::cerr << "subject = " << cert.SubjectName() << std::endl;
		std::cerr << "issuer = " << cert.IssuerName() << std::endl;
	}

	std::vector<X509Certificate> verification_vector(verification_chain.begin(), verification_chain.end());

	ByteArray digest = verification_vector.at(0).SHA256Digest();
	for (int i = 0; i < digest.Length(); i++) {
		printf("%02x", digest.Data()[i] & 0xff);
	}
	printf("\n");

	X509Verifier &v = X509Verifier::SystemVerifier();
	X509VerifierOptions opts;
	opts.dns_name = std::string("www.google.com");
	opts.time = std::time(nullptr);
	bool ok = v.VerifyChain(verification_vector, opts);
	std::cerr << "verification of chain = " << ok << std::endl;

	return 1;
}

void ConnectionPrivate::OnPingTimer(uv_timer_t *tmr, int) {
	std::cerr << "ping timer fired!" << std::endl;
}

}