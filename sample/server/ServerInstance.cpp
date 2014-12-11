//
//  ServerInstance.cpp
//  websocketscpp_sample
//
//  Created by Stachowicz, Lukasz on 10/12/14.
//  Copyright (c) 2014 Łukasz Stachowicz. All rights reserved.
//

#include "ServerInstance.h"

#include <iostream>
#include <string>

ServerInstance::ServerInstance()
{
}

ServerInstance::~ServerInstance()
{
}

void ServerInstance::Recv(int socket, const char *data, const size_t data_size, const Data flag)
{
	auto it = users.find(socket);
	
	if (it != users.end())
	{
		std::string message(data, data_size);
		std::cout << "ServerInstance: New message from [ " << socket << " ] "
				  << " type=" << (flag == Text ? "Text" : "Binary")
		          << " size=" << data_size
		          << " data=" << (flag == Text ? message : "BINARY DATA")
				  << "." << std::endl;
	}
}

void ServerInstance::Opened(int socket, const std::string &address)
{
	auto it = users.find(socket);
	
	if (it == users.end())
	{
		std::cout << "ServerInstance: New user [ " << socket << " ] from [ " << address
				  << " ]." << std::endl;
		
		users.insert(socket);
	}
	else
	{
		std::cout << "ServerInstance: User [ " << socket << " ] already on list." << std::endl;
	}
}

void ServerInstance::Closed(int socket)
{
	auto it = users.find(socket);
	
	if (it != users.end())
	{
		std::cout << "ServerInstance: Delete user [ " << socket << " ]." << std::endl;
		
		users.erase(it);
	}
	else
	{
		std::cout << "ServerInstance: User [ " << socket << " ] no exist on list." << std::endl;
	}
}

void ServerInstance::Run()
{
	bool quit = false;
	
	int send = 0;
	
	while(!quit)
	{
		quit = (Wait(std::numeric_limits<unsigned int>::max()) == -1);
		++send;
		
		if (send > 10)
		{
			for (auto it : users)
			{
				int socket = it;
				std::string message = "ServerInstance: Foo";
				std::cout << "ServerInstance: Sending message "
				          << Send(socket, message.c_str(), message.length())
			              << std::endl;
			}
			send = 0;
		}
	}
}