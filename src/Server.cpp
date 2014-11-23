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
#include <assert.h>

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
				std::cerr << "Interrupt while waiting for new events error no " << errno << " "  << strerror(errno) << " Closing program." << std::endl;

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
						ConnectionAdd(i);
					}
					else
					{

						auto it = connections.find(i);

						if (it == connections.end())
						{
							std::cerr << "Connection was not added to list of connections." << std::endl;

							::close(i);

							FD_CLR(i, &fd_connections);

							continue;
						}

						switch (it->second->getState())
						{
							case ClientConnection::CONNECTION_ACCEPTED:
								ConnectionHandshake(i);
								break;
							case ClientConnection::CONNECTION_REGISTRED:
								ConnectionHandle(i);
								break;
							default:
								std::cerr << ConnectionStateToString(it->second->getState()) << std::endl;
								break;
						}
					}
				}
			}
		}
	}

	return 1;
}

int Server::ConnectionAdd(int fd)
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
		FD_SET(new_fd, &fd_connections);

		if(new_fd > fd_max - 1)
		{
			fd_max = new_fd + 1;
		}

		connections[new_fd] = std::shared_ptr<ClientConnection>(new ClientConnection(new_fd, ntohs(remote_addr.sin_port), inet_ntoa(remote_addr.sin_addr)));
	}

	return 0;
}

int Server::ConnectionHandshake(int fd)
{
	ssize_t bytes;
	char buf[1024];

	if ((bytes = ::recv(fd, buf, sizeof(buf), 0)) <= 0)
	{
		std::cout << "Close connection on fd [" << fd << "] data size " << bytes << ".\n" << std::endl;

		::close(fd);
		FD_CLR(fd, &fd_connections);
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

		tcp_proto = getprotobyname("TCP");
		//setsockopt(fd_listener, tcp_proto->p_proto, TCP_NODELAY, &optval, optlen);

		fcntl(fd, F_SETFL, O_NONBLOCK);

		Parser parser(buf, bytes);

		if (parser.GetParserError() != Parser::PARSER_ERROR_NONE)
		{
			std::cerr << "Faild to parse  data from connection on fd [" << fd << "].\n" << std::endl;

			::close(fd);

			FD_CLR(fd, &fd_connections);
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

			::close(fd);

			FD_CLR(fd, &fd_connections);
		}

		auto it = connections.find(fd);

		if (it != connections.end())
		{
			it->second->setState(ClientConnection::CONNECTION_REGISTRED);
		}
		else
		{
			std::cerr << "Faild to talk with connection on fd [" << fd << "]. Connection was not registred.\n" << std::endl;

			::close(fd);

			FD_CLR(fd, &fd_connections);
		}
	}

	return 0;
}

