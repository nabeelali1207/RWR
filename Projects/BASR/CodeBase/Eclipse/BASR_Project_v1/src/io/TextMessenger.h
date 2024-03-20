/*
 * textMessenger.h
 *
 *  Created on: Jan 18, 2023
 *      Author: gmd
 */

#ifndef TEXTMESSENGER_H_
#define TEXTMESSENGER_H_

#include "serialbuffer.h"
#include "MessageCodecClass.h"

#include <mutex>
#include <queue>
#include <string>

#include "LinuxTcpServer.h"

using std::string;

class TextMessengerServer
{

	MessageCodecClass encoderDecoderObj;

    LinuxTcpServer connServer;

    std::mutex outMutex;                    // another thread writes messages that are to be sent out
    std::mutex inMutex;

    std::queue<string> toBeSentMessages;
    std::queue<uint32_t> toBeSentMessagesType;

    std::queue<string> recvdMessages;
    std::queue<uint32_t> recvdMessagesType;
    uint32_t recvdMesgCount;
    uint32_t toBeSentMsgCount;

    uint32_t maxBufferSize;

public:

    TextMessengerServer(uint32_t portNum,bool printLog=true,uint32_t maxBufferSize=2<<20,uint64_t delim=0xdeadbeefbeeffeedull);

    bool startListeningForMsgs();

    void sendRecvAnyQueuedMessages();
    void sendRecvAnyQueuedMessagesLoopBack();

    void listenForNewClients();

    

    bool submitMessageForSending(string& message,uint32_t messageType);
    bool getNextMessage(string &message,uint32_t& messageType);

    bool submitMessageForLoopBack(string& message,uint32_t messageType);

};



class TextMessengerServerTester
{
public:
	static uint64_t getSystemTime();
	static void testThroughput();
};
#endif 
