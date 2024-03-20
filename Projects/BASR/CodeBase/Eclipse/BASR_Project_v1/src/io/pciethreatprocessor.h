/*
 * pciethreatprocessor.h
 *
 *  Created on: Feb 10, 2021
 *      Author: saad
 */

#ifndef PCIETHREATPROCESSOR_H_
#define PCIETHREATPROCESSOR_H_

#include "../common/common.h"
#include "memorymap.h"
#include "../utility/consthigh.h"
#include "cfcard.h"
#include "../utility/utility.h"
class PcieThreatProcessor {
private:
	//UWord32 *buff;
	constexpr static const char *sharedMemory = "/dev/uio0";
	uint64_t ThreatProcessorReportTimeout, currentTime;

	/*
	 * Function Name:   get_memory_size
	 * Purpose:			This function reads the size of memory buffer used for PCIe using userspace IO
	 * 					In actual this memory size is fixed to 32 MB but if UIO fails or something goes
	 * 					south, this get_memory_size function will return 0 hence avoiding the page faults
	 * 					or segmentation faults
	 * Inputs:          path to UIO
	 * Outputs:         Size of UIO memory
	 * Scope:           Can be called by any function
	 */
	UWord32 get_memory_size(const char *sysfs_path_file);

	/*
	 * Function Name:   initializeSharedMemoryBuffer
	 * Purpose:			This function maps the UIO Shared memory buffer used for PCIe for access
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void initializeSharedMemoryBuffer();

	/*
	 * Function Name:   sendWordVdmPcieReg
	 * Purpose:			This function sends the word to address offset on VDM side buffer of PCIe
	 * Inputs:          address
	 * 					word
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void sendWordVdmPcieReg(UWord32 address, UWord32 word);

	/*
	 * Function Name:   recvWordVdmPcieReg
	 * Purpose:			This function receives the data from address offset on VDM side buffer of PCIe
	 * Inputs:          address
	 * Outputs:         word
	 * Scope:           Can be called by any function
	 */
	UWord32 recvWordVdmPcieReg(UWord32 address);

	/*
	 * Function Name:   initializeVdmPcieReg
	 * Purpose:			This function maps the VDM side buffer of PCIe for access
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void initializeVdmPcieReg();
public:
	UWord8 tpUp;
	UWord8 filecount;

	/*
	 * Function Name:   sendWordToSharedMemoryBuffer
	 * Purpose:			This function sends the word to address offset on shared memory buffer of PCIe
	 * Inputs:          address
	 * 					word
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void sendWordToSharedMemoryBuffer(UWord32 address, UWord32 word);

	/*
	 * Function Name:   recvWordToSharedMemoryBuffer
	 * Purpose:			This function receives the data from address offset on shared memory buffer of PCIe
	 * Inputs:          address
	 * Outputs:         word
	 * Scope:           Can be called by any function
	 */
	UWord32 recvWordToSharedMemoryBuffer(UWord32 address);

	/*
	 * Function Name:   initPcie
	 * Purpose:			This function is main logic of PCIe initialization between TP and CCP
	 * 					Here, we first initialize UIO shared memory buffer and then VDM side buffer
	 * 					to communicate. Afterwards we send TP a message that we are up and wait for
	 * 					it to start, if it doesn't TP UP is status is turned false
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void initPcie();

	/*
	 * Function Name:   getThreatProcessorReport
	 * Purpose:			This report checks if TP is up and it has sent a new report
	 * 					If so, report is extracted from PCIe shared memory buffer
	 * Inputs:          Buffer to be filled with TP report
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	bool getThreatProcessorReport(uint8_t *buff);

	/*
	 * Function Name:   checkZeroizeSignal
	 * Purpose:			VDM sends a zeroize signal in its buffer, if it is received, all the Keys
	 * 					present in CF Card and emmc are overwritten by Zeroes (0)
	 * Inputs:          CF Card identifier
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void checkZeroizeSignal(CfCard &cfCard);

	/*
	 * Function Name:   checkTpUp
	 * Purpose:			It checks if TP is up during normal operations
	 * Inputs:          Send Band Switching data to TP flag
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void checkTpUp(UWord8 &sendBandSwitchDataToTp);
};

#endif /* PCIETHREATPROCESSOR_H_ */
