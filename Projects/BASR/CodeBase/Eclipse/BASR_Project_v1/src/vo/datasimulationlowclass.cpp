/*
 * datasimulationlowclass.cpp
 *
 *  Created on: Oct 18, 2018
 *      Author: hassan
 */

#define DATASIMULATIONLOWCLASS_CPP_

#include "datasimulationlowclass.h"

DataSimulationLowClass::DataSimulationLowClass() {
	// TODO Auto-generated constructor stub
	this->currSystemTime = Utility::getSystemTime();
	this->lastSystemTime = Utility::getSystemTime();
	this->pdwIterationStartTime = 0;
	this->pdwIterationEndTime = 0;
	this->systemTimeToToaTimeOffset = 0;

	this->systemTimeToToaTimeOffset = this->currSystemTime;
	this->systemTimeToToaTimeOffset = this->systemTimeToToaTimeOffset
			- (int64_t) this->pdwIterationEndTime;

	iterationTimeFromFile_ptr=0;
	callReportingFlag=0;
}

DataSimulationLowClass::~DataSimulationLowClass() {
	// TODO Auto-generated destructor stub
}

