/*
 * analysistrackprilowdelegate.h
 *
 *  Created on: Mar 21, 2020
 *      Author: gmd
 */

#ifndef ANALYSISTRACKPRILOWDELEGATE_H_
#define ANALYSISTRACKPRILOWDELEGATE_H_

#include <boost/circular_buffer.hpp>
#include "analysishistogramdelegate.h"
#include "../utility/constlow.h"
#include "../utility/utility.h"

class AnalysisTrackPriLowClass
{
public:
	int8_t newResultsAvailableForCopyFlag; // is set when new spots are found by pwAnalysis
	int8_t lastAnalysisWasSuccessfulFlag;
	uint16_t lastAnalysisPulseCount;
	int32_t samplesProcessed;
	int8_t priTrackingPossibleFlag;
	uint64_t firstLockedTOA;
	uint32_t countSinceLastSuccessfulPriAnalysis;
	SequenceTypesEnum currPriType;

	SequenceTypesEnum lastKnownPriType;
	uint8_t priTypeSwitchFlag;// flag is raised when pri type changes from one type to another

private:

	SequenceTypesEnum histBased_currPriType;
	boost::circular_buffer<SequenceTypesEnum> priTypeHistory;

	PyramidOfHistograms priHistogram;
	PyramidOfHistograms diffPriHistogram;
	vector<PeakInInputData> priHistogramPeaks;
	vector<PeakInInputData> priHistogramPeaksFiltered; //temporary storage for filtered peaks used in dwell or stagger
	vector<PeakInInputData> priHistogramPeaksSorted; //sorted by sample count in each peak
	vector<PeakInInputData> diffPriHistogramPeaks;
	vector<PeakInInputData> diffPriHistogramPeaksSorted; //sorted by sample count in each peak
	uint32_t histogramedPulseCount;
	int32_t histogramingEnabledFlag;
	uint32_t countSinceLastPriAnalysisCall;

	int16_t priConcMatrix[33][33];// max 32 peaks, 0th index belongs to unknown value
	int16_t diffPriConcMatrix[33][33];// max 32 peaks, 0th index belongs to unknown value

	vector<uint64_t> seqSearchLockedToaBuffer;
	boost::circular_buffer<uint64_t> toaBuffer;
	boost::circular_buffer<int32_t> diffToaBuffer;
	boost::circular_buffer<int16_t> priLabels;

	uint16_t staggerPhaseCount;
	vector<int32_t> staggerPhases;
	int8_t staggerExpectedNextIndex[32];
	int8_t staggerValidFeaturePointFlag[32];
	uint64_t staggerPhaseLastOccurranceToa[32];	//time toa whem last phase occurrance was found
	int8_t staggerMultiFrameIndex[32];
	int32_t staggerFrameInferred;						//sum of stagger phases
	int32_t staggerFrameActual;	//actual measured from frame repitition detection
	int8_t staggerCyclingSpotsFoundFlag;//stagger cycle was found in peaks or not
	int16_t currentStaggerPhase;
	int16_t nextPredictedStaggerPhase;
	int16_t strengthOfStaggerPhases[33];
	int16_t peakValueIndices[33];
	int16_t alternateMissingPulseCount;

	int32_t slidePosStartPri;
	int32_t slidePosRate;
	int32_t slidePosEndPri;
	int32_t slidePosLockedPriValue;
	int32_t slidePosLockedPriValueSampleIndex; // sampleNumber when pri was locked

	int32_t slideNegStartPri;
	int32_t slideNegRate;
	int32_t slideNegEndPri;
	int32_t slideNegLockedPriValue;
	int32_t slideNegLockedPriValueSampleIndex; // sampleNumber when pri was locked

	int32_t slideTriStartPri;
	int32_t slideTriRate;
	int32_t slideTriEndPri;
	int32_t slideTriLockedPriValue;
	int32_t slideTriLockedPriValueSampleIndex; // sampleNumber when pri was locked

	vector<int32_t> sinePriSpots;
	vector<int16_t> sinePriSpotsDeviationAllowed;
	int16_t sineNextPredictedIndex;
	int16_t sineLastLockedIndex;

	vector<int32_t> dwellSpots;
	vector<int16_t> dwellSpotsDeviationAllowed;
	vector<int16_t> dwellPulsesPerSpot;
	vector<uint64_t> dwellSpotLastOccurrenceToa;
	int16_t currActiveDwellSpotIndex;

	uint32_t labelingFailedCount;
	uint32_t trackingFailedCount;

	uint32_t pdwThresholdForPriAnalysis;

	uint32_t tempSpotStdDevValue[32];
	uint32_t tempSpotAverageValue[32];
	uint8_t tempSpotAverageCount[32];
	uint8_t tempSpotExpectedNextIndex[32];
	int8_t tempIgnoreSpotFlag[32];
	uint32_t tempSpotMaxValue[32];
	uint32_t tempSpotMinValue[32];
	uint32_t tempSpotRange[32];
	uint8_t tempSpotWithinRangeFlag[32];

	uint32_t priMax;
	uint32_t lastPriMax;

	vector<int32_t> priSpots;
	vector<int32_t> priSpotsDevAllowed;
	vector<int32_t> priRangeHigh;
	vector<int32_t> priRangeLow;
	vector<int16_t> priSpotsExpectedNextSpot;
	vector<int32_t> priSpotsLastOccurrance;

	int32_t SumOfVector(vector<int32_t> &dVector);

	int16_t LabelAccordingToPriType(SequenceTypesEnum seqType,
			int32_t currDiffToa);
	int16_t LabelPriUsingAverageValues(int32_t currDiffToa);

