#pragma once

namespace XS {

	//
	// XS Filesystem layer
	//
	//	All files are internally treated as binary files to avoid line ending conversions (i.e. \n -> \r\n)
	//	When using non-binary file modes (e.g. File::READ) the file will be treated as text and space will be
	//		reserved for null terminator (i.e. file.length will be increased, file.read() will add null
	//		terminator at buffer[length-1]
	//
	//	The only acceptable path separator is '/' for consistency between platforms
	//
	//	There are currently no case sensitivity rules, but lower-case is strongly recommend and may be
	//		enforced later
	//
	//	You may only specify RELATIVE paths, not full system paths or network paths.
	//	com_path will be prepended to any file path
	//	TODO: restrict accessing files outside of com_path, including absolute paths and directory traversal
	//
	//	Example of file reading:
	//		File f = File::Open( 'path/to.file', File::READ );
	//		char *buffer = new char[file.length];
	//			f.Read( buffer, file.length );	// second argument is optional, defaults to file.length
	//			// do things to buffer here
	//		delete[] buffer;
	//

	class File {
	public:

		enum Mode {
			READ=0,
			READ_BINARY,
			WRITE,
			WRITE_BINARY,
			APPEND,
			NUM_MODES
		};

	private:
		File(); // can not instantiate with default constructor

		static Cvar *com_path;

		FILE *file;
		Mode mode;

	public:
		long length;
		char path[FILENAME_MAX];

		static void Init( void );
		static void GetPath( const char *gamePath, char *outPath, size_t outLen );

		File( const char *gamePath, Mode mode = READ );
		~File();
		void Read( byte *buf, size_t len = 0U );
		void AppendString( const char *str );
	};

} // namespace XS
