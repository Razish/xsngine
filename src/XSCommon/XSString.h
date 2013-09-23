#pragma once

namespace XS {

	namespace String {

		void						 Concatenate( char *dst, size_t len, const char *src );
		void						 Copy		( char *dst, const char *src, size_t len );
		std::string					 Format		( const std::string fmt, ... );
		std::vector<std::string>	&Split		( const std::string &s, char delim, std::vector<std::string> &elems );
		std::vector<std::string>	 Split		( const std::string &s, char delim );
		std::string					 Join		( const std::vector<std::string> &strings, const std::string &separator );

	}; // namespace String

}; // namespace XS
