/*
 * logginglowdelegate.cpp
 *
 *  Created on: Jan 15, 2019
 *      Author: hassan
 */

#define LOGGINGLOWDELEGATE_CPP_

#include "logginglowdelegate.h"

void logLowIterationInfo(DataAnalysisClass &analysisObj,
		uint64_t lastReportingItr, uint64_t currSysIter)
{

	if (ConstLow::logHexFile == false
			|| ConstLow::logHexFileIterationInfo == false)
	{
		return;
	}

	LogIterationInfoClass iterationInfoObj;
	iterationInfoObj.reportingFlag = (lastReportingItr == currSysIter);

	iterationInfoObj.pulseCount = analysisObj.bandEJ.currentPdws.size()
			+ analysisObj.bandG.currentPdws.size()
			+ analysisObj.bandI.currentPdws.size()
			+ analysisObj.bandJ.currentPdws.size();

	if (iterationInfoObj.pulseCount == 0 && iterationInfoObj.reportingFlag == 0)
		return;

	uint64_t firstToa = -1;
	uint64_t endToa = 0;

	if (analysisObj.bandEJ.currentPdws.size() > 0)
	{
		firstToa = min(firstToa,
				analysisObj.bandEJ.currentPdws[0].timeOfArrivalStart);
		endToa = max(endToa,
				analysisObj.bandEJ.currentPdws.back().timeOfArrivalStart);
	}

	if (analysisObj.bandG.currentPdws.size() > 0)
	{
		firstToa = min(firstToa,
				analysisObj.bandG.currentPdws[0].timeOfArrivalStart);
		endToa = max(endToa,
				analysisObj.bandG.currentPdws.back().timeOfArrivalStart);
	}

	if (analysisObj.bandI.currentPdws.size() > 0)
	{
		firstToa = min(firstToa,
				analysisObj.bandI.currentPdws[0].timeOfArrivalStart);
		endToa = max(endToa,
				analysisObj.bandI.currentPdws.back().timeOfArrivalStart);
	}

	if (analysisObj.bandJ.currentPdws.size() > 0)
	{
		firstToa = min(firstToa,
				analysisObj.bandJ.currentPdws[0].timeOfArrivalStart);
		endToa = max(endToa,
				analysisObj.bandJ.currentPdws.back().timeOfArrivalStart);
	}
	
	iterationInfoObj.header.systemIteration = currSysIter;

	if (iterationInfoObj.pulseCount == 0 && iterationInfoObj.reportingFlag == 1)
	{

	}
	else
	{
		iterationInfoObj.iterationStartToa = firstToa;
		iterationInfoObj.iterationEndToa = endToa;
	}
	iterationInfoObj.logToFile(ConstLow::hexDebugIterDataFileName);
}

