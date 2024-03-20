/*
 * analysistrackprilowdelegate.cpp
 *
 *  Created on: Apr 2, 2020
 *      Author: gmd
 */

#include "analysistrackprilowdelegate.h"

AnalysisTrackPriLowClass::AnalysisTrackPriLowClass() : // @suppress("Class members should be properly initialized")
		priHistogram((1 << 21), 0, 64, 0), diffPriHistogram((1 << 19),
				(1 << 18) - 1, 16, 0)
{ // constructor
// Initialization
	newResultsAvailableForCopyFlag = 0; // is set when new spots are found by priAnalysis
	lastAnalysisWasSuccessfulFlag = 0;
	lastAnalysisPulseCount = 0;
	firstLockedTOA = 0;

	samplesProcessed = 0;
	priTrackingPossibleFlag = 0;
	histogramedPulseCount = 0;
	histogramingEnabledFlag = 1;
	countSinceLastPriAnalysisCall = 0;
	countSinceLastSuccessfulPriAnalysis = 0;
	currPriType = SequenceTypesEnum::seq_unknown;
	lastKnownPriType = SequenceTypesEnum::seq_unknown;
	priTypeSwitchFlag = 0;
	histBased_currPriType = SequenceTypesEnum::seq_unknown;

	priTypeHistory.set_capacity(5);
	priTypeHistory.push_back(SequenceTypesEnum::seq_unknown);

//	seqSearchLockedToaBuffer.reserve(256);
//	seqSearchLockedToaBuffer.resize(0);
	toaBuffer.set_capacity(256);
	toaBuffer.push_back(0); // 1 element is required at start
	diffToaBuffer.set_capacity(256);
	diffToaBuffer.push_back(0); // 1 element is required at start

	priLabels.set_capacity(256);

	staggerPhaseCount = 0;
	currentStaggerPhase = -1;
	nextPredictedStaggerPhase = -1;
	staggerFrameInferred = 0;
	staggerFrameActual = 0;
	staggerCyclingSpotsFoundFlag = 0;
	alternateMissingPulseCount = 0;

	slidePosStartPri = 0;
	slidePosRate = 0;
	slidePosEndPri = 0;
	slidePosLockedPriValue = 0;
	slidePosLockedPriValueSampleIndex = 0;

	sineLastLockedIndex = -1;
	sineNextPredictedIndex = -1;

	currActiveDwellSpotIndex = -1;
	priMax = 0;
	lastPriMax = 0;
	labelingFailedCount = 0;
	trackingFailedCount = 0;

	pdwThresholdForPriAnalysis = ConstLow::asPriPdwCountMin;
}

void AnalysisTrackPriLowClass::setFirstToa(uint64_t &firstToa)
{ //this function is called only once per track (on track creation)
	toaBuffer[0] = firstToa;
}
uint8_t AnalysisTrackPriLowClass::CanGuessNextTOA()
{/* if pri of track is declared and pri is locked then next toa could be guessed
 * consecutive 4 missing pulses breaks the lock of pri
 */
	if (priTrackingPossibleFlag == 0)
		return 0;
	if (priTypeHistory.back() == SequenceTypesEnum::seq_unknown)
	{
		return 0;
	}
	return 1;
}
int8_t AnalysisTrackPriLowClass::DoesToaBelongToThisTrack(uint64_t &queryToa)
{/* This function does compares incoming pri with the track's estimated pri to find a match
 * for scoring
 */
	int64_t currDiffToa = (int64_t) queryToa - (int64_t) toaBuffer.back();
//	int64_t currDiffPri = currDiffToa - (int64_t) diffToaBuffer.back();

	int16_t lbl = -1;
	switch (currPriType)
	{
	case SequenceTypesEnum::seq_dwell:
	{
		//        lbl = LabelAccordingToPriType(SequenceTypesEnum::seq_dwell,
		//                currDiffToa);
		if (currActiveDwellSpotIndex != -1)
		{
			if (abs(currDiffToa - dwellSpots[currActiveDwellSpotIndex])
					<= dwellSpotsDeviationAllowed[currActiveDwellSpotIndex])
			{
				lbl = currActiveDwellSpotIndex;
			}
			else if (abs(currDiffToa - 2 * dwellSpots[currActiveDwellSpotIndex])
					<= dwellSpotsDeviationAllowed[currActiveDwellSpotIndex])
			{
				lbl = currActiveDwellSpotIndex; //  1 missing pulse
			}
			else if (abs(currDiffToa - 3 * dwellSpots[currActiveDwellSpotIndex])
					<= dwellSpotsDeviationAllowed[currActiveDwellSpotIndex])
			{
				lbl = currActiveDwellSpotIndex; //  2 missing pulses
			}
		}
		if (lbl != -1)
		{
			return 1;
		}
		break;
	}
	case SequenceTypesEnum::seq_stagger:
	{
		if (nextPredictedStaggerPhase != -1)
		{
			int32_t diff = currDiffToa;
			int16_t expectedStaggerPhase = nextPredictedStaggerPhase;
			diff = diff - staggerPhases[expectedStaggerPhase];
			if (abs(diff) < 33)
			{
				return 1;
			}
			else if (diff > 33) // missing pulses detected
			{
				expectedStaggerPhase =
						staggerExpectedNextIndex[expectedStaggerPhase]; // advance expected stagger phase to search for missing pulse
				diff = diff - staggerPhases[expectedStaggerPhase];
				if (abs(diff) < 33)
				{ /* 1 missing pulse */
					return 1;
				}
				else if (diff > 33) // currently it handles only 3 consecutive missing pulses
				{
					expectedStaggerPhase =
							staggerExpectedNextIndex[expectedStaggerPhase]; // advance expected stagger phase to search for another missing pulse
					diff = diff - staggerPhases[expectedStaggerPhase];
					if (abs(diff) < 33)
					{/* 2 missing pulses */
						return 1;
					}
					else if (diff > 33)
					{
						expectedStaggerPhase =
								staggerExpectedNextIndex[expectedStaggerPhase]; // advance expected stagger phase to search for another missing pulse
						diff = diff - staggerPhases[expectedStaggerPhase];
						if (abs(diff) < 33)
						{/* 3 missing pulses */
							return 1;
						}
					}
				}
				return -1;
			}
		}
		break;
	}
	case SequenceTypesEnum::seq_slidePos:
	{
		int32_t currDiffPri = currDiffToa - slidePosLockedPriValue;
		if (abs(currDiffPri - slidePosRate) < 10)
		{
			return 1;
		}
		break;
	}
	case SequenceTypesEnum::seq_slideNeg:
	{
		int32_t currDiffPri = currDiffToa - slideNegLockedPriValue;
		if (abs(currDiffPri - slideNegRate) < 10)
		{
			return 1;
		}
		break;
	}
	case SequenceTypesEnum::seq_slideTri:
	{
		int32_t currDiffPri = currDiffToa - slideTriLockedPriValue;
		if (abs(currDiffPri - slideTriRate) < 10)
		{
			return 1;
		}
		else if (abs(currDiffPri + slideTriRate) < 10)
		{
			return 1;
		}
		break;
	}
	case SequenceTypesEnum::seq_slideSine:
	{
		int32_t currDiffPri = currDiffToa
				- sinePriSpots[sineNextPredictedIndex];
		if (currDiffPri < sinePriSpotsDeviationAllowed[sineNextPredictedIndex])
		{
			return 1;
		}
		break;
	}
	case SequenceTypesEnum::seq_jitter:
	{
		for (int16_t i = 0; i < (int16_t) priSpots.size(); i++)
		{
			if (abs(currDiffToa - priSpots[i]) < priSpotsDevAllowed[i])
			{ // under 15% of any jitter priSpot
				return 1;
			}
		}
		break;
	}
	default:
		break;

	}
	return 0;
}
int8_t AnalysisTrackPriLowClass::PushToa(uint64_t currPulseToa)
{
	/* This function is called when a pulse is successfully associated with a track
	 * In this function a new TOA is pushed into the buffer
	 * if sequence tracking fails then PRI analysis is called
	 */
	if( currPulseToa== 2364914)
		currPulseToa=currPulseToa*1;
	int8_t returnVal = 0;
	samplesProcessed++;
	int32_t currDiffToa = (int32_t) (currPulseToa - toaBuffer.back());
	int32_t currDiffPri = currDiffToa - diffToaBuffer.back();
	int16_t priTrackingFlag=0;
//	if (currDiffToa == 0)
//		return;
	// TOAs are inserted into histogram pyramids for peak finding if histograms have any space left
	PushDataToHistObjects(currDiffToa, currDiffPri);

	bool priAnalysisCalled = false;

	toaBuffer.push_back(currPulseToa);	// TOA buffer with original TOA values
	diffToaBuffer.push_back(currDiffToa);   // PRI buffer with diffTOA values
	if (priTrackingPossibleFlag == 1)
	{
		priTrackingFlag = PriTracking(currDiffToa, currDiffPri);
	}
//	int16_t priTrackingFlag=0;
	if (priTrackingFlag == 1) 		// PRI successfully tracked
	{
		trackingFailedCount = 0;
		priTrackingPossibleFlag = 1;
		returnVal = 1;
	}
	else
	{
		trackingFailedCount++;
	}
	countSinceLastPriAnalysisCall++;
	countSinceLastSuccessfulPriAnalysis++;

	if (trackingFailedCount > 7 && histogramingEnabledFlag == 0)
	{
		ActivateHistObjects();
		uint16_t interestedPulseCount = min(countSinceLastPriAnalysisCall,
				trackingFailedCount);
		interestedPulseCount = min((uint16_t) 256, interestedPulseCount);
		PushLastNPulsesToHistograms(interestedPulseCount);
	}

	if (trackingFailedCount >= pdwThresholdForPriAnalysis
			&& countSinceLastPriAnalysisCall >= 8)
	{ // consecutive 8 or more pulses have been failed for pri tracking
		priAnalysisCalled = true;
		countSinceLastPriAnalysisCall = 0;

		SequenceTypesEnum newPriType = PriAnalysisNew(); // complete PRI analysis
		lastAnalysisPulseCount = histogramedPulseCount;
//		SequenceTypesEnum newPriType=SequenceTypesEnum::seq_unknown;
		if (newPriType == SequenceTypesEnum::seq_unknown)
		{ // pri analysis failed, now increase the trigger to pri analysis
			pdwThresholdForPriAnalysis += min((uint32_t) 64,
					pdwThresholdForPriAnalysis);
			if (pdwThresholdForPriAnalysis > 256)
			{
				pdwThresholdForPriAnalysis = ConstLow::asPriPdwCountMin; // minimum 8 pulses

				DeactivateHistObjects();
				trackingFailedCount = 0;
				histogramingEnabledFlag = 1;

			}
		}
		else // pri found successfully
		{
			returnVal = 2;
			pdwThresholdForPriAnalysis = ConstLow::asPriPdwCountMin; // minimum 8 pulses
			UpdateCurrPriType(newPriType);

			DeactivateHistObjects();
			trackingFailedCount = 0;
			if (priTrackingPossibleFlag == 0)
			{ // we need to start accumulating pulses for next
			  //analysis if complex pri type was detected

				histogramingEnabledFlag = 1;
			}

			lastAnalysisWasSuccessfulFlag = 1;
			countSinceLastSuccessfulPriAnalysis = 0;
		}

	}
	else if (trackingFailedCount == 8)
	{
		priTrackingPossibleFlag = 0;	// disable further pri tracking
		// Try to re-lock from the first failed pulse and afterwards search the rest of the pulses in sequence
		RelockPriTracking(trackingFailedCount);
	}
	else if (trackingFailedCount == 4)
	{// Try to re-lock from the first failed pulse and afterwards search the rest 3 pulses in sequence
		RelockPriTracking(trackingFailedCount);
	}
	else if (trackingFailedCount == 7)
	{// Try to re-lock from the first failed pulse and afterwards search the rest 3 pulses in sequence
		RelockPriTracking(trackingFailedCount);
	}
	else if (trackingFailedCount == 3)
	{
		priTrackingPossibleFlag = 0; // disable further pri tracking after 3 consecutive pulses have failed tracking
	}

	if (priAnalysisCalled == true)
	{
		newResultsAvailableForCopyFlag = 1;
	}
	if (currDiffToa > (1 << 22))
	{
		DeactivateHistObjects();
		histogramingEnabledFlag = 1;
		countSinceLastPriAnalysisCall = 0;
		trackingFailedCount = 0;
		pdwThresholdForPriAnalysis = ConstLow::asPriPdwCountMin;
	}
	return returnVal;
}

void AnalysisTrackPriLowClass::CalledBeforeRporting()
{
	if (currPriType == SequenceTypesEnum::seq_slidePos
			|| currPriType == SequenceTypesEnum::seq_slideNeg
			|| currPriType == SequenceTypesEnum::seq_slideTri
			|| currPriType == SequenceTypesEnum::seq_slideSine)
	{
		newResultsAvailableForCopyFlag = 1;
		lastAnalysisWasSuccessfulFlag = 1;
	}
}
void AnalysisTrackPriLowClass::CalledOnReportingPeriodEnd()
{
	if (currPriType == SequenceTypesEnum::seq_dwell)
	{
		ExpireOldDwellSpots();
	}
}
int32_t AnalysisTrackPriLowClass::SumOfVector(vector<int32_t> &dVector)
{ // sum any vector array
	int32_t sum = 0;
	for (int16_t i = 0; i < (int16_t) dVector.size() - 1; i++)
	{
		sum = sum + dVector[i];

	}
	return sum;
}

