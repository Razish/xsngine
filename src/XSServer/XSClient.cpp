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
		: connection( connection )
		{
			clients[connection.guid] = this;
		}

		Client::~Client() {
			clients[connection.guid] = nullptr;
		}

		void Client::Print( const char *msg ) const {
			Network::XSPacket msgPacket( Network::ID_XS_SV2CL_PRINT );
			ByteBuffer msgBuffer;
			ByteBuffer::Error status;
			status = msgBuffer.WriteString( msg );
			msgPacket.data = msgBuffer.GetMemory( &msgPacket.dataLen );
			connection.Send( msgPacket );
		}

	} // namespace ServerGame

} // namespace XS
