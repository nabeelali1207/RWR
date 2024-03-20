/*
 * trackpwclass.cpp
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#define TRACKPWCLASS_CPP_

#include "../vo/trackpwclass.h"

//default constructor
TrackPwClass::TrackPwClass()
{
	pwCircular.set_capacity(ConstLow::trackPwBuffCircSize);
	this->curPwType = PwTypeEnum::pw_uncomputed;
	this->timeToaPwAnl = 0;
	lastAnalysisSpotCount=0;
	pwAnalysisDoneOnPriBuffer =0;
	accumulatePw.set_capacity(ConstHigh::trackPwBuffAccumulateCircSize);
	this->accumulatePwCount = 0;
}
LimitPwClass TrackPwClass::getPwMinSpot() {
	LimitPwClass defaultPw;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulatePw.size();
	for(index=0; index < bufferSize; index++)
	{
		if(firstSetFlag==false)
		{
			defaultPw=accumulatePw[index];
			firstSetFlag=true;
		}
		else
		{
			if (defaultPw.pwLow < accumulatePw[index].pwLow)
			{
				defaultPw=accumulatePw[index];
			}
		}

	}
	return defaultPw;
}
LimitPwClass TrackPwClass::getPwMaxSpot() {
	LimitPwClass defaultPw;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulatePw.size();
	for(index=0; index < bufferSize; index++)
	{
		if(firstSetFlag==false)
		{
			defaultPw=accumulatePw[index];
			firstSetFlag=true;
		}
		else
		{
			if (defaultPw.pwLow > accumulatePw[index].pwLow)
			{
				defaultPw=accumulatePw[index];
			}
		}

	}
	return defaultPw;
}

void TrackPwClass::AllocatePwTrackingObject()
{
	pwTrackingObj.resize(1);
}
TrackPwClass::~TrackPwClass()
{
	// TODO Auto-generated destructor stub
}

void TrackPwClass::serialize(SerialBufferClass & buffer)
{
	buffer.serialize(pwCircular, 0);
	buffer.serialize(lastAnalysisSpotCount, 0);
	buffer.serialize(pwFirst, 0);
	buffer.serialize(curPwType, 0);
	buffer.serialize(timeToaPwAnl, 0);
}
