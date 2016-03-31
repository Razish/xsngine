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
	//		const File f( "path/to.file", FileMode::Read );
	//		if ( !f.open ) {
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
	//		if ( !f.open ) {
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
		// upon failure, reset all values to defaults in-case someone tries to use a dangling or invalid file handle
		void Clear(
			void
		);

		FILE		*file = nullptr;
		//size_t		 cursor = 0u; // used for binary append
		FileMode	 mode;

	public:
		long	length = 0;
		bool	open = false;
		char	path[FILENAME_MAX]; //TODO: XS_MAX_FILENAME

		// initialise the filesystem
		// any file operations requested before this is called will fail
		static void Init(
			void
		);

		// set the virtual root directory for the game to read/write files
		static void SetBasePath(
			void
		);

		// resolve a relative game path into a full OS path
		static bool GetFullPath(
			const char *gamePath,
			char *outPath,
			size_t outLen
		);

		// retrieve the file extension for the given filename
		// upon failure, an empty string is written and the return value is false
		static bool GetExtension(
			const char *gamePath,
			char *outExtension,
			size_t outLen
		);

		// replace path separators with the native separators for your OS/FS
		static void ReplaceSeparators(
			char *path
		);

		// create the folders necessary to store the specified file
		static bool CreatePath(
			const char *fullPath
		);

		// don't allow default instantiation
		File() = delete;
		File( const File& ) = delete;
		File& operator=( const File& ) = delete;

		// open a file for reading/writing
		File(
			const char *gamePath,
			FileMode fileMode = FileMode::Read
		);

		~File();

		// read len bytes into buf
		// if len is 0 or not specified, read the entirety of the file
		void Read(
			uint8_t *buf,
			size_t len = 0U
		) const;

		// append a string to the end of the file
		// must be used on a file opened with APPEND mode
		void AppendString(
			const char *str
		) const;

		// write len bytes from buf into file
		// if append is false, file will be truncated
		//FIXME: implement binary append
		void Write(
			const void *buf,
			size_t len
			//bool append = false
		);
	};

} // namespace XS
