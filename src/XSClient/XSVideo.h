#pragma once

#include "XSCommon/XSCvar.h"

#include <SDL2/SDL.h>

namespace XS {

	namespace Client {

		extern class Video {
		private:
			SDL_Window *displayWindow;
			SDL_Renderer *displayRenderer;
			SDL_RendererInfo displayRendererInfo;
	
			Cvar *vid_width,
				*vid_height,
				*vid_noBorder,
				*r_swapInterval,
				*r_fov;
	
			void RegisterCvars( void );
			void CreateDisplay( void );
			void InitGL( void );
			void SetViewport( const int width, const int height );
	
		public:
			Video(){};
	
			void Init( void );
			void Update( void );
		} video;

	}; // namespace Client

}; // namespace XS
