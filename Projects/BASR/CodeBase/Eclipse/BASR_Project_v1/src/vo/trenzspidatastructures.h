/*
 * trenzspidatastructures.h
 *
 *  Created on: Feb 10, 2021
 *      Author: saad
 */

#ifndef IO_TRENZSPIDATASTRUCTURES_H_
#define IO_TRENZSPIDATASTRUCTURES_H_

#include "../common/common.h"

//These are Blocks and Data Structures used in communication among Trenz and WMMC
//No methods are a part of this file

////////////////////////////////////////
///////// Block-00 Starts //////////////
////////////////////////////////////////

////////////////// INPUT BLOCK /////////////////////

typedef struct STATE_OF_OPERATION_EWSC
{
	UWord8 mainMode;
	UWord8 eof;
	UWord8 threatNum;
	UWord8 ecm;
	UWord8 audio;
	UWord8 blanking;
	UWord8 standby;
//    UWord8 libraryId;
	UWord8 setDate;
	UWord8 pflReq;
} STATE_OF_OPERATION_EWSC;

typedef struct GPS_TIME_YYMMDD_01
{
	UWord16 Year;
} GPS_TIME_YYMMDD_01;

typedef struct GPS_TIME_YYMMDD_02
{
	UWord8 Month;
	UWord8 Day;
} GPS_TIME_YYMMDD_02;

typedef struct M1ewSop
{
	STATE_OF_OPERATION_EWSC sop;
	GPS_TIME_YYMMDD_01 year;
	GPS_TIME_YYMMDD_02 mmdd;
} M1ewSop;

////////////////// INPUT BLOCK ENDS ////////////////

////////////////// OUTPUT BLOCK ////////////////////

typedef struct STATUS_1
{
	UWord8 mainMode;
	UWord8 eof;
	UWord8 threatNum;
	UWord8 ecm;
	UWord8 audio;
	UWord8 blanking;
	UWord8 standby;
	UWord8 dateNotSet;
	UWord8 newPfl3;
	UWord8 newPfl4;
	UWord8 pfl3;
	UWord8 pfl4;
} STATUS_1;

typedef struct STATUS_2
{
	UWord8 recordingMemStatus;
//    UWord8 defaultLibrary;
//    UWord8 libsStatus[6];
} STATUS_2;

typedef struct ACK_NAK
{
	UWord8 ackNakStatus;
	UWord8 direct;
	UWord8 pBlock;
	UWord8 lBlock;
} ACK_NAK;

typedef struct VIS_WRN_REQ
{
	UWord8 missileLaunch;
	UWord8 beingTracked;
	UWord8 threatFound;
} VIS_WRN_REQ;

typedef struct Ewm1Sop
{
	STATUS_1 status1Sop;
	STATUS_2 status2Sop;
	UWord16 spare;
	ACK_NAK ackNak;
	VIS_WRN_REQ visWrnReq;
} Ewm1Sop;

////////////////////////////////////////
/////////// Block-00 Ends //////////////
////////////////////////////////////////

////////////////////////////////////////
///////// Block-01 Starts //////////////
////////////////////////////////////////

////////////////// OUTPUT BLOCK ////////////////////

typedef struct TGT_BLOCK_HEADER
{
	UWord8 blockNum;
	UWord8 lastBlock;
} TGT_BLOCK_HEADER;

typedef struct THREAT_VALIDITY
{
	UWord8 tgtValid[7];
} THREAT_VALIDITY;

typedef struct THREAT_TGT_PARA_1
{
	UWord8 batchNum;
	UWord8 radarName;
} THREAT_TGT_PARA_1;

typedef struct THREAT_TGT_PARA_2
{
	UWord8 attribute;
	UWord8 status;
	UWord8 platform;
	UWord8 unkTgt;
	UWord8 maxPrtyTgt;
	UWord8 tgtPresent;
	UWord8 priority;
} THREAT_TGT_PARA_2;

typedef struct THREAT_TGT_AZIMUTH
{
	UWord16 azimuth;
} THREAT_TGT_AZIMUTH;

typedef struct THREAT_TGT_DISTANCE
{
	UWord16 distance;
} THREAT_TGT_DISTANCE;

typedef struct THREAT_TGT_DATA
{
	THREAT_TGT_PARA_1 tgtPara1;
	THREAT_TGT_PARA_2 tgtPara2;
	THREAT_TGT_AZIMUTH tgtAzimuth;
	THREAT_TGT_DISTANCE tgtDistance;
} THREAT_TGT_DATA;

typedef struct Ewm1TgtData
{
	TGT_BLOCK_HEADER header;
	THREAT_VALIDITY threatsValidity;
	THREAT_TGT_DATA threatsData[7];
} Ewm1TgtData;

////////////////////////////////////////
/////////// Block-01 Ends //////////////
////////////////////////////////////////

