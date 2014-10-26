#include "Server.h"

#include <iostream>

int main(int argc, char** argv)
{
	WebSocketCpp::Server server;

	server.CreateServer(3000);

	server.Run();
}
