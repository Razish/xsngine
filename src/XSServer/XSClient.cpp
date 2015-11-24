#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unordered_map>

#include "XSCommon/XSCommon.h"
#include "XSCommon/XSString.h"
#include "XSCommon/XSConsole.h"
#include "XSServer/XSClient.h"
#include "XSServer/XSServer.h"
#include "XSServer/XSResources.h"

namespace XS {

	namespace Server {

		std::unordered_map<uint64_t, Client *>	clients;

		void IncomingConnection( uint64_t guid ) {
			// drop client if they're already connected
			DropClient( guid );

			Client *client = new Client();

			// store it now, we accessed clients[] above meaning any iteration of clients will return a nullptr
			clients[guid] = client;

			BroadcastMessage( String::Format( "Connection from %" PRIX64, guid ).c_str() );

			// initialise
			client->guid = guid;
			client->connectionState = Client::ConnectionState::Connecting;

			// send resource list
			ServerGame::NetworkResources( true );
		}

		void DropClient( uint64_t guid ) {
			Client *client = clients[guid];
			if ( client ) {
				console.Print( PrintLevel::Normal,
					"Dropping client with guid %" PRIX64 "\n",
					guid
				);
				delete client;
				clients.erase( guid );
			}
		}

	} // namespace ServerGame

} // namespace XS
