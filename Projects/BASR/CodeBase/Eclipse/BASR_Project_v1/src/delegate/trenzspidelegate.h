/*
 * trenzspidelegate.h
 *
 *  Created on: Apr 4, 2021
 *      Author: zumair
 */

#ifndef DELEGATE_TRENZSPIDELEGATE_H_
#define DELEGATE_TRENZSPIDELEGATE_H_

#include "../model/model.h"
#include "../utility/consthigh.h"

static const unsigned int SYSTEM_ERROR = 0;
static const unsigned int SYSTEM_OK = 1;
static const unsigned int DELIMETER_ERROR = 2;
static const unsigned int CRC_ERROR = 3;
static const unsigned int LIBRARY_CRC_ERROR = 4;
static const unsigned int KEY_ERROR = 5;

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
static const unsigned int TOTAL_ERROR_COUNT = (CPF_ERROR_COUNT + PPF_ERROR_COUNT
		+ CCP_ERROR_COUNT + TP_ERROR_COUNT + DIFM_ERROR_COUNT + CWP_ERROR_COUNT
		+ DCM_ERROR_COUNT + DRU_ERROR_COUNT);

#ifdef TRENZSPIDELEGATE_CPP_
UWord16 startPoint = 0;
UWord8 lastOne = 0;
UWord8 mflPresent = 0;
UWord8 faultArray[TOTAL_ERROR_COUNT];
UWord8 spiAudioRequest = 0;
UWord8 trenzSelfStarted = 0;
UWord8 trenzSelfFinished = 0;
SpiSelfTestDataReply *pSpiSelfTestResults;
SpiSelfTestDataReply spiSelfTestResults;
M1ewSop sopIn;
Ewm1Sop sopOut;
M1ewCfdRequest cfdRequestIn;
Ewm1CfdRequest cfdRequestOut;
M1ewCfdProgram cfdProgramIn;
Ewm1CfdProgram cfdProgramOut;
Ewm1Pfl pflOut;
M1ewMaint maintIn;
Ewm1Maint maintOut;
uint64_t timeStampLastAyncSop = 0;
#else

extern UWord16 startPoint;
extern UWord8 lastOne;
extern UWord8 mflPresent;
extern UWord8 faultArray[TOTAL_ERROR_COUNT];
extern UWord8 spiAudioRequest;
extern UWord8 trenzSelfStarted;
extern UWord8 trenzSelfFinished;
extern SpiSelfTestDataReply *pSpiSelfTestResults;
extern SpiSelfTestDataReply spiSelfTestResults;
extern M1ewSop sopIn;
extern Ewm1Sop sopOut;
extern M1ewCfdRequest cfdRequestIn;
extern Ewm1CfdRequest cfdRequestOut;
extern M1ewCfdProgram cfdProgramIn;
extern Ewm1CfdProgram cfdProgramOut;
extern Ewm1Pfl pflOut;
extern M1ewMaint maintIn;
extern Ewm1Maint maintOut;
extern uint64_t timeStampLastAyncSop;

#endif

/*
 * Function Name:   extractTrenzMessage
 * Purpose:			This function is the brain of communication amongst Trenz and CCP
 * 					It extracts messages from iterator, checks the delimeter and parses
 * 					the data to appropriate class data buffers
 * Inputs:          Iterator
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void extractTrenzMessage(UWord8 iterator);

/*
 * Function Name:   fillMflData
 * Purpose:			This function fills Output Maintainance structure from data extracted from
 * 					SPI mfl structure
 * Inputs:          Maintainance structure that is to be filled and logical block number
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillMflData(Ewm1Maint *outputMaintenance, UWord8 lb);

/*
 * Function Name:   mflMessagesProcess
 * Purpose:			This function sends MFL to Trenz according to /tmp/emmc/mfl/current.txt file
 * Inputs:          none
 * Outputs:         none
 * Scope:           Can be called by any function
 */
