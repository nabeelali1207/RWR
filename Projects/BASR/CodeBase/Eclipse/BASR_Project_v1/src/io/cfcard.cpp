/*
 * cfcard.cpp
 *
 *  Created on: Feb 8, 2021
 *      Author: saad
 */

#define CFCARD_CPP_

#include "cfcard.h"

//This function is used to restart CF Card controller
UWord8 CfCard::resetCfCardController()
{
	UWord32 fd;
//	const char filePath1[] = "/sys/bus/usb/devices/usb1/authorized";
	endTimeCfControllerReset = Utility::getSystemTime();

	timeDifferenceCfControllerReset = (endTimeCfControllerReset
			- startTimeCfControllerReset)/1000000;
	//checks if 60 seconds have passed since last reset
	if (timeDifferenceCfControllerReset > 60)
	{
		cfCardMounted = 0;
		//opens the file to USB controller of linux
		fd = open("/sys/bus/usb/devices/usb1/authorized", O_WRONLY);
		if (fd < 0)
		{
			printf("CF Card ERROR:\tGPIO Export Failed\n\r");
			return 0;
		}
		//resets the file i.e. restarts the CF Card Controller
		write(fd, "0", 1);
		write(fd, "1", 1);
		close(fd);
		printf("CF restarted\n\r");
		resetCfFlag = 1;
		startTimeCfControllerReset = Utility::getSystemTime();
		return 1;
	}
	return 0;
}

//This function checks if CF Card is connected with our system
//It goes as follows, checks for disk devices with FAT16 setting,
//as CF Card is the only device with FAT16 format, all others being
//ext4 format, we pluck its name out to mount it on a folder later on.
UWord8 CfCard::checkCfCardExistance()
{
	FILE *fp;
	Word8 path[1035];
	Word8 returnVar;
	Word8 dummyCommand[150] =
	{ ' ' };
//    Word8  returnError = 0;

	/* Open the command for reading. */
	//This cleans the output from old cached data/messages
	fflush(NULL);
	//Following is the command used to search for CF and
	//result is stored in out.txt
//	sprintf(dummyCommand, "fdisk -l | grep FAT16 > /out.txt");
	sprintf(dummyCommand, "fdisk -l | grep /dev/sd?? > /out.txt");
	//returnError = system(dummyCommand);
	//we run the command
	system(dummyCommand);
	fp = fopen("/out.txt", "r");
	if (fp == NULL)
	{
		printf("Failed to run command\n");
		returnVar = 0;
	}
	else
	{
		//as CF Card alwars comes as /dev/sd*#,
		//* being a,b,c,d; # being 1 to n
		fread(path, 10, 1, fp);
		//checks if path is legit
		if (path[0] == '/')
		{
			memset(cfCardName, 0, sizeof(cfCardName));
//            printf("CF Card Present\n");
			//copies the name
			cfCardName[0] = path[5];
			cfCardName[1] = path[6];
			cfCardName[2] = path[7];
			cfCardName[3] = path[8];
			cfCardName[4] = '\0';
			cfCardNumber[0] = path[7];
			//printf("\n%s\n", cfCardName);
			returnVar = 1;
			//cfCardMounted = 0;
		}
		else
		{
			printf("No CF Card\n");
			returnVar = 0;
			cfCardMounted = 0;
		}
		/* close */
		pclose(fp);
	}
	return returnVar;
}

//This function is main CF Card logic, i.e. it makes CF Card
//hot-swappabale and enables system to access CF Card even if
//is connected after startup
void CfCard::mainProcessingCfCardCheckLogic()
{
	if (ConstHigh::simOnActualHwFlag == 1)
	{
		endTimeCfCheck = Utility::getSystemTime();
		timeDifferenceCfCheck =
				(endTimeCfCheck - startTimeCfCheck)/1000000;
		//checks if 5 seconds have passed
		if (timeDifferenceCfCheck > 5)
		{
			//Checks if CF Card exists
			foundCfFlag = checkCfCardExistance();
			if (foundCfFlag == 1)
			{

				//if CF Card is connected and not mounted to filesystem
				if (cfCardMounted == 0)
				{
					//Mounting CF Card to /tmp/cf
					mountCfCard();
					resetCfFlag = 0;
				}
			}
			else
			{
				//if CF Card is not found, CF Card controller is reset
				resetCfCardController();
			}
			startTimeCfCheck = Utility::getSystemTime();
		}
	}
}

//This function checks if CF card is connected with system. If so,
//it mounts the CF Card and reads Keys from the CF Card
void CfCard::startUpCfCardCheckLogic()
{
	//checks if CF card exists
	foundCfFlag = checkCfCardExistance();
	if (foundCfFlag == 1)
	{
		//mounts CF Card to /tmp/cf
		mountCfCard();
		//extracts keynames from /tmp/cf
		extractKeyNamesFromCfCard();
	}
}

