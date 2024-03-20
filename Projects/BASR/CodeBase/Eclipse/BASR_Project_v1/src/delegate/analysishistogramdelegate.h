/*
 * analysishistogramsdelegate.h
 *
 *  Created on: Sep 6, 2019
 *      Author: gmd
 */

#ifndef ANALYSISHISTOGRAMDELEGATE_H_
#define ANALYSISHISTOGRAMDELEGATE_H_

#include "../common/common.h"

struct PeakInHistogramPyramid
{
	uint32_t histogramLevel;
	uint32_t sampleCount;
	uint32_t startIndex;
	uint32_t endIndex;
};
struct PeakInInputData
{
	int16_t precision;
	int16_t percentSignificance;
	int32_t rangeMean;
	int32_t rangeStdDev;
	uint32_t rangeSampleCount;
	int32_t rangeMinValue;
	int32_t rangeMaxValue;

	static bool comparisonByWeight(const PeakInInputData & first,const PeakInInputData& second)
	{
		return (first.rangeSampleCount>second.rangeSampleCount);
	}
};
enum class SequenceTypesEnum : uint8_t
{
	seq_unknown = 0x00,
	seq_jitter = 0x01,
	seq_stagger = 0x02,
	seq_complex = 0x03,
	seq_dwell = 0x04,
	seq_dwellOrStagger = 0x06,
	seq_dwellOrjitterOrcomplex = 0x07,

	seq_slidePos = 0x08,
	seq_slideNeg = 0x10,
	seq_slidePosOrNeg = 0x18,
	seq_slideTri = 0x20,
	seq_slideSine = 0x21,
	seq_difftoa = 0x22
};
class PyramidOfHistograms
{
private:

	int8_t	histogramType;
	uint16_t totalInputSamples;

	uint32_t totalMemorySize;
	uint16_t *hist_L0;
	uint16_t *hist_L1;
	uint16_t *hist_L2;
	uint16_t *hist_L3;

	int32_t pyramidLevelForValidPeaks;
	vector<PeakInInputData> peaksFound;
	vector<PeakInHistogramPyramid> L4_PeakBins;
	vector<PeakInHistogramPyramid> L3_PeakBins;
	vector<PeakInHistogramPyramid> L2_PeakBins;
	vector<PeakInHistogramPyramid> L1_PeakBins;
	vector<PeakInHistogramPyramid> L0_PeakBins;

	// constants
	uint16_t maxPeaksToFind;
	int32_t maxHistogramInputValue;
	int32_t offsetInInput;
	uint32_t histogramShiftLevels[5];
	uint32_t histogramSizes[5];
	uint16_t histDiscontinuityAllowed[5] =
	{ 1, 0, 0, 0, 0 };

public:
	uint16_t getInputSamplesCount()
	{
		return totalInputSamples;
	}
	void allocateHistogram(int32_t maxInputValue);
	PyramidOfHistograms(int32_t maxInputValue, int32_t offset,
			uint16_t maxPeaks,int8_t histType);
	~PyramidOfHistograms();
	void operator=(const PyramidOfHistograms & other);
	PyramidOfHistograms(const PyramidOfHistograms & other);
	void clearHistograms();
	void setNumberOfPeaksToFind(uint16_t count)
	{
		maxPeaksToFind = count;
	}
	void addToHistogram(int32_t inputValue);

	void findHistogramPeaks();
	void findPyramidOperatingLevel_V1();
	void findPyramidOperatingLevel_V2(uint32_t percent = 70);
	vector<PeakInInputData> & getPeaks();

	uint32_t findSampleCountInARange(int32_t startRange,int32_t endRange);

	int32_t getRangePrecisionLevel()
	{
		return pyramidLevelForValidPeaks;
	}
	uint32_t getMaxHistogramInputValue()
	{
		return maxHistogramInputValue;
	}
private:
	void findLocalMaximas(vector<PeakInHistogramPyramid> &resultInterestPoints,
			uint16_t * hist, uint32_t pyramidLevel,
			vector<PeakInHistogramPyramid> & suspectedInterestPoints);
	void saveFoundPeaks(vector<PeakInHistogramPyramid> &currBinLevel,
			vector<PeakInHistogramPyramid> & nextBinLevel);
	void convertHistogramPeakToOutputFormat(PeakInHistogramPyramid & source,
			PeakInInputData & dest);
	void findMeanAndStdDevInPyramid(int32_t pyramidLevel, int32_t & mean,
			int32_t &stdDev, uint32_t startIndex, uint32_t endIndex);
}
;

#endif /* ANALYSISHISTOGRAMDELEGATE_H_ */
