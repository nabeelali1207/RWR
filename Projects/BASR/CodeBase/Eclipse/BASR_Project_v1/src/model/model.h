/*
 * Model.h
 *
 *  Created on: Oct 15, 2018
 *      Author: hassan
 */

#ifndef MODEL_H_
#define MODEL_H_

#include "../common/common.h"
#include "../io/serialdatahandler.h"
#include "../io/trenzspilink.h"
#include "../io/cwpcqsilink.h"
#include "../io/difmcsilink.h"
#include "../io/cfduartlink.h"
#include "../io/pciethreatprocessor.h"
#include "../io/serializeobject.h"
#include "../vo/dataanalysisclass.h"
#include "../vo/dataliveclass.h"
#include "../vo/libraryclass.h"
#include "../vo/librarycfdclass.h"
#include "../io/livepdwreceiver.h"
#include "../vo/datasimulationlowclass.h"
#include "../io/fileoutputflightrecordhigh.h"
#include "../io/selftestccphigh.h"
#include "../delegate/selftestmflhighdelegate.h"
#include "../delegate/selftesthighdelegate.h"
#include "../io/xadctemperaturehigh.h"
#include "../io/threatprocessorbandswitchingtime.h"
#include "../io/watchdoghigh.h"
#include "../io/selftestcontrollerlow.h"
#include "../io/ethernetlinktp.h"
#include "../io/lowNetworkInterface.h"
#include "../io/highNetworkInterface.h"

class Model
{
public:
	static LibraryStruct library;
	static CfdLibraryStruct cfdLibrary;
	static LibraryMetaClass libraryMeta;
	static CfdLibraryMetaClass cfdLibraryMeta;

//	static SelfTestHighDelegate stDelegate;
	static CfCard cfCard;

	static Ethernetlinktp ethernetlinktp;
	//Failure Flags
	static bool cdFailure;
	static bool cwFailure;
	static bool fmrFailure;
	static bool analyzerFailure;
	static bool memoryFailure;
	static bool tlFailure;

	//Analysis Data Containers
	static DataSimulationLowClass simLowData;
	//put global start and running time of system here in both system and toa
	static DataAnalysisClass analysisLow;
	static DataAnalysisClass analysisHigh;
	static circular_buffer<DataAnalysisClass> analysisHistory;
	static FileOutputFlightRecordHigh fileoutputflightrecord;
	static EmitterLog emitterLog;
	static EmitterLog emitterLogLow;

	//added for CWP
	static circular_buffer<DataAnalysisClass> analysisCwpHistory;
	static DataAnalysisClass analysisCwp;
	static CwpCqsiLink cwpCqsiLink;
	static CwSelfTestResults cwSelfTestResults;

	static TrenzSpiLink trenzSpiLink;
	static DifmCsiLink difmCsiLink;
	static CfdUartLink cfdUartLink;
	static PcieThreatProcessor pcieThreatProcessor;
	static XadcTemperatureHigh xadcTemperatureHigh;
	static ThreatProcessorBandSwitchingTimeClass threatProcessorBandSwitchingTimeClass;
	static SelfTestCcpHigh selfTestCcpHigh;
	static SelfTestHighDelegate stDelegate;
	static SelfTestMflHigh selfTestMflHigh;
	static WatchdogHigh watchdogHigh;

	static uint64_t lastTrackingDataSentTime;
	static uint32_t selfTestDataBuffer[1000];

	//Interboard communication objects
	static LivePdwReceiver dspLivePdwReceiver;
	static SerializeClass classObjectSerializer;

	static SerialDataHandlerLow dspIoHandle;
	static SerialDataHandlerHigh cpuIoHandle;

	// Selftest controllers
	static SelfTestControllerLow dspSelfTestController;

	//Global Situation Variables
	static uint8_t ptyEnabled;
	static uint8_t noOfChannels;
	static uint8_t cfdPermission;

	// Selftest controllers
	static uint8_t dfRecieverSTStart;
	static uint8_t dfRecieverSTState;
	static uint8_t dfRecieverSTStatePause;
	static uint8_t dfRecieverAntennaLow;
	static uint8_t dfRecieverSTStateLow;
	static uint8_t dfRecieverBandLow;
	static uint8_t dfRecieverMsgLow;
	static uint8_t dfRecieverSetThresholdLow;
	static uint16_t chaffRemaining;
	static uint16_t flareRemaining;
	static uint16_t lastAudioToneId;
	static uint64_t lastAudioTrackId;
	static uint64_t lastChaffTrackId;
	static uint64_t lastFlareTrackId;
	static uint64_t timeSysMlWarningOn;
	static uint64_t timeSysUkWarningOn;
	static uint64_t timeSysAudioLast;
	static uint64_t timeSysChaffFire;
	static uint64_t timeSysFlareFire;

	//System and Simulation Variables
	static uint64_t simTimeMicrosecIterationActual;
	static uint64_t countThreadMainIterLow;
	static uint64_t countThreadMainIterHigh;
	static uint64_t toaLastMatFileBuffer;
	static uint8_t startupSystemTimeIsSet;
	static time_t startupSystemTime;

