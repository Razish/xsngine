#include "XSCommon/XSCommon.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSVector.h"
#include "XSRenderer/XSRenderer.h"
#include "XSRenderer/XSBackend.h"
#include "XSRenderer/XSScreenshot.h"

namespace XS {

	namespace Renderer {

		namespace Backend {

			Cvar *r_zRange = nullptr;

			GLuint defaultVao = 0u;
			GLuint defaultPbo = 0u;

			static struct State {
				bool wireFrame;
			} state = {};

			static void RegisterCvars( void ) {
				r_zRange = Cvar::Create( "r_zRange", "0.1 4000.0", "Clipping plane range", CVAR_ARCHIVE );
			}

			void Init( void ) {
				RegisterCvars();
				Command::AddCommand( "screenshot", Cmd_Screenshot );

				ClearBuffer( true, true, vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );

				// state changes
				ToggleDepthTest( true );
				SetDepthFunction( DepthFunc::LessOrEqual );

				// back-face culling
				glEnable( GL_CULL_FACE );
				glCullFace( GL_BACK );
				glFrontFace( GL_CCW );

				// alpha blending
				ToggleAlphaBlending( true );
				SetBlendFunction( BlendFunc::SourceAlpha, BlendFunc::OneMinusSourceAlpha );

				// activate the default vertex array object
				SDL_assert( glGenVertexArrays && "glBindVertexArray unavailable" );
				glGenVertexArrays(1, &defaultVao);
				SDL_assert( glBindVertexArray && "glBindVertexArray unavailable" );
				glBindVertexArray(defaultVao);

				glGenBuffers( 1, &defaultPbo );
				glBindBuffer( GL_PIXEL_PACK_BUFFER, defaultPbo );
				glBufferData( GL_PIXEL_PACK_BUFFER,
					4 * Renderer::state.window.width * Renderer::state.window.height,
					NULL,
					GL_STREAM_COPY
				);
			}

			void Shutdown( void ) {
				glDeleteVertexArrays( 1, &defaultVao );
				glDeleteBuffers( 1, &defaultPbo );
			}

			void SetWireframe( bool on ) {
				state.wireFrame = on;
				glPolygonMode( GL_FRONT_AND_BACK, on ? GL_LINE : GL_FILL );
			}

			bool GetWireframe( void ) {
				return state.wireFrame;
			}

			void ClearBuffer( bool clearColour, bool clearDepth, const vector4 &colour ) {
				GLbitfield bits = 0u;

				if ( clearColour ) {
					bits |= GL_COLOR_BUFFER_BIT;
				}

				if ( clearDepth ) {
					bits |= GL_DEPTH_BUFFER_BIT;
				}

				glClear( bits );

				if ( clearColour ) {
					glClearColor( colour.r, colour.g, colour.b, colour.a );
				}

				if ( clearDepth ) {
					glClearDepth( 1.0f );
				}
			}

			void ToggleDepthTest( bool enabled ) {
				if ( enabled ) {
					glEnable( GL_DEPTH_TEST );
				}
				else {
					glDisable( GL_DEPTH_TEST );
				}
			}

			void SetDepthFunction( DepthFunc func ) {
				GLenum glFunc = GL_LEQUAL;

				switch( func ) {

				case DepthFunc::LessOrEqual: {
					glFunc = GL_LEQUAL;
				} break;

				default: {
					// ...
				} break;

				}

				glDepthFunc( glFunc );
			}

			void ToggleAlphaBlending( bool enabled ) {
				if ( enabled ) {
					glEnable( GL_BLEND );
				}
				else {
					glDisable( GL_BLEND );
				}
			}

			static GLenum GetGLBlendFunction( BlendFunc func ) {
				GLenum result = GL_ONE;

				switch( func ) {

				case BlendFunc::Zero: {
					result = GL_ZERO;
				} break;

				case BlendFunc::One: {
					result = GL_ONE;
				} break;

				case BlendFunc::SourceColour: {
					result = GL_SRC_COLOR;
				} break;

				case BlendFunc::OneMinusSourceColour: {
					result = GL_ONE_MINUS_SRC_COLOR;
				} break;

				case BlendFunc::DestColour: {
					result = GL_DST_COLOR;
				} break;

				case BlendFunc::OneMinusDestColour: {
					result = GL_ONE_MINUS_DST_COLOR;
				} break;

				case BlendFunc::SourceAlpha: {
					result = GL_SRC_ALPHA;
				} break;

				case BlendFunc::OneMinusSourceAlpha: {
					result = GL_ONE_MINUS_SRC_ALPHA;
				} break;

				case BlendFunc::DestAlpha: {
					result = GL_DST_ALPHA;
				} break;

				case BlendFunc::OneMinusDestAlpha: {
					result = GL_ONE_MINUS_DST_ALPHA;
				} break;

				case BlendFunc::ConstantColour: {
					result = GL_CONSTANT_COLOR;
				} break;

				case BlendFunc::OneMinusConstantColour: {
					result = GL_ONE_MINUS_CONSTANT_COLOR;
				} break;

				case BlendFunc::ConstantAlpha: {
					result = GL_CONSTANT_ALPHA;
				} break;

				case BlendFunc::OneMinusConstantAlpha: {
					result = GL_ONE_MINUS_CONSTANT_ALPHA;
				} break;

				case BlendFunc::SourceAlphaSaturate: {
					result = GL_SRC_ALPHA_SATURATE;
				} break;

				default: {
					result = GL_ONE;
				} break;

				}

				return result;
			}

			void SetBlendFunction( BlendFunc sourceFunc, BlendFunc destFunc ) {
				GLenum glSourceFunc = GetGLBlendFunction( sourceFunc );
				GLenum glDestFunc = GetGLBlendFunction( destFunc );

				glBlendFunc( glSourceFunc, glDestFunc );
			}

		} // namespace Backend

	} // namespace Renderer

} // namespace XS
