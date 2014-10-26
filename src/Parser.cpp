//
//  Parser.cpp
//  websocketscpp
//
//  Created by Stachowicz, Lukasz on 23/10/14.
//  Copyright (c) 2014 Łukasz Stachowicz. All rights reserved.
//

#include "Parser.h"

#include <stdio.h>
#include <iostream>

using namespace WebSocketCpp;

Parser::Parser(const char* data, long len)
	: parser_error(0)
{
	long readed { 0 };

	while ((readed + 1 <= len) && ((*data + readed) != '\x0d') && (*(data + readed + 1) != '\x0a'))
	{
		header += *(data + readed);
		++readed;
	}

	++readed;
	++readed;

	std::cout << "Header: " << header << std::endl;

	parser_error = parseData(data + readed, len - readed);
}

Parser::~Parser()
{
}

const std::string Parser::operator[](const std::string& key)
{
	return map[key];
}

int Parser::parseData(const char *data, long len)
{
	std::string key;
	std::string value;

	key.reserve(1024);
	value.reserve(1024);

	long readed { 0 };

	while ((readed + 1 <= len) && ((*data + readed) != '\x0d') && (*(data + readed + 1) != '\x0a'))
	{
		key.clear();
		value.clear();

		while ((readed + 1 <= len)
				&& ((*data + readed) != '\x0d')
				&& (*(data + readed + 1) != '\x0a')
				&& (*(data + readed) != ':')
				&& (*(data + readed + 1) != ' '))
		{
			key += *(data + readed);
			++readed;
		}

		++readed;
		++readed;

		while ((readed + 1 <= len)
			   && ((*data + readed) != '\x0d')
			   && (*(data + readed + 1) != '\x0a'))
		{
			value += *(data + readed);
			++readed;
		}

		++readed;
		++readed;

		std::cout << "Adding " << key << " value " << value << std::endl;
		map[key] = value;
	}

	return PARSER_ERROR_NONE;
}

int Parser::GetParserError()
{
	return 0;
}