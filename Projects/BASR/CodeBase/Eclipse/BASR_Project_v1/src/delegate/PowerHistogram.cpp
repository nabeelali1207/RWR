/*
 * PowerHistogram.cpp
 *
 *  Created on: Sep 9, 2020
 *      Author: tehseen
 */

#include "PowerHistogram.h"

PowerHistogram::PowerHistogram()
{
	histogram = new uint16_t[HIST_SIZE];
	clearHistogram();
}
PowerHistogram::~PowerHistogram()
{
	delete[] histogram;
}

PowerHistogram::PowerHistogram(const PowerHistogram &other)
{
	histogram = new uint16_t[HIST_SIZE];
	(*this) = other;
}

void PowerHistogram::operator =(const PowerHistogram &other)
{
	inputSampleCount = other.inputSampleCount;
	indexOfFoundPeak = other.indexOfFoundPeak;
//	histogram = other.histogram;
	memcpy(histogram, other.histogram, HIST_SIZE * sizeof(uint16_t));

}
void PowerHistogram::clearHistogram()
{
	indexOfFoundPeak = -1;
	inputSampleCount = 0;
	memset(histogram, 0, HIST_SIZE * sizeof(uint16_t));
}

void PowerHistogram::addToHistogram(uint16_t inputValue)
{
	if (inputValue >= HIST_SIZE)
		inputValue = HIST_SIZE - 1;

//	if(inputValue==2244)
//		inputValue=2243+1;
	histogram[inputValue]++;
	inputSampleCount++;
}

vector<PowerHistogramPeak>& PowerHistogram::getFoundPeaks()
{
	return peaksFound;
}
PowerHistogramPeak& PowerHistogram::getFoundPeak()
{
	return peaksFound[indexOfFoundPeak];
}
int32_t PowerHistogram::getNumberOfPeaksFound()
{
	return peakCount;
}

void PowerHistogram::findPeaks()
{
	int32_t discontinuityCounter = 0;
	int16_t PeakThreshold = 1;
	int32_t peakSamplesCount = 0;
	int32_t PeakStartPoint = 0;
	int32_t PeakEndPoint = 0;
	PowerHistogramPeak tempPeak;
	peaksFound.resize(0);
	peakCount = 0;
	for (int32_t i = 0; i < HIST_SIZE; i++)
	{
		if (histogram[i] >= PeakThreshold) // PeakThreshold is noise floor
		{
			if (peakSamplesCount == 0)
			{
				PeakStartPoint = i;
			}
			PeakEndPoint = i;
			peakSamplesCount += histogram[i];
			discontinuityCounter = 0;
		}
		else
		{
			discontinuityCounter++; // spread break means current peak has ended
		}

		if (peakSamplesCount > 10
				&& (discontinuityCounter > 320 || i == (HIST_SIZE - 1)))
		{
			// valid peak
			peakCount++;
			// store peak related information to be used in the next level histogram
			tempPeak.rangeMinValue = PeakStartPoint;
			tempPeak.rangeMaxValue = PeakEndPoint;
			tempPeak.rangeSampleCount = peakSamplesCount;
			peaksFound.push_back(tempPeak);
			peakSamplesCount = 0;
		}
		else if(discontinuityCounter > 320)
		{
			peakSamplesCount = 0;
		}
	}

}
uint16_t PowerHistogram::findMaxPowerFromPeaks()
{
	int32_t validMaxPeakIndex = 0;
	uint16_t maxPeakStrength = 0;
	int32_t distanceFromOriginalPeak = 0;
	uint16_t maxPower = 0;
	if (peakCount == 1)
	{
		validMaxPeakIndex = 0;
	}
	else
	{
		for (int32_t i = 0; i < peakCount; i++)
		{
			if (peaksFound[i].rangeSampleCount > maxPeakStrength)
			{
				maxPeakStrength = peaksFound[i].rangeSampleCount;
				validMaxPeakIndex = i;
			}

		}

		for (int32_t i = peakCount - 1; i > 0; i--)
		{
			if (peaksFound[i].rangeSampleCount > 0.4 * maxPeakStrength)
			{
				distanceFromOriginalPeak = peaksFound[i].rangeMinValue
						- peaksFound[i - 1].rangeMaxValue;
				if (distanceFromOriginalPeak < 410)
				{
					validMaxPeakIndex = i;
					break;
				}

			}
		}
	}
	maxPower = peaksFound[validMaxPeakIndex].rangeMaxValue;
	indexOfFoundPeak = validMaxPeakIndex;
	return maxPower;
}

ScanAnalysisHistogram::ScanAnalysisHistogram()
{
	histogram = new uint16_t[SCAN_HIST_SIZE];
	clearHistogram();

}

ScanAnalysisHistogram::~ScanAnalysisHistogram()
{
	delete[] histogram;
}

ScanAnalysisHistogram::ScanAnalysisHistogram(const PowerHistogram &other)
{
	histogram = new uint16_t[SCAN_HIST_SIZE];
	(*this) = other;
}

void ScanAnalysisHistogram::operator =(const ScanAnalysisHistogram &other)
{
	inputSampleCount = other.inputSampleCount;
	memcpy(histogram, other.histogram, SCAN_HIST_SIZE * sizeof(uint16_t));
}

void ScanAnalysisHistogram::clearHistogram()
{
	inputSampleCount = 0;
	memset(histogram, 0, SCAN_HIST_SIZE * sizeof(uint16_t));
}

void ScanAnalysisHistogram::addToHistogram(uint16_t inputValue)
{
	histogram[inputValue]++;
	inputSampleCount++;
}

vector<ScanHistogramLevel>& ScanAnalysisHistogram::getFoundLevels()
{
	return levelsFound;
}

int32_t ScanAnalysisHistogram::getNumberOfLevelsFound()
{
	return levelCount;
}

void ScanAnalysisHistogram::findLevels()
{
	int16_t PeakThreshold = 1;
	int32_t peakSamplesCount = 0;
	ScanHistogramLevel templevel;
	levelsFound.resize(0);
	levelCount = 0;
	for (int32_t i = 0; i < SCAN_HIST_SIZE; i++)
	{
		if (histogram[i] >= PeakThreshold) // PeakThreshold is noise floor
		{
			peakSamplesCount = histogram[i];
		}
		if (peakSamplesCount > 0)
		{
			// valid peak
			levelCount++;
			templevel.levelValue = i;
			templevel.levelPulseCount = peakSamplesCount;
			levelsFound.push_back(templevel);
		}
	}

}
