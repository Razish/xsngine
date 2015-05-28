#pragma once

#if defined(XS_OS_LINUX) || defined(XS_OS_MAC)
#include <termios.h>
#endif

namespace XS {

	namespace Server {
		extern class ServerConsole {
		private:
			static const char *promptText;// = "tty] ";
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

			void Close(
				void
			);

		public:
			ServerConsole();

			const char *Read(
				void
			);

			~ServerConsole();
		} *serverConsole;

	} // namespace Server

} // namespace XS
