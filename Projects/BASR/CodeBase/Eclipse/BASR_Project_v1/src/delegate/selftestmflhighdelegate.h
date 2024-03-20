/*
 * selftestmfldelegate.h
 *
 *  Created on: Jun 25, 2021
 *      Author: zumair
 */

#ifndef DELEGATE_SELFTESTMFLHIGHDELEGATE_H_
#define DELEGATE_SELFTESTMFLHIGHDELEGATE_H_

#include "../utility/consthigh.h"
#include "../common/common.h"
#include "../io/fileoutputflightrecordhigh.h"
#include "../io/selftestmflhandlerhigh.h"

typedef struct MflStructure
{
	UWord8 exists;
	UWord8 sys1;
	UWord8 sys2;
	UWord8 subsys;
	UWord8 lru[3];
	UWord8 fctn1;
	UWord8 fctn2;
	UWord16 amount;
	UWord32 occurTime;
	UWord32 durationTime;
	UWord8 description[40];
} MflStructure;

typedef struct SpiMflData
{
	UWord32 delimeter;
	UWord8 messageCode;
	UWord16 CRC;
	UWord8 lastBlock;
	MflStructure mfl;
} SpiMflData;

typedef struct SingleFaultData
{
	UWord8 packetId;
	UWord8 module;
	UWord8 faultId;
	UWord8 count;
	UWord8 status;
} SingleFaultData;

typedef struct Level3Pfl
{
	UWord8 identificationFailure;
	UWord8 cfdCommFailure;
	UWord8 sysCntlrFailure;
	UWord8 detectionFailure;
} Level3Pfl;

typedef struct Level4Pfl
{
	UWord8 difmProcessorFailure;
	UWord8 vdmProcessorFailure;
	UWord8 zeroizerFailure;
	UWord8 blankingFailure;
	UWord8 opsTlFailure;
	UWord8 cwFailure;
	UWord8 audioFailure;
	UWord8 recordingFailure;
	UWord8 frontLeftRxFailure;
	UWord8 backLeftRxFailure;
	UWord8 backRightRxFailure;
	UWord8 frontRightRxFailure;
} Level4Pfl;

typedef struct PflStructure
{
	UWord32 delimeter;
	UWord8 messageCode;
	UWord16 CRC;
	Level3Pfl pflLevel3;
	Level4Pfl pflLevel4;
} PflStructure;

class SelfTestMflHigh
{
	static const unsigned int CPF_ERROR_COUNT = 11;
	static const unsigned int PPF_ERROR_COUNT = 15;
	static const unsigned int CCP_ERROR_COUNT = 11;
	static const unsigned int TP_ERROR_COUNT = 7;
	static const unsigned int DIFM_ERROR_COUNT = 62;
	static const unsigned int DIFM_AUTO_ERROR_COUNT = 14;
	static const unsigned int CWP_ERROR_COUNT = 24;
	static const unsigned int CWP_AUTO_ERROR_COUNT = 6;
	static const unsigned int DCM_ERROR_COUNT = 5;
	static const unsigned int DCM_AUTO_ERROR_COUNT = 3;
	static const unsigned int DRU_ERROR_COUNT = 4;
	static const unsigned int TOTAL_ERROR_COUNT = (CPF_ERROR_COUNT
			+ PPF_ERROR_COUNT + CCP_ERROR_COUNT + TP_ERROR_COUNT
			+ DIFM_ERROR_COUNT + CWP_ERROR_COUNT + DCM_ERROR_COUNT
			+ DRU_ERROR_COUNT);
private:
	Word8 command[80];
	SelfTestMflHandlerHigh mflHandler;
public:
	PflStructure oldPfls;
	PflStructure pfl;
	UWord8 *pMfl;
	UWord8 mflData[750];

	SingleFaultData fault;
	MflStructure mfl;
	UWord8 newPfl3;
	UWord8 newPfl4;
	UWord8 existencePfl3;
	UWord8 existencePfl4;
	UWord8 pflRelatedAsyncSopNeededFlag;

	SpiMflData spiMfl;

	/*
	 * Function Name:   initMflDirectory
	 * Purpose:			This function checks whether /tmp/emmc/mfl/ directory exists, if it
	 * 					doesn't, it makes the directory and makes all the essential files of mfl
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void initMflDirectory();

	/*
	 * Function Name:   initMflFiles
	 * Purpose:			This function initializes all the mfl files used to record faults in
	 * 					/mfl
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void initMflFiles();

	void initMflStructure();

	/*
	 * Function Name:   fillFaultPacketInfo
	 * Purpose:			This function extracts results out of readData
	 * Inputs:          Result of Self Test data by name of readData
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void fillFaultPacketInfo(UWord32 readData);

	/*
	 * Function Name:   updateMflEntry
	 * Purpose:			This function updates the mfl files corresponding to fault
	 * Inputs:          fault
	 * 					Time
	 * 					Flight Record Maintainance header
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void updateMflEntry(SingleFaultData fault, uint32_t rtcTime,
			FlightRecordHeader &flightRecordMaintainanceHeader);

	/*
	 * Function Name:   fillPflStructure
	 * Purpose:			This function fills PFL structure with respect to index given to it
	 * Inputs:          index
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void fillPflStructure(UWord8 index);
	void fillSpecialPfls(UWord32 channelStatus);

	/*
	 * This function checks if pfl changes
	 * Before a self test we store old pfls present, and after self test we compare with old pfls
	 * It returns true if any new pfl found that has not been reported earlier
	 * GIVEN that the caller saves the current pfl into old pfls before any self test*/

	UWord8 checkPfl3Existence(Level3Pfl &pflStruct);
	UWord8 checkPfl4Existence(Level4Pfl &pflStruct);

	UWord8 checkNewPfl3Existence();
	UWord8 checkNewPfl4Existence();

	UWord8 checkPflFlagChanged();

	void clearPflStruct();

	//called after a self test is completed
	/*selfTestType is
		0 cfd status update
		1 periodic BIT (only modular self test)
		2 periodic BIT (modular + functional self test)
		3 IBIT
		4 MBIT */
	void updatePflFlags(UWord8 selfTestType);

	void updateCfdCommFlag(UWord8 cfdUpStatus);

	/*
		This function needs to be called when WMMC has been notified that 
		EWSC updated pfl flags (on change of pfls)
	*/
	void onPflStatusMessageSent();

};

#endif /* DELEGATE_SELFTESTMFLHIGHDELEGATE_H_ */