// void mflMessagesProcess();

/*
 * Function Name:   extractSelfTestResults
 * Purpose:			This function extracts self test results from data buffer
 * Inputs:          none
 * Outputs:         Success/Failure of extraction
 * Scope:           Can be called by any function
 */
UWord8 extractSelfTestResults();

/*
 * Function Name:   startInitiatedBuiltInSelfTestProcess
 * Purpose:			This function calls Initiated Self Test if requested by Trenz
 * Inputs:          none
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void startInitiatedBuiltInSelfTestProcess();

/*
 * Function Name:   startMaintainanceBuiltInSelfTestProcess
 * Purpose:			This function calls Maintenance Self Test if requested by Trenz
 * Inputs:          none
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void startMaintainanceBuiltInSelfTestProcess();

/*
 * Function Name:   parseSopMessage
 * Purpose:			This function parses SOP messages from Trenz to Output SOP
 * 					and takes relevant actions as instructed by SOP messages
 * Inputs:          Relevant structures and objects
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void parseSopMessage(M1ewSop *inputSop, Ewm1Sop *outputSop);

/*
 * Function Name:   mflMessagesProcess
 * Purpose:			This function sends MFL to Trenz according to /tmp/emmc/mfl/current.txt file
 * Inputs:          none
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void mflMessagesProcess();
void updateMflPresentFlag();

/*
 * Function Name:   sendMflFault
 * Purpose:			This function finds the mfl file to be opened and sends it to Trenz
 * Inputs:          Count and lastOne
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void sendMflFault(UWord16 count, UWord8 lastOne);

/*
 * Function Name:   flightRecordMemoryPercentageToBcd
 * Purpose:			This function finds the percentage of Flight Record already written
 * 					and converts it to BCD according to its percentage
 * Inputs:          none
 * Outputs:         Memory percentage as BCD
 * Scope:           Can be called by any function
 */
UWord8 flightRecordMemoryPercentageToBcd();

/*
 * Function Name:   fillEofData
 * Purpose:			This function fills EoF data structure from SPI data
 * Inputs:          Output EoF structure that is to be filled
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillEofData(Ewm1Eof *outputEof);

/*
 * Function Name:   checkPflChange
 * Purpose:			This function searches for PFL changes through output sop message
 * 					and if there are some updation, it updates output sop accordingly
 * Inputs:          Output SOP structure
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void checkPflChange(Ewm1Sop *outputSop);

/*
 * Function Name:   parseInputMaintenanceMessage
 * Purpose:			This function parses maintenance messages to relevant class objects and
 * 					returns reply type to be sent to Trenz
 * Inputs:          Maintenance buffer and objects of relevant classes
 * Outputs:         Reply type
 * Scope:           Can be called by any function
 */
Word32 parseInputMaintenanceMessage(M1ewMaint *maintenance,
		Ewm1Maint *outputMaintenance);

