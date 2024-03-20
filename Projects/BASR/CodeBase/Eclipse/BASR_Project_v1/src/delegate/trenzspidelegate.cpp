/*
 * trenzspidelegate.cpp
 *
 *  Created on: Apr 4, 2021
 *      Author: zumair
 */

#define TRENZSPIDELEGATE_CPP_

#include "../delegate/trenzspidelegate.h"

//This function is the brain of communication amongst Trenz and CCP
//It extracts messages from iterator, checks the delimeter and parses
//the data to appropriate class data buffers
void extractTrenzMessage(UWord8 iterator)
{
	UWord8 extractionResult = 0, messageType = 0;
	UWord8 lb = 0;
	UWord32 trenzData[10000];
	UWord16 sizeToWrite = 0;
	M1ewSop inputSop;
	Ewm1Sop transmitSop;
	M1ewMaint inputMaintenance;
	Ewm1Maint outputMaintenance;
	Ewm1Eof outputEof;
	TrenzCommunicationHeader *trenzCommunicationHeader;
	memset(&inputSop, 0, sizeof(inputSop));
	memset(&transmitSop, 0, sizeof(transmitSop));
	memset(&inputMaintenance, 0, sizeof(inputMaintenance));
	memset(&outputMaintenance, 0, sizeof(outputMaintenance));
	memset(&outputEof, 0, sizeof(outputEof));
	memcpy(&trenzData[0], &Model::trenzSpiLink.data[iterator],
			(sizeof(UWord32) * (Model::trenzSpiLink.bytesRead - iterator)));

	memset(&Model::trenzSpiLink.receiveBuffer[0], 0,
			sizeof(Model::trenzSpiLink.receiveBuffer));
	trenzCommunicationHeader = (TrenzCommunicationHeader*) &trenzData[0];

	if (trenzCommunicationHeader->delimeter != 0xdeadbeef)
	{
		printf("DelimMismatch Trenz_MessageType:%d \n",trenzCommunicationHeader->messageType);
		return;
	}
	if (trenzCommunicationHeader->messageType == 1)
	{
		sizeToWrite = 3 * sizeof(UWord16);
		sizeToWrite = sizeToWrite + sizeToWrite % 4;
		printf("SOP-Rec\n");
		if (Model::trenzSpiLink.calculateCRC((UWord8*) &trenzData[0], 14, 8)
				!= trenzCommunicationHeader->crc)
		{
			printf("T-CRC-F\n");
			return;
		}
		memcpy(&Model::trenzSpiLink.receiveBuffer[0], &trenzData[2],
				sizeToWrite);

		fillM1ewSopStructure(&sopIn);
		fillM1ewSopStructure(&inputSop);

//		if ((ConstHigh::FstInProgress == 1) && (sopIn.sop.mainMode == 2))
//		{
//			printf("\nIBIT in progress, ignored sop\n");
//			return;
//		}

		if (inputSop.sop.pflReq == 1)
		{
			printf("\nPFLReq-Rx\n");
			sendPflLogic();
		}

		if (inputSop.sop.ecm != ConstHigh::ecmMode)
		{
			printf("\n\t B - ecmMode: %d\n", ConstHigh::ecmMode);
			ConstHigh::ecmMode = inputSop.sop.ecm;
			handleCfdStatusRequest();
		}

		parseSopMessage(&inputSop, &transmitSop);

		updatePflFlagsInSopOut(transmitSop);
		Model::selfTestMflHigh.onPflStatusMessageSent();

		fillEwm1SopBuffer(&transmitSop);
		printf("\nSOP-Rec-Tx\n");
		Model::trenzSpiLink.writeReplyToTrenz(1, 5, 0);
		extractionResult = 1;
		messageType = trenzCommunicationHeader->messageType;
		//extract SOP request
	}
	else if (trenzCommunicationHeader->messageType == 2)
	{
		sizeToWrite = 5 * sizeof(UWord16);
		sizeToWrite = sizeToWrite + sizeToWrite % 4;
		printf("SOP-Reply-Rec\n");
		if (Model::trenzSpiLink.calculateCRC((UWord8*) &trenzData[0], 18, 8)
				!= trenzCommunicationHeader->crc)
		{
			printf("T-CRC-F\n");
			return;
		}
		memcpy(&Model::trenzSpiLink.receiveBuffer[0], &trenzData[2],
				sizeToWrite);
		extractionResult = 1;
		messageType = trenzCommunicationHeader->messageType;
		//log this recieve buffer 10 bytes into FR
		//extract SOP reply
	}
	else if (trenzCommunicationHeader->messageType == 3)
	{
		sizeToWrite = 3 * sizeof(UWord16);
		sizeToWrite = sizeToWrite + sizeToWrite % 4;
		printf("Maint-Rec\n");
		if (Model::trenzSpiLink.calculateCRC((UWord8*) &trenzData[0], 14, 8)
				!= trenzCommunicationHeader->crc)
		{
			printf("T-CRC-F\n");
			return;
		}
		memcpy(&Model::trenzSpiLink.receiveBuffer[0], &trenzData[2],
				sizeToWrite);
		memset(&outputMaintenance, 0, sizeof(outputMaintenance));
		fillM1ewMaintStructure(&inputMaintenance);
		lb = parseInputMaintenanceMessage(&inputMaintenance,
				&outputMaintenance);

		fillMflData(&outputMaintenance, lb);
		fillEwm1MaintBuffer(&outputMaintenance, lb);

		Model::fileoutputflightrecord.writeEncryptedEventFlightRecord(4, 60,
				(UWord8*) &Model::trenzSpiLink.transmitBuffer[0],
				&Model::libraryMeta, ConstHigh::flightRecordPathCurrent,
				Model::memoryFailure);
		Model::trenzSpiLink.writeReplyToTrenz(3, 29, 0);

		extractionResult = 1;
		messageType = trenzCommunicationHeader->messageType;
		//extract maintenance in
	}
	else if (trenzCommunicationHeader->messageType == 4)
	{
		printf("Maint-R-Rec\n");
		if (Model::trenzSpiLink.calculateCRC((UWord8*) &trenzData[0], 66, 8)
				!= trenzCommunicationHeader->crc)
		{
			printf("T-CRC-F\n");
			return;
		}

		updateMflPresentFlag();
		maintOut.mflStatus.status = mflPresent;
		if (flightRecordMemoryPercentageToBcd() > 0)
		{
			maintOut.tgtStatus.exists = 1;
		}
		else
		{
			maintOut.tgtStatus.exists = 0;
		}

		fillEwm1MaintBuffer(&maintOut, 1);
		Model::trenzSpiLink.writeReplyToTrenz(3, 29, 0);
		Model::trenzSpiLink.spiOption = -1;

//        printf("\nmaint st sent\n");

		//memcpy(&Model::trenzSpiLink.receiveBuffer[0],&trenzData[2],29*sizeof(UWord16));
		//log this recieve buffer 58 bytes into FR from above
		//extract maintenance reply
	}
	else if (trenzCommunicationHeader->messageType == 5)
	{
		//printf("RTC_Rec\n");
		if (Model::trenzSpiLink.calculateCRC((UWord8*) &trenzData[0], 12, 8)
				!= trenzCommunicationHeader->crc)
		{
			printf("T-CRC-F\n");
			return;
		}
		Model::fileoutputflightrecord.RTC_TIME = trenzData[2];
		//extract rtc time no need to write in FR
	}
	else if (trenzCommunicationHeader->messageType == 6)
	{
//        printf("\n A_R_Rec \n \n");
		ConstHigh::spiAudioRequest = 1;
		//extract audio request, no need to save in FR
	}
	else if (trenzCommunicationHeader->messageType == 7)
	{
		sizeToWrite = 2 * sizeof(UWord16);
		sizeToWrite = sizeToWrite + sizeToWrite % 4;
		printf("PFL_Rec\n");
		if (Model::trenzSpiLink.calculateCRC((UWord8*) &trenzData[0], 12, 8)
				!= trenzCommunicationHeader->crc)
		{
			printf("T-CRC-F\n");
			return;
		}
		memcpy(&Model::trenzSpiLink.receiveBuffer[0], &trenzData[2],
				sizeToWrite);
		extractionResult = 1;
		messageType = trenzCommunicationHeader->messageType;
		//write into FR
	}
	else if (trenzCommunicationHeader->messageType == 8)
	{
		printf("S-EOF-2 \n");
		Model::trenzSpiLink.spiOption = 14;
		Model::trenzSpiLink.spiSubOption = 2;
		memset(&outputEof, 0, sizeof(outputEof));
		fillEofData(&outputEof);
		fillEwm1EofBuffer(&outputEof);
		Model::trenzSpiLink.writeReplyToTrenz(8, 29, 0);
		//no need in FR
	}
	else if (trenzCommunicationHeader->messageType == 10)
	{
		sizeToWrite = 1 * sizeof(UWord16);
		sizeToWrite = sizeToWrite + sizeToWrite % 4;
		printf("Tr_ST_Rec\n");
		if (Model::trenzSpiLink.calculateCRC((UWord8*) &trenzData[0], 10, 8)
				!= trenzCommunicationHeader->crc)
		{
			printf("T-CRC-F\n");
			return;
		}
		if (trenzData[2] == 1)
		{
			trenzSelfStarted = 1;
		}
		else if (trenzData[2] == 2)
		{
			trenzSelfFinished = 1;
		}
		memcpy(&Model::trenzSpiLink.receiveBuffer[0], &trenzData[2],
				sizeToWrite);
		extractionResult = 1;
		messageType = trenzCommunicationHeader->messageType;
		//write in FR for both start and end. Stop pending
	}
	else if (trenzCommunicationHeader->messageType == 11)
	{
		printf("Tr-P_ST_Rec\n");
//        if(calculateCRC((UWord8*)&trenzData[0],8,8) != trenzCommunicationHeader->crc)
//        {
//            printf("T-CRC-F\n");
//            return;
//        }
		memcpy((void*) &Model::stDelegate.stData[0], (void*) &trenzData[2],
				CPF_ERROR_COUNT * sizeof(UWord32));

		Model::stDelegate.trenzStResults = 1;
		//no need, covered in mfl
	}
	else if (trenzCommunicationHeader->messageType == 12)
	{
		sizeToWrite = 25 * sizeof(UWord16);
		sizeToWrite = sizeToWrite + sizeToWrite % 4;
		printf("CFDProg-Rec\n");
		if (Model::trenzSpiLink.calculateCRC((UWord8*) &trenzData[0], 58, 8)
				!= trenzCommunicationHeader->crc)
		{
			printf("T-CRC-F\n");
			return;
		}
		memcpy(&Model::trenzSpiLink.receiveBuffer[0], &trenzData[2],
				sizeToWrite);

		fillM1ewCfdProgramStructure(&cfdProgramIn);

		handleCfdCommunication(cfdProgramIn);

		//send ACK/NAK via ASYNC SOP

		extractionResult = 1;
		messageType = trenzCommunicationHeader->messageType;
	}
	else if (trenzCommunicationHeader->messageType == 13)
	{
		sizeToWrite = 2 * sizeof(UWord16);
		sizeToWrite = sizeToWrite + sizeToWrite % 4;
		printf("CFDReq-Rec\n");
		if (Model::trenzSpiLink.calculateCRC((UWord8*) &trenzData[0], 12, 8)
				!= trenzCommunicationHeader->crc)
		{
			printf("T-CRC-F\n");
			return;
		}
		memcpy(&Model::trenzSpiLink.receiveBuffer[0], &trenzData[2],
				sizeToWrite);

		fillM1ewCfdRequestStructure(&cfdRequestIn);
		if (cfdRequestIn.cfdDataRequest.progListRequest == 1)
		{
			handleCfdListRequest(cfdRequestIn, cfdProgramOut);
			fillEwm1CfdProgramBuffer(&cfdProgramOut);

			Model::trenzSpiLink.writeReplyToTrenz(12, 30, 0);
		}
		else
		{
			handleCfdCommunication(cfdRequestIn, cfdRequestOut);
			fillEwm1CfdRequestBuffer(&cfdRequestOut);

			Model::trenzSpiLink.writeReplyToTrenz(13, 3, 0);
		}
		extractionResult = 1;
		messageType = trenzCommunicationHeader->messageType;
	}
	else if (trenzCommunicationHeader->messageType == 16)
	{
		Model::trenzSpiLink.spiOption = 12; // Trenz restarted, asking for Power on BIT
	}
	else
	{
		printf("Trenz_MessageType:%d \n",trenzCommunicationHeader->messageType);
		return;
	}
	if (extractionResult == 1)
	{
		Model::fileoutputflightrecord.writeEncryptedEventFlightRecord(
				messageType, sizeToWrite,
				(UWord8*) &Model::trenzSpiLink.receiveBuffer[0],
				&Model::libraryMeta, ConstHigh::flightRecordPathCurrent,
				Model::memoryFailure);
	}
}

