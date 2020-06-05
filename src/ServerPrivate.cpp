//
//  Server.cpp
//  websocketscpp
//
//  Created by Stachowicz, Lukasz on 23/10/14.
//  Copyright (c) 2014 Łukasz Stachowicz. All rights reserved.
//

#include "Parser.h"
#include "ServerPrivate.h"
#include "Select.h"
#include "ClientConnection.h"

#include <iostream>

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>

#include "sha1.h"
#include "base64.h"

#include <vector>
#include <assert.h>

using namespace WebSocketCpp;

ServerPrivate::ServerPrivate(Server *server)
	: callback_server(server)
	, listener(-1)
{
	if (server == nullptr)
		assert(!"Passing nullptr parameter to constructor.");
}

ServerPrivate::~ServerPrivate()
{
	callback_server = nullptr;
	
	if (listener != -1)
	{
		::close(listener);
	}
}

int ServerPrivate::Bind(short port)
{
	struct sockaddr_in my_addr = {AF_INET, 0, INADDR_ANY};
	
	struct protoent* protocol = ::getprotobyname("tcp");
	
	listener = ::socket(PF_INET, SOCK_STREAM, protocol->p_proto);
	
	if (listener == -1)
	{
		std::cerr << "Could not create socket" << std::endl;
		return -1;
	}
	
	my_addr.sin_port = htons(port);
	
	if (::bind(listener, (struct sockaddr *)&my_addr, sizeof(my_addr)))
	{
		std::cerr << "Faild to bind port\n" << std::endl;
		
		if (listener != -1)
			::close(listener);
		listener = -1;
		
		return -1;
	}
	
	if (::listen(listener, max_listen) != 0)
	{
		std::cerr << "Faild to listen on port\n" << std::endl;
		return -1;
	}
	
	selector.Add(listener);
	
	int optval = 1;
	socklen_t optlen = sizeof(optval);
	
	if (::setsockopt(listener, SOL_SOCKET, SO_KEEPALIVE,
					 (const void *)&optval, optlen) < 0)
	{
		std::cerr << "Faild to setsocket option\n" << std::endl;
	}
	
	fcntl(listener, F_SETFL, O_NONBLOCK);
	
	return 1;
}

int ServerPrivate::Wait(unsigned int time_ms)
{
	if (listener == -1)
	{
		std::cerr << "Server was not initialized\n" << std::endl;
		return -1;
	}

	std::vector<int> ready;

	int retval = selector.Wait(time_ms, ready);

	if (retval != 1)
	{
		return retval;
	}
	else
	{
		std::for_each(ready.begin(), ready.end(), [&](int i)
		{
			if (i == listener)
			{
				ConnectionAdd(i);
			}
			else
			{
				auto con_it = connections.find(i);

				if (con_it == connections.end())
				{
					std::cerr << "Connection was not added to list of connections." << std::endl;

					ConnectionClose(i);
				}
				else
				{
					switch (con_it->second->getState())
					{
						case ClientConnection::CONNECTION_ACCEPT:
							ConnectionHandshake(i);
							break;
						case ClientConnection::CONNECTION_NONE:
							ConnectionHandle(i);
							break;
						default:
							std::cerr << ConnectionStateToString(con_it->second->getState()) << std::endl;
							break;
					}
				}
			}

		});
	}

	return 1;
}

int ServerPrivate::Send(int socket, const char *data, const size_t data_size, Server::Data flag)
{
	auto it = connections.find(socket);
	
	if (it != connections.end())
	{
		std::vector<char> food;
		CreateFood(food, data, data_size);
		return ::send(it->first, food.data(), food.size(), 0);
	}
	
	return -1;
}

int ServerPrivate::ConnectionAdd(int fd)
{
	std::cout << "New connection on fds.\n" << std::endl;

	struct sockaddr_in remote_addr;
	socklen_t addrlen;

	addrlen = sizeof(remote_addr);
	int new_fd = ::accept(fd, (struct sockaddr *)&remote_addr, &addrlen);

	if (new_fd == -1)
	{
		std::cout << "Fail to get new connection.\n" << std::endl;

		return -1;
	}
	else
	{
		selector.Add(new_fd);

		connections[new_fd] = std::shared_ptr<ClientConnection>(new ClientConnection(new_fd, ntohs(remote_addr.sin_port), inet_ntoa(remote_addr.sin_addr)));
	}

	return 0;
}

