//
//  ClientConnection.h
//  websocketscpp
//
//  Created by Stachowicz, Lukasz on 23/10/14.
//  Copyright (c) 2014 Łukasz Stachowicz. All rights reserved.
//

#ifndef WEBSOCKETCPP_CLIENTCONNECTION
#define WEBSOCKETCPP_CLIENTCONNECTION

#include <string>

namespace WebSocketCpp {

class ClientConnection
{
public:
	enum ConnectionState
	{
		CONNECTION_ACCEPTED = 0
		, CONNECTION_REGISTRED
	};
	
	ClientConnection(int fd, long port, const std::string& address);
	~ClientConnection();

	ConnectionState getState();
	void setState(ConnectionState state);

private:
	int connection_fd = -1;
	long connection_port = -1;
	std::string connection_address;

	ConnectionState connection_state;
};

	std::string ConnectionStateToString(ClientConnection::ConnectionState state);

}

#endif // WEBSOCKETCPP_CLIENTCONNECTION