////////////////////////////////////////
///////// Block-03 Starts //////////////
////////////////////////////////////////

////////////////// INPUT BLOCK /////////////////////

typedef struct CFD_DISPLAY_DATA_REQUEST
{
	UWord8 cfRequest;
	UWord8 progTypeRequest;
	UWord8 progListRequest;

} CFD_DISPLAY_DATA_REQUEST;

typedef struct CFD_PARAMS_UPDATE
{
	UWord8 dispenseProgNum;
	UWord8 dispenseType;
} CFD_PARAMS_UPDATE;

typedef struct M1ewCfdRequest
{
	CFD_DISPLAY_DATA_REQUEST cfdDataRequest;
	CFD_PARAMS_UPDATE cfdParams;
} M1ewCfdRequest;

////////////////// INPUT BLOCK ENDS ////////////////

////////////////// OUTPUT BLOCK ////////////////////

typedef struct CFD_COUNT
{
	UWord8 numChaff;
	UWord8 numFlare;
} CFD_COUNT;

typedef struct CFD_PROG_DATA
{
	UWord8 dispenseProgNum;
	UWord8 dispenseType;
	UWord8 dispenseStatus;
	UWord8 dispenseMode;
} CFD_PROG_DATA;

typedef struct CFD_PRESET
{
	UWord8 presetChaff;
	UWord8 presetFlare;
} CFD_PRESET;

typedef struct Ewm1CfdRequest
{
	CFD_COUNT cfdCount;
	CFD_PROG_DATA cfdProgData;
	CFD_PRESET cfdPreset;
} Ewm1CfdRequest;

////////////////// OUTPUT BLOCK ENDS ///////////////

////////////////////////////////////////
/////////// Block-03 Ends //////////////
////////////////////////////////////////

////////////////////////////////////////
///////// Block-04 Starts //////////////
////////////////////////////////////////

////////////////// INPUT BLOCK /////////////////////

//block 1
typedef struct LOAD_TO_EWSC_BID
{
	UWord8 blockId;
	UWord8 lastBlock;
} LOAD_TO_EWSC_BID;

typedef struct DISPENSE_PROG_NUMBER
{
	UWord8 dispenseProgNum;

} DISPENSE_PROG_NUMBER;

typedef struct DISPENSE_PROG_PRMTR
{
	UWord8 numChaffGroup;
	UWord8 intervalChaffGroup;
	UWord8 chaffPerGroup;
	UWord8 sglDblChaff;
	UWord8 intervalChaff;
	UWord8 numFlare;
	UWord8 intervalFlare;
	UWord8 sglDblFlare;

} DISPENSE_PROG_PRMTR;

typedef struct DISPENSE_PROG
{
	DISPENSE_PROG_NUMBER dispenseProgNum;
	DISPENSE_PROG_PRMTR dispenseProgParams;

} DISPENSE_PROG;

//block 2
typedef struct PRESET_CHAFF_AND_FLARE
{
	UWord8 presetChaff;
	UWord8 presetFlare;
} PRESET_CHAFF_AND_FLARE;

typedef struct M1ewCfdProgram
{
	LOAD_TO_EWSC_BID blockInfo;
	DISPENSE_PROG dispenseProg[8];
	PRESET_CHAFF_AND_FLARE presetCF;
} M1ewCfdProgram;

////////////////// INPUT BLOCK ENDS ////////////////

////////////////// OUTPUT BLOCK ////////////////////

typedef struct Ewm1CfdProgram
{
	DISPENSE_PROG_PRMTR dispenseProg[15];
} Ewm1CfdProgram;

////////////////// OUTPUT BLOCK ENDS ///////////////

////////////////////////////////////////
/////////// Block-04 Ends //////////////
////////////////////////////////////////

////////////////////////////////////////
///////// Block-05 Starts //////////////
////////////////////////////////////////

////////////////// INPUT BLOCK /////////////////////

typedef struct DATA_VALIDITY
{
	UWord8 altitudeValid;
	UWord8 headingValid;
	UWord8 pitchRollValid;
	UWord8 insNavDataValid;
	UWord8 insDataValid;
} DATA_VALIDITY;

typedef struct M1ewAcData
{
	DATA_VALIDITY dataValid;
	UWord32 timeTag;
	Word16 velNorth;
	Word16 velWest;
	Word16 velUp;
	Word16 roll;
	Word16 pitch;
	Word16 heading;
	Word16 rollRate;
	Word16 pitchRate;
	Word16 yawRate;
	Word16 altitude;
	Word32 latitude;
	Word32 longitude;
} M1ewAcData;

////////////////// INPUT BLOCK ENDS ////////////////

////////////////////////////////////////
/////////// Block-05 Ends //////////////
////////////////////////////////////////

