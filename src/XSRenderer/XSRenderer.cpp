#include <SDL2/SDL.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSError.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSGlobals.h"
#include "XSCommon/XSTimer.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSRenderCommand.h"
#include "XSRenderer/XSBackend.h"
#include "XSRenderer/XSFramebuffer.h"
#include "XSRenderer/XSShaderProgram.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSFont.h"
#include "XSRenderer/XSImagePNG.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSVertexAttributes.h"
#include "XSRenderer/XSMaterial.h"

#if defined(XS_OS_MAC)
//#include <OpenCL/cl_gl_ext.h>
#endif

namespace XS {

	namespace Renderer {

		RendererState state = {};

		static SDL_Window *window = nullptr;
		static SDL_GLContext context;

		static Cvar *r_clear = nullptr;
		static Cvar *r_debug = nullptr;
		static Cvar *r_multisample = nullptr;
		static Cvar *r_skipRender = nullptr;
		static Cvar *r_swapInterval = nullptr;
		static Cvar *vid_height = nullptr;
		static Cvar *vid_noBorder = nullptr;
		static Cvar *vid_width = nullptr;

		std::vector<View *> views;
		static View *currentView = nullptr;

		static ShaderProgram *compositeShader = nullptr;
		static ShaderProgram *lightAmbientShader = nullptr;

#if 1
		static const char *DEBUG_GLErrorSeverityToString( GLenum severity ) {
			switch ( severity ) {
			case GL_DEBUG_SEVERITY_HIGH_ARB: {
				return "High";
			} break;

			case GL_DEBUG_SEVERITY_MEDIUM_ARB: {
				return "Medium";
			} break;

			case GL_DEBUG_SEVERITY_LOW_ARB: {
				return "Low";
			} break;

			default: {
				return "?";
			} break;
			}
		}

		static const char *DEBUG_GLErrorSourceToString( GLenum source ) {
			switch ( source ) {
			case GL_DEBUG_SOURCE_API_ARB: {
				return "API";
			} break;

			case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: {
				return "WS";
			} break;

			case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: {
				return "GLSL";
			} break;

			case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: {
				return "3rd";
			} break;

			case GL_DEBUG_SOURCE_APPLICATION_ARB: {
				return "App";
			} break;

			case GL_DEBUG_SOURCE_OTHER_ARB: {
				return "Other";
			} break;

			default: {
				return "?";
			} break;
			}
		}

		static const char *DEBUG_GLErrorTypeToString( GLenum type ) {
			switch ( type ) {
			case GL_DEBUG_TYPE_ERROR_ARB: {
				return "Error";
			} break;

			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: {
				return "Deprecated";
			} break;

			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: {
				return "UB";
			} break;

			case GL_DEBUG_TYPE_PORTABILITY_ARB: {
				return "Portability";
			} break;

			case GL_DEBUG_TYPE_PERFORMANCE_ARB: {
				return "Performance";
			} break;

			case GL_DEBUG_TYPE_OTHER_ARB: {
				return "Other";
			} break;

			default: {
				return "?";
			} break;
			}
		}

		static void OnGLError( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
			const GLchar *message, GLvoid *userParam )
		{
			const int level = r_debug->GetInt32();
			if ( !level || (level == 1 && type == GL_DEBUG_TYPE_PERFORMANCE_ARB) ) {
				return;
			}

			console.Print( PrintLevel::Normal,
				"[%s] [%s] %s: %s\n",
				DEBUG_GLErrorSeverityToString( severity ),
				DEBUG_GLErrorSourceToString( source ),
				DEBUG_GLErrorTypeToString( type ),
				message
			);
		}
#endif

		void Init( void ) {
			state.valid = true;

			RegisterCvars();

			CreateDisplay();

			glewExperimental = GL_TRUE;
			GLenum error = glewInit();
			if ( error != GLEW_OK ) {
				throw( XSError( String::Format( "Failed to initialise GLEW: %s\n",
					glewGetErrorString( error ) ).c_str() ) );
			}

#if 1
			// this causes a driver bug on Nvidia GTX 680 driver version 350.12 targeting OpenGL 3.1 core profile
			if ( GLEW_ARB_debug_output ) {
				glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB );
				glDebugMessageCallbackARB( OnGLError, nullptr );
			}
#endif

			Backend::Init();

			Texture::Init();

			ShaderProgram::Init();

			// create light ambient shader
			static const VertexAttribute lightAmbientAttributes[] = {
				{ 0, "in_position" },
				{ 1, "in_texCoord" },
				{ 2, "in_colour" },
			};
			lightAmbientShader = new ShaderProgram(
				"lightAmbient",
				"lightAmbient",
				lightAmbientAttributes,
				ARRAY_LEN( lightAmbientAttributes )
			);

			// create composite shader
			static const VertexAttribute compositeAttributes[] = {
				{ 0, "in_position" },
				{ 1, "in_texCoord" },
				{ 2, "in_colour" },
			};
			compositeShader = new ShaderProgram(
				"composite",
				"composite",
				compositeAttributes,
				ARRAY_LEN( compositeAttributes )
			);

