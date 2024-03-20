/*
 * selftesthighdelegate.h
 *
 *  Created on: Jun 25, 2021
 *      Author: zumair
 */

#ifndef DELEGATE_SELFTESTHIGHDELEGATE_H_
#define DELEGATE_SELFTESTHIGHDELEGATE_H_

#include <time.h>
#include "../common/common.h"
#include "../utility/consthigh.h"
#include "../model/model.h"
#include "../io/selftestcontrollerhigh.h"
#include "../delegate/selftestmflhighdelegate.h"
#include "../delegate/trenzspidelegate.h"

typedef struct TpSelfTestResults
{
	UWord16 tpDetectionResults;
	UWord16 tpPulseAmpResults;
	UWord16 tpRfAmpResults;
	UWord8 pulseParamResult;
} TpSelfTestResults;

typedef struct StatusPacket
{
	UWord8 packetId;
	UWord8 faultBitStatus;
	UWord8 periodicBitStatus;
	UWord8 functionalBitStatus;
	UWord8 modularBitStatus;
} StatusPacket;

class SelfTestHighDelegate
{
	static const unsigned int SELFTEST_CONTROL = 0x00000000;
	static const unsigned int SELFTEST_STATUS = 0x00000004;
	static const unsigned int SELFTEST_TX_DATA = 0x00000008;
	static const unsigned int SELFTEST_RX_DATA = 0x0000000C;
	static const unsigned int SELFTEST_FIFO_EMPTY = 0x00000001;
	static const unsigned int TP_SELFTEST_WRITE_REG = 0x00000008;

private:
	StatusPacket testStatus;
	UWord16 readCount;
	uint64_t startTime, endTime, selfTestTimeout, fstTimeout,lastSpecialPeriodicBitTime;
public:
	UWord8 selfTestActiveFlag;
	UWord8 selfTestType;
	/*selfTestType is
		0 cfd status update
		1 periodic BIT (only modular self test)
		2 periodic BIT (modular + functional self test)
		3 IBIT or PoweronBit
		4 MBIT */

	UWord32 stData[1000];
	UWord8 trenzStResults;
	TpSelfTestResults tpSelfTestResults;
	CwSelfTestResults cwSelfTestResults;

	/*
	 * Function Name:   selfTestInitialize
	 * Purpose:			This function is main logic of Self Test initializations
	 * 					First, it initializes TP self test registers and then
	 * 					initializes VDM self test registers
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void selfTestInitialize();

	/*
	 * Function Name:   selfTestPpf
	 * Purpose:			This function is main logic of PPF Self Test
	 * 					First, it initializes VDM board self test and then
	 * 					reads the self test results
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void selfTestPpf();

	/*
	 * Function Name:   selfTestCcp
	 * Purpose:			This function is main logic of CCP Self Test
	 * 					First, it initializes CCP self test and then
	 * 					reads the self test results
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void selfTestCcp();

	/*
	 * Function Name:   selfTestTrenz
	 * Purpose:			This function is main logic of Trenz Self Test
	 * 					First, it initializes Trenz self test and then
	 * 					reads the self test results
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void selfTestTrenz();

	/*
	 * Function Name:   selfTestTp
	 * Purpose:			This function is main logic of TP board Self Test
	 * 					First, it initializes TP board self test and then
	 * 					reads the self test results
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void selfTestTp();

	/*
	 * Function Name:   selfTestRfBoard
	 * Purpose:			This function is main logic of RF board Self Test
	 * 					First, it initializes RF board self test and then
	 * 					reads the self test results
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void selfTestRfBoard();

	void selfTestUpdateMfl(UWord8 count, UWord8 module);

	/*
	 * Function Name:   selfTestBuiltIn
	 * Purpose:			This is the main logic of Self Test. All the Self Test steps are started i.e.
	 * 					firstly, all modular tests are completed and eventually functional self test
	 * 					gets started
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void selfTestBuiltIn();

	/*
	 * Function Name:   selfTestModular
	 * Purpose:			This is the main logic of Modular Self Test of each LRU of BASR
	 * 					This function performs Modular self tests and collects their results one by one
	 * 					It starts with VDM, then TP, then CCP, then Trenz and eventually RF Board
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void selfTestModular();

	/*
	 * Function Name:   selfTestFunctional
	 * Purpose:			This is the main logic of Functional Self Test of BASR
	 * 					This function sends the signal to RF board to initiate the FST
	 * 					It then starts a timeout variable and sets the FstInProgress flag high
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void selfTestFunctional();

	/*
	 * Function Name:   selfTestHandleFstMfl
	 * Purpose:			This function gathers all the Functional Self Test results after FST is complete
	 * 					All the results are extracted and eventually stored in MFL files
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void selfTestHandleFstMfl();

	/*
	 * Function Name:   selfTestPeriodic
	 * Purpose:			This is the main logic of Periodic Self Test
	 * 					This function performs Periodic self tests and collects their results one by one
	 * 					It starts with Trenz, then VDM, then TP, then CCP and eventually RF Board
	 * Inputs:          Synthetic threats flag
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void selfTestPeriodic(uint32_t syntheticReportFlag);

	/*
	 * Function Name:   selfTestSynthetic
	 * Purpose:			This is the main logic of Synthetic failures Self Test
	 * 					This function performs self tests for synthetic failures
	 * Inputs:          Failures variable
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void selfTestSynthetic(uint32_t failures);

	/*
	 * Function Name:   fillPacketInfo
	 * Purpose:			This function extracts results out of readData and returns the type of
	 * 					results it got
	 * Inputs:          Result of Self Test data by name of readData
	 * Outputs:         Type of Self Test result
	 * Scope:           Can be called by any function
	 */
	UWord8 fillPacketInfo(UWord32 readData);

};

#endif /* DELEGATE_SELFTESTHIGHDELEGATE_H_ */
