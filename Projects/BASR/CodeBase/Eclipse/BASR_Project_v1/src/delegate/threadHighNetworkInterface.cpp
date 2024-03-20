/*
 * threadHighNetworkInterface.cpp
 *
 *  Created on: Jan 27, 2023
 *      Author: gmd
 */

#include "threadHighNetworkInterface.h"
#include "../utility/threadutility.h"

void* threadHighNetworkInterface(void*)
{
	//sendDummyMessagesToHighNetworkInterface();
	std::cout << "Network Interface Thread:" << getMyProcessorCore() << endl;
	HighNetworkInterface &networkInterface = Model::highNetworkInterface;

	networkInterface.init();

	uint64_t lastUpdateTime = Utility::getSystemTime();

	while (1)
	{
		uint64_t currTime = Utility::getSystemTime();

		bool anyNewMessage =
				networkInterface.networkInterfaceMainProcessingLogic();

		if (currTime - lastUpdateTime > 100000 || anyNewMessage)
		{
			lastUpdateTime = currTime;

			highNetworkInterfacePeriodUpdateState();
		}
		usleep(10000);
	}
}

void highNetworkInterfacePeriodUpdateState()
{

}

void sendDummyMessagesToHighNetworkInterface()
{
	HighNetworkInterface &networkInterface = Model::highNetworkInterface;
//	string StopMessage =
//			"{      \"MessageName\": \"BasrTpInterfaceV100\", \"CommandType\": \"PlaybackControl\",      \"CommandOpCode\": \"StopPlayback\"    }";
//
//	string StartMessage =
//			"{      \"MessageName\": \"BasrTpInterfaceV100\", \"CommandType\": \"PlaybackControl\",      \"CommandOpCode\": \"StartPlayback\"    }";
//
//	string StatusMessage =
//			"{      \"MessageName\": \"BasrTpInterfaceV100\", \"CommandType\": \"PlaybackControl\",      \"CommandOpCode\": \"GetPlaybackStatus\"    }";
//
//	string CaseNamesMessage =
//			"{      \"MessageName\": \"BasrTpInterfaceV100\", \"CommandType\": \"PlaybackControl\",      \"CommandOpCode\": \"GetPlaybackCaseNames\",      \"Options\": \"/home/user/Eclipse_IDE_WorkSpaces/Basr_WorkSpace/tp_v3.4simulator/input/\"    }";
//
//	string LoadCasesMessage =
//			"{      \"MessageName\": \"BasrTpInterfaceV100\", \"CommandType\": \"PlaybackControl\",      \"CommandOpCode\": \"LoadPlaybackCases\",      \"Options\": [        \"test2.bin\"     ]    }";
//
//	string copy;
//	networkInterface.submitMessageForLoopBack(copy = CaseNamesMessage);
//	networkInterface.submitMessageForLoopBack(copy = LoadCasesMessage);
}
