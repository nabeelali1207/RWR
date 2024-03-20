/*
 * dataenvironmentclass.cpp
 *
 *  Created on: Jan 1, 2019
 *      Author: hassan
 */

#define DATAENVIRONMENTCLASS_CPP_

#include "../vo/dataenvironmentclass.h"

DataEnvironmentClass::DataEnvironmentClass() {
	// TODO Auto-generated constructor stub
	this->bandCode = BandCodeEnum::band_uncomputed;
	this->bearing = 0;
	this->updateCount = 0;
	this->priType = PriTypeEnum::pri_uncomputed;
	this->priCount = 0;
	this->pwLow = 0;
	this->pwHigh = 0;
	this->rfLow = 0;
	this->rfHigh = 0;
	this->priLow = 0;
	this->priHigh = 0;
	this->priSum = 0;
	this->lifeTime = 0;
	this->toaStart = 0;
	this->toaEnd = 0;
	this->rangePositionDisplay = 0;
	this->rangeMeters = 0;
}

DataEnvironmentClass::~DataEnvironmentClass() {
	// TODO Auto-generated destructor stub
}





