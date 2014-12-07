#include "Server.h"

#include <iostream>
#include <random>

int main(int argc, char** argv)
{
	WebSocketCpp::Server server;

	srand(static_cast<unsigned int>(time(nullptr)));

	int port = 3000 + (rand() % 5);

	if (server.CreateServer(port))
	{
		std::cout << "Created server at port: " << port << std::endl;
	}
	else
	{
		std::cerr << "Could not create server at port [" << port << "]" << std::endl;
		return -1;
	}

	server.Run();
}
