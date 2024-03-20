/*
 * analysistrackpwlowdelegate.cpp
 *
 *  Created on: Apr 27, 2020
 *      Author: gmd
 */

#include "analysistrackpwlowdelegate.h"

AnalysisTrackPwLowClass::AnalysisTrackPwLowClass() :
		pwHistogram((1 << 17), 0, 8,3)
{ // constructor
// Initialization
	lastAnalysisPulseCount = 0;
	newResultsAvailableForCopyFlag = 0;
	lastAnalysisWasSuccessfulFlag = 0;
	currPwType = SequenceTypesEnum::seq_unknown;
	pwTypeHistory.set_capacity(8);
	pwTypeHistory.push_back(currPwType);

	pwBuffer.set_capacity(128);
	samplesProcessed = 0;
	pwTrackingPossibleFlag = 0;

	pwHistogramPeaks.reserve(8);
	pwHistogramPeaksSorted.reserve(8);
	pwHistogramPeaksSortedFiltered.reserve(8);

	histogramingEnabledFlag = 1;
	histogramedPulseCount = 0;
	countSinceLastPwAnalysisCall = 0;

	pdwThresholdForPwAnalysis = 8;
	trackingFailedCount = 0;

	currActiveSpotIndex = -1;

}
void AnalysisTrackPwLowClass::ResetPWAnalysisInfo()
{
	lastAnalysisPulseCount = 0;
	newResultsAvailableForCopyFlag = 0;
	lastAnalysisWasSuccessfulFlag = 0;
	currPwType = SequenceTypesEnum::seq_unknown;
	samplesProcessed = 0;
	pwTrackingPossibleFlag = 0;
	histogramedPulseCount = 0;
	countSinceLastPwAnalysisCall = 0;
	DeactivateHistObjects();
	histogramingEnabledFlag = 1;
}

