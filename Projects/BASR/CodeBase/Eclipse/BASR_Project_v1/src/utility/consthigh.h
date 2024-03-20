/*
 * consthigh.h
 *
 *  Created on: Jan 1, 2019
 *      Author: hassan
 */

#ifndef CONSTHIGH_H_
#define CONSTHIGH_H_

#include "../common/common.h"

//notes
//constants may need to be defined using the const keyword to avoid dynamic
//..allocation problems when using these values to initialize container sizes
//end of notes

class ConstHigh
{
public:

	static uint8_t SW_VER_MIN_1;
	static uint8_t SW_VER_MIN_2;
	static uint8_t SW_VER_MAJ_1;
	static uint8_t SW_VER_MAJ_2;
	static uint16_t SW_VER_YEAR;
	static uint8_t SW_VER_MONTH;
	static uint8_t SW_VER_DATE;

	static std::string cwHexDumpFileName;
	static std::string cpuDumpFileName;
	static std::string recDataFromDspDumpFileNameCurrent;
	static std::string recDataFromDspDumpFileNameDesktop;
	static std::string recDataFromDspDumpFileNameHardware;
	static std::string dspReportFileNameCurrent;
	static std::string dspReportFileNameDesktop;
	static std::string dspReportFileNameHardware;
	//we need variables combination to be for sim data on pc, sim data on
	//actual hardware, live on hardware
	static uint8_t simOnActualHwFlag;
	static uint8_t simLogIterationHigh;
	static uint8_t readingFromLvds;
	static uint8_t simTimeSyntheticFlag;
	static uint8_t cwReportsOn;
	static uint8_t cwReportsLogOn;

	//Track and Merge Constants
	static uint16_t trackPriBuffAccumulateCircSize;
	static uint16_t trackPwBuffAccumulateCircSize;
	static uint16_t trackRfBuffAccumulateCircSize;
	static uint16_t trackIdsLowAccumulateCircSize;
	static uint16_t trackLimitRangeCalcIterDefault;
	static uint16_t trackLimitBearingCalcIterDefault;
	static uint16_t trackLimitRangeCalcIterAerial;
	static uint16_t trackLimitBearingCalcIterAerial;
	static uint16_t trackLimitMaxEnvironmentSize;
	static uint16_t trackLimitMaxEnvironmentIterLifetime;
	static uint16_t trackLimitMaxEmittersHigh;
	static uint16_t trackLimitMaxConsecutiveUpdateIter;
	static uint16_t trackLimitPartialConsecutiveUpdateIter;
	static uint16_t mergeScoreStringRequired;
	static uint16_t mergeScoreStringBand;
	static uint16_t mergeScoreStringBandEstimate;
	static uint16_t mergeDeltaStringRf;
	static uint16_t mergeScoreStringRf;

	static uint16_t mergeLimitStringBearingPrimaryLarge;
	static uint16_t mergeLimitStringBearingSecondaryLarge;
	static uint16_t mergeLimitStringBearingPrimary;
	static uint16_t mergeLimitStringBearingSecondary;
	static uint16_t mergeScoreStringBearingMax;
	static uint16_t mergeLimitEnvironmentRedeemBearing;
	static uint16_t mergeLimitFinalizeTrackBearing;
	static uint16_t mergeLimitMergeLowBandBearingPrimary;
	static uint16_t mergeLimitMergeLowBandBearingSecondary;
	static uint16_t mergeLimitMergePriStaggerBearingPrimary;
	static uint16_t mergeLimitMergePriStaggerBearingSecondary;
	static uint16_t mergeLimitMergeDefaultBearingPrimary;
	static uint16_t mergeLimitMergeDefaultBearingSecondary;
	static uint16_t mergeDeltaMergeBearingSmall;
	static uint16_t mergeDeltaMergeBearingMedium;
	static uint16_t mergeDeltaMergeBearingLarge;

