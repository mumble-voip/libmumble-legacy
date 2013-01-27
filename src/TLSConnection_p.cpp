// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/TLSConnection.h>
#include "TLSConnection_p.h"
#include <mumble/X509Certificate.h>
#include "X509Certificate_p.h"
#include <mumble/Error.h>
#include <mumble/X509Verifier.h>
#include "OpenSSLUtils.h"
#include "UVUtils.h"
#include "UVBio.h"
#include "Utils.h"

#include <string>
#include <iostream>
#include <assert.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/x509.h>

namespace mumble {

TLSConnectionPrivate::TLSConnectionPrivate() : thread_id_(0) {
	OpenSSLUtils::EnsureInitialized();
}

TLSConnectionPrivate::~TLSConnectionPrivate() {
}

Error TLSConnectionPrivate::Connect(const std::string &ipaddr, int port) {
	int err;

	loop_ = uv_loop_new();

	struct sockaddr_in addr = uv_ip4_addr(ipaddr.c_str(), port);

	err = uv_tcp_init(loop_, &tcpsock_);
	if (err != UV_OK) {
		return UVUtils::ErrorFromLastUVError(loop_);
	}

	tcpconn_.data = static_cast<void *>(this);
	tcpsock_.data = static_cast<void *>(this);
	err = uv_tcp_connect(&tcpconn_, &tcpsock_, addr, TLSConnectionPrivate::OnConnect);
	if (err != UV_OK) {
		return UVUtils::ErrorFromLastUVError(loop_);
	}

	uv_mutex_init(&wqlock_);
	uv_async_init(loop_, &wqasync_, TLSConnectionPrivate::OnDrainWriteQueue);
	wqasync_.data = this;

	uv_async_init(loop_, &dcasync_, TLSConnectionPrivate::OnDisconnectRequest);
	dcasync_.data = this;

	state_ = TLS_CONNECTION_STATE_PRE_CONNECT;

	err = uv_thread_create(&thread_, TLSConnectionPrivate::TLSConnectionThread, this);
	if (err  == -1) {
		return Error::ErrorFromDescription(
			std::string("TLSConnection"),
			0L,
			std::string("unable to create connection thread")
		);
	}

	return Error::NoError();
}

bool TLSConnectionPrivate::HandleStarvedConnectState() {
	int err = SSL_connect(ssl_);
	if (err < 0) {
		int SSLerr = SSL_get_error(ssl_, err);
		if (SSLerr != SSL_ERROR_WANT_READ && SSLerr != SSL_ERROR_WANT_WRITE) {
			ShutdownError(OpenSSLUtils::ErrorFromOpenSSLErrorCode(SSLerr));
			return false;
		}
		return false;
	} else if (err == 0) {
		ShutdownRemote();
		return false;
	} else if (err == 1) {
		TransitionToConnectionEstablishedState();
		return true;
	}

	NotReached();
}

void TLSConnectionPrivate::TransitionToConnectionEstablishedState() {
	state_ = TLS_CONNECTION_STATE_ESTABLISHED;
	if (established_handler_) {
		established_handler_();
	}
}

// Request TLSConnection to close its connection.
void TLSConnectionPrivate::Disconnect() {
	unsigned long us = uv_thread_self();
	unsigned long it = thread_id_.load();
	if (us == it) {
		Shutdown(TLS_CONNECTION_STATE_DISCONNECTED_LOCAL);
	} else if (it > 0) {
		uv_async_send(&dcasync_);
	}
}

void TLSConnectionPrivate::Shutdown(TLSConnectionState state) {
	state_ = state;
	uv_close(reinterpret_cast<uv_handle_t *>(&tcpsock_), nullptr);
	uv_close(reinterpret_cast<uv_handle_t *>(&wqasync_), nullptr);
	uv_close(reinterpret_cast<uv_handle_t *>(&dcasync_), nullptr);
}

// Shutdown because we encountered an error.
void TLSConnectionPrivate::ShutdownError(const Error &err) {
	err_ = err;
	Shutdown(TLS_CONNECTION_STATE_DISCONNECTED_ERROR);
}

void TLSConnectionPrivate::ShutdownRemote() {
	Shutdown(TLS_CONNECTION_STATE_DISCONNECTED_REMOTE);
}

void TLSConnectionPrivate::Write(const ByteArray &buf) {
	// If called from within the runloop's thread, allow the operation
	// to go through immediately.
	unsigned long us = uv_thread_self();
	unsigned long it = thread_id_.load();
	if (us == it) {
		int nread = SSL_write(ssl_, reinterpret_cast<const void *>(buf.ConstData()), buf.Length());
		if (nread < 0) {
			int SSLerr = SSL_get_error(ssl_, nread);
			this->ShutdownError(OpenSSLUtils::ErrorFromOpenSSLErrorCode(SSLerr));
		} else if (nread == 0) {
			this->ShutdownRemote();
		}
	// If called from another thread, add it to the write queue and
	// inform the runloop that there are new bytes to be written.
	} else if (it > 0) {
		uv_mutex_lock(&wqlock_);
		wq_.push(buf);
		uv_mutex_unlock(&wqlock_);
		uv_async_send(&wqasync_);
	}
}

void TLSConnectionPrivate::TLSConnectionThread(void *udata) {
	TLSConnectionPrivate *cp = static_cast<TLSConnectionPrivate *>(udata);

	cp->thread_id_.store(uv_thread_self());

	uv_loop_t *loop = cp->loop_;
	int err;

	err = uv_run(loop);
	if (err != UV_OK) {
		cp->state_ = TLS_CONNECTION_STATE_DISCONNECTED_ERROR;
		cp->err_ = UVUtils::ErrorFromLastUVError(loop);
	}

	cp->thread_id_.store(0);

	uv_loop_delete(loop);

	uv_mutex_lock(&cp->wqlock_);
	cp->wq_ = std::queue<ByteArray>();
	uv_mutex_unlock(&cp->wqlock_);

	switch (cp->state_) {
		case TLS_CONNECTION_STATE_DISCONNECTED_LOCAL:
			if (cp->disconnect_handler_) {
				cp->disconnect_handler_(true);
			}
			break;
		case TLS_CONNECTION_STATE_DISCONNECTED_REMOTE:
			if (cp->disconnect_handler_) {
				cp->disconnect_handler_(false);
			}
			break;
		case TLS_CONNECTION_STATE_DISCONNECTED_ERROR:
			if (cp->error_handler_) {
				cp->error_handler_(cp->err_);
			}
			break;
		default:
			NotReached();
	}
}

uv_buf_t TLSConnectionPrivate::AllocCallback(uv_handle_t *handle, size_t suggested_size) {
	char *buf = static_cast<char *>(calloc(suggested_size, 1));
	return uv_buf_init(buf, suggested_size);
}

void TLSConnectionPrivate::OnRead(uv_stream_t *stream, ssize_t nread, uv_buf_t buf) {
	TLSConnectionPrivate *cp = static_cast<TLSConnectionPrivate *>(stream->data);
	assert(cp != nullptr);

	// Only allow OnRead calls to have an effect on the TLSConnection in
	// the STARVED_SSL_CONNECT and ESTABLISHED states.
	bool bad = cp->state_ != TLS_CONNECTION_STATE_STARVED_SSL_CONNECT &&
	           cp->state_ != TLS_CONNECTION_STATE_ESTABLISHED;
	if (bad) {
		return;
	}

	if (nread == -1) {
		uv_err_t last = uv_last_error(cp->loop_);
		if (last.code == UV_EOF) {
			cp->ShutdownRemote();
		} else {
			cp->ShutdownError(UVUtils::ErrorFromUVError(last));
		}
		return;
	} else if (nread == 0) {
		cp->ShutdownRemote();
		return;
	}

	ByteArray b(buf.base, nread, buf.len);
	cp->biostate_->PutNewBuffer(b);

	if (cp->state_ == TLS_CONNECTION_STATE_STARVED_SSL_CONNECT) {
		if (!cp->HandleStarvedConnectState()) {
			return;
		}
	}

	if (cp->state_ == TLS_CONNECTION_STATE_ESTABLISHED) {
		ByteArray processed(static_cast<int>(nread));

		bool backoff = false;
		while (!backoff) {
			int nread = SSL_read(cp->ssl_, reinterpret_cast<void *>(processed.Data()), processed.Capacity());
			if (nread == -1) {
				int err = SSL_get_error(cp->ssl_, nread);
				if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
					// OpenSSL needs to be fed more data. Back off.
					backoff = true;
					break;
				}
				cp->ShutdownError(OpenSSLUtils::ErrorFromOpenSSLErrorCode(err));
				return;
			} else if (nread == 0) {
				cp->ShutdownRemote();
				return;
			}

			processed.Truncate(nread);
			if (cp->read_handler_) {
				cp->read_handler_(processed);
			}
		}
	}

