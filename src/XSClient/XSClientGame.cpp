#include <algorithm>

#include <RakNet/RakPeerInterface.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSConsole.h"
#include "XSCommon/XSCvar.h"
#include "XSCommon/XSFile.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSMatrix.h"
#include "XSCommon/XSCommand.h"
#include "XSCommon/XSEvent.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSClient/XSClientGame.h"
#include "XSClient/XSClientGameState.h"
#include "XSClient/XSBaseCamera.h"
#include "XSClient/XSFlyCamera.h"
#include "XSClient/XSEntity.h"
#include "XSClient/XSEntityFXRunner.h"
#include "XSClient/XSEntityModel.h"
#include "XSClient/XSTerrain.h"
#include "XSClient/XSParticleEmitter.h"
#include "XSClient/XSResources.h"
#include "XSInput/XSInput.h"
#include "XSNetwork/XSNetwork.h"
#include "XSRenderer/XSView.h"
#include "XSRenderer/XSModel.h"

namespace XS {

	namespace ClientGame {

		Cvar *cg_fov = nullptr;
		Cvar *cg_numParticles = nullptr;

		static Renderer::View *sceneView = nullptr;
		FlyCamera *camera = nullptr;

		static void RegisterCvars( void ) {
			cg_fov = Cvar::Create( "cg_fov", "108", // fov = degrees( pi * 0.75 ) / 1.25 because math
				"Field of view", CVAR_ARCHIVE
			);
			cg_numParticles = Cvar::Create( "cg_numParticles", "10",
				"Number of particles to display", CVAR_ARCHIVE
			);
			cg_terrainPersistence = Cvar::Create( "cg_terrainPersistence", "0.25",
				"Terrain roughness", CVAR_ARCHIVE
			);
			cg_terrainFrequency = Cvar::Create( "cg_terrainFrequency", "0.075",
				"Terrain complexity", CVAR_ARCHIVE
			);
			cg_terrainAmplitude = Cvar::Create( "cg_terrainAmplitude", "1.0",
				"Terrain max height", CVAR_ARCHIVE
			);
			cg_terrainOctaves = Cvar::Create( "cg_terrainOctaves", "4",
				"Terrain detail", CVAR_ARCHIVE
			);
			cg_terrainDimensions = Cvar::Create( "cg_terrainDimensions", "128",
				"Terrain resolution", CVAR_ARCHIVE
			);
		}

		static void RegisterCommands( void ) {
			// ...
		}

		static void RenderScene( real64_t dt ) {
			sceneView->projectionMatrix = camera->GetProjectionView();
		}

		static void SetupCamera( void ) {
			camera = new FlyCamera( 0.05f );

			const glm::vec3 cameraPos( 0.0f, 0.0f, 0.0f );
			const glm::vec3 lookAt( 0.0f, 0.0f, -1.0f ); // looking down -Z
			const glm::vec3 up( 0.0f, 1.0f, 0.0f ); // Y is up

			Cvar *r_zRange = Cvar::Get( "r_zRange" );
			real32_t zNear = r_zRange->GetReal32( 0 );
			real32_t zFar = r_zRange->GetReal32( 1 );

			camera->SetupPerspective(
				glm::radians( cg_fov->GetReal32() ),
				Renderer::state.window.aspectRatio,
				zNear,
				zFar
			);
			camera->LookAt( cameraPos, lookAt, up );
		}

		void Init( void ) {
			RegisterCvars();
			RegisterCommands();

			sceneView = new Renderer::View( 0u, 0u, false, RenderScene );
			SetupCamera();
		}

		void Shutdown( void ) {
			for ( auto entity : state.entities ) {
				RemoveEntity( entity.second );
			}
			delete sceneView;
		}

		void RunFrame( real64_t dt ) {
			for ( auto &entity : state.entities ) {
				entity.second->Update( dt );
			}
		}

		void DrawFrame( real64_t dt ) {
			camera->Update( dt );

			// add objects to scene
			for ( const auto &entity : state.entities ) {
				entity.second->AddToScene( sceneView );
			}

			// add lights to scene
			static const vector3 lightPos = { 0.0f, 32.0f, 0.0f };
			sceneView->AddPointLight( lightPos );

			state.viewDelta.clear();
		}

