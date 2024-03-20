/*
 * threadmainlowdelegate.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#define THREADMAINLOWDELEGATE_CPP_

#include "threadmainlowdelegate.h"

void * threadMainLowDelegate(void *)
{
	int64_t startTime = Utility::getSystemTime();


	uint8_t dataFinishedFlag = 0;
	configureThreadMainConstants();
	inputReadPdwSimulationFile(Model::simLowData.bandAllPdwBuffer);
	scalePdwSimulationData(Model::simLowData.bandAllPdwBuffer);
	inputCleanPdwSimData(Model::simLowData.bandAllPdwBuffer);
	inputLimitPdwSimData(Model::simLowData.bandAllPdwBuffer);
	simLowInitializeParameters(Model::simLowData);
	initThreadMainIterCountLow(Model::simLowData.bandAllPdwBuffer,
			Model::countThreadMainIterLow);
//	uint64_t lastLogTime = Utility::getSystemTime();
//	uint64_t pdwsProcessedEJ=0;
//	uint64_t maxTrackCountEJ=0;

	while (dataFinishedFlag == 0)
	{
//		pdwsProcessedEJ += Model::analysisLow.bandEJ.currentPdws.size();
//		maxTrackCountEJ =
//				(maxTrackCountEJ > Model::analysisLow.bandEJ.tracks.size() ?
//						maxTrackCountEJ : Model::analysisLow.bandEJ.tracks.size());
//
//		if ((Utility::getSystemTime() - lastLogTime) > 5000000)
//		{
//			lastLogTime = Utility::getSystemTime();
//			cout << "Systime: " << lastLogTime << " Itr: "
//					<< Model::countThreadMainIterLow << '\n';
//			cout << "PDWs processed since last iteration : "
//					<< pdwsProcessedEJ << "\n";
//			cout << "Max Track Count EJ : " << maxTrackCountEJ << endl;
//			pdwsProcessedEJ = 0;
//
//			maxTrackCountEJ = 0;
//		}

		simLowUpdateIterationTime(Model::simLowData);

		inputClearIterationBandPdws(Model::analysisLow.bandEJ.currentPdws);
		inputClearIterationBandPdws(Model::analysisLow.bandCD.currentPdws);
		inputClearIterationBandPdws(Model::analysisLow.bandK.currentPdws);

		inputGetPdwSimIteration(Model::simLowData.pdwIterationStartTime,
				Model::simLowData.pdwIterationEndTime,
				Model::countThreadMainIterLow, dataFinishedFlag,
				Model::simLowData.bandAllPdwBuffer, Model::analysisLow);
//		inputGetPdwLiveIteration(dataFinishedFlag, Model::dspLivePdwReceiver,
//				Model::analysisLow);
		inputGetPdwLiveIteration(dataFinishedFlag, Model::dspLivePdwReceiver,
				Model::lowNetworkInterface.pdwProviderObj,Model::analysisLow);

		threadCompleteIterationLow(Model::analysisLow.bandEJ);
		threadCompleteIterationLow(Model::analysisLow.bandCD);
		threadCompleteIterationLow(Model::analysisLow.bandK);

		reportAnalysisObjectToCpu(Model::analysisLow,Model::simLowData,
				Model::countThreadMainIterLow);


		logLowIterationInfo(Model::analysisLow,Model::analysisLow.lastReportingSystemIter, Model::countThreadMainIterLow);
		incrementThreadMainIterLow(Model::countThreadMainIterLow);
	}

	int64_t endTime = Utility::getSystemTime();

	cout << "DSP took: " << endTime - startTime << "micro secs" << endl;
	return NULL;
}

void configureThreadMainConstants()
{
	//testing parameters
//	ConstLow::asClusterMaxInput = 64;
//	ConstLow::asClusterMinInput = 32;
//	ConstLow::simModeDisabledFlag = 0;
//	ConstLow::simLogPdwIteration = 1;
//	ConstLow::simTimeMsIterationMax = 5;
//	ConstLow::simTimeIterationFixedFlag = 1;
//	ConstLow::simTimeMsIterationLeeway = 0;
//	ConstLow::simTimeEndOffsetMs = 9000;
//	ConstLow::simFastFlag = 1;
//	ConstLow::logOverallFlag = true;
	//model variable must be passed as an argument
	Model::simTimeMicrosecIterationActual = ConstLow::simIterationFixedTime;

	Model::analysisLow.bandCD.powerThresholdSingle =
			ConstLow::asPowerThreshBandCD;
	Model::analysisLow.bandK.powerThresholdSingle =
			ConstLow::asPowerThreshBandK;
	Model::analysisLow.bandEJ.powerThresholdSingle =
			ConstLow::asPowerThreshBandEJ;
			
	Model::analysisLow.bandCD.powerThresholdCombined =
			ConstLow::asPowerThreshPxPyCombinedBandCD;
	Model::analysisLow.bandK.powerThresholdCombined =
			ConstLow::asPowerThreshPxPyCombinedBandK;
	Model::analysisLow.bandEJ.powerThresholdCombined =
			ConstLow::asPowerThreshPxPyCombinedBandEJ;
			
	Model::analysisLow.bandCD.powerThresholdClustering =
			ConstLow::asPowerThreshPxPyClusteringBandCD;
	Model::analysisLow.bandK.powerThresholdClustering =
			ConstLow::asPowerThreshPxPyClusteringBandK;
	Model::analysisLow.bandEJ.powerThresholdClustering =
			ConstLow::asPowerThreshPxPyClusteringBandEJ;

//
//	Model::analysisLow.bandCD.powerThresholdClustering=0;
//	Model::analysisLow.bandCD.powerThresholdCombined=0;
//	Model::analysisLow.bandCD.powerThresholdSingle=0;
//
//
//	Model::analysisLow.bandK.powerThresholdClustering=0;
//	Model::analysisLow.bandK.powerThresholdCombined=0;
//	Model::analysisLow.bandK.powerThresholdSingle=0;
	//end of testing parameters
}

void incrementThreadMainIterLow(uint64_t &countThreadMainIteration)
{

	countThreadMainIteration++;
//	if (ConstLow::simLogPdwIteration == 1)
//	{
//		LoggingClass::logThreadMainIterCount(Model::countMainThreadIteration);
//	}
//	if (countThreadMainIteration == 15725)
//	{
//		std::cout << "hi" << std::endl;
//		exit(0);
//	}
}

void initThreadMainIterCountLow(queue<PdwDefaultClass>& inputPdwList,
		uint64_t &countThreadMainIteration)
{

	LogLowClass::logThreadMainIterInitCount(
			inputPdwList.front().timeOfArrivalStart,
			inputPdwList.back().timeOfArrivalStart, inputPdwList.size());

	countThreadMainIteration = 0;
}

