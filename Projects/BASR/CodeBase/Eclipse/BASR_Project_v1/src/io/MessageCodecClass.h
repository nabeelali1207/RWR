/*
 * MessageCodecClass.h
 *
 *  Created on: Jan 18, 2023
 *      Author: gmd
 */

#ifndef MESSAGECODECCLASS_H_
#define MESSAGECODECCLASS_H_
#include "serialbuffer.h"

class MessageCodecClass
{
	struct MessengerMessageHeader
	{
		uint64_t delim;
		uint32_t msgType;
		uint32_t msgSize;
	};
	MessengerMessageHeader msgHeader;
	uint64_t delimiter;

	SerialBufferClass sendBuffer;
	SerialBufferClass recvBuffer;

	uint32_t toSendByteCount;

	uint32_t recvdByteCount;
	uint32_t recvStartPoint;


    int32_t findDelimInBuffer(char * buff,uint32_t startOffset,uint64_t counts);
public:
	MessageCodecClass (uint32_t txRxBuffSize,uint64_t delim=0xdeadbeefbeeffeedull);

	char* getRecvBufferPointerForRead();
	char* getRecvBufferPointer();

	uint32_t onNewBytesAddedInBuffer(uint32_t newBytes);
	uint32_t getSizeOfLastRecvdMsg()
	{
		return msgHeader.msgSize;
	}
	uint32_t getTypeOfLastRecvdMsg()
	{
		return msgHeader.msgType;
	}
	void onNewMessageProcessed();

	uint32_t encodeNewMessage(char* buff, uint32_t byteCount,uint32_t msgType=1);

	char* getTransmitBufferPtr();

};

#endif /* MESSAGECODECCLASS_H_ */
