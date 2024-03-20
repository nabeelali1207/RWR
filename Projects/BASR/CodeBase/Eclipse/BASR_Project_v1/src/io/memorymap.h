/*
 * memorymap.h
 *
 *  Created on: Oct 16, 2019
 *      Author: gmd
 */

#ifndef MEMORYMAP_H_
#define MEMORYMAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "../common/common.h"

class MemoryMapAccessClass
{
	UWord64 mmapSize;
	UWord8 * mmapAddress;
	volatile UWord8 * virtAddress;
	unsigned long long * virtAddressKernel;
	UWord8 * physAddress;
	FILE* fileDescriptor;
	bool isValid;
public:
	MemoryMapAccessClass();
	void initializeFileDescriptor(const char * procName);
	void openThenWriteThenClose(const char * procName,const char* buffer);
	void initializeKernelMemory(UWord8 readWrite, UWord32 memSize, const char * procName);
	void initialize(UWord8 * _physAddress, UWord32 memSize=4096);
	~MemoryMapAccessClass();
	bool isMappingValid();
	void writeDataToReg(UWord32 offset, UWord32 value);
	void writeDataToReg(UWord32 offset, UWord32 value, UWord32 mask);
	UWord32 readDataFromReg(UWord32 offset);
	void writeDataToRegKernel(UWord32 offset, UWord32 value);
	void writeDataToRegKernel(UWord32 offset, UWord64 value);
	void readDataFromRegKernel(UWord32 offset, UWord32& result);
	void readDataFromRegKernel(UWord32 offset, UWord64& result);

	//write data from mapped memory starting at offset to buffer
	void blockReadWordByWord_v1(UWord32 offset, UWord8 * buffer,
			UWord32 byteCount);
	void blockReadWordByWord_v2(UWord32 offset, UWord8 * buffer,
			UWord32 byteCount);
	void blockReadWordByWord_v3(UWord32 offset, UWord8 * buffer,
			UWord32 byteCount);
	void blockWriteWordByWordKernel(UWord32 offset, UWord8 * buffer,
			UWord32 byteCount);
	void blockReadWordByWordKernel(UWord32 offset, UWord8 * buffer,
			UWord32 byteCount);

	void blockReadChunkByChunk(UWord32 offset,
			UWord8 * buffer, UWord32 byteCount);

	//write data from buffer to mapped memory starting at offset
	void blockWriteWordByWord(UWord32 offset, UWord8 * buffer,
			UWord32 byteCount);

	//write a specific value to mapped memory starting at offset
	void blockMemSetWordByWord(UWord32 offset, UWord32 val, UWord32 byteCount);

	void writeToFile(const char* buffer);

	volatile UWord8 * getVirtualAddress();

	volatile UWord8* getVirtualAddressKernel();
};

class MemoryMapAddressClass
{
public:
	static const UWord32 CWP_CQSI_BASE_REGISTER = 0x83C10000;
	static const UWord32 DIFM_CSI_BASE_REGISTER = 0x83C30000;
	static const UWord32 TRENZ_SPI_BASE_REGISTER = 0x83C20000;
	static const UWord32 XADC_TEMPERATURE_REGISTER = 0xF8007100;
	static const UWord32 TP_SELF_TEST_REGISTER = 0x43C10000;
	static const UWord32 PPF_SELF_TEST_REGISTER = 0x83C00000;
	static const UWord32 VDM_PCIE_REGISTER = 0x44A00000;
	static const UWord32 CFD_UART_BASE_REGISTER = 0x43C30000;

	static MemoryMapAccessClass kernelLowPdwBuffer[4];
	static MemoryMapAccessClass kernelLowPollVector;

	static MemoryMapAccessClass kernelLowCcpStatus;
	static MemoryMapAccessClass kernelLowDspReport;
	static MemoryMapAccessClass kernelLowDspReportControl;
	static MemoryMapAccessClass kernelLowDspReportStatus;

	static MemoryMapAccessClass kernelLowSelfTestStatus;
	static MemoryMapAccessClass kernelLowSelfTest;
	static MemoryMapAccessClass kernelLowCcpRx;
	static MemoryMapAccessClass kernelLowPciTest;
	static MemoryMapAccessClass kernelLowVdmMemory;
	
	static MemoryMapAccessClass cwpCqsiReg;
	static MemoryMapAccessClass difmCsiReg;
	static MemoryMapAccessClass trenzSpiReg;
	static MemoryMapAccessClass xadcTemperatureReg;
	static MemoryMapAccessClass tpSelfTestReg;
	static MemoryMapAccessClass ppfSelfTestReg;
	static MemoryMapAccessClass vdmPcieReg;
	static MemoryMapAccessClass sharedMemoryPcieReg;
	static MemoryMapAccessClass cfdUartReg;

};
#ifdef MEMORYMAP_CPP_

MemoryMapAccessClass MemoryMapAddressClass::kernelLowPdwBuffer[4];
MemoryMapAccessClass MemoryMapAddressClass::kernelLowPollVector;

MemoryMapAccessClass MemoryMapAddressClass::kernelLowCcpStatus;
MemoryMapAccessClass MemoryMapAddressClass::kernelLowDspReport;
MemoryMapAccessClass MemoryMapAddressClass::kernelLowDspReportControl;
MemoryMapAccessClass MemoryMapAddressClass::kernelLowDspReportStatus;

MemoryMapAccessClass MemoryMapAddressClass::kernelLowSelfTestStatus;
MemoryMapAccessClass MemoryMapAddressClass::kernelLowSelfTest;
MemoryMapAccessClass MemoryMapAddressClass::kernelLowCcpRx;
MemoryMapAccessClass MemoryMapAddressClass::kernelLowPciTest;
MemoryMapAccessClass MemoryMapAddressClass::kernelLowVdmMemory;

MemoryMapAccessClass MemoryMapAddressClass::cwpCqsiReg;
MemoryMapAccessClass MemoryMapAddressClass::difmCsiReg;
MemoryMapAccessClass MemoryMapAddressClass::trenzSpiReg;
MemoryMapAccessClass MemoryMapAddressClass::xadcTemperatureReg;
MemoryMapAccessClass MemoryMapAddressClass::tpSelfTestReg;
MemoryMapAccessClass MemoryMapAddressClass::ppfSelfTestReg;
MemoryMapAccessClass MemoryMapAddressClass::vdmPcieReg;
MemoryMapAccessClass MemoryMapAddressClass::sharedMemoryPcieReg;
MemoryMapAccessClass MemoryMapAddressClass::cfdUartReg;

#endif

#endif /* MEMORYMAP_H_ */
