#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSColours.h"
#include "XSClient/XSClient.h"
#include "XSClient/XSMenuManager.h"
#include "XSRenderer/XSMaterial.h"
#include "XSRenderer/XSImagePNG.h"
#include "XSRenderer/XSInternalFormat.h"
#include "XSRenderer/XSTexture.h"
#include "XSRenderer/XSRenderCommand.h"

namespace XS {

	namespace Client {

		MenuManager::MenuManager()
		: isOpen( privateIsOpen )
		{
			privateIsOpen = false;

			// create the cursor material
			Renderer::Material *material = new Renderer::Material();

			Renderer::Material::SamplerBinding samplerBinding = {};
			uint8_t *cursorData = Renderer::LoadPNG(
				"textures/ui/cursor.png",
				&cursorWidth, &cursorHeight
			);
			samplerBinding.texture = new Renderer::Texture(
				cursorWidth, cursorHeight,
				Renderer::InternalFormat::RGBA8,
				cursorData
			);
			material->samplerBindings.push_back( samplerBinding );

			material->shaderProgram = Renderer::quadProgram;

			assets.cursor = material;
			delete[] cursorData;
		}

		MenuManager::~MenuManager() {
			delete assets.cursor;
		}

		// load a menu from disk
		bool MenuManager::RegisterMenu( const char *fileName ) {
			// parse out the menu
			Menu *menu = new Menu( fileName );

			if ( menu->name.empty() ) {
				return false;
			}

			// store in dictionary of menus
			menus[menu->name] = menu;

			return true;
		}

		// retrieve the top-most menu
		const Menu *MenuManager::GetCurrentMenu( void ) const {
			return stack.back();
		}

		// retrieve a menu by name
		const Menu *MenuManager::GetMenuByName( const char *menuName ) const {
			for ( const auto menu : menus ) {
				if ( !String::Compare( menu.first.c_str(), menuName ) ) {
					return menu.second;
				}
			}
			return nullptr;
		}

		// push a menu onto the stack
		void MenuManager::OpenMenu( const char *menuName ) {
			Menu *menu = menus[menuName];
			if ( menu ) {
				stack.push_back( menu );
			}
			privateIsOpen = true;
		}

		// pop a menu from the stack
		void MenuManager::PopMenu( void ) {
			if ( !stack.empty() ) {
				stack.pop_back();
			}
			if ( stack.empty() ) {
				privateIsOpen = false;
			}
		}

		void MenuManager::DrawCursor( void ) {
			vector2 realPos = {
				Client::cursorPos[0] * Renderer::rdState.window.width,
				Client::cursorPos[1] * Renderer::rdState.window.height
			};
			Renderer::DrawQuad(
				realPos[0], realPos[1],
				cursorWidth / 4.0f, cursorHeight / 4.0f,
				0.0f, 0.0f, 1.0f, 1.0f,
				&colourTable[ColourIndex( COLOUR_WHITE )],
				assets.cursor
			);
		}

		// paint all menus to the screen
		void MenuManager::PaintMenus( void ) {
			for ( auto menu : stack ) {
				menu->Paint();
			}
		}

		// pass a keyboard event to the top-most menu
		void MenuManager::KeyboardEvent( const struct KeyboardEvent &ev ) {
			//const Menu *menu = GetCurrentMenu();
			if ( ev.down && ev.key == SDLK_ESCAPE ) {
				PopMenu();
			//	menu = nullptr;
			}
			/*
			if ( menu ) {
				menu->KeyboardEvent( ev );
			}
			*/
		}

		// pass a mouse motion event to the menu
		void MenuManager::MouseMotionEvent( const struct MouseMotionEvent &ev ) {
			for ( auto it = stack.rbegin(); it != stack.rend(); ++it ) {
				Menu *menu = *it;
				if ( menu->MouseMotionEvent() ) {
					break;
				}
			}
		}

		// pass a mouse button event to the menu
		void MenuManager::MouseButtonEvent( const struct MouseButtonEvent &ev ) {
			for ( auto it = stack.rbegin(); it != stack.rend(); ++it ) {
				Menu *menu = *it;
				if ( menu->MouseButtonEvent( ev ) ) {
					break;
				}
			}
		}

	} // namespace Client

} // namespace XS