	static uint16_t mergeDeltaMergePowerSmall;
	static uint16_t mergeDeltaMergePowerMedium;
	static uint16_t mergeDeltaMergePowerLarge;
	static uint16_t mergeDeltaMergePowerExcludePriCertain;
	static uint16_t mergeDeltaMergePowerExcludePriUncertain;
	static uint16_t mergeDeltaStringPower;
	static uint16_t mergeScoreStringPower;
	static uint16_t mergeDeltaStringPwLarge;
	static uint16_t mergeDeltaStringPwMedium;
	static uint16_t mergeDeltaStringPwSmall;
	static uint16_t mergeDeltaMergePwLarge;
	static uint16_t mergeDeltaMergePwMedium;
	static uint16_t mergeDeltaMergePwSmall;
	static uint16_t mergeScoreStringPwLarge;
	static uint16_t mergeScoreStringPwMedium;
	static uint16_t mergeScoreStringPwSmall;
	static uint16_t mergeDeltaStringPx;
	static uint16_t mergeDeltaStringPy;
	static uint16_t mergeScoreStringPx;
	static uint16_t mergeScoreStringPy;
	static uint64_t mergeDeltaStringToaDiffSmall;
	static uint64_t mergeDeltaStringToaDiff;
	static uint16_t mergeScoreStringToaDiff;
	static uint16_t mergeScoreStringPriWeak;
	static uint16_t mergeScoreStringPriType;
	static uint16_t mergeScoreStringLibraryPartial;
	static uint16_t mergeScoreStringLibraryComplete;
	static uint16_t mergeScoreStringPriUncertain;
	static uint16_t mergeDeltaStringPriFixedLarge;
	static uint16_t mergeDeltaStringPriFixedMedium;
	static uint16_t mergeDeltaStringPriFixedSmall;
	static uint16_t mergeScoreStringPriFixedLarge;
	static uint16_t mergeScoreStringPriFixedMedium;
	static uint16_t mergeScoreStringPriFixedSmall;
	static uint16_t mergeDeltaStringPriDwellSwitchLarge;
	static uint16_t mergeDeltaStringPriDwellSwitchMedium;
	static uint16_t mergeDeltaStringPriDwellSwitchSmall;
	static uint16_t mergeScoreStringPriDwellSwitchLarge;
	static uint16_t mergeScoreStringPriDwellSwitchMedium;
	static uint16_t mergeScoreStringPriDwellSwitchSmall;
	static uint16_t mergeDeltaStringPriStaggerPhase;
	static uint16_t mergeDeltaStringPriStaggerFrame;
	static uint16_t mergeDeltaStringPriStaggerFrameDwellSwitch;
	static uint16_t mergeScoreStringPriStaggerFrameDwellSwitch;
	static uint16_t mergeScoreStringPriStaggerPhase;
	static uint16_t mergeScoreStringPriStaggerFrame;
	static uint16_t mergeDeltaStringPowerExclusion;
	static uint16_t mergeDeltaStringPowerPriUncertainExclusion;
	static uint16_t mergeDeltaStringRfExcludeSmall;
	static uint16_t mergeDeltaStringRfExcludeMedium;
	static uint16_t mergeDeltaStringRfExcludeLarge;
	static uint16_t mergeScoreStringRequiredTotal;

	//Bearing Calculation
	static int16_t bearingDisplayFilterLimit;

	//Range Calculation
	static float rangePowerNormalizationTable[46];

	//Library constants
	static std::string libraryPathHardware;
	static std::string libraryKeyPathHardware;
	static std::string libraryPathDesktop;
	static std::string libraryKeyPathDesktop;
	static std::string libraryPathCurrent;
	static std::string libraryKeyPathCurrent;
	static uint32_t libraryScoreDefaultThreshold;
	static uint32_t libraryCountPriDefaultThreshold;
	static uint8_t  libraryPriTypeScore;
	static uint8_t  libraryPwTypeScore;
	static uint8_t  libraryRfTypeScore;
	static uint8_t  libraryPriScore;
	static uint8_t  libraryPwScore;
	static uint8_t  libraryRfScore;
	//Flight record constants
	static std::string flightRecordPathHardware;
	static std::string flightRecordPathDesktop;
	static std::string flightRecordPathCurrent;

	static bool flightRecordEncrypted;
	static bool lowLogEncrypted;
	static bool flightRecordAppendMode;

	//Time Constants
	static uint32_t timeIterCountEquivalent200ms;
	static uint32_t timeIterEquivalent200ms;
	static uint32_t timeIterEquivalent400ms;
	static uint32_t timeIterEquivalent600ms;
	static uint32_t timeIterEquivalent800ms;
	static uint32_t timeIterEquivalent1000ms;
	static uint32_t timeIterEquivalent2000ms;
	static uint32_t timeIterEquivalent3000ms;
	static uint32_t timeIterEquivalent4000ms;
	static uint32_t timeIterEquivalent5000ms;
	static const char *faultDescriptor[];
	static const char *faultLru[];
	static const int faultFctn[];
	static int testVar[2][4];
	static F64 cwPowerCorrectionLookup[46];
	static int CPF_ERROR_COUNT;
	static int PPF_ERROR_COUNT;
	static int CCP_ERROR_COUNT;
	static int TP_ERROR_COUNT;
	static int DIFM_ERROR_COUNT;
	static int DIFM_AUTO_ERROR_COUNT;
	static int CWP_ERROR_COUNT;
	static int CWP_AUTO_ERROR_COUNT;
	static int DCM_ERROR_COUNT;
	static int DCM_AUTO_ERROR_COUNT;
	static int DRU_ERROR_COUNT;
	static int TOTAL_ERROR_COUNT;

