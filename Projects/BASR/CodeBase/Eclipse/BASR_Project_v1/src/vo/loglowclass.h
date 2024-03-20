/*
 * loglowclass.h
 *
 *  Created on: Jan 23, 2019
 *      Author: hassan
 */

#ifndef LOGLOWCLASS_H_
#define LOGLOWCLASS_H_

#include "../common/common.h"
#include "../utility/constant.h"
#include "pdwdefaultclass.h"
#include "../utility/utility.h"
#include "../vo/trackdefaultclass.h"

class LogLowClass
{
public:

	//clustering
	static void logClusterSize();
	static void logClusteringBenchmark(double start, double end);
	static void logClusterNewCount(uint16_t clusterCount);
	static void logClusterInput(double paramA, double paramB, double paramC,
			double paramD);
	static void logClusterInputSize(uint64_t clusterInputSize);

	//pri analysis
	static void logPriAnalysisBenchmark(double start, double end);

	//main simulation and live iteration
	static void logThreadMainIterInitCount(uint64_t toaStartFront,
			uint64_t toaStartBack, uint16_t size);
	static void logThreadMainIterCount(uint64_t threadMainIterationCount);
	static void logThreadMainIteration();
	static void logThreadMainIterPenalty(uint64_t timeSinceLastIteration);
	static void logThreadMainIterSimTime(uint64_t timeSinceLastIteration,
			uint64_t timeToWait, uint64_t simTimeMsIterationActual);
	static void logThreadMainIterSimInput(uint64_t lowerTime,
			uint64_t upperTime, uint64_t lowerActualTime,
			uint64_t upperActualTime, uint64_t actualTimeDiff);
	static void logThreadMainIterSimRemain(uint64_t pdwListSize,
			uint32_t bandLowPdwSize, uint32_t bandHighPdwSize,
			uint64_t iterCount);

	//thread complete
	static void logThreadCompleteInputTotalPdw(uint64_t pdwTotal,
			uint64_t threadMainIterationCount);

	//input data limits
	static void logLimitToaStart(uint64_t toaLowerLimit,
			uint64_t timeOfArrivalStart);
	static void logLimitToaEnd(uint64_t toaUpperLimit,
			uint64_t timeOfArrivalStart);
	static void logLimitToaTotal(uint16_t listSize);

	//input data cleanup
	static void logCleanInputPdwStart();
	static void logCleanInputPdwEnd();
	static void logEraseBoundPdw(uint64_t deletionToaLower,
			uint64_t deletionToaUpper);
	static void logEraseInputPdw(uint64_t timeOfArrivalStart);

	//Output data print
	static void writeReportCsv(vector<TrackDefaultClass> &tracks,
			std::string fileName);

	//program startup
	static void logProgramStartup();
	//program termination
	static void logProgramTerminate();
	//utility
	static void logBreakLine();
	static void logn(string input);
	static void logn(string message, uint64_t value);
	LogLowClass();
	virtual ~LogLowClass();
};

struct LogHeaderStruct
{
	uint64_t delim;
	uint16_t messageType;
	uint16_t messageCRC;
	uint32_t messageSize;
	uint64_t systemIteration;
	uint64_t systemTime;
	LogHeaderStruct() :
			delim(0xdeadbeefbeeffeed), messageType(0), messageCRC(0), messageSize(
					0), systemIteration(0)
	{
		systemTime = Utility::getSystemTime();
	}
	int32_t getMessageSize()
	{
		return 32;
	}
	int32_t getAlignedMessageSize()
	{
		return getMessageSize();
	}
};
class LogSystemVariablesClass
{
public:
	LogHeaderStruct header;
	char fileName[256];
	LogSystemVariablesClass()
	{
		for (int i = 0; i < 256; i++)
			fileName[i] = '\0';
	}
	int32_t getMessageSize()
	{
		return header.getMessageSize() + 256;
	}
	int32_t getAlignedMessageSize()
	{
		return getMessageSize();
	}
	ofstream& operator<<(ofstream &fptr)
	{
		header.messageType = 0xAA00;
		header.messageSize = getMessageSize();
		fptr.write((char*) this, header.messageSize);
		return fptr;
	}
	void logToFile(std::string fileName)
	{
		ofstream outputFile;
		outputFile.open(fileName,
				ios::out | ios::app | std::ios::ate | ios::binary);
		long padding = outputFile.tellp();
		padding = ((padding % 32) == 0 ? 0 : 32 - (padding % 32));
		for (; padding > 0; padding -= 4)
			outputFile.write("----", 4);
		operator <<(outputFile);
		outputFile.close();
	}
};
class LogIterationInfoClass
{
public:
	LogHeaderStruct header;
	uint64_t iterationStartToa;
	uint64_t iterationEndToa;
	uint64_t pulseCount;
	uint32_t reportingFlag;
	uint32_t spare;   //size=64 bytes till this point
	LogIterationInfoClass()
	{
		header.messageType = 0xAA11;
		iterationStartToa = iterationEndToa = 0;
		pulseCount = 0;
		reportingFlag=0;
		spare = 0xAAAAAAAA;
	}
	int32_t getMessageSize()
	{
		int32_t sz = header.getMessageSize();
		sz += 32;
		return sz;
	}
	int32_t getAlignedMessageSize()
	{
		int32_t size = getMessageSize();
		size = (size % 32 == 0 ? size : size + 32 - (size % 32));
		return size;
	}
	ifstream& operator>>(ifstream &fptr)
	{

		fptr.read((char*) this, 64);
		return fptr;
	}
	ofstream& operator<<(ofstream &fptr)
	{
		header.messageSize = getMessageSize();

		fptr.write((char*) this, 64);

//		uint64_t padd[] =
//		{ 0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA,
//				0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA,
//				0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA, };
//		int paddingCount = 64 - (getMessageSize()) % 64;
//		if (paddingCount != 64)
//		{
//			fptr.write((char*) padd, paddingCount);
//		}
		return fptr;
	}
	void logToFile(std::string fileName)
	{
		ofstream outputFile;
		outputFile.open(fileName,
				ios::out | ios::app | std::ios::ate | ios::binary);
		long padding = outputFile.tellp();
		padding = ((padding % 32) == 0 ? 0 : 32 - (padding % 32));
		for (; padding > 0; padding -= 4)
			outputFile.write("----", 4);

		operator <<(outputFile);
		outputFile.close();
	}
};

