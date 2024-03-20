/*
 * trackintrapulseclass.cpp
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#define TRACKINTRAPULSECLASS_CPP_

#include "../vo/trackintrapulseclass.h"

//default constructor
TrackIntraPulseClass::TrackIntraPulseClass() {

	this->timeToaIntraAnl = 0;
	this->amopPresent = false;
	this->pmopPresent = false;
	this->fmopPresent = false;
	this->umopPresent = true;
	this->pulseCountSinceLastLpiAnalysis = 0;
	trackLpiType = IntraPulseType::UMOP;
}

TrackIntraPulseClass::~TrackIntraPulseClass() {
	// TODO Auto-generated destructor stub
}

void TrackIntraPulseClass::serialize(SerialBufferClass & buffer)
{
	buffer.serialize(trackLpiType, 0);
	buffer.serialize(amopPresent, 0);
	buffer.serialize(pmopPresent, 0);
	buffer.serialize(fmopPresent, 0);
	buffer.serialize(umopPresent, 0);
}
