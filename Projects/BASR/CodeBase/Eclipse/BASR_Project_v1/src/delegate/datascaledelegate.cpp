/*
 * datascaledelegate.cpp
 *
 *  Created on: Oct 28, 2018
 *      Author: test
 */

#define DATASCALEDELEGATE_CPP_

#include "../delegate/datascaledelegate.h"

void scaleSetSystemConstants()
{

	if (Constant::systemVersionOperate == 1)
	{ //original pup
		ConstLow::toaCountInMs =
				((1000 / Constant::systemOneToaClockNs) * 1000);
		ConstLow::toaCountInSecond = ((1000 / Constant::systemOneToaClockNs)
				* 1000) * 1000;
		ConstLow::toaCountInMinute = ((1000 / Constant::systemOneToaClockNs)
				* 1000) * 1000 * 60;
		ConstLow::toaCountInFiveMin = ((1000 / Constant::systemOneToaClockNs)
				* 1000) * 1000 * 60 * 5;
		ConstLow::toaCountInTenMin = ((1000 / Constant::systemOneToaClockNs)
				* 1000) * 1000 * 60 * 10;

		//more algo related constants here
	}
	else if (Constant::systemVersionOperate == 2)
	{
		ConstLow::toaCountInMs =
				((1000 / Constant::systemTwoToaClockNs) * 1000);
		ConstLow::toaCountInSecond = ((1000 / Constant::systemTwoToaClockNs)
				* 1000) * 1000;
		ConstLow::toaCountInMinute = ((1000 / Constant::systemTwoToaClockNs)
				* 1000) * 1000 * 60;
		ConstLow::toaCountInFiveMin = ((1000 / Constant::systemTwoToaClockNs)
				* 1000) * 1000 * 60 * 5;
		ConstLow::toaCountInTenMin = ((1000 / Constant::systemTwoToaClockNs)
				* 1000) * 1000 * 60 * 10;

		//more algo related constants here
	}
}

void scalePreprocessPdwThreadMainData(list<PdwDefaultClass> &currentPdws)
{

	std::list<PdwDefaultClass>::iterator it;
	std::list<PdwDefaultClass> *pdwList = &currentPdws;

	it = pdwList->begin();
	while (it != pdwList->end())
	{

		it++;
	}
}
bool scaleApplyPowerThreshold(PdwDefaultClass &currentPdw,
		const uint16_t &minSinglePower, const uint16_t &minCombinedPower)
{
	uint16_t maxPower = max(currentPdw.powerX, currentPdw.powerY);
	uint16_t combinedPower = currentPdw.powerX + currentPdw.powerY;

	if (maxPower >= minSinglePower && combinedPower >= minCombinedPower)
	{
		return true;
	}
	return false;
}
void scaleSinglePdwLiveData(PdwDefaultClass &currentPdw)
{
	uint16_t adcScale = 1;
	scaleSinglePdw(currentPdw, adcScale, 1, 1);
}