	//Standard output control
	static fstream stdoutFile;
	static streambuf *stream_buffer_cout;
	static streambuf *stream_buffer_file;

	//Time Management Variables
	static uint64_t timer1EpochMicroseconds;
	static uint64_t cdCwSelfTestStartTime;
	//static PdwIterationTimeSchedular simulationTimeSchedular;
	//put global start and running time of system here in both system and toa

	static LowNetworkInterface lowNetworkInterface;
	static HighNetworkInterface highNetworkInterface;


	Model();
	virtual ~Model();
};

//Definitions
#ifdef MODEL_CPP_

//Allocation for static variables of model
Model model;
CfCard Model::cfCard;
Ethernetlinktp Model::ethernetlinktp;
TrenzSpiLink Model::trenzSpiLink;
DifmCsiLink Model::difmCsiLink;
PcieThreatProcessor Model::pcieThreatProcessor;
XadcTemperatureHigh Model::xadcTemperatureHigh;
uint64_t Model::lastTrackingDataSentTime;
LibraryStruct Model::library;
CfdLibraryStruct Model::cfdLibrary;
LibraryMetaClass Model::libraryMeta;
CfdLibraryMetaClass Model::cfdLibraryMeta;
WatchdogHigh Model::watchdogHigh;
ThreatProcessorBandSwitchingTimeClass Model::threatProcessorBandSwitchingTimeClass;
SelfTestCcpHigh Model::selfTestCcpHigh;
SelfTestHighDelegate Model::stDelegate;
SelfTestMflHigh Model::selfTestMflHigh;

uint32_t Model::selfTestDataBuffer[1000];

//Failure Flags
bool	Model::cdFailure = false;
bool	Model::cwFailure = false;
bool	Model::fmrFailure = false;
bool	Model::analyzerFailure = false;
bool	Model::memoryFailure = false;
bool	Model::tlFailure = false;

//Analysis Data Containers
DataSimulationLowClass Model::simLowData;
DataAnalysisClass Model::analysisLow;
DataAnalysisClass Model::analysisHigh;
circular_buffer<DataAnalysisClass> Model::analysisHistory(2);
FileOutputFlightRecordHigh Model::fileoutputflightrecord;
EmitterLog Model::emitterLog;
EmitterLog Model::emitterLogLow;
circular_buffer<DataAnalysisClass> Model::analysisCwpHistory(1);
DataAnalysisClass Model::analysisCwp;
CwpCqsiLink Model::cwpCqsiLink;
CwSelfTestResults Model::cwSelfTestResults;
CfdUartLink Model::cfdUartLink;

//Interboard communication objects

LivePdwReceiver Model::dspLivePdwReceiver;
SerializeClass Model::classObjectSerializer;

SerialDataHandlerLow Model::dspIoHandle;
SerialDataHandlerHigh Model::cpuIoHandle;

// Selftest controllers
SelfTestControllerLow Model::dspSelfTestController;

//Global Situation Variables
uint8_t Model::ptyEnabled = 0;
uint8_t Model::dfRecieverSTStart = 0;
uint8_t Model::dfRecieverSTState = 1;
uint8_t Model::dfRecieverSTStateLow = 0;
uint8_t Model::dfRecieverSTStatePause = 0;
uint8_t Model::dfRecieverAntennaLow = 0;
uint8_t Model::dfRecieverBandLow = 0;
uint8_t Model::dfRecieverMsgLow = 0;
uint8_t Model::dfRecieverSetThresholdLow = 0;
uint8_t Model::cfdPermission = 0;
uint16_t Model::chaffRemaining = 0;
uint16_t Model::flareRemaining = 0;
uint16_t Model::lastAudioToneId = 0;
uint64_t Model::lastAudioTrackId = 0;
uint64_t Model::lastChaffTrackId = 0;
uint64_t Model::lastFlareTrackId = 0;
uint64_t Model::timeSysMlWarningOn = 0;
uint64_t Model::timeSysUkWarningOn = 0;
uint64_t Model::timeSysAudioLast = 0;
uint64_t Model::timeSysChaffFire = 0;
uint64_t Model::timeSysFlareFire = 0;

//System and Simulation Variables
uint64_t Model::simTimeMicrosecIterationActual = 0;
uint64_t Model::countThreadMainIterLow = 0;
uint64_t Model::countThreadMainIterHigh = 0;
uint64_t Model::toaLastMatFileBuffer = 0;
uint8_t	 Model::startupSystemTimeIsSet = 0;
time_t	 Model::startupSystemTime = time(NULL);

//Standard output control
fstream Model::stdoutFile;
streambuf* Model::stream_buffer_cout;
streambuf* Model::stream_buffer_file;

//Time Management Variables
uint64_t Model::timer1EpochMicroseconds = 0;
uint64_t Model::cdCwSelfTestStartTime = 0;
//PdwIterationTimeSchedular Model::simulationTimeSchedular;

LowNetworkInterface Model::lowNetworkInterface(1347);
HighNetworkInterface Model::highNetworkInterface(1347);

#else
//Global
#endif
//End

#endif /* MODEL_H_ */
