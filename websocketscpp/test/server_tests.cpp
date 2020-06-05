#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <string>

#include <Server.h>

class ServerMock : public WebSocketCpp::Server
{
	MOCK_METHOD4(Recv, void(int, const char*, const size_t, const WebSocketCpp::Server::Data));
	MOCK_METHOD2(Opened, void(int, const std::string&));
	MOCK_METHOD1(Closed, void(int));
};

TEST(ServerTests, ShouldFailedWhenWaitWithoutConnecting)
{
	testing::StrictMock<ServerMock> server;
	EXPECT_EQ(server.Wait(0), -1);
}

TEST(ServerTests, ShouldFaileWhenInvalidPort)
{
	testing::StrictMock<ServerMock> server;
	EXPECT_EQ(server.Bind(22), false);
}

TEST(ServerTests, ShouldBindPortSuccesfuly)
{
	testing::StrictMock<ServerMock> server;
	EXPECT_EQ(server.Bind(3050), true);
}

TEST(ServerTests, ShouldAcceptConnectionSuccesfuly)
{
	testing::StrictMock<ServerMock> server;
	EXPECT_EQ(server.Bind(3051), true);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	std::string host = "127.0.0.1";
	addr.sin_addr.s_addr = gethostname(&host[0], (size_t)host.size());
	addr.sin_port = htons(3051);

	int fd = socket(PF_INET, SOCK_STREAM, 0);
	connect(fd, (sockaddr*)&addr, sizeof(addr));
	sleep(1);
	close(fd);
}

TEST(ServerTest, ShouldFailedOnSendWhenNotConnected)
{
	testing::StrictMock<ServerMock> server;
	EXPECT_EQ(server.Send(0, "test", 4), -1);
}

TEST(ServerTest, ShouldFailedWhenCloseAmdNotOpened)
{
	testing::StrictMock<ServerMock> server;
	EXPECT_EQ(server.Close(0), false);
}