	void filterHistogramPeaksForStagger();
	float CalcStaggerScoreFromConcMatrix();
	int16_t StaggerAnalyzeConcurranceMatrix(vector<PeakInInputData> &peaks);
	void UpdateStaggerSpots();
	void StaggerUpdateExpectedIndices();
	int16_t StaggerFillStaggerSpotsArray(uint16_t peakCount);
	uint16_t StaggerIgnoreMultiplePeaks(uint16_t inputPeakCount);
	int8_t StaggerCheckDoublePeaks(vector<PeakInInputData> &peaks);
	void StaggerFindValidFeaturePoint(uint16_t consecutiveNess);
	void StaggerComputeFrame(uint16_t pulseCount);
	int8_t StaggerTracking(int32_t &diffToa, int32_t &diffPri);
	float SequenceSearchStagger_V1(uint16_t pulseCount);
	float SequenceSearchStagger_V2(uint16_t pulseCount);

	void UpdateSlidePosSpots();
	int8_t SlidePosTracking(int32_t &diffToa, int32_t &diffPri);
	float SequenceSearchSlidePos(uint16_t pulseCount);
	void UpdateSlideNegSpots();
	int8_t SlideNegTracking(int32_t &diffToa, int32_t &diffPri);
	float SequenceSearchSlideNeg(uint16_t pulseCount);
	void UpdateSlideTriSpots();
	int8_t SlideTriTracking(int32_t &diffToa, int32_t &diffPri);
	float SequenceSearchSlideTri(uint16_t pulseCount);
	float CalcProbabilityOfSine(vector<PeakInInputData> &peaks);
	void UpdateSlideSineSpots();
	int8_t SlideSineTracking(int32_t &diffToa, int32_t &diffPri);
	float SequenceSearchSlideSine(uint16_t pulseCount);
	void filterdiffHistogramPeaks(float threshold);

	void filterHistogramPeaks(uint32_t threshold);
	void filterHistogramPeaksSorted(uint32_t threshold);
	void filterHistogramPeaksForDwell(uint32_t threshold);
	bool findIfNthMultiple(int32_t queryValue, int32_t spotValue,
			int16_t devAllowed);
	float CalcDwellScoreFromConcMatrix();
	void AddNewDwellSpot(int32_t spotValue, int16_t devAllowed);
	void UpdateDwellSpots();
	int8_t DwellTracking(int32_t &diffToa, int32_t &diffPri);
	float SequenceSearchDwell(uint16_t pulseCount);
	float RelockDwellPri(uint16_t pulseCount);
	void DwellExpireSpotsWithZeroDwellTime();
	void ExpireOldDwellSpots();

	void UpdateDiffToaSpots();
	void MergeSpots(float threshold);

	void filterPeaksForJitterComplex(uint32_t threshold);
	void UpdateJitterSpots();
	int8_t JitterTracking(int32_t &diffToa, int32_t &diffPri);
	void UpdateComplexSpots();

	SequenceTypesEnum PriAnalysisNew();
	SequenceTypesEnum guessPriTypeUsingPriHistogram();
	SequenceTypesEnum guessPriTypeUsingDiffPriHistogram();

	void UpdateCurrPriType(SequenceTypesEnum newPriType);
	void FindMaxPri(SequenceTypesEnum PriType);

	void ResetPriInfoVectors();
	void ActivateHistObjects();
	void DeactivateHistObjects();
	void PushDataToHistObjects(int32_t &diffToa, int32_t &diffPri);
	void PushLastNPulsesToHistograms(uint16_t pulseCount);
	int16_t LabelUsingHistogramPeaks(int32_t inputValue,
			vector<PeakInInputData> &peaks);
	void FindPeaksInPriHistogram();
	void FindPeaksInDiffPriHistogram();
	void FillPriConcurranceMatrix(uint16_t pulseCount,
			vector<PeakInInputData> &peaksForLabeling, bool multiplesAllowed);
	void FindMeanValuesOfPrisFromHistPeaks(uint16_t pulseCount,
			vector<PeakInInputData> &peaksForLabeling);
	int8_t CheckifRangeWithinLimits(float percentDevAllowed);

	int8_t PriTracking(int32_t &diffToa, int32_t &diffPri);
	void RelockPriTracking(uint32_t&);

public:
	AnalysisTrackPriLowClass();
	void setFirstToa(uint64_t &firstToa);
	uint8_t CanGuessNextTOA();
	int8_t DoesToaBelongToThisTrack(uint64_t&);
	int8_t PushToa(uint64_t currPulseToa);

	void CalledBeforeRporting(); //this function is called just before reporting a track to cpu
	void CalledOnReportingPeriodEnd(); //this function is called at the end of every reporting period

	vector<uint64_t>& GetSeqSearchBuffer();

	SequenceTypesEnum GetCurrPriType();
	vector<int32_t>& GetDwellSpots();
	vector<int32_t>& GetStaggerSpots();
	int32_t& GetStaggerFrame();
	vector<int32_t>& GetPriSpots();
	vector<int32_t>& GetUpperRange();
	vector<int32_t>& GetLowerRange();
	int32_t& GetPosSlideStartPri();
	int32_t& GetPosSlideRate();
	int32_t& GetPosSlideEndPri();
	int32_t& GetNegSlideStartPri();
	int32_t& GetNegSlideRate();
	int32_t& GetNegSlideEndPri();
	int32_t& GetTriSlideStartPri();
	int32_t& GetTriSlideRate();
	int32_t& GetTriSlideEndPri();
	int32_t& GetSineSlideStartPri();
	int32_t& GetSineSlideEndPri();
	uint32_t& GetMaxPri();

};

#endif /* ANALYSISTRACKPRILOWDELEGATE_H_ */
