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
	ClientConnection(int fd, long port, const std::string& address);
	~ClientConnection();

private:
	int fd = -1;
	long port = -1;
	std::string address;
};

}

#endif // WEBSOCKETCPP_CLIENTCONNECTION
