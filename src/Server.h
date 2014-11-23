//
//  Server.h
//  websocketscpp
//
//  Created by Stachowicz, Lukasz on 23/10/14.
//  Copyright (c) 2014 Łukasz Stachowicz. All rights reserved.
//

#ifndef WEBSOCKETCPP_SERVER
#define WEBSOCKETCPP_SERVER

#include <sys/types.h>

#include <map>
#include <memory>

namespace WebSocketCpp {

class ClientConnection;

class Server
{
public:
	const size_t MAX_SEND_DATA = 1024;

	Server();
	~Server();

	int CreateServer(int port);

	void Run();
	int Wait(int time);

private:
	int InternalSend(int fd, size_t size, const char *data);

	int ConnectionAdd(int fd);
	int ConnectionHandshake(int fd);
	int ConnectionHandle(int fd);

	int port = -1;

	int fd_listener = -1;
	int fd_max = -1;


	std::map<int, std::shared_ptr<ClientConnection> > connections;
	fd_set fd_connections;

	int max_listen = 5;

	struct DataFrame
	{;
		enum DataFrameBit
		{
			DATA_FRAME_BIT_ERROR			= 0x80
			, DATA_FRAME_BIT_FINAL			= 0x80
			, DATA_FRAME_BIT_RSV1			= 0x40
			, DATA_FRAME_BIT_RSV2			= 0x20
			, DATA_FRAME_BIT_RSV3			= 0x10
			, DATA_FRAME_BIT_OPCODE			= 0xf
			, DATA_FRAME_BIT_MASK			= 0x80
			, DATA_FRAME_BIT_PAYLOAD_LENGHT	= 0x7f
		};

		enum DataFrameState
		{
			DATA_FRAME_ERROR
			, DATA_FRAME_FINAL
			, DATA_FRAME_RSV1
			, DATA_FRAME_RSV2
			, DATA_FRAME_RSV3
			, DATA_FRAME_OPCODE
			, DATA_FRAME_MASK
			, DATA_FRAME_PAYLOAD_LENGHT
			, DATA_FRAME_MASK_KEY
			, DATA_FRAME_PAYLOAD_DATA
			, DATA_FRAME_MASK_PAYLOAD_DATA
			, DATA_FRAME_END_OF_DATA
		} state;

		bool final;	// 1 bit
		bool rsv1;	// 1 bit
		bool rsv2;	// 1 bit
		bool rsv3;	// 1 bit
		unsigned char opcode;	// 4 bit
		bool mask;	// 1 bit
		unsigned char payload_lenght;// 7 bit
		unsigned long long ex_payload_length; // 16 bit or 64 bit depending on payload (126 / 127) (2 byte or 8 byte)
		unsigned char mask_key[4]; // 0 or 32 bit (0 or 4 bytes)
		// TODO: no extenstiondata
		unsigned char payload_data[2024];
		// Payload data length is payload(extension_data + application_data),
		// but extension_data is 0.

		DataFrame()
		: state(DATA_FRAME_FINAL)
		, final(0)
		, rsv1(0), rsv2(0), rsv3(0)
		, opcode(0)
		, mask(0)
		, payload_lenght(0)
		, ex_payload_length(0)
		, mask_key { 0, 0, 0, 0 }
		, payload_data { 0 }
		{ }
	};
};

}
#endif // WEBSOCKETCPP_SERVER
