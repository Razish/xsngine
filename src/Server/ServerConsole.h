#pragma once

#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)
#include <termios.h>
#endif

namespace Server {

	extern class ServerConsole {

	private:
		static const char *promptText; // = "tty] ";
	#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)
		static int ec_erase;
		static int ec_eof;
	#endif

		struct {

			char buffer[1024];
			size_t head = 0u;

		} input;

	#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)
		struct termios tc;
	#elif defined(XS_OS_WINDOWS)
		void	*hInput;
		void	*hOutput;
		void	*hError;
	#endif

		void Open(
			void
		);

		void FlushInput(
			void
		) const;

		void Backspace(
			void
		) const;

	#if defined(XS_OS_WINDOWS)
		void Show(
			void
		);
	#endif

		void Close(
			void
		);

	public:
		ServerConsole();

		const char *Read(
			void
		) XS_WARN_UNUSED_RESULT;

		~ServerConsole();

	} *serverConsole;

} // namespace Server
