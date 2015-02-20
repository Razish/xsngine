#pragma once

namespace XS {

	enum class AddressType {
		IPV4, // 255.255.255.255
		IPV6, // 0000:0000:0000:0000:0000:0000:0000:0001
		LOOPBACK, // localhost, 127.0.0.1, ::1
		DUMMY // bots
	};

	class NetAddress {
	private:
		bool		resolved;
		bool		valid;
		//char		hostname[256];
		AddressType	type;

		union {
			byteAlias32_t ipv4;
			struct {
				int32_t dummy;
			} ipv6;
		} address;

	public:
		NetAddress() = delete;

		NetAddress(
			const char *hostname
		);
		~NetAddress() {}

		NetAddress ToNetworkOrder( void ) const; // copy
	};

} // namespace XS
