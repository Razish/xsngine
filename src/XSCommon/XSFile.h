#pragma once

namespace XS {

	//
	// XS Filesystem layer
	//
	//	All files are internally treated as binary files to avoid line ending conversions (i.e. \n -> \r\n)
	//	When using non-binary file modes (e.g. FM_READ) the file will be treated as text and space will be reserved for
	//	null terminator (i.e. file.length will be increased, file.read() will add null terminator at buffer[length-1]
	//
	//	The only acceptable path separator is '/' for consistency between platforms
	//
	//	There are currently no case sensitivity rules, but lower-case is strongly recommend and may be enforced later
	//
	//	You may only specify RELATIVE paths, not full system paths or network paths.
	//	com_path will be prepended to any file path
	//	TODO: restrict accessing files outside of com_path, including absolute paths and directory traversal
	//
	//	Example of reading a text file:
	//		const File f( "path/to.file", FM_READ );
	//		if ( !f.open )
	//			return;
	//		char *buffer = new char[f.length];
	//			f.Read( (byte *)buffer, f.length );	// second argument is optional, defaults to file.length
	//			// do things to buffer here
	//		delete[] buffer;
	//
	//	Example of writing a text file:
	//		const File f( "path/to.file", FM_WRITE );
	//		if ( !f.open )
	//			return;
	//		f.AppendString( "Herp derp\n" );
	//

	enum fileMode_t {
		FM_READ=0,
		FM_READ_BINARY,
		FM_WRITE,
		FM_WRITE_BINARY,
		FM_APPEND,
		FM_NUM_MODES
	};

	class File {
	private:
		File(); // can not instantiate with default constructor

		FILE *file;
		fileMode_t mode;

	public:
		long length;
		bool open;
		char path[FILENAME_MAX];

		static void Init( void );
		static void GetPath( const char *gamePath, char *outPath, size_t outLen );

		File( const char *gamePath, fileMode_t mode = FM_READ );
		~File();
		void Read( byte *buf, size_t len = 0U ) const;
		void AppendString( const char *str ) const;
	};

} // namespace XS
