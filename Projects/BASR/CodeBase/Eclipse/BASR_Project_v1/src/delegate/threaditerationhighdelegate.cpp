/*
 * threaditerationhighdelegate.cpp
 *
 *  Created on: Sep 26, 2019
 *      Author: hassan
 */

#define THREADITERATIONHIGHDELEGATE_CPP_

#include "threaditerationhighdelegate.h"

void threadCompleteIterationHigh(DataSensorClass &low,
		DataSensorClass &lowNext, DataSensorClass &high) {

	//debug iter 33 for merge problem
	//debug id 2299 investigate quadrant jump
	//debug iteration 2005, investigate angle jumps
	//iteration debug hook
	if(Model::countThreadMainIterHigh == 2220) {
		Model::countThreadMainIterHigh = 2220;
	}
	//end of iteration debug hook
	trackSortTracksToaStartLow(low);//sort low tracks and store indexes
	trackProcessConsecutiveFirstToaLow(low.tracks, lowNext.tracks);
	trackPreProcessIterLow(low.tracks);
	trackPreProcessIterLow(lowNext.tracks);
	trackPreProcessIterHigh(high.tracks);
	trackMatchLibraryAllLow(low);
	trackProcessMaxToaLastIter(low);
	trackCalcMaxLastToaCurrentIter(low);
	trackCalcQuadrantAndPxPyAllLow(low.tracks);
	trackCalcBearingLastAll(low.tracks);
	trackProcessFutureParametersLow(low.tracks, lowNext.tracks);
	trackCalcOverlapParametersLow(low.tracks, lowNext.tracks);
	trackUpdateTrackFirstToaHigh(low.tracks, lowNext.tracks, high.tracks);
	mergeInitializeScoreStructureLowLow(low);
	mergeInitializeScoreStructureHighLow(low, high);
	mergeCompareInitialAllLowLow(low);
	mergeCompareInitialAllHighLow(low, high);
//	mergeCompareInitialAllHighHigh(high);
	//need to incorporate special rules for library match where we rematch for
	//fixed pri types and perhaps stt, ml launch  tracks before declaring
	//use this functions as an example void UpdateTrackLibraryAlternative(Track* pTrack)
	//declare ground/air and mode based on band and pri types
	//set default erp based on band
	trackMatchLibraryAllHigh(high);
	trackStringAndInsert(low, high);
	trackProcessConsecutiveFirstToaCountAllHigh(high);
	trackMatchLibraryAllHigh(high);
	mergeInitializeScoreStructureHighHigh(high);
	mergeTracksAllHigh(low, high);//merge tracks
	trackFinalizeTracksAllHigh(low, high);
	//trackCalcBearingLastAll(high.tracks);//this function is no longer required
	trackCalcBearingDisplayAllHigh(high.tracks);
	trackCalcRangeDisplayAllHigh(high);
	trackCalcQuadrantAndPxPyAllHigh(high.tracks);
	tracksIlluminationBasedScanAnalysis(high.tracks);
	//trackCalcTrackingModeAllHigh(high.tracks);
	trackPostProcessIterHigh(high.tracks);
	trackExpireTracksHigh(high.tracks);
	trackReleaseTracksAllHigh(high);
	trackExpireHistoryParameterEnvironment(high);
	trackExpireHistoryRangeEnvironment(high);
	//need iteration times var/limits for libmatch, distance, range, scan,
}


