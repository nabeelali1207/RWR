/*
 * textMessenger.h
 *
 *  Created on: Jan 18, 2023
 *      Author: gmd
 */

#include "TextMessenger.h"

TextMessengerServer::TextMessengerServer(uint32_t portNum,bool printLog,
		uint32_t _maxBufferSize, uint64_t delim) :
		encoderDecoderObj(_maxBufferSize,delim), connServer(portNum,printLog)
{

	//networkLink.SetBlockingCall(false);
	recvdMesgCount = 0;
	toBeSentMsgCount = 0;
	maxBufferSize = _maxBufferSize;

}

bool TextMessengerServer::startListeningForMsgs()
{
	return connServer.startListening();

}
void TextMessengerServer::listenForNewClients()
{
	connServer.listenForNewConnection();
}
void TextMessengerServer::sendRecvAnyQueuedMessagesLoopBack()
{
	char* buffPtr = encoderDecoderObj.getRecvBufferPointerForRead();
	uint32_t accMessageSize = 0;
	while (toBeSentMsgCount > 0)
	{
		outMutex.lock();
		string textMessage = std::move(toBeSentMessages.front());
		uint32_t msgType = toBeSentMessagesType.front();

		toBeSentMessages.pop();
		toBeSentMessagesType.pop();
		toBeSentMsgCount--;
		outMutex.unlock();

		uint32_t messageSizeToSend = encoderDecoderObj.encodeNewMessage(
				&textMessage[0], textMessage.size(), msgType);
		char* txPtr = encoderDecoderObj.getTransmitBufferPtr();
		memcpy(buffPtr, txPtr, messageSizeToSend);
		accMessageSize += messageSizeToSend;
		buffPtr += messageSizeToSend;
	}
	if (accMessageSize > 0)
	{
		buffPtr = encoderDecoderObj.getRecvBufferPointer();
		uint32_t msgStartPoint = encoderDecoderObj.onNewBytesAddedInBuffer(
				accMessageSize);

		while (msgStartPoint > 0)
		{
			uint32_t msgType = encoderDecoderObj.getTypeOfLastRecvdMsg();
			uint32_t msgSize = encoderDecoderObj.getSizeOfLastRecvdMsg();

			string textMessage((buffPtr+msgStartPoint),msgSize);
			encoderDecoderObj.onNewMessageProcessed();

			inMutex.lock();
			recvdMessages.push(std::move(textMessage));
			recvdMessagesType.push(msgType);
			recvdMesgCount++;
			inMutex.unlock();
			msgStartPoint = encoderDecoderObj.onNewBytesAddedInBuffer(0);
		}
	}
}
void TextMessengerServer::sendRecvAnyQueuedMessages()
{

	connServer.listenForNewConnection();
	//recv new bytes
	char* buffPtr = encoderDecoderObj.getRecvBufferPointerForRead();
	uint32_t newByteCount = connServer.readFromClients(buffPtr,maxBufferSize);
	newByteCount = (newByteCount == (uint32_t)-1) ? 0 : newByteCount;

	uint32_t msgStartPoint = encoderDecoderObj.onNewBytesAddedInBuffer(
			newByteCount);

	if (msgStartPoint > 0)
	{
		uint32_t msgType = encoderDecoderObj.getTypeOfLastRecvdMsg();
		uint32_t msgSize = encoderDecoderObj.getSizeOfLastRecvdMsg();

		buffPtr = encoderDecoderObj.getRecvBufferPointer();
		string textMessage((buffPtr+msgStartPoint),msgSize);

		encoderDecoderObj.onNewMessageProcessed();

		inMutex.lock();
		recvdMessages.push(std::move(textMessage));
		recvdMessagesType.push(msgType);
		recvdMesgCount++;
		inMutex.unlock();
	}

	if (toBeSentMsgCount > 0)
	{
		outMutex.lock();
		string textMessage = std::move(toBeSentMessages.front());
		uint32_t msgType = toBeSentMessagesType.front();

		toBeSentMessages.pop();
		toBeSentMessagesType.pop();
		toBeSentMsgCount--;
		outMutex.unlock();

		uint32_t messageSizeToSend = encoderDecoderObj.encodeNewMessage(
				&textMessage[0], textMessage.size(), msgType);

		char* txPtr = encoderDecoderObj.getTransmitBufferPtr();
		connServer.broadCastToClients(txPtr, messageSizeToSend);
	}

}

bool TextMessengerServer::submitMessageForSending(string& message,
		uint32_t messageType)
{
	if (toBeSentMsgCount < 10)
	{
		outMutex.lock();
		toBeSentMessages.push(std::move(message));
		toBeSentMessagesType.push(messageType);
		toBeSentMsgCount++;
		outMutex.unlock();
		return true;
	}
	return false;
}


bool TextMessengerServer::submitMessageForLoopBack(string& message,uint32_t messageType)
{
	bool res = submitMessageForSending(message,messageType);
	sendRecvAnyQueuedMessagesLoopBack();
	return res;
}
bool TextMessengerServer::getNextMessage(string &message, uint32_t& messageType)
{

	if (recvdMesgCount > 0)
	{
		outMutex.lock();
		message = std::move(recvdMessages.front());
		messageType = recvdMessagesType.front();

		recvdMessages.pop();
		recvdMessagesType.pop();
		recvdMesgCount--;

		outMutex.unlock();
		return true;
	}
	return false;
}

#include <sys/time.h>
uint64_t TextMessengerServerTester::getSystemTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (((uint64_t) tv.tv_sec) * 1000000 + (uint64_t) tv.tv_usec);
}

void TextMessengerServerTester::testThroughput()
{
	std::cout << "!!!Hello World!!!" << std::endl; // prints !!!Hello World!!!

	TextMessengerServer server(12346);

	string recvdMessageTemplate = "{"
			"\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\","
			"\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\","
			"\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\","
			"\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\","
			"\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\","
			"\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\","
			"\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\","
			"\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\","
			"\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\","
			"\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\","
			"\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\","
			"\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\","
			"\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\","
			"\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\","
			"\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\",\"MessageName\": \"BASR_TPV100\","
			"}";
	for(int i=0;i<4;i++)
	{
		string recvdMessage =recvdMessageTemplate;
		for(int j=0;j<i;j++)
		{
			recvdMessage = recvdMessage + recvdMessage;
		}
		uint32_t recvdMessageType = 0x1000;
		server.submitMessageForSending(recvdMessage,recvdMessageType);
	}

	uint64_t startTime = getSystemTime();
	uint32_t msgCount = 0;
	uint32_t byteCount = 0;

	while(1)
	{
		server.sendRecvAnyQueuedMessagesLoopBack();


		string recvdMessage;
		uint32_t recvdMessageType;

		bool msgRecvdFlag = server.getNextMessage(recvdMessage,recvdMessageType);
		bool msgSentFlag = false;
		if(msgRecvdFlag==true)
		{
			string sMesg = recvdMessage;

			msgSentFlag = server.submitMessageForSending(sMesg,recvdMessageType);
			if(msgSentFlag==true)
			{

			}
			msgCount++;
			byteCount += (recvdMessage.length());
		}
		uint64_t currTime = getSystemTime();

		if((currTime-startTime)> 1000000)
		{
			std::cout << "Msgs Per Second: " << msgCount << " Bytes Per Second: " << byteCount << std::endl;
			byteCount = msgCount = 0;
			startTime = currTime;
		}
	}
}