////////////////////////////////////////
///////// Block-19 Starts //////////////
////////////////////////////////////////

////////////////// OUTPUT BLOCK ////////////////////

typedef struct LEVEL_3_PFL1
{
	UWord8 identificationFailure;
	UWord8 cfdCommFailure;
	UWord8 sysCntlrFailure;
	UWord8 detectionFailure;
} LEVEL_3_PFL1;

typedef struct LEVEL_4_PFL1
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
} LEVEL_4_PFL1;

typedef struct Ewm1Pfl
{
	LEVEL_3_PFL1 pflLevel3;
	LEVEL_4_PFL1 pflLevel4;
} Ewm1Pfl;

////////////////// OUTPUT BLOCK ENDS ///////////////

////////////////////////////////////////
/////////// Block-19 Ends //////////////
////////////////////////////////////////

////////////////////////////////////////
///////// Block-20 Starts //////////////
////////////////////////////////////////

////////////////// INPUT BLOCK /////////////////////

typedef struct MAINT_BID
{
	UWord8 blockId;
} MAINT_BID;

typedef struct MAINT_REQ_EW     // Block 1
{
	UWord8 step;
	UWord8 mflReq;
	UWord8 mflClear;
} MAINT_REQ_EW;

typedef struct MBIT_CMD         // Block 2
{
	UWord8 mbit;
} MBIT_CMD;

typedef struct TABLE_CHECK      // Block 4
{
	UWord8 ewTableCheck;
} TABLE_CHECK;

typedef struct LOAD_CMD         // Block 5
{
	UWord8 loadCmd;
} LOAD_CMD;

typedef struct M1ewMaint
{
	MAINT_BID block;
	MAINT_REQ_EW reqEw;
	MBIT_CMD mbit;
	TABLE_CHECK tlCheckStatus;
	LOAD_CMD load;
} M1ewMaint;

////////////////// INPUT BLOCK ENDS ////////////////

////////////////// OUTPUT BLOCK ////////////////////

typedef struct MFL_STATUS               // Block 1
{
	UWord8 status;
} MFL_STATUS;

typedef struct THREAT_TGT_DATA_STATUS
{
	UWord8 exists;
} THREAT_TGT_DATA_STATUS;

typedef struct SW_VERSION
{
	UWord8 majorVer[2];
	UWord8 minorVer[2];
} SW_VERSION;

typedef struct SW_VER_DATE_YEAR
{
	UWord16 year;
} SW_VER_DATE_YEAR;

typedef struct SW_VER_DATE_MONTH
{
	UWord8 month;
} SW_VER_DATE_MONTH;

typedef struct SW_VER_DATE_DAY
{
	UWord8 day;
} SW_VER_DATE_DAY;

typedef struct MBIT_STATUS              // Block 2
{
	UWord8 status;
} MBIT_STATUS;

typedef struct MFL_REPORT_BID           // Block 3
{
	UWord8 flrExists;
	UWord8 lastBlock;
} MFL_REPORT_BID;

typedef struct MFL_SYS_SUBSYS
{
	UWord8 sys1;
	UWord8 sys2;
	UWord8 subsys;
} MFL_SYS_SUBSYS;

typedef struct MFL_LRU_FCTN
{
	UWord8 lru1;
	UWord8 lru2;
	UWord8 lru3;
	UWord8 fctn1;
	UWord8 fctn2;
} MFL_LRU_FCTN;

typedef struct MFL_FLR_OCCUR_TIME
{
	UWord32 occurance;
} MFL_FLR_OCCUR_TIME;

typedef struct MFL_FLR_DURATION_TIME
{
	UWord32 duration;
} MFL_FLR_DURATION_TIME;

typedef struct MFL_FLR_AMOUNT
{
	UWord16 count;
} MFL_FLR_AMOUNT;

typedef struct MFL_FLR_DESCRIPTION
{
	UWord8 description[40];
} MFL_FLR_DESCRIPTION;

typedef struct EW_TABLE_CHECK_STATUS       // Block 4
{
	UWord8 cfdManExists;
	UWord8 cfdAutoExists;
	UWord8 tlStatus[6];
} EW_TABLE_CHECK_STATUS;

typedef struct EW_TABLE_VERSION
{
	UWord8 firstChar;
	UWord8 secondChar;
	UWord8 thirdChar;
	UWord8 fourthChar;
} EW_TABLE_VERSION;

typedef struct KEY_CHECK_STATUS         // Block 5
{
	UWord8 key_selected;
	UWord8 key_count;
	UWord8 key_lib_status;
} KEY_CHECK_STATUS;

typedef struct KEY_VERSION
{
	UWord8 first_char;
	UWord8 second_char;
	UWord8 third_char;
	UWord8 fourth_char;
} KEY_VERSION;

typedef struct LOAD_STATUS               // Block 6
{
	UWord8 status;
} LOAD_STATUS;