//This function fills Output Maintainance structure from data extracted from
//SPI mfl structure
void fillMflData(Ewm1Maint *outputMaintenance, UWord8 lb)
{
	if (lb == 3)
	{
		mflMessagesProcess();
		outputMaintenance->mflBlockId.flrExists =
				Model::selfTestMflHigh.spiMfl.mfl.exists;
		outputMaintenance->mflBlockId.lastBlock =
				Model::selfTestMflHigh.spiMfl.lastBlock;
		outputMaintenance->sysSubsys.sys1 =
				Model::selfTestMflHigh.spiMfl.mfl.sys1;
		outputMaintenance->sysSubsys.sys2 =
				Model::selfTestMflHigh.spiMfl.mfl.sys2;
		outputMaintenance->sysSubsys.subsys =
				Model::selfTestMflHigh.spiMfl.mfl.subsys;
		outputMaintenance->lruFctn.lru1 =
				Model::selfTestMflHigh.spiMfl.mfl.lru[0];
		outputMaintenance->lruFctn.lru2 =
				Model::selfTestMflHigh.spiMfl.mfl.lru[1];
		outputMaintenance->lruFctn.lru3 =
				Model::selfTestMflHigh.spiMfl.mfl.lru[2];
		outputMaintenance->lruFctn.fctn1 =
				Model::selfTestMflHigh.spiMfl.mfl.fctn1;
		outputMaintenance->lruFctn.fctn2 =
				Model::selfTestMflHigh.spiMfl.mfl.fctn2;
		outputMaintenance->amount.count =
				Model::selfTestMflHigh.spiMfl.mfl.amount;
		outputMaintenance->occurTime.occurance =
				Model::selfTestMflHigh.spiMfl.mfl.occurTime;
		outputMaintenance->durationTime.duration =
				Model::selfTestMflHigh.spiMfl.mfl.durationTime;
		memcpy(&outputMaintenance->description.description[0],
				&Model::selfTestMflHigh.spiMfl.mfl.description[0],
				40 * sizeof(UWord8));
	}
}

void updateMflPresentFlag()
{
	FILE *mflFile;
	UWord8 read = 0;
	UWord16 i = 0;
	if (ConstHigh::simOnActualHwFlag == 1)
	{
		mflFile = fopen("/tmp/emmc/mfl/current.txt", "rb");
	}
	else
	{
		mflFile = fopen("cd .txt", "rb");
	}
	if (mflFile != NULL)
	{
		mflPresent = 0;
		for (i = 0; i < TOTAL_ERROR_COUNT; i++)
		{
			fseek(mflFile, (i) * 2, SEEK_SET);
			fread(&read, 1, 1, mflFile);
			faultArray[i] = read;
			if (read == '1')
			{
				mflPresent = 1;
			}
		}
		fclose(mflFile);
	}
}
//This function sends MFL to Trenz according to /tmp/emmc/mfl/current.txt file
void mflMessagesProcess()
{
	UWord8 mflSent = 0;
	if (Model::trenzSpiLink.spiOption == 14)
	{
		if (Model::trenzSpiLink.spiSubOption == 0)
		{
			startPoint = 0;
			for (startPoint = 0; startPoint < TOTAL_ERROR_COUNT; startPoint++)
			{
				if (faultArray[startPoint] == '1')
				{
					lastOne = startPoint;
				}
			}
			startPoint = 0;
		}
		if (Model::trenzSpiLink.spiSubOption == 1)
		{
			if (mflPresent == 1)
			{
				startPoint = 0;
				for (startPoint = 0; startPoint < TOTAL_ERROR_COUNT;
						startPoint++)
				{
					if (faultArray[startPoint] == '1')
					{
						sendMflFault(startPoint, lastOne);
						startPoint++;
						break;
					}
				}
			}
			else
			{
				memset(&Model::selfTestMflHigh.spiMfl, 0,
						sizeof(Model::selfTestMflHigh.spiMfl));
				Model::selfTestMflHigh.spiMfl.lastBlock = 1;
			}
		}
		else if (Model::trenzSpiLink.spiSubOption == 2)
		{
			for (; startPoint < TOTAL_ERROR_COUNT; startPoint++)
			{
				if (faultArray[startPoint] == '1')
				{
					mflSent = 1;
					sendMflFault(startPoint, lastOne);
					startPoint++;
					break;
				}
			}
			if (mflSent == 0)
			{
				memset(&Model::selfTestMflHigh.spiMfl, 0,
						sizeof(Model::selfTestMflHigh.spiMfl));
				Model::selfTestMflHigh.spiMfl.lastBlock = 1;
				//sendMfl(&spiMfl);
			}
		}
		else if (Model::trenzSpiLink.spiSubOption == 3)
		{
			startPoint = 0;
		}
		Model::trenzSpiLink.spiOption = -1;
	}
}

//This function parses SOP messages from Trenz to Output SOP
//and takes relevant actions as instructed by SOP messages
void parseSopMessage(M1ewSop *inputSop, Ewm1Sop *outputSop)
{
	UWord16 cfCardPercentage = 0;
	Ewm1Eof outputEof;
	memset(outputSop, 0, sizeof(Ewm1Sop));
	if (inputSop->sop.mainMode == 1)
	{
		Model::trenzSpiLink.spiOption = 10;
		//normal processing
	}
	else if (inputSop->sop.mainMode == 2)
	{
		Model::trenzSpiLink.spiOption = 12;
		//IBIT
	}
	else if (inputSop->sop.mainMode == 3)
	{
		Model::trenzSpiLink.spiOption = -1;
		//maintenance
	}
//    if(inputSop->sop.standby == 1)
//    {
//        distanceType = 1;
//    }
//    if(inputSop->sop.standby == 0)
//    {
//        distanceType = 0;
//    }
	if (inputSop->sop.eof != 0)
	{
		printf("S-EOF-1 \n");
		Model::trenzSpiLink.spiOption = 14;
		Model::trenzSpiLink.spiSubOption = 0;
		mflMessagesProcess();
		Model::trenzSpiLink.spiOption = 14;
		Model::trenzSpiLink.spiSubOption = 1;
		memset(&outputEof, 0, sizeof(outputEof));
		fillEofData(&outputEof);
		fillEwm1EofBuffer(&outputEof);
		Model::trenzSpiLink.writeReplyToTrenz(8, 29, 0);
		//MFL logic
	}
	//    if(inputSop->sop.libraryId != 0)
//    {
//        if(inputSop->sop.libraryId > 7)
//        {
//
//        }
//        else if(inputSop->sop.libraryId == 7)
//        {
//            for(i=0; i<6; i++)
//            {
//                if(threatLibraryPresentInEmmc[i] == 1)
//                {
//                    outputSop->status2Sop.libsStatus[i] = 1;
//                }
//            }
//        }
//        else if(threatLibraryPresentInEmmc[inputSop->sop.libraryId-1] == 1)
//        {
//            copySelectedThreatLibraryFromLoadedLibrariesOnTrenzCommand(inputSop->sop.libraryId);
//        }
//        //defaultLibrary = 0;
//        //TL logic
//    }
	cfCardPercentage = flightRecordMemoryPercentageToBcd();

	outputSop->status1Sop.mainMode = inputSop->sop.mainMode;
	outputSop->status1Sop.eof = inputSop->sop.eof;
	outputSop->status1Sop.threatNum = inputSop->sop.threatNum;
	outputSop->status1Sop.ecm = inputSop->sop.ecm;
	outputSop->status1Sop.audio = inputSop->sop.audio;
	outputSop->status1Sop.blanking = inputSop->sop.blanking;
	outputSop->status1Sop.standby = inputSop->sop.standby;
	outputSop->status1Sop.dateNotSet = 0;

	Model::ptyEnabled = inputSop->sop.threatNum;

//    outputSop->status2Sop.defaultLibrary     = defaultLibrary;
	outputSop->status2Sop.recordingMemStatus = cfCardPercentage;

	outputSop->visWrnReq.beingTracked = ConstHigh::sopBeingTracked;
	outputSop->visWrnReq.missileLaunch = ConstHigh::sopMissileLaunch;
	outputSop->visWrnReq.threatFound = ConstHigh::sopThreatFound;

	sopOut.status1Sop.mainMode = inputSop->sop.mainMode;
	sopOut.status1Sop.eof = inputSop->sop.eof;
	sopOut.status1Sop.threatNum = inputSop->sop.threatNum;
	sopOut.status1Sop.ecm = inputSop->sop.ecm;
	sopOut.status1Sop.audio = inputSop->sop.audio;
	sopOut.status1Sop.blanking = inputSop->sop.blanking;
	sopOut.status1Sop.standby = inputSop->sop.standby;
	sopOut.status1Sop.dateNotSet = 0;

//    sopOut.status2Sop.defaultLibrary     = defaultLibrary;
	sopOut.status2Sop.recordingMemStatus = cfCardPercentage;

	sopOut.visWrnReq.beingTracked = ConstHigh::sopBeingTracked;
	sopOut.visWrnReq.missileLaunch = ConstHigh::sopMissileLaunch;
	sopOut.visWrnReq.threatFound = ConstHigh::sopThreatFound;
}

//This function fills EoF data structure from SPI data
void fillEofData(Ewm1Eof *outputEof)
{
	mflMessagesProcess();
	outputEof->lastBlock.lastBlock = Model::selfTestMflHigh.spiMfl.lastBlock;
	outputEof->sysSubsys.sys1 = Model::selfTestMflHigh.spiMfl.mfl.sys1;
	outputEof->sysSubsys.sys2 = Model::selfTestMflHigh.spiMfl.mfl.sys2;
	outputEof->sysSubsys.subsys = Model::selfTestMflHigh.spiMfl.mfl.subsys;
	outputEof->lruFctn.lru1 = Model::selfTestMflHigh.spiMfl.mfl.lru[0];
	outputEof->lruFctn.lru2 = Model::selfTestMflHigh.spiMfl.mfl.lru[1];
	outputEof->lruFctn.lru3 = Model::selfTestMflHigh.spiMfl.mfl.lru[2];
	outputEof->lruFctn.fctn1 = Model::selfTestMflHigh.spiMfl.mfl.fctn1;
	outputEof->lruFctn.fctn2 = Model::selfTestMflHigh.spiMfl.mfl.fctn2;
	outputEof->amount.count = Model::selfTestMflHigh.spiMfl.mfl.amount;
	outputEof->occurTime.occurance =
			Model::selfTestMflHigh.spiMfl.mfl.occurTime;
	outputEof->durationTime.duration =
			Model::selfTestMflHigh.spiMfl.mfl.durationTime;
	memcpy(&outputEof->description.description[0],
			&Model::selfTestMflHigh.spiMfl.mfl.description[0],
			40 * sizeof(UWord8));
}

//This function finds the percentage of Flight Record already written
//and converts it to BCD according to its percentage
UWord8 flightRecordMemoryPercentageToBcd()
{
	UWord8 bcdReturn = 0, *dummy = NULL;
	UWord32 percentage = 0;
	if (Model::fileoutputflightrecord.totalFlightRecordSize == 0)
	{
		//this is used just to get the size of FR present
		//no data will be written here
		Model::fileoutputflightrecord.encryptAndWriteToFile(dummy, 0,
				&Model::libraryMeta, ConstHigh::flightRecordPathCurrent,
				Model::memoryFailure);
		//bcdReturn = 0;
	}
	if (Model::fileoutputflightrecord.totalFlightRecordSize == 0)
	{
		return 0;
	}
//	printf("U: %llu T: %llu\n",
//			Model::fileoutputflightrecord.usedFlightRecordSize,
//			Model::fileoutputflightrecord.totalFlightRecordSize);
	percentage = (Model::fileoutputflightrecord.usedFlightRecordSize * 100)
			/ Model::fileoutputflightrecord.totalFlightRecordSize;
	if (((percentage / 10) == 0)
			&& (Model::fileoutputflightrecord.usedFlightRecordSize > 0)
			&& (Model::fileoutputflightrecord.flightRecordOverflowFlag == 0))
	{
		bcdReturn = 1;
	}
	else if (Model::fileoutputflightrecord.flightRecordOverflowFlag == 0)
	{
		bcdReturn = percentage / 10;
	}
	else
	{
		if ((percentage <= 20) && (percentage >= 0))
		{
			bcdReturn = 11;
		}
		else if ((percentage <= 40) && (percentage > 20))
		{
			bcdReturn = 12;
		}
		else if ((percentage <= 60) && (percentage > 40))
		{
			bcdReturn = 13;
		}
		else if ((percentage <= 80) && (percentage > 60))
		{
			bcdReturn = 14;
		}
		else if ((percentage <= 100) && (percentage > 80))
		{
			bcdReturn = 15;
		}
	}
//    printf("P: %d Sent: %d\n",percentage,bcdReturn);
	return bcdReturn;
}

