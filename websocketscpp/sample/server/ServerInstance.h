#ifndef WEBSOCKETSCPP_SAMPLE_SERVER_INSTANCE
#define WEBSOCKETSCPP_SAMPLE_SERVER_INSTANCE

#include "Server.h"

#include <set>

class ServerInstance : public WebSocketCpp::Server
{
public:
	ServerInstance();
	virtual ~ServerInstance();
	
	void Run();
	
protected:
	virtual void Recv(int socket, const char* data, const size_t data_size, const Data flag = Text);
	
	virtual void Opened(int socket, const std::string& address);
	
	virtual void Closed(int socket);
	
private:
	std::set<int> users;
};

#endif // WEBSOCKETSCPP_SAMPLE_SERVER_INSTANCE
