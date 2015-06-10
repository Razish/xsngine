#include <algorithm>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSMatrix.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSEvent.h"
#include "XSClient/XSClientGame.h"
#include "XSClient/XSClientGameState.h"
#include "XSClient/XSBaseCamera.h"
#include "XSClient/XSFlyCamera.h"
#include "XSClient/XSEntity.h"
#include "XSClient/XSTerrain.h"
#include "XSClient/XSParticleEmitter.h"
#include "XSClient/XSCheckersBoard.h"
#include "XSInput/XSInput.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSModel.h"

namespace XS {

	namespace ClientGame {

		Cvar *cg_fov = nullptr;
		Cvar *cg_numParticles = nullptr;;

		static Renderer::View *checkersView = nullptr;
		static CheckersBoard *board = nullptr;

		void AddObject( Entity *entity ) {
			state.entities.push_back( entity );
		}

		void RemoveObject( Entity *obj ) {
			auto it = std::find( state.entities.begin(), state.entities.end(), obj );
			if ( it != state.entities.end() ) {
				state.entities.erase( it );
			}
		}

		static void RegisterCvars( void ) {
			//fov = degrees( pi * 0.75 ) / 1.25 because math
			cg_fov = Cvar::Create( "cg_fov", "108", "Field of view", CVAR_ARCHIVE );
			cg_numParticles = Cvar::Create( "cg_numParticles", "10", "Number of particles to display", CVAR_NONE );
			cg_terrainPersistence = Cvar::Create( "cg_terrainPersistence", "0.25", "Terrain roughness", CVAR_ARCHIVE );
			cg_terrainFrequency = Cvar::Create( "cg_terrainFrequency", "0.075", "Terrain complexity", CVAR_ARCHIVE );
			cg_terrainAmplitude = Cvar::Create( "cg_terrainAmplitude", "1.0", "Terrain max height", CVAR_ARCHIVE );
			cg_terrainOctaves = Cvar::Create( "cg_terrainOctaves", "4", "Terrain detail", CVAR_ARCHIVE );
			cg_terrainDimensions = Cvar::Create( "cg_terrainDimensions", "128", "Terrain resolution", CVAR_ARCHIVE );
		}

		static void RegisterCommands( void ) {
			// ...
		}

		static void RenderScene( real64_t dt ) {
			// no camera stuff to do
		}

		void Init( void ) {
			RegisterCvars();
			RegisterCommands();

			checkersView = new Renderer::View( 0u, 0u, true, RenderScene );
			board = new CheckersBoard();
		}

		void Shutdown( void ) {
			for ( auto entity : state.entities ) {
				delete entity;
			}
			delete checkersView;
			delete board;
		}

		void RunFrame( real64_t dt ) {
			for ( auto &entity : state.entities ) {
				entity->Update( dt );
			}
			board->Update( dt );
		}

		void DrawFrame( real64_t dt ) {
			checkersView->Bind();
			board->Render( dt );

			state.viewDelta.clear();
		}

		void MouseMotionEvent( const struct MouseMotionEvent &ev ) {
			vector3 delta( ev.y, ev.x, 0.0f );

			delta *= Client::Input::m_sensitivity->GetReal32();

			state.viewAngles += delta;
			state.viewDelta += delta;
		}

	} // namespace ClientGame

} // namespace XS
