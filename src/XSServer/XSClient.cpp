#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unordered_map>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSByteBuffer.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"
#include "XSNetwork/XSNetwork.h"
#include "XSServer/XSClient.h"
#include "XSServer/XSServer.h"
#include "XSServer/XSResources.h"

namespace XS {

	namespace Server {

		std::unordered_map<Network::GUID, Client *> clients;

		Client::Client( Network::Connection &connection )
		: connection( connection ), account( nullptr ), character( nullptr )
		{
			clients[connection.guid] = this;
		}

		Client::~Client() {
			Logout();
			//FIXME: investigate connection timeout to log out
			clients[connection.guid] = nullptr;
		}

		void Client::Print( const char *msg ) const {
			ByteBuffer msgBuffer;
			if ( msgBuffer.WriteString( msg ) == ByteBuffer::Error::Success ) {
				Network::XSPacket msgPacket( Network::ID_XS_SV2CL_PRINT );
				msgPacket.data = msgBuffer.GetMemory( &msgPacket.dataLen );
				connection.Send( msgPacket );
			}
		}

		void Client::SelectCharacter( Character *newCharacter ) {
			SDL_assert( account );

			if ( newCharacter == character ) {
				// nothing to do
				return;
			}

			if ( character ) {
				//TODO: cleanup character info, logout etc
			}

			//TODO: leave world? stop quests? etc

			Print(
				String::Format( "Selecting character %i (%s) from account ID %i\n",
					newCharacter->id,
					newCharacter->name.c_str(),
					account->id
				).c_str()
			);

			character = newCharacter;
		}

		//FIXME: move to Account::Login()
		Account::LoginError Client::Login( const char *name, const char *password ) {
			Logout();

			Account *act = Account::Find( name );
			if ( !act ) {
				return Account::LoginError::AcctNotFound;
			}

			if ( act->password != password ) {
				return Account::LoginError::BadAuthentication;
			}

			if ( !act->RequestLock() ) {
				// could not attain lock - account is in use elsewhere
				return Account::LoginError::Locked;
			}

			account = act;
			return Account::LoginError::Success;
		}

		void Client::Logout( void ) {
			if ( account ) {
				account->ReleaseLock();
			}
			account = nullptr;
		}

	} // namespace ServerGame

} // namespace XS
