#ifndef WEBSOCKETCPP_CLIENT_CONNECTION
#define WEBSOCKETCPP_CLIENT_CONNECTION

#include <string>
#include "Parser.h"
#include "Server.h"

namespace WebSocketCpp {

class ClientConnection
{
public:
	enum ConnectionState
	{
		CONNECTION_NONE		= 0
		, CONNECTION_ACCEPT
	};
	
	ClientConnection(int fd, long port, const std::string& address);
	~ClientConnection();

	ConnectionState getState();
	void setState(ConnectionState state);

private:
	void saveMessage();
	void cleanMessage();
	void cleanDataFrame();

	int connection_fd = -1;
	long connection_port = -1;
	std::string connection_address;

	ConnectionState connection_state;

	DataFrame data_frame;

	friend class ServerPrivate;
};

	std::string ConnectionStateToString(ClientConnection::ConnectionState state);

}

#endif // WEBSOCKETCPP_CLIENTCONNECTION
