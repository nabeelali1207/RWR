/*
 * selftestccphigh.h
 *
 *  Created on: Feb 9, 2021
 *      Author: saad
 */

#ifndef SELFTESTCCPHIGH_H_
#define SELFTESTCCPHIGH_H_

#include "../common/common.h"
#include "cfcard.h"
#include "xadctemperaturehigh.h"
#include "../vo/trenzspidatastructures.h"
#include <sys/statvfs.h>

//class TrenzSpiLink;
class SelfTestCcpHigh {
private:
	SpiFilenameReply filenameReply;
	UWord8 cfCardPercentage;

	/*
	 * Function Name:   checkCfCard
	 * Purpose:			This function fills and returns packet with result of checking
	 * 					if CF Card is connected or not
	 * Inputs:          Object of CF Card Class
	 * Outputs:         Result Packet
	 * Scope:           Can be called by any function
	 */
	Word32 checkCfCard(CfCard &cfCard);

	/*
	 * Function Name:   checkEmmc
	 * Purpose:			This function fills and returns packet with result of checking
	 * 					emmc exists on board
	 * Inputs:          none
	 * Outputs:         Result Packet
	 * Scope:           Can be called by any function
	 */
	Word32 checkEmmc();

	/*
	 * Function Name:   findFreeSpaceEmmc
	 * Purpose:			This function opens up emmc directory through statvfs
	 * 					and finds if there is some empty space in emmc
	 * Inputs:          none
	 * Outputs:         Result Packet
	 * Scope:           Can be called by any function
	 */
	Word32 findFreeSpaceEmmc();

	/*
	 * Function Name:   findFreeSpaceCfCard
	 * Purpose:			This function opens up cf card directory through statvfs
	 * 					and finds if there is some empty space in cf card
	 * Inputs:          none
	 * Outputs:         Result Packet
	 * Scope:           Can be called by any function
	 */
	Word32 findFreeSpaceCfCard(CfCard & cfCard);

	/*
	 * Function Name:   readWriteTestCfCard
	 * Purpose:			This function tests read and write from CF Card
	 * 					It opens a file, writes to it, reads it back and
	 * 					checks sanctity of the data
	 * Inputs:          none
	 * Outputs:         Result Packet
	 * Scope:           Can be called by any function
	 */
	Word32 readWriteTestCfCard(CfCard & cfCard);

	/*
	 * Function Name:   readWriteTestEmmc
	 * Purpose:			This function tests read and write from EMMC
	 * 					It opens a file, writes to it, reads it back and
	 * 					checks sanctity of the data
	 * Inputs:          none
	 * Outputs:         Result Packet
	 * Scope:           Can be called by any function
	 */
	Word32 readWriteTestEmmc();

	/*
	 * Function Name:   readFilename
	 * Purpose:			This function is used to read filename of Selected TL that was selected in
	 * 					previous iteration
	 * Inputs:          Filename buffer in which filename will be copied
	 * Outputs:         Status of filename existance
	 * Scope:           Can be called by any function
	 */
	Word32 readFilename(Word8 filename[]);

	/*
	 * Function Name:   showLibraryInEmmc
	 * Purpose:			This function iterates over /tmp/emmc directory, finds all TLs,
	 * 					copies their names and returns number of TL present in emmc
	 * Inputs:          none
	 * Outputs:         Number of TL present in emmc
	 * Scope:           Can be called by any function
	 */
	Word32 showLibraryInEmmc();

	/*
	 * Function Name:   findLibraryInEmmc
	 * Purpose:			This function returns the TL name with respect to the number in emmc
	 * Inputs:          Number of TL in emmc
	 * Outputs:         Name of TL responding to number
	 * Scope:           Can be called by any function
	 */
	Word8* findLibraryInEmmc(Word32 number);

	/*
	 * Function Name:   checkThreatLibraryExists
	 * Purpose:			This function is main logic of checking either selected TL exists in emmc
	 * 					It extacts selected TL name from file and finds that TL in emmc
	 * Inputs:          none
	 * Outputs:         Result Packet
	 * Scope:           Can be called by any function
	 */
	UWord8 checkThreatLibraryExists();

	/*
	 * Function Name:   checkThreatLibrarySelectionExists
	 * Purpose:			This function checks if a TL was selected previously and its record exists
	 * Inputs:          none
	 * Outputs:         Status in form of 0 or 1
	 * Scope:           Can be called by any function
	 */
	UWord8 checkThreatLibrarySelectionExists();

	/*
	 * Function Name:   checkThreatLibraryFaults
	 * Purpose:			This is the function that collects all the results regarding TL and Key in EMMC
	 * Inputs:          Result buffer to be filled and TL failure flag
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void checkThreatLibraryFaults(UWord32 *input, bool tlFailure);

	/*
	 * Function Name:   returnNumberOfKeysInEmmc
	 * Purpose:			This function returns number of keys present in EMMC i.e. /tmp/emmc directory
	 * Inputs:          none
	 * Outputs:         Number of Keys present in emmc
	 * Scope:           Can be called by any function
	 */
	Word32 returnNumberOfKeysInEmmc();

	/*
	 * Function Name:   checkKeyExists
	 * Purpose:			This function returns if any Key exists in emmc
	 * Inputs:          none
	 * Outputs:         Flag if Key is present in emmc
	 * Scope:           Can be called by any function
	 */
	Word32 checkKeyExists();
public:

	/*
	 * Function Name:   testAllModulesCCP
	 * Purpose:			This is the main function of Modular Self Test done on CCP
	 * 					It checks CF Card, EMMC, free space in both, read write of both,
	 * 					TL and Key presence in EMMC and eventually temperature of SoC
	 * Inputs:          Buffer to be filled with result
	 * 					Some Flags
	 * 					Objects of XADC Temperature Class and CF Card Class
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void testAllModulesCCP(UWord32 *input, bool tlFailure,
			XadcTemperatureHigh &xadcTemperatureHigh, CfCard &cfCard);
};

#endif /* SELFTESTCCPHIGH_H_ */