			Font::Init();

			RenderCommand::Init();

			glViewport( 0, 0, state.window.width, state.window.height );
		}

		void Shutdown( void ) {
			console.Print( PrintLevel::Normal, "Shutting down renderer...\n" );

			RenderCommand::Shutdown();
			Font::Shutdown();
			Backend::Shutdown();

			DestroyDisplay();
		}

		void RegisterCvars( void ) {
			r_clear = Cvar::Create( "r_clear", "0.1607 0.1921 0.2039", "Colour of the backbuffer", CVAR_ARCHIVE );
			r_debug = Cvar::Create( "r_debug", "0", "Enable debugging information", CVAR_ARCHIVE );
			r_multisample = Cvar::Create( "r_multisample", "2", "Multisample Anti-Aliasing (MSAA) level",
				CVAR_ARCHIVE );
			r_skipRender = Cvar::Create( "r_skipRender", "0", "1 - skip 3D views, 2 - skip 2D views, 3 - skip all "
				"views", CVAR_ARCHIVE );
			r_swapInterval = Cvar::Create( "r_swapInterval", "0", "Enable vertical sync", CVAR_ARCHIVE );
			vid_height = Cvar::Create( "vid_height", "720", "Window height", CVAR_ARCHIVE );
			vid_noBorder = Cvar::Create( "vid_noBorder", "0", "Disable window border", CVAR_ARCHIVE );
			vid_width = Cvar::Create( "vid_width", "1280", "Window width", CVAR_ARCHIVE );
		}

