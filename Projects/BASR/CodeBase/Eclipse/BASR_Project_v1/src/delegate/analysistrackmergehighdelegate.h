/*
 * analysistrackmergehighdelegate.h
 *
 *  Created on: Nov 20, 2018
 *      Author: hassan
 */

#ifndef ANALYSISTRACKMERGEHIGHDELEGATE_H_
#define ANALYSISTRACKMERGEHIGHDELEGATE_H_

#include "../common/common.h"
#include "../delegate/analysisangledelegate.h"
#include "../utility/constant.h"
#include "../vo/datasensorclass.h"

//initialization functions needed for highlow,lowlow,highhigh (if required)
//comparison functions needed for highlow,lowlow,highhigh
void mergeTracksAllHigh(DataSensorClass &low, DataSensorClass &high);
int16_t mergeTracksWithNormalPri(DataSensorClass &low, DataSensorClass &high,
		TrackDefaultClass &trackOne, TrackDefaultClass &trackTwo,
		uint16_t indexTrackOne, uint16_t indexTrackTwo);
int16_t mergeTracksWithUncertainPri(DataSensorClass &low, DataSensorClass &high,
		TrackDefaultClass &trackOne, TrackDefaultClass &trackTwo,
		uint16_t indexTrackOne, uint16_t indexTrackTwo);
void mergeInitializeScoreStructureLowLow(DataSensorClass &low);
void mergeInitializeScoreStructureHighLow(DataSensorClass &low,
		DataSensorClass &high);
void mergeInitializeScoreStructureHighHigh(DataSensorClass &high);
void mergeCompareInitialAllLowLow(DataSensorClass &low);
void mergeCompareInitialAllHighLow(DataSensorClass &low,
		DataSensorClass &high);
void mergeCompareInitialAllHighHigh(DataSensorClass &high);
void mergeCompareInitialHighLow(DataSensorClass &low, DataSensorClass &high,
		uint16_t lowIndex, uint16_t highIndex);
uint8_t mergeCompareFinalizeHighHigh(TrackDefaultClass &track,
		DataSensorClass &high);
uint8_t mergeComparePriHighHigh(TrackDefaultClass &track,
		DataSensorClass &high);
bool findIfNthMultiple(uint32_t queryValue, uint32_t spotValue,
		int16_t devAllowed);
uint8_t mergeFindUnknownsWithinAngleWindow(TrackDefaultClass &track,
		DataSensorClass &high);
uint8_t mergeCalcStatusStringRf(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo);
//find if pris were previously sorted in pup-1 and how to get first and last
//index from circ buffer
uint8_t mergeCalcScoreStringPri(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo);
uint16_t mergeDifferencePw(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo);
uint16_t mergeDifferencePwAccumulate(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo);
uint16_t mergeDifferencePriType(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo);
uint16_t mergeCalcScoreStringHighLow(DataSensorClass &low, DataSensorClass &high,
		uint16_t lowIndex, uint16_t highIndex);
uint16_t mergeCalcScoreStringLowLow(DataSensorClass &low,
		DataSensorClass &high, uint16_t indexOne, uint16_t indexTwo);
uint8_t mergeFlagStringExcludeHighLow(DataSensorClass &low, DataSensorClass &high,
		uint16_t lowIndex, uint16_t highIndex);
void mergeCalcBearingDifferenceLowLow(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo, uint16_t &angleDiff,
		uint16_t &angleDiffQuadrantSwap);
//modify this function to use high side calculated px,py,bearing,qcode
void mergeCalcBearingDifferenceHighLow(TrackDefaultClass &low,
		TrackDefaultClass &high, uint16_t &angleDiff,
		uint16_t &angleDiffQuadrantSwap);
//modify this function to use high side calculated px,py,bearing,qcode
void mergeCalcBearingDifferenceHighHigh(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo, uint16_t &angleDiff,
		uint16_t &angleDiffQuadrantSwap);
void mergeCalcOverlapLowLow(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo, uint32_t &pulseOverlap,
		uint64_t &toaOverlap);
void mergeCalcOverlapHighLow(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo, uint32_t &pulseOverlap, uint64_t &toaOverlap);

//All definitions
#ifdef ANALYSISTRACKMERGEHIGHDELEGATE_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion



#endif /* ANALYSISTRACKMERGEHIGHDELEGATE_H_ */
