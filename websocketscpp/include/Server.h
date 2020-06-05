#ifndef WEBSOCKETSCPP_SERVER
#define WEBSOCKETSCPP_SERVER

#include <sys/types.h>
#include <string>

/**
 * @class Server
 *
 * @brief Creating instance of server.
 *
 * @note Server class must be overrider to manage a connections.
 */
namespace WebSocketCpp {
	
class Server
{
	
friend class ServerPrivate;

public:
	Server();
	virtual ~Server();
	
	/**
	 * List of possible data types; used in \a Send function.
	 * @see Send function.
	 */
	enum Data
	{
		Text
		, Binary
	};
	
	/**
	 * Create server and bind to port.
	 * @param port					: [in] port to listen on
	 *
	 * @return on success function return true.
	 */
	bool Bind(int port);
	
	/**
	 * Wait for any server activity e.i. new connection, close connection,
	 * new message. Server could be stopped by using function
	 * Stop.
	 * @param time_ms				: [in] wait time in milis
	 *
	 * @note if time_ms is set to max value of unsigned int then time will
	 * be unlimited.
	 *
	 * @return on new event return value is 1;
	 *         on error return value is -1;
	 *         on timeout return value is 0.
	 *
	 * @see Stop function.
	 */
	int Wait(unsigned int time_ms);
	
	/**
	 * Wakeup server. This will cause immediately stop waiting on connection.
	 *
	 * @note if server is not waiting for event this function will not have
	 * any effect.
	 *
	 * @see Wait function.
	 */
	void Stop();
	
	/**
	 * Send message to client
	 * @param socket				: [in] client socket
	 * @param data 					: [in] data to send
	 * @param data_size				: [in] size of data in bytes
	 * @param flag					: [in] determine type of data
	 *
	 * @note This function is used to send data to client. This is
	 * prefered way in order to keep connection alive. Socket can be obtain
	 * from function Opened and it is avaliable until Closed will not be
	 * called with given socket.
	 *
	 * @return XXX.
	 *
	 * @see Opened function.
	 * @see Closed function.
	 * @see DataFlag enumeration.
	 */
	int Send(int socket, const char* data, const size_t data_size, Data flag = Text);

	/**
	 * Function to close connection. Socket will be immediately closed and Closed
	 * will be called.
	 * @param socket				: [in] client socket
	 *
	 * @see Closed function.
	 *
	 * @return on success function return true.
	 */
	bool Close(int socket);
	
protected:
	/**
	 * Callback function to receive data from server. Function will be called
	 * immediately after receive data from client.
	 * @param socket				: [out] client socket
	 * @param data 					: [out] data to send
	 * @param data_size				: [out] size of data in bytes
	 * @param flag					: [out] determine type of data
	 *
	 * @note This function must be override. In order to store data create a copy.
	 *
	 * @return XXX
	 *
	 * @see Opened function.
	 * @see ServerDataType enumeration.
	 */
	virtual void Recv(int socket, const char* data, const size_t data_size, const Data flag = Text) = 0;
	
	/**
	 * Callback function to inform about new connection. Function will be called
	 * immediately after receive new client connection.
	 * @param socket				: [out] new client socket
	 * @param address 				: [out] IP address of connection
	 *
	 * @note This function must be override. Socket should be save in order to
	 * determine from whom message is and to whom send message. Socket will be
	 * avaliable until Closed handler is not called.
	 *
	 * @see Send function.
	 * @see Recv function.
	 * @see Close enumeration.
	 */
	virtual void Opened(int socket, const std::string& address) = 0;
	
	/**
	 * Callback function to inform about closed connection. Function will be
	 * called immediately after client connection close.
	 * @param socket				: [out] new client socket
	 *
	 * @note This function must be override. Socket should be droped it will
	 * not be avaliable.
	 */
	virtual void Closed(int socket) = 0;
	
private:
	void DoCallRecv(int socket, const char* data, const size_t data_size, const Data flag = Text);
	void DoCallOpened(int socket, const std::string& address);
	void DoCallClosed(int socket);
	
	void* server_private;
};
	
};

#endif // WEBSOCKETSCPP_SERVER
