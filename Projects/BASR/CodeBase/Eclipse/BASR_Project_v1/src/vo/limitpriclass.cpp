/*
 * limitpriclass.cpp
 *
 *  Created on: Dec 30, 2018
 *      Author: test
 */

#define LIMITPRICLASS_CPP_

#include "../vo/limitpriclass.h"

//default constructor
LimitPriClass::LimitPriClass() {
	this->limitType = LimitTypeEnum::type_pri;
	this->priType = PriTypeEnum::pri_uncomputed;
	this->rangeType = LimitRangeEnum::range_discrete;
	this->priLow = 0;
	this->priMean = 0;
	this->priHigh = 0;
	this->pwLow = 0;
	this->pwHigh = 0;
	this->preSilenceTime = 0;
	this->postSilenceTime = 0;
	this->pulseCount = 0;
	this->frame = 0;
	this->extIntraPulseMode = 0;
	this->extScanType = 0;
	this->toaFirst = 0;
	this->toaLast = 0;
}

LimitPriClass::~LimitPriClass() {
	// TODO Auto-generated destructor stub
}




