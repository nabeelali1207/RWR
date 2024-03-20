/*
 * trackcompareclass.cpp
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#define TRACKCOMPARECLASS_CPP_

#include "../vo/trackcompareclass.h"

//default constructor
TrackCompareClass::TrackCompareClass() {
	this->parameterInitialStatus = 0;
	this->parameterCompareStatus = 0;
	this->parameterExclusionStatus = 0;
	this->stitchCompatible = 0;
	this->mergeCompatible = 0;
	this->angleDiff = 0;
	this->angleDiffQuadrantSwap = 0;
	this->angleCompatible = 0;
	this->angleScore = 0;
	this->bandCompatible = 0;
	this->bandScore = 0;
	this->pulseDiffOverlap = 0;
	this->toaScore = 0;
	this->toaDiffFirstLast = 0;
	this->toaDiffOverlap = 0;
	this->pxDiff = 0;
	this->pyDiff = 0;
	this->pxScore = 0;
	this->pyScore = 0;
	this->pwScoreThree = 0;
	this->pwScoreTen = 0;
	this->pwDiff = 0;
	this->powerScore = 0;
	this->powerDiff = 0;
	this->rfScore = 0;
	this->threatLibraryScoreLowBar = 0;
	this->threatLibraryScoreHighBar = 0;
	this->priDetailCompareStatus = 0;
	this->priTypeCompatible = 0;
	this->priTypeScore = 0;
	this->priFailedScore = 0;
	this->priInitialScore = 0;
	this->priDetailScore = 0;
	this->overallScore = 0;
}

TrackCompareClass::~TrackCompareClass() {
	// TODO Auto-generated destructor stub
}
