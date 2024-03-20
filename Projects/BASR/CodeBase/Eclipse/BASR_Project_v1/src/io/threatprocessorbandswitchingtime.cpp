/*
 * threatprocessorbandswitchingtime.cpp
 *
 *  Created on: Feb 11, 2021
 *      Author: saad
 */

#include "threatprocessorbandswitchingtime.h"

//This function returns CRC of threat processor band switching time data structure
UWord32 ThreatProcessorBandSwitchingTimeClass::calculateCrcOfThreatProcessorAntennaBandSwitchingTime() {
	UWord32 iterator = 0, crc = 0;
	UWord8 *crcPtr;
	crcPtr = (UWord8*) &threatProcessorBandSwitchingTime;
	for (iterator = sizeof(threatProcessorBandSwitchingTime.crc);
			iterator < sizeof(threatProcessorBandSwitchingTime); iterator++) {
		crc += crcPtr[iterator];
	}
	return crc;
}

//This function fills threat processing band switching time structure
//for now this is being filled with dummy values
UWord32 ThreatProcessorBandSwitchingTimeClass::fillThreatProcessorAntennaSwitchingTimeStructure() {
	memset(&threatProcessorBandSwitchingTime, 0,
			sizeof(threatProcessorBandSwitchingTime));
	threatProcessorBandSwitchingTime.cdBandSwitchingTime = 10000000;
	threatProcessorBandSwitchingTime.jBandSwitchingTime = 10000000;
	threatProcessorBandSwitchingTime.iBandSwitchingTime = 10000000;
	threatProcessorBandSwitchingTime.gBandSwitchingTime = 20000000;
	threatProcessorBandSwitchingTime.eBandSwitchingTime = 10000000;

	threatProcessorBandSwitchingTime.kBandSwitchingTime = 10000000;
	threatProcessorBandSwitchingTime.crc =
			calculateCrcOfThreatProcessorAntennaBandSwitchingTime();
	return 1;
}

//This function writes Antenna Switching Time Structure to Threat Processor using PCIe and
//TP self test registers
void ThreatProcessorBandSwitchingTimeClass::writeThreatProcessorAntennaSwitchingTimeStructureToThreatProcessor()
{
	UWord32 *ptr;
	UWord8 iterator = 0, index = 0;
	UWord32 sizeOfStructure = 0;

	sizeOfStructure = sizeof(threatProcessorBandSwitchingTime);
	ptr = (UWord32*) &threatProcessorBandSwitchingTime;
	if (sizeof(threatProcessorBandSwitchingTime) % 4 > 0)
	{
		sizeOfStructure = sizeOfStructure + 1;
	}
	for (iterator = TP_BAND_SWITCHING_BASE_REG;
			iterator < TP_BAND_SWITCHING_BASE_REG + sizeOfStructure; iterator +=
					4)
	{
		Model::pcieThreatProcessor.sendWordToSharedMemoryBuffer(iterator / 4,
				ptr[index]);
		index++;
	}
	st->sendWordTpSelfTest(st->TP_SELFTEST_WRITE_REG,
			TP_BAND_SWITCHING_BASE_COMMAND);
	printf("CAL-CRC: %d\n", threatProcessorBandSwitchingTime.crc);
	printf("Ban Sent Finish\n");
}

//This function is main logic of sending Antenna Switching Time to TP
//It checks if we need to send it, if yes, sending function is called
//called from main loop
void ThreatProcessorBandSwitchingTimeClass::sendAntennaSwitchingTimeStructureToThreatProcessorLogic()
{
	if ((ConstHigh::readingFromLvds == 1)
			&& (ConstHigh::simOnActualHwFlag == 1))
	{
		if (sendBandSwitchDataToTp == 1)
		{
			writeThreatProcessorAntennaSwitchingTimeStructureToThreatProcessor();
			sendBandSwitchDataToTp = 0;
		}
	}
}

//This function just checks whether CRC sent back by TP matches with Antenna Switching
//Time Structures CRC, if it doesn't, it raises a flag that structure needs to be sent
//again
//called from where TP report is processed
void ThreatProcessorBandSwitchingTimeClass::checkWriteThreatProcessorAntennaSwitchingTimeStructureToThreatProcessor(
		UWord32 RSSCRC)
{
	if (RSSCRC != threatProcessorBandSwitchingTime.crc)
	{
		//printf("REC-CRC: %d CAL-CRC: %d\n",pAef->RSSCRC, threatProcessorBandSwitchingTime.crc);
		//printf("Ban Sent\n");
		sendBandSwitchDataToTp = 1;
	}
}