//This function makes a directory as /tmp/cf and mounts CF Card
//to /tmp/cf. Later it makes a testing.txt file used to check
//CF Card during Self Tests
void CfCard::mountCfCard()
{
	FILE *ptr;
	Word8 dummyCommand[100] =
	{ ' ' };
	Word8 systemCommand[50];
	Word8 filename[30];
	if (ConstHigh::simOnActualHwFlag == 1)
	{
		sprintf(dummyCommand, "mkdir /tmp/%s", cfCardName);
		strcpy(systemCommand, dummyCommand);
		system(systemCommand);
		sprintf(dummyCommand, "mount /dev/%s /tmp/%s", cfCardName, cfCardName);
		strcpy(systemCommand, dummyCommand);
		system(systemCommand);
		sprintf(filename, "/tmp/%s/testing.txt", cfCardName);
		ptr = fopen(filename, "rb");
		if (ptr == NULL)
		{
			sprintf(systemCommand, "touch %s", filename);
			system(systemCommand);
			ptr = fopen(filename, "rb");
			if (ptr == NULL)
			{
				cfCardMounted = 0;
			}
			else
			{
				cfCardMounted = 1;

			}

		}
		else
		{
			cfCardMounted = 1;
		}
	}
}

//This function unmounts CF Card and deletes the directory
void CfCard::unmountCfCard()
{
	Word8 systemCommand[50];

	sprintf(systemCommand, "umount /tmp/%s", cfCardName);
	system(systemCommand);
	sprintf(systemCommand, "rmdir /tmp/%s", cfCardName);
	system(systemCommand);
}

//This function overwrites all Keys with 0's in CF Card and emmc
void CfCard::zeroizeKeys()
{
	Word8 filenameWithPath[100] =
	{ ' ' };
	UWord8 arr[32];
	int keyFile;
	UWord32 i = 0;
	UWord32 numberOfKeysInEmmc = 0;
	UWord32 numberOfKeysInCfCard = 0;
	//finds number of keys in emmc
	numberOfKeysInEmmc = returnNumberOfKeysInEmmc();
	//finds number of keys in CF Card
	numberOfKeysInCfCard = showKeyInCfCard();
	//array of 0's to overwrite keys
	memset(&arr[0], 0, 32);
	//a loop on all keys in EMMC to be overwritten
	for (i = 0; i < numberOfKeysInEmmc; i++)
	{
		printf("KeyName: %s\n", &keyFilenamesInEmmc[i - 1][0]);
		sprintf(filenameWithPath, "/tmp/emmc/%s.key",
				&keyFilenamesInEmmc[i][0]);
		keyFile = open(filenameWithPath, O_WRONLY | O_SYNC);
		write(keyFile, (UWord8*) &arr, sizeof(arr));
		close(keyFile);

		fsync(keyFile);

		keyFile = open(filenameWithPath, O_RDONLY);
		close(keyFile);
	}

	if (foundCfFlag == 1)
	{
		//a loop on all keys in CF Card to be overwritten
		for (i = 0; i < numberOfKeysInCfCard; i++)
		{
			printf("KeyName: %s\n", &keyFilenamesInCfCard[i - 1][0]);
			sprintf(filenameWithPath, "/tmp/%s/%s.key", cfCardName,
					&keyFilenamesInCfCard[i][0]);
			keyFile = open(filenameWithPath, O_WRONLY | O_SYNC);
			write(keyFile, (UWord8*) &arr, sizeof(arr));
			close(keyFile);

			fsync(keyFile);

			keyFile = open(filenameWithPath, O_RDONLY);
			close(keyFile);
		}
	}
	usleep(10);
}

//This function traverses through /tmp/emmc/ and returns number of Keys present
Word32 CfCard::returnNumberOfKeysInEmmc()
{
	struct dirent *dir;
	Word32 numberOfKeys = 0;
	DIR *directory;
	Word8 *filenamePtr, *extensionPtr;
	Word32 extensionComparison;
	directory = opendir("/tmp/emmc/");
	if (directory)
	{
		//printf("\033[2J");
		//printf("------------\n");
		//loop on  all files in /tmp/emmc
		while ((dir = readdir(directory)) != NULL)
		{
			filenamePtr = strtok(dir->d_name, ".");
			extensionPtr = strtok(NULL, ".");
			if (extensionPtr != NULL)
			{
				extensionComparison = strcmp(extensionPtr, "key");
				if (extensionComparison == 0)
				{
					numberOfKeys++;
				}
			}

		}
		closedir(directory);

	}
	return numberOfKeys;
}

