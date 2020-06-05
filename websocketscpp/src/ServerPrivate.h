#ifndef WEBSOCKETCPP_SERVER_PRIVATE
#define WEBSOCKETCPP_SERVER_PRIVATE

#include "Select.h"
#include "Server.h"

#include <sys/types.h>

#include <map>
#include <memory>
#include <vector>

namespace WebSocketCpp {

class ClientConnection;
	
class ServerPrivate
{
// estimated MTU value
const size_t MAX_SEND_DATA = 1500;
	
public:
	explicit ServerPrivate(Server *server);
	~ServerPrivate();

	int Bind(short port);
	int Wait(unsigned int time_ms);
	
	int Send(int socket, const char *data, const size_t data_size, Server::Data flag);
	
	void SetCallbackServer(Server *server);

private:
	Server *callback_server;

	int ConnectionAdd(int fd);
	int ConnectionClose(int fd);
	int ConnectionHandshake(int fd);
	int ConnectionHandle(int fd);

	int port = -1;

	std::map<int, std::shared_ptr<ClientConnection> > connections;
	fd_set fd_connections;

	int max_listen = 5;

	int listener;
	Select selector;
};

}
#endif // WEBSOCKETCPP_SERVER
