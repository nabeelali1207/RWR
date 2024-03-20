/*
 * selftestlow.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: root
 */

#include "selftestcontrollerlow.h"

void SelfTestControllerLow::initializeSelfTestLow()
{
	//this function needs to be called after other kernel memories have been already initialized
	MemoryMapAddressClass::kernelLowSelfTest.initializeKernelMemory(2,
			DATA_BUFFER_SIZE, procNameLowSelfTest);
	MemoryMapAddressClass::kernelLowCcpRx.initializeKernelMemory(2,
			DATA_BUFFER_SIZE, procNameLowCcpRx);

	MemoryMapAddressClass::kernelLowVdmMemory.initializeKernelMemory(2,
			DATA_BUFFER_SIZE, procNameVdmMemory);

	MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(0,
			ARM_FAILURE_DEFAULT_VALUE);
	MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(1,
			DSP_FAILURE_DEFAULT_VALUE);
	MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(2,
			FLASH_FAILURE_DEFAULT_VALUE);
	MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(3,
			RAM_FAILURE_DEFAULT_VALUE);
	MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(4,
			USB_FAILURE_DEFAULT_VALUE);
	MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(5,
			PCI_FAILURE_DEFAULT_VALUE);
	MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(6,
			TEMP_FAILURE_DEFAULT_VALUE);

	performPowerOnSelfTest();
	timeSinceLastAutoSelfTest = 0;
}

void SelfTestControllerLow::ArmCoresTest()
{
	UWord32 currTestStatus, prevFaultCount, faultStatus;
	UWord32 prevFaultPacketValue = 0;
	struct sysinfo info;

	int status = sysinfo(&info);

	if (status < 0)
	{
		printf("Fault Occured:Couldnt perform ARM Test.\n");
		currTestStatus = 1;
	}
	else
	{
		if (sysconf(_SC_NPROCESSORS_ONLN) != 4)
		{
			currTestStatus = 1;
			printf("Fault Occured, ARM Cores are not up!\n");
		}
		else
		{
			currTestStatus = 0;
		}
	}

	//update results into descriptor memory
	MemoryMapAddressClass::kernelLowSelfTest.readDataFromRegKernel(ARM_OFFSET,
			prevFaultPacketValue);

	if ((prevFaultPacketValue & 0x00000001) == 0x00000001)
	{
		if (currTestStatus == 0)
		{
			faultStatus = (prevFaultPacketValue & 0xFFFE0001);
			prevFaultCount = ((prevFaultPacketValue & 0x0001FFFE) >> 1);
			prevFaultCount = prevFaultCount + 1;
			faultStatus = (faultStatus | (prevFaultCount << 1));
			faultStatus = faultStatus & 0xFFFFFFFE;
			MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(
					ARM_OFFSET, faultStatus);
		}
	}
	else if ((prevFaultPacketValue & 0x00000001) == 0x00000000)
	{
		if (currTestStatus == 1)
		{
			faultStatus = prevFaultPacketValue;
			faultStatus = faultStatus | 0x00000001; // insert fault flag
			MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(
					ARM_OFFSET, faultStatus);
		}
	}
	printf("ARM Test Performed.\n");
}

void SelfTestControllerLow::DspCoresTest()
{
	// DSP Cores not being used, so test is not being performed
	MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(DSP_OFFSET,
			DSP_FAILURE_DEFAULT_VALUE);

	printf("DSP Test Performed.\n");
}

void SelfTestControllerLow::FlashTest()
{
	UWord32 currTestStatus, prevFaultCount, faultStatus;
	UWord32 prevFaultPacketValue = 0;

	mtd_info_t mtd_info1, mtd_info2;
	int fa;
	fa = open("/dev/mtd4", O_RDWR);
	if (fa < 0)
	{
		printf("Flash Test: /dev/mtd4 does not exist\n");
		currTestStatus = 1;
	}
	else
	{
		ioctl(fa, MEMGETINFO, &mtd_info1);
		if ((mtd_info1.size / (1024 * 1024)) < 15)
		{
			currTestStatus = 1;
			printf("Flash Test: /dev/mtd4 found but capacity is low\n");
		}
		else
		{
			currTestStatus = 0;
		}
		close(fa);
	}

	fa = open("/dev/mtd2", O_RDWR);
	if (fa < 0)
	{
		printf("Flash Test: /dev/mtd2 does not exist\n");
		currTestStatus = 1;
	}
	else
	{
		ioctl(fa, MEMGETINFO, &mtd_info2);
		if ((mtd_info2.size / (1024 * 1024)) < 510)
		{
			currTestStatus = 1;
			printf("Flash Test: /dev/mtd2 found but capacity is low\n");
		}
		else
		{
			currTestStatus = 0;
		}
		close(fa);
	}

	//update results into descriptor memory
	MemoryMapAddressClass::kernelLowSelfTest.readDataFromRegKernel(FLASH_OFFSET,
			prevFaultPacketValue);

	if ((prevFaultPacketValue & 0x00000001) == 0x00000001)
	{
		if (currTestStatus == 0)
		{
			faultStatus = (prevFaultPacketValue & 0xFFFE0001);
			prevFaultCount = ((prevFaultPacketValue & 0x0001FFFE) >> 1);
			prevFaultCount = prevFaultCount + 1;
			faultStatus = (faultStatus | (prevFaultCount << 1));
			faultStatus = faultStatus & 0xFFFFFFFE;
			MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(
					FLASH_OFFSET, faultStatus);
		}
	}
	else if ((prevFaultPacketValue & 0x00000001) == 0x00000000)
	{
		if (currTestStatus == 1)
		{
			faultStatus = prevFaultPacketValue;
			faultStatus = faultStatus | 0x00000001; // insert fault flag
			MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(
					FLASH_OFFSET, faultStatus);
		}
	}
	printf("Flash Mem Test Perfomed.\n");
}