	//CFD Constants
	static int cfdUp;
	static int cfdInit;
	static int cfdPause;
	static int ecmMode;
	static int presetChaff;
	static int presetFlare;
	static int programNum;
	static int cfdType;
	static int cfdFailCount;
	static vector<int> cfdThreatIds;

	//TRENZ Constants
	static UWord8 sopAsyncAudioFlag;
	static UWord8 sopMissileLaunch;
	static UWord8 sopBeingTracked;
	static UWord8 sopThreatFound;
	static UWord8 pflCheck;
	static UWord8 spiAudioRequest;

	//ST Constants
	static UWord8 StopFst;
	static UWord8 sendShutdown;
	static UWord8 FstInProgress;
	static UWord8 FstRetryCount;
	static UWord8 mStRetryCount; // modular self test retry count
	static UWord32 FstMaxDuration; // time in millis
	static UWord32 FstCurrTryDuration; // time in millis
	

	ConstHigh();
	virtual ~ConstHigh();
};

//Definitions
#ifdef CONSTHIGH_CPP_

//Allocation for static variables of model
ConstHigh constHigh;

const char* ConstHigh::faultDescriptor[] =
{
    "",

    "CPF - 1553 DMA TEST", "CPF - CSI LINK FAULT", "CPF - AUDIO PLL FAULT",
    "CPF - AUDIO LEFT CHANNEL DAC FAULT", "CPF - AUDIO RIGHT CHANNEL DAC FAULT",
    "CPF - 1553 ENCODER BUS A FAULT", "CPF - 1553 ENCODER BUS B FAULT", "CPF - 1553 RT ADDRESS FAULT",
    "CPF - CSI TIMEOUT FAULT", "CPF - CSI RX TIMEOUT FAULT", "TRENZ TEMPERATURE FAULT",

    "PPF - PCIE TRIGGERED TEST", "PPF - ADC TRIGGERED TEST", "PPF - AURORA LINK FAULT",
    "PPF - AURORA CHANNEL FAULT", "PPF - AURORA CRC FAULT",  "PPF - PCIE LINK FAULT",
    "PPF - ADC CONFIGURATION FAULT", "PPF - CQSI LINK FAULT", "PPF - CQSI TIMEOUT FAULT",
    "PPF - CSI LINK FAULT", "PPF - CSI-DIFM TIMEOUT FAULT", "PPF - CSI-CCP RX TIMEOUT FAULT",
    "PPF - CQSI RX TIMEOUT FAULT", "PPF - CSI-DIFM RX TIMEOUT FAULT", "VDM DETECTION FAILURE",

    "CCP - CF CARD EXISTENCE FAULT", "CCP - EMMC EXISTENCE FAULT", "CCP - CF CARD FULL",
    "CCP - EMMC FULL", "CCP - CF CARD READ WRITE FAULT", "CCP - EMMC READ WRITE FAULT",
    "CCP - THREAT LIBRARY EXISTENCE FAULT", "CCP - THREAT LIBRARY CRC FAULT", "CCP - THREAT LIBRARY KEY FAULT",
    "CCP - THREAT LIBRARY SELECTION FAULT", "PICOZED TEMPERATURE FAULT",

    "TP - ARM CORES FAILURE", "TP - DSP CORES FAILURE", "TP - FLASH MEMORY TEST FAILURE", "TP - RAM FAILURE",
    "TP - USB TEST FAILURE",  "TP - PCIE DATA LOOPBACK FAULT", "TP TEMPERATURE FAULT",

    "DIFM - ADC SYNC TEST FAILURE", "DIFM CSI-CCP RX TIMEOUT FAULT", "DIFM CSI-CCP MDF CRC FAULT",
    "DIFM TO CWP LINK FAILURE", "DIFM - AURORA LINK UP FAULT", "DIFM - AURORA CHANNEL UP FAULT",
    "DIFM - AURORA CRC FAULT", "DIFM FPGA FATAL ERROR", "DIFM DETECTION FAILURE", "DIFM CSI-CCP MDF RECEIPT FAULT",
    "ADC CALIBRATION FAULT", "ADC IC TEMPERATURE FAULT", "ADC CARD TEMPERATURE", "DIFM FPGA XADC TEMPERATURE",
    "DIFM DETECTION FAILURE EA", "DIFM DETECTION FAILURE EB", "DIFM DETECTION FAILURE EC", "DIFM DETECTION FAILURE ED",
    "DIFM DETECTION FAILURE GA", "DIFM DETECTION FAILURE GB", "DIFM DETECTION FAILURE GC", "DIFM DETECTION FAILURE GD",
    "DIFM DETECTION FAILURE IA", "DIFM DETECTION FAILURE IB", "DIFM DETECTION FAILURE IC", "DIFM DETECTION FAILURE ID",
    "DIFM DETECTION FAILURE JA", "DIFM DETECTION FAILURE JB", "DIFM DETECTION FAILURE JC", "DIFM DETECTION FAILURE JD",
    "DIFM RF AMPLITUDE FAILURE EA", "DIFM RF AMPLITUDE FAILURE EB", "DIFM RF AMPLITUDE FAILURE EC", "DIFM RF AMPLITUDE FAILURE ED",
    "DIFM RF AMPLITUDE FAILURE GA", "DIFM RF AMPLITUDE FAILURE GB", "DIFM RF AMPLITUDE FAILURE GC", "DIFM RF AMPLITUDE FAILURE GD",
    "DIFM RF AMPLITUDE FAILURE IA", "DIFM RF AMPLITUDE FAILURE IB", "DIFM RF AMPLITUDE FAILURE IC", "DIFM RF AMPLITUDE FAILURE ID",
    "DIFM RF AMPLITUDE FAILURE JA", "DIFM RF AMPLITUDE FAILURE JB", "DIFM RF AMPLITUDE FAILURE JC", "DIFM RF AMPLITUDE FAILURE JD",
    "DIFM PULSE AMPLITUDE FAILURE EA", "DIFM PULSE AMPLITUDE FAILURE EB", "DIFM PULSE AMPLITUDE FAILURE EC", "DIFM PULSE AMPLITUDE FAILURE ED",
    "DIFM PULSE AMPLITUDE FAILURE GA", "DIFM PULSE AMPLITUDE FAILURE GB", "DIFM PULSE AMPLITUDE FAILURE GC", "DIFM PULSE AMPLITUDE FAILURE GD",
    "DIFM PULSE AMPLITUDE FAILURE IA", "DIFM PULSE AMPLITUDE FAILURE IB", "DIFM PULSE AMPLITUDE FAILURE IC", "DIFM PULSE AMPLITUDE FAILURE ID",
    "DIFM PULSE AMPLITUDE FAILURE JA", "DIFM PULSE AMPLITUDE FAILURE JB", "DIFM PULSE AMPLITUDE FAILURE JC", "DIFM PULSE AMPLITUDE FAILURE JD",

    "CWP CQSI RX TIMEOUT FAULT", "CWP CQSI CRC FAULT", "CWP FPGA FATAL ERROR", "CWP-DIFM LINK FAILURE",
    "CWP DETECTION FAILURE", "CWP FPGA XADC TEMPERATURE FAILURE", "CWP DETECTION FAILURE BAND E GAIN 1",
    "CWP DETECTION FAILURE BAND E GAIN 2", "CWP DETECTION FAILURE BAND G GAIN 1", "CWP DETECTION FAILURE BAND I GAIN 1",
    "CWP DETECTION FAILURE BAND I GAIN 2", "CWP DETECTION FAILURE BAND I GAIN 3", "CWP DETECTION FAILURE BAND J GAIN 1",
    "CWP DETECTION FAILURE BAND J GAIN 2", "CWP DETECTION FAILURE BAND J GAIN 3", "CWP AMPLITUDE FAILURE BAND E GAIN 1",
    "CWP AMPLITUDE FAILURE BAND E GAIN 2", "CWP AMPLITUDE FAILURE BAND G GAIN 1", "CWP AMPLITUDE FAILURE BAND I GAIN 1",
    "CWP AMPLITUDE FAILURE BAND I GAIN 2", "CWP AMPLITUDE FAILURE BAND I GAIN 3", "CWP AMPLITUDE FAILURE BAND J GAIN 1",
    "CWP AMPLITUDE FAILURE BAND J GAIN 2", "CWP AMPLITUDE FAILURE BAND J GAIN 3",

    "PLL 3.75 GHZ FAULT", "PLL 4 GHZ FAULT", "DCM TEMPERATURE FAULT", "PULSE PARAMETER GENERATION FAILURE",
    "CWP GAIN CONTROLLER FAILURE",

    "CW IMMUNITY FAULT ON A", "CW IMMUNITY FAULT ON B", "CW IMMUNITY FAULT ON C", "CW IMMUNITY FAULT ON D"
};

