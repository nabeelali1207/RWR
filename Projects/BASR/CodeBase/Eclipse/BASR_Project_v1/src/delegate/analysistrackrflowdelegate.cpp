/*
 * analysistrackrflowdelegate.cpp
 *
 *  Created on: Apr 28, 2020
 *      Author: gmd
 */

#include "analysistrackrflowdelegate.h"

AnalysisTrackRfLowClass::AnalysisTrackRfLowClass() :
		rfHistogram((1 << 17), 0, 24, 1)
{ // constructor
// Initialization
	lastAnalysisPulseCount = 0;
	newResultsAvailableForCopyFlag = 0;
	lastAnalysisWasSuccessfulFlag = 0;
	currRfType = SequenceTypesEnum::seq_unknown;
	rfTypeHistory.set_capacity(8);
	rfTypeHistory.push_back(currRfType);
	rfMedian = 0;
	rfBuffer.set_capacity(128);
	rfValidBuffer.set_capacity(128);
	samplesProcessed = 0;
	rfTrackingPossibleFlag = 0;

	rfHistogramPeaks.reserve(8);
	rfHistogramPeaksSorted.reserve(8);
	rfHistogramPeaksFiltered.reserve(8);

	histogramingEnabledFlag = 1;
	histogramedPulseCount = 0;
	countSinceLastRfAnalysisCall = 0;

	pdwThresholdForRfAnalysis = 8;
	trackingFailedCount = 0;
	currActiveDwellSpotIndex= -1;
	currActiveSpotIndex = -1;

}

