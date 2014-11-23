//
//  ClientConnection.cpp
//  websocketscpp
//
//  Created by Stachowicz, Lukasz on 23/10/14.
//  Copyright (c) 2014 Łukasz Stachowicz. All rights reserved.
//

#include "ClientConnection.h"

#include <vector>
#include <sstream>

using namespace WebSocketCpp;

ClientConnection::ClientConnection(int fd, long port, const std::string& address)
	: connection_fd(fd)
	, connection_port(port)
	, connection_address(address)
	, connection_state(CONNECTION_ACCEPTED)
{
}

ClientConnection::~ClientConnection()
{
}

ClientConnection::ConnectionState ClientConnection::getState()
{
	return connection_state;
}

void ClientConnection::setState(ConnectionState state)
{
	connection_state = state;
}

std::string WebSocketCpp::ConnectionStateToString(ClientConnection::ConnectionState state)
{
	switch (state)
	{
		case WebSocketCpp::ClientConnection::CONNECTION_ACCEPTED:
			return "CONNECTION_ACCEPTED";
		case WebSocketCpp::ClientConnection::CONNECTION_REGISTRED:
			return "CONNECTION_REGISTRED";
		default:
			break;
	}

	std::stringstream ss;
	ss << "Unknow state was passed to ConnectionStateToString [ " << state << " ].";
	return ss.str();
}