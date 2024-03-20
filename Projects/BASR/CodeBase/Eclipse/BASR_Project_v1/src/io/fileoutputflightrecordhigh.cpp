/*
 * FileOutputFlightRecordHigh.cpp
 *
 *  Created on: Feb 3, 2021
 *      Author: saad
 */

#define FILEOUTPUTFLIGHTRECORDHIGH_CPP_

#include "fileoutputflightrecordhigh.h"

FileOutputFlightRecordHigh::FileOutputFlightRecordHigh()
{ // @suppress("Class members should be properly initialized")

	usedFlightRecordSize = 0;
	usedFlightRecordSizeLow = 0;
	flightRecordOverflowFlag = 0;
	flightRecordOverflowFlagLow = 0;
	totalFlightRecordSize = 0;
	totalFlightRecordSizeLow = 0;
	assignRandomNumberToGlobalSessionID();
	globalSessionIdLowTracks = 0;
	flightRecordCount = 0;
	flightRecordCountLow = 0;
	countToWriteEncryptionHeader = 0;
	countToWriteEncryptionHeaderLowTracks = 0;
	flightRecordAircraftNavigationInformationWritten = 0;
	lowFlightRecordAircraftNavigationInformationWritten = 0;
}

void FileOutputFlightRecordHigh::assignRandomNumberToGlobalSessionID()
{
	time_t randomNumberSeed;
	srand((unsigned) time(&randomNumberSeed));
	globalSessionId = rand() % 255;
}

void FileOutputFlightRecordHigh::writeThreatProcessorReportToFlightRecord(
		UWord8 numberOfTracks, LibraryMetaClass *libraryMeta,
		string flighRecordPath, EmitterLog emitterLogLow, bool &memoryFailure)
{

	UWord32 i = 0;
	UWord16 crc = 0xffff;
	UWord16 *ptr = (UWord16*) &emitterLogLow;
	//UWord32 sizecheck = sizeof(EmitterLog);
	UWord32 sizeEmitter, sizeFR, sizeTotal;
	time_t epoch = time(NULL);
	globalSessionIdLowTracks++;
	if (globalSessionIdLowTracks == 256)
	{
		globalSessionIdLowTracks = 0;
	}
	//FILE *ourFile;
	//FILE *dummy;
	sizeEmitter = sizeof(EmitterLog);
	sizeFR = sizeof(FlightRecord);
	sizeTotal = ((sizeEmitter - (256 * sizeFR)) + (sizeFR * numberOfTracks));
	if (numberOfTracks > 0)
	{
		emitterLogLow.flightRecordHeader.Delimiter = 0xbeeffeeddeadbeef;
		emitterLogLow.flightRecordHeader.structureCount = numberOfTracks;
		emitterLogLow.flightRecordHeader.MessageType = 15;
		emitterLogLow.flightRecordHeader.RtcTime = (uint32_t) epoch;
		for (i = 10; i < sizeTotal / 2; i++)
		{
			crc ^= (UWord16) ptr[i];
		}

		emitterLogLow.flightRecordHeader.CRC = crc;
		//printf("CRC %d \n",emitterLog.CRC);

		//printf("FR N-T: %d\n",numberOfTracks);

		//sizeTotal = ((sizeEmitter-(256*sizeFR))+ (sizeFR * numberOfTracks));
		//ourFile = fopen("Out.txt", "a+");
		//fwrite(&emitterLog,sizeof(EmitterLog),1,ourFile);
		//fwrite(&emitterLog,sizeTotal,1,ourFile);
		//fclose(ourFile);

		//dummy=fopen("OutInd.txt", "w");
		//fwrite(&emitterLog,sizeof(EmitterLog),1,dummy);
		//fwrite(&emitterLog,sizeTotal,1,dummy);
		//fclose(dummy);

		writeEncryptedThreatProcessorFlightRecord(libraryMeta, flighRecordPath,
				emitterLogLow, memoryFailure);
	}

}