//This function traverses through /tmp/cf/ and returns number of
//Keys present and their names are copied to filename array
Word32 CfCard::showKeyInCfCard()
{
	DIR *directory;
	Word8 *filenamePtr = 0, *extensionPtr = 0;
	Word32 extensionComparison;
	Word32 numberOfFiles = 1;
	struct dirent *dir;
	char systemCommand[100];
	sprintf(systemCommand, "/tmp/%s", cfCardName);
	directory = opendir(systemCommand);
	if (directory)
	{
		//printf("\033[2J");
		printf("------------\n");
		printf("List of Keys in CF card \n");
		while ((dir = readdir(directory)) != NULL)
		{
			filenamePtr = strtok(dir->d_name, ".");
			extensionPtr = strtok(NULL, ".");
			if (extensionPtr != NULL)
			{
				extensionComparison = strcmp(extensionPtr, "key");
				if (extensionComparison == 0)
				{
					printf("%d.\t", numberOfFiles);
					printf("%s\n", filenamePtr);
					strcpy(filenameReply.name[numberOfFiles - 1], filenamePtr);
					numberOfFiles++;
				}
			}
			if (numberOfFiles > 7)
			{
				break;
			}

		}
		closedir(directory);
		numberOfFiles--;
	}
	else
	{
		numberOfFiles = 0;
	}
	return numberOfFiles;
}

//This function corelates Keynames to their numbers i.e.
//1 to 6 are key numbers and names are associated w.r.t.
//to their nomenclature
void CfCard::extractKeyNamesFromCfCard()
{
	Word32 i = 0;
	Word8 *filename = 0;
	memset(&keyFilenamesInCfCard[0][0], 0, sizeof(keyFilenamesInCfCard));
	numberOfKeysInCfCard = showKeyInCfCard();
	if (numberOfKeysInCfCard > 6)
	{
		numberOfKeysInCfCard = 6;
	}
	for (i = 1; i <= numberOfKeysInCfCard; i++)
	{
		filename = (Word8*) findKeyInCfCard(i);
		memset(&keyFilenamesInCfCard[i - 1][0], 0,
				sizeof(keyFilenamesInCfCard[i - 1]));
		strcpy(&keyFilenamesInCfCard[i - 1][0], filename);
		//printf("KeyName: %s\n",&keyFilenamesInCfCard[i-1][0]);
	}
}

//This function corelates number to its keyname as number is a part
//of Keyname
Word8* CfCard::findKeyInCfCard(Word32 number)
{
	DIR *directory;
	Word8 *filenamePtr = 0, *extensionPtr = 0, *returnPtr = 0;
	Word32 extensionComparison;
	Word32 i = 1;
	struct dirent *dir;
	char systemCommand[100];
	sprintf(systemCommand, "/tmp/%s", cfCardName);
	directory = opendir(systemCommand);
	if (directory)
	{
		while ((dir = readdir(directory)) != NULL)
		{
			filenamePtr = strtok(dir->d_name, ".");
			extensionPtr = strtok(NULL, ".");
			if (extensionPtr != NULL)
			{
				extensionComparison = strcmp(extensionPtr, "key");
				if (extensionComparison == 0)
				{
					if (i == number)
					{
						returnPtr = filenamePtr;
					}
					i++;
				}
			}

		}
		closedir(directory);
	}
	return (Word8*) returnPtr;
}

//This function corelates Keynames to their numbers i.e.
//1 to 6 are key numbers and names are associated w.r.t.
//to their nomenclature
void CfCard::extractKeyNamesFromEmmc()
{
	Word32 i = 0;
	Word8 *filename = 0;
	//memset(&keyFilenamesInEmmc[0][0],0,sizeof(keyFilenamesInEmmc));
	numberOfKeysInEmmc = returnNumberOfKeysInEmmc();
	if (numberOfKeysInEmmc > 6)
	{
		numberOfKeysInEmmc = 6;
	}
	for (i = 1; i <= numberOfKeysInEmmc; i++)
	{
		filename = (Word8*) findKeyInEmmc(i);
		//memset(&keyFilenamesInEmmc[i-1][0],0,sizeof(keyFilenamesInEmmc[i-1]));
		strcpy(&keyFilenamesInEmmc[i - 1][0], filename);
		//printf("KeyName: %s\n",&keyFilenamesInEmmc[i-1][0]);
	}
}

