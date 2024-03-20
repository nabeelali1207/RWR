/*
 * constlow.h
 *
 *  Created on: Jan 1, 2019
 *      Author: hassan
 */

#ifndef CONSTLOW_H_
#define CONSTLOW_H_

#include "../common/common.h"

//notes
//constants may need to be defined using the const keyword to avoid dynamic
//..allocation problems when using these values to initialize container sizes
//end of notes

enum class LowIterationTypeFlags
{
	simulationModeDisabled=0,
	fixedTimeIterationFastSimulation=1,
	fixedTimeIterationSlowSimulation=2,
	variableTimeIterationWithMaxTimeLimit=3,
	variableTimeIterationWithMinMaxTimeLimit=4,
	readIterationTimeFromFile=5
};
class ConstLow
{
public:
	//Analysis Constants
	static uint8_t asRollPdwIterFlag;
	static uint8_t asRollPdwIterMaxCount;

	//Analysis Tracking Constants
	static uint16_t asTrackingAoaWindow;
	static uint16_t asTrackingAoaLowerWindow;
	static uint16_t asTrackingRfWindow;
	static uint16_t asTrackingPwWindow;
	static uint16_t asTrackingMinScoreThreshold; //minimum score required by a pulse to associated with a track
	static uint16_t asTrackingPdwMaxIter;

	//Analysis Clustering Constants
	static uint32_t asClusterMaxTimeWindowMillis;
	static uint16_t asClusterMinPulsesToDeclareTrack;
	static uint8_t asClusterPdwReqCount;
	static uint16_t asClusterMaxInput;
	static uint16_t asClusterMinInput;
	static uint8_t asClusterPdwMaxIter;
	static uint8_t asClusterDbScanDimensionCount;
	static uint16_t asClusterPowerMax;
	static uint32_t asClusterPulseWidthMax;
	static uint16_t asClusterRfMax;
	static float asClusterDbScanEpsilon;
	static uint16_t asClusterDbScanMinNeighborCount; //valid clusters are those with minimum neighbors
	static uint8_t asClusterDbScanThreadCount;

	//Analysis Power
	static uint16_t asPowerThreshBandCD;
	static uint16_t asPowerThreshBandEJ;
	static uint16_t asPowerThreshBandK;
	static uint16_t asPowerThreshPxPyCombinedBandCD;
	static uint16_t asPowerThreshPxPyCombinedBandEJ;
	static uint16_t asPowerThreshPxPyCombinedBandK;
	static uint16_t asPowerThreshPxPyClusteringBandCD;//asPowerThreshPxPyClusteringBandCD
	static uint16_t asPowerThreshPxPyClusteringBandEJ;
	static uint16_t asPowerThreshPxPyClusteringBandK;
	static uint16_t asPowerPdwCountMin;
	static uint16_t asPowerDefualtVariation;

	static uint16_t asGroundIllumPdwCountMin;

	//Analysis Stringing Constants
	static uint32_t asStringingMaxTimeWindow;

	//Analysis Pri Constant
	static uint16_t asPriPdwCountMax;
	static uint16_t asPriPdwCountMin;
	static uint16_t asPriDiffToaDepthMax;

	//Analysis Rf Constant
	static uint16_t asRfPdwCountMin;
	static uint16_t asRfDefualtVariation;

	//Analysis Pw Constant
	static uint16_t asPwPdwCountMin;
	static uint16_t asPwDefualtVariation;

	//Analysis AOA Constant
	static uint16_t asAoaPdwCountMin;

	//File Name Constants
	static std::string matInFileName;
	static std::string csvOutFileName;
	static std::string hexDebugFileName;
	static std::string hexDebugIterDataFileName;
	static std::string dspReportFileName;
	static std::string dspPdwFlushFileName;
	//Scalar Constants
	static uint64_t toaCountInMicros;
	static uint64_t toaCountInMs;
	static uint64_t toaCountIn10Ms;
	static uint64_t toaCountIn100Ms;
	static uint64_t toaCountInSecond;
	static uint64_t toaCountIn2Second;
	static uint64_t toaCountIn3Second;
	static uint64_t toaCountIn5Second;
	static uint64_t toaCountIn10Second;
	static uint64_t toaCountInMinute;
	static uint64_t toaCountInFiveMin;
	static uint64_t toaCountInTenMin;

	//Simulation Constants
	static uint64_t simIterationMaxTime;
	static uint64_t simIterationFixedTime;
	static uint64_t simIterationMinTime;
	static LowIterationTypeFlags simIterationTypeFlag;
	static uint8_t simOnActualHwFlag;
	static uint8_t simTimeLimitFlag;
	static uint8_t simTimeLimitScaleFlag;
	static uint64_t simTimeStartOffsetMs;
	static uint64_t simTimeEndOffsetMs;
	static uint64_t simTimeStartToa;
	static uint64_t simTimeEndToa;
	static uint8_t simOutRawPdwFileFlag;
	static uint8_t simFastFlag;
	static uint8_t cleanPdwSimFraction;
	static uint8_t cleanPdwSimInputFlag;
	static uint8_t cleanPdwHwFraction;
	static uint8_t cleanPdwHwInputFlag;

