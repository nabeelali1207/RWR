/*
 * LinuxTcpServer.h
 *
 *  Created on: Jan 19, 2023
 *      Author: user
 */

#ifndef LINUXTCPSERVER_H_
#define LINUXTCPSERVER_H_

#include <sys/time.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>    // close()
#include <cstring>    // sizeof()
#include <errno.h>
#include <list>
#include <string>
#include <netinet/tcp.h>
using std::list;

class LinuxTcpServer
{

	struct ClientInfo
	{
		uint32_t clientIndex;
		int32_t socketFd;
		struct sockaddr_in clientAddress;
		bool keepAliveState;
		int32_t keepAliveInterval;
		int32_t keepAliveCount;
		int32_t keepAliveDelay;
	};

	uint32_t port;
	uint32_t backLogSize;

	bool socketCreatedFlag;
	bool socketBindSuccessFlag;

	bool serverOkayFlag;

	list<ClientInfo> clientSockets;

	int32_t serverSocket;

	std::string lastErrorsString;
	uint64_t lastCheckForConn;
	uint64_t lastTransactionTime;
	uint64_t lastSendTime;
	uint64_t lastRecvTime;

	struct sockaddr_in serverAddress;

	bool keepAlives;
	int32_t keepAliveInterval;
	int32_t keepAliveCount;
	int32_t keepAliveDelay;

	uint32_t accConnReqCnt;
	uint32_t activeClientsCount;

	bool printPeriodicStats;


	void setupSocket();
	void bindSocket();

	bool setupNewClient(ClientInfo &tempClient);
	int32_t readFromClient(ClientInfo &c, char* buff, uint32_t maxSize);
	int32_t writeToClient(ClientInfo &c, char* buff, uint32_t size);
public:

	uint64_t getSystemTime()
	{ //returns time in microseconds
		struct timeval tv;
		gettimeofday(&tv, NULL);
		return (((uint64_t) tv.tv_sec) * 1000000 + (uint64_t) tv.tv_usec);
	}

	LinuxTcpServer(uint32_t _port,bool _printStats=true, uint32_t backLog = 10);
	~LinuxTcpServer();

	bool startListening();

	void listenForNewConnection();

	uint32_t readFromClients(char* buff, uint32_t maxSize);

	uint32_t broadCastToClients(char* buff, uint32_t size);


	void rawMethod();
};
void StartEchoServer(uint32_t port=12346);
#endif /* LINUXTCPSERVER_H_ */
