/*
 * difmcsilink.h
 *
 *  Created on: Jan 15, 2021
 *      Author: saad
 */

#ifndef DIFMCSILINK_H_
#define DIFMCSILINK_H_

#include "memorymap.h"
#include "../vo/libraryclass.h"
#include "../utility/consthigh.h"
#include "../utility/utility.h"

typedef struct MdfStructure
{
    UWord8  opcodeRes;
    UWord8  antennaSequence;
    UWord8  reserved1;
    UWord8  s1Reps;
    UWord32 reserved2;
    UWord16 numOfS1;
    UWord16 numOfS2;
    UWord32 reserved3;
    BandFilter bandFilter[10];
    UWord32 eBandThreshold;
    UWord32 gBandThreshold;
    UWord32 iBandThreshold;
    UWord32 jBandThreshold;
    UWord32 cdBandThreshold;
    UWord32 cwProcessTime;
    UWord32 maxAttnProcessTime;
    UWord32 extraParams;
    UWord32 reserved4[5];
    UWord32 crcLsb;
    UWord32 crcMsb;
} MdfStructure;

typedef struct SequenceStructureSent
{
    UWord32 bandResSMin;
    UWord32 sMax;
} SequenceStructureSent;

typedef struct RssStructure
{
    UWord32 opcode;
    SequenceStructureSent s1Data[256];
    SequenceStructureSent s2Data[256];
    UWord32 crcLsb;
    UWord32 crcMsb;

} RssStructure;

class DifmCsiLink {
	static const unsigned int CSI_SLAVE_BASE_REG = 0x83C30000;
	static const unsigned int CSI_CONTROL_OFFSET = 0x00000000;
	static const unsigned int CSI_STATUS_OFFSET = 0x00000004;
	static const unsigned int CSI_TX_OFFSET = 0x00000008;
	static const unsigned int CSI_RX_OFFSET = 0x0000000C;
	static const unsigned int CW_THRESHOLD = 90;
	static const unsigned int DIFM_THRESHOLD = 25;

private:
	RssStructure rssStruct;
	MdfStructure mdfStruct;

public:
	UWord32 csiBuffer[64];

	/*
	 * Function Name:   initialize
	 * Purpose:			This function is used to initialize DIFM CSI link
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void initialize();

	/*
	 * Function Name:   clean
	 * Purpose:			This function cleans up internal data structures of the
	 * 					class i.e. csiBuffeer, mdfStruct and rssStruct
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void clean();

	/*
	 * Function Name:   sendWord
	 * Purpose:			This function sends the word to address offset on DIFM CSI link
	 * Inputs:          address
	 * 					word
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void sendWord(UWord32 address, UWord32 word);

	/*
	 * Function Name:   recvWord
	 * Purpose:			This function receives the data from address offset on DIFM CSI link
	 * Inputs:          address
	 * Outputs:         word
	 * Scope:           Can be called by any function
	 */
	UWord32 recvWord(UWord32 address);

	/*
	 * Function Name:   calculateCrc
	 * Purpose:			This function calculates CRC of the buffer
	 * Inputs:          Buffer of data
	 * 					Size of buffer
	 * Outputs:         CRC
	 * Scope:           Can be called by any function
	 */
	UWord32 calculateCrc(UWord32 *ptr, UWord32 sizeOfStructure);

	/*
	 * Function Name:   fillRssStructure
	 * Purpose:			This function fills RSS Structure from TL or with default values
	 * Inputs:          Flag indicating failure of TL loading
	 * 					TL structure
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void fillRssStructure(bool tlFailure, LibraryStruct &library);

	/*
	 * Function Name:   fillMdfStructure
	 * Purpose:			This function fills MDF Structure from TL or with default values
	 * Inputs:          Flag indicating failure of TL loading
	 * 					TL structure
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void fillMdfStructure(bool tlFailure, LibraryStruct &library);

	/*
	 * Function Name:   writeMdfToCsi
	 * Purpose:			This function implements the main logic of MDF sending to DIFM
	 * 					First, it load MDF from TL, then it calculates its CRC and eventually
	 * 					it sends MDF in the form of 4 byte words through CSI link
	 * Inputs:          Flag indicating failure of TL loading
	 * 					TL structure
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void writeMdfToCsi(bool tlFailure, LibraryStruct &library);

	/*
	 * Function Name:   writeRssToCsi
	 * Purpose:			This function implements the main logic of RSS sending to DIFM
	 * 					First, it load RSS from TL, then it calculates its CRC and eventually
	 * 					it sends RSS in the form of 4 byte words through CSI link
	 * Inputs:          Flag indicating failure of TL loading
	 * 					TL structure
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void writeRssToCsi(bool tlFailure, LibraryStruct &library);

	/*
	 * Function Name:   readCsiData
	 * Purpose:			This function reads data i.e. all the data till 64 words from CSI link
	 * Inputs:          none
	 * Outputs:         Number of words read
	 * Scope:           Can be called by any function
	 */
	UWord8 readCsiData();

	/*
	 * Function Name:   readCsiData
	 * Purpose:			This function sends Self Test activation flag to DIFM and reads the results back
	 * Inputs:          Self Test buffer to be filled with results
	 * 					Type of self test i.e. modular (0) or functional (1)
	 * Outputs:         Number of words read
	 * Scope:           Can be called by any function
	 */
	UWord8 rfBoardSelfTestLogic(UWord32 *st, UWord8 type);

	/*
	 * Function Name:   sendResetToDifm
	 * Purpose:			This function sends reset command to DIFM
	 * Inputs:          None
	 * Outputs:         None
	 * Scope:           Can be called by any function
	 */
	void sendResetToDifm();
};

#endif /* DIFMCSILINK_H_ */