const char* ConstHigh::faultLru[] =
{
    "",

    "ABC", "ABA", "ABA", "ABA", "ABA", "ABC", "ABC", "ABC", "ABA", "ABA", "AAB",

    "ACA", "ACA", "ADA", "ADA", "ADA", "ACA", "ACA", "AFA", "AFA", "ADA", "ADA", "ABA", "AFA", "ADA", "AAA",

    "FAA", "FAA", "ABE", "ABE", "ABD", "ABD", "ABD", "ABD", "ABD", "ABD", "AAB",

    "ACA", "ACA", "ACA", "ACA", "ACA",  "AEA", "AAB",

    "ADA", "ADA", "ADA", "AFA", "AEA", "AEA", "AEA", "ADA", "ADA", "AEA", "ADA", "AAB", "AAB", "AAB",
    "BAA", "CAA", "CBA", "BBA", "BAA", "CAA", "CBA", "BBA", "BAA", "CAA", "CBA", "BBA", "BAA", "CAA", "CBA", "BBA",
    "BAA", "CAA", "CBA", "BBA", "BAA", "CAA", "CBA", "BBA", "BAA", "CAA", "CBA", "BBA", "BAA", "CAA", "CBA", "BBA",
    "BAA", "CAA", "CBA", "BBA", "BAA", "CAA", "CBA", "BBA", "BAA", "CAA", "CBA", "BBA", "BAA", "CAA", "CBA", "BBA",

    "AFA", "AFA", "AFA", "ADA", "AFA", "AAB", "AFA", "AFA", "AFA", "AFA", "AFA", "AFA", "AFA", "AFA", "AFA", "AFA",
    "AFA", "AFA", "AFA", "AFA", "AFA", "AFA", "AFA", "AFA",

    "ADA", "ADA", "AAB", "AEA", "AFA",

    "BBA", "BAA", "CBA", "CAA"
};

