#ifndef WEBSOCKETCPP_PARSER
#define WEBSOCKETCPP_PARSER

#include <map>
#include <string>
#include <vector>

namespace WebSocketCpp {

class ParserBase
{
public:
	
	enum ParserError
	{
		PARSER_ERROR_NONE = 0
	};
	
	ParserError GetError();
	std::string GetErrorString();
	
	virtual ParserError Encode() = 0;
	virtual ParserError Decode() = 0;
	
protected:
	ParserError error = PARSER_ERROR_NONE;
};
	
class ParserHtml : ParserBase
{
public:
	std::string operator[](const std::string& key);
	
	std::string GetHeader();
	void SetHeader(const std::string &header);
	void SetKey(const std::string &key, const std::string &value = "");
	
private:
	std::string data_header;
	std::map<std::string, std::string> key_value;
};
	
class Parser : public ParserBase
{
public:
	Parser(const char* data, long len);
	~Parser();

	const std::string operator[](const std::string& key);
private:
	int parseData(const char* data, long len);

	int parser_error;

	std::string header;
	std::map<std::string, std::string> map;
};

struct DataFrame
{
	/*
	 * Opcode for date defined in websocket protocol
	 */
	enum DataFrameOpcode
	{
		DATA_FRAME_OPCODE_CONTINUATION	= 0
		, DATA_FRAME_OPCODE_TEXT
		, DATA_FRAME_OPCODE_BINARY
		, DATA_FRAME_OPCODE_RES_3
		, DATA_FRAME_OPCODE_RES_4
		, DATA_FRAME_OPCODE_RES_5
		, DATA_FRAME_OPCODE_RES_6
		, DATA_FRAME_OPCODE_RES_7
		, DATA_FRAME_OPCODE_CLOSE
		, DATA_FRAME_OPCODE_PING
		, DATA_FRAME_OPCODE_PONG
		, DATA_FRAME_OPCODE_RES_B
		, DATA_FRAME_OPCODE_RES_C
		, DATA_FRAME_OPCODE_RES_D
		, DATA_FRAME_OPCODE_RES_E
		, DATA_FRAME_OPCODE_RES_F
	} opcode;


	/*
	 * Bit masks for date defined in websocket protocol
	 */
	enum DataFrameBit
	{
		DATA_FRAME_BIT_ERROR			= 0x80
		, DATA_FRAME_BIT_FINAL			= 0x80
		, DATA_FRAME_BIT_RSV1			= 0x40
		, DATA_FRAME_BIT_RSV2			= 0x20
		, DATA_FRAME_BIT_RSV3			= 0x10
		, DATA_FRAME_BIT_OPCODE			= 0xf
		, DATA_FRAME_BIT_MASK			= 0x80
		, DATA_FRAME_BIT_PAYLOAD_LENGHT	= 0x7f
	};

	/*
	 * State of data parser
	 */
	enum DataFrameState
	{
		DATA_FRAME_ERROR = 0
		, DATA_FRAME_FINAL
		, DATA_FRAME_RSV1
		, DATA_FRAME_RSV2
		, DATA_FRAME_RSV3
		, DATA_FRAME_OPCODE
		, DATA_FRAME_MASK
		, DATA_FRAME_PAYLOAD_LENGHT
		, DATA_FRAME_MASK_KEY
		, DATA_FRAME_PAYLOAD_DATA
		, DATA_FRAME_PAYLOAD_DATA_EX
		, DATA_FRAME_MASK_PAYLOAD_DATA
		, DATA_FRAME_END_OF_DATA
	} state;

	/*
	 * Determine if parser success.
	 */
	bool valid;

	/*
	 * Received data fields documented in websocket protocol
	 */
	bool final;
	bool rsv1, rsv2, rsv3;
	bool mask;
	unsigned char payload_length;
	unsigned long long payload_length_ex;
	unsigned char mask_key[4];
	std::vector<char> payload_data;


	/*
	 * Protocol parser helper
	 */
	unsigned short frame_helper;

	DataFrame()
	: state(DATA_FRAME_FINAL)
	, valid(false)
	, final(0)
	, rsv1(false), rsv2(false), rsv3(false)
	, opcode(DATA_FRAME_OPCODE_CONTINUATION)
	, mask(0)
	, payload_length(0)
	, payload_length_ex(0)
	, mask_key { 0, 0, 0, 0 }
	, frame_helper(0)
	{ }
};

extern DataFrame::DataFrameState FeedParser(DataFrame &data_frame, const unsigned char c);
extern int CreateFood(std::vector<char> &data_out, const char* data, const size_t size);
std::string printopcode(const char &value);

}
#endif // WEBSOCKETCPP_PARSER