/*
 * Function Name:   mflClearProcess
 * Purpose:			This function clears all the mfl files present in /tmp/emmc/mfl/ directory
 * Inputs:          none
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void mflClearProcess();

/*
 * Function Name:   fillMflData
 * Purpose:			This function fills Output Maintainance structure from data extracted from
 * 					SPI mfl structure
 * Inputs:          Maintainance structure that is to be filled and logical block number
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillMflData(Ewm1Maint *outputMaintenance, UWord8 lb);

void updatePflFlagsInSopOut(Ewm1Sop &);
void onBitCompletionLogic();

void updateFlagsForNextAsyncSop();
/*
 * Function Name:   fillSopOutStructure
 * Purpose:			This function fills output sop structure from input sop structure
 * Inputs:          Input and Output SOP structures
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillSopOutStructure(M1ewSop *inputSop, Ewm1Sop *outputSop);

/*
 * Function Name:   sendPflLogic
 * Purpose:			This function is the main logic of pfl sent to Trenz
 * 					First it fills PFL structure, fills the maintenance structure
 * 					according to PFL and sends it to Trenz
 * Inputs:          none
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void sendPflLogic();

/*
 * Function Name:   fillPflData
 * Purpose:			This function fills output maintenance data structure from PFL data structure
 * Inputs:          PFL structure
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillPflData(Ewm1Pfl *outputPfl);

/*
 * Function Name:   SpiSelftTestPbitLogic
 * Purpose:			This function is the main logic of Modular Test of Trenz
 * 					Here, first we initiate self test, wait for its completion
 * 					and eventually extract its results to respective object
 * Inputs:          Data buffer
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void SpiSelftTestPbitLogic(UWord32 *input);


//Block-00
/*
 * Function Name:   fillM1ewSopStructure
 * Purpose:			This function fills relavant structure from buffer
 * Inputs:          Structure to be filled
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillM1ewSopStructure(M1ewSop *sop);

/*
 * Function Name:   fillEwm1SopBuffer
 * Purpose:			This function fills relavant structure from buffer
 * Inputs:          Structure to be filled
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillEwm1SopBuffer(Ewm1Sop *sop);

//Block-01
/*
 * Function Name:   fillEwm1TgtDataBuffer
 * Purpose:			This function fills relavant structure from buffer
 * Inputs:          Structure to be filled
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillEwm1TgtDataBuffer(Ewm1TgtData *tgtData);

//Block-03
/*
 * Function Name:   fillM1ewCfdRequestStructure
 * Purpose:			This function fills relavant structure from buffer
 * Inputs:          Structure to be filled
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillM1ewCfdRequestStructure(M1ewCfdRequest *cfdRequest);

/*
 * Function Name:   fillEwm1CfdRequestBuffer
 * Purpose:			This function fills relavant structure from buffer
 * Inputs:          Structure to be filled
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillEwm1CfdRequestBuffer(Ewm1CfdRequest *cfdRequest);

//Block-04
/*
 * Function Name:   fillM1ewCfdProgramStructure
 * Purpose:			This function fills relavant structure from buffer
 * Inputs:          Structure to be filled
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillM1ewCfdProgramStructure(M1ewCfdProgram *cfdProgram);

/*
 * Function Name:   fillEwm1CfdProgramBuffer
 * Purpose:			This function fills relavant structure from buffer
 * Inputs:          Structure to be filled
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillEwm1CfdProgramBuffer(Ewm1CfdProgram *cfdProgram);

//Block-05
/*
 * Function Name:   fillM1ewAcDataStructure
 * Purpose:			This function fills relavant structure from buffer
 * Inputs:          Structure to be filled
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillM1ewAcDataStructure(M1ewAcData *acData);

//Block-19
/*
 * Function Name:   fillEwm1PflBuffer
 * Purpose:			This function fills relavant structure from buffer
 * Inputs:          Structure to be filled
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillEwm1PflBuffer(Ewm1Pfl *pfl);

//Block-20
/*
 * Function Name:   fillM1ewMaintStructure
 * Purpose:			This function fills relavant structure from buffer
 * Inputs:          Structure to be filled
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillM1ewMaintStructure(M1ewMaint *maint);

/*
 * Function Name:   fillEwm1MaintBuffer
 * Purpose:			This function fills relavant structure from buffer
 * Inputs:          Structure to be filled and logical block number
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillEwm1MaintBuffer(Ewm1Maint *maint, int lb);

//Block-21
/*
 * Function Name:   fillEwm1EofBuffer
 * Purpose:			This function fills relavant structure from buffer
 * Inputs:          Structure to be filled
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void fillEwm1EofBuffer(Ewm1Eof *eof);

void sendAsyncSop(int ackNack, int pb, int lb);

void checkAudioAsyncSopNeeded();

#endif /* DELEGATE_TRENZSPIDELEGATE_H_ */
