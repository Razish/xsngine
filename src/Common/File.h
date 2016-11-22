#pragma once

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
//		const File f( "path/to.file", FileMode::Read );
//		if ( !f.isOpen ) {
//			return;
//		}
//		char *buffer = new char[f.length];
//			f.Read( reinterpret_cast<uint8_t *>( buffer ), f.length );	// second argument is optional, defaults to
//																		//	file.length
//			// do things to buffer here
//		delete[] buffer;
//
//	Example of writing a text file:
//		const File f( "path/to.file", FileMode::Write );
//		if ( !f.isOpen ) {
//			return;
//		}
//		f.AppendString( "Herp derp\n" );
//

enum class FileMode {
	Read = 0,
	ReadBinary,
	Write,
	WriteBinary,
	Append,
	NUM_MODES
};

class File {

private:
	FILE		*file;
	//size_t		 cursor = 0u; // used for binary append
	FileMode	 mode;
	bool		 privateIsOpen;

	// upon failure, reset all values to defaults in-case someone tries to use a dangling or invalid file handle
	void Clear(
		void
	);

public:
	long		 length = 0;
	const bool	&isOpen;
	char		 path[FILENAME_MAX]; //TODO: XS_MAX_FILENAME

	// don't allow default instantiation
	File() = delete;
	File( const File& ) = delete;
	File& operator=( const File& ) = delete;

	// opens a file ready for read/write
	// upon failure, file.open will be false and file.length will be 0
	File(
		const char *gamePath,
		FileMode fileMode = FileMode::Read
	);

	~File();

	// append a string to the end of the file
	// must be used on a file opened with APPEND mode
	void AppendString(
		const char *str
	) const;

	// read len bytes into buf
	// if len is 0 or not specified, read the entirety of the file
	void Read(
		uint8_t *buf,
		size_t len = 0U
	) const;

	// write len bytes from buf into file
	// if append is false, file will be truncated
	//FIXME: implement binary append
	void Write(
		const void *buf,
		size_t len
		//bool append = false
	);

	// create the folders necessary to store the specified file
	static bool CreatePath(
		const char *fullPath
	);

	// return the extension for the specified relative gamepath
	//	e.g. "obj" for "models/box.obj"
	// upon failure, an empty string is written and the return value is false
	static bool GetExtension(
		const char *gamePath,
		char *outExtension,
		size_t outLen
	);

	// return the full OS path for the specified relative gamepath
	//	e.g. "C:\xsngine\textures\black.jpg" for "textures/black.jpg"
	static bool GetFullPath(
		const char *gamePath,
		char *outPath,
		size_t outLen
	);

	// initialise the filesystem
	// any file operations requested before this is called will fail
	static void Init(
		void
	);

	// normalise path separators for the current platform
	static void ReplaceSeparators(
		char *path
	);

	// set the virtual root directory for the game to read/write files
	static void SetBasePath(
		void
	);

};