const int ConstHigh::faultFctn[] =
{
    0,

    1, 3, 1, 1, 1, 2, 3, 1, 3, 3, 1,

    1, 4, 1, 1, 1, 1, 4, 1, 1, 1, 1, 3, 1, 1, 1,

    2, 2, 3, 3, 3, 3, 2, 1, 4, 4, 2,

    4, 5, 2, 2, 3,  1, 1,

    3, 1, 3, 1, 1, 1, 1, 2, 2, 1, 3, 2, 2, 2,
    2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
    2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
    2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,

    1, 1, 2, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2,

    3, 3, 2, 2, 2,

    6, 6, 6, 6
};

std::string ConstHigh::cwHexDumpFileName = "/tmp/cf/cwHexDump.bin";
std::string ConstHigh::cpuDumpFileName = "/mnt/emmc/output/flight.csv";
std::string ConstHigh::recDataFromDspDumpFileNameCurrent = "";
std::string ConstHigh::recDataFromDspDumpFileNameDesktop = "output/dspReportDump.hex";
std::string ConstHigh::recDataFromDspDumpFileNameHardware = "/mnt/emmc/output/dspReportDump.hex";
std::string ConstHigh::dspReportFileNameCurrent = "";
std::string ConstHigh::dspReportFileNameDesktop ="output/dspReport.hex";
std::string ConstHigh::dspReportFileNameHardware ="/mnt/emmc/output/dspReport.hex";

//std::string ConstHigh::recDataFromDspDumpFileNameHw = "/media/emmc/output/dspDataDump.hex";
//std::string ConstHigh::dspReportFileNameHw ="/media/emmc/output/AOA_VAR_dspReport.hex";
//std::string ConstHigh::dspReportFileNameHw ="/media/emmc/output/Case4.9v2_dspReport.hex";
//std::string ConstHigh::dspReportFileNameHw ="/media/emmc/output/dspReport.hex";

//High startup flags! Set before production build
uint8_t ConstHigh::simLogIterationHigh = 1;
uint8_t ConstHigh::simOnActualHwFlag = 1;
uint8_t ConstHigh::readingFromLvds = 1;
uint8_t ConstHigh::simTimeSyntheticFlag = 1;
uint8_t ConstHigh::cwReportsOn = 1;
uint8_t ConstHigh::cwReportsLogOn = 1;


 int ConstHigh::CPF_ERROR_COUNT=11;
 int ConstHigh::PPF_ERROR_COUNT=15;
 int ConstHigh::CCP_ERROR_COUNT=11;
 int ConstHigh::TP_ERROR_COUNT=7;
 int ConstHigh::DIFM_ERROR_COUNT=62;
 int ConstHigh::DIFM_AUTO_ERROR_COUNT=14;
 int ConstHigh::CWP_ERROR_COUNT=24;
 int ConstHigh::CWP_AUTO_ERROR_COUNT=6;
 int ConstHigh::DCM_ERROR_COUNT=5;
 int ConstHigh::DCM_AUTO_ERROR_COUNT=3;
 int ConstHigh::DRU_ERROR_COUNT=4;
 int ConstHigh::TOTAL_ERROR_COUNT = (CPF_ERROR_COUNT + PPF_ERROR_COUNT + CCP_ERROR_COUNT + TP_ERROR_COUNT + DIFM_ERROR_COUNT + CWP_ERROR_COUNT + DCM_ERROR_COUNT + DRU_ERROR_COUNT);