void AnalysisTrackPwLowClass::AddNextPwValue(uint32_t currPw)
{
	samplesProcessed++;

	// PW values inserted into histogram pyramids for peak finding if histograms have any space left
	PushDataToHistObjects(currPw);
	bool pwAnalysisCalled = false;

	pwBuffer.push_back(currPw);	// pw buffer with original values

	int16_t pwTrackingFlag = PwTracking(currPw);
	if (pwTrackingFlag == 1)
	{ // PW of incoming pulse successfully tracked
		trackingFailedCount = 0;
		pwTrackingPossibleFlag = 1;
	}
	else
	{
		trackingFailedCount++;
	}

	if (trackingFailedCount >= pdwThresholdForPwAnalysis
			&& countSinceLastPwAnalysisCall >= 16)
	{ // New or updated pw
		pwAnalysisCalled = true;	// Call pw analysis
		TriggerPwAnalysis();

		if (lastAnalysisWasSuccessfulFlag == 0)
		{
			pdwThresholdForPwAnalysis += min((uint32_t) 32,
					pdwThresholdForPwAnalysis);
			if (pdwThresholdForPwAnalysis > 128)	// Max pw buffer size
			{
				pdwThresholdForPwAnalysis = ConstLow::asPwPdwCountMin;
			}
		}
	}
	else if (trackingFailedCount == 8)
	{ // try to re-lock pw tracking if not changed
		pwTrackingPossibleFlag = 0;
		RelockPwTracking(trackingFailedCount);
	}
	else if (trackingFailedCount == 4)
	{ // try to re-lock pw tracking if not changed
		RelockPwTracking(trackingFailedCount);
	}
	else if (trackingFailedCount == 3)
	{
		pwTrackingPossibleFlag = 0;
	}

	if (pwAnalysisCalled == true)
	{
		newResultsAvailableForCopyFlag = 1;
		countSinceLastPwAnalysisCall = 0;
		if (lastAnalysisWasSuccessfulFlag == 1)
			trackingFailedCount = 0;
	}
	else
	{
		countSinceLastPwAnalysisCall++;
	}

}
int8_t AnalysisTrackPwLowClass::TriggerPwAnalysis()
{

	bool pwAnalysisWasSuccessful = false;
	if (histogramingEnabledFlag == 0)
	{ // insert last N pulses into histograms and find peaks and do seq analysis
		ActivateHistObjects();
		uint16_t interestedPulseCount = min(countSinceLastPwAnalysisCall,
				trackingFailedCount);
		interestedPulseCount = min((uint16_t) 128, interestedPulseCount);
		PushLastNPulsesToHistograms(interestedPulseCount);
	}
	/* PW Analysis */
	SequenceTypesEnum newPwType = PwAnalysisNew();

	lastAnalysisPulseCount = histogramedPulseCount;

	if (newPwType == SequenceTypesEnum::seq_unknown)
	{ // PW analysis failed, accumulate more pulses
		pwAnalysisWasSuccessful = false;

	}
	else
	{ // PW analysis successful
		pwAnalysisWasSuccessful = true;
		pdwThresholdForPwAnalysis = ConstLow::asPwPdwCountMin;
		UpdateCurrPwType(newPwType);
		RelockPwTracking(histogramedPulseCount);
		DeactivateHistObjects();		// clear histograms
	}

	if (pwAnalysisWasSuccessful == true)
	{
		lastAnalysisWasSuccessfulFlag = 1;
		newResultsAvailableForCopyFlag = 1;
		countSinceLastPwAnalysisCall = 0;
		if (lastAnalysisWasSuccessfulFlag == 1)
			trackingFailedCount = 0;
	}
	else
	{
		lastAnalysisWasSuccessfulFlag = 0;
	}
	return (pwAnalysisWasSuccessful ? 1 : 0);
}
int8_t AnalysisTrackPwLowClass::GetPwScore(uint16_t queryPw)
{ // compare sample PW with the PW of track and calculate its score
	int32_t diff = queryPw;
	int8_t score = 0;
	int32_t lastPW = pwBuffer.back();
	uint16_t tempDev;
	int16_t currSpotIndex;
	switch (currPwType)
	{
	case SequenceTypesEnum::seq_dwell:
	{
		currSpotIndex = currActiveSpotIndex;
		if (currSpotIndex != -1)
		{
			diff = diff - pwSpots[currActiveSpotIndex];
			if (abs(diff) < (int32_t) deviationAllowed[currActiveSpotIndex])
			{
				score = 100; // pw matched with track's PW spot
			}
//			else // 1 missing pulse
//			{
//				currSpotIndex = expectedNextSpotIndex[currSpotIndex];
//				diff = diff - pwSpots[currSpotIndex];
//				if (currSpotIndex != -1)
//				{
//					if (abs(diff) < (int32_t) deviationAllowed[currSpotIndex])
//					{
//						score = 100; // pw matched with track's PW spot
//					}
//				}
//			}

//			diff = queryPw - lastPW;
//			if (abs(diff) < (int32_t) deviationAllowed[currActiveSpotIndex])
//			{
//				score += 50; // pw matched with track's last PW value
//			}
		}
		break;
	}

//	case SequenceTypesEnum::seq_dwell:
//	{
//		if (currActiveSpotIndex != -1)
//		{
//			{
//				for (uint16_t i = 0; i < pwSpots.size(); i++)
//				{
//					diff = queryPw - pwSpots[i];
//					if (abs(diff) < (int32_t) deviationAllowed[i])
//					{
//						return 100; // pw matched
//					}
//				}
//			}
//		}
//		break;
//	}
	case SequenceTypesEnum::seq_jitter:
	{
//		return 0;	// pw tracking not enabled for jitter type
		float tempDevAllowed = lastPW * 0.2; 	// +-20% deviation allowed

		tempDev = round(tempDevAllowed);
		tempDev = min((uint16_t) 20, tempDev);
		tempDev = max((uint16_t) 10, tempDev);
		diff = queryPw - lastPW;
		if (abs(diff) < tempDev)
		{
			score = 100; // pw matched with track's last PW value
		}
		return score;
		break;
	}
	default:
		break;

	}
	if (score == 0)
	{
		if (pwTrackingPossibleFlag == 0)
		{
			float tempDevAllowed = lastPW * 0.2; 	// +-20% deviation allowed

			tempDev = round(tempDevAllowed);
			tempDev = min((uint16_t) 20, tempDev);
			tempDev = max((uint16_t) 10, tempDev);
			diff = queryPw - lastPW;
			if (abs(diff) < tempDev)
			{
				score = 100; // pw matched with track's last PW value
			}
			return score;
		}
	}
	return score;
}
int8_t AnalysisTrackPwLowClass::PwTracking(uint32_t &currPw)
{ // PW tracking and update
	int32_t diff = currPw;
	int8_t res = -1;
	switch (currPwType)
	{
	case SequenceTypesEnum::seq_dwell:
	{
		if (currActiveSpotIndex != -1)
		{
			diff = currPw - pwSpots[currActiveSpotIndex];
			if (abs(diff) < 10)
			{
				res = 1;
			}
			currActiveSpotIndex = expectedNextSpotIndex[currActiveSpotIndex];
		}
		break;
	}
	default:
		break;

	}
	return res;
}
void AnalysisTrackPwLowClass::RelockPwTracking(int16_t pulseCount)
{
	//finds index of last active pw spot
	int16_t bufferSize = pwBuffer.size();
	uint16_t startIndex = max(0, bufferSize - 1);
	uint16_t endIndex = max(1, bufferSize - (int16_t) pulseCount);
	currActiveSpotIndex = -1;
	for (uint16_t i = startIndex; i > endIndex; i--)
	{
		int32_t currPw = pwBuffer[i];
		for (uint16_t j = 0; j < pwSpots.size(); j++)
		{
			int32_t diff = pwSpots[j];
			diff = abs(diff - currPw);
			if (diff < (int32_t) deviationAllowed[j])
			{
				currActiveSpotIndex = j;
				break;
			}
		}
		if (currActiveSpotIndex != -1)
		{
			break;
		}
	}

	if (currPwType == SequenceTypesEnum::seq_dwell)
	{
		if (currActiveSpotIndex != -1)
		{
			currActiveSpotIndex = expectedNextSpotIndex[currActiveSpotIndex];
		}
	}

}
SequenceTypesEnum AnalysisTrackPwLowClass::PwAnalysisNew()
{/* PW Analysis */
	SequenceTypesEnum sequenceTypeGuess = SequenceTypesEnum::seq_unknown;
	SequenceTypesEnum finalType = SequenceTypesEnum::seq_unknown;
//	uint64_t PRIarr[256];
//	int16_t bufferSize = pwBuffer.size();
//	uint16_t startIndex = max(0, bufferSize - (int16_t) histogramedPulseCount);
//	uint16_t endIndex = bufferSize;
//	for (uint16_t i = startIndex, k = 0; i < endIndex; i++, k++) // DEBUGGING ONLY
//	{
//		PRIarr[k] = pwBuffer[i];
//
//	}
	sequenceTypeGuess = guessPwTypeUsingPwHistogram(); // Analyze histogram and make an initial guess of pw type
	finalType = sequenceTypeGuess;

	if (sequenceTypeGuess != SequenceTypesEnum::seq_unknown)
	{
		pwSpots.resize(0);
		currActiveSpotIndex = -1;
		deviationAllowed.resize(0);
	}
	if (sequenceTypeGuess == SequenceTypesEnum::seq_jitter)
	{
		for (uint16_t i = 0; i < pwHistogramPeaksSortedFiltered.size(); i++)
		{
			uint16_t tempDev;
			float tempDevAllowed = (float) (tempSpotAverageValue[i]);
			tempDev = round(tempDevAllowed);
			tempDev = min((uint16_t) 20, tempDev);
			tempDev = max((uint16_t) 10, tempDev);
			tempDevAllowed = tempDevAllowed * 0.2; 	// +-20% deviation allowed
			pwSpots.push_back(tempSpotAverageValue[i]);
			deviationAllowed.push_back(tempDev);
		}
	}
	else if (sequenceTypeGuess != SequenceTypesEnum::seq_unknown)
	{
		finalType = SequenceTypesEnum::seq_dwell;
		for (uint16_t i = 0; i < pwHistogramPeaksSortedFiltered.size(); i++)
		{
			uint16_t tempDev;

			float tempDevAllowed = (float) (tempSpotAverageValue[i]);
			tempDevAllowed = tempDevAllowed * 0.2; 	// +-20% deviation allowed

			tempDev = round(tempDevAllowed);
			tempDev = min((uint16_t) 20, tempDev);
			tempDev = max((uint16_t) 10, tempDev);
			if (2 * tempSpotStdDevValue[i] < tempDev)	// Finalize pw type
			{

			}
			else
			{
				finalType = SequenceTypesEnum::seq_jitter;
			}

			pwSpots.push_back(tempSpotAverageValue[i]);
			deviationAllowed.push_back(tempDev);
//		if (tempSpotAverageValue[i] > 400 && tempSpotAverageValue[i] < 700
//				&& pwSpots.size() > 1)
//			tempSpotAverageValue[i] = tempSpotAverageValue[i] * 1;
		}
	}
	return finalType;

}
SequenceTypesEnum AnalysisTrackPwLowClass::guessPwTypeUsingPwHistogram()
{
	SequenceTypesEnum sequenceTypeGuess = SequenceTypesEnum::seq_unknown;

	FindPeaksInPwHistogram();	// Analyze histograms to find valid pw peaks
	filterHistogramPeaks();		// filter searched peaks
	FindMeanValuesOfPwsFromHistPeaks(histogramedPulseCount,
			pwHistogramPeaksSortedFiltered); // find average values of filtered peaks and their std dev

	if (pwHistogramPeaksSortedFiltered.size() == 0)
	{
		return sequenceTypeGuess;
	}

	if (pwHistogram.getRangePrecisionLevel() == 0)
	{
		FillPwConcurranceMatrix(histogramedPulseCount,
				pwHistogramPeaksSortedFiltered); // Generate a concurrence matrix representing the sequence of pw peaks in pw buffer
		FindExpectedNextSpotOfEachSpot(); // determine next spot to current spot using pattern in concurrence matrix
		if(pwHistogramPeaksSortedFiltered.size() > 1)
		{
			sequenceTypeGuess = SequenceTypesEnum::seq_jitter;
		}
		else
		{
			float tempDevAllowed = (float) (tempSpotAverageValue[0]);
			tempDevAllowed = tempDevAllowed * 0.1; //0.25% deviation allowed

			tempDevAllowed = round(tempDevAllowed);
			tempDevAllowed = min((uint32_t) 20, (uint32_t)tempDevAllowed);	// Finalize RF type
			if (tempSpotRange[0] > tempDevAllowed)
				sequenceTypeGuess = SequenceTypesEnum::seq_jitter;
			else
				sequenceTypeGuess = SequenceTypesEnum::seq_dwell;
		}
	}
	else if (pwHistogram.getRangePrecisionLevel() == 1
			&& histogramedPulseCount >= 16)
	{
		sequenceTypeGuess = SequenceTypesEnum::seq_jitter;
	}
	else
	{
		sequenceTypeGuess = SequenceTypesEnum::seq_unknown;
	}
	return sequenceTypeGuess;
}

