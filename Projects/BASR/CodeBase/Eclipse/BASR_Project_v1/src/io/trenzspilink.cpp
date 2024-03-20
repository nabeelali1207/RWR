/*
 * trenzspilink.cpp
 *
 *  Created on: Jan 28, 2021
 *      Author: saad
 */

#include "trenzspilink.h"

//This function is main logic of all the communication from Trenz to
//CCP, it reads the data sent by Trenz and extracts the commands from that
//data
void TrenzSpiLink::interProcessorCommunicationLogic()
{
	readTrenzCommandFromSpi();
	if ((bytesRead > 0))
	{
		bufferExtract();
	}
}

//This function is used to initialize Trenz SPI link
void TrenzSpiLink::initialize()
{
	MemoryMapAddressClass::trenzSpiReg.initialize(
			(UWord8*) MemoryMapAddressClass::TRENZ_SPI_BASE_REGISTER);
	TrenzSpiLink::sendWord(TrenzSpiLink::VDM_SPI_CONTROL, TrenzSpiLink::RESET);
	TrenzSpiLink::sendWord(TrenzSpiLink::VDM_SPI_CONTROL,
			TrenzSpiLink::SELF_TEST_OFF);
}

//This function cleans up internal data structures of the
//class i.e. data array
void TrenzSpiLink::clean()
{
	memset(data, 0, 10000 * sizeof(UWord32));
}

//This function sends the word to address offset on Trenz SPI link
void TrenzSpiLink::sendWord(UWord32 address, UWord32 word)
{
	MemoryMapAddressClass::trenzSpiReg.writeDataToReg(address, word);
}

//This function receives the data from address offset on Trenz SPI link
UWord32 TrenzSpiLink::recvWord(UWord32 address)
{
	UWord32 val = MemoryMapAddressClass::trenzSpiReg.readDataFromReg(address);
	return val;
}

//This function reads all the data sent from Trenz to CCP in internal data buffer
void TrenzSpiLink::readTrenzCommandFromSpi()
{
	Word32 read = 0;
	bytesRead = 0;
	memset(data, 0, sizeof(data));
	if (ConstHigh::simOnActualHwFlag == 1)
	{
		read = TrenzSpiLink::recvWord(VDM_SPI_STATUS);
		while (!(read & SLAVE_EMPTY))
		{
			data[bytesRead] = TrenzSpiLink::recvWord(VDM_SPI_SLAVERX);
			//printf("D0: %.8x \n",data[0]);
			if (data[0] == 0xdeadbeef)
			{
//                printf("%.8x \n", data[bytesRead]);
				bytesRead++;
			}
			read = TrenzSpiLink::recvWord(VDM_SPI_STATUS);
			if (read & SLAVE_EMPTY)
			{
				usleep(50);
				read = TrenzSpiLink::recvWord(VDM_SPI_STATUS);
				//printf("SPI:\t Data recieved\n");
			}
		}
	}
	else
	{
		spiSubOption = 0;
		spiOption = 10;
	}
}

//This function checks if Trenz is up and running, indication of which is
//Trenz sends 0x10 in second word of data packet, returns the success flag
UWord8 TrenzSpiLink::checkTrenzUp()
{
	while (1)
	{
		readTrenzCommandFromSpi();
		if (data[1] == 0x10)
		{
			TrenzSpiLink::sendWord(VDM_SPI_MASTERTX, 0x12);
			return 0;
		}
	}
}

//This function writes Tracking data to Trenz SPI
void TrenzSpiLink::writeTrackingDataToSpi(SpiTrackReply *spiTrack)
{
	Word32 read = 0;
	Word32 k = 0;
	UWord32 *ptr;
	//printf("NT: %.8x \n",spiTrack->numberOfTracks);
	//printf("NR: %.8x \n",pExtendedAefReportCurrent->NumberofEmitters);
	//printf("NT: %d \n",spiTrack->audioID);
	ptr = (UWord32*) spiTrack;
	read = TrenzSpiLink::recvWord(VDM_SPI_STATUS);
//	printf("STD4\n");
	if ((!(read & MASTER_FULL)))
	{
//		printf("SPI:\tTr Rp DW \n");
		for (k = 0; k < 75; k++)
		{
//			printf("%.8x \n", ptr[k]);
			TrenzSpiLink::sendWord(TrenzSpiLink::VDM_SPI_MASTERTX, ptr[k]);
		}
	}
}

//This function fills the Trenz SPI data structure and sends the data structure to
//Trenz through Trenz SPI
void TrenzSpiLink::writeReplyToTrenz(UWord8 messageType, UWord8 messageSize,
		UWord8 code)
{
	UWord16 sizeOfData = 0, size;
	SendReplyToTrenz reply;
	sendReplyToTrenz = &reply;
	size = (sizeof(TrenzCommunicationHeader) + messageSize * sizeof(UWord16));
	sizeOfData = (size / 4);
	if ((size % 4) != 0)
	{
		sizeOfData = sizeOfData + 1;
	}
	memset(sendReplyToTrenz, 0, sizeof(SendReplyToTrenz));
	sendReplyToTrenz->trenzCommunicationHeader.delimeter = 0xdeadbeef;
	sendReplyToTrenz->trenzCommunicationHeader.messageType = messageType;
	sendReplyToTrenz->trenzCommunicationHeader.crc = calculateCRC(
			(UWord8*) &transmitBuffer[0], messageSize * sizeof(UWord16), 0);
	if (code != 0)
	{
		transmitBuffer[0] = code;
	}
	memcpy(&sendReplyToTrenz->messageData[0], &transmitBuffer[0],
			messageSize * sizeof(UWord16));
	pOutputWords = (UWord32*) sendReplyToTrenz;
	writeSpi(sizeOfData);
	memset(sendReplyToTrenz, 0, sizeof(SendReplyToTrenz));
	memset(transmitBuffer, 0, sizeof(transmitBuffer));
}

//This function writes words amount of data word to SPI
void TrenzSpiLink::writeSpi(UWord16 words)
{
	UWord32 writeStatus = 0;
	UWord16 i = 0;

	//printf("\n");
	writeStatus = TrenzSpiLink::recvWord(VDM_SPI_STATUS);
	while (!(writeStatus & MASTER_FULL))
	{
		if (i >= words)
		{
			break;
		}
		else
		{
			//printf("%.8x ", pOutputWords[i]);
			TrenzSpiLink::sendWord(VDM_SPI_MASTERTX, pOutputWords[i]);
			i++;

			writeStatus = TrenzSpiLink::recvWord(VDM_SPI_STATUS);
		}
	}
	//printf("\n");
}

//This function extracts all the messages received from Trenz
void TrenzSpiLink::bufferExtract()
{
	UWord32 i = 0;
	for (i = 0; i < bytesRead; i++)
	{
		if (data[i] == 0xdeadbeef)
		{
			extractTrenzMessage(i);
		}
	}
	bytesRead = 0;
}

//This function calculates CRC of the data
UWord16 TrenzSpiLink::calculateCRC(UWord8 *dataPtr, UWord32 size,
		UWord32 offset)
{
	UWord16 crc = 0xffff;
	UWord32 i = 0;
	UWord16 *ptr = (UWord16*) dataPtr;

	size = size / 2; // as size was in number of bytes (conversion to number of 16bit-words)
	offset = (offset % 2) + (offset / 2);    //Adjust offset also

	for (i = offset; i < size; i++)
	{
		crc ^= (UWord16) ptr[i];
	}
	return crc;
}
