/*
 * threaditerationlowdelegate.cpp
 *
 *  Created on: Oct 19, 2018
 *      Author: hassan
 */

#define THREADITERATIONLOWDELEGATE_CPP_

#include "threaditerationlowdelegate.h"

//verify entire sequence of events to make sure analysis is optimal
void threadCompleteIterationLow(DataSensorClass &bandObj)
{
	int32_t inputPdwCount = bandObj.currentPdws.size();

	//log input pdws for current iteration
	logCurrentIterationPulses(bandObj.currentPdws);

	//log of benchmarks
	LogSystemBenchmarkClass logBenchmarks;
	logBenchmarks.header.systemIteration = Model::countThreadMainIterLow;
//	logBenchmarks.iterationStartTime = Utility::getSystemTime();


	timeSetToaReference(bandObj.currentPdws, bandObj.timeToaReference);


	//tracking with benchmarks
//	logBenchmarks.checkPointTime[0] = Utility::getSystemTime();
	trackPdwIntoExistingTracks(bandObj.tracks, bandObj.currentPdws,
			Model::countThreadMainIterLow);
//	logBenchmarks.checkPointTime[1] = Utility::getSystemTime();


	/*clustering with benchmarks */
	// Fill clustering buffer
	clusterPopulateInputPdw(bandObj.currentPdws, bandObj.clusterPdws,
			bandObj.newPulsesInClusteringBufferCount,bandObj.powerThresholdClustering);
	// Perform clustering
	clusterAnalysis(bandObj.clusterPdws, bandObj.clusterTracks,
			bandObj.newPulsesInClusteringBufferCount, bandObj.dbScanObj,
			Model::countThreadMainIterLow, &logBenchmarks.checkPointTime[6]);
//	logBenchmarks.checkPointTime[2] = Utility::getSystemTime();

	if (bandObj.clusterTracks.size() > 0)
	{
		clusterSortTrackPdw(bandObj.clusterTracks); // sort all the pdws from clustering according to TOAs for further processing
		clusterGetUnclusteredPulsesBack(bandObj.clusterPdws, bandObj.currentPdws);
		rfUpdateActiveSpotInfo(bandObj.clusterTracks); // copy RF information to their tracks, if available
		priUpdatePriActiveSpotInfo(bandObj.clusterTracks,bandObj.stringableTrackCount); // copy PRI information to their tracks, if available
		pwUpdateActiveSpotInfoForAllTracks(bandObj.clusterTracks); // copy PW information to their tracks, if available

		// Tracking of pulse with the active tracks
		trackPdwIntoExistingTracks(bandObj.clusterTracks, bandObj.currentPdws,
				Model::countThreadMainIterLow);

		clusterRemoveTrackedPulsesFromClusteringBuffer(bandObj.clusterPdws, bandObj.currentPdws);

		// Initialize new tracks after successful clustering
		clusterInitiateNewTracks(bandObj.tracks, bandObj.clusterTracks,
				Model::countThreadMainIterLow,bandObj.stringableTrackCount);
	}
	//benchmarks
//	logBenchmarks.checkPointTime[3] = Utility::getSystemTime();
	rfUpdateActiveSpotInfo(bandObj.tracks);		// copy new RF information to their tracks, if any
	priUpdatePriActiveSpotInfo(bandObj.tracks,bandObj.stringableTrackCount); // copy new PRI information to their tracks, if any
	pwUpdateActiveSpotInfoForAllTracks(bandObj.tracks); 	// copy new PW information to their tracks, if any


	stringTracksAllLow(bandObj);

//	logBenchmarks.checkPointTime[4] = Utility::getSystemTime();


//	logBenchmarks.checkPointTime[5] = Utility::getSystemTime();
	logActiveTracksInfo(bandObj.tracks, inputPdwCount);

	//.csv logof tracks/output report of low side
	LogLowClass::writeReportCsv(bandObj.tracks, ConstLow::csvOutFileName);

	trackStopUpdatesOfSilentTracks(bandObj.tracks,
			Model::countThreadMainIterLow);		// silent the tracks with no activity
	trackProcessOnIterationEnd(bandObj.tracks, Model::countThreadMainIterLow);

	/* Log current iteration information */
//	logBenchmarks.iterationEndTime = Utility::getSystemTime();

	if ((inputPdwCount != 0) || (bandObj.tracks.size() != 0))
	{
		logBenchmarks.logToFile(ConstLow::hexDebugFileName);
	}
}

void threadTrainingIterationLow(DataSensorClass &bandObj)
{

}
