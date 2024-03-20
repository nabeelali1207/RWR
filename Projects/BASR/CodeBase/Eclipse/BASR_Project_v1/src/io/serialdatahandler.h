/*
 * serialdatahandler.h
 *
 *  Created on: Oct 9, 2019
 *      Author: gmd
 */

#ifndef SERIALDATAHANDLER_H_
#define SERIALDATAHANDLER_H_
#include <mutex>
#include "../utility/constlow.h"
#include "../utility/consthigh.h"
#include "../utility/utility.h"
#include "../io/pciethreatprocessor.h"
#include "../io/fileinputoutputdelimited.h"
#include "../io/seriallinklow.h"

class SerialDataHandlerLow
{
private:
	bool writeDspReportToFile;
	bool runningOnHardware;
	FileOutputDelimitedClass dataFlushFile;
	SerialLinkLowToHigh pcieLinkObj;

	std::mutex mutexLock;
public:
	SerialDataHandlerLow();
	void initializeIoHandler(bool writeToFile, bool isRunningOnHardware);
	bool sendMessageToCPU(uint8_t *dataPtr, uint64_t messageSize);
};

class SerialDataHandlerHigh
{
private:
	bool readDspReportFromFile;
	bool runningOnHardware;
	bool dumpDspReportFlag;
	FileInputDelimitedClass fileForDspReports;
	FileOutputDelimitedClass dspReportDumpFile;
public:

	SerialDataHandlerHigh();
	void initializeIoHandler(bool readFromFile, bool isRunningOnHardware);
	uint64_t recMessageFromTP(uint8_t *dataPtr, uint64_t maxBufferSize);
};

#endif /* SERIALDATAHANDLER_H_ */
