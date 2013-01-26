// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/ByteArray.h>
#include "UVBio.h"

#include "uv.h"
#include <openssl/bio.h>

#include <iostream>
#include <cstring>
#include <assert.h>

namespace mumble {

BIO_METHOD UVBioState::method_ = {
	100 | BIO_TYPE_SOURCE_SINK,
	"UVBioState",
	UVBioState::Write,
	UVBioState::Read,
	UVBioState::Puts,
	UVBioState::Gets,
	UVBioState::Ctrl,
	UVBioState::Create,
	UVBioState::Destroy,
};

BIO_METHOD *UVBioState::GetMethod() {
	return &UVBioState::method_;
}

UVBioState::UVBioState(uv_connect_t *connect) {
	connect_ = connect;
}

UVBioState::~UVBioState() {
}

void UVBioState::PutNewBuffer(ByteArray ba) {
	bufs_.push_back(ba);
}

void UVBioState::PutOldBuffer(ByteArray ba) {
	bufs_.push_front(ba);
}

bool UVBioState::HasBuffers() {
	return bufs_.size() > 0;
}

ByteArray UVBioState::GetBuffer() {
	assert(bufs_.size() > 0);
	ByteArray front = bufs_.front();
	bufs_.pop_front();
	return front;
}

int UVBioState::Create(BIO *b) {
	b->init = 1;
	b->num = 0;
	b->ptr = NULL;
	b->flags = 0;
	return 1;
}

int UVBioState::Destroy(BIO *b) {
	if (b == nullptr) {
		return 0;
	}
	b->ptr = NULL;
	b->init = 0;
	b->flags = 0;
	return 1;
}

int UVBioState::Read(BIO *b, char *buf, int len) {
	UVBioState *state = static_cast<UVBioState *>(b->ptr);
	uv_connect_t *connect = state->connect_;
	uv_stream_t *stream = connect->handle;

	if (!state->HasBuffers()) {
		BIO_set_retry_read(b);
		return -1;
	}

	// Get the front-most buffer
	ByteArray ba = state->GetBuffer();
	// Buffer is bigger than what the Read function
	// has space for in its buffer.  Slice our buffer
	// up, and give the Read function what it can take.
	if (ba.Length() > len) {
		// Slice away the remaining part of the
		// buffer and put it back into the queue.
		ByteArray remain = ba.Slice(len);
		state->PutOldBuffer(remain);
		// Copy data from our original ByteArray
		// into the Read functions buffer.
		memcpy(buf, ba.Data(), len);
		return len;
	// We have less space in our buffer than the Read
	// function requested. Give the Read function what
	// we have.
	} else {
		memcpy(buf, ba.Data(), ba.Length());
		return ba.Length();
	}

	/* NOTREACHED */
	assert(1 == 0);
	return -1;
}

void UVBioState::WriteCallback(uv_write_t *req, int status) {
	assert(req != NULL);

	// todo(mkrautz): should we propagate write errors as errors to the TLSConnection?
	if (status != 0) {
		std::cerr << "UVBioState::WriteCallback status = " << status << std::endl;
		return;
	}
}

int UVBioState::Write(BIO *b, const char *buf, int len) {
	UVBioState *state = static_cast<UVBioState *>(b->ptr);
	uv_connect_t *connect = state->connect_;
	uv_stream_t *stream = connect->handle;

	uv_write_t *req = static_cast<uv_write_t *>(calloc(sizeof(*req), 1));
	uv_buf_t uvbuf;
	uvbuf.base = const_cast<char *>(buf);
#ifdef LIBMUMBLE_OS_WINDOWS
	uvbuf.len = static_cast<ULONG>(len);
#else
	uvbuf.len = static_cast<size_t>(len);
#endif

	int err = uv_write(req, stream, &uvbuf, 1, UVBioState::WriteCallback);
	if (err != UV_OK) {
		std::cerr << "uv_write failed!" << std::endl;
		return -1;
	}

	return len;
}

int UVBioState::Puts(BIO *b, const char *str) {
	return -1;
}

int UVBioState::Gets(BIO *B, char *buf, int len) {
	return -1;
}

long UVBioState::Ctrl(BIO *b, int cmd, long num, void *ptr) {
	if (cmd == BIO_CTRL_FLUSH) {
		return 1;
	}
	return 0;
}

}