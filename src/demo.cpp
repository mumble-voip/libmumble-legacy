// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/X509Certificate.h>
#include <mumble/X509Verifier.h>
#include <mumble/TLSConnection.h>
#include <mumble/Error.h>

#include <string>
#include <iostream>
#include <cstring>

#include "uv.h"

static uv_timer_t timer;

void timerFunc(uv_timer_t *tmr, int) {
	// No-op to keep the main loop running.
}

int main(int argc, char **argv) {
	mumble::TLSConnection conn;

	conn.SetChainVerifyHandler([&](const std::vector<mumble::X509Certificate> &chain) {
		mumble::X509Verifier &system_verifier = mumble::X509Verifier::SystemVerifier();
		
		mumble::X509VerifierOptions opts;
		opts.dns_name = std::string("www.google.com");
		opts.time = 0;

		return system_verifier.VerifyChain(chain, opts);
	}).SetEstablishedHandler([&]{
		const char *req = "GET / HTTP/1.1\r\n"
		                  "Host: www.google.com\r\n"
		                  "Connection: close\r\n\r\n";
		mumble::ByteArray getReq(const_cast<char *>(req), strlen(req));
		conn.Write(getReq);
	}).SetReadHandler([&](const mumble::ByteArray &buf) {
		std::string rsp(buf.ConstData(), buf.Length());
		std::cerr << "Connection response: " << std::endl;
		std::cerr << rsp;
	}).SetErrorHandler([&](const mumble::Error &err) {
		std::cerr << "Connection error: " << err.String() << std::endl;
	}).SetDisconnectHandler([&](bool local) {
		std::cerr << "Disconnect! (local? " << local << ")" << std::endl;
	});

	mumble::Error err = conn.Connect(std::string("173.194.66.106"), 443);
	if (err.HasError()) {
		std::cerr << "Got error from Connect: " << err.String() << std::endl;
	}

	uv_loop_t *loop = uv_default_loop();

	uv_timer_init(loop, &timer);
	uv_timer_start(&timer, timerFunc, 1000, 1000);

	if (uv_run(loop) == -1) {
		std::cerr << "got error: " << uv_strerror(uv_last_error(loop)) << std::endl;
	}
}