typedef struct Ewm1Maint
{
	MAINT_BID block;

	MFL_STATUS mflStatus;               // Block 1
	THREAT_TGT_DATA_STATUS tgtStatus;
	SW_VERSION swVersion;
	SW_VER_DATE_YEAR year;
	SW_VER_DATE_MONTH month;
	SW_VER_DATE_DAY day;

	MBIT_STATUS mbitStatus;             // Block 2

	MFL_REPORT_BID mflBlockId;          // Block 3
	MFL_SYS_SUBSYS sysSubsys;
	MFL_LRU_FCTN lruFctn;
	MFL_FLR_OCCUR_TIME occurTime;
	MFL_FLR_DURATION_TIME durationTime;
	MFL_FLR_AMOUNT amount;
	MFL_FLR_DESCRIPTION description;

	EW_TABLE_CHECK_STATUS tlStatus;     // Block 4
	EW_TABLE_VERSION tlVersions[6];

	LOAD_STATUS loadStatus;             // Block 5
} Ewm1Maint;

////////////////// OUTPUT BLOCK ENDS ///////////////

////////////////////////////////////////
/////////// Block-20 Ends //////////////
////////////////////////////////////////

////////////////////////////////////////
///////// Block-21 Starts //////////////
////////////////////////////////////////

////////////////// OUTPUT BLOCK ////////////////////

typedef struct EOF_RECORD_BID
{
	UWord8 lastBlock;
} EOF_RECORD_BID;

typedef struct SYS_SUBSYS
{
	UWord8 sys1;
	UWord8 sys2;
	UWord8 subsys;
} SYS_SUBSYS;

typedef struct LRU_FCTN
{
	UWord8 lru1;
	UWord8 lru2;
	UWord8 lru3;
	UWord8 fctn1;
	UWord8 fctn2;
} LRU_FCTN;

typedef struct FLR_OCCUR_TIME
{
	UWord32 occurance;
} FLR_OCCUR_TIME;

typedef struct FLR_DURATION_TIME
{
	UWord32 duration;
} FLR_DURATION_TIME;

typedef struct FLR_AMOUNT
{
	UWord16 count;
} FLR_AMOUNT;

typedef struct FLR_DESCRIPTION
{
	UWord8 description[40];
} FLR_DESCRIPTION;

typedef struct Ewm1Eof
{
	EOF_RECORD_BID lastBlock;
	SYS_SUBSYS sysSubsys;
	LRU_FCTN lruFctn;
	FLR_OCCUR_TIME occurTime;
	FLR_DURATION_TIME durationTime;
	FLR_AMOUNT amount;
	FLR_DESCRIPTION description;
} Ewm1Eof;

////////////////// OUTPUT BLOCK ENDS ///////////////

////////////////////////////////////////
/////////// Block-21 Ends //////////////
////////////////////////////////////////

typedef struct SpiRtcData
{
	UWord32 delimeter;
	UWord16 messageCode;
	UWord32 rtc;
} SpiRtcData;

typedef struct SpiCommandReply
{
	UWord32 delimeter;
	UWord16 messageCode;
	UWord16 CRC;
	UWord8 opcode;
	UWord8 options;
	UWord8 audioRequest;
} SpiCommandReply;

typedef struct SpiFilenameReply
{
	UWord32 delimeter;
	UWord16 messageCode;
	UWord16 CRC;
	UWord8 opcode;
	UWord8 numberOfFiles;
	Word8 name[6][4];
} SpiFilenameReply;

typedef struct SpiTrackingData
{
	UWord8 radarCategory;
	UWord8 radarID;
	UWord8 attribute;
	UWord8 status;
	UWord8 platform;
	UWord8 unknownTarget;
	UWord8 maxPriorityThreat;
	UWord8 threatPresent;
	UWord8 priority;
	UWord8 distance;
	UWord16 angleOfArrival;
} SpiTrackingData;

typedef struct SpiTrackReply
{
	UWord32 delimeter;
	UWord16 messageCode;
	UWord16 CRC;
	UWord8 numberOfTracks;
	UWord16 audioID;
	SpiTrackingData trackingData[24];
} SpiTrackReply;

typedef struct SpiSelfTestDataReply
{
	UWord32 delimeter;
	UWord16 messageCode;
	UWord16 CRC;
	UWord32 faultData[11];
} SpiSelfTestDataReply;

typedef struct TrenzCommunicationHeader
{
	UWord32 delimeter;
	UWord16 messageType;
	UWord16 crc;
} TrenzCommunicationHeader;

typedef struct SendReplyToTrenz
{
	TrenzCommunicationHeader trenzCommunicationHeader;
	UWord16 messageData[32];
} SendReplyToTrenz;

#endif /* IO_TRENZSPIDATASTRUCTURES_H_ */
