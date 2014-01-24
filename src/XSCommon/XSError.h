#pragma once

//
// XS error
//
//	Upon unrecoverable error, throw an XSError to clean up all subsystems
//

namespace XS {

	class XSError {
	private:
		XSError(){}

	public:
		XSError( std::string &msg ) { this->msg = msg; }
		XSError( std::string msg ) { this->msg = msg; }
		XSError( const char *msg ) { this->msg = msg; }

		std::string msg;
	};

} // namespace XS
