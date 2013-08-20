#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <stdarg.h>

namespace XS {

	std::string					 Format	( const std::string fmt, ... );
	std::vector<std::string>	&split	( const std::string &s, char delim, std::vector<std::string> &elems );
	std::vector<std::string>	 split	( const std::string &s, char delim );

}; // namespace XS
