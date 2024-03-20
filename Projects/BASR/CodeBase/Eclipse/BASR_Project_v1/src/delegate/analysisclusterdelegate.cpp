/*
 * analysisclusterdelegate.cpp
 *
 *  Created on: Jan 6, 2019
 *      Author: hassan
 */

#define ANALYSISCLUSTERDELEGATE_CPP_

#include "../delegate/analysisclusterdelegate.h"

void clusterAnalysis(vector<PdwDefaultClass> &clusterPdws,
		vector<TrackDefaultClass> &clusterTracks, uint32_t &newPulsesCount,
		DBSCAN &dbScanObj, uint64_t currSystemIteration, uint64_t *benchmarkPtr)
{

	if (newPulsesCount < ConstLow::asClusterMinInput)
	{
		clusterTracks.resize(0);
		return;
	}

	uint64_t clusteringStartTime = Utility::getSystemTime();

	//	LogLowClass::logClusterInputSize(clusterPdws.size());

//	DBSCAN dbScanObj;

	uint32_t clusterCount = dbScanObj.clusterInputPdws(clusterPdws,
			benchmarkPtr);
	vector<int16_t> &clusterPdwsLabels = dbScanObj.getLabels();

	if (clusterCount == 0)
	{
		clusterCount = 0; //remove compiler warning
	}
	if (ConstLow::logHexFile == true
			&& ConstLow::logHexFileClusteringData == true)
	{
		LogClusterClass logObject;
		logObject.header.systemIteration = Model::countThreadMainIterLow;
		logObject.timeBenchmark = Utility::getSystemTime()
				- clusteringStartTime;
		logObject.toas.resize(clusterPdws.size());
		logObject.clusterID.resize(clusterPdws.size());
		for (uint16_t i = 0; i < clusterPdws.size(); i++)
		{
			logObject.toas[i] = (uint64_t) clusterPdws[i].timeOfArrivalStart;
			logObject.clusterID[i] = clusterPdwsLabels[i];
		}
		logObject.logToFile(ConstLow::hexDebugFileName);
		//LoggingClass::logClustersInFile(clusterPdws,clusterPdwsLabels);
	}

	//create tracks based on our clusters, make it return a vector
	clusterCreateTracks(clusterTracks, clusterPdwsLabels, clusterPdws,
			currSystemIteration);
	uint16_t clustersInProbation = clusterTracks.size();
	clusterCheckValidClusters(clusterTracks, clusterPdwsLabels, clusterPdws);
	clusterCheckSelfTestFlagOfTracks(clusterTracks);
	uint16_t validClusters = clusterTracks.size();

	if (validClusters < clustersInProbation)
	{
		newPulsesCount = 8;
	}
	else
	{
		newPulsesCount = 0;
	}

	clusterRemoveClusteredPdwsFromBuffer(clusterPdwsLabels, clusterPdws);
//	if (clusterCount > 0)
//	{
//		LogLowClass::logClusterNewCount(clusterCount);
//	}
//	else
//	{ //debugging
//		LogLowClass::logClusterNewCount(clusterCount);
//	}
}
void clusterRemoveClusteredPdwsFromBuffer(vector<int16_t> &clusterPdwsLabels,
		vector<PdwDefaultClass> &clusterPdws)
{
	uint32_t unclusteredPulseCounter = 0;
	for (uint16_t i = 0; i < clusterPdws.size(); i++)
	{
		if (clusterPdwsLabels[i] == -1)
		{
			clusterPdws[unclusteredPulseCounter] = clusterPdws[i];
			unclusteredPulseCounter++;
		}
	}
	clusterPdws.resize(unclusteredPulseCounter);

	// expire pulses that have not formed a cluster despite N clusterings done
	uint32_t validPdwCount = 0; //count for pulses that are already present in clusterPdwsVector
	for (uint32_t i = 0; i < clusterPdws.size(); i++)
	{
		if (clusterPdws[i].clusterProcessCount < ConstLow::asClusterPdwMaxIter)
		{
			if (validPdwCount != i)
				clusterPdws[validPdwCount] = clusterPdws[i];
			validPdwCount++;
		}
//		if (clusterPdws[i].timeOfArrivalStart == 219904462970) // DEBUGGING ONLY
//		{
//			cout << "hi" << endl;
//		}
	}
	clusterPdws.resize(validPdwCount);
}
void clusterGetUnclusteredPulsesBack(vector<PdwDefaultClass> &clusterPdws,
		vector<PdwDefaultClass> &iterationPdws)
{
	for (uint16_t i = 0; i < clusterPdws.size(); i++)
	{
		for (uint16_t j = 0; j < iterationPdws.size(); j++)
		{
			if (iterationPdws[j].timeOfArrivalStart
					> clusterPdws[i].timeOfArrivalStart)
				break;
			else if (iterationPdws[j].timeOfArrivalStart
					== clusterPdws[i].timeOfArrivalStart)
			{
				iterationPdws[j].clusterProcessFlag =
						clusterPdws[i].clusterProcessFlag;
			}
		}
	}

}
void clusterRemoveTrackedPulsesFromClusteringBuffer(
		vector<PdwDefaultClass> &clusterPdws,
		vector<PdwDefaultClass> &iterationPdws)
{

	for (uint16_t i = 0; i < clusterPdws.size(); i++)
	{
		for (uint16_t j = 0; j < iterationPdws.size(); j++)
		{
			if (iterationPdws[j].timeOfArrivalStart
					> clusterPdws[i].timeOfArrivalStart)
				break;
			else if (iterationPdws[j].timeOfArrivalStart
					== clusterPdws[i].timeOfArrivalStart)
			{
				clusterPdws[i].pulseTrackedFlag =
						iterationPdws[j].pulseTrackedFlag;
			}
		}
	}

	uint32_t validPdwCount = 0; //count for pulses that are already present in clusterPdwsVector
	for (uint32_t i = 0; i < clusterPdws.size(); i++)
	{
		if (clusterPdws[i].pulseTrackedFlag == false)
		{
			if (validPdwCount != i)
				clusterPdws[validPdwCount] = clusterPdws[i];
			validPdwCount++;
		}
	}
	clusterPdws.resize(validPdwCount);

}
void clusterCreateTracks(vector<TrackDefaultClass> &clusterTracks,
		vector<int16_t> &clusterPdwsLabels,
		vector<PdwDefaultClass> &clusterPdws, uint64_t currSystemIteration)
{
	// count number of clusters
	int16_t clusterCount = 0;
	for (uint16_t i = 0; i < clusterPdwsLabels.size(); i++)
	{
		clusterPdws[i].clusterProcessCount++; //increment number of times that this pulse has been clustered

		if (clusterPdwsLabels[i] >= clusterCount)
		{
			clusterCount = clusterPdwsLabels[i] + 1;
		}
	}
	if (clusterCount == 0)
	{
		return;
	}

	//allocate dsp specific objects
	//dsp specific objects are required only at low side, implemented as vectors of size 0 by defualt
	//but low side allocates them to size 1
	clusterTracks.resize(clusterCount);
	for (uint16_t i = 0; i < clusterCount; i++)
	{
		clusterTracks[i].power.AllocateHistograms();
		clusterTracks[i].pri.allocateSequenceSearchObject();
		clusterTracks[i].pw.AllocatePwTrackingObject();
		clusterTracks[i].rf.AllocateRfTrackingObject();
		clusterTracks[i].general.dspTrackingEnabled = true;
		clusterTracks[i].general.dspCandidateForStringingFlag = false;
		clusterTracks[i].general.trackType=TrackTypeEnum::track_pulse;

	}

	int16_t currTrackIndex = 0;
	for (uint16_t i = 0; i < clusterPdwsLabels.size(); i++)
	{
		currTrackIndex = clusterPdwsLabels[i];
		if (clusterPdwsLabels[i] != -1)
			clusterPdws[i].clusterProcessFlag = true;
		if (currTrackIndex >= 0)
		{
//			if (clusterPdws[i].timeOfArrivalStart == 82896498355) // DEBUGGING ONLY
//			{
//				cout << "hi" << endl;
//			}


			if (clusterTracks[currTrackIndex].general.timeSysCreation == 0)
			{

				clusterTracks[currTrackIndex].general.toaFirst =
						clusterPdws[i].timeOfArrivalStart;

				clusterTracks[currTrackIndex].general.timeSysCreation =
						Utility::getSystemTime();
				clusterTracks[currTrackIndex].pri.priTrackingObj[0].setFirstToa(
						clusterPdws[i].timeOfArrivalStart);
				clusterTracks[currTrackIndex].power.scanAnalysis2[0].addFirstPdw(clusterPdws[i]);
				clusterTracks[currTrackIndex].rf.bandCode = clusterPdws[i].band;
				if(clusterPdws[i].frequency!=0)
				{
					clusterTracks[currTrackIndex].rf.rfTrackingObj[0].rfMedian = clusterPdws[i].frequency;
				}

			}
			else
			{
				trackAddPulseToPriPwBuffer(clusterTracks[currTrackIndex],
						clusterPdws[i]);
				if (clusterTracks[currTrackIndex].pri.priTrackingObj[0].newResultsAvailableForCopyFlag
						== 1)
				{
					priCopyResultFromFromTrackingObject(
							clusterTracks[currTrackIndex]);
				}
				pwUpdateActiveSpotInfo(clusterTracks[currTrackIndex]);
				if (clusterPdws[i].band == BandCodeEnum::band_k) // for k band only
					clusterTracks[currTrackIndex].rf.bandCode =
							BandCodeEnum::band_k;
			}

			clusterTracks[currTrackIndex].general.timeSysLastUpdate =
					Utility::getSystemTime();
			clusterTracks[currTrackIndex].general.dspPdwBuffer.push_back(
					clusterPdws[i]);
			clusterTracks[currTrackIndex].general.timeToaLastUpdate =
					clusterPdws[i].timeOfArrivalStart;
			clusterTracks[currTrackIndex].general.toaLast =
					clusterPdws[i].timeOfArrivalStart;
			if(clusterPdws[i].timeOfArrivalStart == 9065003)
				clusterPdws[i].timeOfArrivalStart=clusterPdws[i].timeOfArrivalStart*1;
			clusterTracks[currTrackIndex].rf.rfTrackingObj[0].AddNextRfValue(
					clusterPdws[i].frequency,
					clusterPdws[i].frequencyValidFlag);

			powerUpdateTrackPowersOnPulseAssociation(
					clusterTracks[currTrackIndex], clusterPdws[i]);

			lpiUpdateTrackLpiOnPulseAssociation(clusterTracks[currTrackIndex],clusterPdws[i]);
			clusterTracks[currTrackIndex].general.dspIterationPulseCount++;
			clusterTracks[currTrackIndex].angle.pdwCountSinceLastAoaAnlaysis++;
		}
	}
	for (uint16_t i = 0; i < clusterTracks.size(); i++)
	{
//		clusterTracks[i].general.timeToDeclareSilent = 1000;	//1 millisecond
		powerUpdateFirstStateOfPowerVariables(clusterTracks[i]);
		if (clusterTracks[i].angle.pdwCountSinceLastAoaAnlaysis
				>= ConstLow::asAoaPdwCountMin)
		{
			doaAnalysisExecute(clusterTracks[i]);
			powerDetermineWeights_V2(clusterTracks[i]);
		}

		if (clusterTracks[i].pri.maxPriSpot == 0)
		{
			int64_t startTime =
					clusterTracks[i].general.dspPdwBuffer.front().timeOfArrivalStart;
			int64_t endTime =
					clusterTracks[i].general.dspPdwBuffer.back().timeOfArrivalStart;
			int16_t sampleCount = clusterTracks[i].general.dspPdwBuffer.size();
			int64_t avgPri = (endTime - startTime) / sampleCount;
			int32_t emitterSilenceTimeToa = 20 * avgPri;

			//time in micros
			int32_t emitterSilenceTime = emitterSilenceTimeToa
					/ ConstLow::toaCountInMicros;

			emitterSilenceTime = min(50000, emitterSilenceTime); //50 milli second
			emitterSilenceTime = max(500, emitterSilenceTime); //0.5 milli second

			clusterTracks[i].general.timeToDeclareSilent = emitterSilenceTime;
		}
	}
}