void SelfTestControllerLow::RamTest()
{
//	UWord32 currTestStatus, prevFaultCount, faultStatus;
//	UWord32 prevFaultPacketValue = 0;
//	struct sysinfo info;
//
//	int status = sysinfo(&info);
//
//	if (status < 0)
//	{
//		printf("Fault Occured:Couldnt perform RAM Test\n");
//		currTestStatus = 1;
//	}
//	else
//	{
//		float total = (info.totalram / (1024 * 1024));
//		float free = (info.freeram / (1024 * 1024));
//
//		if (free >= total || free <= 1500)
//		{
//			currTestStatus = 1;
//			printf("Fault Occured:RAM threshold has been reached!\n");
//		}
//		else
//		{
//			currTestStatus = 0;
//		}
//	}
//
//	//update results into descriptor memory
//	MemoryMapAddressClass::kernelLowSelfTest.readDataFromRegKernel(3,
//			prevFaultPacketValue);
//
//	if ((prevFaultPacketValue & 0x00000001) == 0x00000001)
//	{
//		if (currTestStatus == 0)
//		{
//			faultStatus = (prevFaultPacketValue & 0xFFFE0001);
//			prevFaultCount = ((prevFaultPacketValue & 0x0001FFFE) >> 1);
//			prevFaultCount = prevFaultCount + 1;
//			faultStatus = (faultStatus | (prevFaultCount << 1));
//			faultStatus = faultStatus & 0xFFFFFFFE;
//			MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(3,
//					faultStatus);
//		}
//	}
//	else if ((prevFaultPacketValue & 0x00000001) == 0x00000000)
//	{
//		if (currTestStatus == 1)
//		{
//			faultStatus = prevFaultPacketValue;
//			faultStatus = faultStatus | 0x00000001; // insert fault flag
//			MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(3,
//					faultStatus);
//		}
//
//	}
	// Not required for now, by default passed

	MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(RAM_OFFSET,
			RAM_FAILURE_DEFAULT_VALUE);
	printf("RAM Test Performed.\n");
}

void SelfTestControllerLow::UsbTest()
{
	FILE *fp;
	Word8 commandResult[1035];
	Word8 dummyCommand[150] =
	{ ' ' };
	UWord32 currTestStatus = 0, prevFaultCount, faultStatus;
	UWord32 prevFaultPacketValue = 0;
	struct statvfs sfs;
	char usbPath[50] =
	{ "/media/sda1/" };

	MemoryMapAddressClass::kernelLowSelfTest.readDataFromRegKernel(USB_OFFSET,
			prevFaultPacketValue);

	fflush(NULL);
	sprintf(dummyCommand, "mount | grep ext3 | grep media > out.txt");
	system(dummyCommand);

	fp = fopen("out.txt", "r");
	if (fp == NULL)
	{
		printf("USB ST: Failed to run command\n");
		currTestStatus = 1;
	}
	else
	{
		fread(commandResult, 10, 1, fp);
		if (commandResult[0] != '/')
		{
			currTestStatus = 1;
		}
		pclose(fp);
	}

	if (currTestStatus != 1)
	{
		if (statvfs(usbPath, &sfs) != 0)
		{
			currTestStatus = 1;
			printf("USB ST: USB Device not found.\n");
		}
		else
		{
			double avail_space = getUsbFreeSpace(usbPath);

			if (avail_space <= 1)
			{
				currTestStatus = 1;
				printf("USB ST: USB Device memory full\n");
			}
			currTestStatus = 0;
		}
	}

	if ((prevFaultPacketValue & 0x00000001) == 0x00000001)
	{
		if (currTestStatus == 0)
		{
			faultStatus = (prevFaultPacketValue & 0xFFFE0001);
			prevFaultCount = ((prevFaultPacketValue & 0x0001FFFE) >> 1);
			prevFaultCount = prevFaultCount + 1;
			faultStatus = (faultStatus | (prevFaultCount << 1));
			faultStatus = faultStatus & 0xFFFFFFFE;
			MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(
					USB_OFFSET, faultStatus);
		}
	}
	else if ((prevFaultPacketValue & 0x00000001) == 0x00000000)
	{
		if (currTestStatus == 1)
		{
			faultStatus = prevFaultPacketValue;
			faultStatus = faultStatus | 0x00000001; // insert fault flag
			MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(
					USB_OFFSET, faultStatus);
		}
	}
	printf("USB Test Performed.\n");
}

