/*
 * selftestcontroller.cpp
 *
 *  Created on: Jan 29, 2021
 *      Author: saad
 */
#define SELFTESTCONTROLLER_CPP_

#include "selftestcontrollerhigh.h"

//This function is used to initialize TP Self Test Register
void SelfTestControllerHigh::initializeTpSelfTest()
{
	MemoryMapAddressClass::tpSelfTestReg.initialize(
			(UWord8*) MemoryMapAddressClass::TP_SELF_TEST_REGISTER);
}

//This function sends the word to address offset on TP Self Test Register
void SelfTestControllerHigh::sendWordTpSelfTest(UWord32 address, UWord32 word)
{
	MemoryMapAddressClass::tpSelfTestReg.writeDataToReg(address, word);
}

//This function receives the data from address offset on TP Self Test Register
UWord32 SelfTestControllerHigh::recvWordTpSelfTest(UWord32 address)
{
	UWord32 val = MemoryMapAddressClass::tpSelfTestReg.readDataFromReg(address);
	return val;
}

//This function is used to initialize VDM Self Test Register
void SelfTestControllerHigh::initializePpfSelfTest()
{
	MemoryMapAddressClass::ppfSelfTestReg.initialize(
			(UWord8*) MemoryMapAddressClass::PPF_SELF_TEST_REGISTER);
}

//This function sends the word to address offset on VDM Self Test Register
void SelfTestControllerHigh::sendWordPpfSelfTest(UWord32 address, UWord32 word)
{
	MemoryMapAddressClass::ppfSelfTestReg.writeDataToReg(address, word);
}

//This function receives the data from address offset on VDM Self Test Register
UWord32 SelfTestControllerHigh::recvWordPpfSelfTest(UWord32 address)
{
	UWord32 val = MemoryMapAddressClass::ppfSelfTestReg.readDataFromReg(
			address);
	return val;
}

//This function is main logic of Self Test initializations
//First, it initializes TP self test registers and then
//initializes VDM self test registers
UWord8 SelfTestControllerHigh::ppfGetSelfTestStatus(UWord8 mode)
{
	UWord32 timeDifference = 0, status = 0, read = 0;
	uint64_t initTime, currentTime;

	if (mode == 1)
	{
		sendWordPpfSelfTest(SELFTEST_TX_DATA, mode);
	}

	timeDifference = 0;
	initTime = Utility::getSystemTime();
	while (1)
	{
		// check busy/done status
		read = recvWordPpfSelfTest(SELFTEST_STATUS);
		if ((read & SELFTEST_FIFO_EMPTY) == 0)
		{
			status = recvWordPpfSelfTest(SELFTEST_RX_DATA);
		}

		if (status == 1)			// ST BUSY
		{
			return 1;
		}
		else if (status == 2)	// ST DONE
		{
			return 2;
		}

		currentTime = Utility::getSystemTime();
				timeDifference = (currentTime - initTime) / 1000;
		if (timeDifference > 500)
		{
			return 0;
		}
	}
	return 0;
}

UWord16 SelfTestControllerHigh::ppfGetSelfTestResults(UWord32 *stPackets)
{
	UWord16 count = 0;
	UWord32 read = 0;

	read = recvWordPpfSelfTest(SELFTEST_STATUS);
	while ((read & SELFTEST_FIFO_EMPTY) == 0)
	{
		if (count > 999)
		{
			return count;
		}
//		printf("RC: %d\n",readCount);
		stPackets[count] = recvWordPpfSelfTest(SELFTEST_RX_DATA);

		read = recvWordPpfSelfTest(SELFTEST_STATUS);
		count++;
		if (!read)
		{
			usleep(50);
			read = recvWordPpfSelfTest(SELFTEST_STATUS);
		}
	}
	return count;
}

UWord32 SelfTestControllerHigh::tpGetSelfTestStatus()
{
	UWord32 status = 0, command = 0, timeDifference = 0;
	uint64_t initTime, currentTime;

	sendWordTpSelfTest(TP_SELFTEST_WRITE_REG, TP_SELFTEST_START_VALUE);

	timeDifference = 0;
	initTime = Utility::getSystemTime();
	command = 0x33000;
	status = Model::pcieThreatProcessor.recvWordToSharedMemoryBuffer(
			command / 4);

	while (status == 1)
	{
		status = Model::pcieThreatProcessor.recvWordToSharedMemoryBuffer(
				command / 4);

		currentTime = Utility::getSystemTime();
		timeDifference = (currentTime - initTime) / 1000;
		if (timeDifference > 500)
		{
			return 0;
		}
	}

	return status;
}

UWord16 SelfTestControllerHigh::tpGetSelfTestResults(UWord32 *stPackets)
{
	UWord32 command = 0, count = 0;
	command = 0x33000;

	for (auto i = 0; i < ConstHigh::TP_ERROR_COUNT; i++)
	{
		command += 4;
		stPackets[i] = Model::pcieThreatProcessor.recvWordToSharedMemoryBuffer(
				command / 4);
		count++;
	}
	return count;
}

UWord16 SelfTestControllerHigh::rfbGetSelfTestResults(UWord32 *stPackets)
{
	UWord16 count = 0;
	count = Model::difmCsiLink.readCsiData();
	for (auto i = 0; i < count; i++)
	{
		stPackets[i] = Model::difmCsiLink.csiBuffer[i];
	}
//	memcpy(&stPackets, Model::difmCsiLink.csiBuffer, count * sizeof(UWord32));

	return count;
}
