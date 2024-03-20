/*
 * cwpcqsilink.h
 *
 *  Created on: Jan 13, 2021
 *      Author: Saad
 */

#ifndef CWPCQSILINK_H_
#define CWPCQSILINK_H_

#include "memorymap.h"
#include "../utility/consthigh.h"
#include "../vo/angletableoneband.h"
#include "../vo/cwppowerlookup.h"
#include "../io/fileinputoutputdelimited.h"
#include "../delegate/analysisangledelegate.h"
class CwReports {
public:
	UWord8 description;
	UWord8 invalid;
	UWord8 bandCode;
	UWord8 selfTestStatus;
	UWord8 gainStageA;
	UWord8 gainStageB;
	UWord8 gainStageC;
	UWord8 gainStageD;
	UWord8 gainFst;
	UWord8 quadrant;
	Word16 px;
	Word16 py;
	UWord16 angleOfArrival;
	UWord16 frequency;
	Word16 amplitudeA;
	Word16 amplitudeB;
	Word16 amplitudeC;
	Word16 amplitudeD;
	UWord32 delimeter;
	UWord8 trackUpdate;  //0 when new data is here, 1 when updated or processed
	UWord8 LO;
	UWord8 saturationStatusAntenna1;
	UWord8 saturationStatusAntenna2;
	UWord8 saturationStatusAntenna3;
	UWord8 saturationStatusAntenna4;
	Word16 amplitudeAInDbm;
	Word16 amplitudeBInDbm;
	Word16 amplitudeCInDbm;
	Word16 amplitudeDInDbm;
};

class CwReportRecord {
public:
	UWord8 numberOfCWPs;
	CwReports cwReportArray[20];
};

class CwSelfTestResults {
public:
	UWord16 cwDetectionResults;
	UWord16 cwAmpResults;
	UWord8 gainCtlrResult;
};

class TrenzSpiLink;
class ThreatProcessorBandSwitchingTimeClass;
class SelfTestHighDelegate;
class CwpCqsiLink {
	static const unsigned int CWP_CONTROL_OFFSET = 0x00000000;
	static const unsigned int CWP_STATUS_OFFSET = 0x00000004;
	static const unsigned int CWP_TX_OFFSET = 0x00000008;
	static const unsigned int CWP_RX_OFFSET = 0x0000000C;
	static const unsigned int CWP_DELIMETER_MASK = 0xFFFFFFFF;
	static const unsigned int CWP_DESCRIPTION_MASK = 0x00000007;
	static const unsigned int CWP_INVALID_MASK = 0x00000008;
	static const unsigned int CWP_BAND_CODE_MASK = 0x00000070;
	static const unsigned int CWP_SELF_TEST_MASK = 0x00000080;
	static const unsigned int CWP_FREQUENCY_MASK = 0xFFFF0000;
	static const unsigned int CWP_AMP_A_MASK = 0x00001FFF;
	static const unsigned int CWP_AMP_B_MASK = 0x1FFF0000;
	static const unsigned int CWP_AMP_C_MASK = 0x00001FFF;
	static const unsigned int CWP_AMP_D_MASK = 0x1FFF0000;
	static const unsigned int CWP_LO = 0x00000100;
	static const unsigned int CWP_SAT_STATUS_ANT1 = 0x00001000;
	static const unsigned int CWP_SAT_STATUS_ANT2 = 0x00002000;
	static const unsigned int CWP_SAT_STATUS_ANT3 = 0x00004000;
	static const unsigned int CWP_SAT_STATUS_ANT4 = 0x00008000;
	static const unsigned int CWP_GAIN_A = 0x0000E000;
	static const unsigned int CWP_GAIN_B = 0xE0000000;
	static const unsigned int CWP_GAIN_C = 0x0000E000;
	static const unsigned int CWP_GAIN_D = 0xE0000000;
	static const unsigned int CWP_FST_GAIN_MASK = 0x0000C000;

private:
	FileOutputDelimitedClass cwHexDumpFile;
	UWord32 cwReportRecieveBuffer[128];
public:

	/*
	 * Function Name:   initialize
	 * Purpose:			This function is used to initialize CWP CQSI link
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void initialize();

	/*
	 * Function Name:   clean
	 * Purpose:			This function cleans up internal data structures of the
	 * 					class i.e. cwReportRecord and cwRecieveBuffer
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void clean();

	/*
	 * Function Name:   sendWord
	 * Purpose:			This function sends the word to address offset on CWP CQSI link
	 * Inputs:          address
	 * 					word
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void sendWord(UWord32 address, UWord32 word);

	/*
	 * Function Name:   recvWord
	 * Purpose:			This function receives the data from address offset on CWP CQSI link
	 * Inputs:          address
	 * Outputs:         word
	 * Scope:           Can be called by any function
	 */
	UWord32 recvWord(UWord32 address);

