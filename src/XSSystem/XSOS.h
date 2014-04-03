#pragma once

namespace XS {

	namespace OS {

		void GetCurrentWorkingDirectory( char *cwd, size_t bufferLen );
		bool MkDir( const char *path ); // returns true if file exists
		bool Stat( const char *path );

	} // namespace OS

} // namespace XS
