// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_TLSCONNECTION_H_
#define MUMBLE_TLSCONNECTION_H_

#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <functional>

#include <mumble/ByteArray.h>
#include <mumble/X509Certificate.h>
#include <mumble/Error.h>

namespace mumble {

class TLSConnectionPrivate;

/// TLSConnectionChainVerifyHandler is a handler in TLSConnection that overrides
/// the TLSConnection's default X.509 verification mechanism.
///
/// @parm      chain   The certificate chain the TLSConnectionChainVerifyHandler
///                    should attempt to verify.
///
/// @ return   The TLSConnectionChainVerifyHandler should return true if the certificate chain
///            was deemed valid for the TLSConnection's purpose. In all other cases, it should
///            return false.
typedef std::function<bool (const std::vector<X509Certificate> &chain)>  TLSConnectionChainVerifyHandler;

/// TLSConnectionEstablishedHandler is a handler in TLSConnection that is called
/// when a TLS connection has been fully established, signalling that the
/// TLSConnection can be used to send and receive data.
typedef std::function<void ()>                                           TLSConnectionEstablishedHandler;

/// TLSConnectionReadHandler is a handler in TLSConnection that is called whenever
/// new data is received from the remote side of the TLSConnection.
typedef std::function<void (const ByteArray &buf)>                       TLSConnectionReadHandler;

/// TLSConnectionErrorHandler is a handler in TLSConnection that is called whenever
/// an error occurs in the TLSconnection. When the handler is called, the TLSConnection
/// is guaranteed to be fully disconnected.
typedef std::function<void (const Error &err)>                           TLSConnectionErrorHandler;

/// TLSConnectionDisconntHandler is a handler in TLSConnection that is called when either
/// side of the TLSConnection closes the connection.
///
/// @param    local   Local determines whether it was the local side that closed
///                   the connection.
typedef std::function<void (bool local)>                                  TLSConnectionDisconnectHandler;

/// TLSConnection implements a TLS client connection.
class TLSConnection {
public:
	/// Constructs a new TLSConnection.
	TLSConnection();

	/// Destroys a TLSConnection.
	~TLSConnection();

	/// Connect initializes a connection to a remote host.
	///
	/// @param   ipaddr  The IP adress to connect to.
	/// @param   port    The port number to connect to.
	///
	/// @return  Returns an Error object representing whether
	///          or not an Error happened during connection
	///          initialization.
	Error Connect(const std::string &ipaddr, int port);

	/// Disconnect forces the connection to shut down.
	void Disconnect();

	/// Write writes the contents of the ByteArray to the TLS connection.
	///
	/// @param   buf   The ByteArray to write to the TLSConnection.
	void Write(const ByteArray &buf);

	/// SetChainVerifyHandler sets an override handler for the TLSConnection's
	/// certificate chain verification mechanism. By default, TLSConnection will
	/// invoke the system's own X.509 certificate chain verifier, but if this
	/// handler is registered, the verification can be entirely custom.
	///
	/// One use of this handler is to allow clients to store fingerprints of
	/// certificates for servers they frequently visit, if these server's chains
	/// are self-signed. This allows implementors to not be forced to show
	/// warnings messages for self-signed chains, and instead implement a more
	/// friendly flow for self-signed certificates.
	///
	/// @param    fn   The TLSConnectionChainVerifyHandler to register.
	///
	/// @return   Returns a reference to the TLSConnection that this method
	///           was called on. This makes it possible to chain calls to
	///           the various handler setters.
	TLSConnection& SetChainVerifyHandler(TLSConnectionChainVerifyHandler fn);

	/// SetEstablishedHandler sets the TLSConnection's *established handler*
	/// which will be called once the connection is ready to be read from and written
	/// to.
	///
	/// @param    fn   The TLSConnectionEstablishedHandler to register.
	///
	/// @return   Returns a reference to the TLSConnection that this method
	///           was called on. This makes it possible to chain calls to
	///           the various handler setters.
	TLSConnection& SetEstablishedHandler(TLSConnectionEstablishedHandler fn);

	/// SetReadHandler sets the TLSConnection's *read handler* which will be
	/// called once the TLSConnection has new data for the client.
	///
	/// @param    fn   The TLSConnectionReadHandler to register.
	///
	/// @return   Returns a reference to the TLSConnection that this method
	///           was called on. This makes it possible to chain calls to
	///           the various handler setters.
	TLSConnection& SetReadHandler(TLSConnectionReadHandler fn);

	/// SetErrorHandler sets the TLSConnection's *error handler* which will be
	/// called if an error happens in the TLSConnection. Whenever this function
	/// is invoked, the connection is also guaranteed to be disconnected. That is,
	/// it is valid to call Connect on it again to attempt to re-establish the
	/// connection.
	TLSConnection& SetErrorHandler(TLSConnectionErrorHandler fn);

	/// SetDisconnectHandler sets the TLSConnection's *disconnect handler* which
	/// will be called if either the local or remote side of the TLSConnection
	/// close the connection.
	TLSConnection& SetDisconnectHandler(TLSConnectionDisconnectHandler fn);

private:
	friend class TLSConnectionPrivate;
	std::unique_ptr<TLSConnectionPrivate> priv_;
};

}

#endif