//This function corelates number to its keyname as number is a part
//of Keyname
Word8* CfCard::findKeyInEmmc(Word32 number)
{
	DIR *directory;
	Word8 *filenamePtr = 0, *extensionPtr = 0, *returnPtr = 0;
	Word32 extensionComparison;
	Word32 i = 1;
	struct dirent *dir;
	directory = opendir("/tmp/emmc/");
	if (directory)
	{
		while ((dir = readdir(directory)) != NULL)
		{
			filenamePtr = strtok(dir->d_name, ".");
			extensionPtr = strtok(NULL, ".");
			if (extensionPtr != NULL)
			{
				extensionComparison = strcmp(extensionPtr, "key");
				if (extensionComparison == 0)
				{
					if (i == number)
					{
						returnPtr = filenamePtr;
					}
					i++;
				}
			}

		}
		closedir(directory);
	}
	return (Word8*) returnPtr;
}

//This fuction calculates Hash of all the keys present in emmc
void CfCard::calculateHashOfKeysInEmmc()
{
	Word8 filenameWithPath[100] =
	{ ' ' };
	FILE *keyFile;
	Word8 *password = 0;
	uint8_t i = 0, j = 0, SIZE_OF_KEY = 33;
	SHA256_CTX ctx;
	L32 lSize;
	for (i = 0; i < numberOfKeysInEmmc; i++)
	{
		sprintf(filenameWithPath, "/tmp/emmc/%s.key",
				&keyFilenamesInEmmc[i][0]);
		keyFile = fopen(filenameWithPath, "rb");
		if (!keyFile)
		{
			continue;
		}
		fseek(keyFile, 0L, SEEK_END);
		lSize = ftell(keyFile);
		if (lSize > SIZE_OF_KEY)
		{
			continue;
		}
		rewind(keyFile);

		memset(bufferForKey, 0, sizeof(0));
		if (1 != fread(bufferForKey, lSize, 1, keyFile))
		{
			fclose(keyFile);
			continue;
		}
		fclose(keyFile);
		password = bufferForKey;
		for (j = 0; j < sizeof(keys[i]); j++)
		{
			keys[i][j] = *password != 0 ? *password++ : 0;
		}
		sha256_init(&ctx);
		sha256_update(&ctx, keys[i], sizeof(keys[i]));
		sha256_final(&ctx, (UWord8*) &keyDigestsInEmmc[i][0]);
		//printf("Hash-Calc-Emmc\n");
	}
}

//This function finds all TLs in emmc, corelates them to their numbers
//and saves their name to threatLibraryFilenamesInEmmc array
UWord8 CfCard::readThreatLibraryNamesFromEmmc()
{
	Word8 *filename = 0;
	Word32 numberOfThreatLibrariesInEmmc = 0;
	UWord8 i = 0, j = 0;
	//gets number of TLs in emmc
	numberOfThreatLibrariesInEmmc = showLibraryInEmmc();
	//printf("Num: %d \n", numberOfThreatLibrariesInEmmc);
	for (i = 1; i <= numberOfThreatLibrariesInEmmc; i++)
	{
		//corelates names to numbers
		filename = findLibraryInEmmc(i);
		//printf("Filename: %s\n",filename);
		j = corelateThreatLibraryToNumber(filename);
		//printf("Filename corelated with: %d\n",j);
		if (j < 6)
		{
			//copies the name to array for that number
			strcpy(&threatLibraryFilenamesInEmmc[j][0], filename);
			threatLibraryPresentInEmmc[j] = 1;
			//printf("Filename copied: %s\n",&threatLibraryFilenamesInEmmc[j][0]);
		}
	}
	return numberOfThreatLibrariesInEmmc;
}

//This function return number of TL in emmc
Word32 CfCard::showLibraryInEmmc()
{
	DIR *directory;
	Word8 *filenamePtr = 0, *extensionPtr = 0;
	Word32 extensionComparison;
	Word32 numberOfFiles = 1;
	struct dirent *dir;
	directory = opendir("/tmp/emmc/");
	if (directory)
	{
		//printf("\033[2J");
		//printf("------------\n");
		//printf("List of Files in EMMC \n");
		while ((dir = readdir(directory)) != NULL)
		{
			filenamePtr = strtok(dir->d_name, ".");
			extensionPtr = strtok(NULL, ".");
			if (extensionPtr != NULL)
			{
				extensionComparison = strcmp(extensionPtr, "tle");
				if (extensionComparison == 0)
				{
					printf("%d.\t", numberOfFiles);
					printf("%s\n", filenamePtr);
					strcpy(filenameReply.name[numberOfFiles - 1], filenamePtr);
					numberOfFiles++;
				}
			}
			if (numberOfFiles > 7)
			{
				break;
			}

		}
		closedir(directory);
		numberOfFiles--;
	}
	else
	{
		numberOfFiles = 0;
	}
	return numberOfFiles;
}

