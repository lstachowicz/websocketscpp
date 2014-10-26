//
//  ClientConnection.cpp
//  websocketscpp
//
//  Created by Stachowicz, Lukasz on 23/10/14.
//  Copyright (c) 2014 Łukasz Stachowicz. All rights reserved.
//

#include "ClientConnection.h"

#include <vector>

using namespace WebSocketCpp;

ClientConnection::ClientConnection(int fd_in, long port_in, const std::string& address_in)
	: fd(fd_in)
	, port(port_in)
	, address(address_in)
{
}

ClientConnection::~ClientConnection()
{
}