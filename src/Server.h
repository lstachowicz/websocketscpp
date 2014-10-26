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

#include <vector>
#include <memory>

namespace WebSocketCpp {

class ClientConnection;

class Server
{
public:
	Server();
	~Server();

	int CreateServer(int port);

	void Run();
	int Wait(int time);

private:

	int port = -1;

	int fd_listener = -1;
	int fd_max = -1;


	std::vector<std::unique_ptr<ClientConnection> > connections;
	fd_set fd_connections;

	int max_listen = 5;
};

}
#endif // WEBSOCKETCPP_SERVER
