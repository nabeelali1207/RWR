/*
 * trackrfclass.cpp
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#define TRACKRFCLASS_CPP_

#include "../vo/trackrfclass.h"

uint8_t TrackRfClass::getBandEnumeration() {

	uint8_t bandEnumeration = 0;

	if(this->bandCode == BandCodeEnum::band_e) {
		bandEnumeration = 0;
	}
	if(this->bandCode == BandCodeEnum::band_g) {
			bandEnumeration = 1;
	}
	if(this->bandCode == BandCodeEnum::band_i) {
			bandEnumeration = 2;
	}
	if(this->bandCode == BandCodeEnum::band_j) {
			bandEnumeration = 3;
	}
	if(this->bandCode == BandCodeEnum::band_cd) {
			bandEnumeration = 4;
	}
	if(this->bandCode == BandCodeEnum::band_k) {
			bandEnumeration = 5;
	}
	return bandEnumeration;
}

//default constructor
TrackRfClass::TrackRfClass()
{
	lastAnalysisSpotCount=0;
	rfTrackingPossible=0;
	rfCircular.set_capacity(ConstLow::trackRfBuffCircSize);
	this->bandCode = BandCodeEnum::band_uncomputed; //perhaps change this to enum
	this->curRfType = RfTypeEnum::rf_uncomputed;
	this->timeToaRfAnl = 0;
	accumulateRf.set_capacity(ConstHigh::trackRfBuffAccumulateCircSize);
	this->accumulateRfCount = 0;
	this->accumulateFixedRfCount = 0;
	this->accumulateAgileRfCount = 0;
	this->accumulateSwitchRfCount = 0;
	this->accumulateJumpRfCount = 0;
	this->accumulateComplexRfCount = 0;
	this->rfValue = 0;
}

LimitRfClass TrackRfClass::getRfMinSpot() {
	LimitRfClass defaultRf;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulateRf.size();
	for(index=0; index < bufferSize; index++)
	{
		if(firstSetFlag==false)
		{
			defaultRf=accumulateRf[index];
			firstSetFlag=true;
		}
		else
		{
			if (defaultRf.rfLow < accumulateRf[index].rfLow)
			{
				defaultRf=accumulateRf[index];
			}
		}

	}
	return defaultRf;
}
LimitRfClass TrackRfClass::getRfMaxSpot() {
	LimitRfClass defaultRf;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulateRf.size();
	for(index=0; index < bufferSize; index++)
	{
		if(firstSetFlag==false)
		{
			defaultRf=accumulateRf[index];
			firstSetFlag=true;
		}
		else
		{
			if (defaultRf.rfLow < accumulateRf[index].rfLow)
			{
				defaultRf=accumulateRf[index];
			}
		}

	}
	return defaultRf;
}
LimitRfClass TrackRfClass::getRfMinSpotLow() {
	LimitRfClass defaultRf;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =rfCircular.size();
	for(index=0; index < bufferSize; index++)
	{
		if(firstSetFlag==false)
		{
			defaultRf=rfCircular[index];
			firstSetFlag=true;
		}
		else
		{
			if (defaultRf.rfLow < rfCircular[index].rfLow)
			{
				defaultRf=rfCircular[index];
			}
		}

	}
	return defaultRf;
}
LimitRfClass TrackRfClass::getRfMaxSpotLow() {
	LimitRfClass defaultRf;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =rfCircular.size();
	for(index=0; index < bufferSize; index++)
	{
		if(firstSetFlag==false)
		{
			defaultRf=rfCircular[index];
			firstSetFlag=true;
		}
		else
		{
			if (defaultRf.rfLow < rfCircular[index].rfLow)
			{
				defaultRf=rfCircular[index];
			}
		}

	}
	return defaultRf;
}

uint8_t TrackRfClass::getBandCodeLegacy() {
	uint8_t bCode = 0;

	if(this->bandCode == BandCodeEnum::band_e) {
		bCode = 0;
	}
	if(this->bandCode == BandCodeEnum::band_g) {
		bCode = 1;
	}
	if(this->bandCode == BandCodeEnum::band_i) {
		bCode = 2;
	}
	if(this->bandCode == BandCodeEnum::band_j) {
		bCode = 3;
	}
	if(this->bandCode == BandCodeEnum::band_cd) {
		bCode = 4;
	}
	if(this->bandCode == BandCodeEnum::band_k) {
		bCode = 5;
	}
	return bCode;
}

void TrackRfClass::AllocateRfTrackingObject()
{
	rfTrackingObj.resize(1);
}
TrackRfClass::~TrackRfClass()
{
	// TODO Auto-generated destructor stub
}
void TrackRfClass::serialize(SerialBufferClass & buffer)
{
	buffer.serialize(rfCircular,0);
	buffer.serialize(rfAmplitude,0);
	buffer.serialize(lastAnalysisSpotCount,0);
	buffer.serialize(rfFirst,0);
	buffer.serialize(bandCode,0);
	buffer.serialize(curRfType,0);
}
