/*
 * analysishistogramdelegate.cpp
 *
 *  Created on: Sep 6, 2019
 *      Author: gmd
 */

#include "analysishistogramdelegate.h"

PyramidOfHistograms::PyramidOfHistograms(int32_t maxInputValue, int32_t offset,
		uint16_t maxPeaks, int8_t histType)
{
	histogramType=histType;
	allocateHistogram(maxInputValue);
	offsetInInput = offset;
	setNumberOfPeaksToFind(maxPeaks);
	clearHistograms();
}
PyramidOfHistograms::PyramidOfHistograms(const PyramidOfHistograms &other)
{
	histogramType = other.histogramType;
	totalMemorySize = other.totalMemorySize;
	allocateHistogram(other.maxHistogramInputValue);
	this->operator=(other);
}
PyramidOfHistograms::~PyramidOfHistograms()
{
	delete[] (hist_L0);
}
void PyramidOfHistograms::allocateHistogram(int32_t maxInputValue)
{
	maxInputValue = (uint32_t) ceil(log2((double) abs(maxInputValue)));
	maxHistogramInputValue = 1 << maxInputValue;

	static const uint16_t DOWN_SAMPLING_RATE_For_PRI[5] =
	{ 5, 4, 4, 4, 0 };
	static const uint16_t DOWN_SAMPLING_RATE_For_PW[5] =
	{ 3, 4, 4, 4, 0 };

	static const uint16_t DOWN_SAMPLING_RATE_For_RF[5] =
		{ 2, 4, 4, 4, 0 };
	static const uint16_t DOWN_SAMPLING_RATE_For_CW[5] =
	{ 2, 4, 4, 4, 0 };

	const uint16_t *tempArray;
	histDiscontinuityAllowed[1] = 0;
	histDiscontinuityAllowed[2] = 0;
	histDiscontinuityAllowed[3] = 0;
	histDiscontinuityAllowed[4] = 0;

	switch(histogramType )
	{
	case 0:
		tempArray = DOWN_SAMPLING_RATE_For_PRI;
		histDiscontinuityAllowed[0] = 1;
		break;
	case 1:
		tempArray = DOWN_SAMPLING_RATE_For_RF;
		histDiscontinuityAllowed[0] = 1;
		break;
	case 2:
		tempArray = DOWN_SAMPLING_RATE_For_CW;
		histDiscontinuityAllowed[0] = 2;
		break;
	case 3:
		tempArray = DOWN_SAMPLING_RATE_For_PW;
		histDiscontinuityAllowed[0] = 1;
		break;
	default:
		tempArray = DOWN_SAMPLING_RATE_For_PRI;
		histDiscontinuityAllowed[0] = 1;
		break;
	}
	histogramShiftLevels[0] = tempArray[0];
	histogramShiftLevels[1] = histogramShiftLevels[0]
			+ tempArray[1];
	histogramShiftLevels[2] = histogramShiftLevels[1]
			+ tempArray[2];
	histogramShiftLevels[3] = histogramShiftLevels[2]
			+ tempArray[3];
	histogramShiftLevels[4] = histogramShiftLevels[3]
			+ tempArray[4];

	histogramSizes[0] = maxHistogramInputValue >> histogramShiftLevels[0];
	histogramSizes[1] = maxHistogramInputValue >> histogramShiftLevels[1];
	histogramSizes[2] = maxHistogramInputValue >> histogramShiftLevels[2];
	histogramSizes[3] = maxHistogramInputValue >> histogramShiftLevels[3];
	histogramSizes[4] = maxHistogramInputValue >> histogramShiftLevels[4];

	totalMemorySize = histogramSizes[0] + histogramSizes[1] + histogramSizes[2]
			+ histogramSizes[3] + histogramSizes[4];
	totalMemorySize = totalMemorySize * sizeof(uint16_t);
	hist_L0 = new uint16_t[totalMemorySize];
	hist_L1 = (hist_L0 + histogramSizes[0]);
	hist_L2 = (hist_L1 + histogramSizes[1]);
	hist_L3 = (hist_L2 + histogramSizes[2]);
}
void PyramidOfHistograms::operator=(const PyramidOfHistograms &other)
{
	totalInputSamples = other.totalInputSamples;
	memcpy(hist_L0, other.hist_L0, totalMemorySize);
	pyramidLevelForValidPeaks = other.pyramidLevelForValidPeaks;
	maxPeaksToFind = other.maxPeaksToFind;
	maxHistogramInputValue = other.maxHistogramInputValue;
	offsetInInput = other.offsetInInput;
	maxPeaksToFind = other.maxPeaksToFind;

	peaksFound.reserve(maxPeaksToFind);
	L3_PeakBins.reserve(maxPeaksToFind);
	L2_PeakBins.reserve(maxPeaksToFind);
	L1_PeakBins.reserve(maxPeaksToFind);
	L0_PeakBins.reserve(maxPeaksToFind);

	peaksFound = other.peaksFound;
	L4_PeakBins = other.L4_PeakBins;
	L3_PeakBins = other.L3_PeakBins;
	L2_PeakBins = other.L2_PeakBins;
	L1_PeakBins = other.L1_PeakBins;
	L0_PeakBins = other.L0_PeakBins;

	memcpy(histogramShiftLevels, other.histogramShiftLevels,
			sizeof(histogramShiftLevels));
	memcpy(histogramSizes, other.histogramSizes, sizeof(histogramSizes));
	memcpy(histDiscontinuityAllowed, other.histDiscontinuityAllowed,
			sizeof(histDiscontinuityAllowed));

}
void PyramidOfHistograms::clearHistograms()
{
	L4_PeakBins.resize(1);
	peaksFound.reserve(maxPeaksToFind);
	L3_PeakBins.reserve(maxPeaksToFind);
	L2_PeakBins.reserve(maxPeaksToFind);
	L1_PeakBins.reserve(maxPeaksToFind);
	L0_PeakBins.reserve(maxPeaksToFind);

	totalInputSamples = 0;
	pyramidLevelForValidPeaks = -1;
	memset(hist_L0, 0, totalMemorySize);

}
void PyramidOfHistograms::addToHistogram(int32_t inputValue)
{
	inputValue += offsetInInput; // offset will be a positive value for TOA 2nd difference to avoid negative bin values
	inputValue = max(0, min(inputValue, maxHistogramInputValue));

	uint32_t rangeLimitedInputValue = (uint32_t) inputValue;
	uint32_t indexL0 = rangeLimitedInputValue >> histogramShiftLevels[0]; // shift by 5
	uint32_t indexL1 = rangeLimitedInputValue >> histogramShiftLevels[1]; // shift by 9
	uint32_t indexL2 = rangeLimitedInputValue >> histogramShiftLevels[2]; // shift by 13
	uint32_t indexL3 = rangeLimitedInputValue >> histogramShiftLevels[3]; // shift by 17
	hist_L0[indexL0]++; // lowest histogram (in pyramid) with largest size
	hist_L1[indexL1]++;
	hist_L2[indexL2]++;
	hist_L3[indexL3]++; // Highest in the pyramid with smallest size
	totalInputSamples++;
}
void PyramidOfHistograms::findHistogramPeaks()
{/* This function analyzes pyramid of histograms (4 levels of histogram)
 *  to find valid pri peaks with best possible resolution
 */
	peaksFound.resize(0);
	L4_PeakBins[0].histogramLevel = 4; // default histogram with all bins as valid
	L4_PeakBins[0].startIndex = 0;
	L4_PeakBins[0].sampleCount = totalInputSamples; // no. of pulses used in pri analysis
	L4_PeakBins[0].endIndex = histogramSizes[4];

	// Analyze top level histogram first to locate the populated area
	findLocalMaximas(L3_PeakBins, hist_L3, 3, L4_PeakBins);

	// Analyze 2nd level histogram to further narrow down the search area
	findLocalMaximas(L2_PeakBins, hist_L2, 2, L3_PeakBins);

	// Analyze 3rd level histogram to further narrow down the search area
	findLocalMaximas(L1_PeakBins, hist_L1, 1, L2_PeakBins);

	// Analyze lowest level histogram to find valid peaks
	findLocalMaximas(L0_PeakBins, hist_L0, 0, L1_PeakBins);

}

