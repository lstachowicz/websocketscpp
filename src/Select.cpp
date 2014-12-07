//
//  Select.cpp
//  websocketscpp
//
//  Created by Stachowicz, Lukasz on 27/11/14.
//  Copyright (c) 2014 Łukasz Stachowicz. All rights reserved.
//

#include "Select.h"
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <iostream>
#include <vector>

int Select::quit(false);

Select::Select()
{
}

Select::~Select()
{
}

int Select::Add(int socket)
{
	std::cout << "Add socket " << socket << std::endl;
	sockets.push_back(socket);
	return 0;
}

int Select::Rem(int socket)
{
	auto it = sockets.begin();

	while (it != sockets.end())
	{
		if (*it == socket)
		{
			std::cout << "Rem socket " << socket << std::endl;
			sockets.erase(it++);
			return 0;
		}
		else
		{
			it++;
		}
	}

	return -1;
}

int Select::Wait(int ms, std::vector<int> &ready)
{
	timeval tv;
	memset(&tv, 0, sizeof(tv));
	tv.tv_sec = ms;

	fd_set fds;
	FD_ZERO(&fds);
	int fd_max = 0;

	for (auto i : sockets)
	{
		FD_SET(i, &fds);

		if (i + 1 > fd_max)
			fd_max = i + 1;
	}

	int retval = 2;

	while (retval == 2)
	{
		int selected = ::select(fd_max, &fds, nullptr, nullptr, &tv);

		if (selected == -1)
		{
			std::cerr << "Interrupt while waiting for new events error no " << errno << " "  << strerror(errno) << " Closing program." << std::endl;

			if ((errno == EBADF) || (errno == EINVAL) || ((errno == EINTR) && (quit == true)))
			{
				retval = -1;
			}
		}
		else if (selected == 0)
		{
			retval = 0;
		}
		else
		{
			ready.clear();
			for (auto i : sockets)
			{
				if (FD_ISSET(i, &fds))
				{
					ready.push_back(i);
					retval = 1;
				}
			}
		}
	}

	return retval;
}

void Select::Quit(bool value)
{
	quit = value;
}