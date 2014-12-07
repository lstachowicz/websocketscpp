//
//  Select.h
//  websocketscpp
//
//  Created by Stachowicz, Lukasz on 27/11/14.
//  Copyright (c) 2014 Łukasz Stachowicz. All rights reserved.
//

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

	int Wait(int ms, std::vector<int> &ready);

	void Quit(bool value = true);

private:
	std::vector<int> sockets;

	static int quit;
};

#endif // WEBSOCKETSCPP_SELECT
