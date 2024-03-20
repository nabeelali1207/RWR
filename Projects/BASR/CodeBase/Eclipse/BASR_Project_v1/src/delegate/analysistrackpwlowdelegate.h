/*
 * analysistrackpwlowdelegate.h
 *
 *  Created on: Apr 27, 2020
 *      Author: gmd
 */

#ifndef ANALYSISTRACKPWLOWDELEGATE_H_
#define ANALYSISTRACKPWLOWDELEGATE_H_

#include "../delegate/analysishistogramdelegate.h"
#include "../utility/constlow.h"
#include <boost/circular_buffer.hpp>
class AnalysisTrackPwLowClass
{
public:
	int8_t newResultsAvailableForCopyFlag; // is set when new spots are found by pwAnalysis
	int8_t lastAnalysisWasSuccessfulFlag;
	int32_t samplesProcessed;
	int8_t pwTrackingPossibleFlag;
	uint16_t lastAnalysisPulseCount;

	SequenceTypesEnum currPwType;
	vector<uint32_t> pwSpots;
	vector<uint32_t> deviationAllowed;

private:
	boost::circular_buffer<uint32_t> pwBuffer;
	boost::circular_buffer<SequenceTypesEnum> pwTypeHistory;

	PyramidOfHistograms pwHistogram;
	vector<PeakInInputData> pwHistogramPeaks;
	vector<PeakInInputData> pwHistogramPeaksSorted;
	vector<PeakInInputData> pwHistogramPeaksSortedFiltered;

	int8_t histogramingEnabledFlag;
	uint32_t histogramedPulseCount;

	uint32_t countSinceLastPwAnalysisCall;
	uint32_t pdwThresholdForPwAnalysis;
	uint32_t trackingFailedCount;

	int16_t pwConcMatrix[9][9];

	int16_t currActiveSpotIndex;
	int16_t expectedNextSpotIndex[8];

	uint32_t tempSpotStdDevValue[8];
	uint32_t tempSpotAverageValue[8];
	uint8_t tempSpotAverageCount[8];
	uint32_t tempSpotMaxValue[8];
	uint32_t tempSpotMinValue[8];
	uint32_t tempSpotRange[8];

	int8_t PwTracking(uint32_t &);
	void RelockPwTracking(int16_t);

	SequenceTypesEnum PwAnalysisNew();
	SequenceTypesEnum guessPwTypeUsingPwHistogram();

	void FindPeaksInPwHistogram();
	void filterHistogramPeaks();
	void ActivateHistObjects();
	void DeactivateHistObjects();

	void PushDataToHistObjects(uint32_t &);
	void PushLastNPulsesToHistograms(uint16_t pulseCount);
	void FillPwConcurranceMatrix(uint16_t pulseCount,
			vector<PeakInInputData> & peaksForLabeling);
	void FindExpectedNextSpotOfEachSpot();

	void UpdateCurrPwType(SequenceTypesEnum newPriType);

	int16_t LabelUsingHistogramPeaks(uint32_t inputValue,
			vector<PeakInInputData> & peaks);
	void FindMeanValuesOfPwsFromHistPeaks(uint16_t pulseCount,
			vector<PeakInInputData> & peaksForLabeling);
public:
	AnalysisTrackPwLowClass();
	void ResetPWAnalysisInfo();
	int8_t GetPwScore(uint16_t);
	void AddNextPwValue(uint32_t currPulseToa);

	int8_t TriggerPwAnalysis();

	uint16_t GetPeakPwFromHistogram();
	uint16_t GetLastPwValueFromBuffer();
};

#endif /* ANALYSISTRACKPWLOWDELEGATE_H_ */