		bool ReceivePacket( const RakNet::Packet *packet ) {
			switch ( packet->data[0] ) {

			case Network::ID_XS_SV2CL_GAMESTATE: {
				//TODO: store a large history of snapshots, with entity states so we can interpolate between them
				uint8_t *buffer = packet->data + 1;
				size_t bufferLen = packet->length - 1;

				ByteBuffer bb( buffer, bufferLen );

				struct SnapshotHeader {
					uint32_t numEntities;
				} snapshotHeader;
				bb.ReadGeneric( &snapshotHeader, sizeof(snapshotHeader) );

				for ( size_t i = 0u; i < snapshotHeader.numEntities; i++ ) {
					uint32_t entityID = 0xFFFFFFFFu;
					bb.ReadUInt32( &entityID );
					SDL_assert( entityID != Entity::invalidID );

					Entity *entity = GetEntity( entityID );

					if ( entity ) {
						//TODO: delta update of entity
						// must be specialised
						// skip type
						bb.Skip( sizeof(uint32_t) );

						// read position
						bb.ReadReal32( &entity->position[0] );
						bb.ReadReal32( &entity->position[1] );
						bb.ReadReal32( &entity->position[2] );

						if ( entity->type == EntityType::Model ) {
							bb.Skip( sizeof(uint32_t) );
						}
					}
					else {
						EntityType entityType = EntityType::Generic;
						bb.ReadUInt32( reinterpret_cast<uint32_t *>( &entityType ) );

						vector3 tmpPos = {};
						bb.ReadReal32( &tmpPos[0] );
						bb.ReadReal32( &tmpPos[1] );
						bb.ReadReal32( &tmpPos[2] );

						//TODO: spawn factory?
						switch ( entityType ) {

						default:
						case EntityType::Generic: {
							entity = new Entity();
						} break;

						case EntityType::FXRunner: {
							entity = new EntityFXRunner();

							EntityFXRunner *fxEnt = reinterpret_cast<EntityFXRunner*>( entity );
							bb.ReadUInt32( &fxEnt->count );
							bb.ReadUInt32( &fxEnt->life );
							//TODO: refactor ParticleEmitter to not be a Renderable, or implement a
							//	ParticleEmitter::Register() ala Renderer::Model::Register
							/*
							fxEnt->renderInfo.handle = new ParticleEmitter(
								fxEnt->count,
								fxEnt->life,
								"textures/fx/orb.png"
							);
							*/
						} break;

						case EntityType::Model: {
							entity = new EntityModel();

							// read model ID
							uint32_t resourceID;
							bb.ReadUInt32( &resourceID );

							EntityModel *modelEnt = reinterpret_cast<EntityModel*>( entity );
						#if 0
							modelEnt->renderInfo.renderable = Renderer::Model::Register( GetResource( resourceID ) );
						#else
							modelEnt->renderInfo.handle = Renderer::Model::Register( "models/torus.xmf" );
						#endif
						} break;

						}

						entity->id = entityID;
						entity->position = tmpPos;
						AddEntity( entity );
					}
				}
			} break;

			case Network::ID_XS_SV2CL_PRINT: {
				uint8_t *buffer = packet->data + 1;
				size_t bufferLen = packet->length - 1;
				ByteBuffer bb( buffer, bufferLen );

				const char *msg = nullptr;
				bb.ReadString( &msg );

				// avoid printf format attacks
				console.Print( PrintLevel::Normal, "%s\n",
					msg
				);

				delete[] msg;
			} break;

			default: {
				return false;
			} break;

			}

			return true;
		}

		void MouseMotionEvent( const struct MouseMotionEvent &ev ) {
			vector3 delta = { static_cast<real32_t>( ev.y ), static_cast<real32_t>( ev.x ), 0.0f };

			delta *= Client::Input::m_sensitivity->GetReal32();

			state.viewAngles += delta;
			state.viewDelta += delta;
		}

		void MouseButtonEvent( const struct MouseButtonEvent &ev ) {
			// ...
		}

	} // namespace ClientGame

} // namespace XS