void clusterCheckSelfTestFlagOfTracks(vector<TrackDefaultClass> &clusterTracks)
{
	for (uint16_t i = 0; i < clusterTracks.size(); i++)
	{
		uint16_t selfTestFlagCount = 0;
		for (uint16_t j = 0; j < clusterTracks[i].general.dspPdwBuffer.size(); j++)
		{
			if(clusterTracks[i].general.dspPdwBuffer[j].selfTestFlag == 1)
			{
				selfTestFlagCount++;
			}
		}
		if(selfTestFlagCount > 0.7*clusterTracks[i].general.dspPdwBuffer.size())
		{
			clusterTracks[i].general.flagSelfTest = 1;
		}
		else
		{
			clusterTracks[i].general.flagSelfTest = 0;
		}
	}
}
void clusterCheckValidClusters(vector<TrackDefaultClass> &clusterTracks,
		vector<int16_t> &clusterPdwsLabels,
		vector<PdwDefaultClass> &clusterPdws)
{

	for (uint16_t i = 0; i < clusterTracks.size(); i++)
	{
		if (clusterTracks[i].general.dspPdwBuffer.size() >= 16)
		{
			continue;
		}
		uint16_t stdDevOfPowers[4] =
		{ clusterTracks[i].power.stdPA, clusterTracks[i].power.stdPB,
				clusterTracks[i].power.stdPC, clusterTracks[i].power.stdPD };
		if (clusterTracks[i].pri.priTrackingObj[0].currPriType
				== SequenceTypesEnum::seq_unknown)
		{
			if (stdDevOfPowers[clusterTracks[i].power.dominantAntenna] > 10)
			{
				clusterTracks[i].general.dspDeleteTrack = true;
				for (uint16_t j = 0; j < clusterPdwsLabels.size(); j++)
				{
					if (clusterPdwsLabels[j] == i)
					{
						clusterPdwsLabels[j] = -1;
						clusterPdws[j].clusterProcessFlag = false;
					}
				}

			}

		}

	}
	trackDeleteInactiveTracks(clusterTracks);

}
//delete input pdw that are unwanted for any reason, this is useful when we
//are pre-excluding pulses based on some criteria (such as rf or angle)
void clusterExcludeCriteriaPdw(list<PdwDefaultClass> &currentPdws)
{
	//nothing here for now
}
void clusterInitiateNewTracks(vector<TrackDefaultClass> &tracks,
		vector<TrackDefaultClass> &clusterTracks, uint64_t currSystemIteration,
		uint32_t &stringableTrackCount)
{
	// we find the time after which updation of a track stops based on AVG-PRI
	for (uint16_t i = 0; i < clusterTracks.size(); i++)
	{
//		int64_t startTime =
//				clusterTracks[i].general.dspPdwBuffer.front().timeOfArrivalStart;
//		int64_t endTime =
//				clusterTracks[i].general.dspPdwBuffer.back().timeOfArrivalStart;
//		int16_t sampleCount = clusterTracks[i].general.dspPdwBuffer.size();
//		int64_t avgPri = (endTime - startTime) / sampleCount;
//
//		int32_t emitterSilenceTimeToa = 20 * avgPri;
//
//		//time in micros
//		int32_t emitterSilenceTime = emitterSilenceTimeToa
//				/ ConstLow::toaCountInMicros;
//
//		emitterSilenceTime = min(50000, emitterSilenceTime);//50 milli second
//		emitterSilenceTime = max(500, emitterSilenceTime);	//0.5 milli second
//
//		clusterTracks[i].general.timeToDeclareSilent = emitterSilenceTime;
	}
	for (uint16_t i = 0; i < clusterTracks.size(); i++)
	{
		//we need some criteria to reject clusters that are from a noise source
		//this means that if all processes such as pri analysis, rf analysis
		//and pw analysis failure should result in a rejected track candidate
//		tracks.reserve(1);
		clusterTracks[i].general.timeSysLastUpdate = Utility::getSystemTime();
		clusterTracks[i].general.iterSysLastUpdate = currSystemIteration;
		if (clusterTracks[i].pri.priFirst.size() > 0)
		{
			stringableTrackCount++;
		}
		tracks.push_back(clusterTracks[i]);

	}
	clusterTracks.resize(0);
}
void clusterSortTrackPdw(vector<TrackDefaultClass> &tracks)
{
	for (uint16_t i = 0; i < tracks.size(); i++)
	{
		circular_buffer<PdwDefaultClass> &buffer =
				tracks[i].general.dspPdwBuffer;
		sort(buffer.begin(), buffer.end(), PdwDefaultClass::cmpToaStart);
		tracks[i].general.timeToaLastUpdate = buffer.back().timeOfArrivalStart;
		tracks[i].general.timeSysLastUpdate = Utility::getSystemTime();
	}
}