void AnalysisTrackRfLowClass::AddNextRfValue(uint32_t currRf, int8_t rfValid)
{ // RF Tracking and update
	samplesProcessed++;
	rfBuffer.push_back(currRf);
	rfValidBuffer.push_back(rfValid);

	if (rfValid == 0)
		return;

	bool rfAnalysisCalled = false;
	// RF values inserted into histogram pyramids for peak finding if histograms have any space left
	PushDataToHistObjects(currRf, rfValid);
	int16_t rfTrackingFlag = rfTracking(currRf, rfValid);
	if (rfTrackingFlag == 1)
	{ // RF of incoming pulse successfully tracked
		trackingFailedCount = 0;
		rfTrackingPossibleFlag = 1;
	}
	else
	{
		trackingFailedCount++;
	}

	if (trackingFailedCount >= pdwThresholdForRfAnalysis
			&& countSinceLastRfAnalysisCall >= 8)
	{ // New or updated RF
		rfAnalysisCalled = true;	// Call RF analysis
		if (histogramingEnabledFlag == 0)
		{ // insert last N pulses into histograms and find peaks and do seq analysis
			ActivateHistObjects();
			uint16_t interestedPulseCount = min(countSinceLastRfAnalysisCall,
					trackingFailedCount);
			interestedPulseCount = min(128, interestedPulseCount + 1);
			PushLastNPulsesToHistograms(interestedPulseCount);
		}
		/* RF Analysis */
		SequenceTypesEnum newRFType = SequenceTypesEnum::seq_unknown;
		if (histogramedPulseCount >= 8)
		{ //analysis will be called only if atleast 8 pulses with valid RF are present
			newRFType = RfAnalysisNew();
		}
		if (rfMedian == 0)
			rfMedian = currRf;
		lastAnalysisPulseCount = histogramedPulseCount;
		if (newRFType == SequenceTypesEnum::seq_unknown)
		{ // RF analysis failed, accumulate more pulses
			pdwThresholdForRfAnalysis += min((uint32_t) 32,
					pdwThresholdForRfAnalysis);
			if (pdwThresholdForRfAnalysis > 128) // Max RF buffer size
			{
				pdwThresholdForRfAnalysis = ConstLow::asRfPdwCountMin;
				DeactivateHistObjects();
				trackingFailedCount = 0;
			}
		}
		else
		{ // RF analysis successful
			pdwThresholdForRfAnalysis = ConstLow::asRfPdwCountMin;
			UpdateCurrRfType(newRFType);
			DeactivateHistObjects();	// clear histograms
			trackingFailedCount = 0;
			lastAnalysisWasSuccessfulFlag = 1;
		}
	}
	else if (trackingFailedCount == 3)
	{ // try to re-lock RF tracking if not changed
		rfTrackingPossibleFlag = 0;
	}

	if (rfAnalysisCalled == true)
	{
		newResultsAvailableForCopyFlag = 1;
		countSinceLastRfAnalysisCall = 0;
	}
	else
	{
		countSinceLastRfAnalysisCall++;
	}

}
int8_t AnalysisTrackRfLowClass::GetRfScore(uint16_t queryRf, int8_t rfValid)
{ // compare sample RF with the RF of track and calculate its score
	if (rfValid != 1)
		return 0;
	int32_t diff = queryRf;
	switch (currRfType)
	{
	case SequenceTypesEnum::seq_dwell:
	{
		for (uint16_t i = 0; i < dwellSpots.size(); i++)
		{
			diff = queryRf;
			diff = diff - dwellSpots[i];
			if (abs(diff) < (int32_t) dwellSpotsDeviationAllowed[i])
			{
				return 100;	// RF matched
			}
		}
		break;
	}
	case SequenceTypesEnum::seq_jitter:
	{
		return 0;	// RF jitter tracking not enabled
		break;
	}
	default:
		break;

	}
	return 0;
}
int8_t AnalysisTrackRfLowClass::rfTracking(uint32_t & queryRf, int8_t& rfValid)
{	// RF tracking and update
	if (rfValid != 1)
	{
		return 0;
	}

	int32_t diff = 0;
	int8_t res = -1;
	switch (currRfType)
	{
	case SequenceTypesEnum::seq_dwell:
	{
		for (uint16_t i = 0; i < dwellSpots.size(); i++)
		{
			diff = queryRf;
			diff = diff - dwellSpots[i];
			if (abs(diff) < (int32_t) dwellSpotsDeviationAllowed[i])
			{
				return 1;
			}
		}
		break;
	}
	default:
		break;

	}
	return res;
}
void AnalysisTrackRfLowClass::RelockRfTracking(int16_t pulseCount)
{
}
SequenceTypesEnum AnalysisTrackRfLowClass::RfAnalysisNew()
{/* RF Analysis */
	SequenceTypesEnum sequenceTypeGuess;	//= SequenceTypesEnum::seq_unknown;
	SequenceTypesEnum finalType = SequenceTypesEnum::seq_unknown;
	int8_t dwellCount = 0, jitterCount = 0, complexCount = 0;
//		int64_t RFarr[128];
//	int16_t bufferSize = rfBuffer.size();
//	uint16_t startIndex = max(0, bufferSize - (int16_t) histogramedPulseCount);
//	uint16_t endIndex = bufferSize;
//	for (uint16_t i = startIndex, k = 0; i < endIndex; i++, k++) // DEBUGGING ONLY
//	{
//		RFarr[k] = rfBuffer[i];
//	}

	sequenceTypeGuess = guessRfTypeUsingRfHistogram();// Analyze histogram and make an initial guess of RF type
//	finalType = sequenceTypeGuess;
	if (sequenceTypeGuess != SequenceTypesEnum::seq_unknown
			&& sequenceTypeGuess != SequenceTypesEnum::seq_jitter
			&& sequenceTypeGuess != SequenceTypesEnum::seq_complex)
	{
		for (uint16_t i = 0; i < rfHistogramPeaksFiltered.size(); i++)
		{
			uint32_t tempDev;

			float tempDevAllowed = (float) (tempSpotAverageValue[i]);
			tempDevAllowed = tempDevAllowed * 0.0025; //0.25% deviation allowed

			tempDev = round(tempDevAllowed);
			tempDev = min((uint32_t) 8, tempDev);	// Finalize RF type
			if (tempSpotRange[i] <= tempDev)
			{
				dwellCount++;
			}
			else
			{
				tempDevAllowed = (float) (tempSpotAverageValue[i]);
				tempDevAllowed = tempDevAllowed * 0.10; //10% deviation allowed

				tempDev = round(tempDevAllowed);
				tempDev = min((uint32_t) 500, tempDev);
				if (tempSpotRange[i] < tempDev)
				{
					jitterCount++;
				}
				else
				{
					complexCount++;
				}
			}
		}
		if (complexCount > 0)
		{
			sequenceTypeGuess = SequenceTypesEnum::seq_complex;
		}
		else if (jitterCount > 0)
			sequenceTypeGuess = SequenceTypesEnum::seq_jitter;
		else if (sequenceTypeGuess
				== SequenceTypesEnum::seq_dwellOrjitterOrcomplex)
			sequenceTypeGuess = SequenceTypesEnum::seq_complex;

		// Now compare the total range to find out if it lies within jitter or complex
		if (sequenceTypeGuess == SequenceTypesEnum::seq_jitter)
		{
			ResetRfInfoVectors();
			UpdateJitterAndComplexRange();
			int32_t totalRange = abs(
					(int32_t) (rfRangeHigh[0]) - (int32_t) (rfRangeLow[0]));
			if (totalRange > 500)
				finalType = SequenceTypesEnum::seq_complex;
			else
				finalType = SequenceTypesEnum::seq_jitter;
		}

	}

	if (finalType == SequenceTypesEnum::seq_jitter
			|| finalType == SequenceTypesEnum::seq_complex)
	{
		if (histogramedPulseCount < 64)
		{
			finalType = SequenceTypesEnum::seq_unknown;
		}
	}
	if (sequenceTypeGuess == SequenceTypesEnum::seq_difftoa)
	{ // if sequence search stagger fails, it is declared as difftoa pri but the pulses criteria is stringent
		finalType = SequenceTypesEnum::seq_difftoa;
		ResetRfInfoVectors();
		UpdateJumpSpots();
	}
	else if (sequenceTypeGuess == SequenceTypesEnum::seq_dwell)
	{
		finalType = SequenceTypesEnum::seq_dwell;
		UpdateDwellSpots();
	}
	else if (sequenceTypeGuess == SequenceTypesEnum::seq_jitter)
	{
		finalType = SequenceTypesEnum::seq_jitter;
		ResetRfInfoVectors();
		UpdateJitterAndComplexRange();
	}
	else if (sequenceTypeGuess == SequenceTypesEnum::seq_complex)
	{
		finalType = SequenceTypesEnum::seq_complex;
		ResetRfInfoVectors();
//		UpdateComplexSpots();
		UpdateJitterAndComplexRange();
	}

	return finalType;
}
SequenceTypesEnum AnalysisTrackRfLowClass::guessRfTypeUsingRfHistogram()
{
	SequenceTypesEnum sequenceTypeGuess = SequenceTypesEnum::seq_unknown;
	float dwellScore = 0, jumpScore = 0;
	int8_t noOfPeaks = 0;
	int32_t numOfPulsesCategorized = 0;
	float successRatio = 0;
	if (histogramedPulseCount == 0)
	{
		return sequenceTypeGuess;
	}
	FindPeaksInRfHistogram();	// Analyze histograms to find valid RF peaks

	filterHistogramPeaksForDwell(3); // pick histogram peaks that had occurred at least 2 times
	noOfPeaks = rfHistogramPeaksFiltered.size();
	FindMeanValuesOfRfsFromHistPeaks(histogramedPulseCount,
			rfHistogramPeaksFiltered);
	for (int32_t i = 0; i < noOfPeaks; i++)
	{
		numOfPulsesCategorized += rfHistogramPeaksFiltered[i].rangeSampleCount;
	}
	successRatio = (float) (numOfPulsesCategorized) / histogramedPulseCount;
	if (histogramedPulseCount < 64
			&& (rfHistogram.getRangePrecisionLevel() > 0))
		sequenceTypeGuess = SequenceTypesEnum::seq_unknown;
	else if (rfHistogram.getRangePrecisionLevel() > 1)
	{
		sequenceTypeGuess = SequenceTypesEnum::seq_complex;
	}
	else if (rfHistogram.getRangePrecisionLevel() == 1)
	{
		int32_t agileRange = findAgileRange();
		if (agileRange > 500)
			sequenceTypeGuess = SequenceTypesEnum::seq_complex;
		else
			sequenceTypeGuess = SequenceTypesEnum::seq_jitter;
	}
	else
	{
		if (rfHistogramPeaks.size() > 16)
		{
			sequenceTypeGuess = SequenceTypesEnum::seq_jitter;
		}
		else
		{
			dwellScore = CalcDwellScoreFromConcMatrix();

			if (dwellScore > 0.65 && successRatio > 0.7)	// dwell pri found
			{
				FindMeanValuesOfRfsFromHistPeaks(histogramedPulseCount,
						rfHistogramPeaksFiltered); // find average values of filtered peaks and their ranges
				sequenceTypeGuess = SequenceTypesEnum::seq_dwell; // could be jitter/complex too depending on spot variation
			}
			else	// dwell pattern not found, search for stagger now
			{
				jumpScore = CalcJumpScore();
				if (jumpScore > 0.65 && successRatio > 0.7)
					sequenceTypeGuess = SequenceTypesEnum::seq_difftoa;
			}
		}
	}
	if (sequenceTypeGuess == SequenceTypesEnum::seq_unknown
			&& histogramedPulseCount >= 64
			&& (rfHistogram.getRangePrecisionLevel() == 0))
	{
		sequenceTypeGuess = SequenceTypesEnum::seq_dwellOrjitterOrcomplex;
	}
	return sequenceTypeGuess;
}