		void CreateDisplay( void ) {
			if ( SDL_Init( SDL_INIT_VIDEO ) != 0 ) {
				return;
			}

			// targeting OpenGL 3.1 core
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

			// forward-compatible, debug context
			uint32_t contextFlags = SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
			if ( Common::com_developer->GetBool() ) {
				contextFlags |= SDL_GL_CONTEXT_DEBUG_FLAG;
			}
			SDL_GL_SetAttribute( SDL_GL_CONTEXT_FLAGS, contextFlags );

			//FIXME: no MSAA for deferred rendering
			int multisample = r_multisample->GetInt32();
			if ( multisample > 0 ) {
				SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
				//TODO: find the highest significant bit to ensure samples^2
				SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, multisample );
			}
			else {
				SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 0 );
			}

			// create the window
			Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI;
			if ( vid_noBorder->GetBool() ) {
				windowFlags |= SDL_WINDOW_BORDERLESS;
			}

			const int32_t width = vid_width->GetInt32();
			const int32_t height = vid_height->GetInt32();
			window = SDL_CreateWindow(
				WINDOW_TITLE,
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				width, height,
				windowFlags
			);
			SDL_assert( window && "Failed to create window" );

			context = SDL_GL_CreateContext( window );
			SDL_assert( context && "Failed to create OpenGL context on window" );

			SDL_GL_MakeCurrent( window, context );
			state.window.valid = true;
			state.window.width = static_cast<uint32_t>( width );
			state.window.height = static_cast<uint32_t>( height );
			state.window.aspectRatio = vid_width->GetReal64() / vid_height->GetReal64();

			SDL_GL_SetSwapInterval( r_swapInterval->GetInt32() );
		#if defined(XS_OS_MAC)
			//TODO: force vsync flag in CGL, seems to only have an Obj-C API?
			/*
			CGLContextObj cglContext = CGLGetCurrentContext();
			if ( cglContext ) {
				// ...
			}
			*/
		#endif

			state.driver.vendor = reinterpret_cast<const char *>( glGetString( GL_VENDOR ) );
			state.driver.renderer = reinterpret_cast<const char *>( glGetString( GL_RENDERER ) );
			state.driver.coreVersion = reinterpret_cast<const char *>( glGetString( GL_VERSION ) );
			state.driver.shaderVersion = reinterpret_cast<const char *>( glGetString( GL_SHADING_LANGUAGE_VERSION ) );

			console.Print( PrintLevel::Normal,
				"OpenGL device: %s %s\n",
				state.driver.vendor,
				state.driver.renderer
			);
			console.Print( PrintLevel::Normal,
				"OpenGL version: %s with GLSL %s\n",
				state.driver.coreVersion,
				state.driver.shaderVersion
			);
		}

		void DestroyDisplay( void ) {
			SDL_GL_DeleteContext( context );
			context = nullptr;

			SDL_DestroyWindow( window );
			window = nullptr;
			state.window = {};

			SDL_Quit();
		}

		void Update( real64_t dt ) {
			const vector4 clearColour(
				r_clear->GetReal32( 0 ),
				r_clear->GetReal32( 1 ),
				r_clear->GetReal32( 2 ),
				1.0f
			);

			Backend::SetBlendFunction( Backend::BlendFunc::SourceAlpha, Backend::BlendFunc::OneMinusSourceAlpha );

			for ( const auto &view : views ) {
				if ( r_skipRender->GetInt32() & (1 << static_cast<uint32_t>( view->is2D )) ) {
					continue;
				}

				// bind the view's FBO
				view->Bind();
				view->fbo->ToggleDeferredMRT( true, true );

				// set up information needed for the view (e.g. camera projection)
				view->PreRender( dt );

				// geometry pass
				Backend::ClearBuffer( true, true, clearColour );
				while ( !view->renderCommands.empty() ) {
					const auto &cmd = view->renderCommands.front();

					cmd.Execute();

					view->renderCommands.pop();
				}

				// lighting pass
				Material lightingMaterial = {};
				lightingMaterial.shaderProgram = lightAmbientShader;

				Material::SamplerBinding diffuseBinding(
					const_cast<Texture *>( view->fbo->colourTextures[View::RenderTarget::Diffuse] ),
					"u_diffuseTexture",
					0
				);
				lightingMaterial.samplerBindings.push_back( diffuseBinding );

				Material::SamplerBinding normalBinding(
					const_cast<Texture *>( view->fbo->colourTextures[View::RenderTarget::Normal] ),
					"u_normalTexture",
					1
				);
				lightingMaterial.samplerBindings.push_back( normalBinding );

				Material::SamplerBinding positionBinding(
					const_cast<Texture *>( view->fbo->colourTextures[View::RenderTarget::Position] ),
					"u_positionTexture",
					2
				);
				lightingMaterial.samplerBindings.push_back( positionBinding );

				Material::SamplerBinding texcoordBinding(
					const_cast<Texture *>( view->fbo->colourTextures[View::RenderTarget::TexCoord] ),
					"u_texcoordTexture",
					3
				);
				lightingMaterial.samplerBindings.push_back( texcoordBinding );

				view->fbo->ToggleDeferredMRT( true, false );

				DrawQuadCommand cmd = {};
					cmd.x = -1.0f;
					cmd.y = 1.0f;
					cmd.s1 = 0.0f;
					cmd.t1 = 1.0f;
					cmd.s2 = 1.0f;
					cmd.t2 = 0.0f;
					cmd.w = 2.0f;
					cmd.h = -2.0f;
					cmd.material = &lightingMaterial;
					cmd.colour = nullptr;
				DrawQuad( cmd );

				view->fbo->ToggleDeferredMRT( false, false );

				// post processing
				view->PostRender( dt );
			}

			// composite pass
			// render all views to the backbuffer
			Framebuffer::BindDefault();
			Backend::ClearBuffer( true, true, clearColour );
			Backend::SetBlendFunction( Backend::BlendFunc::One, Backend::BlendFunc::One );

			for ( const auto &view : views ) {
				Material compositeMaterial = {};
				compositeMaterial.shaderProgram = compositeShader;

				Material::SamplerBinding colourBinding(
					view->is2D
						? const_cast<Texture *>( view->fbo->colourTextures[0] )
						: const_cast<Texture *>( view->fbo->colourTextures[View::RenderTarget::Lighting] ),
					"u_viewTexture",
					0
				);
				compositeMaterial.samplerBindings.push_back( colourBinding );

				DrawQuadCommand cmd = {};
					cmd.x = -1.0f;
					cmd.y = 1.0f;
					cmd.s1 = 0.0f;
					cmd.t1 = 1.0f;
					cmd.s2 = 1.0f;
					cmd.t2 = 0.0f;
					cmd.w = 2.0f;
					cmd.h = -2.0f;
					cmd.material = &compositeMaterial;
					cmd.colour = nullptr;
				DrawQuad( cmd );
			}

			SDL_GL_SwapWindow( window );
		}

		void RegisterView( View *view ) {
			views.push_back( view );
		}

		void SetCurrentView( View *view ) {
			currentView = view;
		}

		const View *GetCurrentView( void ) {
			return currentView;
		}

		static void AssertView( void ) {
			if ( !currentView ) {
				throw( XSError( "Attempted to issue render command without binding a view" ) );
			}
		}

		void DrawQuad( real32_t x, real32_t y, real32_t w, real32_t h, real32_t s1, real32_t t1, real32_t s2,
			real32_t t2, const vector4 *colour, const Material *material )
		{
			AssertView();

			RenderCommand cmd( CommandType::DrawQuad );
			cmd.drawQuad.x = x;
			cmd.drawQuad.y = y;
			cmd.drawQuad.w = w;
			cmd.drawQuad.h = h;
			cmd.drawQuad.s1 = s1;
			cmd.drawQuad.t1 = t1;
			cmd.drawQuad.s2 = s2;
			cmd.drawQuad.t2 = t2;
			cmd.drawQuad.colour = colour;
			cmd.drawQuad.material = material;

			currentView->renderCommands.push( cmd );
		}

		void DrawModel( const Model *model ) {
			AssertView();

			RenderCommand cmd( CommandType::DrawModel );
			cmd.drawModel.model = model;
			cmd.drawModel.transform.identity();
			currentView->renderCommands.push( cmd );
		}

	} // namespace Renderer

} // namespace XS