//Track and Merge Constants
uint16_t ConstHigh::trackPriBuffAccumulateCircSize = 64;
uint16_t ConstHigh::trackPwBuffAccumulateCircSize = 16;
uint16_t ConstHigh::trackRfBuffAccumulateCircSize = 32;
uint16_t ConstHigh::trackIdsLowAccumulateCircSize = 64;
uint16_t ConstHigh::trackLimitRangeCalcIterDefault = 10;
uint16_t ConstHigh::trackLimitBearingCalcIterDefault = 10;
uint16_t ConstHigh::trackLimitRangeCalcIterAerial = 4;
uint16_t ConstHigh::trackLimitBearingCalcIterAerial = 4;
uint16_t ConstHigh::trackLimitMaxEnvironmentSize = 20;
uint16_t ConstHigh::trackLimitMaxEnvironmentIterLifetime = 25;
uint16_t ConstHigh::trackLimitMaxEmittersHigh = 256;
uint16_t ConstHigh::trackLimitMaxConsecutiveUpdateIter = 15;
uint16_t ConstHigh::trackLimitPartialConsecutiveUpdateIter = 9;
uint16_t ConstHigh::mergeScoreStringRequired = 20;
uint16_t ConstHigh::mergeScoreStringBand = 5;//previous was 10
uint16_t ConstHigh::mergeScoreStringBandEstimate = 5;
uint16_t ConstHigh::mergeDeltaStringRf = 50;
uint16_t ConstHigh::mergeScoreStringRf = 10;

uint16_t ConstHigh::mergeLimitStringBearingPrimaryLarge = 71;//previous was 43
uint16_t ConstHigh::mergeLimitStringBearingSecondaryLarge = 29;
uint16_t ConstHigh::mergeLimitStringBearingPrimary = 45;//previous was 43
uint16_t ConstHigh::mergeLimitStringBearingSecondary = 29;
uint16_t ConstHigh::mergeScoreStringBearingMax = 11;
uint16_t ConstHigh::mergeLimitEnvironmentRedeemBearing = 12;
uint16_t ConstHigh::mergeLimitFinalizeTrackBearing = 43;
uint16_t ConstHigh::mergeLimitMergeLowBandBearingPrimary = 78;
uint16_t ConstHigh::mergeLimitMergeLowBandBearingSecondary = 28;
uint16_t ConstHigh::mergeLimitMergePriStaggerBearingPrimary = 71;
uint16_t ConstHigh::mergeLimitMergePriStaggerBearingSecondary = 21;
uint16_t ConstHigh::mergeLimitMergeDefaultBearingPrimary = 42;//previous was 42
uint16_t ConstHigh::mergeLimitMergeDefaultBearingSecondary = 21;
uint16_t ConstHigh::mergeDeltaMergeBearingSmall = 14;
uint16_t ConstHigh::mergeDeltaMergeBearingMedium = 21;
uint16_t ConstHigh::mergeDeltaMergeBearingLarge = 42;

