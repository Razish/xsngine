#include <map>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSByteBuffer.h"

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

	static const std::map<ByteBuffer::Error, const char *> errorToString {
		{ ByteBuffer::Error::Success,			"success" },
		{ ByteBuffer::Error::BufferOverflow,	"buffer overflow" },
		{ ByteBuffer::Error::ReadWhileWriting,	"read while writing" },
		{ ByteBuffer::Error::WriteWhileReading,	"write while reading" }
	};

	const char *ByteBuffer::ErrorToString( ByteBuffer::Error error ) {
		const auto &it = errorToString.find( error);
		if ( it == errorToString.end() ) {
			return "Unknown";
		}
		else {
			return it->second;
		}
	}

	void ByteBuffer::DumpToFile( const char *path ) const {
		File f( path, FileMode::Write );
		if ( !f.open ) {
			return;
		}

		//TODO: hex visualisation?
		f.Write( static_cast<const void *>( &buffer[0] ), size );
	}


	ByteBuffer::Error ByteBuffer::Skip( size_t count ) {
		//TODO: should we allow skipping / writing null bytes?
		// waiting for users to request it to see if there's any interest in this functionality
		SDL_assert( reading );

		if ( offset + count > size ) {
			return Error::BufferOverflow;
		}

		offset += count;

		return Error::Success;
	}

	ByteBuffer::Error ByteBuffer::WriteRaw( const void *data, size_t dataLen ) {
		if ( reading ) {
			return Error::WriteWhileReading;
		}
		if ( offset + dataLen > size ) {
			// nop: not actually an error, we'll just expand the buffer accordingly
		}

		Resize( dataLen );
		std::memcpy( &buffer[offset], data, dataLen );

		offset += dataLen;

		return Error::Success;
	}

	ByteBuffer::Error ByteBuffer::WriteString( const char *str ) {
		if ( reading ) {
			return Error::WriteWhileReading;
		}

		// write it out pascal style
		// 4 bytes for length
		// n bytes for contents (no null terminator)
		const uint32_t strLen = static_cast<uint32_t>( strlen( str ) );
		Error status;
		if ( (status = Write<uint32_t>( strLen )) != Error::Success ) {
			return status;
		}
		status = WriteRaw( str, strLen );
		if ( status != Error::Success ) {
			return status;
		}

		return Error::Success;
	}

	ByteBuffer::Error ByteBuffer::ReadRaw( void *outData, size_t dataLen ) {
		if ( !reading ) {
			return Error::ReadWhileWriting;
		}
		if ( offset + dataLen > size ) {
			return Error::BufferOverflow;
		}

		std::memcpy( outData, &buffer[offset], dataLen );
		offset += dataLen;

		return Error::Success;
	}

	ByteBuffer::Error ByteBuffer::ReadString( ByteBuffer::String &out ) {
		if ( !reading ) {
			return Error::ReadWhileWriting;
		}

		// read the size
		uint32_t length;
		Error tmp;
		if ( (tmp = Read<uint32_t>( &length )) != Error::Success ) {
			return tmp;
		}

		// alloc the memory - size + 1 for null terminator
		out.reserve( length );
		// this also updates the std::string's size for when not accessing the memory directly
		out.resize( length );

		// read the contents
		if ( (tmp = ReadRaw( &out[0], length )) != Error::Success ) {
			return tmp;
		}

		// add null terminator
		out[length] = '\0';

		return Error::Success;
	}

	ByteBuffer::Error ByteBuffer::ReadString( const char ** outStr, uint32_t *outLen ) {
		if ( !reading ) {
			return Error::ReadWhileWriting;
		}

		// read the size
		uint32_t len = 0u;
		// read the size
		uint32_t length;
		Error tmp;
		if ( (tmp = Read<uint32_t>( &length )) != Error::Success ) {
			return tmp;
		}

		// alloc the memory - size + 1 for null terminator
		char *str = new char[len + 1];

		// read the contents
		if ( (tmp = ReadRaw( str, len )) != Error::Success ) {
			return tmp;
		}

		// add null terminator
		str[len] = '\0';
		*outStr = str;
		if ( outLen ) {
			*outLen = len;
		}

		return Error::Success;
	}

} // namespace XS
