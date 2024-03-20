/*
 * trackangleclass.cpp
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#define TRACKANGLECLASS_CPP_

#include "../vo/trackangleclass.h"

//default constructor
TrackAngleClass::TrackAngleClass()
{
	this->curQuadrant = 0;
	this->curAngle = 0;
	this->pdwCountSinceLastAoaAnlaysis = 0;
	this->timeToaAngleUpdate = 0;
	this->timeSysAngleUpdate = 0;
	this->bearingFirst = 0;
	this->bearingLast = 0;
	this->bearingPreviousLast = 0;
	this->bearingRemainderLast = 0;
	this->bearingFilterLimit =
			ConstHigh::bearingDisplayFilterLimit;
	this->bearingDisplay = 0;
	this->minimumAngle = 0;
	this->maximumAngle = 0;
	this->timeIterCountBearingCalc = 0;
	this->timeIterLimitBearingCalc =
			ConstHigh::trackLimitBearingCalcIterDefault;
}

TrackAngleClass::~TrackAngleClass()
{
	// TODO Auto-generated destructor stub
}
void TrackAngleClass::serialize(SerialBufferClass & buffer)
{
	buffer.serialize(curQuadrant,0);
	buffer.serialize(curAngle,0);
	buffer.serialize(bearingLast,0);
	buffer.serialize(pdwCountSinceLastAoaAnlaysis,0);
	buffer.serialize(timeToaAngleUpdate,0);
	buffer.serialize(timeSysAngleUpdate,0);
}

