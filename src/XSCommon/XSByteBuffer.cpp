#include "XSCommon/XSCommon.h"
#include "XSCommon/XSByteBuffer.h"

#define BYTE_SWAP

#if !defined(BYTE_SWAP)
#undef htonll
#define htonll(x) x
#undef htonl
#define htonl(x) x
#undef htons
#define htons(x) x

#undef ntohll
#define ntohll(x) x
#undef ntohl
#define ntohl(x) x
#undef ntohs
#define ntohs(x) x
#endif

namespace XS {

	// write-only buffer
	ByteBuffer::ByteBuffer()
	: reading( false )
	{
	}

	// read-only buffer
	ByteBuffer::ByteBuffer( const void *data, size_t dataLen )
	: reading( true )
	{
		Resize( dataLen );
		std::memcpy( &buffer[0], data, dataLen );
	}

	void ByteBuffer::Resize( size_t addSize ) {
		buffer.resize( buffer.size() + addSize, 0 );
		size += addSize;
	}

	const void *ByteBuffer::GetMemory( size_t *outLen, bool encode ) const {
		*outLen = size;
		return static_cast<const void *>( &buffer[0] );
	}

	void ByteBuffer::Skip( size_t count ) {
		SDL_assert( reading );
		offset += count;
		SDL_assert( count < size );
	}

	void ByteBuffer::WriteGeneric( const void *data, size_t dataLen ) {
		SDL_assert( !reading );

		// resize the buffer
		Resize( dataLen );
		std::memcpy( &buffer[offset], data, dataLen );
		offset += dataLen;
	}

	void ByteBuffer::WriteInt64( int64_t data ) {
		SDL_assert( !reading );

		size_t writeSize = sizeof(int64_t);

		Resize( writeSize );
		*reinterpret_cast<int64_t *>( &buffer[offset] ) = htonll( data );

		offset += writeSize;
	}

	void ByteBuffer::WriteUInt64( uint64_t data ) {
		SDL_assert( !reading );

		size_t writeSize = sizeof(uint64_t);

		Resize( writeSize );
		*reinterpret_cast<uint64_t *>( &buffer[offset] ) = htonll( data );

		offset += writeSize;
	}

	void ByteBuffer::WriteInt32( int32_t data ) {
		SDL_assert( !reading );

		size_t writeSize = sizeof(int32_t);

		Resize( writeSize );
		*reinterpret_cast<int32_t *>( &buffer[offset] ) = htonl( data );

		offset += writeSize;
	}

	void ByteBuffer::WriteUInt32( uint32_t data ) {
		SDL_assert( !reading );

		size_t writeSize = sizeof(uint32_t);

		Resize( writeSize );
		*reinterpret_cast<uint32_t *>( &buffer[offset] ) = htonl( data );

		offset += writeSize;
	}

	void ByteBuffer::WriteInt16( int16_t data ) {
		SDL_assert( !reading );

		size_t writeSize = sizeof(int16_t);

		Resize( writeSize );
		*reinterpret_cast<int16_t *>( &buffer[offset] ) = htons( data );

		offset += writeSize;
	}

	void ByteBuffer::WriteUInt16( uint16_t data ) {
		SDL_assert( !reading );

		size_t writeSize = sizeof(uint16_t);

		Resize( writeSize );
		*reinterpret_cast<uint16_t *>( &buffer[offset] ) = htons( data );

		offset += writeSize;
	}

	void ByteBuffer::WriteInt8( int8_t data ) {
		SDL_assert( !reading );

		size_t writeSize = sizeof(int8_t);

		Resize( writeSize );
		*reinterpret_cast<int8_t *>( &buffer[offset] ) = data;

		offset += writeSize;
	}

	void ByteBuffer::WriteUInt8( uint8_t data ) {
		SDL_assert( !reading );

		size_t writeSize = sizeof(uint8_t);

		Resize( writeSize );
		*reinterpret_cast<uint8_t *>( &buffer[offset] ) = data;

		offset += writeSize;
	}

