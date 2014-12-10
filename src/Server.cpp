#include "../include/Server.h"
#include "ServerPrivate.h"

#include <assert.h>

using namespace WebSocketCpp;

inline ServerPrivate* GetServer(void *server)
{
	return static_cast<WebSocketCpp::ServerPrivate *>(server);
}

Server::Server()
	: server(nullptr)
{
}

Server::~Server()
{
	if (server != nullptr)
	{
		auto server_imp = GetServer(server);
		delete server_imp;
		server = nullptr;
	}
}

bool Server::Bind(int port)
{
	if (server != nullptr)
		return false;
	
	server = new WebSocketCpp::ServerPrivate();
	GetServer(server)->SetCallbackServer(this);
	
	if (GetServer(server)->Bind(port) != -1)
		return true;
	
	delete GetServer(server);
	server = nullptr;
	return false;
}
	

int Server::Wait(int time_ms)
{
	if (server == nullptr)
		return -1;
	
	return GetServer(server)->Wait(time_ms);
}

int Server::Send(int socket, const char *data, const size_t data_size, Data flag)
{
	if (server == nullptr)
		return false;
	
	switch (flag)
	{
	case Text:
		return 0;
	case Binary:
		return 0;
	default:
		assert(!"Server::Send: Unknow data type.");
		break;
	}
	
	return 0;
}

bool Server::Close(int socket)
{
	if (server == nullptr)
		return false;
	
	return false;
}