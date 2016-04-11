#define __STDC_FORMAT_MACROS
#include <inttypes.h>
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
			camera = new FlyCamera( *sceneView, 0.05f );

			const glm::vec3 cameraPos( 0.0f, 0.0f, 0.0f );
			const glm::vec3 lookAt( 0.0f, 0.0f, -1.0f ); // looking down -Z
			const glm::vec3 up( 0.0f, 1.0f, 0.0f ); // Y is up

			Cvar *r_zRange = Cvar::Get( "r_zRange" );
			real32_t zNear = r_zRange->GetReal32( 0 );
			real32_t zFar = r_zRange->GetReal32( 1 );

			camera->SetupPerspective(
				glm::radians( cg_fov->GetReal32() ),
				Renderer::rdState.window.aspectRatio,
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
			for ( auto &entity : clgState.entities ) {
				delete entity.second;
			}
			clgState.entities.clear();
			delete sceneView;
		}

		void RunFrame( real64_t dt ) {
			for ( auto &entity : clgState.entities ) {
				entity.second->Update( dt );
			}
		}

		void DrawFrame( real64_t dt ) {
			camera->Update( dt );

			// add objects to scene
			Renderer::View &v = *sceneView; // cache
			for ( const auto &entity : clgState.entities ) {
				entity.second->AddToScene( v );
			}

			// add lights to scene
			static const vector3 lightPos = { 0.0f, 32.0f, 0.0f };
			sceneView->AddPointLight( lightPos );

			clgState.viewDelta.clear();
		}

		bool ReceivePacket( const RakNet::Packet *packet ) {
			switch ( packet->data[0] ) {

			case Network::ID_XS_SV2CL_CONNECTION_STATE: {
				using State = Network::Connection::State;
				const Network::GUID guid = packet->guid.g;
				Network::Connection &connection = Network::Connection::Get( guid );
				SDL_assert( guid == connection.guid );

				const uint8_t *buffer = packet->data + 1;
				size_t bufferLen = packet->length - 1;
				ByteBuffer bb( buffer, bufferLen );

				State newState;
				if ( bb.Read<State>( &newState ) != ByteBuffer::Error::Success ) {
					// invalid packet format
					break;
				}

				if ( connection.ChangeState( newState ) ) {
					// the server is telling us our state has changed
					switch ( newState ) {

					case State::NotConnected: {
						// initial state: no connections requested
						// error: should never change to this
						SDL_assert( !"unhandled case" );
					} break;

					case State::SynSent: {
						// we request a connection to a host
						SDL_assert( !"unhandled case" );
					} break;

					case State::SynReceived: {
						// the host sends us back a challenge

						//TODO: respond to challenge sent by host

						connection.ChangeState( State::AckSent, true );
					} break;

					case State::AckSent: {
						// we respond to the challenge
						SDL_assert( !"unhandled case" );
					} break;

					case State::AckReceived: {
						// host allocates client instance and starts sending session info

						//TODO: wait for baselines

						connection.ChangeState( State::Active, true );
					} break;

					case State::Active: {
						// baselines have been receieved, connection is active
						// nothing to do at the moment
						SDL_assert( !"unhandled case" );
					} break;

					case State::Disconnecting: {
						// disconnection has been requested
						// this is handled by another RakNet message currently
						SDL_assert( !"unhandled case" );
					} break;

					case State::Dead: {
						// this host has finished disconnecting or has timed out
						// this is handled by another RakNet message currently
						SDL_assert( !"unhandled case" );
					} break;

					default: {
					} break;

					}
				}
			} break;

			case Network::ID_XS_SV2CL_GAMESTATE: {
				//TODO: store a large history of snapshots, with entity states so we can interpolate between them
				uint8_t *buffer = packet->data + 1;
				size_t bufferLen = packet->length - 1;

				ByteBuffer bb( buffer, bufferLen );
				ByteBuffer::Error status;

				struct SnapshotHeader {
					uint32_t numEntities;
				} snapshotHeader;
				status = bb.ReadRaw( &snapshotHeader, sizeof(snapshotHeader) );

				if ( status != ByteBuffer::Error::Success ) {
					// can't operate on snapshot format we didn't expect
					break;
				}

				for ( size_t i = 0u; i < snapshotHeader.numEntities; i++ ) {
					uint32_t entityID = 0xFFFFFFFFu;
					status = bb.Read<uint32_t>( &entityID );
					SDL_assert( entityID != Entity::invalidID );

					Entity *entity = Entity::Get( entityID );

					if ( entity ) {
						//TODO: delta update of entity
						// must be specialised
						// skip type
						status = bb.Skip( sizeof(uint32_t) );

						// read position
						status = bb.Read<real32_t>( &entity->position[0] );
						status = bb.Read<real32_t>( &entity->position[1] );
						status = bb.Read<real32_t>( &entity->position[2] );

						if ( entity->type == EntityType::Model ) {
							status = bb.Skip( sizeof(uint32_t) );
						}
					}
					else {
						EntityType entityType = EntityType::Generic;
						status = bb.Read<uint32_t>( reinterpret_cast<uint32_t *>( &entityType ) );

						vector3 tmpPos = {};
						status = bb.Read<real32_t>( &tmpPos[0] );
						status = bb.Read<real32_t>( &tmpPos[1] );
						status = bb.Read<real32_t>( &tmpPos[2] );

						//TODO: spawn factory?
						switch ( entityType ) {

						default:
						case EntityType::Generic: {
							entity = new Entity();
						} break;

						case EntityType::FXRunner: {
							entity = new EntityFXRunner();

							EntityFXRunner *fxEnt = reinterpret_cast<EntityFXRunner*>( entity );
							status = bb.Read<uint32_t>( &fxEnt->count );
							status = bb.Read<uint32_t>( &fxEnt->life );
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
							status = bb.Read<uint32_t>( &resourceID );

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
						entity->AddToWorld();
					}
				}
			} break;

			case Network::ID_XS_SV2CL_RESOURCES: {
				uint8_t *buffer = packet->data + 1;
				size_t bufferLen = packet->length - 1;
				ByteBuffer bb( buffer, bufferLen );

				uint32_t numResources = 0u;
				if ( bb.Read<uint32_t>( &numResources ) != ByteBuffer::Error::Success ) {
					break;
				}

				console.Print( PrintLevel::Debug, "numResources: %u\n",
					numResources
				);

				for ( uint32_t i = 0u; i < numResources; i++ ) {
					uint32_t resourceID = 0u;
					ByteBuffer::Error status = bb.Read<uint32_t>( &resourceID );

					uint32_t strLength = 0u;
					const char *resourceName = nullptr;
					status = bb.ReadString( &resourceName, &strLength );

					console.Print( PrintLevel::Debug, "resource %02u: %s\n",
						resourceID,
						resourceName
					);

					if ( status != ByteBuffer::Error::Success ) {
					}

					delete[] resourceName;
				}
			} break;

			case Network::ID_XS_SV2CL_PRINT: {
				uint8_t *buffer = packet->data + 1;
				size_t bufferLen = packet->length - 1;
				ByteBuffer bb( buffer, bufferLen );

				ByteBuffer::String str;
				if ( bb.ReadString( str ) == ByteBuffer::Error::Success ) {
					console.Print( PrintLevel::Normal, "%s",
						str.c_str()
					);
				}
			} break;

			case Network::ID_XS_SV2CL_CHAT: {
				uint8_t *buffer = packet->data + 1;
				size_t bufferLen = packet->length - 1;
				ByteBuffer bb( buffer, bufferLen );

				Network::GUID senderGUID;
				if ( bb.Read<Network::GUID>( &senderGUID ) != ByteBuffer::Error::Success ) {
					break;
				}
				uint32_t flags;
				if ( bb.Read<uint32_t>( &flags ) != ByteBuffer::Error::Success ) {
					break;
				}
				ByteBuffer::String str;
				if ( bb.ReadString( str ) != ByteBuffer::Error::Success ) {
					break;
				}
				console.Print( PrintLevel::Normal, "%" PRIX64 " said: %s\n", senderGUID, str.c_str() );
			} break;

			default: {
				return false;
			} break;

			}

			return true;
		}

		bool KeyboardEvent( const struct KeyboardEvent &ev ) {
			return false;
		}

		bool MouseButtonEvent( const struct MouseButtonEvent &ev ) {
			return false;
		}

		bool MouseMotionEvent( const struct MouseMotionEvent &ev ) {
			vector3 delta = { static_cast<real32_t>( ev.y ), static_cast<real32_t>( ev.x ), 0.0f };

			delta *= Client::Input::m_sensitivity->GetReal32();

			clgState.viewAngles += delta;
			clgState.viewDelta += delta;

			return false;
		}

		bool MouseWheelEvent( const struct MouseWheelEvent &ev ) {
			return false;
		}

	} // namespace ClientGame

} // namespace XS
