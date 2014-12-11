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
#include <assert.h>

using namespace WebSocketCpp;

ParserBase::ParserError ParserBase::GetError()
{
	return error;
}

std::string ParserBase::GetErrorString()
{
	switch (error)
	{
		case PARSER_ERROR_NONE:
			return "PARSER_ERROR_NONE";
  		default:
			assert(!"Unknow parser error");
			return "Unknow parser error";
	}
}

std::string ParserHtml::operator[](const std::string &key)
{
	return key_value[key];
}

std::string ParserHtml::GetHeader()
{
	return data_header;
}

void ParserHtml::SetHeader(const std::string &header)
{
	data_header = header;
}

void ParserHtml::SetKey(const std::string &key, const std::string &value)
{
	key_value[key] = value;
}

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

DataFrame::DataFrameState WebSocketCpp::FeedParser(DataFrame &data_frame, const unsigned char c)
{
	switch (data_frame.state)
	{
		case DataFrame::DATA_FRAME_FINAL:
			data_frame.final = c & DataFrame::DATA_FRAME_BIT_FINAL;
		case DataFrame::DATA_FRAME_RSV1:
			data_frame.rsv1 = c & DataFrame::DATA_FRAME_BIT_RSV1;
		case DataFrame::DATA_FRAME_RSV2:
			data_frame.rsv2 = c & DataFrame::DATA_FRAME_BIT_RSV2;
		case DataFrame::DATA_FRAME_RSV3:
			data_frame.rsv3 = c & DataFrame::DATA_FRAME_BIT_RSV3;
		case DataFrame::DATA_FRAME_OPCODE:
			data_frame.opcode = static_cast<DataFrame::DataFrameOpcode>(c & DataFrame::DATA_FRAME_BIT_OPCODE);
			data_frame.state = DataFrame::DATA_FRAME_MASK;
			break;
		case DataFrame::DATA_FRAME_MASK:
			data_frame.mask = c & DataFrame::DATA_FRAME_BIT_MASK;
		case DataFrame::DATA_FRAME_PAYLOAD_LENGHT:
		{
			switch (c & DataFrame::DATA_FRAME_BIT_PAYLOAD_LENGHT)
			{
				case 127:
					data_frame.frame_helper = 56;
					data_frame.state = DataFrame::DATA_FRAME_PAYLOAD_DATA_EX;
					break;
				case 126:
					data_frame.frame_helper = 8;
					data_frame.state = DataFrame::DATA_FRAME_PAYLOAD_DATA_EX;
					break;
				default:
					data_frame.payload_length_ex = static_cast<size_t>(c & DataFrame::DATA_FRAME_BIT_PAYLOAD_LENGHT);

					data_frame.state = data_frame.mask
						? DataFrame::DATA_FRAME_MASK_KEY
						: DataFrame::DATA_FRAME_PAYLOAD_DATA;
					break;
			}

			break;
		}
		case DataFrame::DATA_FRAME_PAYLOAD_DATA_EX:
			if (data_frame.frame_helper > 0)
			{
				#if !defined(__LP64__)
				if (data_frame.payload_length_ex_block >= 32)
					break;
				#endif

				data_frame.payload_length_ex |= static_cast<size_t>(c) << data_frame.frame_helper;
				data_frame.frame_helper -= 8;
			}
			else
			{
				data_frame.payload_length_ex |= static_cast<size_t>(c);
				data_frame.state = data_frame.mask
					? DataFrame::DATA_FRAME_MASK_KEY
					: DataFrame::DATA_FRAME_PAYLOAD_DATA;
			}
			break;
		case DataFrame::DATA_FRAME_MASK_KEY:
		{
			data_frame.mask_key[data_frame.frame_helper] = static_cast<unsigned char>(c);
			if (++data_frame.frame_helper == 4)
			{
				data_frame.frame_helper = 0;
				data_frame.state = DataFrame::DATA_FRAME_MASK_PAYLOAD_DATA;
			}
			break;
		}
		case DataFrame::DATA_FRAME_MASK_PAYLOAD_DATA:
		{
			assert(data_frame.mask);

			data_frame.payload_data.push_back(static_cast<char>(c ^ data_frame.mask_key[data_frame.frame_helper++]));

			if (data_frame.payload_data.size() == data_frame.payload_length_ex)
				data_frame.state = DataFrame::DATA_FRAME_END_OF_DATA;
			else if (data_frame.frame_helper == 4)
				data_frame.frame_helper = 0;

			break;
		}
		case DataFrame::DATA_FRAME_PAYLOAD_DATA:
			assert(!data_frame.mask);

			data_frame.payload_data.push_back(static_cast<char>(c));

			if (data_frame.payload_data.size() == data_frame.payload_length_ex)
				data_frame.state = DataFrame::DATA_FRAME_END_OF_DATA;

			break;
		case DataFrame::DATA_FRAME_ERROR:
			assert(!"Error in data frame");
		default:
			assert(!"Undefined state of parser");
			break;
	}

	return data_frame.state;
}

