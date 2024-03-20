/*
 * threatprocessorbandswitchingtime.h
 *
 *  Created on: Feb 11, 2021
 *      Author: saad
 */

#ifndef THREATPROCESSORBANDSWITCHINGTIME_H_
#define THREATPROCESSORBANDSWITCHINGTIME_H_

#include "../common/common.h"
#include "../model/model.h"
#include "../io/selftestcontrollerhigh.h"

typedef struct ThreatProcessorBandSwitchingTime {
	UWord32 crc;
	UWord64 cdBandSwitchingTime;
	UWord64 eBandSwitchingTime;
	UWord64 gBandSwitchingTime;
	UWord64 iBandSwitchingTime;
	UWord64 jBandSwitchingTime;
	UWord64 kBandSwitchingTime;
} ThreatProcessorBandSwitchingTime;

class ThreatProcessorBandSwitchingTimeClass {
	static const unsigned int TP_BAND_SWITCHING_BASE_REG = 0x00000018;
	static const unsigned int TP_BAND_SWITCHING_BASE_COMMAND = 0x10000001;
private:
	SelfTestControllerHigh *st;
	ThreatProcessorBandSwitchingTime threatProcessorBandSwitchingTime;

	/*
	 * Function Name:   calculateCrcOfThreatProcessorAntennaBandSwitchingTime
	 * Purpose:			This function returns CRC of threat processor band switching time data structure
	 * Inputs:          none
	 * Outputs:         CRC
	 * Scope:           Can be called by any function
	 */
	UWord32 calculateCrcOfThreatProcessorAntennaBandSwitchingTime();
	void sendAntennaSwitchingTimeStructureToThreatProcessor();
public:
	UWord8 sendBandSwitchDataToTp;

	/*
	 * Function Name:   fillThreatProcessorAntennaSwitchingTimeStructure
	 * Purpose:			This function fills threat processing band switching time structure
	 * 					for now this is being filled with dummy values
	 * Inputs:          none
	 * Outputs:         Flag of success
	 * Scope:           Can be called by any function
	 */
	UWord32 fillThreatProcessorAntennaSwitchingTimeStructure();

	/*
	 * Function Name:   writeThreatProcessorAntennaSwitchingTimeStructureToThreatProcessor
	 * Purpose:			This function writes Antenna Switching Time Structure to Threat Processor using PCIe and
	 * 					TP self test registers
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void writeThreatProcessorAntennaSwitchingTimeStructureToThreatProcessor();

	/*
	 * Function Name:   sendAntennaSwitchingTimeStructureToThreatProcessorLogic
	 * Purpose:			This function is main logic of sending Antenna Switching Time to TP
	 * 					It checks if we need to send it, if yes, sending function is called
	 * Inputs:          None
	 * Outputs:         none
	 * Scope:           Shall be called by main loop
	 */
	void sendAntennaSwitchingTimeStructureToThreatProcessorLogic();

	/*
	 * Function Name:   checkWriteThreatProcessorAntennaSwitchingTimeStructureToThreatProcessor
	 * Purpose:			This function just checks whether CRC sent back by TP matches with Antenna Switching
	 * 					Time Structures CRC, if it doesn't, it raises a flag that structure needs to be sent
	 * 					again
	 * Inputs:          Received CRC
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void checkWriteThreatProcessorAntennaSwitchingTimeStructureToThreatProcessor(
			UWord32 RSSCRC);
};

#endif /* THREATPROCESSORBANDSWITCHINGTIME_H_ */