class LogIterationInputPulseClass
{
public:
	LogHeaderStruct header;
	uint64_t timeBenchmark;
	uint32_t pulseCount;
	uint32_t spare[5];   //size=64 bytes till this point
	vector<uint64_t> toas;
	LogIterationInputPulseClass()
	{
		header.messageType = 0xAAA0;
		timeBenchmark = 0;
		pulseCount = 0;
		for (int i = 0; i < 5; i++)
			spare[i] = 0xAAAAAAAA;
	}
	int32_t getMessageSize()
	{
		int32_t sz = header.getMessageSize();
		sz += 32;
		sz += (pulseCount * 8);
		return sz;
	}
	int32_t getAlignedMessageSize()
	{
		int32_t size = getMessageSize();
		size = (size % 32 == 0 ? size : size + 32 - (size % 32));
		return size;
	}
	ofstream& operator<<(ofstream &fptr)
	{
		pulseCount = toas.size();
		header.messageSize = getMessageSize();

		fptr.write((char*) this, 64);

		for (uint32_t i = 0; i < toas.size(); i++)
		{
			fptr.write((char*) &toas[i], 8);
		}
		uint64_t padd[] =
		{ 0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA,
				0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA,
				0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA, };
		int paddingCount = 64 - (getMessageSize()) % 64;
		if (paddingCount != 64)
		{
			fptr.write((char*) padd, paddingCount);
		}
		return fptr;
	}
	void logToFile(std::string fileName)
	{
		ofstream outputFile;
		outputFile.open(fileName,
				ios::out | ios::app | std::ios::ate | ios::binary);
		long padding = outputFile.tellp();
		padding = ((padding % 32) == 0 ? 0 : 32 - (padding % 32));
		for (; padding > 0; padding -= 4)
			outputFile.write("----", 4);

		operator <<(outputFile);
		outputFile.close();
	}
};

