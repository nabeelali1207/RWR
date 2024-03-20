/*
 * limitrfclass.cpp
 *
 *  Created on: Dec 30, 2018
 *      Author: test
 */

#define LIMITRFCLASS_CPP_

#include "../vo/limitrfclass.h"

//default constructor
LimitRfClass::LimitRfClass() {
	this->limitType = LimitTypeEnum::type_rf;
	this->rfType = RfTypeEnum::rf_uncomputed;
	this->rangeType = LimitRangeEnum::range_discrete;
	this->rfLow = 0;
	rfAvg=0;
	this->rfHigh = 0;
	rfStdDev=0;
	this->preSilenceTime = 0;
	this->postSilenceTime = 0;
	this->pulseCount = 0;
	this->frame = 0;
	this->extIntraPulseMode = 0;
	this->extScanMode = 0;
	this->toaFirst = 0;
	this->toaLast = 0;
}

LimitRfClass::~LimitRfClass() {
	// TODO Auto-generated destructor stub
}

