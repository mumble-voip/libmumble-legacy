// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/TLSConnection.h>
#include "TLSConnection_p.h"

#include <string>

namespace mumble {

TLSConnection::TLSConnection() : priv_(new TLSConnectionPrivate) {
}

TLSConnection::~TLSConnection() {
}

Error TLSConnection::Connect(const std::string &ipaddr, int port) {
	return priv_->Connect(ipaddr, port);
}

void TLSConnection::Disconnect() {
	priv_->Disconnect();
}

void TLSConnection::Write(const ByteArray &buf) {
	priv_->Write(buf);
}

TLSConnection& TLSConnection::SetChainVerifyHandler(TLSConnectionChainVerifyHandler fn) {
	priv_->chain_verify_handler_ = fn;
	return *this;
}

TLSConnection& TLSConnection::SetEstablishedHandler(TLSConnectionEstablishedHandler fn) {
	priv_->established_handler_ = fn;
	return *this;
}

TLSConnection& TLSConnection::SetReadHandler(TLSConnectionReadHandler fn) {
	priv_->read_handler_ = fn;
	return *this;
}

TLSConnection& TLSConnection::SetErrorHandler(TLSConnectionErrorHandler fn) {
	priv_->error_handler_ = fn;
	return *this;
}

TLSConnection &TLSConnection::SetDisconnectHandler(TLSConnectionDisconnectHandler fn) {
	priv_->disconnect_handler_ = fn;
	return *this;
}

}