//This function corelates number to its TL as number is a part
//of TL filename
Word8* CfCard::findLibraryInEmmc(Word32 number)
{
	DIR *directory;
	Word8 *filenamePtr = 0, *extensionPtr = 0, *returnPtr = 0;
	Word32 extensionComparison;
	Word32 i = 1;
	struct dirent *dir;
	directory = opendir("/tmp/emmc/");
	if (directory)
	{
		while ((dir = readdir(directory)) != NULL)
		{
			filenamePtr = strtok(dir->d_name, ".");
			extensionPtr = strtok(NULL, ".");
			if (extensionPtr != NULL)
			{
				extensionComparison = strcmp(extensionPtr, "tle");
				if (extensionComparison == 0)
				{
					if (i == number)
					{
						returnPtr = filenamePtr;
					}
					i++;
				}
			}

		}
		closedir(directory);
	}
	return (Word8*) returnPtr;
}

//A simple function to convert 1 to 6 char to int; itoa can be used in C++
UWord8 CfCard::corelateThreatLibraryToNumber(Word8 *filename)
{
	if (filename[0] == '1')
	{
		return 0;
	}
	else if (filename[0] == '2')
	{
		return 1;
	}
	else if (filename[0] == '3')
	{
		return 2;
	}
	else if (filename[0] == '4')
	{
		return 3;
	}
	else if (filename[0] == '5')
	{
		return 4;
	}
	else if (filename[0] == '6')
	{
		return 5;
	}
	else
	{
		return 6;
	}
}

//This function return number of TL in CF Card
Word32 CfCard::showLibraryInCfCard()
{
	DIR *directory;
	Word8 *filenamePtr = 0, *extensionPtr = 0;
	Word32 extensionComparison;
	Word32 numberOfFiles = 1;
	struct dirent *dir;
	char systemCommand[100];
	sprintf(systemCommand, "/tmp/%s", cfCardName);
	directory = opendir(systemCommand);
	if (directory)
	{
		//printf("\033[2J");
		printf("------------\n");
		printf("List of Files in CF card \n");
		while ((dir = readdir(directory)) != NULL)
		{
			filenamePtr = strtok(dir->d_name, ".");
			extensionPtr = strtok(NULL, ".");
			if (extensionPtr != NULL)
			{
				extensionComparison = strcmp(extensionPtr, "tle");
				if (extensionComparison == 0)
				{
					printf("%d.\t", numberOfFiles);
					printf("%s\n", filenamePtr);
					strcpy(filenameReply.name[numberOfFiles - 1], filenamePtr);
					numberOfFiles++;
				}
			}
			if (numberOfFiles > 7)
			{
				break;
			}

		}
		closedir(directory);
		numberOfFiles--;
	}
	else
	{
		numberOfFiles = 0;
	}
	return numberOfFiles;
}

//This function corelates number to its TL as number is a part
//of TL filename
Word8* CfCard::findLibraryInCfCard(Word32 number)
{
	DIR *directory;
	Word8 *filenamePtr, *extensionPtr, *returnPtr = 0;
	Word32 extensionComparison;
	Word32 i = 1;
	struct dirent *dir;
	char systemCommand[100];
	sprintf(systemCommand, "/tmp/%s", cfCardName);
	directory = opendir(systemCommand);
	if (directory)
	{
		while ((dir = readdir(directory)) != NULL)
		{
			filenamePtr = strtok(dir->d_name, ".");
			extensionPtr = strtok(NULL, ".");
			if (extensionPtr != NULL)
			{
				extensionComparison = strcmp(extensionPtr, "tle");
				if (extensionComparison == 0)
				{
					if (i == number)
					{
						returnPtr = filenamePtr;
					}
					i++;
				}
			}

		}
		closedir(directory);
	}
	return (Word8*) returnPtr;
}

//This function finds any CFD Library in CF Card
Word8* CfCard::findCfdLibraryInCfCard()
{
	DIR *directory;
	Word8 *filenamePtr = 0, *extensionPtr = 0, *returnPtr = 0;
	Word32 extensionComparison;
	struct dirent *dir;
	directory = opendir("/tmp/emmc/");
	if (directory)
	{
		while ((dir = readdir(directory)) != NULL)
		{
			filenamePtr = strtok(dir->d_name, ".");
			extensionPtr = strtok(NULL, ".");
			if (extensionPtr != NULL)
			{
				extensionComparison = strcmp(extensionPtr, "cfd");
				if (extensionComparison == 0)
				{
					returnPtr = filenamePtr;
				}
			}
		}
		closedir(directory);
	}
	return (Word8*) returnPtr;
}

