#ifndef WEBSOCKETSCPP_SELECT
#define WEBSOCKETSCPP_SELECT

#include "Select.h"

#include <sys/select.h>
#include <vector>

class Select
{
public:
	Select();
	~Select();

	int Add(int socket);
	int Rem(int socket);

	int Wait(unsigned int ms, std::vector<int> &ready);

	void Quit(bool value = true);

private:
	std::vector<int> sockets;

	static int quit;
};

#endif // WEBSOCKETSCPP_SELECT
