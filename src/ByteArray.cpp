// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#include <mumble/ByteArray.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "assert.h"

namespace mumble {

ByteArray::ByteArray() {
	buf_ = nullptr;
	len_ = 0;
	cap_ = 0;
}

ByteArray::ByteArray(int len) {
	buf_ = static_cast<char *>(malloc(len));
	len_ = len;
	cap_ = len;
}

ByteArray::ByteArray(char *buf, int len, int cap) {
	if (buf == nullptr) {
		buf_ = nullptr;
		len_ = 0;
		cap_ = 0;
	} else {
		if (cap == -1) {
			cap = len;
		}
		buf_ = static_cast<char *>(malloc(cap));
		memcpy(buf_, buf, cap);
		len_ = len;
		cap_ = cap;
	}
}

ByteArray::ByteArray(const ByteArray &ba) {
	if (ba.buf_ != nullptr) {
		buf_ = static_cast<char *>(malloc(ba.cap_));
		memcpy(buf_, ba.buf_, ba.cap_);
		len_ = ba.len_;
		cap_ = ba.cap_;
	} else {
		buf_ = nullptr;
		len_ = 0;
		cap_ = 0;
	}
}

void ByteArray::swap(ByteArray &a, ByteArray &b) {
	std::swap(a.buf_, b.buf_);
	std::swap(a.len_, b.len_);
	std::swap(a.cap_, b.cap_);
}

ByteArray& ByteArray::operator=(ByteArray ba) {
	swap(*this, ba);
	return *this;
}

ByteArray::~ByteArray() {
	free(buf_);
}

bool ByteArray::IsNull() const {
	return buf_ == nullptr;
}

int ByteArray::Capacity() const {
	return cap_;
}

int ByteArray::Length() const {
	return len_;
}

const char *ByteArray::ConstData() const {
	return buf_;
}

char *ByteArray::Data() {
	return buf_;
}

ByteArray ByteArray::Slice(int off, int len) {
	int remain = len_ - off;
	// If len is -1, it simply means we should
	// copy all the remaining bytes.
	if (len == -1) {
		len = remain;
	}
	
	assert(len <= remain);

	char *tmp = static_cast<char *>(malloc(len));
	memcpy(tmp, buf_+off, remain);
	return ByteArray(tmp, remain);
}

// Append appends chunk to the ByteArray.
ByteArray &ByteArray::Append(const ByteArray &chunk) {
	// First, ensure we have space for the chunk.
	int remain = cap_ - len_;
	if (remain < chunk.Length()) {
		int larger_cap = Length() + chunk.Length();
		char *larger_buf = static_cast<char *>(realloc(buf_, larger_cap));
		buf_ = larger_buf;
		cap_ = larger_cap;
	}
	// Do the append.
	char *dst = buf_ + len_;
	memcpy(dst, chunk.ConstData(), chunk.Length());
	len_ += chunk.Length();

	return *this;
}

// Truncate truncates the length of the ByteArray to len.
// The new length must be <= the current length.
ByteArray &ByteArray::Truncate(int len) {
	assert(len <= len_);
	len_ = len;
	return *this;
}

// Check whether the other ByteArray is equal to this.
bool ByteArray::Equal(const ByteArray &other) const {
	if (Length() != other.Length()) {
		return false;
	}
	if (Length() == 0 && other.Length() == 0) {
		// If both are empty, ensure that they are either
		// both null, or both non-null.
		return IsNull() == other.IsNull();
	}
	if (memcmp(ConstData(), other.ConstData(), Length()) == 0) {
		return true;
	}
	return false;
}

// Operator == is an alias for Equal.
bool ByteArray::operator==(const ByteArray &other) const {
	return Equal(other);
}

// Operator != is an alias for !Equal.
bool ByteArray::operator!=(const ByteArray &other) const {
	return !Equal(other);
}

}