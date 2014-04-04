#pragma once

//
// XS error
//
//	Upon unrecoverable error, throw an XSError to clean up all subsystems
//

namespace XS {

	class XSError : public std::exception {
	private:
		XSError();

	public:
		XSError( std::string &msg ) : msg( msg ) {}
		XSError( const char *msg ) : msg( msg ) {}
		virtual const char* what() const throw () {
			return msg.c_str();
		}

	protected:
		std::string msg;
	};

} // namespace XS
