// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_CONNECTION_H_
#define MUMBLE_CONNECTION_H_

#include <memory>

namespace mumble {

class ConnectionPrivate;

class Connection {
public:
	Connection();

	/// Connect initiazes a connection to a remote host.
	///
	/// @param  host  The address and port to connect to.
	int Connect(std::string host);
private:
	friend class ConnectionPrivate;
	std::unique_ptr<ConnectionPrivate> dptr_;
};

}

#endif