int16_t AnalysisTrackPriLowClass::LabelAccordingToPriType(
		SequenceTypesEnum seqType, int32_t currDiffToa)
{ // this function matches current difftoa with existing pri spots according to pri type of emitter
	switch (seqType)
	{
	case SequenceTypesEnum::seq_dwell:
	{
		for (uint16_t i = 0; i < dwellSpots.size(); i++)
		{

			if (abs(currDiffToa - dwellSpots[i])
					<= dwellSpotsDeviationAllowed[i])
			{
				return i;
			}
			else
			{
				int32_t referenceValue = dwellSpots[i];
				int32_t deviationAllowed = dwellSpotsDeviationAllowed[i];

				if (findIfNthMultiple(currDiffToa, referenceValue,
						deviationAllowed) == true)
					return i;
//
//				if (remainderValue <= deviationAllowed)
//				{
//					return i;
//				}
//				else if (abs(remainderValue - referenceValue)
//						<= deviationAllowed)
//				{
//					return i;
//				}
			}
		}
		break;
	}
	case SequenceTypesEnum::seq_stagger:
	{
		for (uint16_t i = 0; i < staggerPhases.size(); i++)
		{
			if (abs(currDiffToa - staggerPhases[i]) < 33)
			{
				return i;
			}
		}
		break;
	}
	default:
		break;

	}
	return -1;
}
int16_t AnalysisTrackPriLowClass::LabelPriUsingAverageValues(
		int32_t currDiffToa)
{
	for (uint16_t i = 0; i < priHistogramPeaksFiltered.size(); i++)
	{

		if (currDiffToa >= (int32_t) tempSpotMinValue[i]
				&& currDiffToa <= (int32_t) tempSpotMaxValue[i])
		{
			return i;
		}
		else
		{
			int32_t referenceValue = tempSpotAverageValue[i];
			int32_t deviationAllowed = tempSpotRange[i];
			bool doesSpotMatch = findIfNthMultiple(currDiffToa, referenceValue,
					deviationAllowed);
			if (doesSpotMatch)
			{
				return i;
			}
		}
	}
	return -1;
}
void AnalysisTrackPriLowClass::filterHistogramPeaksForStagger()
{ // This function filters the pris/phases of stagger.

	uint32_t maxSampleCount = 0;
	/*
	 // 1 missing pulse could be handled here

	 for (uint16_t i = 0; i < priHistogramPeaksSorted.size(); i++)
	 {
	 //if this peak is sum of two peaks then handle
	 }
	 */
	if (priHistogramPeaksSorted.size() == 0)
	{
		return;
	}
	maxSampleCount = priHistogramPeaksSorted[0].rangeSampleCount;

// ideally pulsesCount of all stagger phases should be same. Some variation is possible due to missing pulses.
// Here phases with pulse count less than 30% of max phase are discarded
	priHistogramPeaksFiltered.resize(0);
	for (uint16_t i = 0; i < min(32, (int32_t) priHistogramPeaksSorted.size());
			i++)
	{
		if (priHistogramPeaksSorted[i].rangeSampleCount > 0.3 * maxSampleCount)
			priHistogramPeaksFiltered.push_back(priHistogramPeaksSorted[i]); // Filtered peaks
	}
}
float AnalysisTrackPriLowClass::CalcStaggerScoreFromConcMatrix()
{
	/*this function is called when some peaks have been found and dwell cannot be declared,
	 * hence there is probability of stagger's presence
	 */
	memset(strengthOfStaggerPhases, 0, sizeof(strengthOfStaggerPhases));
	memset(peakValueIndices, 0, sizeof(peakValueIndices));
	memset(tempSpotExpectedNextIndex, -1, sizeof(tempSpotExpectedNextIndex));
	vector<PeakInInputData> priHistogramPeaksFilterednew;

	if (priHistogramPeaks.size() == 0)
	{
		return 0;
	}
//after finding peaks in histograms, filter some peaks before further processing
// filter peaks/phases to avoid processing false peaks

	uint16_t peakCount = priHistogramPeaksFiltered.size();

	if (peakCount == 0 || peakCount > 32)
	{
		return 0;
	}
	int16_t maxOccurrence = StaggerAnalyzeConcurranceMatrix(
			priHistogramPeaksFiltered);

	float probabilityOfStagger = 0;
	for (uint16_t rowIndex = 1; rowIndex < peakCount + 1; rowIndex++) //first row and column in concurrence matrix is for unknown value
	{
		if (strengthOfStaggerPhases[rowIndex] >= ceil(0.4 * maxOccurrence))
		{
			probabilityOfStagger =
					probabilityOfStagger
							+ min(1.0f,
									(float) strengthOfStaggerPhases[rowIndex]
											/ priHistogramPeaksFiltered[peakValueIndices[rowIndex]
													- 1].rangeSampleCount);
			if (strengthOfStaggerPhases[peakValueIndices[rowIndex]]
					>= ceil(0.4 * maxOccurrence))
				tempSpotExpectedNextIndex[rowIndex - 1] =
						peakValueIndices[rowIndex] - 1; // subtract 1 as first index priConcMatrix is for unlabeled value
			else
				tempSpotExpectedNextIndex[rowIndex - 1] = -1;
			priHistogramPeaksFilterednew.push_back(
					priHistogramPeaksFiltered[rowIndex - 1]);
		}
	}
	peakCount = priHistogramPeaksFilterednew.size();
	float staggerScore = 0;
	if (peakCount < priHistogramPeaksFiltered.size())
	{
		priHistogramPeaksFiltered = priHistogramPeaksFilterednew;
		staggerScore = CalcStaggerScoreFromConcMatrix();
		return staggerScore;
	}

	staggerScore = probabilityOfStagger / peakCount;

	return staggerScore;
}
int16_t AnalysisTrackPriLowClass::StaggerAnalyzeConcurranceMatrix(
		vector<PeakInInputData> &peaks)
{
	uint16_t peakCount = peaks.size();

	if (peakCount == 0 || peakCount > 32)
	{
		return 0;
	}
	FillPriConcurranceMatrix(histogramedPulseCount, peaks, false);

//float expectedPulsesPerPhase = histogramedPulseCount / peakCount;

// using peaks in concurrence matrix determine next spot to current spot (ignoring unknown indexes)
	int16_t maxOccurrence = priConcMatrix[1][1];
	for (uint16_t rowIndex = 1; rowIndex < peakCount + 1; rowIndex++) //first row and column in concurrence matrix is for unknown value
	{

		for (uint16_t colIndex = 1; colIndex < peakCount + 1; colIndex++)
		{
			if (priConcMatrix[rowIndex][colIndex] > maxOccurrence)
			{
				maxOccurrence = priConcMatrix[rowIndex][colIndex];
			}
		}
	}
	for (uint16_t rowIndex = 1; rowIndex < peakCount + 1; rowIndex++) //first row and column in concurrence matrix is for unknown value
	{
		int16_t peakValueIndex = 1;
		int16_t peakValue = priConcMatrix[rowIndex][1];

		for (uint16_t colIndex = 1; colIndex < peakCount + 1; colIndex++)
		{
			if (priConcMatrix[rowIndex][colIndex] > peakValue)
			{
				peakValue = priConcMatrix[rowIndex][colIndex];

				peakValueIndex = colIndex;
			}
		}
		if (peakValueIndex == rowIndex)
			peakValue = 0;
		strengthOfStaggerPhases[rowIndex] = peakValue;
		peakValueIndices[rowIndex] = peakValueIndex;
	}
	return maxOccurrence;
}
int8_t AnalysisTrackPriLowClass::StaggerTracking(int32_t &diffToa,
		int32_t &diffPri)
{
// this function needs to be quick and preferably avoid using loops for searching as it is called per pulse
	int32_t expectedStaggerPhase = nextPredictedStaggerPhase;

	if (expectedStaggerPhase == -1)
	{
		return -1;
	}

	int32_t diff = diffToa;
	diff = diff - staggerPhases[expectedStaggerPhase];
	if (abs(diff) < 33)
	{ // phase matched successfully with estimated one
		currentStaggerPhase = expectedStaggerPhase;
		nextPredictedStaggerPhase =
				staggerExpectedNextIndex[expectedStaggerPhase];
		alternateMissingPulseCount =0;
		return 1;
	}
	else if (diff > 33) // missing pulses detected, currently it handles only 3 consecutive missing pulses
	{
		expectedStaggerPhase = staggerExpectedNextIndex[expectedStaggerPhase]; // advance expected stagger phase to search for 1 missing pulse
		diff = diff - staggerPhases[expectedStaggerPhase];
		if (abs(diff) < 33)
		{ /* 1 missing pulse */
			currentStaggerPhase = expectedStaggerPhase;
			nextPredictedStaggerPhase =
					staggerExpectedNextIndex[expectedStaggerPhase];
			alternateMissingPulseCount++;
			return 1;
		}
		else if (diff > 33)
		{ /* 2 missing pulses */
			expectedStaggerPhase =
					staggerExpectedNextIndex[expectedStaggerPhase]; // advance expected stagger phase to search for another missing pulse
			diff = diff - staggerPhases[expectedStaggerPhase];
			if (abs(diff) < 33)
			{
				currentStaggerPhase = expectedStaggerPhase;
				nextPredictedStaggerPhase =
						staggerExpectedNextIndex[expectedStaggerPhase];
				alternateMissingPulseCount++;
				return 1;
			}
			else if (diff > 33)
			{ /* 3 missing pulses */
				expectedStaggerPhase =
						staggerExpectedNextIndex[expectedStaggerPhase]; // advance expected stagger phase to search for another missing pulse
				diff = diff - staggerPhases[expectedStaggerPhase];
				if (abs(diff) < 33)
				{
					currentStaggerPhase = expectedStaggerPhase;
					nextPredictedStaggerPhase =
							staggerExpectedNextIndex[expectedStaggerPhase];
					alternateMissingPulseCount++;
					return 1;
				}
			}
		}
		nextPredictedStaggerPhase =
				staggerExpectedNextIndex[expectedStaggerPhase];
		return -1;
	}
	return -1;
}

void AnalysisTrackPriLowClass::UpdateStaggerSpots()
{ // copy the results i.e. spots and their relevant information
// this function is called when PriHistogram-ConcurranceMatrix calculates the presence of a staggered PRI

	uint16_t histogramPeaks = priHistogramPeaksFiltered.size();

	StaggerIgnoreMultiplePeaks(histogramPeaks); // not used currently

	StaggerUpdateExpectedIndices();

	StaggerFillStaggerSpotsArray(histogramPeaks);

}
void AnalysisTrackPriLowClass::StaggerUpdateExpectedIndices()
{
	for (uint16_t i = 0; i < priHistogramPeaksFiltered.size(); i++)
	{
		staggerExpectedNextIndex[i] = tempSpotExpectedNextIndex[i];
	}

}
int16_t AnalysisTrackPriLowClass::StaggerFillStaggerSpotsArray(
		uint16_t peakCount)
{

// inserting every spot into staggerSpots in order of expected occurrence
	staggerPhases.resize(0);
	int8_t tempMappingVariable[32]; // a map of staggerCurrent Spot to next Spot
	int8_t tempMappingVariable2[32]; // a map of peaks to staggerPhases
	int8_t tempMappingCounter = 0;
	vector<uint8_t> spotVisitedFlag(peakCount, 0);
	vector<uint8_t> spotPushedToPhasesFlag(peakCount, 0);
	int16_t currPhaseIndex = -1;
	int16_t possibleStaggerFramesCount = 0;
	for (uint16_t i = 0; i < peakCount; i++)
	{

//find next unvisited spot which has next spot value defined
		if (currPhaseIndex == -1)
		{

			for (uint16_t j = 0; j < peakCount; j++)
			{
				if (spotVisitedFlag[j] == 0 && staggerExpectedNextIndex[j] != -1
						&& tempIgnoreSpotFlag[j] == 0)
				{
					currPhaseIndex = j;
					possibleStaggerFramesCount++;
					break;
				}
			}

			if (currPhaseIndex == -1)
			{
				// if unable to find any value which still unvisited and its expected next stagger spot is not -1
				break;//break the loop when no more unvisited valid spots can be found
			}
		}
// end of find next unvisited spot

		int8_t expectedNextSpot = staggerExpectedNextIndex[currPhaseIndex];
		if (spotVisitedFlag[currPhaseIndex] == 0)
		{
			spotVisitedFlag[currPhaseIndex] = 1;
			spotPushedToPhasesFlag[currPhaseIndex] = 1;
			//			staggerPhases.push_back(priHistogramPeaksFiltered[currPhaseIndex].rangeMean);
			tempMappingVariable2[currPhaseIndex] = staggerPhases.size();
			staggerPhases.push_back(tempSpotAverageValue[currPhaseIndex]); // average was found earlier

			currPhaseIndex = staggerExpectedNextIndex[currPhaseIndex];
			if (expectedNextSpot == -1)
			{
				tempMappingVariable[tempMappingCounter++] = -1;
			}
			else
			{

				if (spotVisitedFlag[expectedNextSpot] == 0)
				{
					//in this case next spot inserted in next iteration into stagger phases will be the next spot to current phase
					tempMappingVariable[tempMappingCounter++] =
							staggerPhases.size();
				}
				else
				{ //next spot has been already visited so do not do anything just find next spot to current spot
					tempMappingVariable[tempMappingCounter++] =
							tempMappingVariable2[expectedNextSpot];
					currPhaseIndex = -1;
				}
			}
		}
		else
		{
			currPhaseIndex = -1;
		}
	}

//We can determine if all stagger phases were found or not
//a check for finding if spot1.nextSpot == spot2 ... spotN.next==spot1
//otherwise staggerspots are not cyclic
	int8_t isSingleCyclePresentInStaggerPhases = 1; //assume every pri has a valid next pri
	for (uint16_t i = 0; i < peakCount; i++)
	{
		if (spotPushedToPhasesFlag[i] == 0 && tempIgnoreSpotFlag[i] == 0)
		{
			isSingleCyclePresentInStaggerPhases = 0; //if there is a single such spot present otherwise then break
			break;
		}
	}
	staggerCyclingSpotsFoundFlag = isSingleCyclePresentInStaggerPhases;

//push remaining unvisited spots into stagger spots
//     (we may mark these spots for filtering in future)
	for (uint16_t i = 0; i < peakCount; i++)
	{
		if (spotPushedToPhasesFlag[i] == 0 && tempIgnoreSpotFlag[i] == 0)
		{
			spotPushedToPhasesFlag[i] = 1;
			tempMappingVariable2[i] = staggerPhases.size();
			if (staggerExpectedNextIndex[i] == -1)
			{
				tempMappingVariable[tempMappingCounter++] = -1;
			}
			else
			{
				tempMappingVariable[tempMappingCounter++] =
						tempMappingVariable2[staggerExpectedNextIndex[i]];
			}
			tempMappingVariable[tempMappingCounter++] = -1;
			staggerPhases.push_back(tempSpotAverageValue[i]);
		}
	}
	staggerPhaseCount = staggerPhases.size();
	memcpy(staggerExpectedNextIndex, tempMappingVariable, tempMappingCounter);
//if(staggerExpectedNextIndex[0] == 2 && staggerPhaseCount == 2)
//staggerPhaseCount = 1+1;
	return possibleStaggerFramesCount;
}
uint16_t AnalysisTrackPriLowClass::StaggerIgnoreMultiplePeaks(
		uint16_t inputPeakCount)
{

	memset(tempIgnoreSpotFlag, 0, sizeof(tempIgnoreSpotFlag));
	return 0;
//	uint16_t outputPeakCount = 0;
//	int8_t extraPeaksCount = 0;
//
//	for (uint16_t i = 0; i < inputPeakCount; i++)
//	{
//		int16_t expectedNextSpot = staggerExpectedNextIndex[i];
//		if (expectedNextSpot == -1)
//		{
//			continue;
//		}
//
//		int32_t sumOfSpots = tempSpotAverageValue[i]
//				+ tempSpotAverageValue[expectedNextSpot];
//		int32_t deviationAllowed = tempSpotStdDevValue[i]
//				+ tempSpotStdDevValue[expectedNextSpot];
//		deviationAllowed = max(20, deviationAllowed);
//		for (uint16_t j = 0; j < inputPeakCount; j++)
//		{
//			int32_t diff = tempSpotAverageValue[j];
//			if (abs(diff - sumOfSpots) <= deviationAllowed)
//			{
//				tempIgnoreSpotFlag[j] = 1;
//
//				break;
//			}
//		}
//	}
//
//	for (uint16_t i = 0; i < inputPeakCount; i++)
//	{
//		if (tempIgnoreSpotFlag[i] == 1 && staggerExpectedNextIndex[i] != -1)
//		{
//			extraPeaksCount++;
//			for (uint16_t j = 0; j < inputPeakCount; j++)
//			{
//				if (staggerExpectedNextIndex[j] == i)
//				{
//					staggerExpectedNextIndex[j] =
//							staggerExpectedNextIndex[staggerExpectedNextIndex[j]];
//				}
//			}
//		}
//	}
//
//	outputPeakCount = inputPeakCount - extraPeaksCount;
//	return outputPeakCount;

}
int8_t AnalysisTrackPriLowClass::StaggerCheckDoublePeaks(
		vector<PeakInInputData> &peaks)
{
	int8_t doublePeaks = 0;

	if (peaks.size() == 2)
	{
		int32_t maxPhaseIndex =
				(peaks[0].rangeMean > peaks[1].rangeMean ? 0 : 1);
		int32_t minPhaseIndex =
				(peaks[0].rangeMean < peaks[1].rangeMean ? 0 : 1);
		int32_t deviationAllowed = peaks[maxPhaseIndex].rangeMaxValue
				- peaks[maxPhaseIndex].rangeMinValue;
		if ((abs(
				peaks[maxPhaseIndex].rangeMean
						- 2 * peaks[minPhaseIndex].rangeMean))
				<= deviationAllowed)
			doublePeaks = 1;
	}
	else
		return 0;
	return doublePeaks;
}
void AnalysisTrackPriLowClass::StaggerFindValidFeaturePoint(
		uint16_t consecutiveNess)
{
//if a spot has N next phases defined then valid flag will be set only for that
//e.g in stagger pattern A,B,C,D,-1 with N=2 feature point will only be ABC and BCD
// because only Pri spots A and B has 2 next spots valid

	consecutiveNess = min(consecutiveNess, (uint16_t) staggerPhases.size());

	memset(staggerValidFeaturePointFlag, 0,
			sizeof(staggerValidFeaturePointFlag));
	for (uint16_t i = 0; i < staggerPhases.size(); i++)
	{
		bool valid = true;

		int8_t expectedNextSpot = tempSpotExpectedNextIndex[i];
		for (uint16_t j = 0; j <= consecutiveNess; j++)
		{
			if (expectedNextSpot == -1)
			{
				valid = false;
				break;
			}
			else
			{
				expectedNextSpot = tempSpotExpectedNextIndex[expectedNextSpot];
			}
		}
		if (valid == true)
		{
			staggerValidFeaturePointFlag[i] = 1;
		}
	}

}
void AnalysisTrackPriLowClass::StaggerComputeFrame(uint16_t pulseCount)
{
// this function is used to compute actual stagger frame and stagger frame inferred from adding stagger spots
// actual stagger frame: that frame which exists in data i.e (Toa of current spotA + frame) = toa of next A
// inferrred stagger frame: is equal to sum of every found spot
// in some conditions both might not be same i.e. where not all peaks have been found by histograming

//sum of peaks
	int32_t sumOfStaggerPhases = 0;
	for (uint16_t i = 0; i < staggerPhases.size(); i++)
	{
		sumOfStaggerPhases += staggerPhases[i];
	}
	staggerFrameInferred = sumOfStaggerPhases;

	uint16_t staggerFeaturePointMin = min((uint16_t) 2,
			(uint16_t) (staggerPhases.size() / 2));

// determine spots that can be used to detect new frame, (those spots are called feature points also)
	StaggerFindValidFeaturePoint(staggerFeaturePointMin);

// using feature point search to find 3 values of actual stagger frame
// then use median filter to select the middle value
	int32_t staggerFrameValues[5] =
	{ 0, 0, 0, 0, 0 };
	int8_t framesFoundCounter = 0;

	int16_t bufferSize = diffToaBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;
	int8_t currSpotLabel = -1;
//	uint64_t tempSeqOccurToas[32];

	memset(staggerPhaseLastOccurranceToa, 0,
			sizeof(staggerPhaseLastOccurranceToa));

	boost::circular_buffer<int8_t> lastNLabels;
	lastNLabels.set_capacity(staggerFeaturePointMin + 1);
	lastNLabels.resize(0);

	for (uint16_t i = startIndex; i < startIndex + staggerFeaturePointMin; i++)
	{
		currSpotLabel = LabelAccordingToPriType(SequenceTypesEnum::seq_stagger,
				diffToaBuffer[i]);
		lastNLabels.push_back(currSpotLabel);
	}

	for (uint16_t i = startIndex + staggerFeaturePointMin; i < endIndex; i++)
	{
		currSpotLabel = LabelAccordingToPriType(SequenceTypesEnum::seq_stagger,
				diffToaBuffer[i]);
		lastNLabels.push_back(currSpotLabel);

		if (diffToaBuffer[i] > 2 * staggerFrameInferred)
		{
			for (uint16_t i = 0; i < staggerPhases.size(); i++)
			{
				staggerPhaseLastOccurranceToa[i] = 0;
			}
		}
		if (lastNLabels[0] != -1
				&& staggerValidFeaturePointFlag[lastNLabels[0]] == 1)
		{
			bool matchFound = true;
			for (uint16_t j = 0; j < staggerFeaturePointMin; j++)
			{
				if (staggerExpectedNextIndex[lastNLabels[j]]
						!= lastNLabels[j + 1])
				{
					matchFound = false;
				}
			}
			if (matchFound == true)
			{
				uint64_t newFoundValue = toaBuffer[i - staggerFeaturePointMin];
				uint64_t lastFoundValue =
						staggerPhaseLastOccurranceToa[lastNLabels[0]];
				if (lastFoundValue != 0)
				{
					lastFoundValue = newFoundValue - lastFoundValue;
					staggerFrameValues[framesFoundCounter] =
							(int32_t) lastFoundValue;

					framesFoundCounter++;
					if (framesFoundCounter > 4)
					{
						break;
					}
				}
				staggerPhaseLastOccurranceToa[lastNLabels[0]] = newFoundValue;
			}
		}
	}
	if (framesFoundCounter >= 3)
	{
		staggerFrameActual = medianFilterN<int32_t>(staggerFrameValues,
				framesFoundCounter);
	}
	else
	{
		staggerFrameActual = 0;
	}
}
float AnalysisTrackPriLowClass::SequenceSearchStagger_V1(uint16_t pulseCount)
{/* this function is once called when histogram peaks guess a stagger pattern
 * This function confirms the pulse sequence present in pri buffer in
 * accordance with detected pri i.e. stagger
 */
	int16_t bufferSize = diffToaBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;

	int32_t pulsesFound = 0;

	int16_t totalFoundCount = 0;
	int16_t consecutiveMissedCount = 0;

	int8_t currSpotLabel = -1;

	int16_t lockedIndex = currentStaggerPhase;
	int16_t expectedPhase = nextPredictedStaggerPhase;

	bool continueLoopTillValidPri = (lockedIndex == -1 ? true : false); // this will be true until a valid label is found
	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		currSpotLabel = LabelAccordingToPriType(SequenceTypesEnum::seq_stagger,
				diffToaBuffer[i]);

// this portion stalls the loop while lastLabel is below zero======
		if (continueLoopTillValidPri && currSpotLabel == -1)
		{
			continue;
		}
		else
		{
			//when a valid label is found and a valid next expected spot exists
			if (currSpotLabel != -1
					&& staggerExpectedNextIndex[currSpotLabel] != -1)
			{ // continue with regular sequence search
				continueLoopTillValidPri = false;
			}
		}
// end of stall portion============================================
		if (expectedPhase == -1)
		{
			consecutiveMissedCount++;
		}
		else if (currSpotLabel == -1) // missing pulses handling
		{
			consecutiveMissedCount++;
			if (expectedPhase != -1)
			{
				expectedPhase = staggerExpectedNextIndex[expectedPhase];
				if (expectedPhase != -1)
				{
					expectedPhase = staggerExpectedNextIndex[expectedPhase];
				}
			}
			else
			{
				consecutiveMissedCount++;
			}
		}
		else if (currSpotLabel == expectedPhase)
		{ //predicted next label from last spot matches actual label
			seqSearchLockedToaBuffer.push_back(toaBuffer[i]);
			lockedIndex = currSpotLabel; //to save global state of current spot
			if (pulsesFound == 0)
			{
				totalFoundCount++;
			}
			totalFoundCount++;
			pulsesFound++;
//			if (totalFoundCount == 1)
//				firstLockedTOA = toaBuffer[i];
			consecutiveMissedCount = 0;
		}
		if (consecutiveMissedCount >= 3)
		{
			consecutiveMissedCount = 0;
			pulsesFound = 0;
			continueLoopTillValidPri = true;
		}
		if (currSpotLabel != -1)
		{
			expectedPhase = staggerExpectedNextIndex[currSpotLabel];
		}

	}
	currentStaggerPhase = lockedIndex;
	nextPredictedStaggerPhase = expectedPhase;
	return ((float) totalFoundCount / (float) pulseCount);
}
int8_t AnalysisTrackPriLowClass::SlidePosTracking(int32_t &diffToa,
		int32_t &diffPri)
{
// positive sawtooth pri type
	if (diffToa >= slidePosStartPri) // valid slide PRI
	{
		if (abs((int32_t) diffPri - slidePosRate) < 10) // 2nd difference should be equal to slide rate
		{ // successfully tracked
			slidePosEndPri = max(slidePosEndPri, (int32_t) diffToa); // update end PRI
			slidePosLockedPriValue = diffToa;
			priMax = slidePosEndPri;
			return 1;
		}
		else if (abs((int32_t) diffToa - slidePosStartPri) < 10)
		{
			slidePosLockedPriValue = slidePosStartPri;
			return 1;
		}
	}
	else if (abs((int32_t) diffToa - slidePosStartPri) < 10)
	{
		slidePosLockedPriValue = slidePosStartPri;
		return 1;
	}
	return -1;
}

