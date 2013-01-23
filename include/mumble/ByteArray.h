// Copyright (c) 2012 The libmumble Developers
// The use of this source code is goverened by a BSD-style
// license that can be found in the LICENSE-file.

#ifndef MUMBLE_BYTEARRAY_H_
#define MUMBLE_BYTEARRAY_H_

namespace mumble {

/// ByteArray implements a minimal API for working with auto-expanding buffers.
class ByteArray {
public:
	/// Constructs a null ByteArray. A null ByteArray has no underlying storage.
	ByteArray();

	/// Constructs a ByteArray with a length and capacity of *len*.
	/// This is typically used to create a buffer that code can write
	/// into by accessing the underlying array via Data().
	///
	/// It is possible to truncate a ByteArray returned by this function
	/// by using the Truncate method. This is useful if the initial length
	/// exceeds the actual content of the ByteArray. For example, one might
	/// use this constructor for use with a Unix-style read() system call
	/// that is allowed to return less bytes than requested. The ByteArray
	/// that the read() system read into can then be truncated using the
	/// Truncate method.
	///
	/// @param  len   The length of ByteArray.
	ByteArray(int len);

	/// Constructs a ByteArray using the contents of buf as as
	/// the basis for the ByteArray. The constructor will copy
	/// *len* bytes of *buf* into its underlying storage. The
	/// ByteArray's underlying storage will consist of at least
	/// *len* bytes, but this can be overridden by using the *cap*
	/// parameter.
	///
	/// @param   buf   The buffer to use as the basis for the
	///                newly constructed ByteArray.
	/// @param   len   Length of the buffer *buf*.
	/// @param   cap   The requested capacity of the newly
	///                constructed ByteArray's underlying
	///                storage. Must be >= *len*.
	///
	/// @return  Returns a new ByteArray with the *len* bytes of *buf*
	///          as its initial content, and a capacity of *cap*. If
	///          invalid parameters were passed to the function, it may
	///          return a null ByteArray. (See IsNull).
	ByteArray(char *buf, int len, int cap = -1);

	/// Constructs a copy of ba.
	ByteArray(const ByteArray &ba);

	/// Assigns ba to this ByteArray.
	ByteArray& operator=(ByteArray ba);
	~ByteArray();

	/// Operator == returns true if *other* holds the same bytes
	/// as this ByteArray. It is implemented using the Equal method.
	///
	/// @param   other   The ByteArray this ByteArray should be compared to.
	///
	/// @return  Returns true if *other* is equal to this ByteArray.
	bool operator==(const ByteArray &other) const;

	/// Operator != checks whether *other* is not equal to this ByteArray.
	/// It is implemented using the Equal method.
	///
	/// @param    other   The ByteArray this ByteArray should be compared to.
	///
	/// @return   Returns true if *other* is not equal to this ByteArray.
	bool operator!=(const ByteArray &other) const;

	/// IsNull returns true if the ByteArray has no underlying storage
	/// allocated for it. This is useful for distinguishing between
	/// an empty ByteArray and a non-present ByteArray in some cases.
	///
	/// A null ByteArray can be an error state. For example, a constructor
	/// of ByteArray may return a null ByteArray if the input parameters it
	/// was given are invalid.
	bool IsNull() const;

	/// Capacity returns the capacity of the ByteArray. This represents the
	/// size of the underlying storage for the ByteArray. To avoid repated
	/// allocations, it can make sense to pre-allocate a large ByteArray.
	/// For example, this allows calls to Append to avoid allocating a new
	/// underlying storage block for the ByteArray on each call.
	///
	/// @return  Capacity of the ByteArray. (Length of the underlying storage).
	int Capacity() const;

	/// Length returns the length of content of the ByteArray. The length
	/// of a ByteArray can be changed at will using the *Truncate* method.
	///
	/// @return  The length of the content of the ByteArray in bytes.
	int Length() const;

	/// Data returns a pointer to the ByteArray's underlying storage.
	///
	/// @return  Pointer to the ByteArray's underlying storage.
	char *Data();

	/// ConstData returns pointer to a const-decorated version of the ByteArray's
	/// underlying storage.
	///
	/// @return   Pointer to const-decorated underlying storage of the ByteArray.
	const char *ConstData() const;

	/// Slice slices the ByteArray. Slice returns a new ByteArray
	/// that will use *len* bytes starting at *off* from this
	/// ByteArray as it's initial content.
	///
	/// @param   off   The offset into the ByteArray's underlying storage
	///                that will be sliced into a new ByteArray.
	/// @param   len   The amount of bytes, starting from *off* that will
	///                be used for the new ByteArray's initial content.
	ByteArray Slice(int off, int len = -1);

	/// Append appends *chunk* to this ByteArray. If this ByteArray's capacity
	/// allows it, this will be a simple memory copy operation. However, if this
	/// ByteArray's underlying storage cannot contain *chunk*, a new underlying
	/// storage array will be allocated for this array, which will include the
	/// original content followed by the content of *chunk*.
	///
	/// @param   chunk   The chunk to append to this ByteArray.
	///
	/// @return  Returns a reference to the ByteArray that *chunk* was appended to.
	ByteArray &Append(const ByteArray &chunk);

	/// Truncate changes the *length* of the ByteArray.
	///
	/// This is useful if the initial length of the ByteArray exceeds the
	/// actual content of the ByteArray. For example, one might construct
	/// a ByteArray with a fairly big length, and use that ByteArray as the
	/// destination for a read() system call that is allowed to return less
	/// bytes than requested. The ByteArray that the read() system read into
	/// can then be truncated using this method to signal the *actual* length
	/// of the ByteArray.
	///
	/// @param   len   The new length of the ByteArray.
	///
	/// @return  Returns a reference to the ByteArray that was truncated.
	ByteArray &Truncate(int len);

	/// Equal determines whether *other* is equal to this ByteArray.
	/// A ByteArray is equal to another ByteArray if their contents match.
	/// The content of a ByteArray is specified by its length, and *length*
	/// bytes of its underlying storage. This means that two ByteArrays with
	/// *length* bytes of identical content, but differing capacities still
	/// can be equal to each other.
	///
	/// @param   other   The ByteArray that this ByteArray should be checked
	///                  for equality against.
	/// @return  Returns true if both ByteArrays are equal. Otherwise false.
	bool Equal(const ByteArray &other) const;
private:
	char  *buf_;
	int   len_;
	int   cap_;
};

}

#endif