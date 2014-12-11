#include "Server.h"
#include "ServerPrivate.h"

#include <assert.h>

using namespace WebSocketCpp;

inline ServerPrivate* GetServer(void *server)
{
	return static_cast<WebSocketCpp::ServerPrivate *>(server);
}

Server::Server()
	: server_private(nullptr)
{
}

Server::~Server()
{
	if (server_private != nullptr)
	{
		auto server_imp = GetServer(server_private);
		delete server_imp;
		server_private = nullptr;
	}
}

bool Server::Bind(int port)
{
	if (server_private != nullptr)
		return false;
	
	server_private = new ServerPrivate(this);
	
	if (GetServer(server_private)->Bind(port) != -1)
		return true;
	
	delete GetServer(server_private);
	server_private = nullptr;
	return false;
}
	

int Server::Wait(unsigned int time_ms)
{
	if (server_private == nullptr)
		return -1;
	
	return GetServer(server_private)->Wait(time_ms);
}

int Server::Send(int socket, const char *data, const size_t data_size, Data flag)
{
	if (server_private == nullptr)
		return -1;
	
	return GetServer(server_private)->Send(socket, data, data_size, flag);
}

bool Server::Close(int socket)
{
	if (server_private == nullptr)
		return false;
	
//	return GetServer(server)->Close(socket);
	return false;
}