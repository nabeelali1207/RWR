/*
 * pciethreatprocessor.cpp
 *
 *  Created on: Feb 10, 2021
 *      Author: saad
 */

#define PCIETHREATPROCESSOR_CPP_
#include "pciethreatprocessor.h"

//This function reads the size of memory buffer used for PCIe using userspace IO
//In actual this memory size is fixed to 32 MB but if UIO fails or something goes
//south, this get_memory_size function will return 0 hence avoiding the page faults
//or segmentation faults
UWord32 PcieThreatProcessor::get_memory_size(const char *sysfs_path_file)
{
	FILE *size_fp;
	UWord32 size;

	// open the file that describes the memory range size that is based on the
	// reg property of the node in the device tree
	size_fp = fopen(sysfs_path_file, "r");
	if (size_fp == NULL)
	{
		printf("unable to open the uio size file\n");
		exit(-1);
	}
	// get the size which is an ASCII string such as 0xXXXXXXXX and then be stop
	// using the file
	fscanf(size_fp, "0x%08X", &size);
	fclose(size_fp);

	return size;
}

//This function sends the word to address offset on shared memory buffer of PCIe
void PcieThreatProcessor::sendWordToSharedMemoryBuffer(UWord32 address,
		UWord32 word)
{
	MemoryMapAddressClass::sharedMemoryPcieReg.writeDataToRegKernel(address,
			word);
}

//This function receives the data from address offset on shared memory buffer of PCIe
UWord32 PcieThreatProcessor::recvWordToSharedMemoryBuffer(UWord32 address)
{
	UWord32 val;
	MemoryMapAddressClass::sharedMemoryPcieReg.readDataFromRegKernel(address,
			val);
	return val;
}

//This function maps the UIO Shared memory buffer used for PCIe for access
void PcieThreatProcessor::initializeSharedMemoryBuffer()
{
	Word32 gpio_size = get_memory_size("/sys/class/uio/uio0/maps/map0/size");
	MemoryMapAddressClass::sharedMemoryPcieReg.initializeKernelMemory(2,
			gpio_size, sharedMemory);
}

//This function sends the word to address offset on VDM side buffer of PCIe
void PcieThreatProcessor::sendWordVdmPcieReg(UWord32 address, UWord32 word)
{
	MemoryMapAddressClass::vdmPcieReg.writeDataToReg(address, word);
}

//This function receives the data from address offset on VDM side buffer of PCIe
UWord32 PcieThreatProcessor::recvWordVdmPcieReg(UWord32 address)
{
	UWord32 val;
	val = MemoryMapAddressClass::vdmPcieReg.readDataFromReg(address);
	return val;
}

//This function maps the VDM side buffer of PCIe for access
void PcieThreatProcessor::initializeVdmPcieReg()
{
	MemoryMapAddressClass::vdmPcieReg.initialize(
			(UWord8*) MemoryMapAddressClass::VDM_PCIE_REGISTER);
}

//This function is main logic of PCIe initialization between TP and CCP
//Here, we first initialize UIO shared memory buffer and then VDM side buffer
//to communicate. Afterwards we send TP a message that we are up and wait for
//it to start, if it doesn't TP UP is status is turned false
void PcieThreatProcessor::initPcie()
{
	//buff = (UWord32*) malloc(1 << 18);

	Word32 status = 0;
	uint64_t timeDifference = 0;
	UWord8 tpSuccess = 0;
	if ((ConstHigh::readingFromLvds == 1)
			&& (ConstHigh::simOnActualHwFlag == 1))
	{
		ThreatProcessorReportTimeout = Utility::getSystemTime();
//		for (; buffercount < 65536; buffercount++)
//		{
//			buff[buffercount] = 0;
//		}
		initializeSharedMemoryBuffer();
		initializeVdmPcieReg();
		printf("PCIE:\tSetting the address \n");
		//printf("\t 36 : 3fc00000 \n");
		sendWordVdmPcieReg(36, 0x3fc00000);
		//returnStatus=_getWordSlave(4);
		//returnStatus= returnStatus||4;
		//printf("\t 4 : \t 4 \n");

		status = recvWordVdmPcieReg(4);
		//printf("PCIE:\tStatus Register value: 0x%.08x \n",status);
		status = status | 4;
		sendWordVdmPcieReg(4, status);
		//printf("PCIE:\tStatus Register value after writing: 0x%.08x \n",_getWordSlave(4));
		printf("PCIE:\tCCP is ready \n");
		//printf("PCIE:\tWord at 36 offset is: 0x%.08x \n",_getWordSlave(36));
		//printf("Waiting for TP \n");
		status = recvWordVdmPcieReg(4);
		printf("PCIE:\tWaiting for TP \n");

		currentTime = Utility::getSystemTime();

		tpSuccess = 1;
		while (status != 6)
		{
			//printf("At reg: %d \n",_getWordSlave(4));
			//printf("Waiting for TP \n");
			status = recvWordVdmPcieReg(4);
			//status = status & 2 ;
			currentTime = Utility::getSystemTime();
			timeDifference = (currentTime -ThreatProcessorReportTimeout)
							/ 1000;
			if (timeDifference > 200)
			{
				tpSuccess = 0;
				break;
			}
		}
		if (tpSuccess == 1)
		{
			printf("PCIE:\tConnected with TP \n");
			printf("PCIE:\t Init Success \n");
			tpUp = 1;
		}
		else
		{
			printf("PCIE:\tNot connected with TP \n");
		}
		filecount = 0;
	}
}

