/*
 * MessageCodecClass.cpp
 *
 *  Created on: Jan 18, 2023
 *      Author: gmd
 */

#include "MessageCodecClass.h"

MessageCodecClass::MessageCodecClass(uint32_t txRxBuffSize, uint64_t delim) :
		sendBuffer(SerialBufferStreamTypeEnum::InputStream, txRxBuffSize), recvBuffer(
				SerialBufferStreamTypeEnum::InputStream, txRxBuffSize)
{
	delimiter = delim;
	toSendByteCount = 0;
	recvdByteCount = 0;
	recvStartPoint = 0;
	delimiter = delim;
	sendBuffer.seekPosition(0);
	recvBuffer.seekPosition(0);

}

int32_t MessageCodecClass::findDelimInBuffer(char * buff, uint32_t startOffset,
		uint64_t counts)
{

	char* delimPtr = (char*) &delimiter;

	uint32_t matchCount = 0;

	for (uint32_t i = startOffset,j=0; j < counts; i++,j++)
	{
		if (buff[i] == delimPtr[matchCount])
		{
			matchCount++;
		}
		else
		{
			matchCount = 0;
		}
		if (matchCount == sizeof(delimiter))
		{
			return (i+1-sizeof(delimiter));
		}
	}
	return -1;
}

char* MessageCodecClass::getRecvBufferPointerForRead()
{
	return (recvBuffer.getRawBufferPointer() + recvStartPoint + recvdByteCount);
}
char* MessageCodecClass::getRecvBufferPointer()
{
	return recvBuffer.getRawBufferPointer();
}
uint32_t MessageCodecClass::onNewBytesAddedInBuffer(uint32_t newBytes)
{
	char * buffPtr = recvBuffer.getRawBufferPointer();
	recvdByteCount += newBytes;
	uint32_t endPoint = recvStartPoint + recvdByteCount;
	int32_t foundPos = findDelimInBuffer(buffPtr, recvStartPoint,
			recvdByteCount);

	bool delimFoundFlag = (foundPos != -1);
	if (delimFoundFlag == false)
	{
		if (recvdByteCount > 100000)
		{
			uint32_t bytesToRetain = sizeof(msgHeader);
			uint32_t copyStartPoint = endPoint - bytesToRetain;
			uint32_t copyEndPoint = endPoint;

			for (uint32_t i = copyStartPoint, j = 0; i < copyEndPoint; i++, j++)
			{
				buffPtr[j] = buffPtr[i];
			}
			recvStartPoint = 0;
			recvdByteCount = bytesToRetain;
		}
		return 0;
	}
	//delimiter found, check for message size and header

	uint32_t bytesAfterDelim = endPoint - foundPos;
	bool fullHeaderPresent = (bytesAfterDelim > sizeof(msgHeader));
	if (fullHeaderPresent == false)
	{
		return 0;
	}
	memcpy((void*) &msgHeader, (void*) (buffPtr + foundPos), sizeof(msgHeader));

	if(msgHeader.msgSize==0)
	{
		recvStartPoint = 0;
		recvdByteCount = 0;
		return 0;
	}
	bool fullMsgPresent = (msgHeader.msgSize
			<= (bytesAfterDelim - sizeof(msgHeader)));
	if (fullMsgPresent)
	{
		return (foundPos + sizeof(msgHeader));
	}
	else
	{
		return 0;
	}

}

void MessageCodecClass::onNewMessageProcessed()
{
	uint32_t oldEndPoint = recvStartPoint + recvdByteCount;
	recvStartPoint += msgHeader.msgSize+ sizeof(msgHeader);
	int32_t validBytesRemaining = (int32_t) recvdByteCount
			- (int32_t) (msgHeader.msgSize+ sizeof(msgHeader));

	if (validBytesRemaining < 0)
	{
		recvStartPoint = 0;
		recvdByteCount = 0;
	}
	else if (validBytesRemaining == 0)
	{
		recvStartPoint = 0;
		recvdByteCount = 0;
	}
	else if (recvStartPoint > 100000)
	{
		uint32_t bytesToRetain = validBytesRemaining;
		uint32_t copyStartPoint = oldEndPoint - bytesToRetain;
		uint32_t copyEndPoint = oldEndPoint;
		char * buffPtr = recvBuffer.getRawBufferPointer();

		for (uint32_t i = copyStartPoint, j = 0; i < copyEndPoint; i++, j++)
		{
			buffPtr[j] = buffPtr[i];
		}
		recvStartPoint = 0;
		recvdByteCount = bytesToRetain;
	}
	else
	{
		recvdByteCount = validBytesRemaining;
	}
}

uint32_t MessageCodecClass::encodeNewMessage(char* buff, uint32_t byteCount,
		uint32_t msgType)
{
	char * buffPtr = sendBuffer.getRawBufferPointer();
	msgHeader.msgSize = byteCount;
	msgHeader.msgType = msgType;
	msgHeader.delim = delimiter;
	memcpy(buffPtr, &msgHeader, sizeof(msgHeader));
	memcpy(buffPtr+sizeof(msgHeader), buff, byteCount);
	return byteCount + sizeof(msgHeader);
}

char* MessageCodecClass::getTransmitBufferPtr()
{
	return sendBuffer.getRawBufferPointer();
}