	return;
}

// OnDrainWriteQueue ensures that all ByteArrays queued up in the TLSConnection's
// write queue are sent.
void TLSConnectionPrivate::OnDrainWriteQueue(uv_async_t *handle, int status) {
	assert(handle != nullptr);
	assert(handle->data != nullptr);

	TLSConnectionPrivate *cp = static_cast<TLSConnectionPrivate *>(handle->data);
	if (cp->state_ == TLS_CONNECTION_STATE_ESTABLISHED) {
		uv_mutex_lock(&cp->wqlock_);
		while (cp->wq_.size() > 0) {
			const ByteArray &buf = cp->wq_.front();
			cp->Write(buf);
			cp->wq_.pop();
		}
		uv_mutex_unlock(&cp->wqlock_);
	}
}

void TLSConnectionPrivate::OnDisconnectRequest(uv_async_t *handle, int status) {
	assert(handle != nullptr);
	assert(handle->data != nullptr);

	TLSConnectionPrivate *cp = static_cast<TLSConnectionPrivate *>(handle->data);
	cp->Shutdown(TLS_CONNECTION_STATE_DISCONNECTED_LOCAL);
}

void TLSConnectionPrivate::OnConnect(uv_connect_t *connect, int status) {
	TLSConnectionPrivate *cp = static_cast<TLSConnectionPrivate *>(connect->data);
	assert(cp->state_ == TLS_CONNECTION_STATE_PRE_CONNECT);

	// Unable to connect.
	if (status == -1) {
		cp->ShutdownError(UVUtils::ErrorFromLastUVError(cp->loop_));
		return;
	}

	// Begin reading the incoming stream of data.
	int err = uv_read_start(connect->handle, TLSConnectionPrivate::AllocCallback, TLSConnectionPrivate::OnRead);
	if (err != UV_OK) {
		cp->ShutdownError(UVUtils::ErrorFromLastUVError(cp->loop_));
		return;
	}

	// Set up the OpenSSL context
	const SSL_METHOD *meth = TLSv1_client_method();
	cp->ctx_ = SSL_CTX_new(meth);

	SSL_CTX_set_verify(cp->ctx_, SSL_VERIFY_PEER|SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);
	SSL_CTX_set_cert_verify_callback(cp->ctx_, TLSConnectionPrivate::SSLVerifyCallback, cp);

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

	cp->state_ = TLS_CONNECTION_STATE_STARVED_SSL_CONNECT;
	cp->HandleStarvedConnectState();
}

