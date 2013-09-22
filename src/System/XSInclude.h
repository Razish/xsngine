#pragma once

// commonly included headers from stl/stdlib

#include <cassert>
#include <cctype>
#include <climits>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#if defined(_WIN32)
	#include <direct.h>
#elif defined(__linux__)
	#include <unistd.h>
#endif

#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#ifdef _MSC_VER
	#pragma warning( error: 4297 ) // 'x' : function assumed not to throw an exception but does
#endif
