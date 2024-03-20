/*
 * threadlownetworkinterface.cpp
 *
 *  Created on: Jan 23, 2023
 *      Author: gmd
 */

#include "threadlownetworkinterface.h"
#include "datainputdelegate.h"
#include "../utility/threadutility.h"
void * threadLowNetworkInterface(void *)
{
	//sendDummyMessagesToLowNetworkInterface();

	//usleep(5000000);

	std::cout << "Network Interface Thread:" << getMyProcessorCore() << endl;
	LowNetworkInterface &networkInterface = Model::lowNetworkInterface;
	PlaybackPdwProvider &pdwProviderObj = networkInterface.pdwProviderObj;
	networkInterface.init();

	uint64_t lastUpdateTime = Utility::getSystemTime();

	while (1)
	{
		uint64_t currTime = Utility::getSystemTime();

		pdwProviderObj.update();

		bool anyNewMessage = networkInterface.networkInterfaceMainProcessingLogic();


		if (currTime - lastUpdateTime > 100000 || anyNewMessage)
		{
			lastUpdateTime = currTime;

			lowNetworkInterfacePeriodicUpdateState();
		}
		usleep(10000);
	}

}

void lowNetworkInterfacePeriodicUpdateState()
{
	LowNetworkInterface &networkInterface = Model::lowNetworkInterface;
	if (networkInterface.pbNeedToLoadPlaybackCases() == true)
	{
		loadPdwsForPlayback();
	}
	if (networkInterface.pdwProviderObj.isPlaybackCompleted())
	{
		networkInterface.pbOnPlaybackFinished();
	}

}

bool loadPdwsForPlayback()
{
	LowNetworkInterface &networkInterface = Model::lowNetworkInterface;
	vector<string>& cases = networkInterface.pbGetNameOfCasesToLoad();
	vector<PdwSmall32> inputPdws;

	vector<string> validCases;
	vector<string> invalidCases;
	PlaybackPdwProvider &pdwProviderObj = networkInterface.pdwProviderObj;
	pdwProviderObj.stopPlayback();
	pdwProviderObj.clearPdws();
	for (uint32_t i = 0; i < cases.size(); i++)
	{
		readAllPdwsBasrHexFile(
				(networkInterface.playbackCasesPath + cases[i]).c_str(),
				inputPdws);

		// hack to offset toa of every case
		for (uint32_t ii = 0; ii < inputPdws.size(); ii++)
		{
			inputPdws[ii].TOA += 4 * i;
		}
		//

		if (inputPdws.size() > 0)
		{
			validCases.push_back(cases[i]);
			pdwProviderObj.addPdwsForPlayback(inputPdws);

		}
		else
		{
			invalidCases.push_back(cases[i]);
		}
	}

	networkInterface.pbOnCasesLoaded(validCases, invalidCases);
	return true;

}

void sendDummyMessagesToLowNetworkInterface()
{
	LowNetworkInterface &networkInterface = Model::lowNetworkInterface;
	string StopMessage =
			"{      \"MessageName\": \"BasrTpInterfaceV100\", \"CommandType\": \"PlaybackControl\",      \"CommandOpCode\": \"StopPlayback\"    }";

	string StartMessage =
			"{      \"MessageName\": \"BasrTpInterfaceV100\", \"CommandType\": \"PlaybackControl\",      \"CommandOpCode\": \"StartPlayback\"    }";

	string StatusMessage =
			"{      \"MessageName\": \"BasrTpInterfaceV100\", \"CommandType\": \"PlaybackControl\",      \"CommandOpCode\": \"GetPlaybackStatus\"    }";

	string CaseNamesMessage =
			"{      \"MessageName\": \"BasrTpInterfaceV100\", \"CommandType\": \"PlaybackControl\",      \"CommandOpCode\": \"GetPlaybackCaseNames\",      \"Options\": \"/home/user/Eclipse_IDE_WorkSpaces/Basr_WorkSpace/tp_v3.4simulator/input/\"    }";

	string LoadCasesMessage =
			"{      \"MessageName\": \"BasrTpInterfaceV100\", \"CommandType\": \"PlaybackControl\",      \"CommandOpCode\": \"LoadPlaybackCases\",      \"Options\": [        \"test2.bin\"     ]    }";

	string copy;
	networkInterface.submitMessageForLoopBack(copy = CaseNamesMessage);
	networkInterface.submitMessageForLoopBack(copy = LoadCasesMessage);
//	networkInterface.submitMessageForLoopBack(copy = StatusMessage);
//	networkInterface.submitMessageForLoopBack(copy = StopMessage);
//	networkInterface.submitMessageForLoopBack(copy = StatusMessage);
	networkInterface.submitMessageForLoopBack(copy = StartMessage);
//	networkInterface.submitMessageForLoopBack(copy = StatusMessage);
//	networkInterface.submitMessageForLoopBack(copy = StopMessage);
//	networkInterface.submitMessageForLoopBack(copy = StatusMessage);
}