int ServerPrivate::ConnectionClose(int fd)
{
	callback_server->Closed(fd);
	
	::close(fd);
	
	selector.Rem(fd);
	
	auto it = connections.find(fd);
	
	if (it != connections.end())
	{
		connections.erase(it);
	}
	
	return 0;
}

int ServerPrivate::ConnectionHandshake(int fd)
{
	ssize_t bytes;
	char buf[1024];

	if ((bytes = ::recv(fd, buf, sizeof(buf), 0)) <= 0)
	{
		std::cout << "Close connection on fd [" << fd << "] data size " << bytes << ".\n" << std::endl;

		ConnectionClose(fd);
	}
	else
	{
		int optval = 1;
		socklen_t optlen = sizeof(optval);

		struct protoent *tcp_proto;

		if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE,
					   (const void *)&optval, optlen) < 0)
		{
			return 0;
		}

		optval = 1;

		tcp_proto = getprotobyname("tcp");
		//setsockopt(fd_listener, tcp_proto->p_proto, TCP_NODELAY, &optval, optlen);

		fcntl(fd, F_SETFL, O_NONBLOCK);

		Parser parser(buf, bytes);

		if (parser.GetError() != Parser::PARSER_ERROR_NONE)
		{
			std::cerr << "Faild to parse data from connection on fd [" << fd << "].\n" << std::endl;

			ConnectionClose(fd);
		}

		std::string sec_websocket_key {"258EAFA5-E914-47DA-95CA-C5AB0DC85B11"};
		sec_websocket_key.insert(0, parser["Sec-WebSocket-Key"]);

		unsigned char hash[20];

		sha1::calc(sec_websocket_key.data(), (int)sec_websocket_key.length(), hash);

		char answer[1024];

		snprintf(answer, 1024, "HTTP/1.1 101 Switching Protocols\x0d\x0a"
				"Upgrade: websocket\x0d\x0a"
				"Connection: Upgrade\x0d\x0a"
				"Sec-WebSocket-Accept: %s\x0d\x0a"
				"Sec-WebSocket-Protocol: %s\x0d\x0a\x0d\x0a",
				base64_encode(hash, 20).c_str(), parser["Sec-WebSocket-Protocol"].c_str());

		if (::send(fd, answer, strlen(answer), 0) != strlen(answer))
		{
			std::cerr << "Faild to send all data to connection on fd [" << fd << "].\n" << std::endl;

			ConnectionClose(fd);
		}

		auto it = connections.find(fd);

		if (it != connections.end())
		{
			it->second->setState(ClientConnection::CONNECTION_NONE);
			
			callback_server->Opened(fd, "Not implemented");
		}
		else
		{
			std::cerr << "Faild to talk with connection on fd [" << fd << "]. Connection was not added.\n" << std::endl;

			ConnectionClose(fd);
		}
	}

	return 0;
}

int ServerPrivate::ConnectionHandle(int fd)
{
	ssize_t bytes;
	char buf[1024];

	if ((bytes = ::recv(fd, buf, sizeof(buf), 0)) <= 0)
	{
		std::cout << "Close connection on fd [" << fd << "] data size " << bytes << "." << std::endl;

		ConnectionClose(fd);
	}
	else
	{
		std::cout << "New data from fd [ " << fd << " ] " << " data size " << bytes << std::endl;

		auto it = connections.find(fd);

		if (it != connections.end())
		{
			char *data = buf;
			char *end = data + bytes;

			while ((data < end) // eat until end of current data
				   && (FeedParser(it->second->data_frame, *data) != DataFrame::DATA_FRAME_ERROR))
			{
				++data;
			}

			std::vector<char> test = it->second->data_frame.payload_data;
			test.push_back(static_cast<char>('\0'));
			std::cout << "Debug: size=" << it->second->data_frame.payload_data.size() << " Data=" << test.data() << std::endl;

			// clean last frame
			if (it->second->data_frame.final && (it->second->data_frame.state == DataFrame::DATA_FRAME_END_OF_DATA))
			{
				it->second->cleanDataFrame(); // before clean notif about new data.
			}
		}
		else
		{
			std::cerr << "Connection fd [ " << fd << " ] was not added\n";
		}
	}

	return 0;
}

void ServerPrivate::SetCallbackServer(Server *server)
{
	if (server != nullptr)
		callback_server = server;
}
