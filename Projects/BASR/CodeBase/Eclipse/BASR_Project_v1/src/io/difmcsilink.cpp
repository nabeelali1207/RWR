/*
 * difmcsilink.cpp
 *
 *  Created on: Jan 15, 2021
 *      Author: saad
 */
#define DIFMCSILINK_CPP_

#include "difmcsilink.h"

//This function is used to initialize DIFM CSI link
void DifmCsiLink::initialize()
{
	MemoryMapAddressClass::difmCsiReg.initialize(
			(UWord8*) MemoryMapAddressClass::DIFM_CSI_BASE_REGISTER);
	DifmCsiLink::sendWord(DifmCsiLink::CSI_CONTROL_OFFSET, 1);
	DifmCsiLink::sendWord(DifmCsiLink::CSI_CONTROL_OFFSET, 0);
	memset(&mdfStruct, 0, sizeof(mdfStruct));
	memset(&rssStruct, 0, sizeof(rssStruct));
}

//This function cleans up internal data structures of the
//class i.e. csiBuffeer, mdfStruct and rssStruct
void DifmCsiLink::clean()
{
	memset(csiBuffer, 0, 64 * sizeof(UWord32));
	memset(&mdfStruct, 0, sizeof(mdfStruct));
	memset(&rssStruct, 0, sizeof(rssStruct));
}

//This function sends the word to address offset on DIFM CSI link
void DifmCsiLink::sendWord(UWord32 address, UWord32 word)
{
	MemoryMapAddressClass::difmCsiReg.writeDataToReg(address, word);
}

//This function receives the data from address offset on DIFM CSI link
UWord32 DifmCsiLink::recvWord(UWord32 address)
{
	UWord32 val = MemoryMapAddressClass::difmCsiReg.readDataFromReg(address);
	return val;
}

//This function calculates CRC of the buffer
UWord32 DifmCsiLink::calculateCrc(UWord32 *ptr, UWord32 sizeOfStructure)
{
	UWord32 i = 0;
	UWord64 localCrc = 0;
	for (i = 0; i < sizeOfStructure - 2; i++) {
		localCrc ^= ptr[i];
	}
	return localCrc;
}

void DifmCsiLink::sendResetToDifm()
{
	UWord32 opcode = 0x32;	// Dec 50, Hex 32
	DifmCsiLink::sendWord(CSI_TX_OFFSET, opcode);

	usleep(1000);
}

//This function fills RSS Structure from TL or with default values
void DifmCsiLink::fillRssStructure(bool tlFailure, LibraryStruct &library)
{
	UWord32 i = 0, j = 0;

	memset(&rssStruct, 0, sizeof(RssStructure));

	rssStruct.opcode = 9;

	if ((tlFailure == false) && (library.constants.rss.s1Block[0].sMin != 0))
	{
		//fill from library
		for (i = 0; i < 256; i++)
		{
			rssStruct.s1Data[i].bandResSMin =
					(library.constants.rss.s1Block[i].band)
							| (library.constants.rss.s1Block[i].sMin << 8);
			rssStruct.s1Data[i].sMax = library.constants.rss.s1Block[i].sMax;
			rssStruct.s2Data[i].bandResSMin =
					(library.constants.rss.s2Block[i].band)
							| (library.constants.rss.s2Block[i].sMin << 8);
			rssStruct.s2Data[i].sMax = library.constants.rss.s2Block[i].sMax;
		}
	}
	else
	{
		//no library exists so hardcoded

		// fill RSS structure as per TL data
		for (i = 0; i < 5; i++)
		{
			rssStruct.s1Data[j + 0].bandResSMin = ((0) | (400000 << 8));
			rssStruct.s1Data[j + 0].sMax = 2000000;

			rssStruct.s1Data[j + 1].bandResSMin = ((2) | (400000 << 8));
			rssStruct.s1Data[j + 1].sMax = 2000000;

			rssStruct.s1Data[j + 2].bandResSMin = ((3) | (400000 << 8));
			rssStruct.s1Data[j + 2].sMax = 2000000;

			rssStruct.s1Data[j + 3].bandResSMin = ((1) | (800000 << 8));
			rssStruct.s1Data[j + 3].sMax = 4000000;

			rssStruct.s1Data[j + 4].bandResSMin = ((3) | (400000 << 8));
			rssStruct.s1Data[j + 4].sMax = 2000000;

			j += 5;
		}

		rssStruct.s2Data[0].bandResSMin = ((6) | (400000 << 8));
		rssStruct.s2Data[0].sMax = 2000000;

		j = 1;
		for (i = 0; i < 5; i++)
		{
			// S2 Values
			rssStruct.s2Data[j + 0].bandResSMin = ((0) | (400000 << 8));
			rssStruct.s2Data[j + 0].sMax = 2000000;

			rssStruct.s2Data[j + 1].bandResSMin = ((2) | (400000 << 8));
			rssStruct.s2Data[j + 1].sMax = 2000000;

			rssStruct.s2Data[j + 2].bandResSMin = ((3) | (400000 << 8));
			rssStruct.s2Data[j + 2].sMax = 2000000;

			rssStruct.s2Data[j + 3].bandResSMin = ((1) | (800000 << 8));
			rssStruct.s2Data[j + 3].sMax = 4000000;

			j += 4;
		}
	}
}

