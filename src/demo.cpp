// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/Connection.h>

#include <string>
#include <iostream>

#include "uv.h"

static uv_timer_t timer;

void timerFunc(uv_timer_t *tmr, int) {
	// No-op to keep the main loop running.
}

int main(int argc, char **argv) {
	int err;

	mumble::Connection *conn = new mumble::Connection();

	err = conn->Connect(std::string());
	std::cerr << "got err " << err << " from connect" << std::endl;

	uv_loop_t *loop = uv_default_loop();

	uv_timer_init(loop, &timer);
	uv_timer_start(&timer, timerFunc, 1000, 1000);

	err = uv_run(loop);
	if (err != UV_OK) {
		std::cerr << "got error: " << uv_strerror(uv_last_error(loop)) << std::endl;
	}
}
