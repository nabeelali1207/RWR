/*
 * LinuxTcpServer.cpp
 *
 *  Created on: Jan 19, 2023
 *      Author: user
 */

#include "LinuxTcpServer.h"

LinuxTcpServer::~LinuxTcpServer()
{
	// TODO Auto-generated destructor stub
}

LinuxTcpServer::LinuxTcpServer(uint32_t _port,bool _printStats, uint32_t backLog
		)
{
	port = _port;
	socketCreatedFlag = socketBindSuccessFlag = serverOkayFlag = false;
	serverSocket = -1;
	backLogSize = backLog;

	lastCheckForConn = lastRecvTime = lastSendTime = lastTransactionTime = 0;
	lastErrorsString.reserve(1024);

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);

	keepAlives = true;
	keepAliveInterval = 1;
	keepAliveCount = 3;
	keepAliveDelay = 10;

	accConnReqCnt = activeClientsCount = 0;

	printPeriodicStats = _printStats;

}

void LinuxTcpServer::setupSocket()
{
	bool newSocketCreatedFlag = false;
	if (socketCreatedFlag == false)
	{
		serverSocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		if (serverSocket < 0)
		{
			lastErrorsString += "Create Socket Failed!\n";
			socketCreatedFlag = false;
			socketBindSuccessFlag = false;
			return;
		}
		else
		{
			newSocketCreatedFlag = true;
			socketCreatedFlag = true;
			serverOkayFlag = false;
		}
	}

	int opt = 1;
	int res;

	if (newSocketCreatedFlag)
	{
		res = setsockopt(serverSocket, SOL_SOCKET,SO_REUSEADDR, &opt, sizeof(opt));
		//res = setsockopt(serverSocket, SOL_SOCKET,
		//SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
		if (res < 0)
		{
			socketBindSuccessFlag = false;
			socketCreatedFlag = false;
			serverOkayFlag = false;
			close(serverSocket);
			serverSocket = -1;
			lastErrorsString += "Socket setsockopt::SO_REUSEADDR Failed!\n";
			return;
		}
//		res = setsockopt(serverSocket, SOL_SOCKET, SOCK_NONBLOCK, &opt, sizeof(opt));
//		if (res < 0)
//		{
//			socketBindSuccessFlag = false;
//			socketCreatedFlag = false;
//			serverOkayFlag = false;
//			close(serverSocket);
//			serverSocket = -1;
//			lastErrorsString += "Socket setsockopt::SO_REUSEADDR Failed!\n";
//			return;
//		}
	}
}
void LinuxTcpServer::bindSocket()
{
	if (socketCreatedFlag == false)
	{
		return;
	}
	int res;
	if (socketBindSuccessFlag == false)
	{
		res = bind(serverSocket, (struct sockaddr*) &serverAddress,
				sizeof(serverAddress));
		if (res < 0)
		{
			lastErrorsString += "Socket bind Failed!\n";

			socketCreatedFlag = false;
			serverOkayFlag = false;
			return;
		}
		else
		{
			socketBindSuccessFlag = true;
		}
		res = listen(serverSocket, 10);
		if (res < 0)
		{
			lastErrorsString += "Socket listen Failed!\n";

			socketCreatedFlag = false;
			serverOkayFlag = false;
			return;
		}
		else
		{
			serverOkayFlag = true;
		}
	}
}
bool LinuxTcpServer::startListening()
{
	setupSocket();
	bindSocket();
	if (printPeriodicStats)
	{
		if (serverOkayFlag)
		{
			printf("TcpServer started at port:%d\n", port);
		}
		else
		{
			printf("TcpServer starting failed at port:%d\n", port);
			printf("TcpServer errors: %s", lastErrorsString.c_str());
			lastErrorsString = "";
		}

	}
	return serverOkayFlag;
}
bool LinuxTcpServer::setupNewClient(ClientInfo &tempClient)
{

	bool setupSuccessful = true;
	int32_t s = tempClient.socketFd;
	int32_t optval1, optval2, optval3, optval4;
	uint32_t optlen = sizeof(optval1);

	int32_t res1 = 1, res2 = 1, res3 = 1, res4 = 1;

	optval1 = (keepAlives == true ? 1 : 0);
	optval2 = keepAliveDelay;
	optval3 = keepAliveInterval;
	optval4 = keepAliveCount;

	res1 = setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &optval1, optlen);
	res2 = setsockopt(s, SOL_TCP, TCP_KEEPIDLE, &optval2, optlen);
	res3 = setsockopt(s, SOL_TCP, TCP_KEEPINTVL, &optval3, optlen);
	res4 = setsockopt(s, SOL_TCP, TCP_KEEPCNT, &optval4, optlen);

	if (res1 < 0)
	{
		lastErrorsString += "SetSocketOpt SOL_SOCKET::SO_KEEPALIVE Failed!\n";
		close(s);
		return false;
	}

	if (res2 < 0)
	{
		lastErrorsString += "SetSocketOpt SOL_TCP::TCP_KEEPIDLE Failed!\n";
		close(s);
		return false;
	}

	if (res3 < 0)
	{
		lastErrorsString += "SetSocketOpt SOL_TCP::TCP_KEEPINTVL Failed!\n";
		close(s);
		return false;
	}

	if (res4 < 0)
	{
		lastErrorsString += "SetSocketOpt SOL_TCP::TCP_KEEPCNT Failed!\n";
		close(s);
		return false;
	}

	res1 = getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &optval1, &optlen);
	res2 = getsockopt(s, SOL_TCP, TCP_KEEPIDLE, &optval2, &optlen);
	res3 = getsockopt(s, SOL_TCP, TCP_KEEPINTVL, &optval3, &optlen);
	res4 = getsockopt(s, SOL_TCP, TCP_KEEPCNT, &optval4, &optlen);

	if (res1 < 0)
	{
		lastErrorsString += "GetSocketOpt SOL_SOCKET::SO_KEEPALIVE Failed!\n";
		close(s);
		return false;
	}

	if (res2 < 0)
	{
		lastErrorsString += "GetSocketOpt SOL_TCP::TCP_KEEPIDLE Failed!\n";
		close(s);
		return false;
	}

	if (res3 < 0)
	{
		lastErrorsString += "GetSocketOpt SOL_TCP::TCP_KEEPINTVL Failed!\n";
		close(s);
		return false;
	}

	if (res4 < 0)
	{
		lastErrorsString += "GetSocketOpt SOL_TCP::TCP_KEEPCNT Failed!\n";
		close(s);
		return false;
	}

	tempClient.keepAliveState = (optval1 > 0);
	tempClient.keepAliveDelay = optval2;
	tempClient.keepAliveInterval = optval3;
	tempClient.keepAliveCount = optval4;
	return setupSuccessful;
}
void LinuxTcpServer::listenForNewConnection()
{
	if (serverOkayFlag == false)
	{
		return;
	}
	ClientInfo tempClient;
	socklen_t addressSize = sizeof(tempClient.clientAddress);
	tempClient.socketFd = accept4(serverSocket,
			(struct sockaddr*) &tempClient.clientAddress,
			(socklen_t*) &addressSize, SOCK_NONBLOCK);

	if (tempClient.socketFd > 0)
	{

		//new client connected, need to setup parameters
		bool setupSuccessful = setupNewClient(tempClient);

		if (printPeriodicStats && setupSuccessful)
		{
			unsigned char *bytes =
					(unsigned char*) &tempClient.clientAddress.sin_addr;
			printf("Tcp Server(%d): new client connected %d.%d.%d.%d:%d \n",
					port, bytes[0], bytes[1], bytes[2], bytes[3],
					tempClient.clientAddress.sin_port);
//			printf(" Client info\n");
		}
		else if (printPeriodicStats)
		{
			unsigned char *bytes =
					(unsigned char*) &tempClient.clientAddress.sin_addr;
			printf("Tcp Server(%d): Failed connection with %d.%d.%d.%d:%d \n",
					port, bytes[0], bytes[1], bytes[2], bytes[3],
					tempClient.clientAddress.sin_port);

			printf("Tcp Server(%d) errors: %s", port, lastErrorsString.c_str());
			lastErrorsString = "";
		}

		if (setupSuccessful)
		{
			tempClient.clientIndex = accConnReqCnt;
			clientSockets.push_back(tempClient);
			accConnReqCnt++;
		}
		else
		{
			close(tempClient.socketFd);
		}
	}
}
uint32_t LinuxTcpServer::readFromClients(char* buff, uint32_t maxSize)
{
	std::list<ClientInfo>::iterator it;
	int bytesRead=0;
	for (it = clientSockets.begin(); it != clientSockets.end();)
	{
		bytesRead = readFromClient(*it, buff, maxSize);
		if (bytesRead > 0)
		{
			break;
		}
		else if (bytesRead < 0)
		{
			if (printPeriodicStats)
			{
				ClientInfo & tempClient = *it;
				unsigned char *bytes =
						(unsigned char*) &tempClient.clientAddress.sin_addr;
				printf("Tcp Server(%d): Connection Dropped %d.%d.%d.%d:%d \n",
						port, bytes[0], bytes[1], bytes[2], bytes[3],
						tempClient.clientAddress.sin_port);

				printf("Tcp Server(%d) errors: %s", port,
						lastErrorsString.c_str());
				lastErrorsString = "";
			}

			close(it->socketFd);
			it = clientSockets.erase(it);
		}
		else
		{
			it++;
		}
	}
	if (bytesRead < 0)
	{
		bytesRead = 0;
	}
	return (uint32_t) bytesRead;

}

