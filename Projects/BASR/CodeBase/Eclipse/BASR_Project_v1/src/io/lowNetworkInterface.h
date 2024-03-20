/*
 * lowNetworkInterface.h
 *
 *  Created on: Jan 18, 2023
 *      Author: gmd
 */

#ifndef LOWNETWORKINTERFACE_H_
#define LOWNETWORKINTERFACE_H_


#include "serialbuffer.h"
#include <string>
#include <mutex>
#include <queue>
#include "liveplaybackpdwprovider.h"
#include "TextMessenger.h"
using std::vector;
using std::string;
#include "../utility/BasicJsonParser.h"


class LowNetworkInterface
{

	TextMessengerServer msgServer;
	vector<string> playbackCasesAvailable;
	vector<string> playbackCasesToLoad;
	vector<string> playbackCasesLoaded;
	bool pbHaveToLoadPlaybackCases;
	// playback controller


	bool printLogFlag;


	BasicJsonParser jsonParserObj;
public:
	static const uint32_t BASR_TPPb_INTERFACE_MESSAGE_TYPE = 0x1001;
	static const uint32_t BASR_TP_INTERFACE_MESSAGE_TYPE = 0x1000;
	LowNetworkInterface(uint32_t port,bool printLogFlag=true);

	void init();

	bool networkInterfaceMainProcessingLogic();



    bool submitMessageForSending(string& message,uint32_t messageType=BASR_TPPb_INTERFACE_MESSAGE_TYPE);
    bool submitMessageForLoopBack(string& message,uint32_t messageType=BASR_TPPb_INTERFACE_MESSAGE_TYPE);

	string errorString;

	PlaybackPdwProvider pdwProviderObj;

	string playbackCasesPath;
	bool isOn;
private:

	bool pbProcessMessage(string &message);


	void pbOnLoadSelectedCases();

	void pbStopPlayback();

	void pbStartPlayback();

	void pbOnPlaybackStatusRequested();

	void pbOnPlaybackCaseNameRequested();

	void pbOnPlaybackStarted();

	void pbOnPlaybackStopped();


public:
	void pbOnPlaybackFinished();

	bool pbNeedToLoadPlaybackCases();
	vector<string> & pbGetNameOfCasesToLoad();
	void pbOnCasesLoaded(vector<string> & loadedCases,vector<string> &skippedCases);

};


#endif /* LOWNETWORKINTERFACE_H_ */
