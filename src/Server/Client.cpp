#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unordered_map>

#include "Common/Common.h"
#include "Common/ByteBuffer.h"
#include "Common/String.h"
#include "Common/Console.h"
#include "Network/Network.h"
#include "Server/Client.h"
#include "Server/Server.h"
#include "Server/Resources.h"

namespace Server {

	std::unordered_map<Network::GUID, Client *> clients;

	Client::Client( Network::Connection &connection )
	: connection( connection )
	{
		clients[connection.guid] = this;
	}

	Client::~Client() {
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

} // namespace ServerGame
