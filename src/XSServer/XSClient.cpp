#include <unordered_map>

#include <RakNet/RakPeerInterface.h>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"
#include "XSServer/XSClient.h"
#include "XSServer/XSServer.h"

namespace XS {

	namespace Server {

		std::unordered_map<uint64_t, Client *>	clients;

		void IncomingConnection( const RakNet::Packet *packet ) {
			const uint64_t guid = packet->guid.g;

			// see if client is already connected - should not happen
			if ( clients[guid] ) {
				console.Print( PrintLevel::Normal, "Incoming connection from already-connected client (guid: %X)\n",
					guid
				);
				delete clients[guid];
			}

			Client *client = new Client();
			BroadcastMessage( String::Format( "Connection from %X", guid ).c_str() );

			// initialise
			client->guid = packet->guid.g;
			client->connectionState = Client::ConnectionState::Connecting;

			// store
			clients[guid] = client;
		}

	} // namespace ServerGame

} // namespace XS