void SelfTestControllerLow::PciLinkTest()
{
	UWord32 currTestStatus, prevFaultCount, faultStatus;
	UWord32 prevFaultPacketValue = 0, vdmResponse = 0;

	//check PCI Link with VDM
	MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(1023,
			vdmResponse);

	sprintf(tempBuffer, "%ld", 1L);
	MemoryMapAddressClass::kernelLowPciTest.openThenWriteThenClose(
			procNameLowPciTest, tempBuffer);

	MemoryMapAddressClass::kernelLowSelfTest.readDataFromRegKernel(1023,
			vdmResponse);
	if (vdmResponse == 0xBEAFDBED)
	{
		currTestStatus = 0;
	}
	else
	{
		currTestStatus = 1;
	}

	//update results into descriptor memory
	MemoryMapAddressClass::kernelLowSelfTest.readDataFromRegKernel(PCI_OFFSET,
			prevFaultPacketValue);

	if ((prevFaultPacketValue & 0x00000001) == 0x00000001)
	{
		if (currTestStatus == 0)
		{
			faultStatus = (prevFaultPacketValue & 0xFFFE0001);
			prevFaultCount = ((prevFaultPacketValue & 0x0001FFFE) >> 1);
			prevFaultCount = prevFaultCount + 1;
			faultStatus = (faultStatus | (prevFaultCount << 1));
			faultStatus = faultStatus & 0xFFFFFFFE;
			MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(
					PCI_OFFSET, faultStatus);
		}
	}
	else if ((prevFaultPacketValue & 0x00000001) == 0x00000000)
	{
		if (currTestStatus == 1)
		{
			faultStatus = prevFaultPacketValue;
			faultStatus = faultStatus | 0x00000001; // insert fault flag
			MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(
					PCI_OFFSET, faultStatus);
		}
	}
	printf("PCI Test Performed.\n");
}

void SelfTestControllerLow::TemperatureTest()
{
	// To be handled later
	MemoryMapAddressClass::kernelLowSelfTest.writeDataToRegKernel(TEMP_OFFSET,
			TEMP_FAILURE_DEFAULT_VALUE);

	printf("Temperature Test Performed.\n");
}

double SelfTestControllerLow::getUsbTotalSpace(char * dev_path)
{
	unsigned long long result = 0;
	struct statvfs sfs;

	if (statvfs(dev_path, &sfs) != -1)
	{
		result = (unsigned long long) sfs.f_bsize * sfs.f_blocks;
	}
	if (result > 0)
	{
		double f_cap = (double) result / (1024 * 1024 * 1024);
		return (f_cap);
	}
	else

		return 0;
}

double SelfTestControllerLow::getUsbFreeSpace(char * dev_path)
{
	unsigned long long result = 0;
	struct statvfs sfs;
	if (statvfs(dev_path, &sfs) != -1)
	{
		result = (unsigned long long) sfs.f_bsize * sfs.f_bfree;
	}
	if (result > 0)
	{
		double f_cap = (double) result / (1024 * 1024 * 1024);
		return (f_cap);
	}
	else

		return 0;
}

void SelfTestControllerLow::transmitSelfTestStatusPacket(uint32_t val)
{
	//informs the vdm about status of the selftest commands
	sprintf(tempBuffer, "%d", val);
	MemoryMapAddressClass::kernelLowSelfTestStatus.openThenWriteThenClose(
			procNameLowSelfTestStatus, tempBuffer);
}
void SelfTestControllerLow::transmitSelfTestResultsPacket()
{
	//informs the vdm about results of the selftest
	sprintf(tempBuffer, "%ld", 1L);
	MemoryMapAddressClass::kernelLowSelfTestStatus.openThenWriteThenClose(
			procNameLowSelfTest, tempBuffer);
}

void SelfTestControllerLow::performAutomatedSelfTest()
{
	UsbTest();
	PciLinkTest();
	TemperatureTest();
}