//This function finds the mfl file to be opened and sends it to Trenz
void sendMflFault(UWord16 count, UWord8 lastOne)
{
	FILE *ptr;
	Word8 filename[30];
	UWord8 lru = 0, sru = 0, subModule = 0, faultId = 0;
	count++;
	lastOne++;
	if (count <= CPF_ERROR_COUNT)
	{
		lru = 1;
		sru = 1;
		subModule = 1;
		faultId = count;
	}
	else if (count <= CPF_ERROR_COUNT + PPF_ERROR_COUNT)
	{
		lru = 1;
		sru = 1;
		subModule = 2;
		faultId = count - CPF_ERROR_COUNT;
	}
	else if (count <= CPF_ERROR_COUNT + PPF_ERROR_COUNT + CCP_ERROR_COUNT)
	{
		lru = 1;
		sru = 1;
		subModule = 3;
		faultId = count - (CPF_ERROR_COUNT + PPF_ERROR_COUNT);
	}
	else if (count
			<= CPF_ERROR_COUNT + PPF_ERROR_COUNT + CCP_ERROR_COUNT
					+ TP_ERROR_COUNT)
	{
		lru = 1;
		sru = 2;
		subModule = 0;
		faultId = count - (CCP_ERROR_COUNT + CPF_ERROR_COUNT + PPF_ERROR_COUNT);
	}
	else if (count
			<= CPF_ERROR_COUNT + PPF_ERROR_COUNT + CCP_ERROR_COUNT
					+ TP_ERROR_COUNT + DIFM_ERROR_COUNT)
	{
		lru = 1;
		sru = 3;
		subModule = 1;
		faultId = count
				- (TP_ERROR_COUNT + CCP_ERROR_COUNT + CPF_ERROR_COUNT
						+ PPF_ERROR_COUNT);
	}
	else if (count
			<= CPF_ERROR_COUNT + PPF_ERROR_COUNT + CCP_ERROR_COUNT
					+ TP_ERROR_COUNT + DIFM_ERROR_COUNT + CWP_ERROR_COUNT)
	{
		lru = 1;
		sru = 3;
		subModule = 2;
		faultId = count
				- (DIFM_ERROR_COUNT + TP_ERROR_COUNT + CCP_ERROR_COUNT
						+ CPF_ERROR_COUNT + PPF_ERROR_COUNT);
	}
	else if (count
			<= CPF_ERROR_COUNT + PPF_ERROR_COUNT + CCP_ERROR_COUNT
					+ TP_ERROR_COUNT + DIFM_ERROR_COUNT + CWP_ERROR_COUNT
					+ DCM_ERROR_COUNT)
	{
		lru = 1;
		sru = 4;
		subModule = 0;
		faultId = count
				- (CWP_ERROR_COUNT + DIFM_ERROR_COUNT + TP_ERROR_COUNT
						+ CCP_ERROR_COUNT + CPF_ERROR_COUNT + PPF_ERROR_COUNT);
	}
	else if (count
			<= CPF_ERROR_COUNT + PPF_ERROR_COUNT + CCP_ERROR_COUNT
					+ TP_ERROR_COUNT + DIFM_ERROR_COUNT + CWP_ERROR_COUNT
					+ DCM_ERROR_COUNT + DRU_ERROR_COUNT)
	{
		lru = 2;
		sru = 0;
		subModule = 0;
		faultId = count
				- (DCM_ERROR_COUNT + CWP_ERROR_COUNT + DIFM_ERROR_COUNT
						+ TP_ERROR_COUNT + CCP_ERROR_COUNT + CPF_ERROR_COUNT
						+ PPF_ERROR_COUNT);
	}
	sprintf(filename, "/tmp/emmc/mfl/%d-%d-%d-%d.mfl", lru, sru, subModule,
			faultId);
	ptr = fopen(filename, "rb");
	if (ptr != NULL)
	{
		memset(&Model::selfTestMflHigh.mfl, 0, sizeof(MflStructure));
		fread((UWord8*) &Model::selfTestMflHigh.mfl, sizeof(MflStructure), 1,
				ptr);
		memcpy(&Model::selfTestMflHigh.spiMfl.mfl, &Model::selfTestMflHigh.mfl,
				sizeof(MflStructure));
		fclose(ptr);
		if (count == lastOne)
		{
			Model::selfTestMflHigh.spiMfl.lastBlock = 1;
		}
		else
		{
			Model::selfTestMflHigh.spiMfl.lastBlock = 0;
		}
		//sendMfl(&spiMfl);
	}
}

void updatePflFlagsInSopOut(Ewm1Sop &transmitSop)
{

	transmitSop.status1Sop.newPfl3 = Model::selfTestMflHigh.newPfl3;
	transmitSop.status1Sop.newPfl4 = Model::selfTestMflHigh.newPfl4;
	transmitSop.status1Sop.pfl3 = Model::selfTestMflHigh.existencePfl3;
	transmitSop.status1Sop.pfl4 = Model::selfTestMflHigh.existencePfl4;
	sopOut.status1Sop.pfl3 = transmitSop.status1Sop.pfl3;
	sopOut.status1Sop.pfl4 = transmitSop.status1Sop.pfl4;
	sopOut.status1Sop.newPfl3 = transmitSop.status1Sop.newPfl3;
	sopOut.status1Sop.newPfl4 = transmitSop.status1Sop.newPfl4;
	sopOut.ackNak.direct = transmitSop.ackNak.direct;
	sopOut.ackNak.pBlock = transmitSop.ackNak.pBlock;
	sopOut.ackNak.lBlock = transmitSop.ackNak.lBlock;
	sopOut.ackNak.ackNakStatus = transmitSop.ackNak.ackNakStatus;


}
void onBitCompletionLogic()
{
	Ewm1Sop transmitSop;

	Model::selfTestMflHigh.updatePflFlags(Model::stDelegate.selfTestType);
	if(Model::stDelegate.selfTestType == 1 || Model::stDelegate.selfTestType == 2)
	{ // auto self tests
		if(Model::selfTestMflHigh.pflRelatedAsyncSopNeededFlag==1)
		{
			updatePflFlagsInSopOut(transmitSop);
			timeStampLastAyncSop = Utility::getSystemTime();

			fillSopOutStructure(&sopIn, &transmitSop);
			transmitSop.status1Sop.mainMode = 1;
			transmitSop.ackNak.ackNakStatus = 3;
			transmitSop.ackNak.direct = 0;
			transmitSop.ackNak.pBlock = 0;
			transmitSop.ackNak.lBlock = 0;

			

			fillEwm1SopBuffer(&transmitSop);
			printf("Sending PFL Update on AutoBIT");
			Model::trenzSpiLink.writeReplyToTrenz(1, 5, 0);
			Model::selfTestMflHigh.onPflStatusMessageSent();
		}
		Model::trenzSpiLink.spiOption =10;
	}
	else if(Model::stDelegate.selfTestType == 3)
	{// IBIT end
		updatePflFlagsInSopOut(transmitSop);
		timeStampLastAyncSop = Utility::getSystemTime();
		fillSopOutStructure(&sopIn, &transmitSop);
		transmitSop.status1Sop.mainMode = 1;
		transmitSop.ackNak.ackNakStatus = 3;
		transmitSop.ackNak.direct = 0;
		transmitSop.ackNak.pBlock = 0;
		transmitSop.ackNak.lBlock = 0;

		

		fillEwm1SopBuffer(&transmitSop);
		printf("Sending PFL Update on IBIT");
		Model::trenzSpiLink.writeReplyToTrenz(1, 5, 0);
		Model::selfTestMflHigh.onPflStatusMessageSent();
		Model::trenzSpiLink.spiOption = 10;
	}
	else if(Model::stDelegate.selfTestType == 4)
	{// MBIT Done
		maintOut.block.blockId = 2;
		maintOut.mbitStatus.status = 2;
		fillEwm1MaintBuffer(&maintOut, 2);
		Model::trenzSpiLink.writeReplyToTrenz(3, 29, 0);
		updateMflPresentFlag();
		mflMessagesProcess();
		usleep(150000);

		maintOut.block.blockId = 1;
		maintOut.mflStatus.status = mflPresent;
		maintOut.swVersion.majorVer[0] = ConstHigh::SW_VER_MAJ_1;
		maintOut.swVersion.majorVer[1] = ConstHigh::SW_VER_MAJ_2;
		maintOut.swVersion.minorVer[0] = ConstHigh::SW_VER_MIN_1;
		maintOut.swVersion.minorVer[1] = ConstHigh::SW_VER_MIN_2;
		maintOut.year.year = ConstHigh::SW_VER_YEAR;
		maintOut.month.month = ConstHigh::SW_VER_MONTH;
		maintOut.day.day = ConstHigh::SW_VER_DATE;

		if (flightRecordMemoryPercentageToBcd() > 0)
		{
			maintOut.tgtStatus.exists = 1;
		}
		else
		{
			maintOut.tgtStatus.exists = 0;
		}

		fillEwm1MaintBuffer(&maintOut, 1);
		Model::trenzSpiLink.writeReplyToTrenz(3, 29, 0);
	}
	Model::stDelegate.selfTestType = 0;
	Model::stDelegate.selfTestActiveFlag = 0;
}

//This function fills output sop structure from input sop structure
void fillSopOutStructure(M1ewSop *inputSop, Ewm1Sop *outputSop)
{

	UWord16 cfCardPercentage = 0;

//    if(inputSop->sop.libraryId != 0)
//    {
//        if(inputSop->sop.libraryId > 7)
//        {
//
//        }
//        else if(inputSop->sop.libraryId == 7)
//        {
//            for(i=0; i<6; i++)
//            {
//                if(threatLibraryPresentInEmmc[i] == 1)
//                {
//                    outputSop->status2Sop.libsStatus[i] = 1;
//                }
//            }
//        }
//        else if(threatLibraryPresentInEmmc[inputSop->sop.libraryId-1] == 1)
//        {
//            copySelectedThreatLibraryFromLoadedLibrariesOnTrenzCommand(inputSop->sop.libraryId);
//        }
//        //defaultLibrary = 0;
//        //TL logic
//    }
	cfCardPercentage = flightRecordMemoryPercentageToBcd();

	outputSop->status1Sop.eof = inputSop->sop.eof;
	outputSop->status1Sop.threatNum = inputSop->sop.threatNum;
	outputSop->status1Sop.ecm = inputSop->sop.ecm;
	outputSop->status1Sop.audio = inputSop->sop.audio;
	outputSop->status1Sop.blanking = inputSop->sop.blanking;
	outputSop->status1Sop.standby = inputSop->sop.standby;
	outputSop->status1Sop.dateNotSet = 0;

//    outputSop->status2Sop.defaultLibrary     = defaultLibrary;
	outputSop->status2Sop.recordingMemStatus = cfCardPercentage;

	outputSop->visWrnReq.beingTracked = ConstHigh::sopBeingTracked;
	outputSop->visWrnReq.missileLaunch = ConstHigh::sopMissileLaunch;
	outputSop->visWrnReq.threatFound = ConstHigh::sopThreatFound;
	if (inputSop->sop.ecm == 0)
	{
		outputSop->status1Sop.ecm = ConstHigh::ecmMode;
	}

	sopOut.status1Sop.mainMode = inputSop->sop.mainMode;
	sopOut.status1Sop.eof = inputSop->sop.eof;
	sopOut.status1Sop.threatNum = inputSop->sop.threatNum;
	sopOut.status1Sop.ecm = inputSop->sop.ecm;
	sopOut.status1Sop.audio = inputSop->sop.audio;
	sopOut.status1Sop.blanking = inputSop->sop.blanking;
	sopOut.status1Sop.standby = inputSop->sop.standby;
	sopOut.status1Sop.dateNotSet = 0;

//    sopOut.status2Sop.defaultLibrary     = defaultLibrary;
	sopOut.status2Sop.recordingMemStatus = cfCardPercentage;

	sopOut.visWrnReq.beingTracked = ConstHigh::sopBeingTracked;
	sopOut.visWrnReq.missileLaunch = ConstHigh::sopMissileLaunch;
	sopOut.visWrnReq.threatFound = ConstHigh::sopThreatFound;
	if (inputSop->sop.ecm == 0)
	{
		sopOut.status1Sop.ecm = ConstHigh::ecmMode;
	}
}

//This function is the main logic of Modular Test of Trenz
//Here, first we initiate self test, wait for its completion
//and eventually extract its results to respective object
void SpiSelftTestPbitLogic(UWord32 *input)
{
	UWord8 extractionResult = 0, i = 0;
	UWord32 timeDifference = 0;
	uint64_t initTime, currentTime;
	trenzSelfStarted = 0;
	trenzSelfFinished = 0;
	//initiate self test to trenz

	printf("waiting for ST-Init\n");
	Model::trenzSpiLink.writeReplyToTrenz(10, 1, 1);
	//timer 50
	timeDifference = 0;
	initTime = Utility::getSystemTime();

	while (1)
	{
		Model::trenzSpiLink.interProcessorCommunicationLogic();
		//printf("%d\n",spiCommand.options);
		//printf("%d\n",spiCommand.opcode);
		if (trenzSelfStarted == 1)
		{
			break;
		}

		currentTime = Utility::getSystemTime();
		timeDifference = (currentTime - initTime) / 1000;

		if (timeDifference > 50)
		{
			break;
		}
	}
	//printf("%d\n",spiCommand.options);
	//printf("%d\n",spiCommand.opcode);
	printf("SPI:\tST Init\n");
	printf("waiting for ST-Done\n");

	initTime = Utility::getSystemTime();
	while (1)
	{
		if (timeDifference > 50)
		{
			break;
		}
		timeDifference = 0;
		Model::trenzSpiLink.interProcessorCommunicationLogic();
		//printf("%d\n",spiCommand.options);
		//printf("%d\n",spiCommand.opcode);
		if (trenzSelfFinished == 1)
		{
			break;
		}

		currentTime = Utility::getSystemTime();
		timeDifference = (currentTime - initTime) / 1000;

	}
	//printf("%d\n",spiCommand.options);
	//printf("%d\n",spiCommand.opcode);
	printf("SPI:\tST Done\n");
	Model::trenzSpiLink.writeReplyToTrenz(10, 1, 2);
	timeDifference = 0;
	initTime = Utility::getSystemTime();
	while (Model::trenzSpiLink.bytesRead < CPF_ERROR_COUNT)
	{
		Model::trenzSpiLink.bytesRead = 0;
		memset(Model::trenzSpiLink.data, 0, sizeof(Model::trenzSpiLink.data));
		Model::trenzSpiLink.readTrenzCommandFromSpi();
		currentTime = Utility::getSystemTime();
			timeDifference = (currentTime - initTime) / 1000;
		if (timeDifference > 50)
		{
			break;
		}
	}
	if (Model::trenzSpiLink.bytesRead >= CPF_ERROR_COUNT)
	{
		extractionResult = extractSelfTestResults();
	}
	if (extractionResult == 1)
	{
		for (i = 0; i < CPF_ERROR_COUNT; i++)
		{
			input[i] = spiSelfTestResults.faultData[i];
		}
	}
	trenzSelfStarted = 0;
	trenzSelfFinished = 0;
}