void AnalysisTrackPriLowClass::UpdateSlidePosSpots()
{ // copy the results i.e. spots and their relevant information

	slidePosStartPri = priHistogramPeaksSorted[0].rangeMinValue;
	slidePosEndPri = priHistogramPeaksSorted[0].rangeMaxValue;

// find mean slide rate
	int16_t bufferSize = diffToaBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) histogramedPulseCount);
	uint16_t endIndex = bufferSize;
	int64_t slideRateAvg = 0;
	int16_t avgSampleCount = 0;
	int32_t lastPri = diffToaBuffer[startIndex];
	int32_t minPri = lastPri, maxPri = lastPri;

	for (uint16_t i = startIndex + 1; i < endIndex; i++)
	{
		int32_t currPri = diffToaBuffer[i];
		int32_t diffPri = currPri - lastPri;
		if (diffPriHistogramPeaksSorted[0].rangeMinValue <= diffPri
				&& diffPri <= diffPriHistogramPeaksSorted[0].rangeMaxValue)
		{
			slideRateAvg += diffPri;
			avgSampleCount++;
		}
		lastPri = currPri;
		minPri = min(minPri, currPri);
		maxPri = max(maxPri, currPri);
	}

	if (avgSampleCount > 0)
	{
		slidePosRate = slideRateAvg / avgSampleCount;
	}
	else
	{
		slidePosRate = diffPriHistogramPeaksSorted[0].rangeMean;
	}
	if (minPri > slidePosStartPri)
		slidePosStartPri = minPri;
	if (maxPri < slidePosEndPri)
		slidePosEndPri = maxPri;
}
float AnalysisTrackPriLowClass::SequenceSearchSlidePos(uint16_t pulseCount)
{
	int16_t bufferSize = diffToaBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;
	uint16_t foundSeqCount = 0;

	int32_t currLockedPriValue = slidePosLockedPriValue;

	int32_t lastPriValue = diffToaBuffer[startIndex];

	int16_t conseqLblSuccessCount = 0;
	for (uint16_t i = startIndex + 1; i < endIndex; i++)
	{
		int32_t currPriValue = diffToaBuffer[i];
		int32_t currDiffPriValue = currPriValue - lastPriValue;
//		if (currPriValue >= slidePosStartPri && currPriValue <= slidePosEndPri)
//		{
//		}

		if (abs(currDiffPriValue - slidePosRate) < 10)
		{
			conseqLblSuccessCount++;
			foundSeqCount++;
		}
		else
		{
			conseqLblSuccessCount = 0;
		}
		if (conseqLblSuccessCount > 2)
		{
			conseqLblSuccessCount = 2; // if next time slide rate not found body of this if will not be executed
			currLockedPriValue = currPriValue;
			slidePosLockedPriValueSampleIndex = samplesProcessed - bufferSize
					+ i;
			slidePosEndPri = max(slidePosEndPri, currPriValue);
			slidePosStartPri = min(slidePosStartPri, currPriValue);
		}

		lastPriValue = currPriValue;
	}
	slidePosLockedPriValue = currLockedPriValue;

	return ((float) foundSeqCount) / (pulseCount - 1);
}
void AnalysisTrackPriLowClass::UpdateSlideNegSpots()
{ // copy the results i.e. spots and their relevant information

	slideNegStartPri = priHistogramPeaksSorted[0].rangeMinValue;
	slideNegEndPri = priHistogramPeaksSorted[0].rangeMaxValue;

// find mean slide rate
	int16_t bufferSize = diffToaBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) histogramedPulseCount);
	uint16_t endIndex = bufferSize;
	int64_t slideRateAvg = 0;
	int16_t avgSampleCount = 0;
	int32_t lastPri = diffToaBuffer[startIndex];
	int32_t minPri = lastPri, maxPri = lastPri;

	for (uint16_t i = startIndex + 1; i < endIndex; i++)
	{
		int32_t currPri = diffToaBuffer[i];
		int32_t diffPri = currPri - lastPri;
		if (diffPriHistogramPeaksSorted[0].rangeMinValue <= diffPri
				&& diffPri <= diffPriHistogramPeaksSorted[0].rangeMaxValue)
		{
			slideRateAvg += diffPri;
			avgSampleCount++;
		}
		lastPri = currPri;
		minPri = min(minPri, currPri);
		maxPri = max(maxPri, currPri);
	}

	if (avgSampleCount > 0)
	{
		slideNegRate = slideRateAvg / avgSampleCount;
	}
	else
	{
		slideNegRate = diffPriHistogramPeaksSorted[0].rangeMean;
	}
	if (minPri > slideNegStartPri)
		slideNegStartPri = minPri;
	if (maxPri < slideNegEndPri)
		slideNegEndPri = maxPri;
}
int8_t AnalysisTrackPriLowClass::SlideNegTracking(int32_t &diffToa,
		int32_t &diffPri)
{
// positive sawtooth pri type
	if (diffToa <= slideNegEndPri) // valid slide PRI
	{
		if (abs((int32_t) diffPri - slideNegRate) < 10) // 2nd difference should be equal to slide rate
		{ // successfully tracked
			slideNegStartPri = min(slideNegStartPri, (int32_t) diffToa); // update start PRI
			slideNegLockedPriValue = diffToa;
			return 1;
		}
		else if (abs((int32_t) diffToa - slideNegEndPri) < 10)
		{
			slideNegLockedPriValue = slideNegEndPri;
			return 1;
		}
	}
	else if (abs((int32_t) diffToa - slideNegEndPri) < 10)
	{
		slideNegLockedPriValue = slideNegEndPri;
		return 1;
	}
	return -1;
}