//class IterationTrackedPulsesLog
//{
//public:
//	LoggingHeader header;
//	uint64_t timeBenchmark;
//	uint32_t pulseCount;
//	uint32_t spare[5];   //size=64 bytes till this point
//	vector<uint64_t> toas;
//	vector<uint64_t> trackID;
//	IterationTrackedPulsesLog()
//	{
//		header.messageType = 0xAAB3;
//		timeBenchmark = 0;
//		pulseCount = 0;
//		for (int i = 0; i < 5; i++)
//			spare[i] = 0xAAAAAAAA;
//	}
//	int32_t getMessageSize()
//	{
//		int32_t sz = header.getMessageSize();
//		sz += 32;
//		sz += (pulseCount * 16);
//		return sz;
//	}
//	int32_t getAlignedMessageSize()
//	{
//		return getMessageSize();
//	}
//	ofstream & operator<<(ofstream & fptr)
//	{
//		pulseCount = toas.size();
//		header.messageSize = getMessageSize();
//
//		fptr.write((char*) this, 64);
//		for (uint32_t i = 0; i < toas.size(); i++)
//		{
//			fptr.write((char*) &toas[i], 8);
//			fptr.write((char*) &trackID[i], 8);
//		}
//		return fptr;
//	}
//	void logToFile(std::string fileName)
//	{
//		ofstream outputFile;
//		outputFile.open(fileName,
//				ios::out | ios::app | std::ios::ate | ios::binary);
//		long padding = outputFile.tellp();
//padding = ((padding %64)==0?0:64-(padding%64));
//for(;padding>0;padding-=4)
//	outputFile.write("----",4);
//
//		operator <<(outputFile);
//		outputFile.close();
//	}
//};
class LogTrackGeneralDataClass
{
public:
	uint32_t delim;
	uint16_t messageType;
	uint16_t spare;
	uint64_t trackID;
	uint16_t avgPa, stdPa, avgPb, stdPb, avgPc, stdPc, avgPd, stdPd;
	uint16_t avgAngle, stdAngle, avgPw, stdPw;
	uint16_t avgRf, stdRf;
	uint8_t trackingActiveStatus;
	uint8_t spare2[19];
	LogTrackGeneralDataClass()
	{
		trackID = 0;
		delim = 0xdeadbeef;
		messageType = 0xAAB1;
		spare = 0xAAAA;
		avgPa = avgPb = avgPc = avgPd = stdPa = stdPb = stdPc = stdPd = 0;
		avgAngle = stdAngle = avgPw = stdPw = 0;
		avgRf = stdRf = 0;
		trackingActiveStatus = 0;
		for (uint8_t i = 0; i < sizeof(spare2); i++)
			spare2[i] = 0xAA;
	}
	int32_t getMessageSize()
	{
		return sizeof(LogTrackGeneralDataClass);
	}
	int32_t getAlignedMessageSize()
	{
		return getMessageSize();
	}
	ofstream& operator<<(ofstream &fptr)
	{
		fptr.write((char*) this, 64);
		return fptr;
	}
};
struct LogTrackedPulseInfoStruct
{
	uint64_t toa;
	int16_t trackingPriScore;
	int16_t trackingRfScore;
	int16_t trackingPwScore;
	int16_t trackingPowerScore;   // needs to be atleast 8byte aligned
};
class LogTrackPulseToaClass
{
public:
	uint32_t delim;
	uint16_t messageType;
	uint16_t pulseCount;
	uint64_t trackID;
	vector<LogTrackedPulseInfoStruct> trackedPulses;

	LogTrackPulseToaClass()
	{
		delim = 0xdeadbeef;
		messageType = 0xAAB2;
		pulseCount = 0;
		trackID = 0;
	}
	int32_t getMessageSize()
	{
		pulseCount = trackedPulses.size();
		return 16 + (pulseCount * sizeof(LogTrackedPulseInfoStruct));
	}
	int32_t getAlignedMessageSize()
	{
		int32_t size = getMessageSize();
		size = (size % 32 == 0 ? size : size + 32 - (size % 32));
		return size;
	}
	ofstream& operator<<(ofstream &fptr)
	{
		pulseCount = trackedPulses.size();
		fptr.write((char*) this, 16);
		for (uint16_t i = 0; i < pulseCount; i++)
		{
			fptr.write((char*) &trackedPulses[i],
					sizeof(LogTrackedPulseInfoStruct));
		}
		uint64_t padd[] =
		{ 0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA, 0xAAAAAAAAAAAAAAAA,
				0xAAAAAAAAAAAAAAAA };
		int paddingCount = 32 - (getMessageSize() % 32);
		if (paddingCount != 32)
		{
			fptr.write((char*) padd, paddingCount);
		}
		return fptr;
	}
};

struct LogTrackPriSpotStruct
{
	uint32_t delim;
	uint16_t messageType;
	uint16_t priType;
	uint64_t trackID;
	uint32_t priSpotsCount;
	uint32_t pulseCount;
	uint64_t timeBenchmark;
	int32_t priSpots[32];
	LogTrackPriSpotStruct() :
			delim(0xdeadbeef), messageType(0xAAB3), priType(-1), trackID(0), priSpotsCount(
					0), pulseCount(0), timeBenchmark(0)
	{
		for (uint16_t i = 0; i < 32; i++)
		{
			priSpots[i] = 0xAAAAAAAA;
		}
	}
	int32_t getMessageSize()
	{
		return 160;
	}
	int32_t getAlignedMessageSize()
	{
		return getMessageSize();
	}
	ofstream& operator<<(ofstream &fptr)
	{
		fptr.write((char*) this, getMessageSize());
		return fptr;
	}
};
class LogTracksPresentClass
{
public:
	LogHeaderStruct header;
	uint64_t timeBenchmark;
	uint32_t spare;
	uint32_t trackCount;
	uint8_t spare2[32];   //size=64 Bytes Till this Point
	vector<LogTrackGeneralDataClass> trackGeneralData;
	vector<LogTrackPulseToaClass> trackToaData;
	vector<LogTrackPriSpotStruct> priData;
	LogTracksPresentClass()
	{
		header.messageType = 0xAAB0;
		timeBenchmark = 0;
		spare = 0xAAAAAAAA;
		trackCount = 0;
		for (uint8_t i = 0; i < sizeof(spare2); i++)
			spare2[i] = 0xAA;
	}
	int32_t getMessageSize()
	{
		int32_t sz = header.getMessageSize();
		sz += 32;
		for (uint16_t i = 0; i < trackCount; i++)
		{
			sz += trackGeneralData[i].getAlignedMessageSize();
			sz += trackToaData[i].getAlignedMessageSize();
			sz += priData[i].getAlignedMessageSize();
		}
		return sz;
	}