int Server::ConnectionHandle(int fd)
{
	ssize_t bytes;
	char buf[1024];

	if ((bytes = ::recv(fd, buf, sizeof(buf), 0)) <= 0)
	{
		std::cout << "Close connection on fd [" << fd << "] data size " << bytes << "." << std::endl;

		::close(fd);
		FD_CLR(fd, &fd_connections);
	}
	else
	{
		std::cout << "New data from fd [ " << fd << " ] " << " data size " << bytes << std::endl;

		DataFrame data_frame;

		unsigned int offset { 0 };

		while (data_frame.state != DataFrame::DATA_FRAME_END_OF_DATA)
		{
			switch (data_frame.state)
			{
			case DataFrame::DATA_FRAME_FINAL:
				data_frame.final = buf[offset] & DataFrame::DATA_FRAME_BIT_FINAL;
			case DataFrame::DATA_FRAME_RSV1:
				data_frame.rsv1 = buf[offset] & DataFrame::DATA_FRAME_BIT_RSV1;
			case DataFrame::DATA_FRAME_RSV2:
				data_frame.rsv2 = buf[offset] & DataFrame::DATA_FRAME_BIT_RSV2;
			case DataFrame::DATA_FRAME_RSV3:
				data_frame.rsv3 = buf[offset] & DataFrame::DATA_FRAME_BIT_RSV3;
			case DataFrame::DATA_FRAME_OPCODE:
				// TODO: Implement opcode usage
				data_frame.opcode = buf[offset] & DataFrame::DATA_FRAME_BIT_OPCODE;
				data_frame.state = DataFrame::DATA_FRAME_MASK;
				break;
			case DataFrame::DATA_FRAME_MASK:
				data_frame.mask = buf[offset] & DataFrame::DATA_FRAME_BIT_MASK;
			case DataFrame::DATA_FRAME_PAYLOAD_LENGHT:
			{
				switch (buf[offset] & DataFrame::DATA_FRAME_BIT_PAYLOAD_LENGHT)
				{
				case 126:
					data_frame.payload_lenght = buf[offset] << 8;
					data_frame.payload_lenght |= buf[++offset];
					break;
				case 127:
					// TODO: Check size
					// on 32 bit machine we don't have 64 bit type
				#if defined __LP64__
					data_frame.payload_lenght = (size_t)buf[offset] << 56;
					data_frame.payload_lenght |= (size_t)buf[++offset] << 48;
					data_frame.payload_lenght |= (size_t)buf[++offset] << 40;
					data_frame.payload_lenght |= (size_t)buf[++offset] << 32;
				#else
					offset += 4;
				#endif
					data_frame.payload_lenght |= (size_t)buf[++offset] << 24;
					data_frame.payload_lenght |= (size_t)buf[++offset] << 16;
					data_frame.payload_lenght |= (size_t)buf[++offset] << 8;
					data_frame.payload_lenght |= (size_t)buf[++offset];
					break;
				default:
					data_frame.payload_lenght = (buf[offset] & DataFrame::DATA_FRAME_BIT_PAYLOAD_LENGHT);
					break;
				}

				data_frame.state = data_frame.mask ? DataFrame::DATA_FRAME_MASK_KEY : DataFrame::DATA_FRAME_PAYLOAD_DATA;
				break;
			}
			case DataFrame::DATA_FRAME_MASK_KEY:
				// TODO: Check size of buf
				data_frame.mask_key[0] = buf[offset];
				data_frame.mask_key[1] = buf[++offset];
				data_frame.mask_key[2] = buf[++offset];
				data_frame.mask_key[3] = buf[++offset];
			case DataFrame::DATA_FRAME_MASK_PAYLOAD_DATA:
			{
				++offset;
				size_t mask_offset = 0;
				size_t payload_data_it = 0;
				while (offset + 4 < bytes)
				{
					mask_offset = 0;
					data_frame.payload_data[payload_data_it] = buf[offset] ^ data_frame.mask_key[mask_offset];
					data_frame.payload_data[++payload_data_it] = buf[++offset] ^ data_frame.mask_key[++mask_offset];
					data_frame.payload_data[++payload_data_it] = buf[++offset] ^ data_frame.mask_key[++mask_offset];
					data_frame.payload_data[++payload_data_it] = buf[++offset] ^ data_frame.mask_key[++mask_offset];
					++payload_data_it;
					++offset;
				}

				data_frame.payload_data[payload_data_it] = '\0';
				std::cout << "data: " << data_frame.payload_data << std::endl;

				data_frame.state = DataFrame::DATA_FRAME_END_OF_DATA;

				break;
			}
			case DataFrame::DATA_FRAME_PAYLOAD_DATA:
				assert(!data_frame.mask);
				data_frame.state = DataFrame::DATA_FRAME_END_OF_DATA;
				break;
			case DataFrame::DATA_FRAME_END_OF_DATA:
					// dump packet?
			default:
				break;
			}

			++offset;
		}
	}

	return 0;
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

/**
 * InternalSend
 *
 * Return values:
 * -1 Send data fail.
 * -2 Socket in.

 */
int InternalSend(int fd, size_t size, const char *data)
{
	::send(fd, data, size, 0);

	return 0;
}