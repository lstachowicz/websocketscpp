#include "ServerInstance.h"

#include <iostream>
#include <random>

int main(int argc, char** argv)
{
	ServerInstance server;

	srand(static_cast<unsigned int>(time(nullptr)));

	int port = 3000;

	if (server.Bind(port))
	{
		std::cout << "Created server at port: " << port << std::endl;
	}
	else
	{
		std::cerr << "Could not create server at port [" << port << "]"
		          << std::endl;
		
		return -1;
	}

	server.Run();
}
