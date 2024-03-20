/*
 * FileOutputFlightRecordHigh.h
 *
 *  Created on: Feb 3, 2021
 *      Author: saad
 */

#ifndef FILEOUTPUTFLIGHTRECORDHIGH_H_
#define FILEOUTPUTFLIGHTRECORDHIGH_H_

#include "../common/common.h"
#include "../vo/libraryclass.h"
#include "../utility/sha256.h"
#include "../utility/aes.h"

typedef struct LegacyEmitterRecordReport
{
	UWord64 TOAStart, TOAEnd, Range;
	UWord64 ScanPeriod;

	UWord32 StaggerFrame[6];
	UWord32 StaggerPhase[64];
	UWord32 DwellPRISpot[32];
	UWord32 JitterPRIMin, JitterPRIMax;
	UWord32 ComplexPRIMin, ComplexPRIMax;
	UWord32 SlidingPRIMin, SlidingPRIMax;

	UWord16 EmitterID;
	UWord16 PulseCountQuadrant1, PulseCountQuadrant2;
	UWord16 PxQuadrant1, PxQuadrant2, PyQuadrant1, PyQuadrant2;
	UWord16 AngleOfArrival;
	UWord16 RFSpots[16];
	UWord16 PulseWidthSpot[8];

	UWord8 Quadrant1, Quadrant2;
	UWord8 New_Updated_Flag;
	UWord8 Band;
	UWord8 Scan_Track;
	UWord8 PWType;
	UWord8 RFType;
	UWord8 PRIType;
	UWord8 StaggerPhaseCountFrame[6];
	UWord8 LPIType;
	UWord8 SlideType;
	UWord8 SweepType;
	UWord8 SelfTest;
	UWord8 StaggerPRI_QualityFlag, SlidingPRI_QualityFlag,
			JitterPRI_QualityFlag, ComplexPRI_QualityFlag;

	UWord8 DwellPRI_QualityFlag[32];
	UWord8 RF_QualityFlag[16];
	UWord8 PW_QualityFlag[8];

} LegacyEmitterRecordReport;

typedef struct FlightRecordAircraftNavigationInformation
{
	UWord8 sessionId;
	UWord16 upVelocity;
	UWord16 northVelocity;
	UWord16 westVelocity;
	UWord16 roll;
	UWord16 pitch;
	UWord16 heading;
	UWord16 rollRate;
	UWord16 pitchRate;
	UWord16 yawRate;
	UWord16 altitude;
	UWord32 latitude;
	UWord32 longitude;
} FlightRecordAircraftNavigationInformation;

typedef struct FlightRecordSizeHeader
{
	UWord32 crc;
	UWord32 size;
	UWord32 overFlowFlag;
} FlightRecordSizeHeader;

typedef struct FlightRecordHeader
{
	UWord64 Delimiter;
	UWord16 CRC;
	UWord8 MessageType;
	UWord8 structureCount;
	UWord32 RtcTime;
} FlightRecordHeader;

typedef struct GenralProperties
{
	UWord8 Pilot[16];
	UWord8 Aircraft[16];
	UWord16 Date;
	UWord16 Time;
} GenralProperties;

typedef struct Time
{
	UWord8 hour;
	UWord8 minute;
	UWord8 second;
} Time;

typedef struct Date
{
	UWord8 day;
	UWord8 month;
	UWord16 year;
} Date;

typedef struct ModeID
{
	UWord8 modeID[2];
} ModeID;

typedef struct FlightRecord
{
	UWord8 matchCount; //number of matched emitters out of 3
	UWord8 trackUpdateStatus;
	UWord8 priority;
	UWord16 SymbolCode1[3];
	UWord16 SymbolCode2[3];
	UWord16 SymbolCode3[3];
	UWord16 SymbolCode4[3];
	UWord16 emitterId[3];
	UWord16 batchId[3];
	UWord16 OuterCode[3];
	ModeID modeId[3];
	UWord32 MatchScore[3];
	UWord32 MatchIndex[3];
	Time time;
	Date date;
	LegacyEmitterRecordReport EmitterData;
} FlightRecord;

//totalsize= 172088
typedef struct EmitterLog
{
	FlightRecordHeader flightRecordHeader;
	FlightRecord Flights[256];
} EmitterLog;

typedef struct EncryptionHeader
{
	UWord64 Delimiter;
	UWord64 Hash;
	UWord64 InformationVector;
} EncryptionHeader;