//This function parses maintenance messages to relevant class objects and
//returns reply type to be sent to Trenz
Word32 parseInputMaintenanceMessage(M1ewMaint *maintenance,
		Ewm1Maint *outputMaintenance)
{
	Word32 replyType = 0, i = 0;
	if (maintenance->block.blockId == 1)
	{
		if (maintenance->reqEw.mflReq == 1)
		{
			replyType = 3;
			if (maintenance->reqEw.step == 1)
			{
				Model::trenzSpiLink.spiOption = 14;
				Model::trenzSpiLink.spiSubOption = 2;
			}
			else
			{
				Model::trenzSpiLink.spiOption = 14;
				Model::trenzSpiLink.spiSubOption = 1;
			}
		}
		else if (maintenance->reqEw.mflReq == 0)
		{
			Model::trenzSpiLink.spiOption = 14;
			Model::trenzSpiLink.spiSubOption = 0;
			
			mflMessagesProcess();
			outputMaintenance->mflStatus.status = mflPresent;

			outputMaintenance->swVersion.majorVer[0] = ConstHigh::SW_VER_MAJ_1;
			outputMaintenance->swVersion.majorVer[1] = ConstHigh::SW_VER_MAJ_2;
			outputMaintenance->swVersion.minorVer[0] = ConstHigh::SW_VER_MIN_1;
			outputMaintenance->swVersion.minorVer[1] = ConstHigh::SW_VER_MIN_2;
			outputMaintenance->year.year = ConstHigh::SW_VER_YEAR;
			outputMaintenance->month.month = ConstHigh::SW_VER_MONTH;
			outputMaintenance->day.day = ConstHigh::SW_VER_DATE;

			if (flightRecordMemoryPercentageToBcd() > 0)
			{
				outputMaintenance->tgtStatus.exists = 1;
			}
			else
			{
				outputMaintenance->tgtStatus.exists = 0;
			}
			if (maintenance->reqEw.mflClear == 1)
			{
				Model::trenzSpiLink.spiOption = 15;
				outputMaintenance->mflStatus.status = 0;
				mflClearProcess();
				mflPresent = 0;

				//to do
			}
//            printf("\nMaint Status\n");
		}
	}
	else if (maintenance->block.blockId == 2)
	{
		if (maintenance->mbit.mbit == 0)
		{
			ConstHigh::StopFst = 1;
			Model::trenzSpiLink.spiOption = -1;
			outputMaintenance->mbitStatus.status = 3;
		}
		else if (maintenance->mbit.mbit == 1)
		{
			outputMaintenance->mbitStatus.status = 1;
			Model::trenzSpiLink.spiOption = 13;
			//outputMaintenance->block.blockId = 2;
			//to do
		}
	}
	else if (maintenance->block.blockId == 4)
	{
		if (maintenance->tlCheckStatus.ewTableCheck == 1)
		{
			//for(i=0; i<6; i++)
			//{
			if (Model::tlFailure == 0)
			{
				outputMaintenance->tlStatus.tlStatus[i] = 1;
				outputMaintenance->tlVersions[i].firstChar =
						Model::library.header.minorVersion[0];
				outputMaintenance->tlVersions[i].secondChar =
						Model::library.header.minorVersion[1];
				outputMaintenance->tlVersions[i].thirdChar =
						Model::library.header.majorVersion[0];
				outputMaintenance->tlVersions[i].fourthChar =
						Model::library.header.majorVersion[1];
			}
			//}
			//send all tl names etc
		}
	}
	else if (maintenance->block.blockId == 5)
	{
		if (maintenance->load.loadCmd == 1)
		{
			printf("so: %d\n", Model::trenzSpiLink.spiOption);
			outputMaintenance->loadStatus.status = 1;
			Model::trenzSpiLink.spiOption = 16;
		}
	}
	if (replyType == 0)
	{
		replyType = maintenance->block.blockId;
	}
	return replyType;
}

//This function clears all the mfl files present in /tmp/emmc/mfl/ directory
void mflClearProcess()
{
	if (Model::trenzSpiLink.spiOption == 15)
	{
		Model::selfTestMflHigh.initMflFiles();
		Model::trenzSpiLink.spiOption = -1;
	}
}

//This function searches for PFL changes through output sop message
//and if there are some updation, it updates output sop accordingly
void checkPflChange(Ewm1Sop *outputSop)
{
	if (ConstHigh::FstInProgress == 1)
	{
		return;
	}

	outputSop->status1Sop.newPfl3 = 0;
	outputSop->status1Sop.newPfl4 = 0;

	if (pflOut.pflLevel3.identificationFailure
			!= Model::selfTestMflHigh.pfl.pflLevel3.identificationFailure)
	{
		pflOut.pflLevel3.identificationFailure =
				Model::selfTestMflHigh.pfl.pflLevel3.identificationFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel3.identificationFailure == 1)
		{
			outputSop->status1Sop.newPfl3 = 1;
		}
	}
	if (pflOut.pflLevel3.cfdCommFailure
			!= Model::selfTestMflHigh.pfl.pflLevel3.cfdCommFailure)
	{
		pflOut.pflLevel3.cfdCommFailure =
				Model::selfTestMflHigh.pfl.pflLevel3.cfdCommFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel3.cfdCommFailure == 1)
		{
			outputSop->status1Sop.newPfl3 = 1;
		}
	}
	if (pflOut.pflLevel3.sysCntlrFailure
			!= Model::selfTestMflHigh.pfl.pflLevel3.sysCntlrFailure)
	{
		pflOut.pflLevel3.sysCntlrFailure =
				Model::selfTestMflHigh.pfl.pflLevel3.sysCntlrFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel3.sysCntlrFailure == 1)
		{
			outputSop->status1Sop.newPfl3 = 1;
		}
	}
	if (pflOut.pflLevel3.detectionFailure
			!= Model::selfTestMflHigh.pfl.pflLevel3.detectionFailure)
	{
		pflOut.pflLevel3.detectionFailure =
				Model::selfTestMflHigh.pfl.pflLevel3.detectionFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel3.detectionFailure == 1)
		{
			outputSop->status1Sop.newPfl3 = 1;
		}
	}

	// If PFL Level 3 Exists
	if (Model::selfTestMflHigh.pfl.pflLevel3.identificationFailure
			|| Model::selfTestMflHigh.pfl.pflLevel3.cfdCommFailure
			|| Model::selfTestMflHigh.pfl.pflLevel3.sysCntlrFailure
			|| Model::selfTestMflHigh.pfl.pflLevel3.detectionFailure)
	{
		outputSop->status1Sop.pfl3 = 1;
	}
	else
	{
		outputSop->status1Sop.pfl3 = 0;
	}

	if (pflOut.pflLevel4.difmProcessorFailure
			!= Model::selfTestMflHigh.pfl.pflLevel4.difmProcessorFailure)
	{
		pflOut.pflLevel4.difmProcessorFailure =
				Model::selfTestMflHigh.pfl.pflLevel4.difmProcessorFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel4.difmProcessorFailure == 1)
		{
			outputSop->status1Sop.newPfl4 = 1;
		}
	}
	if (pflOut.pflLevel4.vdmProcessorFailure
			!= Model::selfTestMflHigh.pfl.pflLevel4.vdmProcessorFailure)
	{
		pflOut.pflLevel4.vdmProcessorFailure =
				Model::selfTestMflHigh.pfl.pflLevel4.vdmProcessorFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel4.vdmProcessorFailure == 1)
		{
			outputSop->status1Sop.newPfl4 = 1;
		}
	}
	if (pflOut.pflLevel4.zeroizerFailure
			!= Model::selfTestMflHigh.pfl.pflLevel4.zeroizerFailure)
	{
		pflOut.pflLevel4.zeroizerFailure =
				Model::selfTestMflHigh.pfl.pflLevel4.zeroizerFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel4.zeroizerFailure == 1)
		{
			outputSop->status1Sop.newPfl4 = 1;
		}
	}
	if (pflOut.pflLevel4.blankingFailure
			!= Model::selfTestMflHigh.pfl.pflLevel4.blankingFailure)
	{
		pflOut.pflLevel4.blankingFailure =
				Model::selfTestMflHigh.pfl.pflLevel4.blankingFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel4.blankingFailure == 1)
		{
			outputSop->status1Sop.newPfl4 = 1;
		}
	}
	if (pflOut.pflLevel4.opsTlFailure
			!= Model::selfTestMflHigh.pfl.pflLevel4.opsTlFailure)
	{
		pflOut.pflLevel4.opsTlFailure =
				Model::selfTestMflHigh.pfl.pflLevel4.opsTlFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel4.opsTlFailure == 1)
		{
			outputSop->status1Sop.newPfl4 = 1;
		}
	}
	if (pflOut.pflLevel4.cwFailure
			!= Model::selfTestMflHigh.pfl.pflLevel4.cwFailure)
	{
		pflOut.pflLevel4.cwFailure =
				Model::selfTestMflHigh.pfl.pflLevel4.cwFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel4.cwFailure == 1)
		{
			outputSop->status1Sop.newPfl4 = 1;
		}
	}
	if (pflOut.pflLevel4.audioFailure
			!= Model::selfTestMflHigh.pfl.pflLevel4.audioFailure)
	{
		pflOut.pflLevel4.audioFailure =
				Model::selfTestMflHigh.pfl.pflLevel4.audioFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel4.audioFailure == 1)
		{
			outputSop->status1Sop.newPfl4 = 1;
		}
	}
	if (pflOut.pflLevel4.recordingFailure
			!= Model::selfTestMflHigh.pfl.pflLevel4.recordingFailure)
	{
		pflOut.pflLevel4.recordingFailure =
				Model::selfTestMflHigh.pfl.pflLevel4.recordingFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel4.recordingFailure == 1)
		{
			outputSop->status1Sop.newPfl4 = 1;
		}
	}
	if (pflOut.pflLevel4.frontLeftRxFailure
			!= Model::selfTestMflHigh.pfl.pflLevel4.frontLeftRxFailure)
	{
		pflOut.pflLevel4.frontLeftRxFailure =
				Model::selfTestMflHigh.pfl.pflLevel4.frontLeftRxFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel4.frontLeftRxFailure == 1)
		{
			outputSop->status1Sop.newPfl4 = 1;
		}
	}
	if (pflOut.pflLevel4.backLeftRxFailure
			!= Model::selfTestMflHigh.pfl.pflLevel4.backLeftRxFailure)
	{
		pflOut.pflLevel4.backLeftRxFailure =
				Model::selfTestMflHigh.pfl.pflLevel4.backLeftRxFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel4.backLeftRxFailure == 1)
		{
			outputSop->status1Sop.newPfl4 = 1;
		}
	}
	if (pflOut.pflLevel4.backRightRxFailure
			!= Model::selfTestMflHigh.pfl.pflLevel4.backRightRxFailure)
	{
		pflOut.pflLevel4.backRightRxFailure =
				Model::selfTestMflHigh.pfl.pflLevel4.backRightRxFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel4.backRightRxFailure == 1)
		{
			outputSop->status1Sop.newPfl4 = 1;
		}
	}
	if (pflOut.pflLevel4.frontRightRxFailure
			!= Model::selfTestMflHigh.pfl.pflLevel4.frontRightRxFailure)
	{
		pflOut.pflLevel4.frontRightRxFailure =
				Model::selfTestMflHigh.pfl.pflLevel4.frontRightRxFailure;
		if (Model::selfTestMflHigh.pfl.pflLevel4.frontRightRxFailure == 1)
		{
			outputSop->status1Sop.newPfl4 = 1;
		}
	}

	// If PFL Level 4 Exists
	if (Model::selfTestMflHigh.pfl.pflLevel4.difmProcessorFailure
			|| Model::selfTestMflHigh.pfl.pflLevel4.vdmProcessorFailure
			|| Model::selfTestMflHigh.pfl.pflLevel4.zeroizerFailure
			|| Model::selfTestMflHigh.pfl.pflLevel4.blankingFailure
			|| Model::selfTestMflHigh.pfl.pflLevel4.opsTlFailure
			|| Model::selfTestMflHigh.pfl.pflLevel4.cwFailure
			|| Model::selfTestMflHigh.pfl.pflLevel4.audioFailure
			|| Model::selfTestMflHigh.pfl.pflLevel4.recordingFailure
			|| Model::selfTestMflHigh.pfl.pflLevel4.frontLeftRxFailure
			|| Model::selfTestMflHigh.pfl.pflLevel4.backLeftRxFailure
			|| Model::selfTestMflHigh.pfl.pflLevel4.backRightRxFailure
			|| Model::selfTestMflHigh.pfl.pflLevel4.frontRightRxFailure)
	{
		outputSop->status1Sop.pfl4 = 1;
	}
	else
	{
		outputSop->status1Sop.pfl4 = 0;
	}

	outputSop->ackNak.ackNakStatus = 0;
	if (outputSop->status1Sop.newPfl3 || outputSop->status1Sop.newPfl4)
	{
		printf("\nPFL-NEW\n");
		ConstHigh::pflCheck = 1;
		outputSop->ackNak.ackNakStatus = 3;
	}
	if ((sopOut.status1Sop.pfl3 != outputSop->status1Sop.pfl3)
			|| (sopOut.status1Sop.pfl4 != outputSop->status1Sop.pfl4))
	{
		printf("\nPFL-CH\n");
		ConstHigh::pflCheck = 1;
		outputSop->ackNak.ackNakStatus = 3;
	}

	sopOut.status1Sop.pfl3 = outputSop->status1Sop.pfl3;
	sopOut.status1Sop.pfl4 = outputSop->status1Sop.pfl4;
	sopOut.status1Sop.newPfl3 = outputSop->status1Sop.newPfl3;
	sopOut.status1Sop.newPfl4 = outputSop->status1Sop.newPfl4;
	sopOut.ackNak.ackNakStatus = outputSop->ackNak.ackNakStatus;
}

