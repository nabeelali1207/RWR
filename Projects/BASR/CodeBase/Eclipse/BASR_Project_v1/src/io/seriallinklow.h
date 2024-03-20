/*
 * seriallinklow.h
 *
 *  Created on: Oct 16, 2019
 *      Author: gmd
 */

#ifndef SERIALLINKLOW_H_
#define SERIALLINKLOW_H_

#include "../io/memorymap.h"
#include "../utility/utility.h"

class SerialLinkLowToHigh
{
	constexpr static const char* procNameCcpStatus = "/proc/PCI_PICOZED_Paramc_ccp_status";
	constexpr static const char* procNameDspReport = "/proc/PCI_PICOZED_Param_dsp_r";
	constexpr static const char* procNameDspReportControl = "/proc/PCI_PICOZED_Paramc_control_buffer";
	constexpr static const char* procNameDspReportStatus = "/proc/PCI_PICOZED_Paramc_report_status";

	static const uint16_t PDW_BUFFER_SIZE = 16384;
	static const uint16_t DSP_REPORT_SIZE = 8192;
	static const uint16_t DATA_BUFFER_SIZE = 4096;
	static const uint16_t MAX_PDWS = 512;
	static const uint16_t PDW_SIZE = 32;

private:
	char tempBuffer[10];
	bool ccpUpFlag;
	UWord8* localBuffer;
	uint64_t lastCcpUpTime;

public:
	SerialLinkLowToHigh();
	static void checkKernelPciMod();
	void initialize();
	void waitForCcpUp();
	void checkCcpUp();
	bool sendDspReport(uint8_t * dataPtr, uint64_t messageSize);
};

#endif /* SERIALLINKLOW_H_ */
