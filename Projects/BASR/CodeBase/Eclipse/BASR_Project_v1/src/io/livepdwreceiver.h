/*
 * livepdwreceiver.h
 *
 *  Created on: Nov 5, 2019
 *      Author: gmd
 */

#ifndef LIVEPDWRECEIVER_H_
#define LIVEPDWRECEIVER_H_

#include <iostream>
#include "../io/memorymap.h"
#include "../io/fileinputoutputdelimited.h"
#include "../vo/pdwdefaultclass.h"
#include "../utility/utility.h"
#include <boost/lockfree/spsc_queue.hpp>

struct benchmarksWriterThread
{
	uint64_t memcpyBytes;
	uint64_t memcpyTime;
	uint64_t memcpyItr;

	uint64_t parseTime;
	uint64_t bufferInsertTime;
};

class LivePdwReceiver
{
	constexpr static const char* procNameBuffer1 = "/proc/PCI_PICOZED_Param";
	constexpr static const char* procNameBuffer2 = "/proc/PCI_PICOZED_Param2";
	constexpr static const char* procNameBuffer3 = "/proc/PCI_PICOZED_Param3";
	constexpr static const char* procNameBuffer4 = "/proc/PCI_PICOZED_Param4";
	constexpr static const char* procNamePollVector = "/proc/PCI_PICOZED_Paramv";

	static const uint16_t PDW_BUFFER_SIZE = 16384;
	static const uint16_t DATA_BUFFER_SIZE = 4096;
	static const uint16_t MAX_PDWS = 512;
	static const uint16_t PDW_SIZE = 32;

	static const uint16_t SPSC_CAPACITY=3000;
	FileOutputDelimitedClass dataDumpFile;
	FileOutputDelimitedClass dataDumpFile2;
	FileOutputDelimitedClass dataDumpFile3;
	FileOutputDelimitedClass dataDumpFile4;
	vector<PdwSmall32> pdwsSinceLastRead;
	vector<PdwSmall32> filteredPdwsFromBuffer;
	uint64_t lastPrintoutTime;
	uint64_t pdwsReceivedSinceLastPrint;
	uint64_t pdwsDiscardedSinceLastPrint;
	uint64_t invalidpdwsSinceLastPrint;
	uint64_t pdwsProcessedSinceLastPrint;
	int64_t toaOffsetFromBootTime;

	uint64_t timeOfLastBufferCorrection;
	//DataLivePdwBuffersClass threadSafePdwReceiver;
	boost::lockfree::spsc_queue<PdwSmall32, boost::lockfree::capacity<SPSC_CAPACITY> > spsc_queue;

	bool recordPdws;

	bool vdmError, dataPresent[4];
	UWord64 dataAvailableStatus, pdwCount[4];

	UWord8 lastValidBuffer;
	UWord8* localBuffer;
	UWord64 localBufferSize;
//	MemoryMapAddressClass *currentReadTarget;
	void KernelPdwReader();
	void extractPdwsFromBuffer(UWord8 * buffer, UWord32 size);
	void copyDataFromKernelBufferToLocalBuffer(UWord8 numBuffer);
	bool KernelBufferOrderValidChecker(UWord8 numBuffer);
	bool KernelBufferDataValidChecker(UWord8 numBuffer);
	void resetPdwBufferCounts(UWord64 invalidBufferMask);

	uint8_t checkpdwValidity(PdwSmall32 & pdw);

	void printInfo();
	void resetLastBuffer();
public:

	uint64_t memcpyBytes;
	uint64_t memcpyItrs;
	uint64_t memcpyTimeBenchmark;
	uint64_t parseTimeBenchmark;
	uint64_t enqueTimeBenchmark;
	uint64_t pdwsProcessed;

	void initialize(bool recordingEnabled, std::string fileName);
	void readAvailableData();
	vector<PdwSmall32> & getReceivedPdws();

	vector<PdwSmall32> tempPdwsssss;
};

#endif /* LIVEPDWRECEIVER_H_ */
