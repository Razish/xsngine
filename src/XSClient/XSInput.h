#pragma once

namespace XS {

	namespace Client {

		// exists in XSClient/XSInput.cpp for now
		extern class Input {
		private:
			std::map<SDL_Keycode, bool> keystate;
			std::map<SDL_Keycode, std::string> bind;

		public:
			Input();
			void Poll( void );
		} input;

	}; // namespace Client

}; // namespace XS