int WebSocketCpp::CreateFood(std::vector<char> &data_out, const char* data, const size_t size)
{
	data_out.clear();

	unsigned char c { 0 };

	c |= DataFrame::DATA_FRAME_BIT_FINAL;
	c |= DataFrame::DATA_FRAME_OPCODE_TEXT;
	data_out.push_back(c);

	c = 0;
    #if CPPWEBSOCKET_MASK_DATA
	c |= DataFrame::DATA_FRAME_BIT_MASK;
    #endif

	if (size > std::numeric_limits<unsigned short>::max())
	{
		c |= 127;
		data_out.push_back(c);

		for (int i = 56; i > 0; i-=8)
		{
			#if !defined(__LP64__)
			if (data_frame.payload_length_ex_block >= 32)
				data_out.push_back(0);
			#endif
			data_out.push_back((size >> i) & 0xFF);
		}
	}
	else if (size > 125)
	{
		c |= 126;
		data_out.push_back(c);
		data_out.push_back((size >> 8) & 0xFF);
		data_out.push_back(size & 0xFF);
	}
	else
	{
		c |= size;
		data_out.push_back(c);
	}

	//
	// Data is masked only by client.
	// Masked data from server is not requested and Safari
	// could have problem with this :)
	// So it can be used only by Firefox?
	#if CPPWEBSOCKET_MASK_DATA
	char mask[4];
	for(int i = 0; i < sizeof(mask); i++)
	{
		mask[i] = rand() % 256;
		data_out.push_back(mask[i]);
	}

	char *data_start = const_cast<char *>(data);
	char *data_end = data_start + size;

	while (data_start + sizeof(mask) < data_end)
	{
		data_out.push_back(*(data_start++) ^ mask[0]);
		data_out.push_back(*(data_start++) ^ mask[1]);
		data_out.push_back(*(data_start++) ^ mask[2]);
		data_out.push_back(*(data_start++) ^ mask[3]);
	}

	for (int i = 0; i < data_end - data_start; ++i)
	{
		data_out.push_back(data_start[i] ^ mask[i]);
	}

	#else
	size_t last_index = data_out.size();
	data_out.resize(data_out.size() + size);
	memcpy(&(data_out.data()[last_index]), data, size);
	#endif

	return 0;
}

std::string WebSocketCpp::printopcode(const char& value)
{
	switch (value)
	{
		case DataFrame::DATA_FRAME_OPCODE_CONTINUATION:
			return "DATA_FRAME_OPCODE_CONTINUATION";
		case DataFrame::DATA_FRAME_OPCODE_TEXT:
			return "DATA_FRAME_OPCODE_TEXT";
		case DataFrame::DATA_FRAME_OPCODE_BINARY:
			return "DATA_FRAME_OPCODE_BINARY";
		case DataFrame::DATA_FRAME_OPCODE_RES_3:
			return "DATA_FRAME_OPCODE_RES_3";
		case DataFrame::DATA_FRAME_OPCODE_RES_4:
			return "DATA_FRAME_OPCODE_RES_4";
		case DataFrame::DATA_FRAME_OPCODE_RES_5:
			return "DATA_FRAME_OPCODE_RES_5";
		case DataFrame::DATA_FRAME_OPCODE_RES_6:
			return "DATA_FRAME_OPCODE_RES_6";
		case DataFrame::DATA_FRAME_OPCODE_RES_7:
			return "DATA_FRAME_OPCODE_RES_7";
		case DataFrame::DATA_FRAME_OPCODE_CLOSE:
			return "DATA_FRAME_OPCODE_CLOSE";
		case DataFrame::DATA_FRAME_OPCODE_PING:
			return "DATA_FRAME_OPCODE_PING";
		case DataFrame::DATA_FRAME_OPCODE_PONG:
			return "DATA_FRAME_OPCODE_PONG";
		case DataFrame::DATA_FRAME_OPCODE_RES_B:
			return "DATA_FRAME_OPCODE_RES_B";
		case DataFrame::DATA_FRAME_OPCODE_RES_C:
			return "DATA_FRAME_OPCODE_RES_C";
		case DataFrame::DATA_FRAME_OPCODE_RES_D:
			return "DATA_FRAME_OPCODE_RES_D";
		case DataFrame::DATA_FRAME_OPCODE_RES_E:
			return "DATA_FRAME_OPCODE_RES_E";
		case DataFrame::DATA_FRAME_OPCODE_RES_F:
			return "DATA_FRAME_OPCODE_RES_F";
		default:
			assert(!"Unknow opcode");
			return "Unknow opcode";
	}
}