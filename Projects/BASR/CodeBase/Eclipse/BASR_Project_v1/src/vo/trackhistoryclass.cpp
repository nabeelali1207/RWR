/*
 * trackhistoryclass.cpp
 *
 *  Created on: Sep 26, 2019
 *      Author: hassan
 */

#define TRACKHISTORYCLASS_CPP_

#include "../vo/trackhistoryclass.h"

//default constructor
TrackHistoryClass::TrackHistoryClass() {
	this->lowHighFlag = 0;
	this->stitchedCount = 0;
	this->stitchedJitterAndComplexCount = 0;
	this->stitchedDiffToaCount = 0;
	this->totalToaOverlap = 0;
	this->totalPulseOverlap = 0;
	this->totalPulseCount = 0;
	this->stitchedUpdateCount = 0;
	this->stitchedUpdateNextCount = 0;
	this->stitchedUpdateDiffToaCount = 0;
	this->stitchedUpdateNextDiffToaCount = 0;
	this->activeFlag = 0;

	this->countConsecutiveSameSpotFixedPri = 0;

	this->countPulseWidthOverlapMaxLow = 0;
	this->countPriTypeOverlapMaxLow = 0;
	this->countPulseOverlapMaxLow = 0;
	this->countTrackOverlapMaxLow = 0;
	this->durationToaOverlapMaxLow = 0;
	this->flagOverlapPotential = 0;
	this->lowToaStartUpdateCount = 0;
	this->highToaStartUpdateTotalCount = 0;
	this->highToaStartUpdateConsecutiveCount = 0;
	this->highToaStartUpdateFlag = 0;
	this->lowToaStartUpdateFlag = 0;
	this->lowToaStartUpdateNextFlag = 0;
	this->lowIndexUpdateNext = 0;
	this->highTrackContinuationIterFlag=0;
	this->flagUpdateOnceIterHigh = 0;
	idsAssociateTracksLow.set_capacity(
			ConstHigh::trackIdsLowAccumulateCircSize);
}

TrackHistoryClass::~TrackHistoryClass() {
	// TODO Auto-generated destructor stub
}



