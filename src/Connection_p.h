// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_CONNECTION_P_H_
#define MUMBLE_CONNECTION_P_H_

#include <memory>
#include "uv.h"

#include <openssl/ssl.h>

#include "UVBio.h"

namespace mumble {

class ConnectionPrivate {
public:
	ConnectionPrivate();
	int Connect(std::string host);
private:
	enum ConnectionState {
		CONNECTION_STATE_NONE,
		CONNECTION_STATE_PRE_CONNECT,         // uv_connect has not yet succeeded.
		CONNECTION_STATE_STARVED_SSL_CONNECT, // SSL_connect has not yet succeded, but failed with a SSL_ERROR_WANT_READ
		CONNECTION_STATE_RUNNING,
	};

	ConnectionState  state_;

	uv_timer_t       pingtmr_;
	uv_loop_t        *loop_;
	uv_thread_t      thread_;
	uv_tcp_t         tcpsock_;
	uv_connect_t     tcpconn_;

	UVBioState       *biostate_;

	SSL_CTX          *ctx_;
	SSL              *ssl_;
	BIO              *bio_;

private:
	static void InitializeSSL();
	static void ConnectionThread(void *conn);
	static void OnConnect(uv_connect_t *conn, int status);
	static void OnPingTimer(uv_timer_t *tmr, int);
	static void OnRead(uv_stream_t *stream, ssize_t nread, uv_buf_t buf);
	static uv_buf_t AllocCallback(uv_handle_t *handle, size_t suggested_size);
	static int SSLVerifyCallback(X509_STORE_CTX *store, void *udata);
};

}

#endif