	//Track Constants
	static uint16_t trackPdwBuffCircSize;
	static uint16_t trackPdwBuffBaseSize;
	static uint16_t trackPriBuffCircSize;
	static uint16_t trackPriDwellBuffSize;
	static uint16_t trackPriSlideBuffSize;
	static uint16_t trackPriJitterBuffSize;
	static uint16_t trackPriStaggerBuffSize;
	static uint16_t trackPriStaggerFrameBuffSize;
	static uint16_t trackPriComplexBuffSize;
	static uint16_t trackPriUnknownBuffSize;
	static uint16_t trackPwBuffCircSize;
	static uint16_t trackRfBuffCircSize;
	static uint16_t trackScanBuffSize;
	static uint16_t trackMinPdwCount; // minimum pulses required to declare a track

	//Logging Constants
	static bool readThreatDataFromEthernet;
	static bool logOverallFlag;
	static bool logPrintEmitters;
	static bool logDspCsvReportFlag;
	static bool logDspReportFlag;
	static bool logRecordLivePdws;
	static bool logHexFile;
	static bool logHexFileTimeBenchmarks;
	static bool logHexFileClusteringData;
	static bool logHexFilePriData;
	static bool logHexFileTrackData;
	static bool logHexFileIterationInfo;

	//libgomp Constants
	static uint16_t threadCountPriAnalysis;

	//Watch Dog Constants
	static std::string watchDogFileName;
	static uint8_t watchDogEnabledFlag;

	ConstLow();
	virtual ~ConstLow();
};

//Definitions
#ifdef CONSTLOW_CPP_

//Allocation for static variables of model
ConstLow constlow;

//Analysis Constants
uint8_t ConstLow::asRollPdwIterFlag = 1;
uint8_t ConstLow::asRollPdwIterMaxCount = 1;
uint8_t ConstLow::asClusterPdwReqCount = 7;

//Analysis Tracking Constants
uint16_t ConstLow::asTrackingAoaWindow 	=90;
uint16_t ConstLow::asTrackingAoaLowerWindow 	=7;
uint16_t ConstLow::asTrackingRfWindow	=1800;
uint16_t ConstLow::asTrackingPwWindow 	=50; // in 100Mhz Terms 100=>1 micro second
uint16_t ConstLow::asTrackingMinScoreThreshold	=60;
uint16_t ConstLow::asTrackingPdwMaxIter = 1;
//Analysis Clustering Constants
uint32_t ConstLow::asClusterMaxTimeWindowMillis 	= 120;
uint16_t ConstLow::asClusterMinPulsesToDeclareTrack = 8;
uint16_t ConstLow::asClusterMaxInput = 128;
uint16_t ConstLow::asClusterMinInput = 16;
uint8_t ConstLow::asClusterPdwMaxIter = 2;
uint8_t ConstLow::asClusterDbScanDimensionCount = 5;
uint16_t ConstLow::asClusterPowerMax = 4096;
uint32_t ConstLow::asClusterPulseWidthMax = 100000;
uint16_t ConstLow::asClusterRfMax = 18000;

float ConstLow::asClusterDbScanEpsilon = 0.1;
uint16_t ConstLow::asClusterDbScanMinNeighborCount=4;
uint8_t ConstLow::asClusterDbScanThreadCount = 4;

//Analysis Power Constant
uint16_t ConstLow::asPowerThreshBandCD  = 16 * 20;
uint16_t ConstLow::asPowerThreshBandK  = 16 * 20;
uint16_t ConstLow::asPowerThreshBandEJ = 16 * 20;

uint16_t ConstLow::asPowerThreshPxPyCombinedBandCD  = 16 * 20;
uint16_t ConstLow::asPowerThreshPxPyCombinedBandK  = 16 * 20;
uint16_t ConstLow::asPowerThreshPxPyCombinedBandEJ = 16 * 20;

uint16_t ConstLow::asPowerThreshPxPyClusteringBandCD  = 16 * 20;
uint16_t ConstLow::asPowerThreshPxPyClusteringBandK  = 16 * 20;
uint16_t ConstLow::asPowerThreshPxPyClusteringBandEJ = 16 * 20;

uint16_t ConstLow::asPowerPdwCountMin  = 8;
uint16_t ConstLow::asPowerDefualtVariation = 160;

uint16_t ConstLow::asGroundIllumPdwCountMin  = 8;

//Analysis Stringing Constants
uint32_t ConstLow::asStringingMaxTimeWindow = 8000000;

//Analysis Pri Constant
uint16_t ConstLow::asPriPdwCountMax = 128;
uint16_t ConstLow::asPriPdwCountMin = 8;
uint16_t ConstLow::asPriDiffToaDepthMax = 32;

//Analysis Rf Constant
uint16_t ConstLow::asRfPdwCountMin		=8;
uint16_t ConstLow::asRfDefualtVariation =10;

//Analysis Pw Constant
uint16_t ConstLow::asPwPdwCountMin	=8;
uint16_t ConstLow::asPwDefualtVariation	=10;

//Analysis AOA Constant
uint16_t ConstLow::asAoaPdwCountMin=8;