//This function fills MDF Structure from TL or with default values
void DifmCsiLink::fillMdfStructure(bool tlFailure, LibraryStruct &library)
{
	UWord8 i = 0;
	UWord32 cwThreshold = 90;
	UWord32 difmThreshold = 25;
	memset(&mdfStruct, 0, sizeof(MdfStructure));

	mdfStruct.opcodeRes = 8;
	mdfStruct.antennaSequence = 228;

	if ((tlFailure == false) && (library.constants.rss.s1Block[0].sMin != 0))
	{
		mdfStruct.s1Reps = library.constants.mdf.s1Reps;
		mdfStruct.numOfS1 = library.constants.mdf.numOfS1;
		mdfStruct.numOfS2 = library.constants.mdf.numOfS2;

		for (i = 0; i < 10; i++)
		{
			mdfStruct.bandFilter[i].bandFilterLower =
					library.constants.funcParams.funcConfig.bandFilter[i].bandFilterLower;
			mdfStruct.bandFilter[i].bandFilterUpper =
					library.constants.funcParams.funcConfig.bandFilter[i].bandFilterUpper;
		}
	} else {
		mdfStruct.s1Reps = 4;
		mdfStruct.numOfS1 = 25;
		mdfStruct.numOfS2 = 21;
	}

	mdfStruct.eBandThreshold = ((cwThreshold) | (difmThreshold << 19));
	mdfStruct.gBandThreshold = ((cwThreshold) | (difmThreshold << 19));
	mdfStruct.iBandThreshold = ((cwThreshold) | (difmThreshold << 19));
	mdfStruct.jBandThreshold = ((cwThreshold) | (difmThreshold << 19));
	mdfStruct.cdBandThreshold = ((cwThreshold) | (difmThreshold << 19));

	mdfStruct.cwProcessTime = 0x11170;
	mdfStruct.maxAttnProcessTime = 0x7A1200;
	mdfStruct.extraParams = 4 | (2 << 4) | (2 << 8) | (1 << 12); // x, y, z, k
}

