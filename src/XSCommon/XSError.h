#pragma once

#include <algorithm>
#include <string>

// upon unrecoverable error, throw an XSError to clean up all subsystems

namespace XS {

	struct XSError : public std::exception {
	protected:
		std::string		msg;

	public:
		XSError( std::string &msg )
		: msg( msg )
		{
		}

		XSError( const char *msg )
		: msg( msg )
		{
		}

		virtual const char *what() const throw () {
			return msg.c_str();
		}

		// don't allow default instantiation
		XSError() = delete;
		XSError& operator=( const XSError& ) = delete;
	};

} // namespace XS