void scaleSinglePdw(PdwDefaultClass &pdw, uint16_t adcScale, uint16_t toaScale,
		uint16_t pwScale)
{
	pdw.frequencyValidFlag = pdw.frequencyValidFlagRaw;
	pdw.popFlag = pdw.popFlagRaw;

	pdw.antennaCode = pdw.antennaCodeRaw;
	pdw.pulseWidth = pdw.pulseWidthRaw;
	pdw.angleOfArrival = pdw.angleOfArrivalRaw;

	pdw.powerA = pdw.powerARaw;
	pdw.powerB = pdw.powerBRaw;
	pdw.powerC = pdw.powerCRaw;
	pdw.powerD = pdw.powerDRaw;

	pdw.powerXRaw = max(pdw.powerARaw, pdw.powerCRaw);
	pdw.powerYRaw = max(pdw.powerBRaw, pdw.powerDRaw);

	if (pdw.powerA > pdw.powerC)
	{
		pdw.powerX = pdw.powerA;
		pdw.pxAntenna = 0;
	}
	else
	{
		pdw.powerX = pdw.powerC;
		pdw.pxAntenna = 2;
	}
	if (pdw.powerB > pdw.powerD)
	{
		pdw.powerY = pdw.powerB;
		pdw.pyAntenna = 1;
	}
	else
	{
		pdw.powerY = pdw.powerD;
		pdw.pyAntenna = 3;
	}

	pdw.powerAmplitude = pdw.powerAmplitudeRaw;
	pdw.frequency = pdw.frequencyRaw;
	pdw.timeOfArrivalStart = pdw.timeOfArrivalStartRaw;
	pdw.timeOfArrivalEnd = pdw.timeOfArrivalEndRaw;

	switch (pdw.bandRaw)
	{
	case 0:
		pdw.band = BandCodeEnum::band_e;
		break;
	case 1:
		pdw.band = BandCodeEnum::band_g;
		break;
	case 2:
		pdw.band = BandCodeEnum::band_i;
		break;
	case 3:
		pdw.band = BandCodeEnum::band_j;
		break;
	case 4:
		pdw.band = BandCodeEnum::band_cd;
		break;
	case 5:
		pdw.band = BandCodeEnum::band_k;
		break;
	default:
		pdw.band = BandCodeEnum::band_uncomputed;
	}

	if (pdw.frequencyValidFlag == 0)
	{
		pdw.frequency = 0;
	}
	else
	{
		pdw.frequency = /*(uint32_t) (2000 + (*/float(pdw.frequency); /** 0.244));*/
	}
// DEBUGGING ONLY
//	pdw.frequency = 0;
//	pdw.frequencyValidFlag = 0;

	if (pdw.powerARaw == pdw.powerXRaw)
	{
		if (pdw.powerBRaw == pdw.powerYRaw)
		{
			pdw.quadrantCode = 0;
		}
		else
		{
			pdw.quadrantCode = 2;
		}
	}
	else
	{
		if (pdw.powerBRaw == pdw.powerYRaw)
		{
			pdw.quadrantCode = 1;
		}
		else
		{
			pdw.quadrantCode = 3;
		}

	}

	pdw.pulseTrackedFlag = false;
	pdw.pulseSentForClusteringFlag = false;
	pdw.trackProcessFlag = false;
	pdw.clusterProcessFlag = false;
	pdw.trackProcessCount = 0;
	pdw.clusterProcessCount = 0;

	pdw.powerA = pdw.powerA * adcScale;
	pdw.powerB = pdw.powerB * adcScale;
	pdw.powerC = pdw.powerC * adcScale;
	pdw.powerD = pdw.powerD * adcScale;
	pdw.powerAmplitude = pdw.powerAmplitude * adcScale;
	pdw.powerX = pdw.powerX * adcScale;
	pdw.powerY = pdw.powerY * adcScale;
	pdw.pulseWidth = pdw.pulseWidth * pwScale;
	pdw.timeOfArrivalStart = pdw.timeOfArrivalStart * toaScale;

	pdw.angleOfArrival = doaCalculateAngle(pdw.powerX, pdw.powerY,
			pdw.quadrantCode, pdw.bandRaw, Constant::systemBandCountOperate);

//	pdw.angleOfArrival = doaCalculateAngle_Low(pdw.powerA, pdw.powerB,
//			pdw.powerC, pdw.powerD, pdw.quadrantCode, pdw.bandRaw,
//			Constant::systemBandCountOperate);

	//pdw.angleOfArrivalRaw = pdw.angleOfArrivalRaw * 360 / 255;
//	if (abs((int32_t) pdw.angleOfArrival - (int32_t) pdw.angleOfArrivalRaw)
//			> 45)
//	{
//		pdw.angleOfArrivalRaw = pdw.angleOfArrivalRaw + 1;
//	}
	typedef std::pair<uint16_t, uint8_t> myPair;
	vector<myPair> powers(4);

	powers[0].first = pdw.powerA;
	powers[0].second = 0;
	powers[1].first = pdw.powerB;
	powers[1].second = 1;
	powers[2].first = pdw.powerC;
	powers[2].second = 2;
	powers[3].first = pdw.powerD;
	powers[3].second = 3;
	sort(powers.begin(), powers.end(), [](const myPair &l, const myPair &r)
	{	return l.first>r.first;});
	if(pdw.timeOfArrivalStart == 45251322530)
		pdw.timeOfArrivalStart = 45251322530;
	int8_t oppositeAxisAntenna[4] = {1,2,3,0};
	pdw.dominantAntennaCode = powers[0].second;
	if(powers[1].first == 0)
	{
		pdw.secondaryAntennaCode = oppositeAxisAntenna[pdw.dominantAntennaCode];
	}
	else
		pdw.secondaryAntennaCode = powers[1].second;
	pdw.antennaCode= pdw.dominantAntennaCode;

}

