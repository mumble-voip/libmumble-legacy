// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_UVBIO_H_
#define MUMBLE_UVBIO_H_

#include <mumble/ByteArray.h>

#include "uv.h"

#include <openssl/bio.h>

#include <list>

namespace mumble {

class UVBioState {
public:
	static BIO_METHOD *GetMethod();

	UVBioState(uv_connect_t *connect);
	~UVBioState();

	void PutNewBuffer(ByteArray ba);
	void PutOldBuffer(ByteArray ba);
	bool HasBuffers();
	ByteArray GetBuffer();

	static int Create(BIO *b);
	static int Destroy(BIO *b);
	static int Read(BIO *b, char *buf, int len);
	static void WriteCallback(uv_write_t *req, int status);
	static int Write(BIO *b, const char *buf, int len);
	static int Puts(BIO *b, const char *str);
	static int Gets(BIO *B, char *buf, int len);
	static long Ctrl(BIO *b, int cmd, long num, void *ptr);

	static BIO_METHOD    method_;
	uv_connect_t         *connect_;
	std::list<ByteArray>  bufs_;
};

}

#endif
