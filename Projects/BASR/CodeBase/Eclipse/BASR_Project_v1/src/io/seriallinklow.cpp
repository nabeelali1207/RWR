/*
 * seriallinklow.cpp
 *
 *  Created on: Oct 16, 2019
 *      Author: gmd
 */

#include "seriallinklow.h"

SerialLinkLowToHigh::SerialLinkLowToHigh()
{
	ccpUpFlag = false;
	localBuffer = NULL;
	lastCcpUpTime = 0;
}

void SerialLinkLowToHigh::initialize()
{
	MemoryMapAddressClass::kernelLowCcpStatus.initializeKernelMemory(2,
			DATA_BUFFER_SIZE, procNameCcpStatus);
//	MemoryMapAddressClass::kernelPdwCcpStatus.initializeFileDescriptor(
//			procNameCcpStatus);

	MemoryMapAddressClass::kernelLowDspReport.initializeKernelMemory(2,
			DSP_REPORT_SIZE, procNameDspReport);
//	MemoryMapAddressClass::kernelPdwDspReport.initializeFileDescriptor(
//			procNameDspReportStatus);

	MemoryMapAddressClass::kernelLowDspReportControl.initializeKernelMemory(2,
			DATA_BUFFER_SIZE, procNameDspReportControl);

	MemoryMapAddressClass::kernelLowDspReportStatus.initializeKernelMemory(2,
			DATA_BUFFER_SIZE, procNameDspReportStatus);
//	MemoryMapAddressClass::kernelPdwDspReportStatus.initializeFileDescriptor(
//			procNameDspReportStatus);

	localBuffer = (UWord8*) malloc(204800);

	waitForCcpUp();
}

void SerialLinkLowToHigh::waitForCcpUp()
{
	UWord32 ccpUp = 0;
	while(ccpUp != 6)
	{
		sprintf(tempBuffer, "%ld", 1L);
		MemoryMapAddressClass::kernelLowCcpStatus.openThenWriteThenClose(procNameCcpStatus, tempBuffer);

		//printf("CCP not UP\n");
		ccpUp = 0;
		MemoryMapAddressClass::kernelLowCcpStatus.readDataFromRegKernel(0, ccpUp);
	}
	ccpUpFlag=true;
	printf("CCP UP on Both Sides\n");
}

void SerialLinkLowToHigh::checkCcpUp()
{
	UWord32 ccpUp = 0;
	uint64_t currSystemTime = Utility::getSystemTime();
	uint64_t timeSinceCcpFail = 0;

	sprintf(tempBuffer, "%ld", 1L);
	MemoryMapAddressClass::kernelLowCcpStatus.openThenWriteThenClose(procNameCcpStatus, tempBuffer);
	MemoryMapAddressClass::kernelLowCcpStatus.readDataFromRegKernel(0,
					ccpUp);

	if((ccpUpFlag == true) && (ccpUp != 6))
	{
		timeSinceCcpFail = currSystemTime - lastCcpUpTime;
		if(timeSinceCcpFail > 3000 * 1000)
		{
			// reboot TP here
			system("reboot");
		}
	}
	else
	{
		lastCcpUpTime = Utility::getSystemTime();
	}
}

void SerialLinkLowToHigh::checkKernelPciMod()
{
	FILE *fp;
	Word8 commandResult[1035];
	Word8 dummyCommand[150] =
	{ ' ' };
	UWord8 pciModExists = 0;

	fflush(NULL);
	sprintf(dummyCommand, "lsmod | grep pci > pci.txt");
	system(dummyCommand);

	fp = fopen("pci.txt", "r");
	if (fp != NULL)
	{
		fread(commandResult, 10, 1, fp);
		if((commandResult[0] == 'p') && (commandResult[1] == 'c'))
		{
			pciModExists = 1;
		}
		pclose(fp);
	}

	if(pciModExists == 0)
	{
		sprintf(dummyCommand, "insmod /home/root/pci_mod_PBv8.ko > pci.txt");
		system(dummyCommand);
		printf("PCIe: Insmod Done\n");
	}
}

bool SerialLinkLowToHigh::sendDspReport(uint8_t * dataPtr, uint64_t messageSize)
{
	UWord8 transactions = 0;
	UWord32 reportFlag = 1, reportControlFlag = 1;
	UWord32 done = 0;
	bool reportSent;

	checkCcpUp();
	transactions = (messageSize) / 8192;
	if ((messageSize) % 8192 != 0)
	{
		transactions = transactions + 1;
	}

	//printf("No of Transaction = %d\n", transactions);
	//printf("DSP Report available\n");
	memcpy(localBuffer, dataPtr, messageSize);
	//printf("Copied Report\n");

	sprintf(tempBuffer, "%ld", 1L);
//	MemoryMapAddressClass::kernelPdwDspReportStatus.writeToFile(tempBuffer);
	MemoryMapAddressClass::kernelLowDspReportStatus.openThenWriteThenClose(
			procNameDspReportStatus, tempBuffer);

	usleep(5);
	MemoryMapAddressClass::kernelLowDspReportStatus.readDataFromRegKernel(0,
			reportFlag);

	if (reportFlag == 0)
	{
		for (int i = 0; i < transactions;)
		{
			if (reportControlFlag == 1)
			{
				reportControlFlag = 0;
				MemoryMapAddressClass::kernelLowDspReport.blockWriteWordByWordKernel(
						0, localBuffer + (8192 * i), 8192);

				if ((i + 1 == transactions) || (i == 24))
				{
					done = 1;
					MemoryMapAddressClass::kernelLowDspReportControl.writeDataToRegKernel(
							4, done);
				}
				else
				{
					done = 0;
					MemoryMapAddressClass::kernelLowDspReportControl.writeDataToRegKernel(
							4, done);
				}

				sprintf(tempBuffer, "%ld", (long) i);
//				MemoryMapAddressClass::kernelPdwDspReport.writeToFile(
//						tempBuffer);

				MemoryMapAddressClass::kernelLowDspReport.openThenWriteThenClose(
						procNameDspReport, tempBuffer);
				usleep(5);
				i++;
			}
			else
			{
				MemoryMapAddressClass::kernelLowDspReportControl.readDataFromRegKernel(
						1, reportControlFlag);
			}
		}
		transactions = 0;
		reportFlag = 0;
		done = 0;
		reportSent=true;
	}
	else
	{
		reportSent=false;
		printf("CCP has not served previous Report, discarding \n");
	}

	return reportSent;
}