	/*
	 * Function Name:   readDataFromCwpCqsi
	 * Purpose:			This function is the main loop of CW report reading and extraction
	 * 					It starts reading data till data stops coming or 127 packets of 4 bytes each
	 * 					have received.Afterwards, it calls the extractor and extracts data from those bytes
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void readDataFromCwpCqsi();

	/*
	 * Function Name:   extractCwReportsLogic
	 * Purpose:			This function simply calls extraction logic on buffer of data received from CWP
	 * 					Extracted data is stored on cwReportRecord structure
	 * Inputs:          Number of CW reports received
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void extractCwReportsLogic(UWord32 count);

	/*
	 * Function Name:   extractCWreports
	 * Purpose:			This function fills cwReport structure after extracting information from data buffer
	 * 					After extracting all essential information from buffer, it calculates AoA of emitter
	 * Inputs:          Pointer of CW Report structure to be filled
	 * 					Iterator to identify which offset of buffer is to be extracted
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	Word8 extractCWreports(CwReports *cwReports, UWord8 iterator);

	/*
	 * Function Name:   convertAllCwpAmplitudesFromRfADCToTpADC
	 * Purpose:			This function converts all the RF board/CWP ADC counts to dBms.
	 * 					Afterwards this converts dBms to TP/VDM ADC counts
	 * Inputs:          Pointer of CW Report structure
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void convertAllCwpAmplitudesFromRfADCToTpADC(CwReports *cw);

	/*
	 * Function Name:   rfAdcToDbm
	 * Purpose:			This function converts amplitude in CWP ADC counts to dBms using fixed formula
	 * 					and lookup tables for power correction given by team CWP
	 * Inputs:          Amplitude in ADC counts
	 * 					Gain of emitter
	 * 					RF of emitter
	 * 					Band of emitter
	 * Outputs:         Power in dBm
	 * Scope:           Can be called by any function
	 */
	Word16 rfAdcToDbm(Word16 amplitude, UWord8 gain, UWord16 rf, UWord8 band);

	/*
	 * Function Name:   dbmToTpAdc
	 * Purpose:			This function converts dBm to TP ADC counts
	 * Inputs:          Power in dBm
	 * Outputs:         Power in TP ADC counts
	 * Scope:           Can be called by any function
	 */
	Word16 dbmToTpAdc(Word16 correctedPowerInDbm);
	Word16 tpAdcToDbm(Word16 correctedPowerInDbm);

	/*
	 * Function Name:   cwPowerLookup
	 * Purpose:			This function returns corrected power in dBm based on correction lookup tables
	 * 					provided by team CWP.
	 * Inputs:          Amplitude in dBms
	 * 					Gain of emitter
	 * 					RF of emitter
	 * 					Band of emitter
	 * Outputs:         Corrected Power in dBm
	 * Scope:           Can be called by any function
	 */
	Word16 cwPowerLookup(float power, UWord8 gain, UWord16 rf, UWord8 band);

	/*
	 * Function Name:   findAngleOfArrival
	 * Purpose:			This function first finds quadrant of emitter and then finds AoA of emitter
	 * Inputs:          Pointer of CW Report structure
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void findAngleOfArrival(CwReports *cw);

	/*
	 * Function Name:   findQuadrant
	 * Purpose:			This function first finds max powers across x and y and then finds the quadrant of emitter
	 * Inputs:          Pointer of CW Report structure
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void findQuadrant(CwReports *cw);

	/*
	 * Function Name:   findPxPy
	 * Purpose:			This function finds max powers across x and y axis
	 * Inputs:          Pointer of CW Report structure
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void findPxPy(CwReports *cw);

	/*
	 * Function Name:   angleOfArrivalCalculation
	 * Purpose:			This function extracts angle of arrival from lookup table based on Px, Py and quadrant of emitter
	 * Inputs:          Px (dominant power across x)
	 * 					Py (dominant power across y)
	 * 					Quadrant of emitter
	 * Outputs:         Angle of Arrival
	 * Scope:           Can be called by any function
	 */
	UWord16 angleOfArrivalCalculation(UWord16 Px, UWord16 Py,
			UWord8 QuadrantCode);

	/*
	 * Function Name:   findCwSelfTestEmitters
	 * Purpose:			This function simply finds Self Test emitters amongst report CW reports and
	 * 					fills self test results accordingly
	 * Inputs:          Pointer of CW Report structure
	 * 					CW Self Test Results
	 * 					Functional Self Test execution flag
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void findCwSelfTestEmitters(CwReports *cwReports,
			CwSelfTestResults *cwSelfTestResults, UWord8 FstInProgress);

	CwReportRecord cwReportRecord;
};

#endif /* CWPCQSILINK_H_ */
