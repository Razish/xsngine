#pragma once

#include <vector>
#include <string>

// a ByteBuffer represents a chunk of memory than can *either* be written to, or read from, but never both.
// common usage includes serialising an object for network transmission, or R/W of a binary file
//
// strings are written in pascal-style meaning the size of the string is written as a 32 bit integer followed by the
//	contents of the string
//
// memory chunks are written as is, and the size must be transmitted by other means. the size is not written to the
//	ByteBuffer at all
//
// PORTABILITY:
//	note that strings are written as pascal-style strings, consistent with most network transmission code
//	a side effect of this is that strings, as well as generic data chunks ARE NOT 64 BIT SAFE (i.e. sizes are
//		written as 32-bit fields

class ByteBuffer {

public:
	using String = std::string;
	enum class Error : uint32_t {
		Success = 0u,
		BufferOverflow,
		ReadWhileWriting,
		WriteWhileReading,
	};

private:
	bool					reading;
	std::vector<uint8_t>	buffer;
	size_t					offset = 0u; // current read/write position
	size_t					size = 0u; // total

	//TODO: add string pool to avoid runtime heap allocations

	void Resize(
		size_t addSize
	);

public:
	// write-only buffer
	ByteBuffer();

	// read-only buffer
	ByteBuffer(
		const void *data,
		size_t dataLen
	);

	// get a pointer to buffer memory
	//NOTE: invalidated after any future writes
	const void *GetMemory(
		size_t *outLen,
		bool encode = false // RLE
	) const XS_WARN_UNUSED_RESULT;

	// string representation of error codes
	static const char *ErrorToString(
		Error error
	);

	// debug: dump buffer contents to disk
	void DumpToFile(
		const char *path
	) const;

	Error Skip(
		size_t count
	) XS_WARN_UNUSED_RESULT;

	// append a raw buffer
	// dataLen: size in bytes
	Error WriteRaw(
		const void *data,
		size_t dataLen
	) XS_WARN_UNUSED_RESULT;

	// write a primitive type
	template<typename T>
	Error Write(
		T data
	) //XS_WARN_UNUSED_RESULT
	{
		if ( reading ) {
			return Error::WriteWhileReading;
		}
		const size_t writeSize = sizeof(T);
		if ( offset + writeSize > size ) {
			// nop: not actually an error, we'll just expand the buffer accordingly
		}

		Resize( writeSize );
		*reinterpret_cast<T *>( &buffer[offset] ) = data;

		offset += writeSize;

		return Error::Success;
	}

	//NOTE: see disclaimer at top of class regarding storage mechanism of strings
	Error WriteString(
		const char *str
	) XS_WARN_UNUSED_RESULT;

	// read a raw buffer
	Error ReadRaw(
		void *outData,
		size_t dataLen
	) XS_WARN_UNUSED_RESULT;

	// read a primitive type
	template<typename T>
	Error Read(
		T *outData
	) //XS_WARN_UNUSED_RESULT
	{
		const size_t addSize = sizeof(T);
		if ( !reading ) {
			return Error::ReadWhileWriting;
		}
		if ( offset + addSize > size ) {
			return Error::BufferOverflow;
		}

		*outData = *reinterpret_cast<T *>( &buffer[offset] );
		offset += addSize;

		return Error::Success;
	}

	//NOTE: see disclaimer at top of class regarding storage mechanism of strings
	Error ReadString(
		String &out
	) XS_WARN_UNUSED_RESULT;

	//NOTE: see disclaimer at top of class regarding storage mechanism of strings
	//NOTE: caller must delete[] the string contents
	Error ReadString(
		const char ** outStr,
		uint32_t *outLen = nullptr // sometimes we don't care about the size
	) XS_WARN_UNUSED_RESULT;

};