	void ByteBuffer::WriteReal64( real64_t data ) {
		SDL_assert( !reading );

		size_t writeSize = sizeof(real64_t);

		Resize( writeSize );
		*reinterpret_cast<real64_t *>( &buffer[offset] ) = data;

		offset += writeSize;
	}

	void ByteBuffer::WriteReal32( real32_t data ) {
		SDL_assert( !reading );

		size_t writeSize = sizeof(real32_t);

		Resize( writeSize );
		*reinterpret_cast<real32_t *>( &buffer[offset] ) = data;

		offset += writeSize;
	}

	void ByteBuffer::WriteString( const char *str ) {
		SDL_assert( !reading );

		const uint32_t strLen = static_cast<uint32_t>( strlen( str ) );
		WriteUInt32( strLen );
		WriteGeneric( reinterpret_cast<const void *>( str ), strLen );
	}

	void ByteBuffer::ReadGeneric( void *outData, size_t dataLen ) {
		SDL_assert( reading );

		std::memcpy( outData, &buffer[offset], dataLen );
		offset += dataLen;
	}

	void ByteBuffer::ReadInt64( int64_t *outData ) {
		SDL_assert( reading );

		*outData = ntohll( *reinterpret_cast<int64_t *>( &buffer[offset] ) );
		offset += sizeof(int64_t);
	}

	void ByteBuffer::ReadUInt64( uint64_t *outData ) {
		SDL_assert( reading );

		*outData = ntohll( *reinterpret_cast<uint64_t *>( &buffer[offset] ) );
		offset += sizeof(uint64_t);
	}

	void ByteBuffer::ReadInt32( int32_t *outData ) {
		SDL_assert( reading );

		*outData = ntohl( *reinterpret_cast<int32_t *>( &buffer[offset] ) );
		offset += sizeof(int32_t);
	}

	void ByteBuffer::ReadUInt32( uint32_t *outData ) {
		SDL_assert( reading );

		*outData = ntohl( *reinterpret_cast<uint32_t *>( &buffer[offset] ) );
		offset += sizeof(uint32_t);
	}

	void ByteBuffer::ReadInt16( int16_t *outData ) {
		SDL_assert( reading );

		*outData = ntohs( *reinterpret_cast<int16_t *>( &buffer[offset] ) );
		offset += sizeof(int16_t);
	}

	void ByteBuffer::ReadUInt16( uint16_t *outData ) {
		SDL_assert( reading );

		*outData = ntohs( *reinterpret_cast<uint16_t *>( &buffer[offset] ) );
		offset += sizeof(uint16_t);
	}

	void ByteBuffer::ReadInt8( int8_t *outData ) {
		SDL_assert( reading );

		*outData = *reinterpret_cast<int8_t *>( &buffer[offset] );
		offset += sizeof(int8_t);
	}

	void ByteBuffer::ReadUInt8( uint8_t *outData ) {
		SDL_assert( reading );

		*outData = *reinterpret_cast<uint8_t *>( &buffer[offset] );
		offset += sizeof(uint8_t);
	}

	void ByteBuffer::ReadReal64( real64_t *outData ) {
		SDL_assert( reading );

		*outData = *reinterpret_cast<real64_t *>( &buffer[offset] );
		offset += sizeof(real64_t);
	}

	void ByteBuffer::ReadReal32( real32_t *outData ) {
		SDL_assert( reading );

		*outData = *reinterpret_cast<real32_t *>( &buffer[offset] );
		offset += sizeof(real32_t);
	}

	void ByteBuffer::ReadString( const char ** outStr, uint32_t *outLen ) {
		SDL_assert( reading );

		// read the size
		uint32_t len = 0u;
		ReadUInt32( &len );

		// alloc the memory - size + 1 for null terminator
		char *str = new char[len + 1];

		// read the contents
		ReadGeneric( static_cast<void *>( str ), len );

		// add null terminator
		str[len] = '\0';
		*outStr = str;
		if ( outLen ) {
			*outLen = len;
		}
	}

} // namespace XS
