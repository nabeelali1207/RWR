/*
 * trenzspilink.h
 *
 *  Created on: Jan 28, 2021
 *      Author: saad
 */

#ifndef IO_TRENZSPILINK_H_
#define IO_TRENZSPILINK_H_

#include "../io/cfcard.h"
#include "../io/memorymap.h"
#include "../utility/consthigh.h"
#include "../delegate/cfduartdelegate.h"
#include "../delegate/trenzspidelegate.h"
#include "../vo/trenzspidatastructures.h"

class SelfTestControllerHigh;
class SelfTestCcpHigh;
class TrenzSpiLink
{
	static const unsigned int VDM_SPI_BASEADDRESS = 0x83C20000;
	static const unsigned int VDM_SPI_CONTROL = 0x00000000;
	static const unsigned int VDM_SPI_STATUS = 0x00000004;
	static const unsigned int VDM_SPI_MASTERTX = 0x00000008;
	static const unsigned int VDM_SPI_SLAVERX = 0x0000000C;

	static const unsigned int RESET = 0x00000001;
	static const unsigned int SELF_TEST = 0x00000002;
	static const unsigned int SELF_TEST_OFF = 0x00000000;
	static const unsigned int LINKUP = 0x00000001;
	static const unsigned int MASTER_FULL = 0x00000001;
	static const unsigned int SLAVE_EMPTY = 0x00000002;

private:

public:
	UWord32 data[10000];
	UWord32 bytesRead;
	UWord16 receiveBuffer[32];
	UWord16 transmitBuffer[32];
	UWord32 spiSubOption;
	UWord8 spiOption;
	UWord32 *pOutputWords;
	SendReplyToTrenz *sendReplyToTrenz;

	/*
	 * Function Name:   interProcessorCommunicationLogic
	 * Purpose:			This function is main logic of all the communication from Trenz to
	 * 					CCP, it reads the data sent by Trenz and extracts the commands from that
	 * 					data
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void interProcessorCommunicationLogic();

	/*
	 * Function Name:   initialize
	 * Purpose:			This function is used to initialize Trenz SPI link
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void initialize();

	/*
	 * Function Name:   clean
	 * Purpose:			This function cleans up internal data structures of the
	 * 					class i.e. data array
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void clean();

	/*
	 * Function Name:   sendWord
	 * Purpose:			This function sends the word to address offset on Trenz SPI link
	 * Inputs:          address
	 * 					word
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void sendWord(UWord32 address, UWord32 word);

	/*
	 * Function Name:   recvWord
	 * Purpose:			This function receives the data from address offset on Trenz SPI link
	 * Inputs:          address
	 * Outputs:         word
	 * Scope:           Can be called by any function
	 */
	UWord32 recvWord(UWord32 address);

	/*
	 * Function Name:   readTrenzCommandFromSpi
	 * Purpose:			This function reads all the data sent from Trenz to CCP in internal data buffer
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void readTrenzCommandFromSpi();

	/*
	 * Function Name:   checkTrenzUp
	 * Purpose:			This function checks if Trenz is up and running, indication of which is
	 * 					Trenz sends 0x10 in second word of data packet, returns the success flag
	 * Inputs:          none
	 * Outputs:         flag suggesting success of trenz is up
	 * Scope:           Can be called by any function
	 */
	UWord8 checkTrenzUp();

	/*
	 * Function Name:   writeTrackingDataToSpi
	 * Purpose:			This function writes Tracking data to Trenz SPI
	 * Inputs:          Tracking data to be sent
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void writeTrackingDataToSpi(SpiTrackReply *spiTrack);

	/*
	 * Function Name:   writeReplyToTrenz
	 * Purpose:			This function fills the Trenz SPI data structure and sends the data structure to
	 * 					Trenz through Trenz SPI
	 * Inputs:          Message Type
	 * 					Size of Message
	 * 					Code of Message
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void writeReplyToTrenz(UWord8 messageType, UWord8 messageSize, UWord8 code);

	/*
	 * Function Name:   writeSpi
	 * Purpose:			This function writes words amount of data word to SPI
	 * Inputs:          Number of words to be sent
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void writeSpi(UWord16 words);

	/*
	 * Function Name:   bufferExtract
	 * Purpose:			This function extracts all the messages received from Trenz
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void bufferExtract();

	/*
	 * Function Name:   calculateCRC
	 * Purpose:			This function calculates CRC of the data
	 * Inputs:          CRC
	 * Outputs:         Relevant objects of classes
	 * Scope:           Can be called by any function
	 */
	UWord16 calculateCRC(UWord8 *dataPtr, UWord32 size, UWord32 offset);
};

#endif /* IO_TRENZSPILINK_H_ */