void AnalysisTrackRfLowClass::FindPeaksInRfHistogram()
{	// This function analyzes pyramid of histograms of RF values to find peaks
	rfHistogram.findHistogramPeaks();
	rfHistogram.findPyramidOperatingLevel_V2(90);// operating histogram level from pyramid
	rfHistogramPeaks = rfHistogram.getPeaks();

	rfHistogramPeaksSorted = rfHistogramPeaks;
	// sort peaks according to no. of pulses belonging each
	sort(rfHistogramPeaksSorted.begin(), rfHistogramPeaksSorted.end(),
			PeakInInputData::comparisonByWeight);
	if (rfHistogramPeaksSorted.size() > 0)
	{
		rfMedian = rfHistogramPeaksSorted[0].rangeMean;
	}
}
void AnalysisTrackRfLowClass::filterHistogramPeaks()
{
	rfHistogramPeaksFiltered.resize(0);
	if (rfHistogramPeaksSorted.size() > 0)
	{
		if (rfHistogramPeaksSorted[0].rangeSampleCount
				>= 4 /*0.6*histogramedPulseCount*/)
			rfHistogramPeaksFiltered.push_back(rfHistogramPeaksSorted[0]);
	}
}
void AnalysisTrackRfLowClass::ActivateHistObjects()
{
//the function enables pushing of toas into histograms if already not enabled
	if (histogramingEnabledFlag == 1)
	{

	}
	else
	{
		rfHistogram.clearHistograms();
		histogramedPulseCount = 0;
		histogramingEnabledFlag = 1;
	}
}
void AnalysisTrackRfLowClass::DeactivateHistObjects()
{
	rfHistogram.clearHistograms();
	histogramedPulseCount = 0;
	histogramingEnabledFlag = 0;
}
void AnalysisTrackRfLowClass::PushDataToHistObjects(uint32_t & currRf,
		int8_t& rfValid)
{
//toas/data are put into histogram pyramids for peak finding if histograms are enabled
	if (histogramingEnabledFlag == 1)
	{
		if (rfValid == 1)
		{
			rfHistogram.addToHistogram(currRf);
			histogramedPulseCount++;
			if (histogramedPulseCount >= 128)
			{
				histogramingEnabledFlag = 0;
			}
		}
	}
}
void AnalysisTrackRfLowClass::PushLastNPulsesToHistograms(uint16_t pulseCount)
{
	int16_t bufferSize = rfBuffer.size();
	uint16_t startIndex = max(1, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;

	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		PushDataToHistObjects(rfBuffer[i], rfValidBuffer[i]);
	}
}
int16_t AnalysisTrackRfLowClass::LabelUsingHistogramPeaks(int32_t inputValue,
		vector<PeakInInputData> &peaks)
{ // This function matches the input Value with all peaks and return the label of peak if a match is found
	int16_t res = -1; // unknown label
	for (uint16_t i = 0; i < peaks.size(); i++)
	{
		if (peaks[i].rangeMinValue <= inputValue
				&& inputValue <= peaks[i].rangeMaxValue)
		{
			res = i;
		}
	}
	return res;
}
void AnalysisTrackRfLowClass::FillRFConcurranceMatrix(uint16_t pulseCount,
		vector<PeakInInputData> & peaksForLabeling)
{
	// using last N values specified by pulse count fills the priConcurranceMatrix
	int16_t bufferSize = rfBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;
	int16_t lastRfValueLabel = 0;

	memset(rfConcMatrix, 0, sizeof(rfConcMatrix)); // 2D matrix
	for (uint16_t i = startIndex; i < endIndex; i++) // scan the whole pri buffer
	{
		int32_t currRfValue = rfBuffer[i];
		int16_t currRfLabel = 0;
		currRfLabel = LabelUsingHistogramPeaks(currRfValue, peaksForLabeling); // find a match of pri value from available peaks
		rfConcMatrix[lastRfValueLabel + 1][currRfLabel + 1]++; // add one to label to cater -1 i.e. unknown label

		lastRfValueLabel = currRfLabel;

	}
}
void AnalysisTrackRfLowClass::filterHistogramPeaksForDwell(uint32_t threshold)
{
	rfHistogramPeaksFiltered.resize(0);
	for (uint16_t i = 0; i < min(16, (int32_t) rfHistogramPeaks.size()); i++)
	{

		if (rfHistogramPeaks[i].rangeSampleCount >= threshold)
		{
			rfHistogramPeaksFiltered.push_back(rfHistogramPeaks[i]); // Filtered peaks
		}
	}
}
int32_t AnalysisTrackRfLowClass::findAgileRange()
{
	if (rfHistogramPeaks.size() == 0)
		return 0;
	int8_t maxIndex = rfHistogramPeaks.size() - 1;
	int32_t totalRange = abs(
			(int32_t) (rfHistogramPeaks[maxIndex]).rangeMaxValue
					- (int32_t) (rfHistogramPeaks[0].rangeMinValue));
	return totalRange;
}
float AnalysisTrackRfLowClass::CalcDwellScoreFromConcMatrix()
{

	if (rfHistogramPeaksFiltered.size() == 0
			|| rfHistogramPeaksFiltered.size() > 16)
	{
		return 0;
	}
// Generate a concurrence matrix representing the sequence of pri peaks in pri buffer
	FillRFConcurranceMatrix(histogramedPulseCount, rfHistogramPeaksFiltered);

	float sumofProbabilityOfDwell = 0;
	int32_t peakCount = rfHistogramPeaksFiltered.size() + 1; // plus one as one spot is for unknown
	float probOfDwellSpots[33];
// calculate dwell score using sequence found in concurrence matrix
	for (int16_t j = 0; j < peakCount; j++) //columns
	{ //first row and column in concurrence matrix is for unknown value
		int32_t tempSum = 0;
		for (int16_t i = 0; i < peakCount; i++) // rows
		{
			tempSum += rfConcMatrix[i][j]; // sum all rows of given column
		}
		if (tempSum > 0)
		{
			probOfDwellSpots[j] = rfConcMatrix[j][j];
			probOfDwellSpots[j] = probOfDwellSpots[j] / tempSum; // prob of current spot
		}
		else
			probOfDwellSpots[j] = 0;
	}
	for (int16_t i = 1; i < peakCount; i++) // ignore start point which is for unlabeled
	{
		sumofProbabilityOfDwell = sumofProbabilityOfDwell + probOfDwellSpots[i];
	}
	float dwellScore = sumofProbabilityOfDwell
			/ (rfHistogramPeaksFiltered.size());

	return dwellScore;
}
float AnalysisTrackRfLowClass::CalcJumpScore()
{

	if (rfHistogramPeaksFiltered.size() == 0
			|| rfHistogramPeaksFiltered.size() > 16)
	{
		return 0;
	}

	int8_t ProbabilityOfJump = 0;
	int8_t peakCount = rfHistogramPeaksFiltered.size();
	int8_t pulsePerLevel = histogramedPulseCount / peakCount;
	uint8_t diff = 0;

	for (int16_t j = 0; j < peakCount; j++)
	{
		diff = abs(
				(int8_t) rfHistogramPeaksFiltered[j].rangeSampleCount
						- pulsePerLevel);
		if (diff < 2)
		{
			ProbabilityOfJump++;
		}
	}

	float jumpScore = (float) (ProbabilityOfJump) / peakCount;

	return jumpScore;
}
void AnalysisTrackRfLowClass::FindExpectedNextSpotOfEachSpot()
{
//	memset(expectedNextSpotIndex, -1, sizeof(expectedNextSpotIndex));
//	uint16_t peakCount = rfHistogramPeaksSorted.size();
//	// using peaks in concurrance matrix determine next spot to current spot (ignoring unknown indexes)
//	for (uint16_t rowIndex = 1; rowIndex < peakCount + 1; rowIndex++) //first row and column in concurrance matrix is for unknown value
//	{
//		int16_t peakValueIndex = 1;
//		int16_t peakValue = rfConcMatrix[rowIndex][1];
//
//		for (uint16_t colIndex = 1; colIndex < peakCount + 1; colIndex++)
//		{
//			if (rfConcMatrix[rowIndex][colIndex] > peakValue)
//			{
//				peakValue = rfConcMatrix[rowIndex][colIndex];
//				peakValueIndex = colIndex;
//			}
//		}
//		if (peakValue > 0)
//		{
//			expectedNextSpotIndex[rowIndex - 1] = peakValueIndex - 1; // subtract 1 as first index pwConcMatrix is for unlabeled value
//		}
//	}
}
void AnalysisTrackRfLowClass::UpdateJitterAndComplexRange()
{ // copy the results i.e. spots and their relevant information
//SequenceTypesEnum rfType = SequenceTypesEnum::seq_unknown;
	float tempDevAllowed = (float) (tempSpotAverageValue[0]);
	tempDevAllowed = round(tempDevAllowed * 0.1);
	tempDevAllowed = min((uint32_t) 500, (uint32_t) tempDevAllowed); //0.25% deviation allowed

	int8_t middleIndex = round(rfHistogramPeaksFiltered.size() / 2);
	int8_t endIndex = rfHistogramPeaksFiltered.size() - 1;
	//uint32_t averageRF = (tempSpotMaxValue[endIndex]+tempSpotMinValue[0])/2;

	rfSpots.push_back(tempSpotAverageValue[middleIndex]);
	rfRangeHigh.push_back(tempSpotMaxValue[endIndex]);
	rfRangeLow.push_back(tempSpotMinValue[0]);
	deviationAllowed.push_back(tempDevAllowed);
	rfSpotsLastOccurrance.push_back(samplesProcessed);
}
void AnalysisTrackRfLowClass::UpdateCurrRfType(SequenceTypesEnum newRfType)
{
	if (currRfType != newRfType)
	{
		rfTypeHistory.push_back(newRfType);
	}

	currRfType = newRfType;

	if (newRfType == SequenceTypesEnum::seq_dwell)
	{
		rfTrackingPossibleFlag = 1;
	}
	else
	{
		rfTrackingPossibleFlag = 0;
	}
}
void AnalysisTrackRfLowClass::UpdateJumpSpots()
{ // copy the results i.e. spots and their relevant information
	int8_t alreadyPresentFlag = 0;
	for (uint16_t i = 0; i < rfHistogramPeaksFiltered.size(); i++)
	{
		float tempDevAllowed = (float) (tempSpotAverageValue[i]);
		tempDevAllowed = round(tempDevAllowed * 0.0025);
		tempDevAllowed = min((uint32_t) 8, (uint32_t) tempDevAllowed); //0.25% deviation allowed

		for (uint16_t j = 0; j < rfSpots.size(); j++)
		{
			int32_t diff = tempSpotAverageValue[i] - rfSpots[j];
			if (abs(diff) <= tempDevAllowed)
			{
				alreadyPresentFlag = 1;
				break;
			}
		}
		if (alreadyPresentFlag == 0)
		{
			if (rfSpots.size() >= 16)
			{
				uint8_t foundIndex = 0;
				rfSpots[foundIndex] = tempSpotAverageValue[i];
				deviationAllowed[foundIndex] = tempDevAllowed;
				rfRangeLow[foundIndex] = tempSpotMinValue[i];
				rfRangeHigh[foundIndex] = tempSpotMaxValue[i];
				//load default values

				rfSpotsLastOccurrance[foundIndex] = samplesProcessed;
			}
			else
			{
				rfSpots.push_back(tempSpotAverageValue[i]);
				rfRangeHigh.push_back(tempSpotMaxValue[i]);
				rfRangeLow.push_back(tempSpotMinValue[i]);
				deviationAllowed.push_back(tempDevAllowed);
				rfSpotsLastOccurrance.push_back(samplesProcessed);
			}
		}
	}

}
void AnalysisTrackRfLowClass::UpdateDwellSpots()
{ // copy the results i.e. spots and their relevant information

// expire old spots

// if new spots were found from histogramming
// we need to update them too

	for (uint16_t i = 0; i < rfHistogramPeaksFiltered.size(); i++)
	{

		int32_t dwellDeviationAllowed = 0;
		int8_t alreadyPresentFlag = 0;

		float tempDevAllowed = (float) (tempSpotAverageValue[i]);
		tempDevAllowed = tempDevAllowed * 0.0025; //0.25% deviation allowed

		dwellDeviationAllowed = tempDevAllowed;
		dwellDeviationAllowed = min(50, dwellDeviationAllowed);
		dwellDeviationAllowed = max(32, dwellDeviationAllowed);
		for (uint16_t j = 0; j < dwellSpots.size(); j++)
		{
			int32_t diff = tempSpotAverageValue[i] - dwellSpots[j];
			if (abs(diff) <= dwellDeviationAllowed)
			{
				alreadyPresentFlag = 1;
				break;
			}
			else
			{
				int32_t remainderValue = tempSpotAverageValue[i]
						% dwellSpots[j];

				if (remainderValue <= dwellDeviationAllowed)
				{
					alreadyPresentFlag = 1;
					currActiveDwellSpotIndex = j;
					break;
				}
				else if (abs(remainderValue - (int32_t) (dwellSpots[j]))
						<= dwellDeviationAllowed)
				{
					alreadyPresentFlag = 1;
					currActiveDwellSpotIndex = j;
					break;
				}
			}
		}

		if (alreadyPresentFlag == 0) // Not found, add new
		{
			AddNewDwellSpot(tempSpotAverageValue[i], dwellDeviationAllowed);
		}
	}

}
void AnalysisTrackRfLowClass::AddNewDwellSpot(int32_t spotValue,
		int16_t devAllowed)
{
	if (dwellSpots.size() >= 32)
	{

	}
	else
	{
		dwellSpots.push_back(spotValue);
		dwellSpotsDeviationAllowed.push_back(devAllowed);
		currActiveDwellSpotIndex = dwellSpots.size() - 1;
	}
}
void AnalysisTrackRfLowClass::UpdateJitterSpots()
{ // copy the results i.e. spots and their relevant information
	int8_t alreadyPresentFlag = 0;
// copy the peaks
	for (uint16_t i = 0; i < rfHistogramPeaksFiltered.size(); i++)
	{
		float tempDevAllowed = (float) (tempSpotAverageValue[i]);
		tempDevAllowed = tempDevAllowed * 0.10; //10% deviation allowed
		tempDevAllowed = min((int32_t) (500), (int32_t) (tempDevAllowed));

//
		for (uint16_t j = 0; j < rfSpots.size(); j++)
		{
			int32_t diff = tempSpotAverageValue[i] - rfSpots[j];
			if (abs(diff) <= tempDevAllowed)
			{
				alreadyPresentFlag = 1;
				break;
			}
		}
		if (alreadyPresentFlag == 0)
		{
			if (rfSpots.size() >= 16)
			{
				uint8_t foundIndex = 0;
				rfSpots[foundIndex] = tempSpotAverageValue[i];
				deviationAllowed[foundIndex] = tempDevAllowed;
				rfRangeLow[foundIndex] = tempSpotMinValue[i];
				rfRangeHigh[foundIndex] = tempSpotMaxValue[i];

				//load default values

				rfSpotsLastOccurrance[foundIndex] = samplesProcessed;
			}
			else
			{
				rfSpots.push_back(tempSpotAverageValue[i]);
				rfRangeHigh.push_back(tempSpotMaxValue[i]);
				rfRangeLow.push_back(tempSpotMinValue[i]);
				deviationAllowed.push_back(tempDevAllowed);
				rfSpotsLastOccurrance.push_back(samplesProcessed);
			}
		}
	}

}
void AnalysisTrackRfLowClass::UpdateComplexSpots()
{ // copy the results i.e. spots and their relevant information
	int8_t alreadyPresentFlag = 0;
//	uint16_t i = 0;
// copy the peaks

	for (uint16_t i = 0; i < rfHistogramPeaksFiltered.size(); i++)
	{
		float tempDevAllowed = (float) (tempSpotAverageValue[i]);
		tempDevAllowed = tempDevAllowed * 0.3; //15% deviation allowed

		for (uint16_t j = 0; j < rfSpots.size(); j++)
		{
			int32_t diff = tempSpotAverageValue[i] - rfSpots[j];
			if (abs(diff) <= tempDevAllowed)
			{
				alreadyPresentFlag = 1;
				break;
			}
		}
		if (alreadyPresentFlag == 0)
		{
			if (rfSpots.size() >= 32)
			{
				uint8_t foundIndex = 0;
				rfSpots[foundIndex] = tempSpotAverageValue[i];
				deviationAllowed[foundIndex] = tempDevAllowed;
				rfRangeLow[foundIndex] = tempSpotMinValue[i];
				rfRangeHigh[foundIndex] = tempSpotMaxValue[i];
				//load default values

				rfSpotsLastOccurrance[foundIndex] = samplesProcessed;
			}
			else
			{
				rfSpots.push_back(tempSpotAverageValue[i]);
				rfRangeHigh.push_back(tempSpotMaxValue[i]);
				rfRangeLow.push_back(tempSpotMinValue[i]);
				deviationAllowed.push_back(tempDevAllowed);
				rfSpotsLastOccurrance.push_back(samplesProcessed);
			}
		}

	}

}
void AnalysisTrackRfLowClass::ResetRfInfoVectors()
{
	rfSpots.resize(0);	// reset the rf array for switch in rf type
	deviationAllowed.resize(0);
	rfRangeHigh.resize(0);
	rfRangeLow.resize(0);
	rfSpotsExpectedNextSpot.resize(0);
	rfSpotsLastOccurrance.resize(0);
}
void AnalysisTrackRfLowClass::FindMeanValuesOfRfsFromHistPeaks(
		uint32_t pulseCount, vector<PeakInInputData> & peaksForLabeling)
{
// using last N values specified by pulse count find mean Pri Values
	int16_t bufferSize = rfBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;

	memset(tempSpotStdDevValue, 0, sizeof(tempSpotStdDevValue));
	memset(tempSpotAverageValue, 0, sizeof(tempSpotAverageValue));
	memset(tempSpotAverageCount, 0, sizeof(tempSpotAverageCount));

	memset(tempSpotMaxValue, 0, sizeof(tempSpotMaxValue));
	memset(tempSpotMinValue, 0xFFFFFFFF, sizeof(tempSpotMinValue));

	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		int32_t currRfValue = rfBuffer[i];
		if (rfValidBuffer[i] == 1)
		{
			int16_t currRfLabel = LabelUsingHistogramPeaks(currRfValue,
					peaksForLabeling);
			if (currRfLabel != -1)
			{
				tempSpotAverageCount[currRfLabel]++;
				tempSpotAverageValue[currRfLabel] += currRfValue;
			}
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

	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		int32_t currRfValue = rfBuffer[i];
		if (rfValidBuffer[i] == 1)
		{
			int16_t currRfLabel = LabelUsingHistogramPeaks(currRfValue,
					peaksForLabeling);
			if (currRfLabel != -1)
			{
				int32_t temp = tempSpotAverageValue[currRfLabel];
				temp = abs(temp - currRfValue);
				tempSpotStdDevValue[currRfLabel] += temp;
			}
		}
	}

	for (uint16_t i = 0; i < peaksForLabeling.size(); i++)
	{
		if (tempSpotAverageCount[i] != 0)
		{
			tempSpotStdDevValue[i] = tempSpotStdDevValue[i]
					/ tempSpotAverageCount[i];
		}
	}

	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		int32_t currRfValue = rfBuffer[i];

		int16_t currLabel = LabelUsingHistogramPeaks(currRfValue,
				peaksForLabeling);
		if (currLabel != -1)
		{
			if (currRfValue > tempSpotMaxValue[currLabel])
				tempSpotMaxValue[currLabel] = currRfValue;
			if (currRfValue < tempSpotMinValue[currLabel])
				tempSpotMinValue[currLabel] = currRfValue;
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
SequenceTypesEnum AnalysisTrackRfLowClass::GetCurrRfType()
{
	return currRfType;
}
vector<uint32_t>& AnalysisTrackRfLowClass::GetDwellSpots()
{
	return dwellSpots;
}
vector<uint32_t>& AnalysisTrackRfLowClass::GetRfSpots()
{
	return rfSpots;
}
vector<uint32_t>& AnalysisTrackRfLowClass::GetUpperRange()
{
	return rfRangeHigh;
}
vector<uint32_t>& AnalysisTrackRfLowClass::GetLowerRange()
{
	return rfRangeLow;
}
uint32_t AnalysisTrackRfLowClass::GetSpotRange(uint16_t spotIndex)
{
	return tempSpotRange[spotIndex];
}
uint32_t AnalysisTrackRfLowClass::GetSpotMinValue(uint16_t spotIndex)
{
	return tempSpotMinValue[spotIndex];
}
uint32_t AnalysisTrackRfLowClass::GetSpotMaxValue(uint16_t spotIndex)
{
	return tempSpotMaxValue[spotIndex];
}
