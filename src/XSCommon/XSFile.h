#pragma once

namespace XS {

	class File {
	public:

		enum Mode {
			READ=0,
			WRITE,
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