//This function implements the main logic of MDF sending to DIFM
//First, it load MDF from TL, then it calculates its CRC and eventually
//it sends MDF in the form of 4 byte words through CSI link
void DifmCsiLink::writeMdfToCsi(bool tlFailure, LibraryStruct &library)
{
	Word32 status = 0;
	UWord32 k = 0;
	UWord32 *ptr;
	if (ConstHigh::simOnActualHwFlag == 1)
	{
		fillMdfStructure(tlFailure, library);
		ptr = (UWord32*) &mdfStruct;
		mdfStruct.crcMsb = calculateCrc(ptr, sizeof(mdfStruct)/4);

		status = DifmCsiLink::recvWord(CSI_STATUS_OFFSET);

		if ((status & 0x00000001) == 0)
		{
			UWord32 wordsToSend = sizeof(mdfStruct)/4;
			if((sizeof(mdfStruct)%4)!=0)
			{
				wordsToSend++;
			}

			printf("CSI:\tData Writing \n");
			for (k = 0; k < wordsToSend; k++)
			{
//                printf("%d, %.8x \n", k, ptr[k]);
				DifmCsiLink::sendWord(CSI_TX_OFFSET, ptr[k]);
			}
		}
	}
}

//This function implements the main logic of RSS sending to DIFM
//First, it load RSS from TL, then it calculates its CRC and eventually
//it sends RSS in the form of 4 byte words through CSI link
void DifmCsiLink::writeRssToCsi(bool tlFailure, LibraryStruct &library)
{
	Word32 status = 0;
	UWord32 *ptr;
	UWord32 k = 0;
	if (ConstHigh::simOnActualHwFlag == 1)
	{
		fillRssStructure(tlFailure, library);
		ptr = (UWord32*) &rssStruct;
		rssStruct.crcMsb = calculateCrc(ptr, sizeof(rssStruct)/4);
		status = DifmCsiLink::recvWord(CSI_STATUS_OFFSET);

		if ((status & 0x00000001) == 0)
		{
			printf("CSI:\tData Writing \n");
			UWord32 wordsToSend = sizeof(rssStruct)/4;
			for (k = 0; k < wordsToSend; k++)
			{
//                printf("%d, %.8x \n", k, ptr[k]);
				DifmCsiLink::sendWord(CSI_TX_OFFSET, ptr[k]);
			}
		}
	}
}

//This function reads data i.e. all the data till 64 words from CSI link
UWord8 DifmCsiLink::readCsiData()
{
	UWord32 status, rx = 0, i = 0;
	memset(&csiBuffer, 0, sizeof(csiBuffer));
	status = DifmCsiLink::recvWord(CSI_STATUS_OFFSET);
	while ((status & 0x00000002) != 2)
	{
		rx = DifmCsiLink::recvWord(CSI_RX_OFFSET);
		csiBuffer[i] = rx;
		status = DifmCsiLink::recvWord(CSI_STATUS_OFFSET);
		i++;
		if (i == 64)
		{
			break;
		}
	}
	return i;
}

//This function sends Self Test activation flag to DIFM and reads the results back
UWord8 DifmCsiLink::rfBoardSelfTestLogic(UWord32 *st, UWord8 type)
{
	UWord32 status = 0, i = 0, timeDifference = 0;
	uint64_t initTime, currentTime;

	if (type == 0)       // Modular test
	{
		DifmCsiLink::sendWord(CSI_TX_OFFSET, 1);

		//timer 500
		timeDifference = 0;
		initTime=Utility::getSystemTime();

		status = DifmCsiLink::recvWord(CSI_STATUS_OFFSET);
		while ((status & 0x00000002) != 2)
		{
			if (DifmCsiLink::recvWord(CSI_RX_OFFSET) == 2)
			{
				break;
			}
			status = DifmCsiLink::recvWord(CSI_STATUS_OFFSET);
			currentTime=Utility::getSystemTime();
			timeDifference = (currentTime - initTime) / 1000;
			if (timeDifference > 500)
			{
				break;
			}
		}
		i = readCsiData();
		printf("RFB reps: %d \n", i);
		memcpy(st, csiBuffer, i * sizeof(UWord32));
	}
	else if (type == 1)       // Functional test
	{
		DifmCsiLink::sendWord(CSI_TX_OFFSET, 2);
	}
	return i;
}