int TLSConnectionPrivate::SSLVerifyCallback(X509_STORE_CTX *ctx, void *udata) {
	TLSConnectionPrivate *cp = static_cast<TLSConnectionPrivate *>(udata);

	X509 *cert = ctx->cert;
	STACK_OF(X509) *chain = ctx->untrusted;

	std::list<X509Certificate> verification_chain;
	int ncerts = sk_X509_num(chain);
	for (int i = 0; i < ncerts; i++) {
		cert = sk_X509_value(chain, i);

		int buflen = i2d_X509(cert, NULL);
		ByteArray cert_der(buflen);
		char *tmp = cert_der.Data();
		int err = i2d_X509(cert, reinterpret_cast<unsigned char **>(&tmp));
		if (err == 0) {
			cp->ShutdownError(OpenSSLUtils::ErrorFromLastCryptoError());
			return 0;
		}

		X509Certificate parsed_cert = X509Certificate::FromRawDERData(cert_der);
		verification_chain.push_back(parsed_cert);
	}

	std::vector<X509Certificate> verification_vector(verification_chain.begin(), verification_chain.end());

	if (cp->chain_verify_handler_) {
		bool ok = cp->chain_verify_handler_(verification_vector);
		if (ok) {
			return 1;
		}
		return 0;
	} else {
		X509Verifier &v = X509Verifier::SystemVerifier();
		X509VerifierOptions opts;
		opts.dns_name = std::string("www.google.com");
		opts.time = std::time(nullptr);
		bool ok = v.VerifyChain(verification_vector, opts);
		if (ok) {
			return 1;
		}
		return 0;
	}

	NotReached();
}

}