uint32_t LinuxTcpServer::broadCastToClients(char* buff, uint32_t size)
{
	std::list<ClientInfo>::iterator it;
	int bytesSent=0;
	for (it = clientSockets.begin(); it != clientSockets.end();)
	{
		bytesSent = writeToClient(*it, buff, size);
		if (bytesSent > 0)
		{
			break;
		}
		else if (bytesSent < 0)
		{
			if (printPeriodicStats)
			{
				ClientInfo & tempClient = *it;
				unsigned char *bytes =
						(unsigned char*) &tempClient.clientAddress.sin_addr;
				printf("Tcp Server(%d): Connection Dropped %d.%d.%d.%d:%d \n",
						port, bytes[0], bytes[1], bytes[2], bytes[3],
						tempClient.clientAddress.sin_port);

				printf("Tcp Server(%d) errors: %s", port,
						lastErrorsString.c_str());
				lastErrorsString = "";
			}
			close(it->socketFd);
			it = clientSockets.erase(it);
		}
		else
		{
			it++;
		}
	}
	if (bytesSent < 0)
	{
		bytesSent = 0;
	}
	return (uint32_t) bytesSent;
}
int32_t LinuxTcpServer::readFromClient(ClientInfo &c, char* buff,
		uint32_t maxSize)
{

	int bytesRead;
	bytesRead = recv(c.socketFd, buff, maxSize, MSG_DONTWAIT);
	if (bytesRead < 0)
	{
		if (errno == EAGAIN)
		{
			bytesRead = 0;
		}
		else
		{
			bytesRead = -1;
		}
	}
	return bytesRead;
}
int32_t LinuxTcpServer::writeToClient(ClientInfo &c, char* buff, uint32_t size)
{

	int bytesSent;
	bytesSent = send(c.socketFd, buff, size, 0);
	if (bytesSent < 0)
	{
		if (errno == EWOULDBLOCK)
		{

		}
		else
		{
			bytesSent = -1;
		}
	}
	return bytesSent;
}