void scalePdwSimulationData(queue<PdwDefaultClass> &inputPdwList)
{
	queue<PdwDefaultClass> newList;
	uint16_t adcScale = 1, toaScale = 1;
	while (inputPdwList.size()>0)
	{
		PdwDefaultClass pdw = inputPdwList.front();
		scaleSinglePdw(pdw, adcScale, toaScale, 1);
		newList.push(pdw);
		inputPdwList.pop();
	}
	inputPdwList = std::move(newList);
//	uint16_t adcScale = 0;
//	std::list<PdwDefaultClass>::iterator it;
//	std::list<PdwDefaultClass> *pdwList = &inputPdwList;
//
//	if (ConstLow::simIterationTypeFlag
//			== LowIterationTypeFlags::simulationModeDisabled)
//	{
//		return;
//	}
//
//	//first copy all data from raw variables to actual
//	it = pdwList->begin();
//	while (it != pdwList->end())
//	{
//		//it->band = it->bandRaw;
//		it->frequencyValidFlag = it->frequencyValidFlagRaw;
//		it->popFlag = it->popFlagRaw;
//
////		it->antennaCode = it->antennaCodeRaw;
//		it->pulseWidth = it->pulseWidthRaw;
//		it->angleOfArrival = it->angleOfArrivalRaw;
//		it->powerA = it->powerARaw;
//		it->powerB = it->powerBRaw;
//		it->powerC = it->powerCRaw;
//		it->powerD = it->powerDRaw;
//		it->powerAmplitude = it->powerAmplitudeRaw;
//		it->frequency = it->frequencyRaw;
//		it->timeOfArrivalStart = it->timeOfArrivalStartRaw;
//		it->timeOfArrivalEnd = it->timeOfArrivalEndRaw;
//
//		it->powerXRaw = max(it->powerARaw, it->powerCRaw);
//		it->powerYRaw = max(it->powerBRaw, it->powerDRaw);
//		it->powerX = it->powerXRaw;
//		it->powerY = it->powerYRaw;
//		if (it->timeOfArrivalStart * 5 == 1289860)
//		{
//			cout << "hi" << endl;
//		}
//		if (it->powerARaw == it->powerXRaw)
//		{
//			if (it->powerBRaw == it->powerYRaw)
//			{
//				it->quadrantCode = 0;
//			}
//			else
//			{
//				it->quadrantCode = 2;
//			}
//		}
//		else
//		{
//			if (it->powerBRaw == it->powerYRaw)
//			{
//				it->quadrantCode = 1;
//			}
//			else
//			{
//				it->quadrantCode = 3;
//			}
//		}
//
//		//antenna code
//		if (abs((int16_t) it->powerARaw - (int16_t) it->powerCRaw) < 10)
//		{
//			if (it->powerBRaw > it->powerDRaw)
//			{
//				it->antennaCode = 1;
//			}
//			else
//			{
//				it->antennaCode = 3;
//			}
//		}
//		else if (abs((int16_t) it->powerBRaw - (int16_t) it->powerDRaw) < 10)
//		{
//			if (it->powerARaw > it->powerCRaw)
//			{
//				it->antennaCode = 0;
//			}
//			else
//			{
//				it->antennaCode = 2;
//			}
//		}
//		else if (it->powerARaw == it->powerXRaw)
//		{
//
//			if (it->powerXRaw > it->powerYRaw)
//			{
//				it->antennaCode = 0;
//			}
//			else
//			{
//				if (it->powerBRaw == it->powerYRaw)
//				{
//					it->antennaCode = 1;
//				}
//				else
//				{
//					it->antennaCode = 3;
//				}
//			}
//		}
//		else
//		{
//			if (it->powerXRaw > it->powerYRaw)
//			{
//				it->antennaCode = 2;
//			}
//			else
//			{
//				if (it->powerBRaw == it->powerYRaw)
//				{
//					it->antennaCode = 1;
//				}
//				else
//				{
//					it->antennaCode = 3;
//				}
//			}
//		}
//
//		it->angleOfArrival = doaCalculateAngle(it->powerXRaw, it->powerYRaw,
//				it->quadrantCode, it->bandRaw, 4);
//
//		it++;
//	}
//
//	//if system data input is version 1 and we want to operate in version 2
//	if ((Constant::systemVersionInput == 1)
//			&& (Constant::systemVersionOperate == 2))
//	{
//
//		adcScale = Constant::systemTwoAdcPowerMax
//				/ Constant::systemOneAdcPowerMax;
//		it = pdwList->begin();
//		while (it != pdwList->end())
//		{
//
//			if (it->frequencyValidFlag == 0)
//			{
//				it->frequency = 0;
//			}
//			else
//			{
//				it->frequency = (uint16_t) (2000
//						+ (float(it->frequency) * 0.244));
//			}
//			if (Constant::systemBandCountOperate == 2)
//			{
//
//				if (it->bandRaw == 0)
//				{
//					it->band = BandCodeEnum::band_eg;
//				}
//				else if (it->bandRaw == 1)
//				{
//					it->band = BandCodeEnum::band_ij;
//				}
//				else
//				{
//					it->band = BandCodeEnum::band_uncomputed;
//				}
//			}
//			else if (Constant::systemBandCountOperate == 4)
//			{
//				switch (it->bandRaw)
//				{
//				case 0:
//					it->band = BandCodeEnum::band_e;
//					break;
//				case 1:
//					it->band = BandCodeEnum::band_g;
//					break;
//				case 2:
//					it->band = BandCodeEnum::band_i;
//					break;
//				case 3:
//					it->band = BandCodeEnum::band_j;
//					break;
//				default:
//					it->band = BandCodeEnum::band_uncomputed;
//				}
//			}
//			it->powerA = it->powerA * adcScale;
//			it->powerB = it->powerB * adcScale;
//			it->powerC = it->powerC * adcScale;
//			it->powerD = it->powerD * adcScale;
//			it->powerAmplitude = it->powerAmplitude * adcScale;
//			it->powerX = it->powerX * adcScale;
//			it->powerY = it->powerY * adcScale;
//			it->pulseWidth = (uint16_t) (((float) it->pulseWidth) * 2.5);
//			it->timeOfArrivalStart = it->timeOfArrivalStart * 5;
//			it->timeOfArrivalEnd = it->timeOfArrivalEnd * 5;
//
//			it++;
//		}
//	}
}

