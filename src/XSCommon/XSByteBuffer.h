#pragma once

#include <vector>

namespace XS {

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
	private:
		bool					reading;
		std::vector<uint8_t>	buffer;
		size_t					offset = 0u; // current read/write position
		size_t					size = 0u; // total

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
			size_t *outLen
		) const;

		// write generic data to the buffer
		void WriteGeneric(
			const void *data,
			size_t dataLen
		);

		// write a signed 32 bit integer to the buffer
		void WriteInt32(
			int32_t data
		);

		// write an unsigned 32 bit integer to the buffer
		void WriteUInt32(
			uint32_t data
		);

		// write a signed 16 bit integer to the buffer
		void WriteInt16(
			int16_t data
		);

		// write an unsigned 16 bit integer to the buffer
		void WriteUInt16(
			uint16_t data
		);

		// write a signed 8 bit integer to the buffer
		void WriteInt8(
			int8_t data
		);

		// write an unsigned 8 bit integer to the buffer
		void WriteUInt8(
			uint8_t data
		);

		// write a 64-bit floating point value to the buffer
		void WriteReal64(
			real64_t data
		);

		// write a 32-bit floating point value to the buffer
		void WriteReal32(
			real32_t data
		);

		//NOTE: see disclaimer at top of class regarding storage mechanism of strings
		void WriteString(
			const char *str
		);

		// write generic data to the buffer
		void ReadGeneric(
			void *outData,
			size_t dataLen
		);

		// write a signed 32 bit integer to the buffer
		void ReadInt32(
			int32_t *outData
		);

		// write an unsigned 32 bit integer to the buffer
		void ReadUInt32(
			uint32_t *outData
		);

		// write a signed 16 bit integer to the buffer
		void ReadInt16(
			int16_t *outData
		);

		// write an unsigned 16 bit integer to the buffer
		void ReadUInt16(
			uint16_t *outData
		);

		// write a signed 8 bit integer to the buffer
		void ReadInt8(
			int8_t *outData
		);

		// write an unsigned 8 bit integer to the buffer
		void ReadUInt8(
			uint8_t *outData
		);

		// write a 64-bit floating point value to the buffer
		void ReadReal64(
			real64_t *outData
		);

		// write a 32-bit floating point value to the buffer
		void ReadReal32(
			real32_t *outData
		);

		//NOTE: see disclaimer at top of class regarding storage mechanism of strings
		//NOTE: caller must free the string contents
		void ReadString(
			const char ** outStr,
			uint32_t *outLen = nullptr
		);
	};

} // namespace XS
