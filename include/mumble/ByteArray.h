// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_BYTEARRAY_H_
#define MUMBLE_BYTEARRAY_H_

namespace mumble {

class ByteArray {
public:
	ByteArray();
	ByteArray(int len);
	ByteArray(char *buf, int len, int cap = -1);
	ByteArray(const ByteArray &ba);
	ByteArray& operator=(ByteArray ba);
	~ByteArray();

	bool operator==(const ByteArray &other) const;
	bool operator!=(const ByteArray &other) const;

	bool IsNull() const;
	int Capacity() const;
	int Length() const;
	char *Data();
	const char *ConstData() const;
	ByteArray Slice(int off, int len = -1);
	ByteArray &Append(const ByteArray &chunk);
	ByteArray &Truncate(int len);
	bool Equal(const ByteArray &other) const;
protected:
	void swap(ByteArray &a, ByteArray &b);
	char  *buf_;
	int   len_;
	int   cap_;
};

}

#endif