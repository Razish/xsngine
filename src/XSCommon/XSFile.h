#pragma once

namespace XS {

	//
	// XS Filesystem layer
	//
	//	All files are internally treated as binary files to avoid line ending conversions (i.e. \n -> \r\n)
	//	When using non-binary file modes (e.g. FileMode::READ) the file will be treated as text and space will be reserved
	//	for null terminator (i.e. file.length will be increased, file.read() will add null terminator at buffer[length-1]
	//
	//	All requests to files will automatically have their path separators replaced to that of the host OS
	//
	//	There are currently no case sensitivity rules, but lower-case is strongly recommend and may be enforced later
	//
	//	You may only specify relative paths, not full system paths or network paths
	//	com_path will be prepended to any file path
	//	com_path may only be set during initialisation
	//
	//	Example of reading a text file:
	//		const File f( "path/to.file", FileMode::READ );
	//		if ( !f.open )
	//			return;
	//		char *buffer = new char[f.length];
	//			f.Read( (byte *)buffer, f.length );	// second argument is optional, defaults to file.length
	//			// do things to buffer here
	//		delete[] buffer;
	//
	//	Example of writing a text file:
	//		const File f( "path/to.file", FileMode::WRITE );
	//		if ( !f.open )
	//			return;
	//		f.AppendString( "Herp derp\n" );
	//

	enum FileMode {
		READ = 0,
		READ_BINARY,
		WRITE,
		WRITE_BINARY,
		APPEND,
		NUM_MODES
	};

	class File {
	private:
		File();

		void Clear( void );

		FILE *file;
		FileMode mode;

	public:
		long length;
		bool open;
		char path[FILENAME_MAX];

		static void Init( void );
		static void SetBasePath( void );
		static bool GetFullPath( const char *gamePath, char *outPath, size_t outLen );
		static void ReplaceSeparators( char *path );

		File( const char *gamePath, FileMode mode = FileMode::READ );
		~File();
		void Read( byte *buf, size_t len = 0U ) const;
		void AppendString( const char *str ) const;
	};

} // namespace XS