//This function is the main logic of pfl sent to Trenz
//First it fills PFL structure, fills the maintenance structure
//according to PFL and sends it to Trenz
void sendPflLogic()
{
	Ewm1Pfl outputPfl;
	fillPflData(&outputPfl);
	fillPflData(&pflOut);
	fillEwm1PflBuffer(&outputPfl);
	printf("\nPFL-Sent\n");
	Model::trenzSpiLink.writeReplyToTrenz(7, 2, 0);
}

//This function fills output maintenance data structure from PFL data structure
void fillPflData(Ewm1Pfl *outputPfl)
{
	outputPfl->pflLevel3.cfdCommFailure =
			Model::selfTestMflHigh.pfl.pflLevel3.cfdCommFailure;
	outputPfl->pflLevel3.detectionFailure =
			Model::selfTestMflHigh.pfl.pflLevel3.detectionFailure;
	outputPfl->pflLevel3.identificationFailure =
			Model::selfTestMflHigh.pfl.pflLevel3.identificationFailure;
	outputPfl->pflLevel3.sysCntlrFailure =
			Model::selfTestMflHigh.pfl.pflLevel3.sysCntlrFailure;
	outputPfl->pflLevel4.audioFailure =
			Model::selfTestMflHigh.pfl.pflLevel4.audioFailure;
	outputPfl->pflLevel4.backLeftRxFailure =
			Model::selfTestMflHigh.pfl.pflLevel4.backLeftRxFailure;
	outputPfl->pflLevel4.backRightRxFailure =
			Model::selfTestMflHigh.pfl.pflLevel4.backRightRxFailure;
	outputPfl->pflLevel4.blankingFailure =
			Model::selfTestMflHigh.pfl.pflLevel4.blankingFailure;
	outputPfl->pflLevel4.cwFailure =
			Model::selfTestMflHigh.pfl.pflLevel4.cwFailure;
	outputPfl->pflLevel4.difmProcessorFailure =
			Model::selfTestMflHigh.pfl.pflLevel4.difmProcessorFailure;
	outputPfl->pflLevel4.frontLeftRxFailure =
			Model::selfTestMflHigh.pfl.pflLevel4.frontLeftRxFailure;
	outputPfl->pflLevel4.frontRightRxFailure =
			Model::selfTestMflHigh.pfl.pflLevel4.frontRightRxFailure;
	outputPfl->pflLevel4.opsTlFailure =
			Model::selfTestMflHigh.pfl.pflLevel4.opsTlFailure;
	outputPfl->pflLevel4.recordingFailure =
			Model::selfTestMflHigh.pfl.pflLevel4.recordingFailure;
	outputPfl->pflLevel4.vdmProcessorFailure =
			Model::selfTestMflHigh.pfl.pflLevel4.vdmProcessorFailure;
	outputPfl->pflLevel4.zeroizerFailure =
			Model::selfTestMflHigh.pfl.pflLevel4.zeroizerFailure;

}

//This function extracts self test results from data buffer
UWord8 extractSelfTestResults()
{
	UWord8 i = 0;
	UWord16 localCRC;
	for (i = 0; i < Model::trenzSpiLink.bytesRead; i++)
	{
		if (Model::trenzSpiLink.data[i] == 0xdeadbeef)
		{
			if (Model::trenzSpiLink.data[i + 1] == 11)
			{
				break;
			}
		}
	}
	if (i >= Model::trenzSpiLink.bytesRead)
	{
		return 0;
	}
	pSpiSelfTestResults = (SpiSelfTestDataReply*) &Model::trenzSpiLink.data[i];
	memcpy(&spiSelfTestResults, pSpiSelfTestResults,
			sizeof(SpiSelfTestDataReply));
	memset(Model::trenzSpiLink.data, 0, sizeof(Model::trenzSpiLink.data));
	if (spiSelfTestResults.delimeter != 0xdeadbeef)
	{
		memset(&spiSelfTestResults, 0, sizeof(SpiSelfTestDataReply));
		return 1;
	}
	for (i = 0; i < CPF_ERROR_COUNT; i++)
	{
		localCRC += spiSelfTestResults.faultData[i];
	}
	/*
	 if(spiSelfTestResults.CRC!=localCRC)
	 {
	 memset(&spiSelfTestResults,0,sizeof(SpiSelfTestDataReply));
	 return 2;
	 }
	 */
	return 1;
}

//This function calls Initiated Self Test if requested by Trenz
void startInitiatedBuiltInSelfTestProcess()
{
	if (Model::trenzSpiLink.spiOption == 12 && ConstHigh::FstInProgress==0)
	{
		if ((ConstHigh::simOnActualHwFlag == 1)
				&& (ConstHigh::readingFromLvds == 1))
		{
			printf("IBIT-Init\n");
			Model::stDelegate.selfTestType = 3;
			Model::stDelegate.selfTestActiveFlag = 1;
			Model::stDelegate.selfTestBuiltIn();
			//Model::trenzSpiLink.spiOption = 10;
			//writeCommandReplySpi(2,12);
			//writeEncryptedEventFlightRecord(EVENT_FLIGHT_RECORD_IBIT,1);
		}
	}
}

//This function calls Maintenance Self Test if requested by Trenz
void startMaintainanceBuiltInSelfTestProcess()
{
	if (Model::trenzSpiLink.spiOption == 13)
	{
		if ((ConstHigh::simOnActualHwFlag == 1)
				&& (ConstHigh::readingFromLvds == 1))
		{
			printf("MBIT-Init\n");
			Model::cfCard.foundCfFlag = Model::cfCard.checkCfCardExistance();
			Model::stDelegate.selfTestType = 4;
			Model::stDelegate.selfTestActiveFlag = 1;
			Model::stDelegate.selfTestBuiltIn();
			//writeEncryptedEventFlightRecord(EVENT_FLIGHT_RECORD_MBIT,1);
		}
		Model::trenzSpiLink.spiOption = -1;
	}
}

//*****Blocks Implementation****//
void fillM1ewSopStructure(M1ewSop *sop)
{

	sop->sop.mainMode = Model::trenzSpiLink.receiveBuffer[0] >> 14;
	sop->sop.eof = (Model::trenzSpiLink.receiveBuffer[0] & 0x2000) >> 13;
	sop->sop.threatNum = (Model::trenzSpiLink.receiveBuffer[0] & 0x1000) >> 12;
	sop->sop.ecm = (Model::trenzSpiLink.receiveBuffer[0] & 0x0C00) >> 10;
	sop->sop.audio = (Model::trenzSpiLink.receiveBuffer[0] & 0x0080) >> 7;
	sop->sop.blanking = (Model::trenzSpiLink.receiveBuffer[0] & 0x0040) >> 6;
	sop->sop.standby = (Model::trenzSpiLink.receiveBuffer[0] & 0x0020) >> 5;
//    sop->sop.libraryId      = (Model::trenzSpiLink.receiveBuffer[0] & 0x001C) >> 2;
	sop->sop.setDate = (Model::trenzSpiLink.receiveBuffer[0] & 0x0002) >> 1;
	sop->sop.pflReq = (Model::trenzSpiLink.receiveBuffer[0] & 0x0001);

	sop->year.Year = Model::trenzSpiLink.receiveBuffer[1];

	sop->mmdd.Month = Model::trenzSpiLink.receiveBuffer[2] >> 8;
	sop->mmdd.Day = Model::trenzSpiLink.receiveBuffer[2] & 0x00FF;
}

void fillEwm1SopBuffer(Ewm1Sop *sop)
{
	Model::trenzSpiLink.transmitBuffer[0] = (sop->status1Sop.mainMode & 0x0003)
			<< 14;
	Model::trenzSpiLink.transmitBuffer[0] |= (sop->status1Sop.eof & 0x0001)
			<< 13;
	Model::trenzSpiLink.transmitBuffer[0] |=
			(sop->status1Sop.threatNum & 0x0001) << 12;
	Model::trenzSpiLink.transmitBuffer[0] |= (sop->status1Sop.ecm & 0x0003)
			<< 10;
	Model::trenzSpiLink.transmitBuffer[0] |= (sop->status1Sop.audio & 0x0001)
			<< 7;
	Model::trenzSpiLink.transmitBuffer[0] |= (sop->status1Sop.blanking & 0x0001)
			<< 6;
	Model::trenzSpiLink.transmitBuffer[0] |= (sop->status1Sop.standby & 0x0001)
			<< 5;
	Model::trenzSpiLink.transmitBuffer[0] |= (sop->status1Sop.dateNotSet
			& 0x0001) << 4;
	Model::trenzSpiLink.transmitBuffer[0] |= (sop->status1Sop.newPfl3 & 0x0001)
			<< 3;
	Model::trenzSpiLink.transmitBuffer[0] |= (sop->status1Sop.newPfl4 & 0x0001)
			<< 2;
	Model::trenzSpiLink.transmitBuffer[0] |= (sop->status1Sop.pfl3 & 0x0001)
			<< 1;
	Model::trenzSpiLink.transmitBuffer[0] |= (sop->status1Sop.pfl4 & 0x0001);

	Model::trenzSpiLink.transmitBuffer[1] = (sop->status2Sop.recordingMemStatus
			& 0x00FF) << 12;
//    Model::trenzSpiLink.transmitBuffer[1]      |= sop->status2Sop.defaultLibrary << 6;
//    Model::trenzSpiLink.transmitBuffer[1]      |= sop->status2Sop.libsStatus[0] << 5;
//    Model::trenzSpiLink.transmitBuffer[1]      |= sop->status2Sop.libsStatus[1] << 4;
//    Model::trenzSpiLink.transmitBuffer[1]      |= sop->status2Sop.libsStatus[2] << 3;
//    Model::trenzSpiLink.transmitBuffer[1]      |= sop->status2Sop.libsStatus[3] << 2;
//    Model::trenzSpiLink.transmitBuffer[1]      |= sop->status2Sop.libsStatus[4] << 1;
//    Model::trenzSpiLink.transmitBuffer[1]      |= sop->status2Sop.libsStatus[5];

	Model::trenzSpiLink.transmitBuffer[2] = 0;

	Model::trenzSpiLink.transmitBuffer[3] = (sop->ackNak.ackNakStatus & 0x0003)
			<< 14;
	Model::trenzSpiLink.transmitBuffer[3] |= (sop->ackNak.direct & 0x0001)
			<< 13;
	Model::trenzSpiLink.transmitBuffer[3] |= (sop->ackNak.pBlock & 0x00FF) << 8;
	Model::trenzSpiLink.transmitBuffer[3] |= (sop->ackNak.lBlock & 0x00FF);

	Model::trenzSpiLink.transmitBuffer[4] |= (sop->visWrnReq.missileLaunch
			& 0x0001) << 2;
	Model::trenzSpiLink.transmitBuffer[4] |= (sop->visWrnReq.beingTracked
			& 0x0001) << 1;
	Model::trenzSpiLink.transmitBuffer[4] |= (sop->visWrnReq.threatFound
			& 0x0001);
}

