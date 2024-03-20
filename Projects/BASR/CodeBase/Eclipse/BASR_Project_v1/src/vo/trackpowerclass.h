/*
 * trackpowerclass.h
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#ifndef TRACKPOWERCLASS_H_
#define TRACKPOWERCLASS_H_

#include "../common/common.h"
#include "../io/serialbuffer.h"
#include "../utility/consthigh.h"
#include "../delegate/PowerHistogram.h"
#include "../delegate/ScanAnalysisClass.h"
//verify all possible scan types
enum class EmitterArpEnum {scan_unknown, scan_circular, scan_sectorial, scan_lock, scan_conical,
	scan_tws, scan_cw, scan_complex, scan_missilelaunch, scan_cylindrical, scan_raster};

/*Platform:
{"N/A", "Ground", "Air", "Ship", "Single Cannon", "Double Cannon"}
    0,      1,      2,      3,          4,              5

Attribute:
{"N/A", "Friend", "Foe", "Ours"}
    0,      1,      2,      3

ARP Type:
{"Unknown", "Circular", "Sectorial", "Lock", "Conical", "TWS", "CW", "Complex", "Missile Launch"}
    0,          1,          2,          3,      4,          5,  6,      7,              8
*/
enum class GroundIlluminationEnum {no_axis, x_axis, y_axis};
//enum class DominantAntennaEnum {A,B,C,D};
struct ScanAnalysisHistoryClass
{
	EmitterArpEnum scanType;
	uint16_t score;

	EmitterArpEnum scanTypeOther;
	uint16_t scoreOther;

	uint64_t analysisTimestamp;
	uint64_t analysisToaMin;
	uint64_t analysisToaMax;
	uint16_t avgIlluminationTime;
	uint16_t avgPower;
	uint32_t arpMin;
	uint32_t arpMax;
	uint32_t analysisPulseCount;

	ScanAnalysisHistoryClass()
	{
		scanType = EmitterArpEnum::scan_unknown;
		analysisTimestamp= analysisToaMin=analysisToaMax=0;
		avgIlluminationTime=avgPower=0;
		arpMin=arpMax =analysisPulseCount=0;
	}
};
class TrackPowerClass {
public:

	vector<PowerHistogram> powerHistogram;
	vector <ScanAnalysisClass> scanAnalysis;
	vector<ScanAnalysisClassV2> scanAnalysis2;
	// power tracking variables
	float weightsVector[4];
	uint16_t lastPa;
	uint16_t lastPb;
	uint16_t lastPc;
	uint16_t lastPd;
	// pulse to pulse variables
	boost::circular_buffer<uint16_t> trackPowerA;	//to hold N last pulses
	boost::circular_buffer<uint16_t> trackPowerB;	//to hold N last pulses
	boost::circular_buffer<uint16_t> trackPowerC;	//to hold N last pulses
	boost::circular_buffer<uint16_t> trackPowerD;	//to hold N last pulses
	// per N pulses variables N=10 for now
	uint16_t pulseCountSinceLastPowerAnalysis;
	uint16_t pulseCountSinceLastScanAnalysis;
	uint16_t pulseCountSinceLastGroundIlluminationFlagsUpdate;
	uint16_t avgPA,stdPA;
	uint16_t avgPB,stdPB;
	uint16_t avgPC,stdPC;
	uint16_t avgPD,stdPD;
	// per reporting period variables, including clustering pulses
	uint16_t pulseCountSinceLastReport;//we need this set at low side
	uint32_t curMinStartPower;
	uint32_t curMinEndPower;
	uint32_t curMaxPower;
	uint64_t toaMinStartPower;
	uint64_t toaMinEndPower;
	uint8_t pxAntennaFirst;//stringing usage
	uint8_t pxAntennaLast;//stringing usage
	uint8_t pyAntennaFirst;//stringing usage
	uint8_t pyAntennaLast;//stringing usage
	uint16_t pxValueFirst;//stringing usage, do we need local maxima?
	uint16_t pxValueLast;//stringing usage, do we need local maxima?
	uint16_t pxMax,pyMax;//reported for bearing calculation at high
	uint16_t sumPxPyMax;
	uint8_t quadrantCodeMaxPxPy;//stringing usage, max pulse count
	uint64_t maxPowerToa;//reported for arp calculation at high
	uint16_t pyValueFirst;//stringing usage, do we need local maxima?
	uint16_t pyValueLast;//stringing usage, do we need local maxima?
	uint8_t quadrantCodeFirst;//stringing usage, max pulse count
	uint8_t quadrantCodeLast;//stringing usage, max pulse count
	uint32_t countQuadrantCodesIter[4];//count quadrants in last iteration
	uint8_t primaryAntennaFirst;//stringing usage
	uint8_t primaryAntennaLast;//stringing usage
	uint8_t dominantAntenna;
	uint8_t secondaryAntenna;
	uint8_t pxAntenna;
	uint8_t pyAntenna;
	//changes and work on variables below is pending/ongoing
	uint8_t groundIlluminationFlag;//stringing usage
	GroundIlluminationEnum groundIlluminationAxis;//stringing usage
	uint16_t countDominantAntenna[4]; // dominant antenna
	uint16_t countSecDominantAntenna[4]; // dominant antenna
	EmitterArpEnum scanTypeFirst;//stringing usage
	EmitterArpEnum scanTypeLast;//stringing usage
	uint32_t scanIlluminationTimeFirst;//stringing usage
	uint32_t scanIlluminationTimeLast;//stringing usage
	uint32_t scanPulseCountFirst;//stringing usage
	uint32_t scanPulseCountLast;//stringing usage
	uint64_t arpFirst;//stringing usage
	uint64_t arpLast;//stringing usage
	uint64_t timeToaScanAnalysisLast;
	uint8_t flagTrackingModeHigh;
	uint8_t flagScanningModeHigh;
	uint8_t flagMissileLaunchHigh;
	uint32_t countTrackingModeIterHigh;
	uint32_t countTrackingModeLowIterHigh;
	uint32_t countScanningModeIterHigh;
	uint32_t countScanningModeLowIterHigh;
	uint16_t pxMaxQuadrant[4];//computed at high side, maximum value of px per quadrant
	uint16_t pyMaxQuadrant[4];//computed at high side, maxumum value of py per quadrant
	uint8_t quadrantCodeMax;//to be computed at high side
	uint8_t flagResetQuadrantCodes;
	uint8_t cwGainStage;

	uint8_t flagScanAnalysisRequested;
	circular_buffer<IlluminationCycleInfo> illuminationInfo;
	circular_buffer<ScanAnalysisHistoryClass> scanAnalysisHistory;
	TrackPowerClass();
	void AllocateHistograms()
	{
		powerHistogram.resize(1);
		scanAnalysis.resize(1);
		scanAnalysis2.resize(1);
	}
	~TrackPowerClass();
	void serialize(SerialBufferClass & buffer);
	void copyParametersFromStringedEmitter(TrackPowerClass & other);
};


#endif /* TRACKPOWERCLASS_H_ */