//This function finds all TLs in CF Card, corelates them to their numbers
//and saves their name to threatLibraryFilenamesInCfCard array
void CfCard::readThreatLibraryNamesFromCfCard()
{
	Word8 *filename = 0;
	Word32 numberOfThreatLibrariesInCfCard = 0;
	UWord8 i = 0, j = 0;
	//memset(&threatLibraryFilenamesInCfCard[j][0],0,sizeof(threatLibraryFilenamesInCfCard));
	numberOfThreatLibrariesInCfCard = showLibraryInCfCard();
	//printf("Num: %d \n", numberOfThreatLibrariesInCfCard);
	for (i = 1; i <= numberOfThreatLibrariesInCfCard; i++)
	{
		filename = findLibraryInCfCard(i);
		//printf("Filename: %s\n",filename);
		j = corelateThreatLibraryToNumber(filename);
		//printf("Filename corelated with: %d\n",j);
		if (j < 6)
		{
			strcpy(&threatLibraryFilenamesInCfCard[j][0], filename);
			threatLibraryPresentInCfCard[j] = 1;
			//printf("Filename copied: %s\n",&threatLibraryFilenamesInCfCard[i][0]);
		}
	}
}

//This function finds any CFD library in CF Card, sets cfdLibraryPresent flag high
//and saves its name to cfdLibraryName variable
void CfCard::readCfdLibraryName()
{
	Word8 *filename = 0;
	filename = findCfdLibraryInCfCard();

	if (filename != NULL)
	{
		cfdLibraryPresent = 1;
		strcpy(&cfdLibraryName[0], filename);
	}
	else
	{
		cfdLibraryPresent = 0;
	}
}

//This function calculates Hash of all the keys present in CF Card
void CfCard::calculateHashOfKeysInCF()
{
	Word8 filenameWithPath[100] =
	{ ' ' };
	FILE *keyFile;
	Word8 *password;
	uint8_t i = 0, j = 0;
	UWord8 decryptionKeyName[32];
	SHA256_CTX ctx;
	L32 lSize;
	for (i = 0; i < numberOfKeysInCfCard; i++)
	{
		sprintf(filenameWithPath, "/tmp/%s/%s.key", cfCardName,
				&keyFilenamesInCfCard[i][0]);
		keyFile = fopen(filenameWithPath, "rb");
		if (!keyFile)
		{
			continue;
		}
		fseek(keyFile, 0L, SEEK_END);
		lSize = ftell(keyFile);
		if (lSize > 33)
		{
			continue;
		}
		rewind(keyFile);
		memset(bufferForKey, 0, sizeof(0));

		if (1 != fread(bufferForKey, lSize, 1, keyFile))
		{
			fclose(keyFile);
			continue;
		}
		fclose(keyFile);
		password = bufferForKey;
		for (j = 0; j < sizeof(decryptionKeyName); j++)
		{
			decryptionKeyName[j] = *password != 0 ? *password++ : 0;
		}
		sha256_init(&ctx);
		sha256_update(&ctx, decryptionKeyName, sizeof(decryptionKeyName));
		sha256_final(&ctx, (UWord8*) &keyDigestsInCfCard[i][0]);
		//printf("Hash-Calc\n");
	}
}

//This function corelates TL to keys present in CF Card
//Sequence is as follows, TL is opened, delimeter is checked,
//Key Hash present in TL header is extracted and is matched
//with all hashes of keys present in CF Card, if it matches with
//a single one, TL is corelated with that key
void CfCard::corelateThreatLibraryToKeysInCfCard()
{
	FILE *input;
	L32 lSize;
	ThreatLibraryUnencryptedHeader *tlHeader;
	Word8 filenameWithPath[100] =
	{ ' ' };
	UWord8 i = 0, j = 0, k = 0, l = 0;
	memset(threatLibraryHashMatchesInCfCard, 0,
			sizeof(threatLibraryHashMatchesInCfCard));
	memset(keyHashMatchesInCfCard, 0, sizeof(keyHashMatchesInCfCard));
	memset(keyCorelatedToTlInCfCard, 0, sizeof(keyCorelatedToTlInCfCard));
	for (i = 0; i < 6; i++)
	{
		if (threatLibraryPresentInCfCard[i] == 1)
		{
			sprintf(filenameWithPath, "/tmp/%s/%s.tle", cfCardName,
					&threatLibraryFilenamesInCfCard[i][0]);
			input = fopen(filenameWithPath, "rb");
			if (input == NULL)
			{
				printf("TL-Unavailable\n");
				threatLibraryPresentInCfCard[i] = 0;
			}
			else
			{
				fseek(input, 0, SEEK_END);
				lSize = ftell(input);
				//if file size is greater than normal TL, we won't read it and use
				if (lSize > (L32) SIZE_OF_THREAT_LIBRARY)
				{
					fclose(input);
					continue;
				}
				rewind(input);
				memset(bufferForUse1, 0, sizeof(bufferForUse1));
				fread(bufferForUse1, lSize, 1, input);
				fclose(input);
				tlHeader = (ThreatLibraryUnencryptedHeader*) bufferForUse1;
				if (tlHeader->delimeter != 0xdeadbeefbeeffeed)
				{
					printf("TL delimeter Fail\n");
					threatLibraryPresentInCfCard[i] = 0;
				}
				else
				{
					for (j = 0; j < numberOfKeysInCfCard; j++)
					{
						l = 0;
						for (k = 0; k < 32; k++)
						{
							if (keyDigestsInCfCard[j][k] != tlHeader->hash[k])
							{
								l++;
							}
						}
						if (l == 0)
						{
							keyHashMatchesInCfCard[j] = 1;
							keyCorelatedToTlInCfCard[i] = j;
							threatLibraryHashMatchesInCfCard[i] = 1;
							printf("Key Hash Matched for %d \n", i);
							break;
						}
					}
				}
			}
		}
	}
}

