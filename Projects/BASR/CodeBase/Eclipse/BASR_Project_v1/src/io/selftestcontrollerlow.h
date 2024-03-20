/*
 * selftestlow.h
 *
 *  Created on: Feb 24, 2021
 *      Author: root
 */

#ifndef SELFTESTLOW_H_
#define SELFTESTLOW_H_

#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <mtd/mtd-user.h>
#include "../common/common.h"
#include "../io/memorymap.h"
#include "../utility/utility.h"

class SelfTestControllerLow {

	constexpr static const char* procNameLowSelfTestStatus	= "/proc/PCI_PICOZED_Send_selftest_status";
	constexpr static const char* procNameLowSelfTest 		= "/proc/PCI_PICOZED_Param_self_r";
	constexpr static const char* procNameLowCcpRx 			= "/proc/PCI_PICOZED_CCP_RX";
	constexpr static const char* procNameLowPciTest 		= "/proc/PCI_PICOZED_PCI_VDM";
	constexpr static const char* procNameVdmMemory 			= "/proc/PCI_PICOZED_Param_vdm";

	static const uint16_t DATA_BUFFER_SIZE 				= 4096;

	static const uint32_t ARM_FAILURE_DEFAULT_VALUE 	= 0x44020000;
	static const uint32_t DSP_FAILURE_DEFAULT_VALUE 	= 0x44040000;
	static const uint32_t FLASH_FAILURE_DEFAULT_VALUE 	= 0x44060000;
	static const uint32_t RAM_FAILURE_DEFAULT_VALUE 	= 0x44080000;
	static const uint32_t USB_FAILURE_DEFAULT_VALUE 	= 0x440A0000;
	static const uint32_t PCI_FAILURE_DEFAULT_VALUE 	= 0x440C0000;
	static const uint32_t TEMP_FAILURE_DEFAULT_VALUE 	= 0x440E0000;

	static const uint32_t ARM_OFFSET 					= 0x0;
	static const uint32_t DSP_OFFSET 					= 0x1;
	static const uint32_t FLASH_OFFSET 					= 0x2;
	static const uint32_t RAM_OFFSET 					= 0x3;
	static const uint32_t USB_OFFSET 					= 0x4;
	static const uint32_t PCI_OFFSET 					= 0x5;
	static const uint32_t TEMP_OFFSET 					= 0x6;

private:
	char tempBuffer[10];
	uint64_t timeSinceLastAutoSelfTest;

	void ArmCoresTest();
	void DspCoresTest();
	void FlashTest();
	void RamTest();
	void UsbTest();
	void PciLinkTest();
	void TemperatureTest();

	double getUsbFreeSpace(char * dev_path);
	double getUsbTotalSpace(char * dev_path);

	void transmitSelfTestStatusPacket(uint32_t val);
	void transmitSelfTestResultsPacket();
	void performAutomatedSelfTest();
	void performTriggeredSelfTest();
	void performPowerOnSelfTest();
	void performIdBasedSelfTest(uint32_t faultId);

public:
	void initializeSelfTestLow();
	void performSelfTest();
	void handleVdmPcieTriggeredSelfTest();
};

#endif /* SELFTESTLOW_H_ */
