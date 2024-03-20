/*
 * threadlownetworkinterface.cpp
 *
 *  Created on: Jan 17, 2023
 *      Author: GMD
 */

#include "lowNetworkInterface.h"
//#include "../utility/json.hpp"
#include "../utility/CommandLineTools.h"
#include "../utility/VectorAndStringUtils.h"

LowNetworkInterface::LowNetworkInterface(uint32_t port, bool _printLogFlag) :
		msgServer(port, _printLogFlag)
{
	printLogFlag = _printLogFlag;

	errorString.reserve(1000);
	pbHaveToLoadPlaybackCases = false;
	isOn = false;
}

void LowNetworkInterface::init()
{

	pdwProviderObj.initialize();
	isOn = msgServer.startListeningForMsgs();
}
bool LowNetworkInterface::networkInterfaceMainProcessingLogic()
{

	msgServer.listenForNewClients();

	msgServer.sendRecvAnyQueuedMessages();

//	msgServer.sendRecvAnyQueuedMessagesLoopBack();

	string message;
	uint32_t messageType;
	bool newMsgFlag = msgServer.getNextMessage(message, messageType);
	if (newMsgFlag)
	{
		if (messageType == BASR_TP_INTERFACE_MESSAGE_TYPE
				|| messageType == BASR_TPPb_INTERFACE_MESSAGE_TYPE)
		{
			pbProcessMessage(message);
		}
	}
	return newMsgFlag;

}