//This function corelates TL to keys present in emmc
//Sequence is as follows, TL is opened, delimeter is checked,
//Key Hash present in TL header is extracted and is matched
//with all hashes of keys present in emmc, if it matches with
//a single one, TL is corelated with that key
void CfCard::corelateThreatLibraryToKeysInEmmc()
{
	UWord8 i = 0, j = 0, k = 0, l = 0;
	FILE *input;
	L32 lSize;
	Word8 filenameWithPath[100] =
	{ ' ' };
//	UWord32 crc = 0;
	ThreatLibraryUnencryptedHeader *tlHeader;

	readThreatLibraryNamesFromEmmc();
	extractKeyNamesFromEmmc();
	calculateHashOfKeysInEmmc();

	for (i = 0; i < 6; i++)
	{
		if (threatLibraryPresentInEmmc[i] == 1)
		{
			sprintf(filenameWithPath, "/tmp/emmc/%s.tle",
					&threatLibraryFilenamesInEmmc[i][0]);
			input = fopen(filenameWithPath, "rb");
			if (input == NULL)
			{
				printf("TL-Unavailable\n");
				threatLibraryPresentInEmmc[i] = 0;
			}
			else
			{
				fseek(input, 0, SEEK_END);
				lSize = ftell(input);
				//if file size is greater than normal TL, we won't read it and use
				if (lSize > (L32) SIZE_OF_THREAT_LIBRARY)
				{
					fclose(input);
					continue;
				}
				rewind(input);
				memset(bufferForUse1, 0, sizeof(bufferForUse1));
				fread(bufferForUse1, lSize, 1, input);
				fclose(input);
				tlHeader = (ThreatLibraryUnencryptedHeader*) bufferForUse1;
				if (tlHeader->delimeter != 0xdeadbeefbeeffeed)
				{
					printf("TL delimeter Fail\n");
					threatLibraryPresentInEmmc[i] = 0;
				}
				else
				{
					for (j = 0; j < numberOfKeysInEmmc; j++)
					{
						l = 0;
						for (k = 0; k < 32; k++)
						{
							if (keyDigestsInEmmc[j][k] != tlHeader->hash[k])
							{
								l++;
							}
						}
						if (l == 0)
						{
//							crc = 0;
							keysCorelatedToTlInEmmc[i] = j;
						}
					}
				}
			}
		}
	}
}