class FileOutputFlightRecordHigh
{
private:
	static const UWord32 SIZE_OF_FR = 100; //in MBs
	static const UWord32 MB_TO_BYTES = 1048576;
	uint8_t globalSessionId;
	UWord8 decryptionKeyName[32];
	UWord8 keyDigest[SHA256_BLOCK_SIZE];
	uint8_t globalSessionIdLowTracks;
	UWord8 lowFlightRecordAircraftNavigationInformationWritten;
	UWord64 totalFlightRecordSizeLow;
	Word8 countToWriteEncryptionHeader;
	Word8 countToWriteEncryptionHeaderLowTracks;
	Word32 flightRecordCount;
	Word32 flightRecordCountLow;
	UWord32 expandedKey[60];
	time_t randomNumberSeed;
	time_t randomNumberSeedLow;
	UWord32 flightRecordOverflowFlagLow;
	EncryptionHeader encryptionHeader;
	EncryptionHeader encryptionHeaderLow;
	UWord8 aesInitializationVector[16];
	UWord8 aesInitializationVectorLow[16];
	UWord64 usedFlightRecordSizeLow;
	FlightRecordAircraftNavigationInformation flightRecordAircraftNavigationInformation;
	FlightRecordAircraftNavigationInformation flightRecordAircraftNavigationInformationLow;
	FlightRecordHeader flightRecordAricraftNavigationInformationHeader;
	FlightRecordHeader flightRecordAricraftNavigationInformationHeaderLow;
	FlightRecordHeader flightRecordEventHeader;
	SHA256_CTX ctx;
	//void FileOutputFlightRecordHigh();
	void assignRandomNumberToGlobalSessionID();
public:
	UWord8 flightRecordAircraftNavigationInformationWritten;
//	UWord8 mflData[420];
	FlightRecordHeader flightRecordMaintainanceHeader;
	UWord64 usedFlightRecordSize;
	UWord32 flightRecordOverflowFlag;
	UWord64 totalFlightRecordSize;
	UWord32 RTC_TIME;
	FileOutputFlightRecordHigh();
	void writeThreatProcessorReportToFlightRecord(UWord8 numberOfTracks,
			LibraryMetaClass *libraryMeta, string flighRecordPath,
			EmitterLog emitterLogLow, bool &memoryFailure);
	void writeEncryptedThreatProcessorFlightRecord(
			LibraryMetaClass *libraryMeta, string flighRecordPath,
			EmitterLog emitterLogLow, bool &memoryFailure);
	UWord16 calculateCRC(UWord8 *dataPtr, UWord32 size, UWord32 offset);
	void writeEncryptedFlightRecordAircraftNavigationInformationLowEmitters(
			LibraryMetaClass *libraryMeta, string flighRecordPath,
			bool &memoryFailure);
	void encryptAndWriteLowTracksToFile(UWord8 *ptr, UWord32 size,
			LibraryMetaClass *libraryMeta, string flighRecordPath,
			bool &memoryFailure);
	void initializeFlightRecordLowFile(string flighRecordPath,
			bool &memoryFailure);
	void writeTrackingDataFlightRecordToFile(UWord8 numberOfTracks,
			LibraryMetaClass *libraryMeta, string flighRecordPath,
			EmitterLog emitterLog, bool &memoryFailure);
	void writeEncryptedTrackingFlightRecord(LibraryMetaClass *libraryMeta,
			string flighRecordPath, EmitterLog emitterLog, bool &memoryFailure);
	void writeEncryptedFlightRecordAircraftNavigationInformation(
			LibraryMetaClass *libraryMeta, string flighRecordPath,
			bool &memoryFailure);
	void encryptAndWriteToFile(UWord8 *ptr, UWord32 size,
			LibraryMetaClass *libraryMeta, string flighRecordPath,
			bool &memoryFailure);
	void initializeFlightRecordFile(string flighRecordPath,
			bool &memoryFailure);
	void fillMaintenaceFlightRecordHeader();
	void writeEncryptedMaintenanceFlightRecord(UWord8 *mflData,
			LibraryMetaClass *libraryMeta, string flighRecordPath,
			bool &memoryFailure);
	void writeEncryptedEventFlightRecord(UWord16 event, UWord16 size,
			UWord8 *ptrData, LibraryMetaClass *libraryMeta,
			string flighRecordPath, bool &memoryFailure);
	UWord8 checkForSsdOnHardware(void);
	UWord8 checkIfSSDIsMounted(string dev_path);
};

#endif /* FILEOUTPUTFLIGHTRECORDHIGH_H_ */
