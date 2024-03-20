/*
 * highNetworkInterface.h
 *
 *  Created on: Jan 27, 2023
 *      Author: gmd
 */

#ifndef HIGHNETWORKINTERFACE_H_
#define HIGHNETWORKINTERFACE_H_

#include "serialbuffer.h"
#include <string>
#include <mutex>
#include <queue>
#include "liveplaybackpdwprovider.h"
#include "TextMessenger.h"
using std::vector;
using std::string;

class HighNetworkInterface
{
	TextMessengerServer msgServer;

	bool printLogFlag;

public:
	static const uint32_t BASR_CCP_1553_INTERFACE_MESSAGE_TYPE = 0x101;
	static const uint32_t BASR_CCP_INTERFACE_MESSAGE_TYPE = 0x100;

	HighNetworkInterface(uint32_t port,bool printLogFlag=true);

	void init();

	bool networkInterfaceMainProcessingLogic();

	bool submitMessageForSending(string& message, uint32_t messageType =
			BASR_CCP_INTERFACE_MESSAGE_TYPE);
	bool submitMessageForLoopBack(string& message, uint32_t messageType =
			BASR_CCP_INTERFACE_MESSAGE_TYPE);

	string errorString;

	bool isOn;
private:
};

#endif /* HIGHNETWORKINTERFACE_H_ */
