/*
 * limitpwclass.cpp
 *
 *  Created on: Dec 30, 2018
 *      Author: test
 */

#define LIMITPWCLASS_CPP_

#include "../vo/limitpwclass.h"

//default constructor
LimitPwClass::LimitPwClass() {
	this->limitType = LimitTypeEnum::type_pw;
	this->pwType = PwTypeEnum::pw_uncomputed;
	this->rangeType = LimitRangeEnum::range_discrete;
	this->pwLow = 0;
	pwAvg=0;
	this->pwHigh = 0;
	this->pwStdDev=0;
	this->preSilenceTime = 0;
	this->postSilenceTime = 0;
	this->pulseCount = 0;
	this->frame = 0;
	this->extIntraPulseMode = 0;
	this->extScanType = 0;
	this->toaFirst = 0;
	this->toaLast = 0;
}

LimitPwClass::~LimitPwClass() {
	// TODO Auto-generated destructor stub
}