uint16_t ConstHigh::mergeDeltaMergePowerSmall = 160;
uint16_t ConstHigh::mergeDeltaMergePowerMedium = 240;
uint16_t ConstHigh::mergeDeltaMergePowerLarge = 480;
uint16_t ConstHigh::mergeDeltaMergePowerExcludePriCertain = 2560;
uint16_t ConstHigh::mergeDeltaMergePowerExcludePriUncertain = 480;
uint16_t ConstHigh::mergeDeltaStringPower = 240;
uint16_t ConstHigh::mergeScoreStringPower = 15;
uint16_t ConstHigh::mergeDeltaStringPwLarge = 50;//review
uint16_t ConstHigh::mergeDeltaStringPwMedium = 40;//review
uint16_t ConstHigh::mergeDeltaStringPwSmall = 10;//review
uint16_t ConstHigh::mergeDeltaMergePwLarge = 25;//review
uint16_t ConstHigh::mergeDeltaMergePwMedium = 25;//review
uint16_t ConstHigh::mergeDeltaMergePwSmall = 15;//review
uint16_t ConstHigh::mergeScoreStringPwLarge = 10;
uint16_t ConstHigh::mergeScoreStringPwMedium = 7;
uint16_t ConstHigh::mergeScoreStringPwSmall = 5;
uint16_t ConstHigh::mergeDeltaStringPx = 32;
uint16_t ConstHigh::mergeDeltaStringPy = 32;
uint16_t ConstHigh::mergeScoreStringPx = 5;
uint16_t ConstHigh::mergeScoreStringPy = 5;
uint64_t ConstHigh::mergeDeltaStringToaDiffSmall = 100000;
uint64_t ConstHigh::mergeDeltaStringToaDiff = 1000000;
uint16_t ConstHigh::mergeScoreStringToaDiff = 5;//previous was 15
uint16_t ConstHigh::mergeScoreStringPriWeak = 5;
uint16_t ConstHigh::mergeScoreStringPriType = 5;
uint16_t ConstHigh::mergeScoreStringLibraryPartial = 10;
uint16_t ConstHigh::mergeScoreStringLibraryComplete = 20;
uint16_t ConstHigh::mergeScoreStringPriUncertain = 0;//previous was 5
uint16_t ConstHigh::mergeDeltaStringPriFixedLarge = 7500;
uint16_t ConstHigh::mergeDeltaStringPriFixedMedium = 1000;
uint16_t ConstHigh::mergeDeltaStringPriFixedSmall = 125;
uint16_t ConstHigh::mergeScoreStringPriFixedLarge = 5;
uint16_t ConstHigh::mergeScoreStringPriFixedMedium = 10;
uint16_t ConstHigh::mergeScoreStringPriFixedSmall = 15;
uint16_t ConstHigh::mergeDeltaStringPriDwellSwitchLarge = 9000;
uint16_t ConstHigh::mergeDeltaStringPriDwellSwitchMedium = 2650;
uint16_t ConstHigh::mergeDeltaStringPriDwellSwitchSmall = 200;
uint16_t ConstHigh::mergeScoreStringPriDwellSwitchLarge = 5;
uint16_t ConstHigh::mergeScoreStringPriDwellSwitchMedium = 10;
uint16_t ConstHigh::mergeScoreStringPriDwellSwitchSmall = 15;
uint16_t ConstHigh::mergeDeltaStringPriStaggerPhase = 200;
uint16_t ConstHigh::mergeDeltaStringPriStaggerFrame = 200;
uint16_t ConstHigh::mergeDeltaStringPriStaggerFrameDwellSwitch = 200;
uint16_t ConstHigh::mergeScoreStringPriStaggerFrameDwellSwitch = 10;
uint16_t ConstHigh::mergeScoreStringPriStaggerPhase = 10;
uint16_t ConstHigh::mergeScoreStringPriStaggerFrame = 10;
uint16_t ConstHigh::mergeDeltaStringPowerExclusion = 1280;
uint16_t ConstHigh::mergeDeltaStringPowerPriUncertainExclusion = 240;
uint16_t ConstHigh::mergeDeltaStringRfExcludeSmall = 100;
uint16_t ConstHigh::mergeDeltaStringRfExcludeMedium = 550;
uint16_t ConstHigh::mergeDeltaStringRfExcludeLarge = 850;
uint16_t ConstHigh::mergeScoreStringRequiredTotal = 20;


//Bearing Calculation
int16_t ConstHigh::bearingDisplayFilterLimit = 19;

//Range Calculation
float ConstHigh::rangePowerNormalizationTable[46] =
{
	0.00,0.00,0.00,0.00,
	0.00,0.00,0.07,0.14,
	0.21,0.28,0.35,0.42,
	0.49,0.56,0.63,0.70,
	0.83,0.96,1.09,1.22,
	1.35,1.48,1.61,1.74,
	1.87,2.00,2.19,2.38,
	2.57,2.76,2.95,3.14,
	3.33,3.52,3.71,3.90,
	4.11,4.32,4.53,4.74,
	4.95,5.16,5.37,5.58,
	5.79,6.00
};
int ConstHigh::testVar[2][4] = {
	    {10, 11, 12, 13},
	    {14, 15, 16, 17}
	};

F64 ConstHigh::cwPowerCorrectionLookup[46]={0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.07, 0.14, 0.21,
0.28, 0.35, 0.42, 0.49, 0.56, 0.63, 0.70, 0.83, 0.96, 1.09, 1.22, 1.35, 1.48,
1.61, 1.74, 1.87, 2.00, 2.19, 2.38, 2.57, 2.76, 2.95, 3.14, 3.33, 3.52, 3.71,
3.90, 4.11, 4.32, 4.53, 4.74, 4.95, 5.16, 5.37, 5.58, 5.79, 6.00};

//Library constants
std::string ConstHigh::libraryPathHardware = "/mnt/dtm_tl/";
std::string ConstHigh::libraryKeyPathHardware = "/mnt/dtm_tl/";
std::string ConstHigh::libraryPathDesktop = "input/";
//std::string ConstHigh::libraryPathDesktop = "/home/hassan/Desktop/1-5 testing/";
std::string ConstHigh::libraryKeyPathDesktop = "input/";
std::string ConstHigh::libraryPathCurrent = "";
std::string ConstHigh::libraryKeyPathCurrent = "";
uint32_t ConstHigh::libraryScoreDefaultThreshold = 50;
uint32_t ConstHigh::libraryCountPriDefaultThreshold = 3;

