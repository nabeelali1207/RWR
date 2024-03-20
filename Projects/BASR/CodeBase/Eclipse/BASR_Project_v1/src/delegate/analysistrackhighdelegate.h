/*
 * analysistrackhighdelegate.h
 *
 *  Created on: Sep 25, 2019
 *      Author: hassan
 */

#ifndef ANALYSISTRACKHIGHDELEGATE_H_
#define ANALYSISTRACKHIGHDELEGATE_H_

#include "../common/common.h"
#include "../model/model.h"
#include "../delegate/analysislibrarydelegate.h"
#include "../delegate/analysistrackmergehighdelegate.h"
#include <numeric>      // std::iota
#include <algorithm>    // std::sort, std::stable_sort

//any processing and corrections that need to be made in low tracks would be
//made in this function. It would operate on low reports one iteration behind
//also copy all the parameters in the accumulative structures in low
void trackPreProcessIterLow(vector<TrackDefaultClass> &tracks);
//reset high processing flag to 0
uint8_t trackComparePriLowLow(TrackDefaultClass &track1,
		TrackDefaultClass &track2);
uint8_t trackComparePwLowLow(TrackDefaultClass &track1,
		TrackDefaultClass &track2);
uint8_t trackCompareAngleLowLow(TrackDefaultClass &track1,
		TrackDefaultClass &track2);
void trackPreProcessIterHigh(vector<TrackDefaultClass> &tracks);
//increment lifetime of each high track
void trackPostProcessIterHigh(vector<TrackDefaultClass> &tracks);
void trackRemovePriDuplicates(TrackDefaultClass &track);
//update the counts for accumulative pri types in this track
void trackCountUpdatePriAccumulative(TrackDefaultClass &track);
void trackCountUpdatePriDominantCurrent(TrackDefaultClass &track);
void trackCountUpdateRfDominantCurrent(TrackDefaultClass &track);
void trackCountUpdatePriDominantFuture(TrackDefaultClass &track,
		TrackDefaultClass &trackNext);
//delete tracks whose lifetime counter has reached a limit or those marked for
//deletion
void trackExpireTracksHigh(vector <TrackDefaultClass> &tracks);
void trackCfdDataTracksHigh(vector<TrackDefaultClass> &tracks,
		LibraryStruct &library, CfdLibrary &cfdLib);
//sort low tracks based on their start toa and store sorted indices inside
//the datasensor object
void trackSortTracksToaStartLow(DataSensorClass &dataSensor);
//compute update count based on first toa in low, set update/updatenext flags
//also set the index of the first toa updated report (for access from lownext)
void trackProcessConsecutiveFirstToaLow(vector <TrackDefaultClass> &tracks,
		vector <TrackDefaultClass> &tracksNext);
void trackProcessConsecutiveFirstToaCountAllHigh(DataSensorClass &high);
//set future pri type, fixed pri flag, future pri counts, trash mismatched pri
//we would also remove duplicate pris here along with unwanted ones
void trackProcessFutureParametersLow(vector <TrackDefaultClass> &tracks,
		vector <TrackDefaultClass> &tracksNext);
//remove the pri type passed in the function parameter from accumulate buffer
void trackRemovePriCircByType(TrackDefaultClass &track, PriTypeEnum priType);
void trackProcessMaxToaLastIter(DataSensorClass &dataSensor);
void trackCalcMaxLastToaCurrentIter(DataSensorClass &dataSensor);
//calculate overlap rf and pw potential from low tracks
void trackCalcOverlapParametersLow(vector <TrackDefaultClass> &tracks,
		vector <TrackDefaultClass> &tracksNext);
//update based on first toa, remove duplicate pri based on current and future
//also force the track to turn unknown and rematch based on certain conditions
//including a mismatched library category
void trackUpdateTrackFirstToaHigh(vector <TrackDefaultClass> &tracksLow,
		vector <TrackDefaultClass> &tracksLowNext,
		vector <TrackDefaultClass> &tracksHigh);
//update actual parameters based on organic update
//re count accumulated pri,pw,rf in high
void trackUpdateParameterOrganic(TrackDefaultClass &low,
		TrackDefaultClass &lowNext, TrackDefaultClass &high, uint16_t lowIndex,
		uint16_t lowNextIndex, uint16_t highIndex);
//update actual parameters based on string update
//re count accumulated pri,pw,rf in high
void trackUpdateParameterString(TrackDefaultClass &low,
		TrackDefaultClass &lowNext, TrackDefaultClass &high, uint16_t lowIndex,
		uint16_t lowNextIndex, uint16_t highIndex);

void trackUpdateAccumlatedPwInfo(TrackDefaultClass &high);
void trackUpdateAccumlatedPriInfo(TrackDefaultClass &high);


void trackMatchLibraryTest(DataSensorClass &high);
void trackMatchLibraryAllLow(DataSensorClass &low);
void trackMatchLibraryAllHigh(DataSensorClass &high);
uint8_t trackCreateEntryParameterEnvironment(DataSensorClass &dataSensor,
		TrackDefaultClass &track);
uint8_t trackRedeemEntryParameterEnvironment(DataSensorClass &dataSensor,
		TrackDefaultClass &track);
void trackExpireHistoryParameterEnvironment(DataSensorClass &dataSensor);
void trackExpireHistoryRangeEnvironment(DataSensorClass &dataSensor);
//time this using iter and calculate with a buffer variable
void trackCalcBearingDisplayAllHigh(vector <TrackDefaultClass> &tracks);
void trackCalcBearingLastAll(vector <TrackDefaultClass> &tracks);
void trackCalcQuadrantAndPxPyAllLow(vector <TrackDefaultClass> &tracks);
void trackCalcQuadrantAndPxPyAllHigh(vector <TrackDefaultClass> &tracks);
void trackCalcQuadrantAndPxPyHigh(TrackDefaultClass &track);
//time this using iter and calculate with a buffer variable
void trackCalcRangeDisplay(TrackDefaultClass &track);
void trackCalcRangeDisplayAllHigh(DataSensorClass &high);
void tracksIlluminationBasedScanAnalysis(vector<TrackDefaultClass>&);
ScanAnalysisHistoryClass trackIlluminationBasedScanAnalysis(TrackDefaultClass &);
void trackCalcTrackingModeAllHigh(vector <TrackDefaultClass> &tracks);
void trackCreateTrackProbationHigh(TrackDefaultClass &low,
		TrackDefaultClass &lowNext, DataSensorClass &dataSensor);
void trackFinalizeTracksAllHigh(DataSensorClass &low, DataSensorClass &high);
void trackFinalizeTrackHigh(TrackDefaultClass &track);
void trackCalcFeasibilityFinalizeTrackHigh(DataSensorClass &high,
		TrackDefaultClass &highTrack, uint8_t &categoryMatchCount,
		uint8_t &indexMatchCount, uint8_t &sttMatchCount,
		uint16_t &minBearingDiff, uint8_t &sttPwComboCount);
void trackReleaseTracksAllHigh(DataSensorClass &high);
int16_t trackGetSortedTrackIndexLowNext(DataSensorClass &low);
void trackStringAndInsert(DataSensorClass &low, DataSensorClass &high);
//All definitions
#ifdef ANALYSISTRACKHIGHDELEGATE_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion

#endif /* ANALYSISTRACKHIGHDELEGATE_H_ */