	int32_t getAlignedMessageSize()
	{
		return getMessageSize();
	}
	ofstream& operator<<(ofstream &fptr)
	{
		trackCount = trackGeneralData.size();
		header.messageSize = getMessageSize();

		fptr.write((char*) this, 64);

		for (uint32_t i = 0; i < trackCount; i++)
		{
			trackGeneralData[i] << fptr;
			trackToaData[i] << fptr;
			priData[i] << fptr;
		}
		return fptr;
	}
	void logToFile(std::string fileName)
	{
		ofstream outputFile;
		outputFile.open(fileName,
				ios::out | ios::app | std::ios::ate | ios::binary);
		long padding = outputFile.tellp();
		padding = ((padding % 32) == 0 ? 0 : 32 - (padding % 32));
		for (; padding > 0; padding -= 4)
			outputFile.write("----", 4);
		operator <<(outputFile);
		outputFile.close();
	}
};

class LogClusterClass
{
public:
	LogHeaderStruct header;
	uint64_t timeBenchmark;
	uint32_t pulseCount;
	uint32_t spare[5];   //size=64 bytes till this point
	vector<uint64_t> toas;
	vector<int16_t> clusterID;
	LogClusterClass()
	{
		header.messageType = 0xBBB0;
		timeBenchmark = 0;
		pulseCount = 0;
		for (int i = 0; i < 5; i++)
			spare[i] = 0xAAAAAAAA;
	}
	int32_t getMessageSize()
	{
		int32_t sz = header.getMessageSize();
		sz += 32;
		sz += (pulseCount * 16);
		return sz;
	}
	int32_t getAlignedMessageSize()
	{
		return getMessageSize();
	}
	ofstream& operator<<(ofstream &fptr)
	{
		pulseCount = toas.size();
		header.messageSize = getMessageSize();

		fptr.write((char*) this, 64);

		uint16_t spareSpace[] =
		{ 0xAAAA, 0xAAAA, 0xAAAA };
		for (uint32_t i = 0; i < toas.size(); i++)
		{
			fptr.write((char*) &toas[i], 8);
			fptr.write((char*) &clusterID[i], 2);
			fptr.write((char*) spareSpace, 6);
		}
		return fptr;
	}
	void logToFile(std::string fileName)
	{
		ofstream outputFile;
		outputFile.open(fileName,
				ios::out | ios::app | std::ios::ate | ios::binary);
		long padding = outputFile.tellp();
		padding = ((padding % 64) == 0 ? 0 : 64 - (padding % 64));
		for (; padding > 0; padding -= 4)
			outputFile.write("----", 4);
		operator <<(outputFile);
		outputFile.close();
	}
};

class LogSystemBenchmarkClass
{
public:
	LogHeaderStruct header;

	uint64_t iterationStartTime;
	uint64_t iterationEndTime;
	uint64_t checkPointTime[10];
	LogSystemBenchmarkClass()
	{
		header.messageType = 0xCCC0;

		iterationStartTime = iterationEndTime = 0;
		for (uint32_t i = 0; i < sizeof(checkPointTime) / sizeof(uint64_t); i++)
			checkPointTime[i] = 0;
	}
	int32_t getMessageSize()
	{
		return sizeof(LogSystemBenchmarkClass);
	}
	int32_t getAlignedMessageSize()
	{
		int32_t sz = getMessageSize();
		return sz;
	}

	ofstream& operator<<(ofstream &fptr)
	{
		header.messageSize = getMessageSize();
		fptr.write((char*) this, getMessageSize());
		return fptr;
	}
	void logToFile(std::string fileName)
	{
		if (ConstLow::logHexFile == false
				|| ConstLow::logHexFileTimeBenchmarks == false)
			return;
		ofstream outputFile;
		outputFile.open(fileName,
				ios::out | ios::app | std::ios::ate | ios::binary);
		long padding = outputFile.tellp();
		padding = ((padding % 32) == 0 ? 0 : 32 - (padding % 32));
		for (; padding > 0; padding -= 4)
			outputFile.write("----", 4);
		operator <<(outputFile);
		outputFile.close();
	}

};

#endif /* LOGLOWCLASS_H_ */
