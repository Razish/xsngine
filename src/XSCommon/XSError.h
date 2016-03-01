#pragma once

#include <algorithm>
#include <string>

// upon unrecoverable error, throw an XSError to clean up all subsystems

namespace XS {

	struct XSError : public std::exception {

	protected:
		std::string		msg;

	public:
		bool			intended;

		XSError& operator=( const XSError& ) = delete;

		XSError( std::string &errorMsg )
		: msg( errorMsg ), intended( false )
		{
		}

		XSError( const char *errorMsg )
		: msg( errorMsg ), intended( false )
		{
		}

		// should only be used if shutdown was desired
		XSError()
		: msg( "generic shutdown" ), intended( true )
		{
		}

		virtual const char *what() const noexcept {
			return msg.c_str();
		}

	};

} // namespace XS
