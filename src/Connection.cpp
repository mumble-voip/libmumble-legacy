// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/Connection.h>
#include "Connection_p.h"

#include <string>

namespace mumble {

Connection::Connection() : dptr_(new ConnectionPrivate) {
}

int Connection::Connect(std::string hostname) {
	return dptr_->Connect(hostname);
}

}