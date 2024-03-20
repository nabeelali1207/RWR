/*
 * PdwDefault.cpp
 *
 *  Created on: Oct 15, 2018
 *      Author: test
 */

#define PDWDEFAULTCLASS_CPP_

#include "../vo/pdwdefaultclass.h"

uint64_t PdwSmall32::getTOA()
{
	return (TOA&0x0000FFFFFFFFFFFF);
}
bool PdwSmall32::toaComparison(const PdwSmall32 &a,const PdwSmall32 &b)
{
	return (a.TOA&0x0000FFFFFFFFFFFF) < (b.TOA&0x0000FFFFFFFFFFFF);
}

bool PdwDefaultClass::cmpToaStart(const PdwDefaultClass & a,
		const PdwDefaultClass & b)
{
	return a.timeOfArrivalStart < b.timeOfArrivalStart;
}

//default constructor
PdwDefaultClass::PdwDefaultClass()
{
	powerThresholdFlag = false;
	pulseTrackedFlag = false;
	pulseSentForClusteringFlag = false;
	trackProcessFlag = false;
	clusterProcessFlag = false;
	trackProcessCount = 0;
	clusterProcessCount = 0;

	trackingPriScore = -1;
	trackingRfScore = -1;
	trackingPwScore = -1;
	trackingPowerScore = -1;
	dominantAntennaCode = 0;
	secondaryAntennaCode = 0;
	pyAntenna = 0;
	pxAntenna = 0;
	rfAmplitude = 0;

	this->band = BandCodeEnum::band_uncomputed;
	this->frequencyValidFlag = 0;
	this->popFlag = 0;
	this->quadrantCode = 0;
	this->antennaCode = 0;
	this->pulseWidth = 0;
	this->angleOfArrival = 0;
	this->powerA = 0;
	this->powerB = 0;
	this->powerC = 0;
	this->powerD = 0;
	this->powerX = 0;
	this->powerY = 0;
	this->powerAmplitude = 0;
	this->frequency = 0;
	this->timeOfArrivalStart = 0;
	this->timeOfArrivalEnd = 0;

	this->bandRaw = 0;
	this->frequencyValidFlagRaw = 0;
	this->popFlagRaw = 0;
	this->quadrantCodeRaw = 0;
	this->antennaCodeRaw = 0;
	this->pulseWidthRaw = 0;
	this->angleOfArrivalRaw = 0;
	this->powerARaw = 0;
	this->powerBRaw = 0;
	this->powerCRaw = 0;
	this->powerDRaw = 0;
	this->powerXRaw = 0;
	this->powerYRaw = 0;
	this->powerAmplitudeRaw = 0;
	this->frequencyRaw = 0;
	this->timeOfArrivalStartRaw = 0;
	this->timeOfArrivalEndRaw = 0;
}