//File Name Constants
std::string ConstLow::matInFileName = "input/Case_4.9_PUP1.5_Version1.mat";
//std::string ConstLow::matInFileName = "input/130508_Flt1_of_1_2100_2600secs.mat";
//std::string ConstLow::csvOutFileName = "output/RawMatFile.csv";
std::string ConstLow::csvOutFileName = "output/lowReport.csv";    // Tehseen updated 29.03.20
std::string ConstLow::hexDebugFileName= "output/debugLogFile.hex";
std::string ConstLow::hexDebugIterDataFileName= "output/iterdata_debugLogFile.hex";
std::string ConstLow::dspReportFileName= "output/dspReport.hex";
std::string ConstLow::dspPdwFlushFileName= "output/livePdwsFlush.hex";
//Scalar Constants
uint64_t ConstLow::toaCountInMicros		= 100;
uint64_t ConstLow::toaCountInMs 		= 100000;
uint64_t ConstLow::toaCountIn10Ms 		= 1000000;
uint64_t ConstLow::toaCountIn100Ms 		= 10000000;
uint64_t ConstLow::toaCountInSecond 	= 100000000;
uint64_t ConstLow::toaCountIn2Second 	= 200000000;
uint64_t ConstLow::toaCountIn3Second 	= 300000000;
uint64_t ConstLow::toaCountIn5Second 	= 500000000;
uint64_t ConstLow::toaCountIn10Second 	= 1000000000;
uint64_t ConstLow::toaCountInMinute 	= 6000000000;
uint64_t ConstLow::toaCountInFiveMin 	= 30000000000;
uint64_t ConstLow::toaCountInTenMin 	= 60000000000;

//Simulation Constants
uint64_t ConstLow::simIterationMaxTime		= 30;
uint64_t ConstLow::simIterationFixedTime 	= 1;
uint64_t ConstLow::simIterationMinTime 		= 1;
LowIterationTypeFlags ConstLow::simIterationTypeFlag=LowIterationTypeFlags::fixedTimeIterationFastSimulation;
uint8_t ConstLow::simOnActualHwFlag 		= 0;
uint8_t ConstLow::simTimeLimitFlag 			= 0;
uint8_t ConstLow::simTimeLimitScaleFlag 	= 0;
uint64_t ConstLow::simTimeStartOffsetMs 	= 0;
uint64_t ConstLow::simTimeEndOffsetMs 		= 0;
uint64_t ConstLow::simTimeStartToa 			= 0;
uint64_t ConstLow::simTimeEndToa 			= 0;
uint8_t ConstLow::simFastFlag 				= 0;
uint8_t ConstLow::simOutRawPdwFileFlag 		= 0;
uint8_t ConstLow::cleanPdwSimFraction 		= 40;
uint8_t ConstLow::cleanPdwSimInputFlag 		= 1;
uint8_t ConstLow::cleanPdwHwFraction 		= 5;
uint8_t ConstLow::cleanPdwHwInputFlag 		= 0;

//Track Constants
uint16_t ConstLow::trackPdwBuffCircSize 	= 2048;
uint16_t ConstLow::trackPdwBuffBaseSize 	= 256;
uint16_t ConstLow::trackPriBuffCircSize 	= 96;
uint16_t ConstLow::trackPriDwellBuffSize 	= 32;
uint16_t ConstLow::trackPriSlideBuffSize 	= 1;
uint16_t ConstLow::trackPriJitterBuffSize 	= 1;
uint16_t ConstLow::trackPriStaggerBuffSize 	= 32;
uint16_t ConstLow::trackPriStaggerFrameBuffSize = 4;
uint16_t ConstLow::trackPriComplexBuffSize 	= 32;
uint16_t ConstLow::trackPriUnknownBuffSize 	= 64;
uint16_t ConstLow::trackPwBuffCircSize 		= 16;
uint16_t ConstLow::trackRfBuffCircSize 		= 32;
uint16_t ConstLow::trackScanBuffSize 		= 1;
uint16_t ConstLow::trackMinPdwCount 		= 16;
//libgomp Constants
uint16_t ConstLow::threadCountPriAnalysis = 2;

//Logging Constants
bool ConstLow::readThreatDataFromEthernet 	= false;
bool ConstLow::logOverallFlag 				= false;
bool ConstLow::logPrintEmitters 			= false;
bool ConstLow::logDspCsvReportFlag 			= false;
bool ConstLow::logDspReportFlag 			= false;
bool ConstLow::logRecordLivePdws 			= false;
bool ConstLow::logHexFile					= false;
bool ConstLow::logHexFileTimeBenchmarks		= false;
bool ConstLow::logHexFileClusteringData		= false;
bool ConstLow::logHexFilePriData			= false;
bool ConstLow::logHexFileTrackData			= false;
bool ConstLow::logHexFileIterationInfo		= false;

//Watch Dog Constants
std::string ConstLow::watchDogFileName = "/dev/watchdog0";
uint8_t ConstLow::watchDogEnabledFlag=1;

#else
//Global
#endif
//End

#endif /* CONSTLOW_H_ */
