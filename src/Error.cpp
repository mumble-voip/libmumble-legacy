// Copyright (c) 2013 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/Error.h>

#include <string>
#include <sstream>

namespace mumble {

struct ErrorPrivate {
public:
	ErrorPrivate();
	std::string  domain_;
	std::string  description_;
	long         code_;
};

ErrorPrivate::ErrorPrivate() : code_(0) {
}

Error Error::ErrorFromDescription(std::string domain, long code, std::string description) {
	Error err;
	err.priv_->domain_ = domain;
	err.priv_->code_ = code;
	err.priv_->description_ = description;
	return err;
}

Error Error::NoError() {
	return Error();
}

Error::Error() : priv_(new ErrorPrivate) {
}

Error::~Error() {
}

Error::Error(const Error &err) : priv_(new ErrorPrivate) {
	this->priv_->domain_ = err.priv_->domain_;
	this->priv_->code_ = err.priv_->code_;
	this->priv_->description_ = err.priv_->description_;
}

Error& Error::operator=(Error err) {
	std::swap((*this).priv_, err.priv_);
	return *this;
}

bool Error::HasError() const {
	return !(priv_->domain_.empty() && priv_->description_.empty() && priv_->code_ == 0L);
}

std::string Error::Domain() const {
	return priv_->domain_;
}

long Error::Code() const {
	return priv_->code_;
}

std::string Error::Description() const {
	return priv_->description_;
}

std::string Error::String() const {
	std::stringstream ss;
	ss << priv_->domain_ << ": " << priv_->description_ << " (error code: " << priv_->code_ << ")";
	return ss.str();
}

}