void clusterPopulateInputPdw(vector<PdwDefaultClass> &currentPdws,
		vector<PdwDefaultClass> &clusterPdws, uint32_t &newPulsesCount,
		uint16_t clusteringPowerThreshold)
{

	// add new pulses
	uint16_t clusterPulseCount = clusterPdws.size();
	for (uint32_t i = 0; i < currentPdws.size(); i++)
	{
		if (clusterPulseCount >= ConstLow::asClusterMaxInput)
		{
			break;
		}
		if (currentPdws[i].powerThresholdFlag == false)
		{ //ignore pdws that are below power threshold
			continue;
		}
		if ((currentPdws[i].powerX + currentPdws[i].powerY)
				< clusteringPowerThreshold)
		{ //ignore pdws that are below clustering power threshold
			currentPdws[i].powerThresholdFlag = false;
			continue;
		}
		if (currentPdws[i].pulseTrackedFlag == false
				&& currentPdws[i].pulseSentForClusteringFlag == false)
		{
			//push untracked pulses into clusterPdwsVector
			newPulsesCount++;
			clusterPdws.push_back(currentPdws[i]);
			clusterPulseCount++;
			currentPdws[i].pulseSentForClusteringFlag = true;
			currentPdws[i].clusterProcessFlag = true;
		}
	}

	// expire pulses older then a max limit
	uint16_t validPdwCount = 0;
	for (uint32_t i = 0; i < clusterPdws.size(); i++)
	{
		int64_t lastPulseTime = clusterPdws.back().timeOfArrivalStart;
		int64_t currPulseTime = clusterPdws[i].timeOfArrivalStart;
		int64_t diff = lastPulseTime - currPulseTime;
		diff = diff / ConstLow::toaCountInMs;

		if (diff < ConstLow::asClusterMaxTimeWindowMillis)
		{
			clusterPdws[validPdwCount] = clusterPdws[i];
			validPdwCount++;
		}
		else if (clusterPdws[i].clusterProcessCount == 0 && newPulsesCount > 0)
		{
			newPulsesCount--;
		}
	}
	clusterPdws.resize(validPdwCount);

}