void AnalysisTrackPwLowClass::FindPeaksInPwHistogram()
{ 	// This function analyzes pyramid of histograms of PW values to find peaks

	pwHistogram.findHistogramPeaks();
	pwHistogram.findPyramidOperatingLevel_V2(); // operating histogram level from pyramid
	pwHistogramPeaks = pwHistogram.getPeaks();

	pwHistogramPeaksSorted = pwHistogramPeaks;

	// sort peaks according to no. of pulses belonging each
	sort(pwHistogramPeaksSorted.begin(), pwHistogramPeaksSorted.end(),
			PeakInInputData::comparisonByWeight);
}
void AnalysisTrackPwLowClass::filterHistogramPeaks()
{
	pwHistogramPeaksSortedFiltered.resize(0);
	int16_t peakValue = pwHistogramPeaksSorted[0].rangeSampleCount;
	// Here peaks with pulse count less than 30% of max peak are discarded
	for (uint16_t i = 0; i < pwHistogramPeaksSorted.size(); i++)
	{
		if (pwHistogramPeaksSorted[i].rangeSampleCount > 0.20 * peakValue
		/*&& pwHistogramPeaksSorted[i].rangeSampleCount >= 8*/)
		{
			pwHistogramPeaksSortedFiltered.push_back(pwHistogramPeaksSorted[i]);
		}
	}
}
void AnalysisTrackPwLowClass::ActivateHistObjects()
{
//the function enables pushing of pw into histograms if already not enabled
	if (histogramingEnabledFlag == 1)
	{

	}
	else
	{
		pwHistogram.clearHistograms();
		histogramedPulseCount = 0;
		histogramingEnabledFlag = 1;
	}
}
void AnalysisTrackPwLowClass::DeactivateHistObjects()
{
	pwHistogram.clearHistograms();
	histogramedPulseCount = 0;
	histogramingEnabledFlag = 0;
}
void AnalysisTrackPwLowClass::PushDataToHistObjects(uint32_t &currPw)
{
//pw values are put into histogram pyramids for peak finding if histograms are enabled
	if (histogramingEnabledFlag == 1)
	{
		pwHistogram.addToHistogram(currPw);
		histogramedPulseCount++;
		if (histogramedPulseCount >= 128) // max pw histogram size 128
		{
			histogramingEnabledFlag = 0;
		}
	}
}
void AnalysisTrackPwLowClass::PushLastNPulsesToHistograms(uint16_t pulseCount)
{
	int16_t bufferSize = pwBuffer.size();
	uint16_t startIndex = max(1, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;

	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		PushDataToHistObjects(pwBuffer[i]);
	}
}
void AnalysisTrackPwLowClass::FillPwConcurranceMatrix(uint16_t pulseCount,
		vector<PeakInInputData> &peaksForLabeling)
{/* This function creates concurrence matrix that stores the sequence of peaks
 * present in pri buffer.
 * using last N values specified by pulse count fills the priConcurranceMatrix
 */
	int16_t bufferSize = pwBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;
	int16_t lastPwValueLabel = 0;

	memset(pwConcMatrix, 0, sizeof(pwConcMatrix));
	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		uint32_t currPwValue = pwBuffer[i];
		int16_t currPwLabel = LabelUsingHistogramPeaks(currPwValue,
				peaksForLabeling);

		pwConcMatrix[lastPwValueLabel + 1][currPwLabel + 1]++; // add one to label to cater -1

		lastPwValueLabel = currPwLabel;
	}
}
void AnalysisTrackPwLowClass::FindExpectedNextSpotOfEachSpot()
{
	memset(expectedNextSpotIndex, -1, sizeof(expectedNextSpotIndex));
	uint16_t peakCount = pwHistogramPeaksSorted.size();
	// using peaks in concurrence matrix determine next spot to current spot (ignoring unknown indexes)
	for (uint16_t rowIndex = 1; rowIndex < peakCount + 1; rowIndex++) //first row and column in concurrence matrix is for unknown value
	{
		int16_t peakValueIndex = 1;
		int16_t peakValue = pwConcMatrix[rowIndex][1];

		for (uint16_t colIndex = 1; colIndex < peakCount + 1; colIndex++)
		{
			if (pwConcMatrix[rowIndex][colIndex] > peakValue)
			{
				peakValue = pwConcMatrix[rowIndex][colIndex];
				peakValueIndex = colIndex;
			}
		}
		if (peakValue > 0)
		{
			expectedNextSpotIndex[rowIndex - 1] = peakValueIndex - 1; // subtract 1 as first index pwConcMatrix is for unlabeled value
		}
	}
}
void AnalysisTrackPwLowClass::UpdateCurrPwType(SequenceTypesEnum newPriType)
{
	if (currPwType != newPriType)
	{
		pwTypeHistory.push_back(newPriType);
	}

	currPwType = newPriType;

	if (newPriType == SequenceTypesEnum::seq_dwell)
	{
		pwTrackingPossibleFlag = 1;
	}
}
int16_t AnalysisTrackPwLowClass::LabelUsingHistogramPeaks(uint32_t inputValue,
		vector<PeakInInputData> &peaks)
{ /* Find which peak does the input value belong to */
	int16_t res = -1;
	for (uint16_t i = 0; i < peaks.size(); i++)
	{
		if ((uint32_t)peaks[i].rangeMinValue <= inputValue
				&& (uint32_t)inputValue <= peaks[i].rangeMaxValue)
		{
			res = i;
		}
	}
	return res;
}
void AnalysisTrackPwLowClass::FindMeanValuesOfPwsFromHistPeaks(
		uint16_t pulseCount, vector<PeakInInputData> &peaksForLabeling)
{
// using last N values specified by pulse count find mean Pw Values
	int16_t bufferSize = pwBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;

	memset(tempSpotStdDevValue, 0, sizeof(tempSpotStdDevValue));
	memset(tempSpotAverageValue, 0, sizeof(tempSpotAverageValue));
	memset(tempSpotAverageCount, 0, sizeof(tempSpotAverageCount));
	memset(tempSpotMaxValue, 0, sizeof(tempSpotMaxValue));
	memset(tempSpotMinValue, 0xFFFFFFFF, sizeof(tempSpotMinValue));
	/* calculate average values using pw samples in buffer */
	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		int32_t currPwValue = pwBuffer[i];
		int16_t currPwLabel = LabelUsingHistogramPeaks(currPwValue,
				peaksForLabeling);	// search this value in histogram peaks
		if (currPwLabel != -1)
		{
			tempSpotAverageCount[currPwLabel]++;
			tempSpotAverageValue[currPwLabel] += currPwValue;
		}
	}

	for (uint16_t i = 0; i < peaksForLabeling.size(); i++)
	{
		if (tempSpotAverageCount[i] != 0)
		{
			tempSpotAverageValue[i] = tempSpotAverageValue[i]
					/ tempSpotAverageCount[i];
		}
	}
	/* calculate std dev values using pw samples in buffer */
	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		int32_t currPwValue = pwBuffer[i];
		int16_t currPwLabel = LabelUsingHistogramPeaks(currPwValue,
				peaksForLabeling);	// search this value in histogram peaks
		if (currPwLabel != -1)
		{
			int32_t temp = tempSpotAverageValue[currPwLabel];
			temp = abs(temp - currPwValue);
			tempSpotStdDevValue[currPwLabel] += (temp * temp);
		}
	}

	for (uint16_t i = 0; i < peaksForLabeling.size(); i++)
	{
		if (tempSpotAverageCount[i] != 0)
		{
			tempSpotStdDevValue[i] = tempSpotStdDevValue[i]
					/ tempSpotAverageCount[i];
			tempSpotStdDevValue[i] = sqrt(tempSpotStdDevValue[i]);
		}
	}
	/* find max variation i.e. range of all spots */
	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		uint32_t currPwValue = pwBuffer[i];
		int16_t currPwLabel = LabelUsingHistogramPeaks(currPwValue,
				peaksForLabeling);	// search this value in histogram peaks
		if (currPwLabel != -1)
		{
			if (currPwValue > tempSpotMaxValue[currPwLabel])
				tempSpotMaxValue[currPwLabel] = currPwValue;
			if (currPwValue < tempSpotMinValue[currPwLabel])
				tempSpotMinValue[currPwLabel] = currPwValue;
		}
	}
	for (uint16_t i = 0; i < peaksForLabeling.size(); i++)
	{
		if (tempSpotAverageCount[i] != 0)
		{
			tempSpotRange[i] = tempSpotMaxValue[i] - tempSpotMinValue[i];
		}
	}

}
uint16_t AnalysisTrackPwLowClass::GetPeakPwFromHistogram()
{

	FindPeaksInPwHistogram();
	if (pwHistogram.getRangePrecisionLevel() == 0)
	{
		return pwHistogramPeaksSorted[0].rangeMean;
	}
	return 0;
}
uint16_t AnalysisTrackPwLowClass::GetLastPwValueFromBuffer()
{
	return (uint16_t) pwBuffer.back();
}

