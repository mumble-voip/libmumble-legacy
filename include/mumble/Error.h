// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_ERROR_H_
#define MUMBLE_ERROR_H_

#include <memory>
#include <string>
#include <ostream>

namespace mumble {

struct ErrorPrivate;

/// Error represents an error returned by a libmumble component.
class Error {
public:
	/// Constructs a new Error with the given domain, code and description.
	///
	/// @param    domain        The error domain. This describes the context in which
	///                        the error code is to be understood. For example, an
	///                        error caused by libuv might have a domain of "uv".
	/// @param    code          An error code in the context of the given domain.
	/// @param    description   A description of the error code in the context of the given domain.
	///
	/// @return   Returns an Error described by domain, code and description.
	static Error ErrorFromDescription(std::string domain, long code, std::string description);

	/// Constructs an empty error signalling
	/// that the Error objects contains no error
	/// code. This is useful for signalling *success*
	/// rather than *failure* when using Error objects.
	///
	/// This form of Error is typically used when
	/// an Error is used as a return value from a
	/// function.
	///
	/// @return  Returns an empty error. The HasError
	///          method on empty errors returns false.
	static Error NoError();

	/// Constructs an empty Error.
	Error();

	/// Copies the Error *err* into this Error.
	Error(const Error &err);

	/// Assigns the Error *err* to this Error.
	Error& operator=(Error err);

	/// Destroys this Error.
	~Error();

	/// HasError determines whether the Error represents an
	/// error state. This function is used to determine
	/// whether an Error returned by a function signals
	/// an error state, or successful execution.
	bool HasError() const;

	/// Domain returns a textual description of the domain.
	///
	std::string Domain() const;
	long Code() const;
	std::string Description() const;

	/// String returns a textual description of the error.
	std::string String() const;

private:
	friend class ErrorPrivate;
	std::unique_ptr<ErrorPrivate> priv_;
};

}

#endif