//Block-01
void fillEwm1TgtDataBuffer(Ewm1TgtData *tgtData)
{
	Model::trenzSpiLink.transmitBuffer[0] = (tgtData->header.blockNum & 0x07)
			<< 1;
	Model::trenzSpiLink.transmitBuffer[0] |= (tgtData->header.lastBlock & 0x01);

	Model::trenzSpiLink.transmitBuffer[1] =
			(tgtData->threatsValidity.tgtValid[0] & 0x01) << 15;
	Model::trenzSpiLink.transmitBuffer[1] |=
			(tgtData->threatsValidity.tgtValid[1] & 0x01) << 14;
	Model::trenzSpiLink.transmitBuffer[1] |=
			(tgtData->threatsValidity.tgtValid[2] & 0x01) << 13;
	Model::trenzSpiLink.transmitBuffer[1] |=
			(tgtData->threatsValidity.tgtValid[3] & 0x01) << 12;
	Model::trenzSpiLink.transmitBuffer[1] |=
			(tgtData->threatsValidity.tgtValid[4] & 0x01) << 11;
	Model::trenzSpiLink.transmitBuffer[1] |=
			(tgtData->threatsValidity.tgtValid[5] & 0x01) << 10;
	Model::trenzSpiLink.transmitBuffer[1] |=
			(tgtData->threatsValidity.tgtValid[6] & 0x01) << 9;

	Model::trenzSpiLink.transmitBuffer[2] =
			(tgtData->threatsData[0].tgtPara1.batchNum & 0xFF) << 8;
	Model::trenzSpiLink.transmitBuffer[2] |=
			(tgtData->threatsData[0].tgtPara1.radarName & 0xFF);

	Model::trenzSpiLink.transmitBuffer[3] =
			(tgtData->threatsData[0].tgtPara2.attribute & 0x03) << 14;
	Model::trenzSpiLink.transmitBuffer[3] |=
			(tgtData->threatsData[0].tgtPara2.status & 0x07) << 11;
	Model::trenzSpiLink.transmitBuffer[3] |=
			(tgtData->threatsData[0].tgtPara2.platform & 0x07) << 8;
	Model::trenzSpiLink.transmitBuffer[3] |=
			(tgtData->threatsData[0].tgtPara2.unkTgt & 0x01) << 7;
	Model::trenzSpiLink.transmitBuffer[3] |=
			(tgtData->threatsData[0].tgtPara2.maxPrtyTgt & 0x01) << 6;
	Model::trenzSpiLink.transmitBuffer[3] |=
			(tgtData->threatsData[0].tgtPara2.tgtPresent & 0x01) << 5;
	Model::trenzSpiLink.transmitBuffer[3] |=
			(tgtData->threatsData[0].tgtPara2.priority & 0x0F);

	Model::trenzSpiLink.transmitBuffer[4] =
			tgtData->threatsData[0].tgtAzimuth.azimuth;

	Model::trenzSpiLink.transmitBuffer[5] =
			tgtData->threatsData[0].tgtDistance.distance;

	Model::trenzSpiLink.transmitBuffer[6] =
			(tgtData->threatsData[1].tgtPara1.batchNum & 0xFF) << 8;
	Model::trenzSpiLink.transmitBuffer[6] |=
			(tgtData->threatsData[1].tgtPara1.radarName & 0xFF);

	Model::trenzSpiLink.transmitBuffer[7] =
			(tgtData->threatsData[1].tgtPara2.attribute & 0x03) << 14;
	Model::trenzSpiLink.transmitBuffer[7] |=
			(tgtData->threatsData[1].tgtPara2.status & 0x07) << 11;
	Model::trenzSpiLink.transmitBuffer[7] |=
			(tgtData->threatsData[1].tgtPara2.platform & 0x07) << 8;
	Model::trenzSpiLink.transmitBuffer[7] |=
			(tgtData->threatsData[1].tgtPara2.unkTgt & 0x01) << 7;
	Model::trenzSpiLink.transmitBuffer[7] |=
			(tgtData->threatsData[1].tgtPara2.maxPrtyTgt & 0x01) << 6;
	Model::trenzSpiLink.transmitBuffer[7] |=
			(tgtData->threatsData[1].tgtPara2.tgtPresent & 0x01) << 5;
	Model::trenzSpiLink.transmitBuffer[7] |=
			(tgtData->threatsData[1].tgtPara2.priority & 0x0F);

	Model::trenzSpiLink.transmitBuffer[8] =
			tgtData->threatsData[1].tgtAzimuth.azimuth;

	Model::trenzSpiLink.transmitBuffer[9] =
			tgtData->threatsData[1].tgtDistance.distance;

	Model::trenzSpiLink.transmitBuffer[10] =
			(tgtData->threatsData[2].tgtPara1.batchNum & 0xFF) << 8;
	Model::trenzSpiLink.transmitBuffer[10] |=
			(tgtData->threatsData[2].tgtPara1.radarName & 0xFF);

	Model::trenzSpiLink.transmitBuffer[11] =
			(tgtData->threatsData[2].tgtPara2.attribute & 0x03) << 14;
	Model::trenzSpiLink.transmitBuffer[11] |=
			(tgtData->threatsData[2].tgtPara2.status & 0x07) << 11;
	Model::trenzSpiLink.transmitBuffer[11] |=
			(tgtData->threatsData[2].tgtPara2.platform & 0x07) << 8;
	Model::trenzSpiLink.transmitBuffer[11] |=
			(tgtData->threatsData[2].tgtPara2.unkTgt & 0x01) << 7;
	Model::trenzSpiLink.transmitBuffer[11] |=
			(tgtData->threatsData[2].tgtPara2.maxPrtyTgt & 0x01) << 6;
	Model::trenzSpiLink.transmitBuffer[11] |=
			(tgtData->threatsData[2].tgtPara2.tgtPresent & 0x01) << 5;
	Model::trenzSpiLink.transmitBuffer[11] |=
			(tgtData->threatsData[2].tgtPara2.priority & 0x0F);

	Model::trenzSpiLink.transmitBuffer[12] =
			tgtData->threatsData[2].tgtAzimuth.azimuth;

	Model::trenzSpiLink.transmitBuffer[13] =
			tgtData->threatsData[2].tgtDistance.distance;

	Model::trenzSpiLink.transmitBuffer[14] =
			(tgtData->threatsData[3].tgtPara1.batchNum & 0xFF) << 8;
	Model::trenzSpiLink.transmitBuffer[14] |=
			(tgtData->threatsData[3].tgtPara1.radarName & 0xFF);

	Model::trenzSpiLink.transmitBuffer[15] =
			(tgtData->threatsData[3].tgtPara2.attribute & 0x03) << 14;
	Model::trenzSpiLink.transmitBuffer[15] |=
			(tgtData->threatsData[3].tgtPara2.status & 0x07) << 11;
	Model::trenzSpiLink.transmitBuffer[15] |=
			(tgtData->threatsData[3].tgtPara2.platform & 0x07) << 8;
	Model::trenzSpiLink.transmitBuffer[15] |=
			(tgtData->threatsData[3].tgtPara2.unkTgt & 0x01) << 7;
	Model::trenzSpiLink.transmitBuffer[15] |=
			(tgtData->threatsData[3].tgtPara2.maxPrtyTgt & 0x01) << 6;
	Model::trenzSpiLink.transmitBuffer[15] |=
			(tgtData->threatsData[3].tgtPara2.tgtPresent & 0x01) << 5;
	Model::trenzSpiLink.transmitBuffer[15] |=
			(tgtData->threatsData[3].tgtPara2.priority & 0x0F);

	Model::trenzSpiLink.transmitBuffer[16] =
			tgtData->threatsData[3].tgtAzimuth.azimuth;

	Model::trenzSpiLink.transmitBuffer[17] =
			tgtData->threatsData[3].tgtDistance.distance;

	Model::trenzSpiLink.transmitBuffer[18] =
			(tgtData->threatsData[4].tgtPara1.batchNum & 0xFF) << 8;
	Model::trenzSpiLink.transmitBuffer[18] |=
			(tgtData->threatsData[4].tgtPara1.radarName & 0xFF);

	Model::trenzSpiLink.transmitBuffer[19] =
			(tgtData->threatsData[4].tgtPara2.attribute & 0x03) << 14;
	Model::trenzSpiLink.transmitBuffer[19] |=
			(tgtData->threatsData[4].tgtPara2.status & 0x07) << 11;
	Model::trenzSpiLink.transmitBuffer[19] |=
			(tgtData->threatsData[4].tgtPara2.platform & 0x07) << 8;
	Model::trenzSpiLink.transmitBuffer[19] |=
			(tgtData->threatsData[4].tgtPara2.unkTgt & 0x01) << 7;
	Model::trenzSpiLink.transmitBuffer[19] |=
			(tgtData->threatsData[4].tgtPara2.maxPrtyTgt & 0x01) << 6;
	Model::trenzSpiLink.transmitBuffer[19] |=
			(tgtData->threatsData[4].tgtPara2.tgtPresent & 0x01) << 5;
	Model::trenzSpiLink.transmitBuffer[19] |=
			(tgtData->threatsData[4].tgtPara2.priority & 0x0F);

	Model::trenzSpiLink.transmitBuffer[20] =
			tgtData->threatsData[4].tgtAzimuth.azimuth;

	Model::trenzSpiLink.transmitBuffer[21] =
			tgtData->threatsData[4].tgtDistance.distance;

	Model::trenzSpiLink.transmitBuffer[22] =
			(tgtData->threatsData[5].tgtPara1.batchNum & 0xFF) << 8;
	Model::trenzSpiLink.transmitBuffer[22] |=
			(tgtData->threatsData[5].tgtPara1.radarName & 0xFF);

	Model::trenzSpiLink.transmitBuffer[23] =
			(tgtData->threatsData[5].tgtPara2.attribute & 0x03) << 14;
	Model::trenzSpiLink.transmitBuffer[23] |=
			(tgtData->threatsData[5].tgtPara2.status & 0x07) << 11;
	Model::trenzSpiLink.transmitBuffer[23] |=
			(tgtData->threatsData[5].tgtPara2.platform & 0x07) << 8;
	Model::trenzSpiLink.transmitBuffer[23] |=
			(tgtData->threatsData[5].tgtPara2.unkTgt & 0x01) << 7;
	Model::trenzSpiLink.transmitBuffer[23] |=
			(tgtData->threatsData[5].tgtPara2.maxPrtyTgt & 0x01) << 6;
	Model::trenzSpiLink.transmitBuffer[23] |=
			(tgtData->threatsData[5].tgtPara2.tgtPresent & 0x01) << 5;
	Model::trenzSpiLink.transmitBuffer[23] |=
			(tgtData->threatsData[5].tgtPara2.priority & 0x0F);

	Model::trenzSpiLink.transmitBuffer[24] =
			tgtData->threatsData[5].tgtAzimuth.azimuth;

	Model::trenzSpiLink.transmitBuffer[25] =
			tgtData->threatsData[5].tgtDistance.distance;

	Model::trenzSpiLink.transmitBuffer[26] =
			(tgtData->threatsData[6].tgtPara1.batchNum & 0xFF) << 8;
	Model::trenzSpiLink.transmitBuffer[26] |=
			(tgtData->threatsData[6].tgtPara1.radarName & 0xFF);

	Model::trenzSpiLink.transmitBuffer[27] =
			(tgtData->threatsData[6].tgtPara2.attribute & 0x03) << 14;
	Model::trenzSpiLink.transmitBuffer[27] |=
			(tgtData->threatsData[6].tgtPara2.status & 0x07) << 11;
	Model::trenzSpiLink.transmitBuffer[27] |=
			(tgtData->threatsData[6].tgtPara2.platform & 0x07) << 8;
	Model::trenzSpiLink.transmitBuffer[27] |=
			(tgtData->threatsData[6].tgtPara2.unkTgt & 0x01) << 7;
	Model::trenzSpiLink.transmitBuffer[27] |=
			(tgtData->threatsData[6].tgtPara2.maxPrtyTgt & 0x01) << 6;
	Model::trenzSpiLink.transmitBuffer[27] |=
			(tgtData->threatsData[6].tgtPara2.tgtPresent & 0x01) << 5;
	Model::trenzSpiLink.transmitBuffer[27] |=
			(tgtData->threatsData[6].tgtPara2.priority & 0x0F);

	Model::trenzSpiLink.transmitBuffer[28] =
			tgtData->threatsData[6].tgtAzimuth.azimuth;

	Model::trenzSpiLink.transmitBuffer[29] =
			tgtData->threatsData[6].tgtDistance.distance;
}

//Block-03
void fillM1ewCfdRequestStructure(M1ewCfdRequest *cfdRequest)
{
	cfdRequest->cfdDataRequest.cfRequest = (Model::trenzSpiLink.receiveBuffer[0]
			>> 15) & 0x0001;
	cfdRequest->cfdDataRequest.progTypeRequest =
			(Model::trenzSpiLink.receiveBuffer[0] >> 14) & 0x0001;
	cfdRequest->cfdDataRequest.progListRequest =
			(Model::trenzSpiLink.receiveBuffer[0] >> 13) & 0x0001;

	cfdRequest->cfdParams.dispenseProgNum =
			(Model::trenzSpiLink.receiveBuffer[1] >> 11) & 0x001F;
	cfdRequest->cfdParams.dispenseType = (Model::trenzSpiLink.receiveBuffer[1]
			>> 9) & 0x0003;
}

