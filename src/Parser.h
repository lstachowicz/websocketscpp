//
//  Parser.h
//  websocketscpp
//
//  Created by Stachowicz, Lukasz on 23/10/14.
//  Copyright (c) 2014 Łukasz Stachowicz. All rights reserved.
//

#ifndef WEBSOCKETCPP_PARSER
#define WEBSOCKETCPP_PARSER

#include <map>
#include <string>

namespace WebSocketCpp {

class Parser
{
public:
	enum ParserError
	{
		PARSER_ERROR_NONE = 0
	};

	Parser(const char* data, long len);
	~Parser();

	const std::string operator[](const std::string& key);

	int GetParserError();
private:
	int parseData(const char* data, long len);

	int parser_error;

	std::string header;
	std::map<std::string, std::string> map;
};

}
#endif // WEBSOCKETCPP_PARSER
