#include <gtest/gtest.h>

#include <string>

#include <Parser.h>

TEST(ParserHtmlTests, ShouldSetHeaderSucessful)
{
	std::string header = "SOME_HEADER";

	WebSocketCpp::ParserHtml parser;
	parser.SetHeader(header);
	EXPECT_EQ(parser.GetHeader(), header);
}

TEST(ParserHtmlTests, ShouldSetKeySucessful)
{
	std::string key = "SOME_KEY";
	std::string value = "SOME_VALUE";

	WebSocketCpp::ParserHtml parser;
	parser.SetKey(key, value);
	EXPECT_EQ(parser[key], value);
}

TEST(printopcodeTests, TestOpcode)
{
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_CONTINUATION), "DATA_FRAME_OPCODE_CONTINUATION");
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_TEXT), "DATA_FRAME_OPCODE_TEXT");
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_BINARY), "DATA_FRAME_OPCODE_BINARY");
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_RES_3), "DATA_FRAME_OPCODE_RES_3");
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_RES_4), "DATA_FRAME_OPCODE_RES_4");
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_RES_5), "DATA_FRAME_OPCODE_RES_5");
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_RES_6), "DATA_FRAME_OPCODE_RES_6");
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_RES_7), "DATA_FRAME_OPCODE_RES_7");
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_CLOSE), "DATA_FRAME_OPCODE_CLOSE");
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_PING), "DATA_FRAME_OPCODE_PING");
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_PONG), "DATA_FRAME_OPCODE_PONG");
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_RES_B), "DATA_FRAME_OPCODE_RES_B");
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_RES_C), "DATA_FRAME_OPCODE_RES_C");
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_RES_D), "DATA_FRAME_OPCODE_RES_D");
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_RES_E), "DATA_FRAME_OPCODE_RES_E");
	EXPECT_EQ(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_RES_F), "DATA_FRAME_OPCODE_RES_F");
	ASSERT_DEATH(WebSocketCpp::printopcode(WebSocketCpp::DataFrame::DATA_FRAME_OPCODE_RES_F + 1), "Unknown opcode");
}