void fillEwm1CfdRequestBuffer(Ewm1CfdRequest *cfdRequest)
{
	Model::trenzSpiLink.transmitBuffer[0] = cfdRequest->cfdCount.numChaff << 8;
	Model::trenzSpiLink.transmitBuffer[0] |= cfdRequest->cfdCount.numFlare;

	Model::trenzSpiLink.transmitBuffer[1] =
			cfdRequest->cfdProgData.dispenseProgNum << 8;
	Model::trenzSpiLink.transmitBuffer[1] |=
			(cfdRequest->cfdProgData.dispenseType & 0x03) << 5;
	Model::trenzSpiLink.transmitBuffer[1] |=
			(cfdRequest->cfdProgData.dispenseStatus & 0x07) << 2;
	Model::trenzSpiLink.transmitBuffer[1] |=
			(cfdRequest->cfdProgData.dispenseMode & 0x03);

	Model::trenzSpiLink.transmitBuffer[2] = cfdRequest->cfdPreset.presetChaff
			<< 8;
	Model::trenzSpiLink.transmitBuffer[2] |= cfdRequest->cfdPreset.presetFlare;
}

//Block-04
void fillM1ewCfdProgramStructure(M1ewCfdProgram *cfdProgram)
{
	cfdProgram->blockInfo.blockId = Model::trenzSpiLink.receiveBuffer[0] >> 8;
	cfdProgram->blockInfo.lastBlock = Model::trenzSpiLink.receiveBuffer[0]
			& 0x0001;

	if (cfdProgram->blockInfo.blockId == 1)
	{
		int i, j = 0;
		for (i = 0; i < 8; i++)
		{
			cfdProgram->dispenseProg[i].dispenseProgNum.dispenseProgNum =
					Model::trenzSpiLink.receiveBuffer[1 + j];

			cfdProgram->dispenseProg[i].dispenseProgParams.numChaffGroup =
					Model::trenzSpiLink.receiveBuffer[2 + j] >> 9;
			cfdProgram->dispenseProg[i].dispenseProgParams.intervalChaffGroup =
					(Model::trenzSpiLink.receiveBuffer[2 + j] >> 5) & 0x000F;
			cfdProgram->dispenseProg[i].dispenseProgParams.chaffPerGroup =
					(Model::trenzSpiLink.receiveBuffer[2 + j] >> 1) & 0x000F;
			cfdProgram->dispenseProg[i].dispenseProgParams.sglDblChaff =
					(Model::trenzSpiLink.receiveBuffer[2 + j] & 0x0001);

			cfdProgram->dispenseProg[i].dispenseProgParams.intervalChaff =
					(Model::trenzSpiLink.receiveBuffer[3 + j] >> 12) & 0x000F;
			cfdProgram->dispenseProg[i].dispenseProgParams.numFlare =
					(Model::trenzSpiLink.receiveBuffer[3 + j] >> 5) & 0x007F;
			cfdProgram->dispenseProg[i].dispenseProgParams.intervalFlare =
					(Model::trenzSpiLink.receiveBuffer[3 + j] >> 1) & 0x000F;
			cfdProgram->dispenseProg[i].dispenseProgParams.sglDblFlare =
					(Model::trenzSpiLink.receiveBuffer[3 + j] & 0x0001);

			j += 3;
		}
	}
	else if (cfdProgram->blockInfo.blockId == 2)
	{
		cfdProgram->presetCF.presetChaff = Model::trenzSpiLink.receiveBuffer[1]
				>> 8;
		cfdProgram->presetCF.presetFlare = Model::trenzSpiLink.receiveBuffer[1]
				& 0x00FF;
	}
}

void fillEwm1CfdProgramBuffer(Ewm1CfdProgram *cfdProgram)
{
	int i, j = 0;
	for (i = 0; i < 15; i++)
	{
		Model::trenzSpiLink.transmitBuffer[j] =
				(cfdProgram->dispenseProg[i].numChaffGroup & 0x3F) << 9;
		Model::trenzSpiLink.transmitBuffer[j] |=
				(cfdProgram->dispenseProg[i].intervalChaffGroup & 0x0F) << 5;
		Model::trenzSpiLink.transmitBuffer[j] |=
				(cfdProgram->dispenseProg[i].chaffPerGroup & 0x0F) << 1;
		Model::trenzSpiLink.transmitBuffer[j] |=
				(cfdProgram->dispenseProg[i].sglDblChaff & 0x01);

		Model::trenzSpiLink.transmitBuffer[j + 1] =
				(cfdProgram->dispenseProg[i].intervalChaff & 0x0F) << 12;
		Model::trenzSpiLink.transmitBuffer[j + 1] |=
				(cfdProgram->dispenseProg[i].numFlare & 0x3F) << 5;
		Model::trenzSpiLink.transmitBuffer[j + 1] |=
				(cfdProgram->dispenseProg[i].intervalFlare & 0x0F) << 1;
		Model::trenzSpiLink.transmitBuffer[j + 1] |=
				(cfdProgram->dispenseProg[i].sglDblFlare & 0x01);

		j += 2;
	}
}

//Block-05
void fillM1ewAcDataStructure(M1ewAcData *acData)
{
	acData->dataValid.altitudeValid = (Model::trenzSpiLink.receiveBuffer[0]
			& 0x06) >> 5;
	acData->dataValid.headingValid = (Model::trenzSpiLink.receiveBuffer[0]
			& 0x05) >> 4;
	acData->dataValid.pitchRollValid = (Model::trenzSpiLink.receiveBuffer[0]
			& 0x03) >> 2;
	acData->dataValid.insNavDataValid = (Model::trenzSpiLink.receiveBuffer[0]
			& 0x02) >> 1;
	acData->dataValid.insDataValid = Model::trenzSpiLink.receiveBuffer[0]
			& 0x01;

	acData->timeTag = Model::trenzSpiLink.receiveBuffer[1] << 16;
	acData->timeTag |= Model::trenzSpiLink.receiveBuffer[2];

	acData->velNorth = Model::trenzSpiLink.receiveBuffer[3];
	acData->velWest = Model::trenzSpiLink.receiveBuffer[4];
	acData->velUp = Model::trenzSpiLink.receiveBuffer[5];
	acData->roll = Model::trenzSpiLink.receiveBuffer[6];
	acData->pitch = Model::trenzSpiLink.receiveBuffer[7];
	acData->heading = Model::trenzSpiLink.receiveBuffer[8];
	acData->rollRate = Model::trenzSpiLink.receiveBuffer[9];
	acData->pitchRate = Model::trenzSpiLink.receiveBuffer[10];
	acData->yawRate = Model::trenzSpiLink.receiveBuffer[11];
	acData->altitude = Model::trenzSpiLink.receiveBuffer[12];

	acData->latitude = Model::trenzSpiLink.receiveBuffer[13] << 16;
	acData->latitude |= Model::trenzSpiLink.receiveBuffer[14];

	acData->longitude = Model::trenzSpiLink.receiveBuffer[15] << 16;
	acData->longitude |= Model::trenzSpiLink.receiveBuffer[16];
}

//Block-19
void fillEwm1PflBuffer(Ewm1Pfl *pfl)
{
	Model::trenzSpiLink.transmitBuffer[0] =
			(pfl->pflLevel3.identificationFailure & 0x01) << 5;
	Model::trenzSpiLink.transmitBuffer[0] |= (pfl->pflLevel3.cfdCommFailure
			& 0x01) << 2;
	Model::trenzSpiLink.transmitBuffer[0] |= (pfl->pflLevel3.sysCntlrFailure
			& 0x01) << 1;
	Model::trenzSpiLink.transmitBuffer[0] |= (pfl->pflLevel3.detectionFailure
			& 0x01);

	Model::trenzSpiLink.transmitBuffer[1] = (pfl->pflLevel4.difmProcessorFailure
			& 0x01) << 11;
	Model::trenzSpiLink.transmitBuffer[1] |= (pfl->pflLevel4.vdmProcessorFailure
			& 0x01) << 10;
	Model::trenzSpiLink.transmitBuffer[1] |= (pfl->pflLevel4.zeroizerFailure
			& 0x01) << 9;
	Model::trenzSpiLink.transmitBuffer[1] |= (pfl->pflLevel4.blankingFailure
			& 0x01) << 8;
	Model::trenzSpiLink.transmitBuffer[1] |=
			(pfl->pflLevel4.opsTlFailure & 0x01) << 7;
	Model::trenzSpiLink.transmitBuffer[1] |= (pfl->pflLevel4.cwFailure & 0x01)
			<< 6;
	Model::trenzSpiLink.transmitBuffer[1] |=
			(pfl->pflLevel4.audioFailure & 0x01) << 5;
	Model::trenzSpiLink.transmitBuffer[1] |= (pfl->pflLevel4.recordingFailure
			& 0x01) << 4;
	Model::trenzSpiLink.transmitBuffer[1] |= (pfl->pflLevel4.frontLeftRxFailure
			& 0x01) << 3;
	Model::trenzSpiLink.transmitBuffer[1] |= (pfl->pflLevel4.backLeftRxFailure
			& 0x01) << 2;
	Model::trenzSpiLink.transmitBuffer[1] |= (pfl->pflLevel4.backRightRxFailure
			& 0x01) << 1;
	Model::trenzSpiLink.transmitBuffer[1] |= (pfl->pflLevel4.frontRightRxFailure
			& 0x01);
}

//Block-20
void fillM1ewMaintStructure(M1ewMaint *maint)
{
	maint->block.blockId = Model::trenzSpiLink.receiveBuffer[0] >> 8;

	if (maint->block.blockId == 1)
	{
		maint->reqEw.step = Model::trenzSpiLink.receiveBuffer[1] >> 3;
		maint->reqEw.mflReq = (Model::trenzSpiLink.receiveBuffer[1] >> 2)
				& 0x0001;
		maint->reqEw.mflClear = Model::trenzSpiLink.receiveBuffer[1] & 0x0001;
	}
	else if (maint->block.blockId == 2)
	{
		maint->mbit.mbit = Model::trenzSpiLink.receiveBuffer[1];
	}
	else if (maint->block.blockId == 4)
	{
		maint->tlCheckStatus.ewTableCheck =
				Model::trenzSpiLink.receiveBuffer[1];
	}
	else if (maint->block.blockId == 5)
	{
		maint->load.loadCmd = Model::trenzSpiLink.receiveBuffer[1];
	}
}

