//
//  Server.cpp
//  websocketscpp
//
//  Created by Stachowicz, Lukasz on 23/10/14.
//  Copyright (c) 2014 Łukasz Stachowicz. All rights reserved.
//

#include "Parser.h"
#include "Server.h"
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

using namespace WebSocketCpp;

Server::Server()
{
	FD_ZERO(&fd_connections);
}

Server::~Server()
{
	if (fd_listener != -1)
	{
		::close(fd_listener);
	}
}

void Server::Run()
{
	bool quit = false;

	while(!quit)
	{
		quit = (Wait(5) == -1);
	}
}

int Server::Wait(int time)
{
	if (fd_listener == -1)
	{
		std::cerr << "Server was not initialized\n" << std::endl;
		return -1;
	}

	bool quit = false;

	struct sockaddr_in remote_addr;
	socklen_t addrlen;

	while (!quit)
	{
		timeval tv;
		memset(&tv, 0, sizeof(tv));
		tv.tv_sec = time;

		fd_set fds = fd_connections;

		int retval = ::select(fd_max, &fds, nullptr, nullptr, &tv);

		if (retval == -1)
		{
			if (errno == EAGAIN)
			{
				continue;
			}
			else if ((errno == EINTR && quit) || errno == EBADF)
			{
				std::cerr << "Interrupt while waiting for new events error no %i." << errno << " Closing program." << std::endl;

				quit = true;

				return -1;
			}
			else
			{
				continue;
			}
		}
		else if (retval == 0)
		{
//			std::cout << "Select timeout when listen on sockets\n" << std::endl;

			return 0;
		}
		else
		{
			for (int i = 0; i < fd_max; ++i)
			{
				if (FD_ISSET(i, &fds))
				{
					if (fd_listener == i)
					{
						std::cout << "New connection on fds.\n" << std::endl;

						addrlen = sizeof(remote_addr);
						int new_fd = ::accept(fd_listener, (struct sockaddr *)&remote_addr, &addrlen);

						if (new_fd == -1)
						{
							std::cout << "Fail to get new connection.\n" << std::endl;
						}
						else
						{
							FD_SET(new_fd, &fd_connections);

							if(new_fd > fd_max - 1)
							{
								fd_max = new_fd + 1;
							}

							connections.push_back(std::unique_ptr<ClientConnection>(new ClientConnection(new_fd, ntohs(remote_addr.sin_port), inet_ntoa(remote_addr.sin_addr))));
						}
					}
					else
					{
						ssize_t bytes;
						char buf[1024];

						if ((bytes = ::recv(i, buf, sizeof(buf), 0)) <= 0)
						{
							std::cout << "Close connection on fd [" << i << "] data size " << bytes << ".\n" << std::endl;

							::close(i);
							FD_CLR(i, &fd_connections);
						}
						else
						{
							int optval = 1;
							socklen_t optlen = sizeof(optval);

							struct protoent *tcp_proto;

							if (setsockopt(i, SOL_SOCKET, SO_KEEPALIVE,
										   (const void *)&optval, optlen) < 0)
							{
								return 0;
							}

							optval = 1;

							tcp_proto = getprotobyname("TCP");
							//setsockopt(fd_listener, tcp_proto->p_proto, TCP_NODELAY, &optval, optlen);
							
							fcntl(i, F_SETFL, O_NONBLOCK);

							Parser parser(buf, bytes);

							if (parser.GetParserError() != Parser::PARSER_ERROR_NONE)
							{
								std::cerr << "Faild to parse  data from connection on fd [" << i << "].\n" << std::endl;

								::close(i);

								FD_CLR(i, &fd_connections);
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

							::send(i, answer, strlen(answer), 0);
						}
					}
				}
			}
		}
	}

	return 1;
}

int Server::CreateServer(int port)
{
	struct sockaddr_in my_addr = {AF_INET, 0, INADDR_ANY};

	struct protoent* protocol = getprotobyname("TCP");

	fd_listener = ::socket(PF_INET, SOCK_STREAM, protocol->p_proto);

	if (fd_listener == -1)
	{
		std::cerr << "Could not create socket" << std::endl;

		return -1;
	}

	my_addr.sin_port = htons(port);

	if (::bind(fd_listener, (struct sockaddr *)&my_addr, sizeof(my_addr)))
	{
		std::cerr << "Faild to bind port\n" << std::endl;

		if (fd_listener != -1)
			::close(fd_listener);
		fd_listener = -1;

		return -1;
	}

	if (::listen(fd_listener, max_listen) != 0)
	{
		std::cerr << "Faild to listen on port\n" << std::endl;

		return -1;
	}

	fd_max = fd_listener + 1;
	FD_SET(fd_listener, &fd_connections);

	int optval = 1;
	socklen_t optlen = sizeof(optval);

	struct protoent *tcp_proto;

	if (setsockopt(fd_listener, SOL_SOCKET, SO_KEEPALIVE,
				   (const void *)&optval, optlen) < 0)
	{
		return 0;
	}

	optval = 1;

	tcp_proto = getprotobyname("TCP");
	//setsockopt(fd_listener, tcp_proto->p_proto, TCP_NODELAY, &optval, optlen);

	fcntl(fd_listener, F_SETFL, O_NONBLOCK);

	return 1;
}