bool LowNetworkInterface::submitMessageForSending(string& message,
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
bool LowNetworkInterface::submitMessageForLoopBack(string& message,
		uint32_t messageType)
{
	return msgServer.submitMessageForLoopBack(message, messageType);
}

bool LowNetworkInterface::pbProcessMessage(string& message)
{

	//cout << message << endl;
//	try
//	{
//		nlohmann::json jsonMessage = nlohmann::json::parse(message);
//		string mName = jsonMessage["MessageName"];
//		if (mName != "BasrTpInterfaceV100")
//		{
//			if (printLogFlag)
//			{
//				errorString += ("Invalid MessageName: " + mName
//						+ " expected \"BasrTpInterfaceV100\"\n");
//			}
//			return false;
//		}
//		string cType = jsonMessage["CommandType"];
//
//		if (cType != "PlaybackControl")
//		{
//			if (printLogFlag)
//			{
//				errorString += ("Invalid MessageName::CommandType " + cType
//						+ " expected \"PlaybackControl\"\n");
//			}
//			return false;
//		}
//
//		string opCode = jsonMessage["CommandOpCode"];
//		bool opCodeError = false;
//		if (opCode == "StopPlayback")
//		{
//			pbStopPlayback();
//		}
//		else if (opCode == "StartPlayback")
//		{
//			pbStartPlayback();
//		}
//		else if (opCode == "GetPlaybackStatus")
//		{
//			pbOnPlaybackStatusRequested();
//		}
//		else if (opCode == "GetPlaybackCaseNames")
//		{
//			if(jsonMessage.contains("Options"))
//			{
//				playbackCasesPath = jsonMessage["Options"];
//
//			}
//			else
//			{
//				playbackCasesPath = "/";
//			}
//			pbOnPlaybackCaseNameRequested();
//		}
//		else if (opCode == "LoadPlaybackCases")
//		{
//			if(jsonMessage.contains("Options") && jsonMessage["Options"].is_array())
//			{
//				nlohmann::json optArray = jsonMessage["Options"];
//				playbackCasesToLoad.resize(0);
//				uint32_t caseCount = optArray.size();
//				for(uint32_t i=0;i<caseCount;i++)
//				{
//					string caseName = jsonMessage["Options"][i];
//					playbackCasesToLoad.push_back(caseName);
//				}
//				pbOnLoadSelectedCases();
//			}
//			else
//			{
//				errorString += ("Invalid MessageName::CommandType::OptionCode:Options "
//										+ opCode + " expected a list<string> \n");
//				opCodeError = true;
//			}
//		}
//		else
//		{
//			opCodeError = true;
//		}
//
//
//		if(opCodeError)
//		{
//			if (printLogFlag)
//			{
//				errorString += ("Invalid MessageName::CommandType::OptionCode: "
//						+ opCode + "\n");
//			}
//			return false;
//		}
//
//	} catch (...)
//	{
//		errorString += ("Exception:: Failed to parse json string");
//		return false;
//	}
	if (jsonParserObj.parseFromString(message) == true)
	{
		string query, mName = "";
		jsonParserObj.getElemValue(query = "MessageName", mName);
		if (mName != "BasrTpInterfaceV100")
		{
			if (printLogFlag)
			{
				errorString += ("Invalid MessageName: " + mName
						+ " expected \"BasrTpInterfaceV100\"\n");
			}
			return false;
		}
		string cType = "";
		jsonParserObj.getElemValue(query = "CommandType", cType);
		if (cType != "PlaybackControl")
		{
			if (printLogFlag)
			{
				errorString += ("Invalid MessageName::CommandType " + cType
						+ " expected \"PlaybackControl\"\n");
			}
			return false;
		}
		string opCode = "";
		jsonParserObj.getElemValue(query = "CommandOpCode", opCode);

		bool opCodeError = false;
		if (opCode == "StopPlayback")
		{
			pbStopPlayback();
		}
		else if (opCode == "StartPlayback")
		{
			pbStartPlayback();
		}
		else if (opCode == "GetPlaybackStatus")
		{
			pbOnPlaybackStatusRequested();
		}
		else if (opCode == "GetPlaybackCaseNames")
		{
			if (jsonParserObj.getElemValue(query = "Options",
					playbackCasesPath))
			{

			}
			else
			{
				playbackCasesPath = "/";
			}
			pbOnPlaybackCaseNameRequested();
		}
		else if (opCode == "LoadPlaybackCases")
		{
			string options = "a";
			jsonParserObj.getElemValue(query = "Options", options);
			if (VectorAndStringUtils::isNumber(options))
			{
				playbackCasesToLoad.resize(0);
				uint32_t caseCount = std::stoi(options);
				for (uint32_t i = 0; i < caseCount; i++)
				{
					string caseName;
					string queryName = "Options[" + std::to_string(i) + "]";
					if (jsonParserObj.getElemValue(queryName, caseName))
					{
						playbackCasesToLoad.push_back(caseName);
					}
					else
					{
						opCodeError = true;
						break;
					}
				}
				if (opCodeError == false)
				{
					pbOnLoadSelectedCases();

				}
			}
			else
			{
				errorString +=
						("Invalid MessageName::CommandType::OptionCode:Options "
								+ opCode + " expected a list<string> \n");
				opCodeError = true;
			}
		}
		else
		{
			opCodeError = true;
		}

		if (opCodeError)
		{
			if (printLogFlag)
			{
				errorString += ("Invalid MessageName::CommandType::OptionCode: "
						+ opCode + "\n");
			}
			return false;
		}

	}
	else
	{
		return false;
	}
	return true;;

}

void LowNetworkInterface::pbStopPlayback()
{
	pdwProviderObj.stopPlayback();
}

void LowNetworkInterface::pbStartPlayback()
{
	pdwProviderObj.startPlayback(true);
	cout << "Playback started" << endl;
}

void LowNetworkInterface::pbOnPlaybackStatusRequested()
{
//	nlohmann::json replyMessage;
//	replyMessage["MessageName"] = "BasrTpInterfaceV100";
//	replyMessage["CommandType"] = "PlaybackControl";
//	replyMessage["CommandOpCode"] = "GetPlaybackStatus";
//	string status = "Stopped";
//	if(pdwProviderObj.isPlaybackRunning()==true)
//	{
//		status = "Running";
//	}
//	else if(pdwProviderObj.isPlaybackCompleted()==true)
//	{
//		status = "Completed";
//	}
//	replyMessage["Response"] = status;
//
//	string reply = replyMessage.dump();

	string reply;
	reply.reserve(200);
	reply += "{\"MessageName\": \"BasrTpInterfaceV100\",";
	reply += "\"CommandType\": \"PlaybackControl\",";
	reply += "\"CommandOpCode\": \"GetPlaybackStatus\",";
	if (pdwProviderObj.isPlaybackRunning() == true)
	{
		reply += "\"Response\": \"Running\"";
	}
	else if (pdwProviderObj.isPlaybackCompleted() == true)
	{
		reply += "\"Response\": \"Completed\"";
	}
	else
	{

		reply += "\"Response\": \"Stopped\"";
	}
	reply += "}";

	submitMessageForSending(reply, BASR_TPPb_INTERFACE_MESSAGE_TYPE);
}

void LowNetworkInterface::pbOnPlaybackCaseNameRequested()
{
	playbackCasesAvailable = CommandLineTools::getAllFilesInDirectory(
			playbackCasesPath, ".bin");
	string reply;
	reply.reserve(200);
	reply += "{\"MessageName\": \"BasrTpInterfaceV100\",";
	reply += "\"CommandType\": \"PlaybackControl\",";
	reply += "\"CommandOpCode\": \"GetPlaybackCaseNames\",";
	reply += "\"Response\": \"Done\",";
	reply += "\"Response2\": [";

	reply += VectorAndStringUtils::vectorToString(playbackCasesAvailable, ',');

	reply += "]}";

	submitMessageForSending(reply, BASR_TPPb_INTERFACE_MESSAGE_TYPE);

}

void LowNetworkInterface::pbOnPlaybackFinished()
{
	pbOnPlaybackStatusRequested();

}

bool LowNetworkInterface::pbNeedToLoadPlaybackCases()
{
	return pbHaveToLoadPlaybackCases;
}
vector<string> & LowNetworkInterface::pbGetNameOfCasesToLoad()
{
	return playbackCasesToLoad;
}

void LowNetworkInterface::pbOnLoadSelectedCases()
{
	string reply;
	reply.reserve(200);
	reply += "{\"MessageName\": \"BasrTpInterfaceV100\",";
	reply += "\"CommandType\": \"PlaybackControl\",";
	reply += "\"CommandOpCode\": \"LoadPlaybackCases\",";
	reply += "\"Response\": \"Started\",";
	reply += "\"Options\": [";

	reply += VectorAndStringUtils::vectorToString(playbackCasesToLoad, ',');

	reply += "]}";

	submitMessageForSending(reply, BASR_TPPb_INTERFACE_MESSAGE_TYPE);
	if (playbackCasesToLoad.size() > 0)
	{
		pbHaveToLoadPlaybackCases = true;
	}
	else
	{
		pbHaveToLoadPlaybackCases = false;
	}
}
void LowNetworkInterface::pbOnCasesLoaded(vector<string> & loadedCases,
		vector<string> &skippedCases)
{
	pbHaveToLoadPlaybackCases = false;

	string reply;
	reply.reserve(200);
	reply += "{\"MessageName\": \"BasrTpInterfaceV100\",";
	reply += "\"CommandType\": \"PlaybackControl\",";
	reply += "\"CommandOpCode\": \"LoadPlaybackCases\",";
	reply += "\"Response\": \"Done\",";
	reply += "\"Found\":[";

	reply += VectorAndStringUtils::vectorToString(loadedCases, ',');

	reply += "],\"NotFound\":[";
	reply += VectorAndStringUtils::vectorToString(skippedCases, ',');
	reply += "]}";

	submitMessageForSending(reply, BASR_TPPb_INTERFACE_MESSAGE_TYPE);
	playbackCasesLoaded = loadedCases;

}
