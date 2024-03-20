/*
 * trackpriclass.cpp
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#define TRACKPRICLASS_CPP_

#include "../vo/trackpriclass.h"

//default constructor
TrackPriClass::TrackPriClass():priTrackingObj(0)
{
	priTrackingPossible = 0;
	lastPriAnalysisSpotCount = 0;
	lastPriAnalysistime = 0;
	lastPriAnalysisPulseCount=0;
	toaBufferSizeFirst = 0;
	toaBufferSizeLast = 0;
	isSeqSearchObjectValid = false;
	toaBufferSizeFirst=0;
	priCircular.set_capacity(ConstLow::trackPriBuffCircSize);
	this->curPriSpot = 0;
	maxPriSpot = 0;
	this->curExpectPriSpot = 0;
	this->curPriType = PriTypeEnum::pri_uncomputed;

	this->countPdwAnalysisLast = 0;
	this->countPdwAnalysisWindow = ConstLow::asPriPdwCountMin;
	this->timeToaAnalysisLast = 0;
	this->diffToaAnalysisWindow = ConstLow::toaCountInSecond;

	this->priTypeCurrentDominant = PriTypeEnum::pri_uncomputed;
	this->priTypeFutureDominant = PriTypeEnum::pri_uncomputed;
	this->flagPriAnalysisLimitedFirst = 0;
	this->flagPriAnalysisLimitedLast = 0;
	accumulatePri.set_capacity(ConstHigh::trackPriBuffAccumulateCircSize);
	this->accumulatePriFixedCount = 0;
	this->accumulatePriDwellSwitchCount = 0;
	this->accumulatePriStaggerCount = 0;
	this->accumulatePriSlidingCount = 0;
	this->accumulatePriJitterCount = 0;
	this->accumulatePriComplexCount = 0;
	this->accumulatePriDiffToaCount = 0;
}

LimitPriClass TrackPriClass::getDwellSwitchLastSpot() {
	LimitPriClass emptyPriClassObj;
	int32_t index;
	uint32_t bufferSize;

	bufferSize =accumulatePri.size();
	for(index=bufferSize-1; index >= 0; index--)
	{
		if ((accumulatePri[index].priType== PriTypeEnum::pri_dwellswitch) ||
				(accumulatePri[index].priType== PriTypeEnum::pri_fixed))
		{
			emptyPriClassObj=accumulatePri[index];
			break;
		}
	}

	return emptyPriClassObj;
}

LimitPriClass TrackPriClass::getDwellSwitchFirstSpot() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if ((accumulatePri[index].priType== PriTypeEnum::pri_dwellswitch) ||
				(accumulatePri[index].priType== PriTypeEnum::pri_fixed))
		{
			emptyPriClassObj=accumulatePri[index];
			break;
		}
	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getDwellSwitchMinSpot() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if ((accumulatePri[index].priType== PriTypeEnum::pri_dwellswitch) ||
				(accumulatePri[index].priType== PriTypeEnum::pri_fixed))
		{
			if(firstSetFlag==false)
			{
				emptyPriClassObj=accumulatePri[index];
				firstSetFlag=true;
			}
			else
			{
				if (emptyPriClassObj.priLow > accumulatePri[index].priLow)
				{
					emptyPriClassObj=accumulatePri[index];
				}
			}

		}

	}

	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getDwellSwitchMaxSpot() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if ((accumulatePri[index].priType== PriTypeEnum::pri_dwellswitch) ||
				(accumulatePri[index].priType== PriTypeEnum::pri_fixed))
		{
			if(firstSetFlag==false)
			{
				emptyPriClassObj=accumulatePri[index];
				firstSetFlag=true;
			}
			else
			{
				if (emptyPriClassObj.priLow < accumulatePri[index].priLow)
				{
					emptyPriClassObj=accumulatePri[index];
				}
			}

		}

	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getStaggerPhaseLastSpot() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;

	bufferSize =accumulatePri.size();
	for(index=bufferSize-1; index >= 0; index--)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_stagger)
		{
			emptyPriClassObj=accumulatePri[index];
			break;
		}
	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getStaggerPhaseFirstSpot() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_stagger)
		{
			emptyPriClassObj=accumulatePri[index];
			break;
		}
	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getStaggerPhaseMinSpot() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_stagger)
		{
			if(firstSetFlag==false)
			{
				emptyPriClassObj=accumulatePri[index];
				firstSetFlag=true;
			}
			else
			{
				if (emptyPriClassObj.priLow > accumulatePri[index].priLow)
				{
					emptyPriClassObj=accumulatePri[index];
				}
			}

		}

	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getStaggerPhaseMaxSpot() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_stagger)
		{
			if(firstSetFlag==false)
			{
				emptyPriClassObj=accumulatePri[index];
				firstSetFlag=true;
			}
			else
			{
				if (emptyPriClassObj.priLow < accumulatePri[index].priLow)
				{
					emptyPriClassObj=accumulatePri[index];
				}
			}

		}

	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getStaggerFrameLastSpot() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;

	bufferSize =accumulatePri.size();
	for(index=bufferSize-1; index >= 0; index--)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_stagger)
		{
			emptyPriClassObj=accumulatePri[index];
			break;
		}
	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getStaggerFrameFirstSpot() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_stagger)
		{
			emptyPriClassObj=accumulatePri[index];
			break;
		}
	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getStaggerFrameMinSpot() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_stagger)
		{
			if(firstSetFlag==false)
			{
				emptyPriClassObj=accumulatePri[index];
				firstSetFlag=true;
			}
			else
			{
				if (emptyPriClassObj.frame > accumulatePri[index].frame)
				{
					emptyPriClassObj=accumulatePri[index];
				}
			}

		}

	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getStaggerFrameMaxSpot() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_stagger)
		{
			if(firstSetFlag==false)
			{
				emptyPriClassObj=accumulatePri[index];
				firstSetFlag=true;
			}
			else
			{
				if (emptyPriClassObj.frame < accumulatePri[index].frame)
				{
					emptyPriClassObj=accumulatePri[index];
				}
			}

		}

	}
	return emptyPriClassObj;
}

LimitPriClass TrackPriClass::getSlidingLastRange() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;

	bufferSize =accumulatePri.size();
	for(index=bufferSize-1; index >= 0; index--)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_sliding)
		{
			emptyPriClassObj=accumulatePri[index];
			break;
		}
	}

	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getSlidingFirstRange() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_sliding)
		{
			emptyPriClassObj=accumulatePri[index];
			break;
		}
	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getSlidingMinRange() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_sliding)
		{
			if(firstSetFlag==false)
			{
				emptyPriClassObj=accumulatePri[index];
				firstSetFlag=true;
			}
			else
			{
				if ((emptyPriClassObj.priHigh-emptyPriClassObj.priLow) >
						(accumulatePri[index].priHigh-accumulatePri[index].priLow))
				{
					emptyPriClassObj=accumulatePri[index];
				}
			}

		}

	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getSlidingMaxRange() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_sliding)
		{
			if(firstSetFlag==false)
			{
				emptyPriClassObj=accumulatePri[index];
				firstSetFlag=true;
			}
			else
			{
				if ((emptyPriClassObj.priHigh-emptyPriClassObj.priLow) <
						(accumulatePri[index].priHigh-accumulatePri[index].priLow))
				{
					emptyPriClassObj=accumulatePri[index];
				}
			}

		}

	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getJitterLastRange() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;

	bufferSize =accumulatePri.size();
	for(index=bufferSize-1; index >= 0; index--)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_jitter)
		{
			emptyPriClassObj=accumulatePri[index];
			break;
		}
	}

	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getJitterFirstRange() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_jitter)
		{
			emptyPriClassObj=accumulatePri[index];
			break;
		}
	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getJitterMinRange() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_jitter)
		{
			if(firstSetFlag==false)
			{
				emptyPriClassObj=accumulatePri[index];
				firstSetFlag=true;
			}
			else
			{
				if ((emptyPriClassObj.priHigh-emptyPriClassObj.priLow) >
						(accumulatePri[index].priHigh-accumulatePri[index].priLow))
				{
					emptyPriClassObj=accumulatePri[index];
				}
			}

		}

	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getJitterMaxRange() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
		uint32_t bufferSize;
		bool  firstSetFlag=false;

		bufferSize =accumulatePri.size();
		for(index=0; index < bufferSize; index++)
		{
			if (accumulatePri[index].priType== PriTypeEnum::pri_jitter)
			{
				if(firstSetFlag==false)
				{
					emptyPriClassObj=accumulatePri[index];
					firstSetFlag=true;
				}
				else
				{
					if ((emptyPriClassObj.priHigh-emptyPriClassObj.priLow) <
							(accumulatePri[index].priHigh-accumulatePri[index].priLow))
					{
						emptyPriClassObj=accumulatePri[index];
					}
				}

			}

		}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getComplexLastRange() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;

	bufferSize =accumulatePri.size();
	for(index=bufferSize-1; index >= 0; index--)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_complex)
		{
			emptyPriClassObj=accumulatePri[index];
			break;
		}
	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getComplexFirstRange() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_complex)
		{
			emptyPriClassObj=accumulatePri[index];
			break;
		}
	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getComplexMinRange() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_complex)
		{
			if(firstSetFlag==false)
			{
				emptyPriClassObj=accumulatePri[index];
				firstSetFlag=true;
			}
			else
			{
				if ((emptyPriClassObj.priHigh-emptyPriClassObj.priLow) >
						(accumulatePri[index].priHigh-accumulatePri[index].priLow))
				{
					emptyPriClassObj=accumulatePri[index];
				}
			}

		}

	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getComplexMaxRange() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_complex)
		{
			if(firstSetFlag==false)
			{
				emptyPriClassObj=accumulatePri[index];
				firstSetFlag=true;
			}
			else
			{
				if ((emptyPriClassObj.priHigh-emptyPriClassObj.priLow) <
						(accumulatePri[index].priHigh-accumulatePri[index].priLow))
				{
					emptyPriClassObj=accumulatePri[index];
				}
			}

		}

	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getDiffToaLastSpot() {
	LimitPriClass emptyPriClassObj;
	int32_t index;
	uint32_t bufferSize;

	bufferSize =accumulatePri.size();
	for(index=bufferSize-1; index >= 0; index--)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_difftoa)
		{
			emptyPriClassObj=accumulatePri[index];
			break;
		}
	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getDiffToaFirstSpot() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_difftoa)
		{
			emptyPriClassObj=accumulatePri[index];
			break;
		}
	}
	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getDiffToaMinSpot() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_difftoa)
		{
			if(firstSetFlag==false)
			{
				emptyPriClassObj=accumulatePri[index];
				firstSetFlag=true;
			}
			else
			{
				if (emptyPriClassObj.priLow > accumulatePri[index].priLow)
				{
					emptyPriClassObj=accumulatePri[index];
				}
			}

		}

	}

	return emptyPriClassObj;
}
LimitPriClass TrackPriClass::getDiffToaMaxSpot() {
	LimitPriClass emptyPriClassObj;
	uint32_t index;
	uint32_t bufferSize;
	bool  firstSetFlag=false;

	bufferSize =accumulatePri.size();
	for(index=0; index < bufferSize; index++)
	{
		if (accumulatePri[index].priType== PriTypeEnum::pri_difftoa)
		{
			if(firstSetFlag==false)
			{
				emptyPriClassObj=accumulatePri[index];
				firstSetFlag=true;
			}
			else
			{
				if (emptyPriClassObj.priLow < accumulatePri[index].priLow)
				{
					emptyPriClassObj=accumulatePri[index];
				}
			}

		}

	}
	return emptyPriClassObj;
}

void TrackPriClass::allocateSequenceSearchObject()
{
	priTrackingObj.resize(1);
	isSeqSearchObjectValid = true;
}
TrackPriClass::~TrackPriClass()
{
}
void TrackPriClass::serialize(SerialBufferClass & buffer)
{
	buffer.serialize(lastPriAnalysisSpotCount, 0);
	buffer.serialize(priCircular, 0);
	buffer.serialize(priFirst, 0);
	buffer.serialize(toaBufferSizeFirst, 0);

	buffer.serialize(curPriSpot, 0);
	buffer.serialize(curExpectPriSpot, 0);
	buffer.serialize(curPriType, 0);

	buffer.serialize(countPdwAnalysisLast, 0);
	buffer.serialize(countPdwAnalysisWindow, 0);
	buffer.serialize(timeToaAnalysisLast, 0);
	buffer.serialize(diffToaAnalysisWindow, 0);
}
