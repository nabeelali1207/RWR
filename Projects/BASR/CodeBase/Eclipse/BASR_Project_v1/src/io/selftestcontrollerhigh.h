/*
 * selftestcontroller.h
 *
 *  Created on: Jan 29, 2021
 *      Author: saad
 */

#ifndef SELFTESTCONTROLLER_H_
#define SELFTESTCONTROLLER_H_

#include "../delegate/trenzspidelegate.h"
#include "../io/memorymap.h"
#include "../model/model.h"
#include <time.h>

class SelfTestControllerHigh
{
public:
	static const unsigned int SELFTEST_CONTROL = 0x00000000;
	static const unsigned int SELFTEST_STATUS = 0x00000004;
	static const unsigned int SELFTEST_TX_DATA = 0x00000008;
	static const unsigned int SELFTEST_RX_DATA = 0x0000000C;
	static const unsigned int SELFTEST_FIFO_EMPTY = 0x00000001;
	static const unsigned int TP_SELFTEST_BASE_REG = 0x43C10000;
	static const unsigned int TP_SELFTEST_START_VALUE = 0x1;
	static const unsigned int TP_SELFTEST_WRITE_REG = 0x00000008;

	/*
	 * Function Name:   initializeTpSelfTest
	 * Purpose:			This function is used to initialize TP Self Test Register
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void initializeTpSelfTest();

	/*
	 * Function Name:   recvWordTpSelfTest
	 * Purpose:			This function receives the data from address offset on TP Self Test Register
	 * Inputs:          address
	 * Outputs:         word
	 * Scope:           Can be called by any function
	 */
	UWord32 recvWordTpSelfTest(UWord32 address);

	/*
	 * Function Name:   initializeVdmSelfTest
	 * Purpose:			This function is used to initialize VDM Self Test Register
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void initializePpfSelfTest();

	/*
	 * Function Name:   sendWordPpfSelfTest
	 * Purpose:			This function sends the word to address offset on VDM Self Test Register
	 * Inputs:          address
	 * 					word
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void sendWordPpfSelfTest(UWord32 address, UWord32 word);

	/*
	 * Function Name:   recvWordPpfSelfTest
	 * Purpose:			This function receives the data from address offset on VDM Self Test Register
	 * Inputs:          address
	 * Outputs:         word
	 * Scope:           Can be called by any function
	 */
	UWord32 recvWordPpfSelfTest(UWord32 address);

	UWord8 ppfGetSelfTestStatus(UWord8 mode);

	UWord16 ppfGetSelfTestResults(UWord32* stPackets);

	UWord32 tpGetSelfTestStatus();

	UWord16 tpGetSelfTestResults(UWord32* stPackets);

	UWord16 rfbGetSelfTestResults(UWord32* stPackets);

	 /* Function Name:   sendWordTpSelfTest
	 * Purpose:			This function sends the word to address offset on TP Self Test Register
	 * Inputs:          address
	 * 					word
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void sendWordTpSelfTest(UWord32 address, UWord32 word);

};

#endif /* SELFTESTCONTROLLER_H_ */