float AnalysisTrackPriLowClass::SequenceSearchSlideNeg(uint16_t pulseCount)
{
	int16_t bufferSize = diffToaBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;
	uint16_t foundSeqCount = 0;

	int32_t currLockedPriValue = slideNegLockedPriValue;

	int32_t lastPriValue = diffToaBuffer[startIndex];

	int16_t conseqLblSuccessCount = 0;
	for (uint16_t i = startIndex + 1; i < endIndex; i++)
	{
		int32_t currPriValue = diffToaBuffer[i];
		int32_t currDiffPriValue = currPriValue - lastPriValue;
//		if (currPriValue >= slidePosStartPri && currPriValue <= slidePosEndPri)
//		{
//		}

		if (abs(currDiffPriValue - slideNegRate) < 10)
		{
			conseqLblSuccessCount++;
			foundSeqCount++;
		}
		else
		{
			conseqLblSuccessCount = 0;
		}
		if (conseqLblSuccessCount > 2)
		{
			conseqLblSuccessCount = 2; // if next time slide rate not found body of this if will not be executed
			currLockedPriValue = currPriValue;
			slidePosLockedPriValueSampleIndex = samplesProcessed - bufferSize
					+ i;
			slideNegEndPri = max(slideNegEndPri, currPriValue);
			slideNegStartPri = min(slideNegStartPri, currPriValue);
		}

		lastPriValue = currPriValue;
	}
	slideNegLockedPriValue = currLockedPriValue;

	return ((float) foundSeqCount) / (pulseCount - 1);
}
void AnalysisTrackPriLowClass::UpdateSlideTriSpots()
{ // copy the results i.e. spots and their relevant information

	slideTriStartPri = priHistogramPeaksSorted[0].rangeMinValue;
	slideTriEndPri = priHistogramPeaksSorted[0].rangeMaxValue;
	for (uint8_t i = 0; i < priHistogramPeaksSorted.size(); i++)
	{
		slideTriStartPri = min(slideTriStartPri,
				priHistogramPeaksSorted[i].rangeMinValue);
		slideTriEndPri = max(slideTriEndPri,
				priHistogramPeaksSorted[i].rangeMaxValue);
	}

// find mean slide rate
	int16_t bufferSize = diffToaBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) histogramedPulseCount);
	uint16_t endIndex = bufferSize;
	int64_t slideRateAvg = 0;
	int16_t avgSampleCount = 0;
	int32_t lastPri = diffToaBuffer[startIndex];
	int32_t minPri = lastPri, maxPri = lastPri;

	for (uint16_t i = startIndex + 1; i < endIndex; i++)
	{
		int32_t currPri = diffToaBuffer[i];
		int32_t diffPri = currPri - lastPri;
		if (diffPriHistogramPeaksSorted[0].rangeMinValue <= diffPri
				&& diffPri <= diffPriHistogramPeaksSorted[0].rangeMaxValue)
		{
			slideRateAvg += abs(diffPri);
			avgSampleCount++;
		}
		else if (diffPriHistogramPeaksSorted[1].rangeMinValue <= diffPri
				&& diffPri <= diffPriHistogramPeaksSorted[1].rangeMaxValue)
		{
			slideRateAvg += abs(diffPri);
			avgSampleCount++;
		}
		lastPri = currPri;
		minPri = min(minPri, currPri);
		maxPri = max(maxPri, currPri);
	}

	if (avgSampleCount > 0)
	{
		slideTriRate = slideRateAvg / avgSampleCount;
	}
	else
	{
		slideTriRate = diffPriHistogramPeaksSorted[0].rangeMean;
	}
	if (minPri > slideTriStartPri)
		slideTriStartPri = minPri;
	else if ((slideTriStartPri - minPri) <= 5000)
		slideTriStartPri = minPri;
	if (maxPri < slideTriEndPri)
		slideTriEndPri = maxPri;
	else if ((maxPri - slideTriEndPri) <= 5000)
		slideTriEndPri = maxPri;
}
int8_t AnalysisTrackPriLowClass::SlideTriTracking(int32_t &diffToa,
		int32_t &diffPri)
{
	if (diffToa >= slideTriStartPri) // valid slide PRI
	{
		if (abs(abs(diffPri) - slideTriRate) < 10) // 2nd difference should be equal to slide rate
		{ // successfully tracked
			slideTriEndPri = max(slideTriEndPri, (int32_t) diffToa); // update end PRI
			slideTriLockedPriValue = diffToa;
			return 1;
		}
		else if (abs((int32_t) diffToa - slideTriStartPri) < 10)
		{
			slideTriLockedPriValue = slideTriStartPri;
			return 1;
		}
	}
	else if (abs((int32_t) diffToa - slideTriStartPri) < 10)
	{
		slideTriLockedPriValue = slideTriStartPri;
		return 1;
	}
// -ve sawtooth pri type
	if (diffToa <= slideTriEndPri) // valid slide PRI
	{
		if (abs((int32_t) diffPri - slideTriRate) < 10) // 2nd difference should be equal to slide rate
		{ // successfully tracked
			slideTriStartPri = min(slideTriStartPri, (int32_t) diffToa); // update start PRI
			slideTriLockedPriValue = diffToa;
			return 1;
		}
		else if (abs((int32_t) diffToa - slideTriEndPri) < 10)
		{
			slideTriLockedPriValue = slideTriEndPri;
			return 1;
		}
	}
	else if (abs((int32_t) diffToa - slideTriEndPri) < 10)
	{
		slideTriLockedPriValue = slideTriEndPri;
		return 1;
	}
	return -1;
}
float AnalysisTrackPriLowClass::SequenceSearchSlideTri(uint16_t pulseCount)
{
	int16_t bufferSize = diffToaBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;
	uint16_t foundSeqCount = 0, foundPosSeqCount = 0, foundNegSeqCount = 0;
	float posScore = 0, negScore = 0;
	int32_t currLockedPriValue = slideTriLockedPriValue;

	int32_t lastPriValue = diffToaBuffer[startIndex];

	int16_t conseqLblSuccessCount = 0;
	for (uint16_t i = startIndex + 1; i < endIndex; i++)
	{
		int32_t currPriValue = diffToaBuffer[i];
		int32_t currDiffPriValue = currPriValue - lastPriValue;
//		if (currPriValue >= slidePosStartPri && currPriValue <= slidePosEndPri)
//		{
//		}

		if (abs(currDiffPriValue - slideTriRate) < 10)
		{
			conseqLblSuccessCount++;
			foundNegSeqCount++;
		}
		else if (abs(currDiffPriValue + slideTriRate) < 10)
		{
			conseqLblSuccessCount++;
			foundPosSeqCount++;
		}
		else
		{
			conseqLblSuccessCount = 0;
		}
		if (conseqLblSuccessCount > 2)
		{
			conseqLblSuccessCount = 2; // if next time slide rate not found body of this if will not be executed
			currLockedPriValue = currPriValue;
			slideTriLockedPriValueSampleIndex = samplesProcessed - bufferSize
					+ i;
			slideTriEndPri = max(slideTriEndPri, currPriValue);
			slideTriStartPri = min(slideTriStartPri, currPriValue);
		}

		lastPriValue = currPriValue;
	}
	slideTriLockedPriValue = currLockedPriValue;

	posScore = (float) foundPosSeqCount / (pulseCount / 2);
	negScore = (float) foundNegSeqCount / (pulseCount / 2);

	if (posScore > 0.4 && negScore > 0.4)
	{
		foundSeqCount = foundPosSeqCount + foundNegSeqCount;
	}
	return ((float) foundSeqCount) / (pulseCount - 1);
}

float AnalysisTrackPriLowClass::CalcProbabilityOfSine(
		vector<PeakInInputData> &peaks)
{
	uint16_t peakCount = peaks.size();

	if (peakCount == 0 || peakCount > 32)
	{
		return 0;
	}
	FillPriConcurranceMatrix(histogramedPulseCount, peaks, false);

//float expectedPulsesPerPhase = histogramedPulseCount / peakCount;

// using peaks in concurrence matrix determine next spot to current spot (ignoring unknown indexes)
	int16_t maxOccurrence = priConcMatrix[1][1];
	for (uint16_t rowIndex = 1; rowIndex < peakCount + 1; rowIndex++) //first row and column in concurrence matrix is for unknown value
	{

		for (uint16_t colIndex = 1; colIndex < peakCount + 1; colIndex++)
		{
			if (priConcMatrix[rowIndex][colIndex] > maxOccurrence)
			{
				maxOccurrence = priConcMatrix[rowIndex][colIndex];
			}
		}
	}
	uint16_t expectedPulsePerSineCycle = (peakCount * 2) - 2;
	float sinePeakThreshold = histogramedPulseCount / expectedPulsePerSineCycle;
	sinePeakThreshold = max(2.0f, floor(sinePeakThreshold));

	float sineProbabilityAccumulated = 0;
	sineProbabilityAccumulated += min(1.0f,
			priConcMatrix[1][2] / sinePeakThreshold);
	sineProbabilityAccumulated += min(1.0f,
			priConcMatrix[peakCount - 1][peakCount] / sinePeakThreshold);
	for (uint16_t rowIndex = 2; rowIndex < peakCount; rowIndex++) //first row and column in concurrence matrix is for unknown value
	{
		sineProbabilityAccumulated += min(1.0f,
				priConcMatrix[rowIndex][rowIndex + 1] / sinePeakThreshold);
		sineProbabilityAccumulated += min(1.0f,
				priConcMatrix[rowIndex][rowIndex - 1] / sinePeakThreshold);
	}
	sineProbabilityAccumulated = sineProbabilityAccumulated
			/ expectedPulsePerSineCycle;
	return sineProbabilityAccumulated;
}
void AnalysisTrackPriLowClass::UpdateSlideSineSpots()
{ // copy the results i.e. spots and their relevant information
	int32_t maxSampleCount = priHistogramPeaksSorted[0].rangeSampleCount;

	priHistogramPeaksFiltered.resize(0);
	for (uint16_t i = 0; i < priHistogramPeaks.size(); i++)
	{
		if (priHistogramPeaks[i].rangeSampleCount > 0.3 * maxSampleCount)
			priHistogramPeaksFiltered.push_back(priHistogramPeaks[i]);
	}
	priHistogramPeaksSorted = priHistogramPeaksFiltered;

	FindMeanValuesOfPrisFromHistPeaks(histogramedPulseCount,
			priHistogramPeaksFiltered);

	sinePriSpots.resize(0);

	for (uint16_t i = 0; i < priHistogramPeaksFiltered.size(); i++)
	{
		sinePriSpots.push_back(tempSpotAverageValue[i]);

		int16_t spotDeviationAllowed = 0;
		float tempDevAllowed = (float) (tempSpotRange[i]);
		tempDevAllowed = tempDevAllowed * 0.0025; //0.25% deviation allowed
		spotDeviationAllowed = tempDevAllowed;
		spotDeviationAllowed = min((int16_t) 20, spotDeviationAllowed);
		spotDeviationAllowed = max((int16_t) 10, spotDeviationAllowed);

		sinePriSpotsDeviationAllowed.push_back(spotDeviationAllowed);
	}
	for (uint16_t i = priHistogramPeaksFiltered.size() - 2; i > 0; i--)
	{
		sinePriSpots.push_back(tempSpotAverageValue[i]);

		int16_t spotDeviationAllowed = 0;
		float tempDevAllowed = (float) (tempSpotRange[i]);
		tempDevAllowed = tempDevAllowed * 0.0025; //0.25% deviation allowed
		spotDeviationAllowed = tempDevAllowed;
		spotDeviationAllowed = min((int16_t) 20, spotDeviationAllowed);
		spotDeviationAllowed = max((int16_t) 10, spotDeviationAllowed);

		sinePriSpotsDeviationAllowed.push_back(spotDeviationAllowed);
	}
}
int8_t AnalysisTrackPriLowClass::SlideSineTracking(int32_t &diffToa,
		int32_t &diffPri)
{
	if (sineNextPredictedIndex == -1
			|| sineNextPredictedIndex > ((int16_t)sinePriSpots.size() - 1))
	{
		sineNextPredictedIndex = -1;
		return -1;
	}
	if (abs(diffToa - sinePriSpots[sineNextPredictedIndex])
			< sinePriSpotsDeviationAllowed[sineNextPredictedIndex])
	{
		sineLastLockedIndex = sineNextPredictedIndex;
		sineNextPredictedIndex = (sineNextPredictedIndex + 1)
				% sinePriSpots.size();
		return 1;
	}
	return -1;
}
float AnalysisTrackPriLowClass::SequenceSearchSlideSine(uint16_t pulseCount)
{
	int16_t bufferSize = diffToaBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;

	int32_t pulsesFound = 0;

	int16_t totalFoundCount = 0;
	int16_t consecutiveMissedCount = 0;

	int8_t currSpotLabel = -1;
//	int8_t prevSpotLabel = -1;

	int16_t lockedIndex = (sineLastLockedIndex == -1 ? 0 : sineLastLockedIndex);
	int16_t expectedPhase = (
			sineNextPredictedIndex == -1 ? 0 : sineNextPredictedIndex);

	int16_t searchStartPoint = lockedIndex;

	bool continueLoopTillValidPri = (sineLastLockedIndex == -1 ? true : false); // this will be true until a valid label is found

	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		int32_t currDiffToa = diffToaBuffer[i];

		for (uint16_t jj = 0; jj < sinePriSpots.size(); jj++)
		{
			uint16_t adjustedIndex = (jj + searchStartPoint)
					% sinePriSpots.size();
			if (abs(currDiffToa - sinePriSpots[adjustedIndex])
					< sinePriSpotsDeviationAllowed[adjustedIndex])
			{
				currSpotLabel = adjustedIndex;
				searchStartPoint = (adjustedIndex + 1) % sinePriSpots.size();
				break;
			}
			else
			{
				currSpotLabel = -1;
			}
		}
// this portion stalls the loop while lastLabel is below zero======
		if (continueLoopTillValidPri == true)
		{
			if (currSpotLabel == -1)
			{
				continue;
			}
			else
			{
				//when a valid label is found and a valid next expected spot exists
				if (currSpotLabel == expectedPhase)
				{ // continue with regular sequence search
					continueLoopTillValidPri = false;
				}
				else
				{
					expectedPhase = (currSpotLabel + 1) % sinePriSpots.size();
				}
			}
		}
// end of stall portion============================================

		if (currSpotLabel == expectedPhase)
		{ //predicted next label from last spot matches actual label
			seqSearchLockedToaBuffer.push_back(toaBuffer[i]);
			lockedIndex = currSpotLabel; //to save global state of current spot
			if (pulsesFound == 0)
			{
				totalFoundCount++;
			}
			totalFoundCount++;
			pulsesFound++;
			//			if (totalFoundCount == 1)
			//				firstLockedTOA = toaBuffer[i];
			consecutiveMissedCount = 0;
			expectedPhase = (currSpotLabel + 1) % sinePriSpots.size();
		}
		else if (currSpotLabel == -1) // missing pulses handling
		{
			consecutiveMissedCount++;
			if (consecutiveMissedCount > 2)
			{
				expectedPhase = -1;
			}
			else
			{
				expectedPhase = (expectedPhase + 1) % sinePriSpots.size();
			}
		}
		else
		{
			expectedPhase = (currSpotLabel + 1) % sinePriSpots.size();
		}
		if (consecutiveMissedCount >= 3)
		{
			consecutiveMissedCount = 0;
			pulsesFound = 0;
			continueLoopTillValidPri = true;
		}
	}
	sineLastLockedIndex = lockedIndex;
	sineNextPredictedIndex = expectedPhase;
	return ((float) totalFoundCount / (float) pulseCount);
}
void AnalysisTrackPriLowClass::filterdiffHistogramPeaks(float threshold)
{
	vector<PeakInInputData> orgPeaks;
	orgPeaks = diffPriHistogramPeaksSorted;
	diffPriHistogramPeaksSorted.resize(0);
	uint32_t maxSampleCount = 0;
	for (uint16_t i = 0; i < min(32, (int32_t) orgPeaks.size()); i++)
	{
		maxSampleCount = max(maxSampleCount, orgPeaks[i].rangeSampleCount);
	}

	for (uint16_t i = 0; i < min(32, (int32_t) orgPeaks.size()); i++)
	{
		if (orgPeaks[i].rangeSampleCount >= maxSampleCount * threshold)
		{
			diffPriHistogramPeaksSorted.push_back(orgPeaks[i]); // Filtered peaks
		}
	}
}
void AnalysisTrackPriLowClass::filterHistogramPeaks(uint32_t threshold)
{/* This function filter out the pri peaks that occurs in the pri buffer more
 * than the given threshold. For dwell, threshold is 8 and for jitter/difftoa it is 16.
 */

	/* // handling N multiples of dwell/jitter/complex spots due to missing pulses
	 // if 2nd multiple peaks exists, we could ignore them here
	 for (uint16_t i = 0; i < priHistogramPeaksSorted.size(); i++)
	 {
	 //if this peak is 2nd multiple of another peak ignore it
	 }
	 */
	priHistogramPeaksFiltered.resize(0);
	for (uint16_t i = 0; i < min(32, (int32_t) priHistogramPeaks.size()); i++)
	{
		if (priHistogramPeaks[i].rangeSampleCount >= threshold)
		{
			priHistogramPeaksFiltered.push_back(priHistogramPeaks[i]); // Filtered peaks
		}
	}
}
void AnalysisTrackPriLowClass::filterHistogramPeaksSorted(uint32_t threshold)
{/* This function filter out the pri peaks that occurs in the pri buffer more
 * than the given threshold. For dwell, threshold is 8 and for jitter/difftoa it is 16.
 */

// handling N multiples of dwell/jitter/complex spots due to missing pulses
// if 2nd multiple peaks exists, we could ignore them here
	for (uint16_t i = 0; i < priHistogramPeaksSorted.size(); i++)
	{
		//if this peak is 2nd multiple of another peak ignore it
	}

	priHistogramPeaksFiltered.resize(0);
	for (uint16_t i = 0; i < min(32, (int32_t) priHistogramPeaksSorted.size());
			i++)
	{
		if (priHistogramPeaksSorted[i].rangeSampleCount >= threshold)
		{
			priHistogramPeaksFiltered.push_back(priHistogramPeaksSorted[i]); // Filtered peaks
		}
	}
}
void AnalysisTrackPriLowClass::filterHistogramPeaksForDwell(uint32_t threshold)
{/* This function filter out the pri peaks that occurs in the pri buffer more
 * than the given threshold. For dwell, threshold is 8 and for jitter/difftoa it is 16.
 */
	uint8_t mergeSpotFlag[32];
	uint8_t nthMultipleExists = 0;
	memset(mergeSpotFlag, 0, sizeof(mergeSpotFlag));

// handling N multiples of dwell/jitter/complex spots due to missing pulses
// if 2nd multiple peaks exists, we could ignore them here
	for (uint16_t i = 0; i < priHistogramPeaks.size(); i++)
	{
		if (mergeSpotFlag[i] != 0)
		{
			continue;
		}
		int32_t referenceValue = tempSpotAverageValue[i];
		for (uint16_t j = i + 1; j < priHistogramPeaks.size(); j++)
		{
			if (mergeSpotFlag[j] == 0)
			{
				int32_t deviationAllowed = tempSpotRange[i];
				bool doesSpotMatch = findIfNthMultiple(tempSpotAverageValue[j],
						referenceValue, deviationAllowed);

				if (doesSpotMatch)
				{
					mergeSpotFlag[j] = 1;
					priHistogramPeaks[i].rangeSampleCount +=
							priHistogramPeaks[j].rangeSampleCount;
					nthMultipleExists = 1;
				}
			}
		}
	}
	vector<PeakInInputData> tempPeaks;
	for (uint16_t i = 0; i < priHistogramPeaks.size(); i++)
	{
		if (mergeSpotFlag[i] == 0)
		{
			tempPeaks.push_back(priHistogramPeaks[i]);
		}
	}

	priHistogramPeaksFiltered.resize(0);
	for (uint16_t i = 0; i < min(32, (int32_t) tempPeaks.size()); i++)
	{

		if (tempPeaks[i].rangeSampleCount >= threshold)
		{
			priHistogramPeaksFiltered.push_back(tempPeaks[i]); // Filtered peaks
		}
		else if (tempPeaks[i].rangeSampleCount == 6
				|| tempPeaks[i].rangeSampleCount == 7)
		{
			if (nthMultipleExists && tempPeaks.size() == 1)
			{
				tempPeaks[i].rangeSampleCount = 8;
				priHistogramPeaksFiltered.push_back(tempPeaks[i]);
			}
		}
	}
}

