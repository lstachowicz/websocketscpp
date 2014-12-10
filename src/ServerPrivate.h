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
const size_t MAX_SEND_DATA = 1024;
	
public:
	ServerPrivate();
	~ServerPrivate();

	int Bind(short port);
	int Wait(int time_ms);
	
	int Send();
	
	void SetCallbackServer(Server *server);

private:
	Server *callback_server;
	
	int InternalSend(int fd, size_t size, const char *data);

	int ConnectionAdd(int fd);
	int ConnectionHandshake(int fd);
	int ConnectionHandle(int fd);
	int ConnectionClose(int fd);

	int port = -1;

	std::map<int, std::shared_ptr<ClientConnection> > connections;
	fd_set fd_connections;

	int max_listen = 5;

	int listener;
	Select selector;
};

}
#endif // WEBSOCKETCPP_SERVER
