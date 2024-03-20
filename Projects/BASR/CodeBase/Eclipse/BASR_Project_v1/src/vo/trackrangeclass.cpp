/*
 * trackrangeclass.cpp
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#define TRACKRANGECLASS_CPP_

#include "../vo/trackrangeclass.h"

//default constructor
TrackRangeClass::TrackRangeClass() {
	this->amplitudeCountMax = 0;
	this->amplitudeCountDelta = 0;
	this->amplitudeStepDisplay = 0;
	this->timeIterCountRangeCalc = 0;
	this->timeIterLimitRangeCalc =
			ConstHigh::trackLimitRangeCalcIterDefault;
	this->rangeMeters=0;
}

TrackRangeClass::~TrackRangeClass() {
	// TODO Auto-generated destructor stub
}
