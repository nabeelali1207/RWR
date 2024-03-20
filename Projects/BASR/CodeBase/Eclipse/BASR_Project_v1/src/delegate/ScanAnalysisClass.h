/*
 * analysisscanlowdelegate.h
 *
 *  Created on: Sep 11, 2020
 *      Author: tehseen
 */

#ifndef SCANANALYSISCLASS_H_
#define SCANANALYSISCLASS_H_
#include "../common/common.h"
#include "PowerHistogram.h"
#include <boost/circular_buffer.hpp>
#include "../vo/pdwdefaultclass.h"

using boost::circular_buffer;
enum class EmitterMode
{
	scan,track,sector,conical,unknown
};
class ScanAnalysisClass
{
	EmitterMode currMode;
	ScanAnalysisHistogram scanHistogram;
	circular_buffer<PdwDefaultClass> scanPdwBuffer;
	int32_t pulseCount;
	vector<uint8_t> powerLvlsPattern;
	int16_t ConcMatrix[10][10];
	int8_t buffersincelastReport;
	uint16_t bufferMaxPower;

public:
	ScanAnalysisClass();
	~ScanAnalysisClass();
	ScanAnalysisClass(const ScanAnalysisClass& other);
	void operator=(const ScanAnalysisClass&other);
	void FillScanBuffer(PdwDefaultClass &pdw);
	void SetMaxPower(uint16_t maxPower);
	void ScanAnalysis(uint8_t dominantAntenna);
	void CreateConcMatrix();
	EmitterMode GetEmitterMode();
	EmitterMode DetectScanType();
	void ClearData();
};
struct IlluminationCycleInfo
{
	uint64_t cycleStartTime;
	uint64_t cycleDuration;
	uint32_t cyclePulseCount;
	uint32_t peakPowerDuration;
	uint64_t minPowerTime;
	uint64_t maxPowerTime;
	uint64_t maxPowerStartTime;
	uint64_t maxPowerEndTime;
	uint16_t startPower;
	uint16_t endPower;
	uint16_t maxPower;
	uint16_t minPower;
	uint16_t meanPower;
	uint16_t variance;
	uint16_t kurtosis; //
	IlluminationCycleInfo()
	{
		cycleStartTime=minPowerTime=maxPowerTime=cycleDuration=0;
		cyclePulseCount = peakPowerDuration = 0;
		startPower=endPower=maxPower=minPower=meanPower=variance=kurtosis=0;
	}
	void takeUnion(IlluminationCycleInfo&other);

	//take info from second and push into first
	static void takeUnion(circular_buffer<IlluminationCycleInfo> &first,circular_buffer<IlluminationCycleInfo>&second);
};
class ScanAnalysisClassV2
{
	EmitterMode currMode;

	struct PowerHistBin
	{
		uint8_t power;
		uint8_t pulseCount;
	};

	static const uint32_t BUFF_ENTRY_COUNT=600;
	PowerHistBin powerLvls[BUFF_ENTRY_COUNT];

	vector<IlluminationCycleInfo> illuminationCycleInfo;


	uint64_t toaOffset;
	static const uint8_t TIME_DOWN_SHIFT=16; //downShiftBits 2.^16 equals 131072 x 10 nano seconds = 1.31Millis
	static const uint8_t POWER_DOWN_SHIFT=4; //downscale power by 16

	uint32_t accumulatedPower;
	uint32_t accumulatedPowerCount;
	uint32_t accumulatedPowerIndex;
public:
	ScanAnalysisClassV2();
	ScanAnalysisClassV2(const ScanAnalysisClassV2& other);
	void operator=(const ScanAnalysisClassV2&other);

	void addFirstPdw(PdwDefaultClass &pdw);
	void addNextPdw(PdwDefaultClass &pdw);
	void performScanAnalysis();
	void clearAll();
	void makeSpaceForNewIncomingData();

	vector<IlluminationCycleInfo> & getCycleInfo()
	{
		return illuminationCycleInfo;
	}
};
#endif /* SCANANALYSISCLASS_H_ */
