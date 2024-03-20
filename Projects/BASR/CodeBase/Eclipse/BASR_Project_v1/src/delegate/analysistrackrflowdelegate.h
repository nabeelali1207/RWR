/*
 * analysistrackrflowdelegate.h
 *
 *  Created on: Apr 28, 2020
 *      Author: gmd
 */

#ifndef ANALYSISTRACKRFLOWDELEGATE_H_
#define ANALYSISTRACKRFLOWDELEGATE_H_


#include "../delegate/analysishistogramdelegate.h"
#include "../utility/constlow.h"
#include <boost/circular_buffer.hpp>
class AnalysisTrackRfLowClass
{
public:
	int8_t newResultsAvailableForCopyFlag; // is set when new spots are found by pwAnalysis
	int8_t lastAnalysisWasSuccessfulFlag;
	int32_t samplesProcessed;
	int8_t rfTrackingPossibleFlag;
	uint16_t lastAnalysisPulseCount;
	uint16_t rfMedian;
	SequenceTypesEnum currRfType;
	vector<uint32_t> rfSpots;
	vector<uint32_t> deviationAllowed;
	vector<uint32_t> rfRangeHigh;
	vector<uint32_t> rfRangeLow;
	vector<uint32_t> rfSpotsExpectedNextSpot;
	vector<uint32_t> rfSpotsLastOccurrance;

private:
	boost::circular_buffer<uint32_t> rfBuffer;
	boost::circular_buffer<int8_t> rfValidBuffer;
	boost::circular_buffer<SequenceTypesEnum> rfTypeHistory;

	PyramidOfHistograms rfHistogram;
	vector<PeakInInputData> rfHistogramPeaks;
	vector<PeakInInputData> rfHistogramPeaksSorted;
	vector<PeakInInputData> rfHistogramPeaksFiltered;

	int8_t histogramingEnabledFlag;
	uint32_t histogramedPulseCount;

	vector<uint32_t> dwellSpots;
	vector<int16_t> dwellSpotsDeviationAllowed;
	int16_t currActiveDwellSpotIndex;

	uint32_t countSinceLastRfAnalysisCall;
	uint32_t pdwThresholdForRfAnalysis;
	uint32_t trackingFailedCount;

	int16_t rfConcMatrix[17][17];

	int16_t currActiveSpotIndex;
	int16_t expectedNextSpotIndex[16];

	uint32_t tempSpotStdDevValue[16];
	uint32_t tempSpotAverageValue[16];
	uint8_t tempSpotAverageCount[16];
	int32_t tempSpotMaxValue[16];
	uint32_t tempSpotMinValue[16];
	uint32_t tempSpotRange[16];
	uint8_t tempSpotWithinRangeFlag[16];

	int8_t rfTracking(uint32_t &,int8_t& );
	void RelockRfTracking(int16_t);

	SequenceTypesEnum RfAnalysisNew();
	SequenceTypesEnum guessRfTypeUsingRfHistogram();

	void FindPeaksInRfHistogram();
	void filterHistogramPeaks();
	void ActivateHistObjects();
	void DeactivateHistObjects();

	void PushDataToHistObjects(uint32_t & currPw,int8_t& rfValid);
	void PushLastNPulsesToHistograms(uint16_t pulseCount);
	void FillRFConcurranceMatrix(uint16_t pulseCount,
			vector<PeakInInputData> & peaksForLabeling);
	void filterHistogramPeaksForDwell(uint32_t threshold);
	int32_t findAgileRange();
	float CalcDwellScoreFromConcMatrix();
	float CalcJumpScore();
	void FindExpectedNextSpotOfEachSpot();
	void UpdateJitterAndComplexRange();
	void UpdateCurrRfType(SequenceTypesEnum newPriType);

	int16_t LabelUsingHistogramPeaks(int32_t inputValue,
			vector<PeakInInputData> & peaks);
	void UpdateJumpSpots();
	void UpdateDwellSpots();
	void AddNewDwellSpot(int32_t spotValue,
			int16_t devAllowed);
	void UpdateJitterSpots();
	void UpdateComplexSpots();
	void ResetRfInfoVectors();
	void FindMeanValuesOfRfsFromHistPeaks(uint32_t pulseCount,
			vector<PeakInInputData> & peaksForLabeling);
public:
	AnalysisTrackRfLowClass();
	int8_t GetRfScore(uint16_t,int8_t rfValid);
	void AddNextRfValue(uint32_t currRf,int8_t rfValid);
	SequenceTypesEnum GetCurrRfType();
	vector<uint32_t>& GetDwellSpots();
	vector<uint32_t>& GetRfSpots();
	vector<uint32_t>& GetUpperRange();
	vector<uint32_t>& GetLowerRange();
	uint32_t GetSpotRange(uint16_t spotIndex);
	uint32_t GetSpotMinValue(uint16_t spotIndex);
	uint32_t GetSpotMaxValue(uint16_t spotIndex);
};


#endif /* ANALYSISTRACKRFLOWDELEGATE_H_ */
