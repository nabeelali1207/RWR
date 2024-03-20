/*
 * analysislpidelegate.cpp
 *
 *  Created on: Aug 24, 2022
 *      Author: root
 */

#include "analysislpidelegate.h"


void lpiUpdateTrackLpiOnPulseAssociation(TrackDefaultClass &track,
		PdwDefaultClass &newPdw)
{
	track.intrapulse.pulseCountSinceLastLpiAnalysis++;
	IntraPulseType lpiType;
//	if(newPdw.intraPulse != IntraPulseType::UMOP)
//	{
//
//		cout << "LPI------------------- "<< (int)newPdw.intraPulse<<endl;
//	}
	if (track.intrapulse.pulseCountSinceLastLpiAnalysis
			>= 50)
	{
		lpiType = lpiAnalysisExecute(track);
		track.intrapulse.pulseCountSinceLastLpiAnalysis = 0;
		if(lpiType == IntraPulseType::FMOP)
		{
			track.intrapulse.fmopPresent = true;
			track.intrapulse.pmopPresent = false;
			track.intrapulse.umopPresent = false;
		}
		else if(lpiType == IntraPulseType::PMOP)
		{

			track.intrapulse.pmopPresent = true;
			track.intrapulse.fmopPresent = false;
			track.intrapulse.umopPresent = false;
		}
		else
		{
			track.intrapulse.umopPresent = true;
			track.intrapulse.fmopPresent = false;
			track.intrapulse.pmopPresent = false;
		}
		track.intrapulse.trackLpiType = lpiType;
	}


}
IntraPulseType lpiAnalysisExecute(TrackDefaultClass &track)
{

//	vector<PdwDefaultClass> pdwInput = lpiGetPdwsForAnalysis(track);

	int32_t inputSize = track.intrapulse.pulseCountSinceLastLpiAnalysis;
	int32_t bufferSize = track.general.dspPdwBuffer.size();


	int32_t startPoint = bufferSize - inputSize;
	int32_t endPoint = bufferSize;
	startPoint = max(0,startPoint);

	uint16_t fMop,pMop,uMop;
	fMop = pMop = uMop = 0;
	IntraPulseType lpiType;
	lpiType = IntraPulseType::UMOP;
	if (inputSize < 3) //just a safety check, not bound to happen
		return lpiType;

	for (uint16_t i = startPoint; i < endPoint ; i++)
	{
		if(track.general.dspPdwBuffer[i].intraPulse == IntraPulseType::FMOP)
			fMop++;
		else if(track.general.dspPdwBuffer[i].intraPulse == IntraPulseType::PMOP)
			pMop++;
//		else
//			uMop++;
	}

//	if(fMop >0 || pMop >0)
//	{
//		cout << "LPI FLAG \n"<<endl;
//	}

	if(fMop > pMop && fMop > uMop && fMop > 2)
		lpiType = IntraPulseType::FMOP;
	else if(pMop > fMop && pMop > uMop && pMop > 2)
		lpiType = IntraPulseType::PMOP;
	else
		lpiType = IntraPulseType::UMOP;
	return lpiType;
}
vector<PdwDefaultClass> lpiGetPdwsForAnalysis(TrackDefaultClass &track)
{
	int32_t inputSize = track.intrapulse.pulseCountSinceLastLpiAnalysis;
	int32_t bufferSize = track.general.dspPdwBuffer.size();

	vector<PdwDefaultClass> pdwInput(inputSize);

	int32_t startPoint = bufferSize - inputSize;
	int32_t endPoint = bufferSize;

	for (uint16_t i = startPoint, j = 0; i < endPoint; i++, j++)
	{
		pdwInput[j] = track.general.dspPdwBuffer[i];
	}
	return pdwInput;
}