//This function first checks all the TLs and Keys in CF Card
//and emmc, corelates them to numbers, the TL number which exists
//in CF Card and emmc gets deleted from emmc, all the TLs and
//their Keys are copied to emmc from CF Card
void CfCard::copyAndDecryptLogic()
{
	UWord8 i = 0, k = 0;
	UWord32 returnError = 0;
	Word8 tlFilenameInCfCard[50];
	Word8 tlFilenameInEmmc[50];
	Word8 keyFilenameInCfCard[50];
	Word8 keyFilenameInEmmc[50];
	numberOfKeysCopiedToEmmc = 0;
	//memset(duplicateTl, 0, sizeof(duplicateTl));
	readThreatLibraryNamesFromCfCard();
	extractKeyNamesFromCfCard();
	calculateHashOfKeysInCF();
	corelateThreatLibraryToKeysInCfCard();
	corelateThreatLibraryToKeysInEmmc();
	for (i = 0; i < 6; i++)
	{
		if (i == 0)
		{
			copyCfdLibraryToEmmc();
		}

		if (threatLibraryHashMatchesInCfCard[i] == 1)
		{
			remove(&threatLibraryFilenamesInEmmc[i][0]);
			remove(&keyFilenamesInEmmc[keysCorelatedToTlInEmmc[i]][0]);
			char tempCommand[100];
			sprintf(tempCommand, "rm /tmp/emmc/%s.tle",
					&threatLibraryFilenamesInEmmc[i][0]);
			system(tempCommand);
			sprintf(tempCommand, "rm /tmp/emmc/%s.key",
					&keyFilenamesInEmmc[keysCorelatedToTlInEmmc[i]][0]);
			system(tempCommand);

			sprintf(tlFilenameInCfCard, "/tmp/%s/%s.tle", cfCardName,
					&threatLibraryFilenamesInCfCard[i][0]);
			sprintf(tlFilenameInEmmc, "/tmp/emmc/%s.tle",
					&threatLibraryFilenamesInCfCard[i][0]);
			returnError = copyFiles(tlFilenameInCfCard, tlFilenameInEmmc);
			if (returnError < 0)
			{
				printf("TL %d copy error\n", i);
			}
			else
			{
				printf("TL %d copied ", i);
			}
			strcpy(&threatLibraryFilenamesCopiedToEmmc[i][0],
					&threatLibraryFilenamesInCfCard[i][0]);
			strcpy(&threatLibraryFilenamesInEmmc[i][0],
					&threatLibraryFilenamesInCfCard[i][0]);
			printf("%s \n", &threatLibraryFilenamesCopiedToEmmc[i][0]);
			threatLibraryCopiedToEmmc[i] = 1;
			sprintf(keyFilenameInCfCard, "/tmp/%s/%s.key", cfCardName,
					&keyFilenamesInCfCard[keyCorelatedToTlInCfCard[i]][0]);
			sprintf(keyFilenameInEmmc, "/tmp/emmc/%s.key",
					&keyFilenamesInCfCard[keyCorelatedToTlInCfCard[i]][0]);
			returnError = copyFiles(keyFilenameInCfCard, keyFilenameInEmmc);
			if (returnError < 0)
			{
				printf("Key %d copy error\n", i);
			}
			else
			{
				printf("Key %d copied ", i);
			}
			strcpy(&keyFilenamesCopiedToEmmc[k][0],
					&keyFilenamesInCfCard[i][0]);
			strcpy(&keyFilenamesInEmmc[k][0], &keyFilenamesInCfCard[i][0]);
			printf("%s \n", &keyFilenamesCopiedToEmmc[i][0]);
			break;
		}
	}
	//decryptionLogic(1);
}

void CfCard::copyCfdLibraryToEmmc()
{
	char systemCommand[100];
	sprintf(systemCommand, "rm /tmp/%s/*.cfd", cfCardName);
	system(systemCommand);
}

//This function reads the file named in filename and dumps it to buffer
int CfCard::readFile(const char *fileName, char *buffer, int maxSize)
{
	/* declare a file pointer */
	FILE *infile;
	int numbytes = 0;

	/* open an existing file for reading */
	infile = fopen(fileName, "r");

	/* quit if the file does not exist */
	if (infile == NULL)
	{
		return -1;
	}
	/* Get the number of bytes */
	fseek(infile, 0L, SEEK_END);
	numbytes = ftell(infile);
	if (numbytes > maxSize)
	{
		return -1;
	}
	/* reset the file position indicator to
	 the beginning of the file */
	fseek(infile, 0L, SEEK_SET);

	/* grab sufficient memory for the
	 buffer to hold the text */

	/* copy all the text into the buffer */
	fread(buffer, sizeof(char), numbytes, infile);
	fclose(infile);
	return numbytes;
}

//This function opens filename and dumps size length from buffer to file
int CfCard::writeFile(const char *filename, char *buffer, int size)
{
	/* declare a file pointer */
	FILE *infile;
	int numbytes = 0;

	/* open an existing file for reading */
	infile = fopen(filename, "wb");
	fseek(infile, 0L, SEEK_SET);
	numbytes = fwrite(buffer, 1, size, infile);
	if (size != numbytes)
	{
		return -1;
	}
	fclose(infile);
	return 1;
}

//This function copies readFilename to writeFilename by reading the file
//to a buffer and dumping the buffer to the writeFilename
int CfCard::copyFiles(char *readFilename, char *writeFilename)
{
	int returnFromFunction = 0;
	memset(bufferForCopy, 0, sizeof(bufferForCopy));
	returnFromFunction = readFile(readFilename, bufferForCopy,
			SIZE_OF_THREAT_LIBRARY);
	if (returnFromFunction < 0)
	{
		return -1;
	}
	returnFromFunction = writeFile(writeFilename, bufferForCopy,
			returnFromFunction);
	if (returnFromFunction < 0)
	{
		return -1;
	}
	return 1;
}

