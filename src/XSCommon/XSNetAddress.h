#pragma once

namespace XS {

	class NetAddress {
	private:
		bool resolved;
		bool valid;

		char hostname[256];
		union {
			struct {
				union {
					byte b[4];
					uint32_t i;
				};
			} ipv4;
			struct {
				int dummy;
			} ipv6;
		} address;

	public:
		enum class Type {
			IPV4, // 255.255.255.255
			IPV6, // 0000:0000:0000:0000:0000:0000:0000:0001
			LOOPBACK, // localhost, 127.0.0.1, ::1
			DUMMY // bots
		};

		NetAddress() = delete;

		NetAddress( const char *hostname );
		~NetAddress() {}

		NetAddress ToNetworkOrder( void ) const; // copy
	};

} // namespace XS
