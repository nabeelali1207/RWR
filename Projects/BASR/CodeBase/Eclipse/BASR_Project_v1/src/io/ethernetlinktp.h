/*
 * ethernetlinktp.h
 *
 *  Created on: Jun 23, 2021
 *      Author: irfan
 */

#ifndef SRC_IO_ETHERNETLINKTP_H_
#define SRC_IO_ETHERNETLINKTP_H_

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include "../common/common.h"
#include "../utility/constlow.h"
#include "../utility/sha256.h"


typedef struct threatReportHeader
{
	Word64	 	delimeter;
	UWord32 	crc;
	UWord32 	faultInjection;
	Word16 	   	messageType;
	Word16      messageSize;
} threatReportHeader;

typedef struct threatReportParameters
{
	Word64 	toaFirst;
	Word64 	toaLast;
	Word8   priType;
	Word32 	priSpot;
	Word8   rfType;
	Word32  rfSpot;
	Word8   pwType;
	Word32  pwSpot;
	Word32  powerAmp;
	Word32  aoa;
	Word8   arpType;
	Word32  arpSpot;
	F64  	PowerAntennaA;
	F64  	PowerAntennaB;
	F64  	PowerAntennaC;
	F64  	PowerAntennaD;
}threatReportParameters;

typedef struct threatReportStructure
{
	threatReportHeader threatHeader;
	UWord32 trackCount;
    threatReportParameters  threatParam[16];
}threatReportStructure;

class Ethernetlinktp{

private:

	int32_t sockfd,connfd;
	UWord32 const TP_THREATS_COUNTS  = 16;
	UWord8  receiveBufferEthernet[1500];


public:

	threatReportStructure *threatMsgs;
	UWord32 const PORT = 5000;
	struct sockaddr SA;
	struct sockaddr_in servaddr;
	int serverLen = sizeof(servaddr);
	void initializeEthernetLink(int readThreatDataFromEthernet);
	void socketClose(int readThreatDataFromEthernet);
	UWord32 checkPacketsAvailableOnEthernetLink(int readThreatDataFromEthernet);
	void ethernetPacketsRead(int readThreatDataFromEthernet);
};




#endif /* SRC_IO_ETHERNETLINKTP_H_ */