void logCurrentIterationPulses(vector<PdwDefaultClass> &currentPdws)
{

	if (ConstLow::logHexFile == false || ConstLow::logHexFileTrackData == false)
	{
		return;
	}
	LogIterationInputPulseClass pulseLogObj;
	pulseLogObj.pulseCount = currentPdws.size();
	pulseLogObj.header.systemIteration = Model::countThreadMainIterLow;
	if (pulseLogObj.pulseCount == 0)
		return;

	pulseLogObj.toas.reserve(pulseLogObj.pulseCount);
	vector<PdwDefaultClass>::iterator iterator;
	for (iterator = currentPdws.begin(); iterator != currentPdws.end();
			iterator++)
	{
		pulseLogObj.toas.push_back(iterator->timeOfArrivalStart);
	}
	pulseLogObj.logToFile(ConstLow::hexDebugFileName);
}
vector<PdwDefaultClass> logGetIterationPdws(TrackDefaultClass &track)
{
	int32_t inputSize = track.general.dspIterationPulseCount;
	int32_t bufferSize = track.general.dspPdwBuffer.size();
	inputSize = min(inputSize, bufferSize);

	vector<PdwDefaultClass> targetPdws(inputSize);

	int32_t startPoint = bufferSize - inputSize;
	int32_t endPoint = bufferSize;

	for (uint16_t i = startPoint, j = 0; i < endPoint; i++, j++)
	{
		targetPdws[j] = track.general.dspPdwBuffer[i];
	}

	return targetPdws;
}
void logActiveTracksInfo(vector<TrackDefaultClass> &tracks,
		int32_t inputPdwCount)
{
	if (ConstLow::logHexFile == false || ConstLow::logHexFileTrackData == false)
	{
		return;
	}
	if (inputPdwCount == 0)
		return;
	if (tracks.size() == 0)
		return;
	LogTracksPresentClass trackLogObj;
	trackLogObj.header.systemIteration = Model::countThreadMainIterLow;
	trackLogObj.trackCount = tracks.size();
	trackLogObj.trackGeneralData.resize(trackLogObj.trackCount);
	trackLogObj.trackToaData.resize(trackLogObj.trackCount);
	trackLogObj.priData.resize(trackLogObj.trackCount);
	for (uint16_t i = 0; i < tracks.size(); i++)
	{
		TrackDefaultClass &src = tracks[i];
		LogTrackGeneralDataClass &dst = trackLogObj.trackGeneralData[i];
		dst.trackID = src.general.trackID;
		dst.trackingActiveStatus = src.general.dspTrackingEnabled;

		dst.avgPa = src.power.avgPA;
		dst.avgPb = src.power.avgPB;
		dst.avgPc = src.power.avgPC;
		dst.avgPd = src.power.avgPD;

		dst.stdPa = src.power.stdPA;
		dst.stdPb = src.power.stdPB;
		dst.stdPc = src.power.stdPC;
		dst.stdPd = src.power.stdPD;

		dst.stdAngle = 0;
		dst.avgAngle = src.angle.curAngle;

		if (src.pw.pwCircular.size() > 0)
		{
			dst.avgPw = src.pw.pwCircular.back().pwAvg;
			dst.stdPw = src.pw.pwCircular.back().pwStdDev;
		}
		if (src.rf.rfCircular.size() > 0)
		{
			dst.avgRf = src.rf.rfCircular.back().rfAvg;
			dst.stdRf = src.rf.rfCircular.back().rfStdDev;
		}

		LogTrackPulseToaClass &dst2 = trackLogObj.trackToaData[i];
		vector<PdwDefaultClass> trackPdws = logGetIterationPdws(src);
		dst2.trackedPulses.resize(trackPdws.size());
		dst2.trackID = src.general.trackID;
		for (uint16_t j = 0; j < trackPdws.size(); j++)
		{
			PdwDefaultClass &tempPdw = trackPdws[j];
			dst2.trackedPulses[j].toa = tempPdw.timeOfArrivalStart;
			dst2.trackedPulses[j].trackingRfScore = tempPdw.trackingRfScore;
			dst2.trackedPulses[j].trackingPwScore = tempPdw.trackingPwScore;
			dst2.trackedPulses[j].trackingPowerScore =
					tempPdw.trackingPowerScore;
			dst2.trackedPulses[j].trackingPriScore = tempPdw.trackingPriScore;
		}

		LogTrackPriSpotStruct &dst3 = trackLogObj.priData[i];
		dst3.trackID = src.general.trackID;
		if (src.pri.priCircular.size() == 0)
		{
			dst3.priSpotsCount = 0;
			dst3.pulseCount = 0;
			continue;
		}
		LimitPriClass &tempPriObj = src.pri.priCircular.back();
		dst3.priSpotsCount = src.pri.lastPriAnalysisSpotCount;
		dst3.priType = (uint32_t) tempPriObj.priType;
		dst3.pulseCount = 0;
		for (uint16_t k = 0; k < dst3.priSpotsCount && k < 32; k++)
		{
			dst3.priSpots[k] = src.pri.priCircular[src.pri.priCircular.size()
					- k - 1].priMean;
		}
	}
	trackLogObj.logToFile(ConstLow::hexDebugFileName);
}