void PyramidOfHistograms::findPyramidOperatingLevel_V1()
{
	saveFoundPeaks(L3_PeakBins, L2_PeakBins);
	saveFoundPeaks(L2_PeakBins, L1_PeakBins);
	saveFoundPeaks(L1_PeakBins, L0_PeakBins);

	L4_PeakBins[0].startIndex = 0;
	L4_PeakBins[0].endIndex = 0;
	L4_PeakBins[0].sampleCount = 0;
	saveFoundPeaks(L0_PeakBins, L4_PeakBins);
}

void PyramidOfHistograms::findPyramidOperatingLevel_V2(uint32_t percent)
{/* This function picks up the histogram level that best defines the pri/any-data peaks
 * with the best possible resolution but constitutes of almost all pulses used in pri analysis
 */
	peaksFound.resize(0);
	uint16_t level0Samples = 0;
	for (uint16_t i = 0; i < L0_PeakBins.size(); i++)
		level0Samples += L0_PeakBins[i].sampleCount;

	uint16_t level1Samples = 0;
	for (uint16_t i = 0; i < L1_PeakBins.size(); i++)
		level1Samples += L1_PeakBins[i].sampleCount;

	uint16_t level2Samples = 0;
	for (uint16_t i = 0; i < L2_PeakBins.size(); i++)
		level2Samples += L2_PeakBins[i].sampleCount;

	uint32_t samplesToHandle = totalInputSamples;
	samplesToHandle = (samplesToHandle * percent) / 100; // default value is 70% of total pulses

	vector<PeakInHistogramPyramid> *targetBins;
	if (level0Samples > samplesToHandle)
	{ // if peaks of lowest level histogram constitutes all pulses, no need to check further as it is highest resolution level
		pyramidLevelForValidPeaks = 0;
		targetBins = &L0_PeakBins;
	}
	else if (level1Samples > samplesToHandle)
	{ // if level 0 was not valid, next best is level 1
		pyramidLevelForValidPeaks = 1;
		targetBins = &L1_PeakBins;

	}
	else if (level2Samples > samplesToHandle)
	{ // if level 1 was not valid, next best is level 2, valid very rarely
		pyramidLevelForValidPeaks = 2;
		targetBins = &L2_PeakBins;
	}
	else
	{ // if level 2 was not valid, next best is level 3
		pyramidLevelForValidPeaks = 3;
		targetBins = &L3_PeakBins;
	}
	// convert peak information to a standard format
	for (uint16_t i = 0; i < targetBins->size(); i++)
	{
		PeakInHistogramPyramid tempData = (*targetBins)[i];
		PeakInInputData tempOutputData;
		convertHistogramPeakToOutputFormat(tempData, tempOutputData);
		peaksFound.push_back(tempOutputData);  // final peaks
	}

}
vector<PeakInInputData>& PyramidOfHistograms::getPeaks()
{
	return peaksFound;
}
void PyramidOfHistograms::findLocalMaximas(
		vector<PeakInHistogramPyramid> &resultInterestPoints, uint16_t *hist,
		uint32_t pyramidLevel,
		vector<PeakInHistogramPyramid> &suspectedInterestPoints)
{
	// This function searches for candidate pris/peaks using histogram info
	uint32_t histogramSize = histogramSizes[pyramidLevel];

	resultInterestPoints.resize(0);
	int32_t peaksFound = 0;

	PeakInHistogramPyramid tempInterestPoint;
	// suspected interest points are bin values with valid peaks from the higher level histogram
	for (uint32_t ii = 0; ii < suspectedInterestPoints.size(); ii++)
	{
		// conversions from higher level histogram to the next level (below)
		uint16_t upscaleLevel = histogramSize
				/ histogramSizes[suspectedInterestPoints[ii].histogramLevel]; // scalar factor to convert bin numbers from the higher level histogram to current level i.e. 1 level down
		int32_t startPoint = upscaleLevel
				* suspectedInterestPoints[ii].startIndex; // convert start index of the suspected peak under observation
		int32_t endPoint = upscaleLevel * suspectedInterestPoints[ii].endIndex;	// convert end index of the suspected peak under observation

		int32_t strongPeakThreshold = 2;
		int32_t weakPeakThreshold = 1;
		if(histogramType==2)
		{
			strongPeakThreshold = max(16,totalInputSamples/(maxPeaksToFind*2));
			weakPeakThreshold = strongPeakThreshold>>1;
		}

		int32_t weakPeakStartPoint = startPoint;
		int32_t weakPeakEndPoint = startPoint;
		int32_t peakSamplesCount = 0;

		int32_t discontinuityAllowed = 1
				+ histDiscontinuityAllowed[pyramidLevel];

		int32_t discontinuityCounter = 0;
		for (int32_t i = startPoint; i < endPoint; i++)
		{
			if (hist[i] >= weakPeakThreshold) // weakPeakThreshold is noise floor
			{
				discontinuityCounter = discontinuityAllowed;
				if (peakSamplesCount == 0)
				{
					weakPeakStartPoint = i;
				}
				weakPeakEndPoint = i + 1;
				peakSamplesCount += hist[i];
			}
			else
			{
				discontinuityCounter--; // spread break means current peak has ended
			}

			if (discontinuityCounter == 0 || i == (endPoint - 1))
			{
				if (peakSamplesCount >= strongPeakThreshold)
				{ // valid peak
					peaksFound++;
					if (peaksFound > maxPeaksToFind)
						break;
					// store peak related information to be used in the next level histogram
					tempInterestPoint.startIndex = weakPeakStartPoint;
					tempInterestPoint.endIndex = weakPeakEndPoint;
					tempInterestPoint.sampleCount = peakSamplesCount;
					tempInterestPoint.histogramLevel =
							suspectedInterestPoints[ii].histogramLevel - 1;
					resultInterestPoints.push_back(tempInterestPoint); // will become suspected points for next level histogram

				}
				peakSamplesCount = 0;
			}
		}
	}
}
void PyramidOfHistograms::saveFoundPeaks(
		vector<PeakInHistogramPyramid> &currBinLevel,
		vector<PeakInHistogramPyramid> &nextBinLevel)
{

	for (uint16_t i = 0; i < currBinLevel.size(); i++)
	{

		bool expandedToNextLevel = false;
		uint32_t pulsesHandledOnNextLevel = 0;
		for (uint16_t j = 0; j < nextBinLevel.size(); j++)
		{
			uint16_t upscaleLevel =
					histogramSizes[nextBinLevel[j].histogramLevel]
							/ histogramSizes[currBinLevel[i].histogramLevel];

			uint32_t startPoint = upscaleLevel * currBinLevel[i].startIndex;
			uint32_t endPoint = upscaleLevel * currBinLevel[i].endIndex;

			if (nextBinLevel[j].startIndex >= startPoint
					&& nextBinLevel[j].endIndex < endPoint)
			{
				pulsesHandledOnNextLevel += nextBinLevel[j].sampleCount;
			}
		}
		if (((pulsesHandledOnNextLevel * 100) / currBinLevel[i].sampleCount)
				> 60)
		{
			expandedToNextLevel = true;
		}
		if (expandedToNextLevel == false)
		{
			peaksFound.push_back(PeakInInputData());
			convertHistogramPeakToOutputFormat(currBinLevel[i],
					peaksFound.back());
		}

	}
}
void PyramidOfHistograms::convertHistogramPeakToOutputFormat(
		PeakInHistogramPyramid &source, PeakInInputData &dest)
{ /* This function  converts peak data from histogram to a standard format used for
 * defining pri spots. It also scales down the bin values to standard pri unit using
 * operating level info.
 */
	dest.percentSignificance = (source.sampleCount * 100) / totalInputSamples;
	dest.rangeSampleCount = source.sampleCount;
	dest.precision = 1 << histogramShiftLevels[source.histogramLevel];

	dest.rangeMinValue = source.startIndex
			<< histogramShiftLevels[source.histogramLevel];
	dest.rangeMaxValue = source.endIndex
			<< histogramShiftLevels[source.histogramLevel];

	uint16_t upscaleLevel = histogramSizes[0]
			/ histogramSizes[source.histogramLevel];

	findMeanAndStdDevInPyramid(0, dest.rangeMean, dest.rangeStdDev,
			upscaleLevel * source.startIndex, upscaleLevel * source.endIndex);

	dest.rangeMinValue -= offsetInInput;
	dest.rangeMaxValue -= offsetInInput;

}
void PyramidOfHistograms::findMeanAndStdDevInPyramid(int32_t pyramidLevel,
		int32_t &mean, int32_t &stdDev, uint32_t startIndex, uint32_t endIndex)
{
	uint16_t *hist = 0;
	switch (pyramidLevel)
	{
	case 0:
		hist = hist_L0;
		break;
	case 1:
		hist = hist_L1;
		break;
	case 2:
		hist = hist_L2;
		break;
	case 3:
		hist = hist_L3;
		break;
	}
	uint16_t shiftLevel = histogramShiftLevels[pyramidLevel];
	double sum = 0, tempMean = 0, tempSum;
	uint32_t sampleCount = 0;
	for (uint32_t i = startIndex; i < endIndex; i++)
	{
		sampleCount += hist[i];
		tempSum = ((((int32_t) i) << shiftLevel) - offsetInInput) * hist[i]
				+ ((((int32_t) i + 1) << shiftLevel) - offsetInInput) * hist[i];
		sum += tempSum / 2;
	}
	tempMean = (sum / sampleCount);
	double sumOfSquaredDistances = 0, tempStd = 0;

	for (uint32_t i = startIndex; i < endIndex; i++)
	{
		double temp = tempMean
				- (double) ((((int32_t) i + 1) << shiftLevel) - offsetInInput);
		temp = (temp * temp) * hist[i];
		sumOfSquaredDistances += temp;
	}
	tempStd = sqrt(sumOfSquaredDistances / sampleCount);
	mean = (int32_t) tempMean;
	stdDev = (int32_t) tempStd;
}
uint32_t PyramidOfHistograms::findSampleCountInARange(int32_t startRange,
		int32_t endRange)
{
	startRange += offsetInInput;
	startRange = max(0, min(startRange, maxHistogramInputValue));

	endRange += offsetInInput;
	endRange = max(0, min(endRange, maxHistogramInputValue));

	uint32_t startIndex = startRange >> histogramShiftLevels[0];
	uint32_t endIndex = endRange >> histogramShiftLevels[0];

	uint32_t sum = 0;

	for (uint32_t i = startIndex; i < endIndex; i++)
	{
		sum += hist_L0[i];
	}
	return 0;

}
