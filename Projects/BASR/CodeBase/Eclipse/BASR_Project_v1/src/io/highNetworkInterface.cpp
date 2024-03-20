/*
 * highNetworkInterface.cpp
 *
 *  Created on: Jan 27, 2023
 *      Author: gmd
 */

#include "highNetworkInterface.h"

HighNetworkInterface::HighNetworkInterface(uint32_t port, bool _printLogFlag) :
		msgServer(port, _printLogFlag)
{
	printLogFlag = _printLogFlag;

	errorString.reserve(1000);
	isOn = false;
}

void HighNetworkInterface::init()
{
	isOn = msgServer.startListeningForMsgs();
}

bool HighNetworkInterface::networkInterfaceMainProcessingLogic()
{
	msgServer.listenForNewClients();

	msgServer.sendRecvAnyQueuedMessages();
	string message;
	uint32_t messageType;
	bool newMsgFlag = msgServer.getNextMessage(message, messageType);
	if (newMsgFlag)
	{
		if (messageType == BASR_CCP_1553_INTERFACE_MESSAGE_TYPE
				|| messageType == BASR_CCP_INTERFACE_MESSAGE_TYPE)
		{

		}
	}
	return newMsgFlag;
}

bool HighNetworkInterface::submitMessageForSending(string& message,
		uint32_t messageType)
{
	if (isOn)
	{
		return msgServer.submitMessageForSending(message, messageType);

	}
	else
	{
		return false;
	}
}

bool HighNetworkInterface::submitMessageForLoopBack(string& message,
		uint32_t messageType)
{
	return msgServer.submitMessageForLoopBack(message, messageType);
}

