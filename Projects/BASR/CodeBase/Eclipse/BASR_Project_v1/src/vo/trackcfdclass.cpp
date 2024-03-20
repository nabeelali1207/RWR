/*
 * trackcfdclass.cpp
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#define TRACKCFDCLASS_CPP_

#include "../vo/trackcfdclass.h"

//default constructor
TrackCfdClass::TrackCfdClass() {
	this->validCfdThreat = 0;
	this->threatIndex = 0;
	this->cmProgramId = 0;
	this->altitudeValidity = 0;
	this->velocityValidity = 0;
	this->aoa = 0;
	this->status = 0;
	this->altitude = 0;
	this->velocity = 0;
	this->flagCfdProgramFired = 0;
}

TrackCfdClass::~TrackCfdClass() {
}