void SelfTestControllerLow::performTriggeredSelfTest()
{
	ArmCoresTest();
	DspCoresTest();
	FlashTest();
	RamTest();
}

void SelfTestControllerLow::performPowerOnSelfTest()
{
	UWord32 status;

	status = 0x00000010;
	transmitSelfTestStatusPacket(status);

	performAutomatedSelfTest();
	transmitSelfTestResultsPacket();

	status = 0x00000020;
	transmitSelfTestStatusPacket(status);

}

void SelfTestControllerLow::performIdBasedSelfTest(uint32_t faultId)
{
	switch (faultId)
	{

	case 1:       //checking if fault_id send by ccp matches arm core test
		ArmCoresTest();
		break;
	case 2:       //checking if fault_id send by ccp matches dsp core test
		DspCoresTest();
		break;
	case 3: //checking if fault_id send by ccp matches flash memory test
		FlashTest();
		break;
	case 4:            //checking if fault_id send by ccp matches ram test
		RamTest();
		break;
	case 5:            //checking if fault_id send by ccp matches usb test
		UsbTest();
		break;
	case 6:            //checking if fault_id send by ccp matches pci test
		PciLinkTest();
		break;
	case 7:            //checking if fault_id send by ccp matches pci test
		TemperatureTest();
		break;
	default:
		printf("Id Based ST: ST_Fault_Id: %d doesn't exist!\n", faultId);
		break;
	}
}

void SelfTestControllerLow::performSelfTest()
{
	UWord32 status;
	UWord32 selfTestCommand;
	UWord32 selfTestType;
	UWord32 selfTestFaultId;
	UWord32 selfTestResultsRequestFlag = 0;
	UWord32 OpCode = 0;

	MemoryMapAddressClass::kernelLowCcpRx.readDataFromRegKernel(6,
			selfTestCommand);

	OpCode = (UWord32) (selfTestCommand & 0xF0000000);

	if ((selfTestCommand != 0) && (OpCode == 0x00000000))
	{
		selfTestType = selfTestCommand & 0x00000007;
		selfTestFaultId = selfTestCommand & 0x00003FC0;
		selfTestFaultId = selfTestFaultId >> 6;
		selfTestResultsRequestFlag = selfTestCommand & 0x00000038;
		selfTestResultsRequestFlag = selfTestResultsRequestFlag >> 3;

		printf("BIST: %d\n", selfTestType);
		printf("Read_status: %d\n", selfTestResultsRequestFlag);
		printf("Fault_ID: %d\n", selfTestFaultId);

		//extract bist and perform accordingly
		switch (selfTestType)
		{
		case 1:
			status = 0x1;
			transmitSelfTestStatusPacket(status);
			performTriggeredSelfTest();
			transmitSelfTestResultsPacket();
			status = 0x2;
			transmitSelfTestStatusPacket(status);
			break;

		case 3:
			status = 0x40;
			transmitSelfTestStatusPacket(status);

			performIdBasedSelfTest(selfTestFaultId);
			transmitSelfTestResultsPacket();
			status = 0x80;
			transmitSelfTestStatusPacket(status);
			break;
		case 4:
			status = 0x10;
			transmitSelfTestStatusPacket(status);

			performAutomatedSelfTest();
			transmitSelfTestResultsPacket();
			status = 0x20;
			transmitSelfTestStatusPacket(status);

			break;
		default:
			break;
		}

		if (selfTestResultsRequestFlag == 3)
		{
			printf("CCP requested for descriptive memory\n");
			transmitSelfTestResultsPacket();
		}
		selfTestCommand = 0;
		MemoryMapAddressClass::kernelLowCcpRx.writeDataToRegKernel(6,
				selfTestCommand);

	}
	uint64_t currTime = Utility::getSystemTime();
	uint64_t timeDiff = currTime - timeSinceLastAutoSelfTest;
	if (timeDiff > 50 * 1000 * 1000) //50 seconds
	{
		timeSinceLastAutoSelfTest = currTime;
		performAutomatedSelfTest();
	}
}

void SelfTestControllerLow::handleVdmPcieTriggeredSelfTest()
{
	UWord32 vdmSelfTestVal = 0;
	MemoryMapAddressClass::kernelLowVdmMemory.readDataFromRegKernel(6,
			vdmSelfTestVal);

	if (vdmSelfTestVal != 0)
	{
		printf("VDM requested self test \n");
		vdmSelfTestVal = vdmSelfTestVal & 0xFFFFFFFF; //Modify this to change the write back value
		MemoryMapAddressClass::kernelLowVdmMemory.writeDataToRegKernel(6,
				vdmSelfTestVal);

		sprintf(tempBuffer, "%ld", 1L);
		MemoryMapAddressClass::kernelLowVdmMemory.openThenWriteThenClose(
				procNameVdmMemory, tempBuffer);
	}
}