bool AnalysisTrackPriLowClass::findIfNthMultiple(int32_t queryValue,
		int32_t spotValue, int16_t devAllowed)
{
	devAllowed = max(3, 2 * devAllowed);
	if (queryValue < 0)
	{
		return false;
	}
	int32_t remainderValue = queryValue % spotValue;

	if (remainderValue <= (devAllowed))
	{
		return true;
	}
	else if (abs(remainderValue - spotValue) <= (devAllowed))
	{
		return true;
	}

	return false;

}
float AnalysisTrackPriLowClass::CalcDwellScoreFromConcMatrix()
{
	if (priHistogramPeaks.size() == 0)
	{
		return 0;
	}
//after finding peaks in histograms, filter some peaks before further processing
	FindMeanValuesOfPrisFromHistPeaks(histogramedPulseCount, priHistogramPeaks);
	filterHistogramPeaksForDwell(8); // pick histogram peaks that had occurred at least 8 times
	FindMeanValuesOfPrisFromHistPeaks(histogramedPulseCount,
			priHistogramPeaksFiltered);
	if (priHistogramPeaksFiltered.size() == 0
			|| priHistogramPeaksFiltered.size() > 32)
	{
		return 0;
	}
// Generate a concurrence matrix representing the sequence of pri peaks in pri buffer
	FillPriConcurranceMatrix(histogramedPulseCount, priHistogramPeaksFiltered,
			true);

	float sumofProbabilityOfDwell = 0;
	int32_t peakCount = priHistogramPeaksFiltered.size() + 1; // plus one as one spot is for unknown
	float probOfDwellSpots[33];
// calculate dwell score using sequence found in concurrence matrix
	for (int16_t j = 0; j < peakCount; j++) //columns
	{ //first row and column in concurrence matrix is for unknown value
		int32_t tempSum = 0;
		for (int16_t i = 0; i < peakCount; i++) // rows
		{
			tempSum += priConcMatrix[i][j]; // sum all rows of given column
		}
		if (tempSum > 0)
		{
			probOfDwellSpots[j] = priConcMatrix[j][j];
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
			/ (priHistogramPeaksFiltered.size());
	int8_t twoPhasesWithDouble = StaggerCheckDoublePeaks(
			priHistogramPeaksFiltered);

	if (twoPhasesWithDouble)
	{

		uint32_t minPriIndex = 0, maxPriIndex = 1;
		if (priHistogramPeaksFiltered[0].rangeMean
				< priHistogramPeaksFiltered[1].rangeMean)
		{
			minPriIndex = 0;
			maxPriIndex = 1;
		}
		else
		{
			minPriIndex = 1;
			maxPriIndex = 0;
		}
		int32_t tempSum = 0;
		tempSum = priConcMatrix[minPriIndex + 1][minPriIndex + 1]
				+ priConcMatrix[minPriIndex + 1][maxPriIndex + 1]
				+ priConcMatrix[maxPriIndex + 1][minPriIndex + 1];

		dwellScore = tempSum / histogramedPulseCount;
		if (dwellScore > 0.5)
		{
			priHistogramPeaksFiltered[0] =
					priHistogramPeaksFiltered[minPriIndex];
			priHistogramPeaksFiltered.resize(1);
		}

	}
	return dwellScore;
}
void AnalysisTrackPriLowClass::AddNewDwellSpot(int32_t spotValue,
		int16_t devAllowed)
{
	if (dwellSpots.size() >= 32)
	{
//find the oldest found spot and replace with it
		uint8_t foundIndex = 0;
		uint64_t tempVal = dwellSpotLastOccurrenceToa[foundIndex];
		for (uint16_t i = 0; i < dwellSpots.size(); i++)
		{
			if (tempVal > dwellSpotLastOccurrenceToa[i])
			{
				tempVal = dwellSpotLastOccurrenceToa[i];
				foundIndex = i;
			}
		}

		dwellSpots[foundIndex] = spotValue;
		dwellSpotsDeviationAllowed[foundIndex] = devAllowed;
//load default values
		dwellPulsesPerSpot[foundIndex] = 0;
		dwellSpotLastOccurrenceToa[foundIndex] = toaBuffer.back();
		currActiveDwellSpotIndex = foundIndex;
	}
	else
	{
		dwellSpots.push_back(spotValue);
		dwellSpotsDeviationAllowed.push_back(devAllowed);
		dwellPulsesPerSpot.push_back(0);
		dwellSpotLastOccurrenceToa.push_back(toaBuffer.back());
		currActiveDwellSpotIndex = dwellSpots.size() - 1;
	}
}
void AnalysisTrackPriLowClass::UpdateDwellSpots()
{ // copy the results i.e. spots and their relevant information

// expire old spots

// if new spots were found from histogramming
// we need to update them too

	for (uint16_t i = 0; i < priHistogramPeaksFiltered.size(); i++)
	{
		if (tempIgnoreSpotFlag[i] == 1)
			continue;

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
				else if (abs(remainderValue - dwellSpots[j])
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
int8_t AnalysisTrackPriLowClass::DwellTracking(int32_t &diffToa,
		int32_t &diffPri)
{
	if (currActiveDwellSpotIndex == -1
			|| currActiveDwellSpotIndex >= (int16_t) (dwellSpots.size())) // Not any valid dwell spot
	{
		return -1;
	}

	int32_t diff = diffToa;
	diff = abs(diff - dwellSpots[currActiveDwellSpotIndex]);
	if (diff < dwellSpotsDeviationAllowed[currActiveDwellSpotIndex])
	{
		dwellSpotLastOccurrenceToa[currActiveDwellSpotIndex] = toaBuffer.back();
		return 1;
	}
	else if (abs(diff - 2 * dwellSpots[currActiveDwellSpotIndex]) // 1 missing pulse
	< dwellSpotsDeviationAllowed[currActiveDwellSpotIndex])
	{
		dwellSpotLastOccurrenceToa[currActiveDwellSpotIndex] = toaBuffer.back();
		return 1;
	}
	else if (abs(diff - 3 * dwellSpots[currActiveDwellSpotIndex]) // 2 missing pulses
	< dwellSpotsDeviationAllowed[currActiveDwellSpotIndex])
	{
		dwellSpotLastOccurrenceToa[currActiveDwellSpotIndex] = toaBuffer.back();
		return 1;
	}
	return -1;
}
float AnalysisTrackPriLowClass::SequenceSearchDwell(uint16_t pulseCount)
{/* this function is once called when histogram peaks guess a dwell pattern
 * so when dwell pri is declared or a new dwell spot is added
 * This function confirms the pulse sequence present in pri buffer in accordance with detected pri i.e. dwell
 */ // 4405732
	int16_t bufferSize = diffToaBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;
	uint16_t totalFoundSequenceCount = 0;

	int16_t currLockedDwellSpotIndex = -1; //index is locked only once it occurs 3 times consecutively

	int16_t lastLabel = -1; //this is the label of last value
	int16_t currLabel = -1;
	int16_t consecLblFailureCount = 0;
	int16_t consecOtherSpotOccuranceCount = 0;
	int16_t currLockedSpotOccurrenceCount = 0;
	memset(tempIgnoreSpotFlag, 1, sizeof(tempIgnoreSpotFlag));

	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		int32_t currPri = diffToaBuffer[i];

//label the current pri using dwell spots
		currLabel = LabelPriUsingAverageValues(currPri);

		if (currLabel == -1)
		{
			consecOtherSpotOccuranceCount = 0;
			consecLblFailureCount++;

			if (currLockedSpotOccurrenceCount >= 8) // for last active spot pulseCount
			{
				totalFoundSequenceCount += currLockedSpotOccurrenceCount;
			}
			currLockedSpotOccurrenceCount = 0;
		}
		else if (currLabel == currLockedDwellSpotIndex) // same dwell spot continues
		{
			seqSearchLockedToaBuffer.push_back(toaBuffer[i]);
			currLockedSpotOccurrenceCount++;
			consecLblFailureCount = 0;
			consecOtherSpotOccuranceCount = 0;
//			if (firstTOAFound == 0 && foundSeqCount > 5)
//			{
//				firstLockedTOA = toaBuffer[i];
//				firstTOAFound = 1;
//			}
		}
		else if (lastLabel == currLabel) // a dwell spot other then current spot repeats
		{
			seqSearchLockedToaBuffer.push_back(toaBuffer[i]);
			consecOtherSpotOccuranceCount++;
			consecLblFailureCount = 0;
//			if (currLockedSpotOccurrenceCount >= 8) // for last active spot pulseCount
//			{
//				totalFoundSequenceCount += currLockedSpotOccurrenceCount;
//			}
			currLockedSpotOccurrenceCount = 0;
		}

		if (consecLblFailureCount > 2)
		{ // lock broke, pri spot switched
			consecOtherSpotOccuranceCount = 0;
			if (currLockedDwellSpotIndex != -1)
			{

			}

			currLockedDwellSpotIndex = -1;
		}
		if (consecOtherSpotOccuranceCount >= 2)
		{ // dwell spot locked
			currLockedDwellSpotIndex = currLabel;
			consecOtherSpotOccuranceCount++;
			currLockedSpotOccurrenceCount = consecOtherSpotOccuranceCount;
			consecLblFailureCount = 0;
		}
		if (currLockedSpotOccurrenceCount >= 8) // one iter
		{
			if (currLockedDwellSpotIndex != -1)
			{
				tempIgnoreSpotFlag[currLockedDwellSpotIndex] = 0;
			}
		}
		lastLabel = currLabel;

	}
	if (currLockedSpotOccurrenceCount >= 8) // for last active spot pulseCount
	{
		totalFoundSequenceCount += currLockedSpotOccurrenceCount;
	}
	currActiveDwellSpotIndex = currLockedDwellSpotIndex;

	return ((float) totalFoundSequenceCount) / pulseCount;
}
float AnalysisTrackPriLowClass::RelockDwellPri(uint16_t pulseCount)
{/* this function is once called when histogram peaks guess a dwell pattern
 * so when dwell pri is declared or a new dwell spot is added
 * This function confirms the pulse sequence present in pri buffer in accordance with detected pri i.e. dwell
 */ // 4405732
	int16_t bufferSize = diffToaBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;
	uint16_t foundSeqCount = 0;

	int16_t currLockedDwellSpotIndex = currActiveDwellSpotIndex; //index is locked only once it occurs 3 times consecutively

	int16_t lastLabel = -1; //this is the label of last value
	int16_t currLabel = -1;
	int16_t consecLblFailureCount = 0;
	int16_t consecOtherSpotOccuranceCount = 0;
	if(currActiveDwellSpotIndex >= (int16_t) dwellSpots.size())
	{
		currActiveDwellSpotIndex = -1;
		return 0;
	}

	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		int32_t currPri = diffToaBuffer[i];

//label the current pri using dwell spots
		currLabel = LabelAccordingToPriType(SequenceTypesEnum::seq_dwell,
				currPri);

// in case of 1 missing pulse pri would be doubled and if that is the case, we can assign last label to curr label
		if (currLabel == -1 && lastLabel != -1
				&& abs(currPri - 2 * dwellSpots[lastLabel])
						<= 2 * dwellSpotsDeviationAllowed[lastLabel])
		{
			currLabel = lastLabel;
		}

		if (currLabel == -1)
		{
			consecOtherSpotOccuranceCount = 0;
			consecLblFailureCount++;
		}
		else if (currLabel == currLockedDwellSpotIndex) // same dwell spot continues
		{
			//dwellSpotLastOccurrenceToa[currLockedDwellSpotIndex] =
			//		samplesProcessed - bufferSize + i;
			
			dwellSpotLastOccurrenceToa[currLockedDwellSpotIndex] =
					toaBuffer[i];
			consecLblFailureCount = 0;
			consecOtherSpotOccuranceCount = 0;
			foundSeqCount++;
//			if (firstTOAFound == 0 && foundSeqCount > 5)
//			{
//				firstLockedTOA = toaBuffer[i];
//				firstTOAFound = 1;
//			}
		}
		else if (lastLabel == currLabel)  // dwell spot still continues
		{
			consecOtherSpotOccuranceCount++;
			consecLblFailureCount = 0;
			foundSeqCount++;
		}

		if (consecLblFailureCount > 2)
		{ // lock broke, pri spot switched
			consecOtherSpotOccuranceCount = 0;
			if (currLockedDwellSpotIndex != -1)
			{
				dwellSpotLastOccurrenceToa[currLockedDwellSpotIndex] =
						toaBuffer[i];
			}

			currLockedDwellSpotIndex = -1;
		}
		if (consecOtherSpotOccuranceCount >= 2)
		{ // dwell spot locked
			currLockedDwellSpotIndex = currLabel;
			dwellSpotLastOccurrenceToa[currLockedDwellSpotIndex] = toaBuffer[i];
			consecOtherSpotOccuranceCount++;
			consecLblFailureCount = 0;
		}
		lastLabel = currLabel;

	}
	currActiveDwellSpotIndex = currLockedDwellSpotIndex;

	return ((float) foundSeqCount) / pulseCount;
}
void AnalysisTrackPriLowClass::ExpireOldDwellSpots()
{
	if (dwellSpots.size() > 0)
	{
		uint64_t lastPulseToa = toaBuffer.back();

		int8_t expireFlag[32];
		memset(expireFlag, 0, sizeof(expireFlag));
		//TODO gmd dwell spot value was coming zero (when multiple same toas in input)

		for (uint16_t i = 0; i < dwellSpots.size(); i++)
		{
//			cout << lastPulseToa - dwellSpotLastOccurrenceToa[i] << endl;
			if ((int32_t)(lastPulseToa - dwellSpotLastOccurrenceToa[i])
					> 200 * dwellSpots[i])
			{
				expireFlag[i] = 1;
			}
		}
		uint16_t newValidSpotCount = 0;
		for (uint16_t i = 0; i < dwellSpots.size(); i++)
		{
			if (expireFlag[i] == 0)
			{
				if (i == currActiveDwellSpotIndex)
				{
					currActiveDwellSpotIndex = newValidSpotCount;
				}
				dwellSpots[newValidSpotCount] = dwellSpots[i];
				dwellSpotsDeviationAllowed[newValidSpotCount] =
						dwellSpotsDeviationAllowed[i];
				dwellPulsesPerSpot[newValidSpotCount] = dwellPulsesPerSpot[i];
				dwellSpotLastOccurrenceToa[newValidSpotCount] =
						dwellSpotLastOccurrenceToa[i];
				newValidSpotCount++;
			}
			else
			{

				if (i == currActiveDwellSpotIndex)
				{
					currActiveDwellSpotIndex = -1;
				}
			}
		}
		if (newValidSpotCount == 0)
		{
			currActiveDwellSpotIndex = -1;
		}
		else if (newValidSpotCount < dwellSpots.size())
		{
			newResultsAvailableForCopyFlag = 1;
			lastAnalysisWasSuccessfulFlag = 1;
		}
		dwellSpots.resize(newValidSpotCount);
		dwellSpotsDeviationAllowed.resize(newValidSpotCount);
		dwellPulsesPerSpot.resize(newValidSpotCount);
		dwellSpotLastOccurrenceToa.resize(newValidSpotCount);
	}

}
void AnalysisTrackPriLowClass::UpdateDiffToaSpots()
{ // copy the results i.e. spots and their relevant information
	int8_t alreadyPresentFlag = 0;
	for (uint16_t i = 0; i < priHistogramPeaksFiltered.size(); i++)
	{
		float tempDevAllowed = (float) (tempSpotAverageValue[i]);
		tempDevAllowed = tempDevAllowed * 0.0025; //0.25% deviation allowed

		for (uint16_t j = 0; j < priSpots.size(); j++)
		{
			int32_t diff = tempSpotAverageValue[i] - priSpots[j];
			if (abs(diff) <= tempDevAllowed)
			{
				alreadyPresentFlag = 1;
				break;
			}
		}
		if (alreadyPresentFlag == 0)
		{
			if (priSpots.size() >= 32)
			{
				uint8_t foundIndex = 0;
				priSpots[foundIndex] = tempSpotAverageValue[i];
				priSpotsDevAllowed[foundIndex] = tempDevAllowed;
				priRangeLow[foundIndex] = tempSpotMinValue[i];
				priRangeHigh[foundIndex] = tempSpotMaxValue[i];
				//load default values

				priSpotsLastOccurrance[foundIndex] = samplesProcessed;
			}
			else
			{
				priSpots.push_back(tempSpotAverageValue[i]);
				priRangeHigh.push_back(tempSpotMaxValue[i]);
				priRangeLow.push_back(tempSpotMinValue[i]);
				priSpotsDevAllowed.push_back(tempDevAllowed);
				priSpotsLastOccurrance.push_back(samplesProcessed);
			}
		}
	}

}
void AnalysisTrackPriLowClass::MergeSpots(float threshold)
{ // merge jitter/complex spots if extreme spots are coming within their variation limits but erroneously divided due to histogram resolution boundaries
	int32_t mergingIndex = 0, updatedMean = 0, updatedSampleCount = 0,
			updatedMaxRange = 0, updatedMinRange = 0, referenceIndex = 0;
	vector<PeakInInputData> priHistogramPeaksFilteredCopy;
	/* Make a copy first */
	priHistogramPeaksFilteredCopy = priHistogramPeaksFiltered;

	while (referenceIndex < ((int32_t) priHistogramPeaksFiltered.size()) - 1)
	{
		for (int16_t i = priHistogramPeaksFiltered.size() - 1;
				i > referenceIndex; i--)
		{
			if (abs(
					priHistogramPeaksFiltered[referenceIndex].rangeMean
							- priHistogramPeaksFiltered[i].rangeMean)
					<= threshold
							* priHistogramPeaksFiltered[referenceIndex].rangeMean)
			{
				mergingIndex = i; // start merging from 0 up until this index
				break;
			}
		}
		updatedMaxRange =
				priHistogramPeaksFiltered[referenceIndex].rangeMaxValue;
		updatedMinRange =
				priHistogramPeaksFiltered[referenceIndex].rangeMinValue;
		if (mergingIndex > referenceIndex
				&& mergingIndex >= (int32_t)priHistogramPeaksFiltered.size())
		{
			mergingIndex = priHistogramPeaksFiltered.size() - 1;
		}
		if (mergingIndex > referenceIndex)
		{ // recalculate mean values, range and sample count
			for (uint16_t i = referenceIndex; i <= mergingIndex; i++)
			{
				updatedMean += priHistogramPeaksFiltered[i].rangeMean
						* priHistogramPeaksFiltered[i].rangeSampleCount;
				updatedSampleCount +=
						priHistogramPeaksFiltered[i].rangeSampleCount;
				updatedMaxRange = max(updatedMaxRange,
						priHistogramPeaksFiltered[i].rangeMaxValue);
				updatedMinRange = min(updatedMinRange,
						priHistogramPeaksFiltered[i].rangeMinValue);
			}
			updatedMean = (int32_t) (updatedMean / updatedSampleCount);
			if (priHistogramPeaksFiltered[mergingIndex].rangeMaxValue
					> priHistogramPeaksFiltered[mergingIndex].rangeMaxValue)

				priHistogramPeaksFilteredCopy[referenceIndex].rangeMean =
						updatedMean;
			priHistogramPeaksFilteredCopy[referenceIndex].rangeSampleCount =
					updatedSampleCount;
			priHistogramPeaksFilteredCopy[referenceIndex].rangeMaxValue =
					updatedMaxRange;
			priHistogramPeaksFilteredCopy[referenceIndex].rangeMinValue =
					updatedMinRange;

			/* Rearrange filtered peaks */
			priHistogramPeaksFiltered.resize(referenceIndex);
			priHistogramPeaksFiltered.push_back(
					priHistogramPeaksFilteredCopy[referenceIndex]);

			for (uint16_t i = mergingIndex + 1;
					i < priHistogramPeaksFilteredCopy.size(); i++)
			{
				priHistogramPeaksFiltered.push_back(
						priHistogramPeaksFilteredCopy[i]);
			}
			priHistogramPeaksFilteredCopy = priHistogramPeaksFiltered;
		}
		referenceIndex = referenceIndex + 1;

	}
}
void AnalysisTrackPriLowClass::filterPeaksForJitterComplex(uint32_t threshold)
{
	uint16_t lengthOfMatrix = priHistogramPeaksFiltered.size() + 1;
	vector<PeakInInputData> priHistogramPeaksFilteredCopy =
			priHistogramPeaksFiltered;
	priHistogramPeaksFiltered.resize(0);

	uint32_t peakStrength = 0;
	for (uint16_t i = 1; i < lengthOfMatrix; i++)
	{
		bool multiplePossible = false;
		if (i + 1 < lengthOfMatrix)
		{
			int32_t doubleMean = 2
					* priHistogramPeaksFilteredCopy[i - 1].rangeMean;
			if ((priHistogramPeaksFilteredCopy[i].rangeMinValue < doubleMean)
					&& (doubleMean
							< priHistogramPeaksFilteredCopy[i].rangeMaxValue))
			{
				multiplePossible = true;
			}
		}

		if (multiplePossible == true)
		{
			peakStrength = priConcMatrix[i][i] + priConcMatrix[i + 1][i]
					+ priConcMatrix[i][i + 1];
		}
		else
		{
			peakStrength = priConcMatrix[i][i];
		}
		if (peakStrength >= threshold)
		{
			priHistogramPeaksFiltered.push_back(
					priHistogramPeaksFilteredCopy[i - 1]); // Filtered peaks
		}
	}

}
void AnalysisTrackPriLowClass::UpdateJitterSpots()
{ // copy the results i.e. spots and their relevant information
	int8_t alreadyPresentFlag = 0;
// copy the peaks
	for (uint16_t i = 0; i < priHistogramPeaksFiltered.size(); i++)
	{
		if (tempSpotWithinRangeFlag[i] == 1)
		{
			float tempDevAllowed = (float) (tempSpotAverageValue[i]);
			tempDevAllowed = tempDevAllowed * 0.15; //15% deviation allowed

			for (uint16_t j = 0; j < priSpots.size(); j++)
			{
				int32_t diff = tempSpotAverageValue[i] - priSpots[j];
				if (abs(diff) <= tempDevAllowed)
				{
					alreadyPresentFlag = 1;
					break;
				}
			}
			if (alreadyPresentFlag == 0)
			{
				if (priSpots.size() >= 32)
				{
					uint8_t foundIndex = 0;
					priSpots[foundIndex] = tempSpotAverageValue[i];
					priSpotsDevAllowed[foundIndex] = tempDevAllowed;
					priRangeLow[foundIndex] = tempSpotMinValue[i];
					priRangeHigh[foundIndex] = tempSpotMaxValue[i];

					//load default values

					priSpotsLastOccurrance[foundIndex] = samplesProcessed;
				}
				else
				{
					priSpots.push_back(tempSpotAverageValue[i]);
					priRangeHigh.push_back(tempSpotMaxValue[i]);
					priRangeLow.push_back(tempSpotMinValue[i]);
					priSpotsDevAllowed.push_back(tempDevAllowed);
					priSpotsLastOccurrance.push_back(samplesProcessed);
				}
			}
		}
		else
			continue;
	}

}
int8_t AnalysisTrackPriLowClass::JitterTracking(int32_t &diffToa,
		int32_t &diffPri)
{
	int32_t diff = diffToa;
	for (int16_t i = 0; i < (int16_t) priSpots.size(); i++)
	{
		if (abs(diff - priSpots[i]) < priSpotsDevAllowed[i])
		{ // falls within 15% of pri mean value
			return 1;
		}
	}
	return -1;
}
void AnalysisTrackPriLowClass::UpdateComplexSpots()
{ // copy the results i.e. spots and their relevant information
	int8_t alreadyPresentFlag = 0;
//	uint16_t i = 0;
// copy the peaks

	for (uint16_t i = 0; i < priHistogramPeaksFiltered.size(); i++)
	{
		float tempDevAllowed = (float) (tempSpotAverageValue[i]);
		tempDevAllowed = tempDevAllowed * 0.3; //15% deviation allowed

		for (uint16_t j = 0; j < priSpots.size(); j++)
		{
			int32_t diff = tempSpotAverageValue[i] - priSpots[j];
			if (abs(diff) <= tempDevAllowed)
			{
				alreadyPresentFlag = 1;
				break;
			}
		}
		if (alreadyPresentFlag == 0)
		{
			if (priSpots.size() >= 32)
			{
				uint8_t foundIndex = 0;
				priSpots[foundIndex] = tempSpotAverageValue[i];
				priSpotsDevAllowed[foundIndex] = tempDevAllowed;
				priRangeLow[foundIndex] = tempSpotMinValue[i];
				priRangeHigh[foundIndex] = tempSpotMaxValue[i];
				//load default values

				priSpotsLastOccurrance[foundIndex] = samplesProcessed;
			}
			else
			{
				priSpots.push_back(tempSpotAverageValue[i]);
				priRangeHigh.push_back(tempSpotMaxValue[i]);
				priRangeLow.push_back(tempSpotMinValue[i]);
				priSpotsDevAllowed.push_back(tempDevAllowed);
				priSpotsLastOccurrance.push_back(samplesProcessed);
			}
		}

	}

}

SequenceTypesEnum AnalysisTrackPriLowClass::PriAnalysisNew()
{ /* Pri Analysis */
	SequenceTypesEnum sequenceTypeGuess;
	SequenceTypesEnum sequenceTypeGuessFromSlide;
	SequenceTypesEnum sequenceTypeSequenceSearch =
			SequenceTypesEnum::seq_unknown;
 //Find pri type using TOA 1st differences
	int64_t PRIarr[256];
	int64_t PRIarr2[256];
	int16_t bufferSize = toaBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) histogramedPulseCount);
	uint16_t endIndex = bufferSize;
	for (uint16_t i = startIndex, k = 0; i < endIndex; i++, k++) // DEBUGGING ONLY
	{
		PRIarr2[k] = toaBuffer[i];
		PRIarr[k] = diffToaBuffer[i];

	}

	FindPeaksInPriHistogram();
	FindPeaksInDiffPriHistogram();

	sequenceTypeGuessFromSlide = guessPriTypeUsingDiffPriHistogram();

//	if (sequenceTypeGuess == SequenceTypesEnum::seq_unknown)
//	{ // Find pri type using TOA 2nd differences only if not found already
	sequenceTypeGuess = guessPriTypeUsingPriHistogram();
//	}
	if (sequenceTypeGuessFromSlide != SequenceTypesEnum::seq_unknown)
	{
		if (sequenceTypeGuess == SequenceTypesEnum::seq_stagger)
		{
			if(sequenceTypeGuessFromSlide == SequenceTypesEnum::seq_slideSine)
			{
				sequenceTypeGuess = sequenceTypeGuessFromSlide;
			}
		}
		else if (histogramedPulseCount >= 64)
		{
			sequenceTypeGuess = sequenceTypeGuessFromSlide;
		}
	}
	seqSearchLockedToaBuffer.resize(0);

	float dwellSeqSearchScore = 0, staggerSeqSearchScore = 0;
	switch (sequenceTypeGuess)
	{
	case SequenceTypesEnum::seq_dwell:
	{
		dwellSeqSearchScore = SequenceSearchDwell(histogramedPulseCount);
		if (dwellSeqSearchScore > 0.7)
		{
			// sequence search successful on 70% of pulses in pri/toa buffer
			UpdateDwellSpots();
			sequenceTypeSequenceSearch = SequenceTypesEnum::seq_dwell;
		}
		break;
	}
	case SequenceTypesEnum::seq_stagger:
	{

		UpdateStaggerSpots();
		StaggerComputeFrame(histogramedPulseCount);

		if (abs(staggerFrameInferred - staggerFrameActual) < 100)
		{
			staggerSeqSearchScore = SequenceSearchStagger_V1(
					histogramedPulseCount);
			if (staggerSeqSearchScore > 0.7)
			{
				// sequence search successful on 70% of pulses in pri/toa buffer
				sequenceTypeSequenceSearch = SequenceTypesEnum::seq_stagger;
			}
		}
		else if(histogramedPulseCount >= 64 && priHistogramPeaksFiltered.size() > 0)
		{
			ResetPriInfoVectors();
			UpdateComplexSpots();
			sequenceTypeSequenceSearch = SequenceTypesEnum::seq_complex;
			break;
		}
		else if (histogramedPulseCount >= 8 * priHistogramPeaksFiltered.size()
				&& histogramedPulseCount >= 256)
		{ // if sequence search stagger fails, it is declared as difftoa pri but the pulses criteria is stringent
			UpdateDiffToaSpots();
			sequenceTypeSequenceSearch = SequenceTypesEnum::seq_difftoa;
		}
		break;
	}
	case SequenceTypesEnum::seq_difftoa:
	{
		if (sequenceTypeGuess != currPriType
				&& (sequenceTypeGuess != SequenceTypesEnum::seq_unknown
						&& currPriType != SequenceTypesEnum::seq_unknown))
		{
			ResetPriInfoVectors();
		}
		UpdateDiffToaSpots();
		sequenceTypeSequenceSearch = SequenceTypesEnum::seq_difftoa;

		break;
	}
	case SequenceTypesEnum::seq_slidePos:
	{
		UpdateSlidePosSpots();
		float slideSeqSearchScore = SequenceSearchSlidePos(
				histogramedPulseCount);
		if (slideSeqSearchScore > 0.7)
		{
			sequenceTypeSequenceSearch = SequenceTypesEnum::seq_slidePos;
		}
		break;
	}
	case SequenceTypesEnum::seq_slideNeg:
	{
		UpdateSlideNegSpots();
		float slideSeqSearchScore = SequenceSearchSlideNeg(
				histogramedPulseCount);
		if (slideSeqSearchScore > 0.7)
		{
			sequenceTypeSequenceSearch = SequenceTypesEnum::seq_slideNeg;
		}
		break;
	}
	case SequenceTypesEnum::seq_slideTri:
	{
		UpdateSlideTriSpots();
		float slideSeqSearchScore = SequenceSearchSlideTri(
				histogramedPulseCount);
		if (slideSeqSearchScore > 0.7)
		{
			sequenceTypeSequenceSearch = SequenceTypesEnum::seq_slideTri;
		}
		break;
	}
	case SequenceTypesEnum::seq_slideSine:
	{

		UpdateSlideSineSpots();
		sineLastLockedIndex = -1;
		sineNextPredictedIndex = -1;
		float slideSeqSearchScore = SequenceSearchSlideSine(
				histogramedPulseCount);
		if (slideSeqSearchScore > 0.7)
		{
			sequenceTypeSequenceSearch = SequenceTypesEnum::seq_slideSine;
		}
		break;
	}
	case SequenceTypesEnum::seq_jitter:
	{
		if (sequenceTypeGuess != currPriType
				&& (sequenceTypeGuess != SequenceTypesEnum::seq_unknown
						&& currPriType != SequenceTypesEnum::seq_unknown))
			ResetPriInfoVectors();
		UpdateJitterSpots();
		sequenceTypeSequenceSearch = SequenceTypesEnum::seq_jitter;
		break;
	}
	case SequenceTypesEnum::seq_complex:
	{
		if (sequenceTypeGuess != currPriType
				&& (sequenceTypeGuess != SequenceTypesEnum::seq_unknown
						&& currPriType != SequenceTypesEnum::seq_unknown))
			ResetPriInfoVectors();
		UpdateComplexSpots();
		sequenceTypeSequenceSearch = SequenceTypesEnum::seq_complex;
		break;
	}
	default:

		break;
	}
	FindMaxPri(sequenceTypeSequenceSearch);
	return sequenceTypeSequenceSearch;

}
SequenceTypesEnum AnalysisTrackPriLowClass::guessPriTypeUsingPriHistogram()
{
	bool dwellSearch = true;
	bool jitterOrComplexSearch = false;
	float dwellScore = 0;
	float staggerScore = 0;
	int8_t noOfPeaks = 0;
	int8_t peaksWithinRange = 0;
	SequenceTypesEnum sequenceTypeGuess = SequenceTypesEnum::seq_unknown;

//FindPeaksInPriHistogram(); // Analyze histograms to find valid pri peaks
	noOfPeaks = priHistogramPeaksSorted.size();
	/* Make initial guess of pri type using TOA 1st differences */
	if (priHistogram.getRangePrecisionLevel() == 0)
	{	// if operating level is zero, dwell and stagger is expected
		dwellSearch = true;
	}
	else
	{
		dwellSearch = false;
		jitterOrComplexSearch = true;
		filterHistogramPeaks(8);
	}

	if (dwellSearch == true)
	{ // score the initial guess according to pattern found in pri buffer
		dwellScore = CalcDwellScoreFromConcMatrix();
		noOfPeaks = priHistogramPeaksFiltered.size();
		if (dwellScore > 0.5)	// dwell pri found
		{
			FindMeanValuesOfPrisFromHistPeaks(histogramedPulseCount,
					priHistogramPeaksFiltered); // find average values of filtered peaks and their ranges
			sequenceTypeGuess = SequenceTypesEnum::seq_dwellOrjitterOrcomplex; // could be jitter/complex too depending on spot variation
		}
		else	// dwell pattern not found, search for stagger now
		{
			filterHistogramPeaksForStagger();
			staggerScore = CalcStaggerScoreFromConcMatrix(); // score the initial guess according to pattern found in pri buffer
			noOfPeaks = priHistogramPeaksFiltered.size();
			if (staggerScore > 0.5)
			{
				if (noOfPeaks > 1) // minimum stagger levels are 2
				{
					FindMeanValuesOfPrisFromHistPeaks(histogramedPulseCount,
							priHistogramPeaksFiltered); // find average values of filtered peaks and their ranges
					sequenceTypeGuess = SequenceTypesEnum::seq_stagger;
				}
			}
			else
			{ // most probably difftoa pri or jitter/complex if variation exceeds fixed levels

				filterHistogramPeaks(8); // filtering criteria for difftoa/jitter is stringent
				FindMeanValuesOfPrisFromHistPeaks(histogramedPulseCount,
						priHistogramPeaksFiltered); // since jitter/complex hence mean and variation of spots are needed
				noOfPeaks = priHistogramPeaksFiltered.size(); // no. of spots repeated atleast 16 times
				peaksWithinRange = CheckifRangeWithinLimits(0.0025);
				if (peaksWithinRange >= 0.8 * priHistogramPeaksFiltered.size()) // 60% peaks are within fixed range
				{
					if (noOfPeaks > 0) // minimum 2 spots/pris required for a valid difftoa pri
					{
//						uint16_t pulsesFound =0;
//						for (uint8_t i =0; i< priHistogramPeaksFiltered.size();i++)
//						{
//							pulsesFound += priHistogramPeaksFiltered[i].rangeSampleCount;
//						}
//						if(pulsesFound > 0.7*histogramedPulseCount)
//						sequenceTypeGuess = SequenceTypesEnum::seq_difftoa;
						sequenceTypeGuess = SequenceTypesEnum::seq_complex;
					}
				}
				else if (histogramedPulseCount >= 64) // jitter/complex shouldn't be declared too soon, acquire at least 64 pulses
				{
					peaksWithinRange = CheckifRangeWithinLimits(0.15);
					if (peaksWithinRange
							>= 0.9 * priHistogramPeaksFiltered.size()) // 90% peaks are within jitter range
						sequenceTypeGuess = SequenceTypesEnum::seq_jitter;
					else
					{
						peaksWithinRange = CheckifRangeWithinLimits(0.3);
						if (peaksWithinRange
								>= 0.9 * priHistogramPeaksFiltered.size()) //90% peaks are within complex range
							sequenceTypeGuess = SequenceTypesEnum::seq_complex;
					}
				}
			}
		}
	}

	noOfPeaks = priHistogramPeaksFiltered.size();

	if (noOfPeaks > 0
			&& sequenceTypeGuess
					== SequenceTypesEnum::seq_dwellOrjitterOrcomplex)
	{
		if (tempSpotRange[0] <= ceil(0.0025 * tempSpotAverageValue[0])
				&& tempSpotRange[0] <= 100) // within +- 0.25% or +-500ns
			sequenceTypeGuess = SequenceTypesEnum::seq_dwell;
//		else if (tempSpotStdDevValue[0] < 0.0025 * tempSpotAverageValue[0])
//		{
//			tempSpotRange[0] = 0.005 * tempSpotAverageValue[0];
//			tempSpotMaxValue[0] = tempSpotAverageValue[0]
//					+ tempSpotRange[0] / 2;
//			tempSpotMinValue[0] = tempSpotAverageValue[0]
//					- tempSpotRange[0] / 2;
//			sequenceTypeGuess = SequenceTypesEnum::seq_dwell;
//		}
		else if (histogramedPulseCount >= 64)
		{
			if (tempSpotRange[0] <= 0.3 * tempSpotAverageValue[0]) // within +-15%
			{
				tempSpotWithinRangeFlag[0] = 1;
				sequenceTypeGuess = SequenceTypesEnum::seq_jitter;
			}
			else if (tempSpotRange[0] <= 0.6 * tempSpotAverageValue[0]) // more than +-15% but within +-30%
				sequenceTypeGuess = SequenceTypesEnum::seq_complex;
		}
	}

	if (jitterOrComplexSearch == true)
	{
		if (histogramedPulseCount >= 64) // jitter/complex shouldn't be declared too soon, acquire at least 64 pulses
		{
			filterHistogramPeaks(8); // level 1 histogram, jitter/complex filter relaxed to 8
			FindMeanValuesOfPrisFromHistPeaks(histogramedPulseCount,
					priHistogramPeaksFiltered); // find average values of filtered peaks and their ranges
			noOfPeaks = priHistogramPeaksFiltered.size();
			if (noOfPeaks == 0)
			{

			}
			if (tempSpotRange[0] <= 0.3 * tempSpotAverageValue[0]) // within +-15%
			{
				tempSpotWithinRangeFlag[0] = 1;
				sequenceTypeGuess = SequenceTypesEnum::seq_jitter;
			}
			else if (tempSpotRange[0] <= 0.6 * tempSpotAverageValue[0]) // more than +-15% but within +-30%
				sequenceTypeGuess = SequenceTypesEnum::seq_complex;

		}
	}
	if (sequenceTypeGuess == SequenceTypesEnum::seq_jitter)
	{
		MergeSpots(0.15);	// merge jitter spots if coming within +-15%
		FindMeanValuesOfPrisFromHistPeaks(histogramedPulseCount,
				priHistogramPeaksFiltered);	// since filtered peaks are updated, again recalculate averge values and ranges
		peaksWithinRange = CheckifRangeWithinLimits(0.3); // ranges are updated, see if still falling under jitter or not
		if (peaksWithinRange >= 0.9 * priHistogramPeaksFiltered.size()) // 90% peaks are within jitter range
			sequenceTypeGuess = SequenceTypesEnum::seq_jitter;
		else
		{ // are actually outside jitter range
			peaksWithinRange = CheckifRangeWithinLimits(0.6);
			if (peaksWithinRange >= 0.9 * priHistogramPeaksFiltered.size()) //90% peaks are within complex range
				sequenceTypeGuess = SequenceTypesEnum::seq_complex;
		}
	}
	else if (sequenceTypeGuess == SequenceTypesEnum::seq_complex)
	{
		MergeSpots(0.3);	// merge jitter spots if coming within +-15%
		FindMeanValuesOfPrisFromHistPeaks(histogramedPulseCount,
				priHistogramPeaksFiltered);	// since filtered peaks are updated, again recalculate averge values and ranges
		peaksWithinRange = CheckifRangeWithinLimits(0.3); // ranges are updated, see if still falling under jitter or not
		if (peaksWithinRange >= 0.9 * priHistogramPeaksFiltered.size()) // 90% peaks are within jitter range
			sequenceTypeGuess = SequenceTypesEnum::seq_complex;
		else
		{
			sequenceTypeGuess = SequenceTypesEnum::seq_unknown;
		}
	}

	if (sequenceTypeGuess == SequenceTypesEnum::seq_jitter
			|| sequenceTypeGuess == SequenceTypesEnum::seq_complex)
	{
		//filterHistogramPeaks(16);
		FillPriConcurranceMatrix(histogramedPulseCount,
				priHistogramPeaksFiltered, false);
		filterPeaksForJitterComplex(0.6 * histogramedPulseCount);
		FindMeanValuesOfPrisFromHistPeaks(histogramedPulseCount,
				priHistogramPeaksFiltered);
		if (priHistogramPeaksFiltered.size() == 0)
			sequenceTypeGuess = SequenceTypesEnum::seq_unknown;
	}
	if (sequenceTypeGuess == SequenceTypesEnum::seq_stagger
			|| sequenceTypeGuess == SequenceTypesEnum::seq_difftoa)
	{ // A special check for stagger, 2 level stagger cannot have phases multiple of each other
		int8_t twoPhasesWithDouble = StaggerCheckDoublePeaks(
				priHistogramPeaksFiltered);
		if (twoPhasesWithDouble)
		{
			if (histogramedPulseCount > 64) // collect more pulses before declaring this stagger
			{
				uint32_t minPriIndex = 0;
				if (priHistogramPeaksFiltered[0].rangeMean
						< priHistogramPeaksFiltered[1].rangeMean)
					minPriIndex = 0;
				else
					minPriIndex = 1;
				if (priConcMatrix[minPriIndex + 1][minPriIndex + 1] > 4)
				{
					priHistogramPeaksFiltered[0] =
							priHistogramPeaksFiltered[minPriIndex];
					priHistogramPeaksFiltered.resize(1);
					sequenceTypeGuess = SequenceTypesEnum::seq_dwell;
				}
				else
					sequenceTypeGuess = SequenceTypesEnum::seq_unknown;
			}
			else
				sequenceTypeGuess = SequenceTypesEnum::seq_unknown;
		}
		else if (priHistogramPeaksFiltered.size() < 5)
		{
			if (priHistogramPeaksFiltered[0].rangeSampleCount < 4)
				sequenceTypeGuess = SequenceTypesEnum::seq_unknown;
		}
	}
	return sequenceTypeGuess; // finalized pri type
}
SequenceTypesEnum AnalysisTrackPriLowClass::guessPriTypeUsingDiffPriHistogram()
{
	SequenceTypesEnum sequenceTypeGuess = SequenceTypesEnum::seq_unknown;

	filterdiffHistogramPeaks(0.3);

	if (histogramedPulseCount < 16)
	{
		return sequenceTypeGuess;
	}
//find top two peaks in diff pri histogram that have max number of samples
	/* Make initial guess of pri type using TOA 2nd differences */
	if (diffPriHistogramPeaksSorted.size() == 0)
		return sequenceTypeGuess;

	if (diffPriHistogram.getRangePrecisionLevel() == 0)
	{ //dwell,slide,stagger

		if (diffPriHistogramPeaksSorted.size() == 1)
		{ //dwell, slide pos, slide neg

			if (abs(diffPriHistogramPeaksSorted[0].rangeMean) < 64)
			{ //dwell
//				sequenceTypeGuess = SequenceTypesEnum::seq_dwell;
			}
			else
			{
				if (diffPriHistogramPeaksSorted[0].rangeMean < 0)

				{
					sequenceTypeGuess = SequenceTypesEnum::seq_slideNeg;
				}
				else
				{
					sequenceTypeGuess = SequenceTypesEnum::seq_slidePos;
				}
			}
		}
		else if (diffPriHistogramPeaksSorted.size() == 2)
		{
			if (priHistogramPeaks.size() < 4)
			{
				return sequenceTypeGuess;
			}
			if (abs(
					abs(diffPriHistogramPeaksSorted[0].rangeMean)
							- abs(diffPriHistogramPeaksSorted[1].rangeMean))
					< 38)
			{
				//triangular slide
				sequenceTypeGuess = SequenceTypesEnum::seq_slideTri;
			}
		}
		else
		{ //stagger, sine

			float probabilityOfSine = 0;
			if (priHistogramPeaks.size() < 4)
			{
				probabilityOfSine = 0;
			}
			else
			{ // for sine pattern, every negative peak will have a corresponding positive peak in diffPri

				probabilityOfSine = CalcProbabilityOfSine(priHistogramPeaks);

			}

			if (probabilityOfSine > 0.75)
			{
				sequenceTypeGuess = SequenceTypesEnum::seq_slideSine;
			}
			else
			{
				//sequenceTypeGuess = SequenceTypesEnum::seq_stagger;
			}
		}
	}
	if(sequenceTypeGuess == SequenceTypesEnum::seq_slideTri)
	{
		filterHistogramPeaksForStagger();
		float staggerScore = CalcStaggerScoreFromConcMatrix(); // score the initial guess according to pattern found in pri buffer
		int8_t noOfPeaks = priHistogramPeaksFiltered.size();
		if (staggerScore > 0.5)
		{
			if (noOfPeaks > 1) // minimum stagger levels are 2
			{
				sequenceTypeGuess = SequenceTypesEnum::seq_unknown;
			}
		}
	}
	if (sequenceTypeGuess != SequenceTypesEnum::seq_unknown)
	{
		if (priHistogramPeaksSorted.size() == 0)
			sequenceTypeGuess = SequenceTypesEnum::seq_unknown;
	}
	return sequenceTypeGuess;
}

void AnalysisTrackPriLowClass::UpdateCurrPriType(SequenceTypesEnum newPriType)
{
	if (currPriType != newPriType)
	{
		priTypeHistory.push_back(newPriType); // keep a history of pri types
	}

//	SequenceTypesEnum lastPriType=currPriType;
	currPriType = newPriType;

	if (newPriType != SequenceTypesEnum::seq_unknown)
	{
		if (newPriType == SequenceTypesEnum::seq_stagger)
		{ // For pri stagger, tracking possible flag is only set if all phases were successfully found
			if (staggerCyclingSpotsFoundFlag == 1)
			{
				priTrackingPossibleFlag = 1;
			}
			else
			{
				priTrackingPossibleFlag = 0;
			}
		}
		else if (newPriType != SequenceTypesEnum::seq_complex)
		{
			priTrackingPossibleFlag = 1;
		}

		if (lastKnownPriType != currPriType)
		{ //e.g. when dwell changes to stagger etc.
			priTypeSwitchFlag = 1;
		}
		else
		{
			priTypeSwitchFlag = 0;
		}

		lastKnownPriType = currPriType;
	}

}
void AnalysisTrackPriLowClass::FindMaxPri(SequenceTypesEnum PriType)
{
	int32_t maxPri = 0;
	if (PriType == SequenceTypesEnum::seq_dwell)
	{
		maxPri = dwellSpots[0];
		for (uint16_t i = 1; i < dwellSpots.size(); i++)
		{
			maxPri = max(maxPri, dwellSpots[i]);
		}
	}
	else if (PriType == SequenceTypesEnum::seq_stagger)
	{
		maxPri = staggerPhases[0];
		for (uint16_t i = 1; i < staggerPhases.size(); i++)
		{
			maxPri = max(maxPri, staggerPhases[i]);
		}
	}
	else if (PriType == SequenceTypesEnum::seq_slidePos)
	{
		maxPri = slidePosEndPri;
	}
	else if (PriType == SequenceTypesEnum::seq_slideNeg)
	{
		maxPri = slideNegEndPri;
	}
	else if (PriType == SequenceTypesEnum::seq_slideTri)
	{
		maxPri = slideTriEndPri;
	}
	else if (PriType == SequenceTypesEnum::seq_slideSine)
	{
		maxPri = GetSineSlideEndPri();
	}
	else if (PriType != SequenceTypesEnum::seq_unknown)
	{
		maxPri = priSpots[0];
		for (uint16_t i = 1; i < priSpots.size(); i++)
		{
			maxPri = max(maxPri, priSpots[i]);
		}
	}
	if (PriType == SequenceTypesEnum::seq_unknown)
	{
		if(lastKnownPriType == SequenceTypesEnum::seq_unknown)
		{
			lastPriMax   = priMax;
			maxPri = priMax;
		}
		else
			maxPri   = lastPriMax;
	}
	else
		lastPriMax = maxPri;
	priMax = maxPri;
}
void AnalysisTrackPriLowClass::ResetPriInfoVectors()
{
	priSpots.resize(0);	// reset the pri array for switch in pri type
	priSpotsDevAllowed.resize(0);
	priRangeHigh.resize(0);
	priRangeLow.resize(0);
	priSpotsExpectedNextSpot.resize(0);
	priSpotsLastOccurrance.resize(0);
}
void AnalysisTrackPriLowClass::ActivateHistObjects()
{
//the function enables pushing of toas into histograms if already not enabled
	if (histogramingEnabledFlag == 1)
	{

	}
	else
	{
		priHistogram.clearHistograms();
//		diffPriHistogram.clearHistograms();
		histogramedPulseCount = 0;
		histogramingEnabledFlag = 1;
	}
}
void AnalysisTrackPriLowClass::DeactivateHistObjects()
{
	priHistogram.clearHistograms();
	diffPriHistogram.clearHistograms();
	histogramedPulseCount = 0;
	histogramingEnabledFlag = 0;
}
void AnalysisTrackPriLowClass::PushDataToHistObjects(int32_t &diffToa,
		int32_t &diffPri)
{
	/* There are 2 histogram pyramids, each having 4 histogram levels.
	 * One is for TOA 1st difference and other is for TOA 2nd difference.
	 */
	if (histogramingEnabledFlag == 1) // Flag to indicate histograms have not reached their max size
	{
		priHistogram.addToHistogram(diffToa); // 1st difference histogram
		diffPriHistogram.addToHistogram(diffPri); // 2nd difference histogram
		histogramedPulseCount++; // pulse count
		if (histogramedPulseCount > 255) // max size of histogram
		{
			histogramingEnabledFlag = 0;
		}
	}
}
void AnalysisTrackPriLowClass::PushLastNPulsesToHistograms(uint16_t pulseCount)
{ // This function picks last pulseCount number of pulses from PRIbuffer and place them in both histogram pyramids
	int16_t bufferSize = diffToaBuffer.size();
	uint16_t startIndex = max(1, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;

	int32_t lastPri = diffToaBuffer[startIndex];
	int32_t currPriValue, currDiffPri;
	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		currPriValue = diffToaBuffer[i];		// TOA 1st difference
		currDiffPri = currPriValue - lastPri;		// TOA 2nd difference
		lastPri = currPriValue;
		PushDataToHistObjects(currPriValue, currDiffPri);
	}
}
int16_t AnalysisTrackPriLowClass::LabelUsingHistogramPeaks(int32_t inputValue,
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
void AnalysisTrackPriLowClass::FindPeaksInPriHistogram()
{ // This function analyzes pyramid of histograms of TOA 1st difference to find peaks
	priHistogram.findHistogramPeaks();
	priHistogram.findPyramidOperatingLevel_V2(); // operating histogram level from pyramid
	vector<PeakInInputData> &tempHistogramPeaks = priHistogram.getPeaks();
	priHistogramPeaks.resize(0);
	for (uint16_t i = 0; i < min(32, (int32_t) tempHistogramPeaks.size()); i++)
	{
		if (tempHistogramPeaks[i].rangeMean > 180)
		{
			priHistogramPeaks.push_back(tempHistogramPeaks[i]);
		}
		else
		{
//			cout << "hi" << endl;
		}
	}
	priHistogramPeaksSorted = priHistogramPeaks;
// sort peaks according to no. of pulses belonging each
	sort(priHistogramPeaksSorted.begin(), priHistogramPeaksSorted.end(),
			PeakInInputData::comparisonByWeight);
// Estimating a rough guess of max pri
	if (priHistogramPeaksSorted.size() > 0)
	{
		int32_t maxSpotOccurence = priHistogramPeaksSorted[0].rangeSampleCount;
		int32_t maxSpotValue = priHistogramPeaksSorted[0].rangeMean;
		priHistogramPeaksFiltered.resize(0);
		vector<PeakInInputData> priHistogramValidPeaks;
		for (uint16_t i = 0;
				i < min(32, (int32_t) priHistogramPeaksSorted.size()); i++)
		{
			if (priHistogramPeaksSorted[i].rangeSampleCount
					> 0.4 * maxSpotOccurence
					&& priHistogramPeaksSorted[i].rangeMean > maxSpotValue)
			{
				maxSpotValue = priHistogramPeaksSorted[i].rangeMean;
			}
		}
		priMax = maxSpotValue;
	}

}

void AnalysisTrackPriLowClass::FindPeaksInDiffPriHistogram()
{
	diffPriHistogram.findHistogramPeaks();
	diffPriHistogram.findPyramidOperatingLevel_V2(50);
	vector<PeakInInputData> &peaks = diffPriHistogram.getPeaks();
	diffPriHistogramPeaks.resize(0);
	for (uint16_t i = 0; i < min(32, (int32_t) peaks.size()); i++)
	{
		if (abs(peaks[i].rangeMean) >= 70 && abs(peaks[i].rangeMean) <= 5030)
		{
			diffPriHistogramPeaks.push_back(peaks[i]);
		}
		else
		{
//			cout << "hi" << endl;
		}
	}
	diffPriHistogramPeaksSorted = diffPriHistogramPeaks;

	sort(diffPriHistogramPeaksSorted.begin(), diffPriHistogramPeaksSorted.end(),
			PeakInInputData::comparisonByWeight);
}

void AnalysisTrackPriLowClass::FillPriConcurranceMatrix(uint16_t pulseCount,
		vector<PeakInInputData> &peaksForLabeling, bool multiplesAllowed)
{ // This function creates concurrence matrix that stores the sequence of peaks present in pri buffer
// using last N values specified by pulse count fills the priConcurranceMatrix
	int16_t bufferSize = diffToaBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;
	int16_t lastPriValueLabel = 0;

	memset(priConcMatrix, 0, sizeof(priConcMatrix)); // 2D matrix
	for (uint16_t i = startIndex; i < endIndex; i++) // scan the whole pri buffer
	{
		int32_t currPriValue = diffToaBuffer[i];
		int16_t currPriLabel = 0;
		if (multiplesAllowed == true)
		{
			currPriLabel = LabelPriUsingAverageValues(currPriValue); // find a match of pri value from available peaks
		}
		else
		{
			currPriLabel = LabelUsingHistogramPeaks(currPriValue,
					peaksForLabeling); // find a match of pri value from available peaks

		}
		priConcMatrix[lastPriValueLabel + 1][currPriLabel + 1]++; // add one to label to cater -1 i.e. unknown label

		lastPriValueLabel = currPriLabel;
	}
}

void AnalysisTrackPriLowClass::FindMeanValuesOfPrisFromHistPeaks(
		uint16_t pulseCount, vector<PeakInInputData> &peaksForLabeling)
{
// using last N values specified by pulse count find mean Pri Values, their std dev and range
	int16_t bufferSize = diffToaBuffer.size();
	uint16_t startIndex = max(0, bufferSize - (int16_t) pulseCount);
	uint16_t endIndex = bufferSize;

	memset(tempSpotStdDevValue, 0, sizeof(tempSpotStdDevValue));
	memset(tempSpotAverageValue, 0, sizeof(tempSpotAverageValue));
	memset(tempSpotAverageCount, 0, sizeof(tempSpotAverageCount));

	memset(tempSpotMaxValue, 0, sizeof(tempSpotMaxValue));
	memset(tempSpotMinValue, 0xFFFFFFFF, sizeof(tempSpotMinValue));
	/* Find average values of spots using original pris in pri buffer */
	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		int32_t currPriValue = diffToaBuffer[i];
		int16_t currPriLabel = LabelUsingHistogramPeaks(currPriValue,
				peaksForLabeling); // search this value in histogram peaks
		if (currPriLabel != -1)
		{
			tempSpotAverageCount[currPriLabel]++;
			tempSpotAverageValue[currPriLabel] += currPriValue;
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
	/* find Standard deviation of all spots */
	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		int32_t currPriValue = diffToaBuffer[i];
		int16_t currPriLabel = LabelUsingHistogramPeaks(currPriValue,
				peaksForLabeling); // search this value in histogram peaks
		if (currPriLabel != -1)
		{
			int32_t temp = tempSpotAverageValue[currPriLabel];
			temp = abs(temp - currPriValue);
			tempSpotStdDevValue[currPriLabel] += temp;
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
	/* find max variation i.e. range of all spots */
	for (uint16_t i = startIndex; i < endIndex; i++)
	{
		uint32_t currPriValue = diffToaBuffer[i];

		int16_t currPriLabel = LabelUsingHistogramPeaks(currPriValue,
				peaksForLabeling);
		if (currPriLabel != -1)
		{
			if (currPriValue > tempSpotMaxValue[currPriLabel])
				tempSpotMaxValue[currPriLabel] = currPriValue;
			if (currPriValue < tempSpotMinValue[currPriLabel])
				tempSpotMinValue[currPriLabel] = currPriValue;
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
int8_t AnalysisTrackPriLowClass::CheckifRangeWithinLimits(
		float percentDevAllowed)
{
	int8_t peaksWithinRange = 0;
	memset(tempSpotWithinRangeFlag, 0, sizeof(tempSpotWithinRangeFlag));
	for (uint16_t i = 0; i < priHistogramPeaksFiltered.size(); i++)
	{
		float tempDevAllowed = (float) (tempSpotAverageValue[i]);
		tempDevAllowed = tempDevAllowed * percentDevAllowed; //+-% deviation allowed
		if (tempSpotRange[i] <= 2 * tempDevAllowed)
		{
			peaksWithinRange++;
			tempSpotWithinRangeFlag[i] = 1;
		}
		else if (tempDevAllowed == 0.0025) // check absolute limit for fixed pri
		{
			if (tempSpotRange[i] <= 100) // +-500ns
			{
				peaksWithinRange++;
				tempSpotWithinRangeFlag[i] = 1;
			}
		}
	}
	return peaksWithinRange;
}
int8_t AnalysisTrackPriLowClass::PriTracking(int32_t &diffToa, int32_t &diffPri)
{
// purpose of this function is to advance PRI search

	int8_t res = -1;
	switch (currPriType)
	{
	case SequenceTypesEnum::seq_dwell:
		res = DwellTracking(diffToa, diffPri);
		break;

	case SequenceTypesEnum::seq_stagger:
		res = StaggerTracking(diffToa, diffPri);
		if(alternateMissingPulseCount > 3)
		{
			priTrackingPossibleFlag =0;
		}
		break;

	case SequenceTypesEnum::seq_slidePos:
		res = SlidePosTracking(diffToa, diffPri);
		break;
	case SequenceTypesEnum::seq_slideNeg:
		res = SlideNegTracking(diffToa, diffPri);
		break;
	case SequenceTypesEnum::seq_slideTri:
		res = SlideTriTracking(diffToa, diffPri);
		break;
	case SequenceTypesEnum::seq_slideSine:
		res = SlideSineTracking(diffToa, diffPri);
		break;
	case SequenceTypesEnum::seq_jitter:
	{
		res = JitterTracking(diffToa, diffPri);
		break;
	}
	default:
		break;

	}

	return res;

}
void AnalysisTrackPriLowClass::RelockPriTracking(
		uint32_t &pulseCountSinceTrackFailure)
{ // This function tries to re-lock from the first failed pulse and afterwards search the rest of pulses in sequence
	switch (currPriType)
	{
	case SequenceTypesEnum::seq_dwell:
		if(RelockDwellPri(pulseCountSinceTrackFailure) > 0.7)
			priTrackingPossibleFlag = 1;
		break;
	case SequenceTypesEnum::seq_stagger:
		if(SequenceSearchStagger_V1(pulseCountSinceTrackFailure) > 0.7)
			priTrackingPossibleFlag = 1;
		break;
	case SequenceTypesEnum::seq_slidePos:
		if(SequenceSearchSlidePos(pulseCountSinceTrackFailure) > 0.7)
			priTrackingPossibleFlag = 1;
		break;
	default:
		break;

	}
}

vector<uint64_t>& AnalysisTrackPriLowClass::GetSeqSearchBuffer()
{
	return seqSearchLockedToaBuffer;
}
SequenceTypesEnum AnalysisTrackPriLowClass::GetCurrPriType()
{
	return currPriType;
}
vector<int32_t>& AnalysisTrackPriLowClass::GetDwellSpots()
{
	return dwellSpots;
}
vector<int32_t>& AnalysisTrackPriLowClass::GetStaggerSpots()
{
	return staggerPhases;
}
int32_t& AnalysisTrackPriLowClass::GetStaggerFrame()
{
	return staggerFrameActual;
}
vector<int32_t>& AnalysisTrackPriLowClass::GetPriSpots()
{
	return priSpots;
}
vector<int32_t>& AnalysisTrackPriLowClass::GetUpperRange()
{
	return priRangeHigh;
}
vector<int32_t>& AnalysisTrackPriLowClass::GetLowerRange()
{
	return priRangeLow;
}
int32_t& AnalysisTrackPriLowClass::GetPosSlideStartPri()
{
	return slidePosStartPri;
}
int32_t& AnalysisTrackPriLowClass::GetPosSlideRate()
{
	return slidePosRate;

}
int32_t& AnalysisTrackPriLowClass::GetPosSlideEndPri()
{
	return slidePosEndPri;

}
int32_t& AnalysisTrackPriLowClass::GetNegSlideStartPri()
{
	return slideNegStartPri;
}
int32_t& AnalysisTrackPriLowClass::GetNegSlideRate()
{
	return slideNegRate;
}
int32_t& AnalysisTrackPriLowClass::GetNegSlideEndPri()
{
	return slideNegEndPri;
}
int32_t& AnalysisTrackPriLowClass::GetTriSlideStartPri()
{
	return slideTriStartPri;
}
int32_t& AnalysisTrackPriLowClass::GetTriSlideRate()
{
	return slideTriRate;
}
int32_t& AnalysisTrackPriLowClass::GetTriSlideEndPri()
{
	return slideTriEndPri;
}

int32_t& AnalysisTrackPriLowClass::GetSineSlideStartPri()
{
	return sinePriSpots[0];
}

int32_t& AnalysisTrackPriLowClass::GetSineSlideEndPri()
{
	return sinePriSpots[(sinePriSpots.size() / 2) - 1];
}

uint32_t& AnalysisTrackPriLowClass::GetMaxPri()
{
	return priMax;

}
