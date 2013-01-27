// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_CONNECTION_P_H_
#define MUMBLE_CONNECTION_P_H_

#include <memory>
#include <atomic>

#include "uv.h"

#include <openssl/ssl.h>

#include <mumble/TLSConnection.h>
#include "UVBio.h"

namespace mumble {

class TLSConnectionPrivate {
public:
	enum TLSConnectionState {
		TLS_CONNECTION_STATE_INVALID,
		TLS_CONNECTION_STATE_PRE_CONNECT,         // uv_connect has not yet succeeded.
		TLS_CONNECTION_STATE_STARVED_SSL_CONNECT, // SSL_connect has not yet succeded, but failed with a SSL_ERROR_WANT_READ
		TLS_CONNECTION_STATE_ESTABLISHED,
		TLS_CONNECTION_STATE_DISCONNECTED_ERROR,  // We've been disconnected by an error.
		TLS_CONNECTION_STATE_DISCONNECTED_REMOTE, // Remote end closed the connection.
		TLS_CONNECTION_STATE_DISCONNECTED_LOCAL,  // Disconnect was used to shut down the TLSConnection on our end.
	};

	TLSConnectionPrivate();
	~TLSConnectionPrivate();

	Error Connect(const std::string &ipaddr, int port);
	void Disconnect();
	void Write(const ByteArray &buf);

	void Shutdown(TLSConnectionState state);
	void ShutdownRemote();
	void ShutdownError(const Error &err);

	TLSConnectionState                state_;

	uv_loop_t                         *loop_;
	uv_thread_t                       thread_;
	uv_tcp_t                          tcpsock_;
	uv_connect_t                      tcpconn_;

	std::atomic<unsigned long>        thread_id_;

	UVBioState                        *biostate_;

	SSL_CTX                           *ctx_;
	SSL                               *ssl_;
	BIO                               *bio_;

	uv_mutex_t                        wqlock_;
	uv_async_t                        wqasync_;
	std::queue<ByteArray>             wq_;

	uv_async_t                        dcasync_;

	Error                             err_;

	TLSConnectionChainVerifyHandler   chain_verify_handler_;
	TLSConnectionEstablishedHandler   established_handler_;
	TLSConnectionReadHandler          read_handler_;
	TLSConnectionErrorHandler         error_handler_;
	TLSConnectionDisconnectHandler    disconnect_handler_;

	bool HandleStarvedConnectState();
	void TransitionToConnectionEstablishedState();

	static void InitializeSSL();
	static void TLSConnectionThread(void *conn);
	static void OnConnect(uv_connect_t *conn, int status);
	static void OnRead(uv_stream_t *stream, ssize_t nread, uv_buf_t buf);
	static void OnDrainWriteQueue(uv_async_t *handle, int status);
	static void OnDisconnectRequest(uv_async_t *handle, int status);
	static uv_buf_t AllocCallback(uv_handle_t *handle, size_t suggested_size);
	static int SSLVerifyCallback(X509_STORE_CTX *store, void *udata);
};

}

#endif