uint8_t ConstHigh::libraryPriTypeScore= 10;
uint8_t ConstHigh::libraryPwTypeScore= 10;
uint8_t ConstHigh::libraryRfTypeScore= 10;
uint8_t ConstHigh::libraryPriScore= 20;
uint8_t ConstHigh::libraryPwScore= 15;
uint8_t ConstHigh::libraryRfScore= 35;
//Flight record constants
std::string ConstHigh::flightRecordPathHardware = "/tmp/cf/";
std::string ConstHigh::flightRecordPathDesktop  = "output/";
std::string ConstHigh::flightRecordPathCurrent  = "";

bool ConstHigh::flightRecordEncrypted=true;
bool ConstHigh::lowLogEncrypted=true;
bool ConstHigh::flightRecordAppendMode=true;
//Library constants

//Time Constants
uint32_t ConstHigh::timeIterCountEquivalent200ms = 1;
uint32_t ConstHigh::timeIterEquivalent200ms =
		ConstHigh::timeIterCountEquivalent200ms;
uint32_t ConstHigh::timeIterEquivalent400ms =
		ConstHigh::timeIterCountEquivalent200ms * 2;
uint32_t ConstHigh::timeIterEquivalent600ms =
		ConstHigh::timeIterCountEquivalent200ms * 3;
uint32_t ConstHigh::timeIterEquivalent800ms =
		ConstHigh::timeIterCountEquivalent200ms * 4;
uint32_t ConstHigh::timeIterEquivalent1000ms =
		ConstHigh::timeIterCountEquivalent200ms * 5;
uint32_t ConstHigh::timeIterEquivalent2000ms =
		ConstHigh::timeIterCountEquivalent200ms * 10;
uint32_t ConstHigh::timeIterEquivalent3000ms =
		ConstHigh::timeIterCountEquivalent200ms * 15;
uint32_t ConstHigh::timeIterEquivalent4000ms =
		ConstHigh::timeIterCountEquivalent200ms * 20;
uint32_t ConstHigh::timeIterEquivalent5000ms =
		ConstHigh::timeIterCountEquivalent200ms * 25;

uint8_t ConstHigh::SW_VER_MIN_1 = 49;
uint8_t ConstHigh::SW_VER_MIN_2 = 51;
uint8_t ConstHigh::SW_VER_MAJ_1 = 48;
uint8_t ConstHigh::SW_VER_MAJ_2 = 50;
uint16_t ConstHigh::SW_VER_YEAR = 0x2021;
uint8_t ConstHigh::SW_VER_MONTH = 0x04;
uint8_t ConstHigh::SW_VER_DATE = 0x28;

//CFD Constants
int ConstHigh::cfdUp = 0;
int ConstHigh::cfdInit = 0;
int ConstHigh::cfdPause = 0;
int ConstHigh::ecmMode = 3;
int ConstHigh::presetChaff = 1;
int ConstHigh::presetFlare = 1;
int ConstHigh::programNum = 15;
int ConstHigh::cfdType = 0;
int ConstHigh::cfdFailCount = 0;
vector<int> ConstHigh::cfdThreatIds;

//TRENZ Constants
UWord8 ConstHigh::sopAsyncAudioFlag = 0;
UWord8 ConstHigh::sopMissileLaunch = 0;
UWord8 ConstHigh::sopBeingTracked = 0;
UWord8 ConstHigh::sopThreatFound = 0;
UWord8 ConstHigh::pflCheck = 0;
UWord8 ConstHigh::spiAudioRequest;

//ST Constants
UWord8 ConstHigh::StopFst;
UWord8 ConstHigh::sendShutdown;
UWord8 ConstHigh::FstInProgress;
UWord8 ConstHigh::FstRetryCount = 0;
UWord8 ConstHigh::mStRetryCount = 0;
UWord32 ConstHigh::FstMaxDuration=10000;
UWord32 ConstHigh::FstCurrTryDuration=0;
#else
//Global
enum class LibraryPriTypes
{
	pri_uncomputed = 0,
	pri_fixed = 1,
	pri_switch,
	pri_stagger,
	pri_slide,
	pri_jitter,
	pri_complex
};

//static const int WIDTH = (8 * sizeof(crc));
//static const int TOPBIT = (1 << (WIDTH - 1));
static const int MAX_RF_LIBRARY = 32;
static const int MAX_PRI_LIBRARY = 64;
static const int MAX_PW_LIBRARY = 32;
static const int MAX_SEQ_LIBRARY = 32;
static const int MAX_LIB_ENTRIES = 5000;

#endif
//End

#endif /* CONSTHIGH_H_ */