void fillEwm1MaintBuffer(Ewm1Maint *maint, int lb)
{

	memset(Model::trenzSpiLink.transmitBuffer, 0,
			sizeof(Model::trenzSpiLink.transmitBuffer));
	maint->block.blockId = lb;
	Model::trenzSpiLink.transmitBuffer[0] = maint->block.blockId << 8;

	if (lb == 1)
	{
		Model::trenzSpiLink.transmitBuffer[1] =
				(maint->mflStatus.status & 0x01);

		Model::trenzSpiLink.transmitBuffer[2] =
				(maint->tgtStatus.exists & 0x01);

		Model::trenzSpiLink.transmitBuffer[3] = maint->swVersion.majorVer[0]
				<< 8;
		Model::trenzSpiLink.transmitBuffer[3] |= maint->swVersion.majorVer[1];

		Model::trenzSpiLink.transmitBuffer[4] = maint->swVersion.minorVer[0]
				<< 8;
		Model::trenzSpiLink.transmitBuffer[4] |= maint->swVersion.minorVer[1];

		Model::trenzSpiLink.transmitBuffer[5] = maint->year.year;

		Model::trenzSpiLink.transmitBuffer[6] = maint->month.month;

		Model::trenzSpiLink.transmitBuffer[7] = maint->day.day;
	}
	else if (lb == 2)
	{
		Model::trenzSpiLink.transmitBuffer[1] =
				(maint->mbitStatus.status & 0x03);
	}
	else if (lb == 3)
	{
		maint->block.blockId = 3;
		Model::trenzSpiLink.transmitBuffer[0] = maint->block.blockId << 8;
		Model::trenzSpiLink.transmitBuffer[0] |= (maint->mflBlockId.flrExists
				& 0x01) << 1;
		Model::trenzSpiLink.transmitBuffer[0] |= (maint->mflBlockId.lastBlock
				& 0x01);

		Model::trenzSpiLink.transmitBuffer[1] = (maint->sysSubsys.sys1 & 0x0F)
				<< 12;
		Model::trenzSpiLink.transmitBuffer[1] |= (maint->sysSubsys.sys2 & 0x0F)
				<< 8;
		Model::trenzSpiLink.transmitBuffer[1] |= maint->sysSubsys.subsys;

		Model::trenzSpiLink.transmitBuffer[2] = maint->lruFctn.lru1 << 8;
		Model::trenzSpiLink.transmitBuffer[2] |= maint->lruFctn.lru2;

		Model::trenzSpiLink.transmitBuffer[3] = maint->lruFctn.lru3 << 8;
		Model::trenzSpiLink.transmitBuffer[3] |= (maint->lruFctn.fctn1 & 0x0F)
				<< 4;
		Model::trenzSpiLink.transmitBuffer[3] |= (maint->lruFctn.fctn2 & 0x0F);

		Model::trenzSpiLink.transmitBuffer[4] = maint->occurTime.occurance
				>> 16;

		Model::trenzSpiLink.transmitBuffer[5] = maint->occurTime.occurance
				& 0x0000FFFF;

		Model::trenzSpiLink.transmitBuffer[6] = maint->durationTime.duration
				>> 16;

		Model::trenzSpiLink.transmitBuffer[7] = maint->durationTime.duration
				& 0x0000FFFF;

		Model::trenzSpiLink.transmitBuffer[8] = maint->amount.count;

		Model::trenzSpiLink.transmitBuffer[9] =
				maint->description.description[0] << 8;
		Model::trenzSpiLink.transmitBuffer[9] |=
				maint->description.description[1];

		Model::trenzSpiLink.transmitBuffer[10] =
				maint->description.description[2] << 8;
		Model::trenzSpiLink.transmitBuffer[10] |=
				maint->description.description[3];

		Model::trenzSpiLink.transmitBuffer[11] =
				maint->description.description[4] << 8;
		Model::trenzSpiLink.transmitBuffer[11] |=
				maint->description.description[5];

		Model::trenzSpiLink.transmitBuffer[12] =
				maint->description.description[6] << 8;
		Model::trenzSpiLink.transmitBuffer[12] |=
				maint->description.description[7];

		Model::trenzSpiLink.transmitBuffer[13] =
				maint->description.description[8] << 8;
		Model::trenzSpiLink.transmitBuffer[13] |=
				maint->description.description[9];

		Model::trenzSpiLink.transmitBuffer[14] =
				maint->description.description[10] << 8;
		Model::trenzSpiLink.transmitBuffer[14] |=
				maint->description.description[11];

		Model::trenzSpiLink.transmitBuffer[15] =
				maint->description.description[12] << 8;
		Model::trenzSpiLink.transmitBuffer[15] |=
				maint->description.description[13];

		Model::trenzSpiLink.transmitBuffer[16] =
				maint->description.description[14] << 8;
		Model::trenzSpiLink.transmitBuffer[16] |=
				maint->description.description[15];

		Model::trenzSpiLink.transmitBuffer[17] =
				maint->description.description[16] << 8;
		Model::trenzSpiLink.transmitBuffer[17] |=
				maint->description.description[17];

		Model::trenzSpiLink.transmitBuffer[18] =
				maint->description.description[18] << 8;
		Model::trenzSpiLink.transmitBuffer[18] |=
				maint->description.description[19];

		Model::trenzSpiLink.transmitBuffer[19] =
				maint->description.description[20] << 8;
		Model::trenzSpiLink.transmitBuffer[19] |=
				maint->description.description[21];

		Model::trenzSpiLink.transmitBuffer[20] =
				maint->description.description[22] << 8;
		Model::trenzSpiLink.transmitBuffer[20] |=
				maint->description.description[23];

		Model::trenzSpiLink.transmitBuffer[21] =
				maint->description.description[24] << 8;
		Model::trenzSpiLink.transmitBuffer[21] |=
				maint->description.description[25];

		Model::trenzSpiLink.transmitBuffer[22] =
				maint->description.description[26] << 8;
		Model::trenzSpiLink.transmitBuffer[22] |=
				maint->description.description[27];

		Model::trenzSpiLink.transmitBuffer[23] =
				maint->description.description[28] << 8;
		Model::trenzSpiLink.transmitBuffer[23] |=
				maint->description.description[29];

		Model::trenzSpiLink.transmitBuffer[24] =
				maint->description.description[30] << 8;
		Model::trenzSpiLink.transmitBuffer[24] |=
				maint->description.description[31];

		Model::trenzSpiLink.transmitBuffer[25] =
				maint->description.description[32] << 8;
		Model::trenzSpiLink.transmitBuffer[25] |=
				maint->description.description[33];

		Model::trenzSpiLink.transmitBuffer[26] =
				maint->description.description[34] << 8;
		Model::trenzSpiLink.transmitBuffer[26] |=
				maint->description.description[35];

		Model::trenzSpiLink.transmitBuffer[27] =
				maint->description.description[36] << 8;
		Model::trenzSpiLink.transmitBuffer[27] |=
				maint->description.description[37];

		Model::trenzSpiLink.transmitBuffer[28] =
				maint->description.description[38] << 8;
		Model::trenzSpiLink.transmitBuffer[28] |=
				maint->description.description[39];
	}
	else if (lb == 4)
	{
		Model::trenzSpiLink.transmitBuffer[1] = (maint->tlStatus.cfdManExists
				& 0x01) << 2;
		Model::trenzSpiLink.transmitBuffer[1] |= (maint->tlStatus.cfdAutoExists
				& 0x01) << 1;
		Model::trenzSpiLink.transmitBuffer[1] |= (maint->tlStatus.tlStatus[0]
				& 0x01);
//        Model::trenzSpiLink.transmitBuffer[1]  = maint->tlStatus.tlStatus[0] << 5;
//        Model::trenzSpiLink.transmitBuffer[1] |= maint->tlStatus.tlStatus[1] << 4;
//        Model::trenzSpiLink.transmitBuffer[1] |= maint->tlStatus.tlStatus[3] << 3;
//        Model::trenzSpiLink.transmitBuffer[1] |= maint->tlStatus.tlStatus[4] << 2;
//        Model::trenzSpiLink.transmitBuffer[1] |= maint->tlStatus.tlStatus[5] << 1;
//        Model::trenzSpiLink.transmitBuffer[1] |= maint->tlStatus.tlStatus[6];

		Model::trenzSpiLink.transmitBuffer[2] = maint->tlVersions[0].firstChar
				<< 8;
		Model::trenzSpiLink.transmitBuffer[2] |=
				maint->tlVersions[0].secondChar;

		Model::trenzSpiLink.transmitBuffer[3] = maint->tlVersions[0].thirdChar
				<< 8;
		Model::trenzSpiLink.transmitBuffer[3] |=
				maint->tlVersions[0].fourthChar;

//        Model::trenzSpiLink.transmitBuffer[4]  = maint->tlVersions[1].firstChar << 8;
//        Model::trenzSpiLink.transmitBuffer[4] |= maint->tlVersions[1].secondChar;
//
//        Model::trenzSpiLink.transmitBuffer[5]  = maint->tlVersions[1].thirdChar << 8;
//        Model::trenzSpiLink.transmitBuffer[5] |= maint->tlVersions[1].fourthChar;
//
//        Model::trenzSpiLink.transmitBuffer[6]  = maint->tlVersions[2].firstChar << 8;
//        Model::trenzSpiLink.transmitBuffer[6] |= maint->tlVersions[2].secondChar;
//
//        Model::trenzSpiLink.transmitBuffer[7]  = maint->tlVersions[2].thirdChar << 8;
//        Model::trenzSpiLink.transmitBuffer[7] |= maint->tlVersions[2].fourthChar;
//
//        Model::trenzSpiLink.transmitBuffer[8]  = maint->tlVersions[3].firstChar << 8;
//        Model::trenzSpiLink.transmitBuffer[8] |= maint->tlVersions[3].secondChar;
//
//        Model::trenzSpiLink.transmitBuffer[9]  = maint->tlVersions[3].thirdChar << 8;
//        Model::trenzSpiLink.transmitBuffer[9] |= maint->tlVersions[3].fourthChar;
//
//        Model::trenzSpiLink.transmitBuffer[10] = maint->tlVersions[4].firstChar << 8;
//        Model::trenzSpiLink.transmitBuffer[10]|= maint->tlVersions[4].secondChar;
//
//        Model::trenzSpiLink.transmitBuffer[11] = maint->tlVersions[4].thirdChar << 8;
//        Model::trenzSpiLink.transmitBuffer[11]|= maint->tlVersions[4].fourthChar;
//
//        Model::trenzSpiLink.transmitBuffer[12] = maint->tlVersions[5].firstChar << 8;
//        Model::trenzSpiLink.transmitBuffer[12]|= maint->tlVersions[5].secondChar;
//
//        Model::trenzSpiLink.transmitBuffer[13] = maint->tlVersions[5].thirdChar << 8;
//        Model::trenzSpiLink.transmitBuffer[13]|= maint->tlVersions[5].fourthChar;
	}
	else if (lb == 5)
	{
		Model::trenzSpiLink.transmitBuffer[1] =
				(maint->loadStatus.status & 0x01);
	}
}

//Block-21
void fillEwm1EofBuffer(Ewm1Eof *eof)
{
	Model::trenzSpiLink.transmitBuffer[0] = (eof->lastBlock.lastBlock & 0x01);

	Model::trenzSpiLink.transmitBuffer[1] = (eof->sysSubsys.sys1 & 0x0F) << 12;
	Model::trenzSpiLink.transmitBuffer[1] |= (eof->sysSubsys.sys2 & 0x0F) << 8;
	Model::trenzSpiLink.transmitBuffer[1] |= eof->sysSubsys.subsys;

	Model::trenzSpiLink.transmitBuffer[2] = eof->lruFctn.lru1 << 8;
	Model::trenzSpiLink.transmitBuffer[2] |= eof->lruFctn.lru2;

	Model::trenzSpiLink.transmitBuffer[3] = eof->lruFctn.lru3 << 8;
	Model::trenzSpiLink.transmitBuffer[3] |= (eof->lruFctn.fctn1 & 0x0F) << 4;
	Model::trenzSpiLink.transmitBuffer[3] |= (eof->lruFctn.fctn2 & 0x0F);

	Model::trenzSpiLink.transmitBuffer[4] = eof->occurTime.occurance >> 16;

	Model::trenzSpiLink.transmitBuffer[5] = eof->occurTime.occurance
			& 0x0000FFFF;

	Model::trenzSpiLink.transmitBuffer[6] = eof->durationTime.duration >> 16;

	Model::trenzSpiLink.transmitBuffer[7] = eof->durationTime.duration
			& 0x0000FFFF;

	Model::trenzSpiLink.transmitBuffer[8] = eof->amount.count;

	Model::trenzSpiLink.transmitBuffer[9] = eof->description.description[0]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[9] |= eof->description.description[1];

	Model::trenzSpiLink.transmitBuffer[10] = eof->description.description[2]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[10] |= eof->description.description[3];

	Model::trenzSpiLink.transmitBuffer[11] = eof->description.description[4]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[11] |= eof->description.description[5];

	Model::trenzSpiLink.transmitBuffer[12] = eof->description.description[6]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[12] |= eof->description.description[7];

	Model::trenzSpiLink.transmitBuffer[13] = eof->description.description[8]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[13] |= eof->description.description[9];

	Model::trenzSpiLink.transmitBuffer[14] = eof->description.description[10]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[14] |= eof->description.description[11];

	Model::trenzSpiLink.transmitBuffer[15] = eof->description.description[12]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[15] |= eof->description.description[13];

	Model::trenzSpiLink.transmitBuffer[16] = eof->description.description[14]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[16] |= eof->description.description[15];

	Model::trenzSpiLink.transmitBuffer[17] = eof->description.description[16]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[17] |= eof->description.description[17];

	Model::trenzSpiLink.transmitBuffer[18] = eof->description.description[18]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[18] |= eof->description.description[19];

	Model::trenzSpiLink.transmitBuffer[19] = eof->description.description[20]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[19] |= eof->description.description[21];

	Model::trenzSpiLink.transmitBuffer[20] = eof->description.description[22]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[20] |= eof->description.description[23];

	Model::trenzSpiLink.transmitBuffer[21] = eof->description.description[24]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[21] |= eof->description.description[25];

	Model::trenzSpiLink.transmitBuffer[22] = eof->description.description[26]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[22] |= eof->description.description[27];

	Model::trenzSpiLink.transmitBuffer[23] = eof->description.description[28]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[23] |= eof->description.description[29];

	Model::trenzSpiLink.transmitBuffer[24] = eof->description.description[30]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[24] |= eof->description.description[31];

	Model::trenzSpiLink.transmitBuffer[25] = eof->description.description[32]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[25] |= eof->description.description[33];

	Model::trenzSpiLink.transmitBuffer[26] = eof->description.description[34]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[26] |= eof->description.description[35];

	Model::trenzSpiLink.transmitBuffer[27] = eof->description.description[36]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[27] |= eof->description.description[37];

	Model::trenzSpiLink.transmitBuffer[28] = eof->description.description[38]
			<< 8;
	Model::trenzSpiLink.transmitBuffer[28] |= eof->description.description[39];
}

void sendAsyncSop(int ackNack, int pb, int lb)
{
	timeStampLastAyncSop = Utility::getSystemTime();
	Ewm1Sop transmitSop;
	memset(&transmitSop, 0, sizeof(Ewm1Sop));

	transmitSop.status1Sop.mainMode = 0;
	updatePflFlagsInSopOut(transmitSop);
	fillSopOutStructure(&sopIn, &transmitSop);

	transmitSop.ackNak.ackNakStatus = ackNack;
	transmitSop.ackNak.pBlock = pb;
	transmitSop.ackNak.lBlock = lb;

	Model::selfTestMflHigh.onPflStatusMessageSent();

	fillEwm1SopBuffer(&transmitSop);
	Model::trenzSpiLink.writeReplyToTrenz(1, 5, 0);
}
void checkAudioAsyncSopNeeded()
{

	if(ConstHigh::sopAsyncAudioFlag!=1 || Model::trenzSpiLink.spiOption !=10)
	{
		return;
	}

	uint64_t currTime = Utility::getSystemTime();
	if((currTime-timeStampLastAyncSop)>2000000)
	{
		ConstHigh::sopAsyncAudioFlag = 0;
		sendAsyncSop(3,0,0);
		timeStampLastAyncSop=currTime;
	}
}