//This report checks if TP is up and it has sent a new report
//If so, report is extracted from PCIe shared memory buffer
bool PcieThreatProcessor::getThreatProcessorReport(uint8_t *buff)
{
	bool reportReceived = false;
	//UWord32 i = 0;
	UWord8 reportFlag = 0;
//	UL32 regread;
	uint64_t timeDifference = 0;
	UWord32 size = 208893;

	if ((ConstHigh::readingFromLvds == 1)
			&& (ConstHigh::simOnActualHwFlag == 1))
	{
		if (tpUp == 1)
		{
			currentTime = Utility::getSystemTime();
			timeDifference = (currentTime -ThreatProcessorReportTimeout)
							/ 1000;
//			printf("time Difference  %lu \n \n",timeDifference);
			while (timeDifference < 10)
			{
				usleep(1000);
				if (recvWordToSharedMemoryBuffer(8 / 4) > 0)
				{
//					printf("TP-Rec\n");
					reportFlag = 1;
//					break;
				}
				currentTime = Utility::getSystemTime();
				timeDifference = (currentTime -ThreatProcessorReportTimeout)
								/ 1000;
			}
			ThreatProcessorReportTimeout = Utility::getSystemTime();

			if (reportFlag == 1)
			{
				//printf("Waiting for Data Write \n");

				sendWordToSharedMemoryBuffer(8 / 4, 0x00000000);
				//printf("Data Write by TP Complete \nReading data from DDR \n");

				//memset(buff, 0, sizeof(buff));

				UWord8 *buffer;
				buffer = (UWord8*) buff;
				MemoryMapAddressClass::sharedMemoryPcieReg.blockReadWordByWordKernel(
						4096, buffer, size - 4096);
//				for (i = 4096; i < size; i = i + 4)
//				{
//					returnMemory = recvWordToSharedMemoryBuffer(i);
//					buff[index] = returnMemory;
//					index++;
//				}

				/*
				 FILE *ourFile=fopen("DSP_Report.dsp","a");
				 fwrite(buff,204800,1,ourFile);
				 fclose(ourFile);
				 */

				filecount++;

				//printf("FN: %d\n", filecount);

				//printf("Report read & memcpy success. Offset 2 set to 0 \n");

//				regread = recvWordToSharedMemoryBuffer(8 / 4);
				//printf("Register value after clear %lu \n \n",regread);
			}
		}
	}
	reportReceived = (reportFlag == 1);
	return reportReceived;
}

//VDM sends a zeroize signal in its buffer, if it is received, all the Keys
//present in CF Card and emmc are overwritten by Zeroes (0)
void PcieThreatProcessor::checkZeroizeSignal(CfCard &cfCard)
{
	Word32 status = 0;

	status = recvWordVdmPcieReg(0);
	if (status >> 5 == 0x01)
	{
		printf("Zero\n");
		cfCard.zeroizeKeys();
	}
}

//It checks if TP is up during normal operations
void PcieThreatProcessor::checkTpUp(UWord8 &sendBandSwitchDataToTp)
{
	Word32 status = 0;
	if ((ConstHigh::readingFromLvds == 1)
			&& (ConstHigh::simOnActualHwFlag == 1))
	{
		if (tpUp == 0)
		{
			status = recvWordVdmPcieReg(4);
			if (status == 6)
			{
				tpUp = 1;
				printf("Ban Sent\n");
				sendBandSwitchDataToTp = 1;
				printf("TP-UP\n");
			}
		}
	}
}
