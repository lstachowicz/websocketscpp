//
//  Server.h
//  websocketscpp
//
//  Created by Stachowicz, Lukasz on 23/10/14.
//  Copyright (c) 2014 Łukasz Stachowicz. All rights reserved.
//

#ifndef WEBSOCKETCPP_SERVER
#define WEBSOCKETCPP_SERVER

#include "Select.h"

#include <sys/types.h>

#include <map>
#include <memory>
#include <vector>

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
	int ConnectionClose(int fd);

	int port = -1;

	std::map<int, std::shared_ptr<ClientConnection> > connections;
	fd_set fd_connections;

	int max_listen = 5;

	int listener;
	Select selector;
};

}
#endif // WEBSOCKETCPP_SERVER
