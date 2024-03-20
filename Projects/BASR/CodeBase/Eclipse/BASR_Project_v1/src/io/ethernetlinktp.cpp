/*
 * ethernetlinktp.cpp
 *
 *  Created on: Jun 23, 2021
 *      Author: Irfan
 */


#define ETHERNETLINKTP_CPP_

#include "ethernetlinktp.h"

void Ethernetlinktp:: initializeEthernetLink(int readThreatDataFromEthernet)
{
	if (readThreatDataFromEthernet == 0)
	{
		return;
	}


	// socket create and verification
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == -1)
	{
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");

	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("192.168.0.20");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	printf("Waiting for Server\n");
	while (bind(sockfd, (sockaddr*) &servaddr, sizeof(servaddr)) != 0)
	{
		//printf("connection with the server failed...\n");
		//exit(0);
	}
	//else
	printf("connected to the server..\n");

	threatMsgs = (threatReportStructure*) receiveBufferEthernet;

}

void Ethernetlinktp :: socketClose(int readThreatDataFromEthernet)
{
	if (readThreatDataFromEthernet == 0)
	{
		return;
	}
	close(sockfd);
}

UWord32 Ethernetlinktp :: checkPacketsAvailableOnEthernetLink(int readThreatDataFromEthernet)
{
	if (readThreatDataFromEthernet == 0)
	{
		return 0;
	}
	int packetsCount = 0;
	ioctl(sockfd,FIONREAD,&packetsCount);
	//printf("%d  \n",packetsCount);
	return packetsCount;
}

void Ethernetlinktp :: ethernetPacketsRead(int readThreatDataFromEthernet)
{
	if (readThreatDataFromEthernet == 0)
	{
		return;
	}
	memset(&receiveBufferEthernet,0,sizeof(receiveBufferEthernet));


	read(sockfd,&receiveBufferEthernet,3000);

//	printf("%lX  \n",threatMsgs->threatHeader.delimeter);
//	printf("%d  \n",threatMsgs->threatHeader.messageType);
//	printf("%d  \n",threatMsgs->threatHeader.messageSize);
//	printf("%lX  \n",threatMsgs->threatHeader.faultInjection);
//	printf("%lX  \n",threatMsgs->threatHeader.crc);
//	printf("%d  \n",threatMsgs->trackCount);
//	printf("%d  \n",threatMsgs->threatParam[0].rfSpot);
//	printf("%d  \n",threatMsgs->threatParam[0].powerAmp);
//	printf("%.3f  \n",threatMsgs->threatParam[0].PowerAntennaA);
//	printf("%.3f  \n",threatMsgs->threatParam[0].PowerAntennaB);
//	printf("%.3f  \n",threatMsgs->threatParam[0].PowerAntennaC);
//	printf("%.3f  \n",threatMsgs->threatParam[0].PowerAntennaD);
//	printf("%d  \n",threatMsgs->threatParam[1].rfSpot);
//	printf("%d  \n",threatMsgs->threatParam[1].powerAmp);
//	printf("%d  \n",threatMsgs->threatParam[2].rfSpot);
//	printf("%d  \n",threatMsgs->threatParam[2].powerAmp);
//	printf("%d  \n",threatMsgs->threatParam[3].rfSpot);
//	printf("%d  \n",threatMsgs->threatParam[3].powerAmp);
}
