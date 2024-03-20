/*
 * serialdatahandler.cpp
 *
 *  Created on: Oct 14, 2019
 *      Author: gmd
 */

#include "serialdatahandler.h"

SerialDataHandlerLow::SerialDataHandlerLow()
{
	runningOnHardware = false;
	writeDspReportToFile = false;
}
void SerialDataHandlerLow::initializeIoHandler(bool writeToFile,
		bool isRunningOnHardware)
{
	runningOnHardware = isRunningOnHardware;
	writeDspReportToFile = writeToFile;

	if (writeDspReportToFile == true)
	{
		dataFlushFile.openFile(ConstLow::dspReportFileName);
	}
	if (runningOnHardware == true)
	{
		pcieLinkObj.initialize();
	}
}
bool SerialDataHandlerLow::sendMessageToCPU(uint8_t * dataPtr,
		uint64_t messageSize)
{
	bool status = true;
	if (writeDspReportToFile == true)
	{
		status = dataFlushFile.writeDelimitedDataToDisk(dataPtr, messageSize)
				&& status;
	}
	if (runningOnHardware == true)
	{
		status = pcieLinkObj.sendDspReport(dataPtr, messageSize) && status;

	}
	return status;
}

SerialDataHandlerHigh::SerialDataHandlerHigh()
{
	runningOnHardware = false;
	readDspReportFromFile = false;
	dumpDspReportFlag=false;
}
void SerialDataHandlerHigh::initializeIoHandler(bool dumpDspRep,
		bool isRunningOnHardware)
{
	dumpDspReportFlag=dumpDspRep;
	runningOnHardware = isRunningOnHardware;
	if (runningOnHardware == true)
	{

		runningOnHardware=true;
		readDspReportFromFile = false;
		if (dumpDspReportFlag)
		{
			dspReportDumpFile.openFile(ConstHigh::recDataFromDspDumpFileNameCurrent);
		}
	}
	else
	{
		runningOnHardware=false;
		readDspReportFromFile = true;
		fileForDspReports.openFile(ConstHigh::dspReportFileNameCurrent);
	}
}
uint64_t SerialDataHandlerHigh::recMessageFromTP(uint8_t * dataPtr,
		uint64_t maxBufferSize)
{
	uint64_t bytesRead = 0;
//	uint64_t strtTime = Utility::getSystemTime();

	if (runningOnHardware == true)
	{
		if(bytesRead>0 && dumpDspReportFlag)
		{
			dspReportDumpFile.writeDelimitedDataToDisk(dataPtr, bytesRead);
		}
	}
	else if (readDspReportFromFile == true)
	{
		bytesRead = fileForDspReports.readDelimitedDataFromDisk(dataPtr,
				maxBufferSize);
	}
	return bytesRead;
}
