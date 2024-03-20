/*
 * PowerHistogram.h
 *
 *  Created on: Sep 9, 2020
 *      Author: tehseen
 */

#ifndef POWERHISTOGRAM_H_
#define POWERHISTOGRAM_H_
#include "../common/common.h"

struct PowerHistogramPeak
{
public:
	uint16_t rangeSampleCount;
	uint16_t rangeMinValue;
	uint16_t rangeMaxValue;

};
class PowerHistogram
{
	static const uint16_t HIST_SIZE=8192;

	uint16_t *histogram;
	int32_t	 peakCount;
	vector<PowerHistogramPeak> peaksFound;
	int8_t indexOfFoundPeak; 		// valid only when a peak has been found
public:
	uint16_t inputSampleCount;
	PowerHistogram();
	~PowerHistogram();
	PowerHistogram(const PowerHistogram & other);

	void operator=(const PowerHistogram & other);
	void clearHistogram();


	void addToHistogram(uint16_t inputValue);
	vector<PowerHistogramPeak> &getFoundPeaks();
	PowerHistogramPeak &getFoundPeak();
	int32_t getNumberOfPeaksFound();
	void findPeaks();
	uint16_t findMaxPowerFromPeaks();
};
struct ScanHistogramLevel
{
public:
	uint16_t levelPulseCount;
	uint16_t levelValue;
};
class ScanAnalysisHistogram
{
public:
	static const uint16_t SCAN_HIST_SIZE=10;

	uint16_t *histogram;
	int32_t	 levelCount;
	vector<ScanHistogramLevel> levelsFound;
public:
	uint16_t inputSampleCount;
	ScanAnalysisHistogram();
	~ScanAnalysisHistogram();
	ScanAnalysisHistogram(const PowerHistogram & other);

	void operator=(const ScanAnalysisHistogram & other);
	void clearHistogram();


	void addToHistogram(uint16_t inputValue);
	vector<ScanHistogramLevel> &getFoundLevels();
	int32_t getNumberOfLevelsFound();
	void findLevels();
};

#endif /* POWERHISTOGRAM_H_ */
