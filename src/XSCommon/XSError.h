#pragma once

//
// XS error
//
//	Upon unrecoverable error, throw an XSError to clean up all subsystems
//

namespace XS {

	class XSError {
	private:
		XSError();

	public:
		std::string msg;
		XSError( std::string &msg ) : msg( msg ) {}
		XSError( std::string msg ) : msg( msg ) {}
		XSError( const char *msg ) : msg( msg ) {}
	};

} // namespace XS