//constructor with parameters
PdwDefaultClass::PdwDefaultClass(uint8_t bandRaw, uint8_t frequencyValidFlagRaw,
		uint8_t popFlagRaw, uint8_t quadrantCodeRaw, uint8_t antennaCodeRaw,
		uint16_t pulseWidthRaw, uint16_t angleOfArrivalRaw, uint16_t powerARaw,
		uint16_t powerBRaw, uint16_t powerCRaw, uint16_t powerDRaw,
		uint16_t powerXRaw, uint16_t powerYRaw, uint16_t powerAmplitudeRaw,
		uint16_t frequencyRaw, uint64_t timeOfArrivalStartRaw,
		uint64_t timeOfArrivalEndRaw)
{

	powerThresholdFlag = false;
	pulseTrackedFlag = false;
	pulseSentForClusteringFlag = false;
	trackProcessFlag = false;
	clusterProcessFlag = false;
	trackProcessCount = 0;
	clusterProcessCount = 0;

	trackingPriScore = -1;
	trackingRfScore = -1;
	trackingPwScore = -1;
	trackingPowerScore = -1;
	dominantAntennaCode = 0;
	secondaryAntennaCode = 0;
	pyAntenna = 0;
	pxAntenna = 0;
	rfAmplitude = 0;

	this->band = BandCodeEnum::band_uncomputed;
	this->frequencyValidFlag = 0;
	this->popFlag = 0;
	this->quadrantCode = 0;
	this->antennaCode = 0;
	this->pulseWidth = 0;
	this->angleOfArrival = 0;
	this->powerA = 0;
	this->powerB = 0;
	this->powerC = 0;
	this->powerD = 0;
	this->powerX = 0;
	this->powerY = 0;
	this->powerAmplitude = 0;
	this->frequency = 0;
	this->timeOfArrivalStart = 0;
	this->timeOfArrivalEnd = 0;

	this->bandRaw = bandRaw;
	this->frequencyValidFlagRaw = frequencyValidFlagRaw;
	this->popFlagRaw = popFlagRaw;
	this->quadrantCodeRaw = quadrantCodeRaw;
	this->antennaCodeRaw = antennaCodeRaw;
	this->pulseWidthRaw = pulseWidthRaw;
	this->angleOfArrivalRaw = angleOfArrivalRaw;
	this->powerARaw = powerARaw;
	this->powerBRaw = powerBRaw;
	this->powerCRaw = powerCRaw;
	this->powerDRaw = powerDRaw;
	this->powerXRaw = powerXRaw;
	this->powerYRaw = powerYRaw;
	this->powerAmplitudeRaw = powerAmplitudeRaw;
	this->frequencyRaw = frequencyRaw;
	this->timeOfArrivalStartRaw = timeOfArrivalStartRaw;
	this->timeOfArrivalEndRaw = timeOfArrivalEndRaw;
}

void PdwDefaultClass::ParseFromPdwSmall(PdwSmall &smallPdw)
{
	timeOfArrivalStartRaw = smallPdw.timeOfArrivalStartRaw;
	pulseWidthRaw = smallPdw.pulseWidth;
	frequencyRaw = smallPdw.frequency;
	powerARaw = smallPdw.powerA;
	powerBRaw = smallPdw.powerB;
	powerCRaw = smallPdw.powerC;
	powerDRaw = smallPdw.powerD;
	bandRaw = smallPdw.band;
	antennaCodeRaw = smallPdw.fmrAnt;
	frequencyValidFlagRaw = smallPdw.freqValid;
}

void PdwDefaultClass::ParseFromPdwSmall32(PdwSmall32 &smallPdw32)
{
	timeOfArrivalStartRaw = (smallPdw32.TOA & 0x0000FFFFFFFFFFFF);
	rfAmplitude = (smallPdw32.TOA >> 48);
	uint8_t attenuationVal = (smallPdw32.LPI & 0x4) >> 2;
	if (attenuationVal == 1)
	{
		attenuationVal = 3 & 1;
	}
	selfTestFlag = (smallPdw32.ST & 1);
	pulseWidthRaw = smallPdw32.PW;
	frequencyRaw = smallPdw32.RF;
	powerARaw = smallPdw32.Pa;
	powerBRaw = smallPdw32.Pb;
	powerCRaw = smallPdw32.Pc;
	powerDRaw = smallPdw32.Pd;
	bandRaw = smallPdw32.BC;
	antennaCodeRaw = smallPdw32.ANT;
	frequencyValidFlagRaw = smallPdw32.RF_V;

	if((smallPdw32.LPI_valid&1)==1)
	{
		uint8_t lpi=smallPdw32.LPI & 0x3;
		intraPulse = IntraPulseType::UMOP;
		if((lpi&0x1)==1)
		{
			intraPulse = IntraPulseType::PMOP;
//			cout << "LPI "<< (int)smallPdw32.LPI<<endl;
		}
		else if((lpi&0x2)==0x2)
		{
			intraPulse = IntraPulseType::FMOP;
//			cout << "LPI "<< (int)smallPdw32.LPI<<endl;
		}
	}
	else
	{
		intraPulse = IntraPulseType::UMOP;
	}
}
PdwDefaultClass::~PdwDefaultClass()
{
	// TODO Auto-generated destructor stub
}