void LinuxTcpServer::rawMethod()
{
	uint16_t port = 12346;
	struct sockaddr_in address;
	int server_fd, new_socket, valread;

	int addrlen = sizeof(address);
	int opt = 1;
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	//if (setsockopt(server_fd, SOL_SOCKET,
	//	SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	if (setsockopt(server_fd, SOL_SOCKET,SO_REUSEADDR, &opt, sizeof(opt)))
	{

	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);

	if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0)
	{
		perror("bind failed");
	}
	if (listen(server_fd, 10) < 0)
	{
		perror("listen");
	}

	char buffer[8000] = "HelloWorld";
	bool connectedFlag = false;
	while (1)
	{
		while ((new_socket = accept4(server_fd, (struct sockaddr*) &address,
				(socklen_t*) &addrlen, SOCK_NONBLOCK)) < 0)
		{
			usleep(50000);
		}
		int s = new_socket;
		int optval;
		socklen_t optlen = sizeof(optval);
		/* Check the status for the keepalive option */
		if (getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0)
		{
			perror("getsockopt()");
			close(s);
			exit(EXIT_FAILURE);
		}
		printf("SO_KEEPALIVE is %s\n", (optval ? "ON" : "OFF"));

		/* Set the option active */
		optval = 1;
		optlen = sizeof(optval);
		if (setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen) < 0)
		{
			perror("setsockopt()");
			close(s);
			exit(EXIT_FAILURE);
		}
		printf("SO_KEEPALIVE set on socket\n");
		/* Check the status again */
		if (getsockopt(s, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) < 0)
		{
			perror("getsockopt()");
			close(s);
			exit(EXIT_FAILURE);
		}
		printf("SO_KEEPALIVE is %s\n", (optval ? "ON" : "OFF"));
		optval = 10;
		if (setsockopt(s, SOL_TCP, TCP_KEEPIDLE, &optval, optlen) < 0)
		{
			perror("setsockopt() TCP_KEEPIDLE");
			close(s);
			exit(EXIT_FAILURE);
		}
		optval = 1;
		if (setsockopt(s, SOL_TCP, TCP_KEEPINTVL, &optval, optlen) < 0)
		{
			perror("setsockopt() TCP_KEEPINTVL");
			close(s);
			exit(EXIT_FAILURE);
		}
		optval = 3;
		if (setsockopt(s, SOL_TCP, TCP_KEEPCNT, &optval, optlen) < 0)
		{
			perror("setsockopt() TCP_KEEPCNT");
			close(s);
			exit(EXIT_FAILURE);
		}

		if (getsockopt(s, SOL_TCP, TCP_KEEPINTVL, &optval, &optlen) < 0)
		{
			perror("getsockopt()");
			close(s);
			exit(EXIT_FAILURE);
		}
		printf("TCP_KEEPINTVL is %d\n", optval);

		if (getsockopt(s, SOL_TCP, TCP_KEEPIDLE, &optval, &optlen) < 0)
		{
			perror("getsockopt()");
			close(s);
			exit(EXIT_FAILURE);
		}
		printf("TCP_KEEPIDLE is %d\n", optval);

		connectedFlag = true;
		printf("Socket Accepted\n");
		while (connectedFlag)
		{
			valread = recv(new_socket, buffer, 8000, MSG_DONTWAIT);
			if (valread > 0)
			{
				printf("count:%d %s\n", valread, buffer + 16);
			}
			else if (valread == -1)
			{
				if (errno == EAGAIN)
				{

				}
				else
				{
					printf("Socket Error\n");
					close(new_socket);
					connectedFlag = false;
					break;
				}
			}

//			int bytesSent = send(new_socket, buffer, 1, 0);
//			if (bytesSent == -1)
//			{
//				printf("Socket Write Error\n");
//				close(new_socket);
//				connectedFlag = false;
//				break;
//			}
			usleep(50000);

		}

	}

	send(new_socket, buffer, valread, 0);
	printf("Hello message sent\n");
}

void StartEchoServer(uint32_t port)
{
	LinuxTcpServer server(port);
	const int maxSize = 50000;
	char buff[maxSize] =
	{ 0 };
	bool serverUpFlag = server.startListening();

	if (serverUpFlag)
	{
		printf("Echo server started at port:%d\n", port);

		while (1)
		{
			server.listenForNewConnection();

			int bytesRead = server.readFromClients(buff, maxSize);
			if (bytesRead > 0)
			{
//				std::string message(buff, bytesRead);
				std::string message(buff + 16, bytesRead - 16);

				printf("%s\n", message.c_str());
				server.broadCastToClients(buff, bytesRead);
			}
			usleep(500);

		}
	}
	else
	{

		printf("Echo server failed to start at port:%d\n", port);
	}
}