UWord16 FileOutputFlightRecordHigh::calculateCRC(UWord8 *dataPtr, UWord32 size,
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

void FileOutputFlightRecordHigh::writeEncryptedThreatProcessorFlightRecord(
		LibraryMetaClass *libraryMeta, string flighRecordPath,
		EmitterLog emitterLogLow, bool &memoryFailure)
{
	UWord32 size = ((sizeof(EmitterLog) - (256 * sizeof(FlightRecord)))
			+ (sizeof(FlightRecord)
					* emitterLogLow.flightRecordHeader.structureCount));
	emitterLogLow.flightRecordHeader.CRC =
			FileOutputFlightRecordHigh::calculateCRC((UWord8*) &emitterLogLow,
					size, sizeof(FlightRecordHeader));
	if (lowFlightRecordAircraftNavigationInformationWritten == 0)
	{
		writeEncryptedFlightRecordAircraftNavigationInformationLowEmitters(
				libraryMeta, flighRecordPath, memoryFailure);
	}
	encryptAndWriteLowTracksToFile((UWord8*) &emitterLogLow, size, libraryMeta,
			flighRecordPath, memoryFailure);

}

void FileOutputFlightRecordHigh::writeEncryptedFlightRecordAircraftNavigationInformationLowEmitters(
		LibraryMetaClass *libraryMeta, string flighRecordPath,
		bool &memoryFailure)
{

	time_t epoch = time(NULL);

	memset(&flightRecordAircraftNavigationInformationLow, 0,
			sizeof(flightRecordAircraftNavigationInformationLow));
	flightRecordAricraftNavigationInformationHeaderLow.Delimiter =
			0xbeeffeeddeadbeef;
	flightRecordAricraftNavigationInformationHeaderLow.structureCount =
			sizeof(flightRecordAircraftNavigationInformationLow);
	flightRecordAricraftNavigationInformationHeaderLow.RtcTime =
			(uint32_t) epoch;
	flightRecordAricraftNavigationInformationHeaderLow.MessageType = 30;
	flightRecordAircraftNavigationInformationLow.sessionId =
			globalSessionIdLowTracks;
	flightRecordAricraftNavigationInformationHeaderLow.CRC = calculateCRC(
			(UWord8*) &flightRecordAircraftNavigationInformationLow,
			sizeof(flightRecordAircraftNavigationInformationLow), 0);

	encryptAndWriteLowTracksToFile(
			(UWord8*) &flightRecordAricraftNavigationInformationHeaderLow,
			sizeof(flightRecordAricraftNavigationInformationHeaderLow),
			libraryMeta, flighRecordPath, memoryFailure);

	encryptAndWriteLowTracksToFile(
			(UWord8*) &flightRecordAircraftNavigationInformationLow,
			sizeof(flightRecordAircraftNavigationInformationLow), libraryMeta,
			flighRecordPath, memoryFailure);

	lowFlightRecordAircraftNavigationInformationWritten = 1;

}

void FileOutputFlightRecordHigh::encryptAndWriteLowTracksToFile(UWord8 *ptr,
		UWord32 size, LibraryMetaClass *libraryMeta, string flighRecordPath,
		bool &memoryFailure)
{
	FlightRecordSizeHeader flightRecordSizeHeader[3];
	L32 myInt = 0;
	UWord32 sizeInBytesTotal = SIZE_OF_FR * MB_TO_BYTES;
	UWord32 pointerToWrite = 0, dummyPtrToWrite = 0;
	FILE *file;
	UWord32 sizeCheck = size % 16;
	UWord32 localSize = size;
	UWord8 *outputPtr;
	UWord64 randomNumber = 0;
	Word32 error = 0;
	Word32 dummyCRC = 0;
	uint8_t *password, j = 0;
	string fileName;

	if (error == 0)
	{        //to remove warning
		error = 0;
	}

	password = libraryMeta->bufferForKey;
	for (j = 0; j < sizeof(decryptionKeyName); j++)
	{
		decryptionKeyName[j] = *password != 0 ? *password++ : 0;
	}

	memcpy(keyDigest, libraryMeta->keyDigest,
			SHA256_BLOCK_SIZE * sizeof(UWord8));
	if (ConstHigh::lowLogEncrypted == true)
	{
		fileName = flighRecordPath + "flightRecordLow.fre";
	}
	else
	{
		fileName = flighRecordPath + "flightRecordLow.fr";
	}

	if (totalFlightRecordSizeLow == 0)
	{
		totalFlightRecordSizeLow = sizeInBytesTotal;
	}
	memset(flightRecordSizeHeader, 0, sizeof(flightRecordSizeHeader));
	if (sizeCheck != 0)
	{
		localSize = localSize - sizeCheck + 16;
	}
	outputPtr = (UWord8*) calloc(localSize, 1);

	file = fopen(fileName.c_str(), "r+");

	if ((uint64_t)file <= 0 || ConstHigh::flightRecordAppendMode == false)
	{
		initializeFlightRecordLowFile(flighRecordPath, memoryFailure);
		countToWriteEncryptionHeaderLowTracks = 0;
		file = fopen(fileName.c_str(), "r+");
		if ((uint64_t)file <= 0)
		{
			//printf("FR unreachable\n");
			memoryFailure = true;
			return;
		}
	}

	if (flightRecordCountLow == 0)
	{

		srand((unsigned) time(&randomNumberSeedLow));
		sha256_init(&ctx);
		sha256_update(&ctx, decryptionKeyName, sizeof(decryptionKeyName));
		sha256_final(&ctx, keyDigest);
		aesSetupEncrypt(expandedKey, decryptionKeyName);
		//key change pe frcount = 0
	}
	fread(&flightRecordSizeHeader[0], 1, sizeof(FlightRecordSizeHeader), file);

	fread(&flightRecordSizeHeader[1], 1, sizeof(FlightRecordSizeHeader), file);

	fread(&flightRecordSizeHeader[2], 1, sizeof(FlightRecordSizeHeader), file);

	dummyCRC = calculateCRC((UWord8*) &flightRecordSizeHeader[0],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));
	dummyCRC = calculateCRC((UWord8*) &flightRecordSizeHeader[1],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));
	dummyCRC = calculateCRC((UWord8*) &flightRecordSizeHeader[2],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));

	if ((flightRecordSizeHeader[0].crc
			== calculateCRC((UWord8*) &flightRecordSizeHeader[0],
					sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
			&& (flightRecordSizeHeader[1].crc
					== calculateCRC((UWord8*) &flightRecordSizeHeader[1],
							sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
			&& (flightRecordSizeHeader[2].crc
					== calculateCRC((UWord8*) &flightRecordSizeHeader[2],
							sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
	{
		if (flightRecordSizeHeader[0].size + flightRecordSizeHeader[1].size
				- 2 * flightRecordSizeHeader[2].size == 0)
		{
			pointerToWrite = flightRecordSizeHeader[0].size;
			flightRecordOverflowFlagLow =
					flightRecordSizeHeader[0].overFlowFlag;
		}
		else if (flightRecordSizeHeader[0].size
				== flightRecordSizeHeader[1].size)
		{
			pointerToWrite = flightRecordSizeHeader[0].size;
			flightRecordOverflowFlagLow =
					flightRecordSizeHeader[0].overFlowFlag;
		}
		else if (flightRecordSizeHeader[0].size
				== flightRecordSizeHeader[2].size)
		{
			pointerToWrite = flightRecordSizeHeader[0].size;
			flightRecordOverflowFlagLow =
					flightRecordSizeHeader[0].overFlowFlag;
		}
		else if (flightRecordSizeHeader[1].size
				== flightRecordSizeHeader[2].size)
		{
			pointerToWrite = flightRecordSizeHeader[1].size;
			flightRecordOverflowFlagLow =
					flightRecordSizeHeader[1].overFlowFlag;
		}
		else
		{
			if (flightRecordSizeHeader[0].size < flightRecordSizeHeader[1].size)
			{
				if (flightRecordSizeHeader[0].size
						< flightRecordSizeHeader[2].size)
				{
					pointerToWrite = flightRecordSizeHeader[0].size;
					flightRecordOverflowFlagLow =
							flightRecordSizeHeader[0].overFlowFlag;
				}
				else
				{
					pointerToWrite = flightRecordSizeHeader[2].size;
					flightRecordOverflowFlagLow =
							flightRecordSizeHeader[2].overFlowFlag;
				}
			}
			else
			{
				if (flightRecordSizeHeader[1].size
						< flightRecordSizeHeader[2].size)
				{
					pointerToWrite = flightRecordSizeHeader[1].size;
					flightRecordOverflowFlagLow =
							flightRecordSizeHeader[1].overFlowFlag;
				}
				else
				{
					pointerToWrite = flightRecordSizeHeader[2].size;
					flightRecordOverflowFlagLow =
							flightRecordSizeHeader[2].overFlowFlag;
				}
			}
		}
	}
	else if (flightRecordSizeHeader[0].crc
			!= calculateCRC((UWord8*) &flightRecordSizeHeader[0],
					sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
	{
		if ((flightRecordSizeHeader[1].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[1],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
				&& (flightRecordSizeHeader[2].crc
						== calculateCRC((UWord8*) &flightRecordSizeHeader[2],
								sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[1].size;
			flightRecordOverflowFlagLow =
					flightRecordSizeHeader[1].overFlowFlag;
		}
		else if ((flightRecordSizeHeader[1].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[1],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[1].size;
			flightRecordOverflowFlagLow =
					flightRecordSizeHeader[1].overFlowFlag;
		}
		else if ((flightRecordSizeHeader[2].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[2],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[2].size;
			flightRecordOverflowFlagLow =
					flightRecordSizeHeader[2].overFlowFlag;
		}
		else
		{
			flightRecordOverflowFlagLow = 0;
			pointerToWrite = 0;
			error = -1;
		}
	}
	else if (flightRecordSizeHeader[1].crc
			!= calculateCRC((UWord8*) &flightRecordSizeHeader[1],
					sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
	{
		if ((flightRecordSizeHeader[0].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[0],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
				&& (flightRecordSizeHeader[2].crc
						== calculateCRC((UWord8*) &flightRecordSizeHeader[2],
								sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[2].size;
			flightRecordOverflowFlagLow =
					flightRecordSizeHeader[2].overFlowFlag;
		}
		else if ((flightRecordSizeHeader[0].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[0],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[0].size;
			flightRecordOverflowFlagLow =
					flightRecordSizeHeader[0].overFlowFlag;
		}
		else if ((flightRecordSizeHeader[2].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[2],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[2].size;
			flightRecordOverflowFlagLow =
					flightRecordSizeHeader[2].overFlowFlag;
		}
		else
		{
			flightRecordOverflowFlagLow = 0;
			pointerToWrite = 0;
			error = -1;
		}
	}
	else if (flightRecordSizeHeader[2].crc
			!= calculateCRC((UWord8*) &flightRecordSizeHeader[2],
					sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
	{
		if ((flightRecordSizeHeader[1].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[1],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
				&& (flightRecordSizeHeader[0].crc
						== calculateCRC((UWord8*) &flightRecordSizeHeader[0],
								sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[0].size;
			flightRecordOverflowFlagLow =
					flightRecordSizeHeader[0].overFlowFlag;
		}
		else if ((flightRecordSizeHeader[1].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[1],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[1].size;
			flightRecordOverflowFlagLow =
					flightRecordSizeHeader[1].overFlowFlag;
		}
		else if ((flightRecordSizeHeader[0].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[0],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[0].size;
			flightRecordOverflowFlagLow =
					flightRecordSizeHeader[0].overFlowFlag;
		}
		else
		{
			flightRecordOverflowFlagLow = 0;
			pointerToWrite = 0;
			error = -1;
		}
	}
	else
	{
		error = -1;
	}
	if ((uint64_t)file < 0 || ConstHigh::flightRecordAppendMode == false)
	{
		fclose(file);
		initializeFlightRecordLowFile(flighRecordPath, memoryFailure);
		countToWriteEncryptionHeaderLowTracks = 0;
		file = fopen(fileName.c_str(), "r+");

		if ((uint64_t)file <= 0 || ConstHigh::flightRecordAppendMode == false)
		{
			initializeFlightRecordLowFile(flighRecordPath, memoryFailure);
			countToWriteEncryptionHeaderLowTracks = 0;
		}
	}
	if ((pointerToWrite) + (sizeof(FlightRecordHeader) * 3) + localSize
			+ sizeof(flightRecordSizeHeader) > sizeInBytesTotal)
	{
		pointerToWrite = 0;
		countToWriteEncryptionHeaderLowTracks = 0;
		if (flightRecordOverflowFlagLow == 0)
		{
			flightRecordOverflowFlagLow = 1;
		}
	}
	if (countToWriteEncryptionHeaderLowTracks <= 0)
	{
		countToWriteEncryptionHeaderLowTracks = 20;
		randomNumber = (UWord64) rand();
		randomNumber = (randomNumber << 32 | rand());
		memset(&encryptionHeaderLow, 0, sizeof(EncryptionHeader));
		encryptionHeaderLow.Delimiter = 0xbeeffeeddeadbeef;
		memcpy(&encryptionHeaderLow.Hash, keyDigest,
				sizeof(encryptionHeaderLow.Hash));

		memcpy((void*) aesInitializationVectorLow, (void*) (&randomNumber), 8);
		memcpy((void*) (aesInitializationVectorLow + 8),
				(void*) (&randomNumber), 8);
		encryptionHeaderLow.InformationVector = randomNumber;
		fseek(file,
				(pointerToWrite % sizeInBytesTotal)
						+ (sizeof(FlightRecordSizeHeader) * 3), SEEK_SET);
		myInt = ftell(file);
		//printf("I am here for Hash: %d\n",myInt);
		fwrite(&encryptionHeaderLow, sizeof(EncryptionHeader), 1, file);
		pointerToWrite = pointerToWrite + (sizeof(EncryptionHeader));
	}

	if (ConstHigh::lowLogEncrypted == true)
	{
		cbcEncryptData(ptr, expandedKey, aesInitializationVectorLow, outputPtr,
				size);
	}
	else
	{
		memcpy(outputPtr, ptr, size);
	}

	//new logic

	dummyCRC = calculateCRC((UWord8*) &flightRecordSizeHeader[0],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));
	dummyCRC = calculateCRC((UWord8*) &flightRecordSizeHeader[1],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));
	dummyCRC = calculateCRC((UWord8*) &flightRecordSizeHeader[2],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));
	flightRecordSizeHeader[0].size = pointerToWrite + localSize;
	flightRecordSizeHeader[0].overFlowFlag = flightRecordOverflowFlagLow;
	flightRecordSizeHeader[0].crc = calculateCRC(
			(UWord8*) &flightRecordSizeHeader[0],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));

	flightRecordSizeHeader[1].size = pointerToWrite + localSize;
	flightRecordSizeHeader[1].overFlowFlag = flightRecordOverflowFlagLow;
	flightRecordSizeHeader[1].crc = calculateCRC(
			(UWord8*) &flightRecordSizeHeader[1],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));

	flightRecordSizeHeader[2].size = pointerToWrite + localSize;
	flightRecordSizeHeader[2].overFlowFlag = flightRecordOverflowFlagLow;
	flightRecordSizeHeader[2].crc = calculateCRC(
			(UWord8*) &flightRecordSizeHeader[2],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));

	fseek(file, 0, SEEK_SET);

	myInt = ftell(file);

	//printf("SB: %d SA:%d\n",pointerToWrite,flightRecordSizeHeader[2].size);
	//printf("I am here for Size: %d\n",myInt);
	myInt = ftell(file);
	fwrite(&flightRecordSizeHeader[0], sizeof(FlightRecordSizeHeader), 1, file);
	myInt = ftell(file);
	fwrite(&flightRecordSizeHeader[1], sizeof(FlightRecordSizeHeader), 1, file);
	myInt = ftell(file);
	fwrite(&flightRecordSizeHeader[2], sizeof(FlightRecordSizeHeader), 1, file);

	memset(flightRecordSizeHeader, 0, sizeof(flightRecordSizeHeader));

	usedFlightRecordSizeLow = pointerToWrite;
	dummyPtrToWrite = pointerToWrite % sizeInBytesTotal;
	fseek(file, (dummyPtrToWrite) + (sizeof(FlightRecordSizeHeader) * 3),
	SEEK_SET);
	//replace a with fr
	//new logic
	myInt = ftell(file);
	//printf("I am here to write: %d\n",myInt);
	fwrite(outputPtr, localSize, 1, file);
	//fwrite(ptr, localSize, 1, file);
	fclose(file);
	free(outputPtr);
	flightRecordCountLow++;
	countToWriteEncryptionHeaderLowTracks--;

	//just to remove warnings
	if (myInt < -100)
	{
		cout << "Error message" << myInt << dummyCRC << endl;
	}

}

void FileOutputFlightRecordHigh::initializeFlightRecordLowFile(
		string flighRecordPath, bool &memoryFailure)
{

	//Word8 systemCommand[50];
	//int * iterator;
	FILE *outfile;

	//iterator = malloc(SIZE_IN_BYTES);
	//memset((void*)iterator,0,SIZE_IN_BYTES);

	FlightRecordSizeHeader flightRecordSizeHeader[3];

	//outfile = fopen("FILE.txt", "w");
	//fwrite (iterator, SIZE_IN_BYTES, 1, outfile);
	//fclose (outfile);
	string fileName;
	if (ConstHigh::lowLogEncrypted == true)
	{
		fileName = flighRecordPath + "flightRecordLow.fre";
	}
	else
	{
		fileName = flighRecordPath + "flightRecordLow.fr";
	}

	//sprintf(systemCommand,"dd if=/dev/zero of=/tmp/cf/EncOut.fre bs=512 count=%d",SIZE_IN_BYTES/512);
	//system(systemCommand);

	flightRecordSizeHeader[0].crc = 0x0000FFFF;
	flightRecordSizeHeader[0].size = 0;
	flightRecordSizeHeader[1].crc = 0x0000FFFF;
	flightRecordSizeHeader[1].size = 0;
	flightRecordSizeHeader[2].crc = 0x0000FFFF;
	flightRecordSizeHeader[2].size = 0;

	outfile = fopen(fileName.c_str(), "w");

	if ((uint64_t)outfile <= 0)
	{
		//printf("FR unreachable\n");
		memoryFailure = true;
		return;
	}
	else
	{
		memoryFailure = false;
	}
	fwrite(&flightRecordSizeHeader[0], sizeof(flightRecordSizeHeader[1]), 1,
			outfile);
	fwrite(&flightRecordSizeHeader[1], sizeof(flightRecordSizeHeader[1]), 1,
			outfile);
	fwrite(&flightRecordSizeHeader[2], sizeof(flightRecordSizeHeader[1]), 1,
			outfile);
	fclose(outfile);
	//free(iterator);

}

void FileOutputFlightRecordHigh::writeTrackingDataFlightRecordToFile(
		UWord8 numberOfTracks, LibraryMetaClass *libraryMeta,
		string flighRecordPath, EmitterLog emitterLog, bool &memoryFailure)
{

	UWord32 i = 0;
	UWord16 crc = 0xffff;
	UWord16 *ptr = (UWord16*) &emitterLog;
	//UWord32 sizecheck = sizeof(EmitterLog);
	UWord32 sizeEmitter, sizeFR, sizeTotal;
	time_t epoch = time(NULL);

	//FILE *ourFile;
	//FILE *dummy;
	sizeEmitter = sizeof(EmitterLog);
	sizeFR = sizeof(FlightRecord);
	sizeTotal = ((sizeEmitter - (256 * sizeFR)) + (sizeFR * numberOfTracks));
	if (numberOfTracks > 0)
	{
		emitterLog.flightRecordHeader.Delimiter = 0xbeeffeeddeadbeef;
		emitterLog.flightRecordHeader.structureCount = numberOfTracks;
		emitterLog.flightRecordHeader.MessageType = 15;
		emitterLog.flightRecordHeader.RtcTime = (uint32_t) epoch;
		for (i = 10; i < sizeTotal / 2; i++)
		{
			crc ^= (UWord16) ptr[i];
		}

		emitterLog.flightRecordHeader.CRC = crc;
		//printf("CRC %d \n",emitterLog.CRC);

		//printf("FR N-T: %d\n",numberOfTracks);

		//sizeTotal = ((sizeEmitter-(256*sizeFR))+ (sizeFR * numberOfTracks));
		//ourFile = fopen("Out.txt", "a+");
		//fwrite(&emitterLog,sizeof(EmitterLog),1,ourFile);
		//fwrite(&emitterLog,sizeTotal,1,ourFile);
		//fclose(ourFile);

		//dummy=fopen("OutInd.txt", "w");
		//fwrite(&emitterLog,sizeof(EmitterLog),1,dummy);
		//fwrite(&emitterLog,sizeTotal,1,dummy);
		//fclose(dummy);

		writeEncryptedTrackingFlightRecord(libraryMeta, flighRecordPath,
				emitterLog, memoryFailure);
	}

}

void FileOutputFlightRecordHigh::writeEncryptedTrackingFlightRecord(
		LibraryMetaClass *libraryMeta, string flighRecordPath,
		EmitterLog emitterLog, bool &memoryFailure)
{
	UWord32 size = ((sizeof(EmitterLog) - (256 * sizeof(FlightRecord)))
			+ (sizeof(FlightRecord)
					* emitterLog.flightRecordHeader.structureCount));
	emitterLog.flightRecordHeader.CRC = calculateCRC((UWord8*) &emitterLog,
			size, sizeof(FlightRecordHeader));
	if (flightRecordAircraftNavigationInformationWritten == 0)
	{
		writeEncryptedFlightRecordAircraftNavigationInformation(libraryMeta,
				flighRecordPath, memoryFailure);
	}
	encryptAndWriteToFile((UWord8*) &emitterLog, size, libraryMeta,
			flighRecordPath, memoryFailure);

}

void FileOutputFlightRecordHigh::writeEncryptedFlightRecordAircraftNavigationInformation(
		LibraryMetaClass *libraryMeta, string flighRecordPath,
		bool &memoryFailure)
{

	time_t epoch = time(NULL);

	memset(&flightRecordAircraftNavigationInformation, 0,
			sizeof(flightRecordAircraftNavigationInformation));
	flightRecordAricraftNavigationInformationHeader.Delimiter =
			0xbeeffeeddeadbeef;
	flightRecordAricraftNavigationInformationHeader.structureCount =
			sizeof(flightRecordAircraftNavigationInformation);
	flightRecordAricraftNavigationInformationHeader.RtcTime = (uint32_t) epoch;
	flightRecordAricraftNavigationInformationHeader.MessageType = 30;
	flightRecordAircraftNavigationInformation.sessionId = globalSessionId;
	flightRecordAricraftNavigationInformationHeader.CRC = calculateCRC(
			(UWord8*) &flightRecordAircraftNavigationInformation,
			sizeof(flightRecordAircraftNavigationInformation), 0);

	encryptAndWriteToFile(
			(UWord8*) &flightRecordAricraftNavigationInformationHeader,
			sizeof(flightRecordAricraftNavigationInformationHeader),
			libraryMeta, flighRecordPath, memoryFailure);

	encryptAndWriteToFile((UWord8*) &flightRecordAircraftNavigationInformation,
			sizeof(flightRecordAircraftNavigationInformation), libraryMeta,
			flighRecordPath, memoryFailure);

	flightRecordAircraftNavigationInformationWritten = 1;

}

void FileOutputFlightRecordHigh::encryptAndWriteToFile(UWord8 *ptr,
		UWord32 size, LibraryMetaClass *libraryMeta, string flighRecordPath,
		bool &memoryFailure)
{

	FlightRecordSizeHeader flightRecordSizeHeader[3];
	L32 myInt = 0;
	UWord32 sizeInBytesTotal = SIZE_OF_FR * MB_TO_BYTES;
	UWord32 pointerToWrite = 0, dummyPtrToWrite = 0;
	FILE *file;
	UWord32 sizeCheck = size % 16;
	UWord32 localSize = size;
	UWord8 *outputPtr;
	UWord64 randomNumber = 0;
	Word32 error = 0;
	Word32 dummyCRC = 0;
	uint8_t *password, j = 0;
	string fileName;

	if (error == 0)
	{    //to remove warning
		error = 0;
	}

	password = libraryMeta->bufferForKey;
	for (j = 0; j < sizeof(decryptionKeyName); j++)
	{
		decryptionKeyName[j] = *password != 0 ? *password++ : 0;
	}

	memcpy(keyDigest, libraryMeta->keyDigest,
			SHA256_BLOCK_SIZE * sizeof(UWord8));
	if (ConstHigh::flightRecordEncrypted == true)
	{
		fileName = flighRecordPath + "flightRecordHigh.fre";
	}
	else
	{
		fileName = flighRecordPath + "flightRecordHigh.fr";
	}

	if (totalFlightRecordSize == 0)
	{
		totalFlightRecordSize = sizeInBytesTotal;
	}
	memset(flightRecordSizeHeader, 0, sizeof(flightRecordSizeHeader));
	if (sizeCheck != 0)
	{
		localSize = localSize - sizeCheck + 16;
	}
	outputPtr = (UWord8*) calloc(localSize, 1);

	file = fopen(fileName.c_str(), "r+");

	if ((uint64_t)file <= 0 || ConstHigh::flightRecordAppendMode == false)
	{
		initializeFlightRecordFile(flighRecordPath, memoryFailure);
		countToWriteEncryptionHeader = 0;
		file = fopen(fileName.c_str(), "r+");
		if ((uint64_t)file <= 0)
		{
			//printf("FR unreachable\n");
			memoryFailure = true;
			return;
		}
	}

	if (flightRecordCount == 0)
	{

		srand((unsigned) time(&randomNumberSeed));
		sha256_init(&ctx);
		sha256_update(&ctx, decryptionKeyName, sizeof(decryptionKeyName));
		sha256_final(&ctx, keyDigest);
		aesSetupEncrypt(expandedKey, decryptionKeyName);
		//key change pe frcount = 0
	}
	fread(&flightRecordSizeHeader[0], 1, sizeof(FlightRecordSizeHeader), file);

	fread(&flightRecordSizeHeader[1], 1, sizeof(FlightRecordSizeHeader), file);

	fread(&flightRecordSizeHeader[2], 1, sizeof(FlightRecordSizeHeader), file);

	dummyCRC = calculateCRC((UWord8*) &flightRecordSizeHeader[0],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));
	dummyCRC = calculateCRC((UWord8*) &flightRecordSizeHeader[1],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));
	dummyCRC = calculateCRC((UWord8*) &flightRecordSizeHeader[2],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));

	if ((flightRecordSizeHeader[0].crc
			== calculateCRC((UWord8*) &flightRecordSizeHeader[0],
					sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
			&& (flightRecordSizeHeader[1].crc
					== calculateCRC((UWord8*) &flightRecordSizeHeader[1],
							sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
			&& (flightRecordSizeHeader[2].crc
					== calculateCRC((UWord8*) &flightRecordSizeHeader[2],
							sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
	{
		if (flightRecordSizeHeader[0].size + flightRecordSizeHeader[1].size
				- 2 * flightRecordSizeHeader[2].size == 0)
		{
			pointerToWrite = flightRecordSizeHeader[0].size;
			flightRecordOverflowFlag = flightRecordSizeHeader[0].overFlowFlag;
		}
		else if (flightRecordSizeHeader[0].size
				== flightRecordSizeHeader[1].size)
		{
			pointerToWrite = flightRecordSizeHeader[0].size;
			flightRecordOverflowFlag = flightRecordSizeHeader[0].overFlowFlag;
		}
		else if (flightRecordSizeHeader[0].size
				== flightRecordSizeHeader[2].size)
		{
			pointerToWrite = flightRecordSizeHeader[0].size;
			flightRecordOverflowFlag = flightRecordSizeHeader[0].overFlowFlag;
		}
		else if (flightRecordSizeHeader[1].size
				== flightRecordSizeHeader[2].size)
		{
			pointerToWrite = flightRecordSizeHeader[1].size;
			flightRecordOverflowFlag = flightRecordSizeHeader[1].overFlowFlag;
		}
		else
		{
			if (flightRecordSizeHeader[0].size < flightRecordSizeHeader[1].size)
			{
				if (flightRecordSizeHeader[0].size
						< flightRecordSizeHeader[2].size)
				{
					pointerToWrite = flightRecordSizeHeader[0].size;
					flightRecordOverflowFlag =
							flightRecordSizeHeader[0].overFlowFlag;
				}
				else
				{
					pointerToWrite = flightRecordSizeHeader[2].size;
					flightRecordOverflowFlag =
							flightRecordSizeHeader[2].overFlowFlag;
				}
			}
			else
			{
				if (flightRecordSizeHeader[1].size
						< flightRecordSizeHeader[2].size)
				{
					pointerToWrite = flightRecordSizeHeader[1].size;
					flightRecordOverflowFlag =
							flightRecordSizeHeader[1].overFlowFlag;
				}
				else
				{
					pointerToWrite = flightRecordSizeHeader[2].size;
					flightRecordOverflowFlag =
							flightRecordSizeHeader[2].overFlowFlag;
				}
			}
		}
	}
	else if (flightRecordSizeHeader[0].crc
			!= calculateCRC((UWord8*) &flightRecordSizeHeader[0],
					sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
	{
		if ((flightRecordSizeHeader[1].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[1],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
				&& (flightRecordSizeHeader[2].crc
						== calculateCRC((UWord8*) &flightRecordSizeHeader[2],
								sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[1].size;
			flightRecordOverflowFlag = flightRecordSizeHeader[1].overFlowFlag;
		}
		else if ((flightRecordSizeHeader[1].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[1],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[1].size;
			flightRecordOverflowFlag = flightRecordSizeHeader[1].overFlowFlag;
		}
		else if ((flightRecordSizeHeader[2].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[2],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[2].size;
			flightRecordOverflowFlag = flightRecordSizeHeader[2].overFlowFlag;
		}
		else
		{
			flightRecordOverflowFlag = 0;
			pointerToWrite = 0;
			error = -1;
		}
	}
	else if (flightRecordSizeHeader[1].crc
			!= calculateCRC((UWord8*) &flightRecordSizeHeader[1],
					sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
	{
		if ((flightRecordSizeHeader[0].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[0],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
				&& (flightRecordSizeHeader[2].crc
						== calculateCRC((UWord8*) &flightRecordSizeHeader[2],
								sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[2].size;
			flightRecordOverflowFlag = flightRecordSizeHeader[2].overFlowFlag;
		}
		else if ((flightRecordSizeHeader[0].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[0],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[0].size;
			flightRecordOverflowFlag = flightRecordSizeHeader[0].overFlowFlag;
		}
		else if ((flightRecordSizeHeader[2].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[2],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[2].size;
			flightRecordOverflowFlag = flightRecordSizeHeader[2].overFlowFlag;
		}
		else
		{
			flightRecordOverflowFlag = 0;
			pointerToWrite = 0;
			error = -1;
		}
	}
	else if (flightRecordSizeHeader[2].crc
			!= calculateCRC((UWord8*) &flightRecordSizeHeader[2],
					sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
	{
		if ((flightRecordSizeHeader[1].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[1],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32)))
				&& (flightRecordSizeHeader[0].crc
						== calculateCRC((UWord8*) &flightRecordSizeHeader[0],
								sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[0].size;
			flightRecordOverflowFlag = flightRecordSizeHeader[0].overFlowFlag;
		}
		else if ((flightRecordSizeHeader[1].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[1],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[1].size;
			flightRecordOverflowFlag = flightRecordSizeHeader[1].overFlowFlag;
		}
		else if ((flightRecordSizeHeader[0].crc
				== calculateCRC((UWord8*) &flightRecordSizeHeader[0],
						sizeof(FlightRecordSizeHeader), sizeof(UWord32))))
		{
			pointerToWrite = flightRecordSizeHeader[0].size;
			flightRecordOverflowFlag = flightRecordSizeHeader[0].overFlowFlag;
		}
		else
		{
			flightRecordOverflowFlag = 0;
			pointerToWrite = 0;
			error = -1;
		}
	}
	else
	{
		error = -1;
	}
	if ((uint64_t)file < 0 || ConstHigh::flightRecordAppendMode == false)
	{
		fclose(file);
		initializeFlightRecordFile(flighRecordPath, memoryFailure);
		countToWriteEncryptionHeader = 0;
		file = fopen(fileName.c_str(), "r+");

		if ((uint64_t)file <= 0 || ConstHigh::flightRecordAppendMode == false)
		{
			initializeFlightRecordFile(flighRecordPath, memoryFailure);
			countToWriteEncryptionHeader = 0;
		}
	}
	if ((pointerToWrite) + (sizeof(FlightRecordHeader) * 3) + localSize
			+ sizeof(flightRecordSizeHeader) > sizeInBytesTotal)
	{
		pointerToWrite = 0;
		countToWriteEncryptionHeader = 0;
		if (flightRecordOverflowFlag == 0)
		{
			flightRecordOverflowFlag = 1;
		}
	}
	if (countToWriteEncryptionHeader <= 0)
	{
		countToWriteEncryptionHeader = 20;
		randomNumber = (UWord64) rand();
		randomNumber = (randomNumber << 32 | rand());
		memset(&encryptionHeader, 0, sizeof(EncryptionHeader));
		encryptionHeader.Delimiter = 0xbeeffeeddeadbeef;
		memcpy(&encryptionHeader.Hash, keyDigest,
				sizeof(encryptionHeader.Hash));

		memcpy((void*) aesInitializationVector, (void*) (&randomNumber), 8);
		memcpy((void*) (aesInitializationVector + 8), (void*) (&randomNumber),
				8);
		encryptionHeader.InformationVector = randomNumber;
		fseek(file,
				(pointerToWrite % sizeInBytesTotal)
						+ (sizeof(FlightRecordSizeHeader) * 3), SEEK_SET);
		myInt = ftell(file);
		//printf("I am here for Hash: %d\n",myInt);
		fwrite(&encryptionHeader, sizeof(encryptionHeader), 1, file);
		pointerToWrite = pointerToWrite + (sizeof(EncryptionHeader));
	}

	if (ConstHigh::flightRecordEncrypted == true)
	{
		cbcEncryptData(ptr, expandedKey, aesInitializationVector, outputPtr,
				size);
	}
	else
	{
		memcpy(outputPtr, ptr, size);
	}

	//new logic

	dummyCRC = calculateCRC((UWord8*) &flightRecordSizeHeader[0],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));
	dummyCRC = calculateCRC((UWord8*) &flightRecordSizeHeader[1],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));
	dummyCRC = calculateCRC((UWord8*) &flightRecordSizeHeader[2],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));
	flightRecordSizeHeader[0].size = pointerToWrite + localSize;
	flightRecordSizeHeader[0].overFlowFlag = flightRecordOverflowFlag;
	flightRecordSizeHeader[0].crc = calculateCRC(
			(UWord8*) &flightRecordSizeHeader[0],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));

	flightRecordSizeHeader[1].size = pointerToWrite + localSize;
	flightRecordSizeHeader[1].overFlowFlag = flightRecordOverflowFlag;
	flightRecordSizeHeader[1].crc = calculateCRC(
			(UWord8*) &flightRecordSizeHeader[1],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));

	flightRecordSizeHeader[2].size = pointerToWrite + localSize;
	flightRecordSizeHeader[2].overFlowFlag = flightRecordOverflowFlag;
	flightRecordSizeHeader[2].crc = calculateCRC(
			(UWord8*) &flightRecordSizeHeader[2],
			sizeof(FlightRecordSizeHeader), sizeof(UWord32));

	fseek(file, 0, SEEK_SET);

	myInt = ftell(file);

	//printf("SB: %d SA:%d\n",pointerToWrite,flightRecordSizeHeader[2].size);
	//printf("I am here for Size: %d\n",myInt);
	myInt = ftell(file);
	fwrite(&flightRecordSizeHeader[0], sizeof(FlightRecordSizeHeader), 1, file);
	myInt = ftell(file);
	fwrite(&flightRecordSizeHeader[1], sizeof(FlightRecordSizeHeader), 1, file);
	myInt = ftell(file);
	fwrite(&flightRecordSizeHeader[2], sizeof(FlightRecordSizeHeader), 1, file);

	memset(flightRecordSizeHeader, 0, sizeof(flightRecordSizeHeader));

	usedFlightRecordSize = pointerToWrite;
	dummyPtrToWrite = pointerToWrite % sizeInBytesTotal;
	fseek(file, (dummyPtrToWrite) + (sizeof(FlightRecordSizeHeader) * 3),
	SEEK_SET);
	//replace a with fr
	//new logic
	myInt = ftell(file);
	//printf("I am here to write: %d\n",myInt);
	fwrite(outputPtr, localSize, 1, file);
	//fwrite(ptr, localSize, 1, file);
	fclose(file);
	free(outputPtr);
	flightRecordCount++;
	countToWriteEncryptionHeader--;

	//just to remove warnings
	if (myInt < -100)
	{
		cout << "Error message" << myInt << dummyCRC << endl;
	}

}

void FileOutputFlightRecordHigh::initializeFlightRecordFile(
		string flighRecordPath, bool &memoryFailure)
{

	//Word8 systemCommand[50];
	//int * iterator;
	FILE *outfile;

	//iterator = malloc(SIZE_IN_BYTES);
	//memset((void*)iterator,0,SIZE_IN_BYTES);

	FlightRecordSizeHeader flightRecordSizeHeader[3];

	//outfile = fopen("FILE.txt", "w");
	//fwrite (iterator, SIZE_IN_BYTES, 1, outfile);
	//fclose (outfile);
	string fileName;
	if (ConstHigh::flightRecordEncrypted == true)
	{
		fileName = flighRecordPath + "flightRecordHigh.fre";
	}
	else
	{
		fileName = flighRecordPath + "flightRecordHigh.fr";
	}

	//sprintf(systemCommand,"dd if=/dev/zero of=/tmp/cf/EncOut.fre bs=512 count=%d",SIZE_IN_BYTES/512);
	//system(systemCommand);

	flightRecordSizeHeader[0].crc = 0x0000FFFF;
	flightRecordSizeHeader[0].size = 0;
	flightRecordSizeHeader[1].crc = 0x0000FFFF;
	flightRecordSizeHeader[1].size = 0;
	flightRecordSizeHeader[2].crc = 0x0000FFFF;
	flightRecordSizeHeader[2].size = 0;

	outfile = fopen(fileName.c_str(), "w");

	if ((uint64_t)outfile <= 0)
	{
		//printf("FR unreachable\n");
		memoryFailure = true;
		return;
	}
	else
	{
		memoryFailure = false;
	}
	fwrite(&flightRecordSizeHeader[0], sizeof(flightRecordSizeHeader[1]), 1,
			outfile);
	fwrite(&flightRecordSizeHeader[1], sizeof(flightRecordSizeHeader[1]), 1,
			outfile);
	fwrite(&flightRecordSizeHeader[2], sizeof(flightRecordSizeHeader[1]), 1,
			outfile);
	fclose(outfile);
	//free(iterator);
}

void FileOutputFlightRecordHigh::fillMaintenaceFlightRecordHeader()
{
	time_t epoch = time(NULL);

	flightRecordMaintainanceHeader.Delimiter = 0xbeeffeeddeadbeef;
	flightRecordMaintainanceHeader.MessageType = 2;
	flightRecordMaintainanceHeader.structureCount = 0;
	flightRecordMaintainanceHeader.CRC = 0;
	flightRecordMaintainanceHeader.RtcTime = (uint32_t) epoch;
}

void FileOutputFlightRecordHigh::writeEncryptedMaintenanceFlightRecord(
		UWord8 *mflData, LibraryMetaClass *libraryMeta, string flighRecordPath,
		bool &memoryFailure)
{

	time_t epoch = time(NULL);

	flightRecordMaintainanceHeader.Delimiter = 0xbeeffeeddeadbeef;
	flightRecordMaintainanceHeader.structureCount =
			flightRecordMaintainanceHeader.structureCount * 5;
	flightRecordMaintainanceHeader.RtcTime = (uint32_t) epoch;
	flightRecordMaintainanceHeader.MessageType = 16;

	flightRecordMaintainanceHeader.CRC = calculateCRC((UWord8*) mflData,
			(flightRecordMaintainanceHeader.structureCount), 0);
	if (flightRecordMaintainanceHeader.structureCount > 0)
	{
		if (flightRecordAircraftNavigationInformationWritten == 0)
		{
			writeEncryptedFlightRecordAircraftNavigationInformation(libraryMeta,
					flighRecordPath, memoryFailure);
		}
		encryptAndWriteToFile((UWord8*) &flightRecordMaintainanceHeader,
				sizeof(flightRecordMaintainanceHeader), libraryMeta,
				flighRecordPath, memoryFailure);
		encryptAndWriteToFile((UWord8*) &mflData[0],
				flightRecordMaintainanceHeader.structureCount, libraryMeta,
				flighRecordPath, memoryFailure);
	}

}

void FileOutputFlightRecordHigh::writeEncryptedEventFlightRecord(UWord16 event,
		UWord16 size, UWord8 *ptrData, LibraryMetaClass *libraryMeta,
		string flighRecordPath, bool &memoryFailure)
{

	time_t epoch = time(NULL);

	memset(&flightRecordEventHeader, 0, sizeof(flightRecordEventHeader));
	flightRecordEventHeader.Delimiter = 0xbeeffeeddeadbeef;
	flightRecordEventHeader.MessageType = event;
	flightRecordEventHeader.structureCount = size;
	flightRecordEventHeader.RtcTime = (uint32_t) epoch;
	flightRecordEventHeader.CRC = calculateCRC((UWord8*) ptrData, size, 0);
	if (flightRecordAircraftNavigationInformationWritten == 0)
	{
		writeEncryptedFlightRecordAircraftNavigationInformation(libraryMeta,
				flighRecordPath, memoryFailure);
	}
	encryptAndWriteToFile((UWord8*) &flightRecordEventHeader,
			sizeof(flightRecordEventHeader), libraryMeta, flighRecordPath,
			memoryFailure);
	encryptAndWriteToFile((UWord8*) ptrData, size, libraryMeta, flighRecordPath,
			memoryFailure);

}

UWord8 FileOutputFlightRecordHigh::checkForSsdOnHardware(void) // function to check wetherthe SSD is connected on Hardware
{
	FILE *fp;
	//char * a;
	Word8 path[1035];
	Word8 returnVar;
	Word8 dummyCommand[150] =
	{ ' ' };
	//Word8  returnError = 0;

	// updateCurrentTracevariable(tracerId, "SSD");

	/* Open the command for reading. */
	fflush(NULL);
	sprintf(dummyCommand, "fdisk -l | grep /dev/sda1 > /out.txt");
	returnVar = system(dummyCommand);

	fp = fopen("/out.txt", "r");
	memset(path, 0, sizeof(path));
	if (fp == NULL)
	{
		printf("Failed to run command\n");
		returnVar = 0;
	}
	else
	{
		fread(path, 9, 1, fp);
		// printf("%s\n", path);
		if (!(strcmp(path, "/dev/sda1")))
		{

			returnVar = 1;
		}
		else
		{
			// printf("No CF Card\n");
			returnVar = 0;
		}
		/* close */
		pclose(fp);

		remove("out.txt");

	}
	// updatePreviousTraceVariable(tracerId, "SSD");
	return returnVar;
}

UWord8 FileOutputFlightRecordHigh::checkIfSSDIsMounted(string dev_path) // function to check wetherthe SSD is mounted as a filesystem
{

	struct stat mountpoint;
	struct stat parent;

	Word8 parentPath[20];
	UWord8 mountStatus = 0;

	//updateCurrentTracevariable(tracerId, "SSD");

	sprintf(parentPath, "%s", dev_path.c_str());
	sprintf(parentPath + strlen(dev_path.c_str()), "/..");

	/* Get the stat structure of the directory...*/
	if (stat(dev_path.c_str(), &mountpoint) == -1)
	{
		perror("failed to stat mountpoint:");
		// exit(EXIT_FAILURE);
	}

	/* ... and its parent. */
	if (stat(parentPath, &parent) == -1)
	{
		perror("failed to stat parent:");
		// exit(EXIT_FAILURE);
	}

	/* Compare the st_dev fields in the results: if they are
	 equal, then both the directory and its parent belong
	 to the same filesystem, and so the directory is not
	 currently a mount point.
	 */
	if (mountpoint.st_dev == parent.st_dev)
	{
		mountStatus = 0;
	}
	else
	{
		mountStatus = 1;
	}
	//updatePreviousTraceVariable(tracerId, "SSD");
	return mountStatus;

}

