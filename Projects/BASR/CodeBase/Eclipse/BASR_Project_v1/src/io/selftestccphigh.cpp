/*
 * selftestccphigh.cpp
 *
 *  Created on: Feb 9, 2021
 *      Author: saad
 */

#define SELFTESTCCPHIGH_CPP_

#include "selftestccphigh.h"

//This function fills and returns packet with result of checking if
//CF Card is connected or not
Word32 SelfTestCcpHigh::checkCfCard(CfCard &cfCard)
{

	UWord32 status;
	status = 1 << 30;
	status |= 3 << 25;
	status |= 1 << 17; //ID
	status |= !cfCard.checkCfCardExistance();

	return status;
}

//This function fills and returns packet with result of checking if
//emmc exists on board
Word32 SelfTestCcpHigh::checkEmmc()
{ //ID2
	UWord8 result;
	UWord32 status;
	FILE *ptr;
	ptr = fopen("/dev/mmcblk0p1", "rb");
	if (ptr == NULL)
	{
		//printf("No Emmc\n");
		result = 1;
	}
	else
	{
		//printf("Emmc exists\n");
		fclose(ptr);
		result = 0;
	}
	status = 1 << 30;
	status |= 3 << 25;
	status |= 2 << 17; //ID
	status |= result;

	return status;
}

//This function opens up emmc directory through statvfs
//and finds if there is some empty space in emmc
Word32 SelfTestCcpHigh::findFreeSpaceEmmc()
{ //ID4
	UWord8 result;
	UWord32 status;
	int a = 0;
	unsigned long total, free;
	struct statvfs fidata;
	a = statvfs("/tmp/emmc", &fidata);
	//printf("EMMC\n");
	if (a >= 0)
	{
		total = (fidata.f_bsize * fidata.f_blocks) / (1024);
		free = (fidata.f_bsize * fidata.f_bfree) / (1024);
		//printf("Total Size: %lu \n",total);

		//printf("Free Size: %lu \n",free);
		if (total - free > 1)
		{
			//printf("Space present\n");
			result = 0;
		}
		else
		{
			result = 1;
		}
	}
	else
	{
		//printf("mount Failed");
		result = 1;
	}
	status = 1 << 30;
	status |= 3 << 25;
	status |= 4 << 17; //ID
	status |= result;

	return status;
}

//This function opens up cf card directory through statvfs
//and finds if there is some empty space in cf card
Word32 SelfTestCcpHigh::findFreeSpaceCfCard(CfCard & cfCard)
{ //ID3
	UWord8 result;
	UWord32 status;
	Word32 a = 0;
	UL32 total, free;
	struct statvfs fidata;

	char systemCommand[100];
	sprintf(systemCommand, "/tmp/%s",cfCard.cfCardName);

	a = statvfs(systemCommand, &fidata);
	//printf("CF Card\n");
	if (a >= 0)
	{
		total = (fidata.f_bsize * fidata.f_blocks) / (1024);
		free = (fidata.f_bsize * fidata.f_bfree) / (1024);
		cfCardPercentage = (free * 100) / total;
		//printf("Total Size: %lu \n",total);

		if (total - free > 1)
		{
			//printf("Space present\n");
			result = 0;
		}
		else
		{
			result = 1;
		}
	}
	else
	{
		//printf("mount Failed");
		result = 1;
	}
	status = 1 << 30;
	status |= 3 << 25;
	status |= 3 << 17; //ID
	status |= result;

	return status;
}

//This function tests read and write from CF Card
//It opens a file, writes to it, reads it back and
//checks sanctity of the data
Word32 SelfTestCcpHigh::readWriteTestCfCard(CfCard & cfCard)
{ //ID5
	UWord8 result;
	UWord32 status;
	UWord8 a = 144, b = 0;
	FILE *input, *output;
	char systemCommand[100];
	sprintf(systemCommand, "/tmp/%s/testing.txt",cfCard.cfCardName);
	input = fopen(systemCommand, "r+b");
	if (input == NULL)
	{
		result = 1;
	}
	else
	{
		fseek(input, 0, SEEK_SET);
		fwrite(&a, 1, 1, input);
		fclose(input);
		output = fopen(systemCommand, "r+b");
		if(output!=NULL)
		{
			fread((char*) &b, 1, 1, output);
			fclose(output);
		}
		if (a == b)
		{
			//printf("Matches\n");
			result = 0;
		}
		else
		{
			result = 1;
		}
	}
	status = 1 << 30;
	status |= 3 << 25;
	status |= 5 << 17; //ID
	status |= result;

	return status;
}

//This function tests read and write from emmc
//It opens a file, writes to it, reads it back and
//checks sanctity of the data
Word32 SelfTestCcpHigh::readWriteTestEmmc()
{ //ID6
	UWord8 result;
	UWord32 status;
	UWord8 a = 144, b = 0;
	FILE *input, *output;
	input = fopen("/tmp/emmc/testing.txt", "r+b");
	if (input == NULL)
	{
		result = 1;
	}
	else
	{
		fseek(input, 0, SEEK_SET);
		fwrite(&a, 1, 1, input);
		fclose(input);
		output = fopen("/tmp/emmc/testing.txt", "r+b");
		fread((char*) &b, 1, 1, output);
		fclose(output);
		if (a == b)
		{
			//printf("Matches\n");
			result = 0;
		}
		else
		{
			result = 1;
		}
	}
	status = 1 << 30;
	status |= 3 << 25;
	status |= 6 << 17; //ID
	status |= result;

	return status;
}

//This function is used to read filename of Selected TL that was selected in
//previous iteration
Word32 SelfTestCcpHigh::readFilename(Word8 filename[])
{
	FILE *selected;
	Word8 buffer[255];
	selected = fopen("/tmp/emmc/selectedlib.txt", "r");
	if (selected != 0)
	{
		fgets(buffer, 16, (FILE*) selected);
		//printf("%s\n",buffer);
		memcpy(filename, buffer, 16 * sizeof(Word8));
		fclose(selected);
		return 1;
	}
	return 0;
}

//This function iterates over /tmp/emmc directory, finds all TLs,
//copies their names and returns number of TL present in emmc
Word32 SelfTestCcpHigh::showLibraryInEmmc()
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

//This function returns the TL name with respect to the number in emmc
Word8* SelfTestCcpHigh::findLibraryInEmmc(Word32 number)
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

//This function is main logic of checking either selected TL exists in emmc
//It extacts selected TL name from file and finds that TL in emmc
UWord8 SelfTestCcpHigh::checkThreatLibraryExists()
{
	Word32 check = 0, i = 0;
	Word8 selectedTlFilename[16], result = 0;
	Word8 *tlFilename = 0;
	Word32 count = 0, charCheck;
	check = readFilename(selectedTlFilename);
	if (check == 0)
	{
		result = 1;
	}
	else
	{
		count = showLibraryInEmmc();
		for (i = 1; i <= count; i++)
		{
			tlFilename = findLibraryInEmmc(i);
			charCheck = strcmp(tlFilename, selectedTlFilename);
			if (charCheck == 0)
			{
				result = 0;
				break;
				//printf("matches\n");
			}
			else
			{
				result = 1;
			}
		}
	}
	return result;
}

//This function checks if a TL was selected previously and its record exists
UWord8 SelfTestCcpHigh::checkThreatLibrarySelectionExists()
{
	Word32 check = 0;
	Word8 selectedTlFilename[16];
	check = readFilename(selectedTlFilename);
	if (check == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//This is the function that collects all the results regarding TL and Key in EMMC
void SelfTestCcpHigh::checkThreatLibraryFaults(UWord32 *input, bool tlFailure)
{
	UWord32 status;
	UWord8 result;
	if (tlFailure == 0)
	{
		//existance ID 7
		status = 1 << 30;
		status |= 3 << 25;
		status |= 7 << 17; //ID
		status |= 0;
		input[0] = status;
		//CRC 8
		status = 1 << 30;
		status |= 3 << 25;
		status |= 8 << 17; //ID
		status |= 0;
		input[1] = status;
		//Key 9
		status = 1 << 30;
		status |= 3 << 25;
		status |= 9 << 17; //ID
		status |= 0;
		input[2] = status;
		//selection 10
		status = 1 << 30;
		status |= 3 << 25;
		status |= 10 << 17; //ID
		status |= 0;
		input[3] = status;
	}
	else
	{
		//selection 10
		result = checkThreatLibrarySelectionExists();
		if (result == 1)
		{
			//selection 10
			status = 1 << 30;
			status |= 3 << 25;
			status |= 10 << 17; //ID
			status |= 1;
			input[3] = status;
			//existance ID 7
			status = 1 << 30;
			status |= 3 << 25;
			status |= 7 << 17; //ID
			status |= 1;
			input[0] = status;
			//CRC 8
			status = 1 << 30;
			status |= 3 << 25;
			status |= 8 << 17; //ID
			status |= 0;
			input[1] = status;
		}
		else
		{
			//selection 10
			status = 1 << 30;
			status |= 3 << 25;
			status |= 10 << 17; //ID
			status |= 0;
			input[3] = status;
			//existance
			result = checkThreatLibraryExists();
			status = 1 << 30;
			status |= 3 << 25;
			status |= 7 << 17; //ID
			status |= result;
			input[0] = status;
			if (result == 0)
			{
				if (tlFailure == 1)
				{
					result = 1;
				}
				else
				{
					result = 0;
				}
				//CRC 8
				status = 1 << 30;
				status |= 3 << 25;
				status |= 8 << 17; //ID
				status |= result;
				input[1] = status;
			}
			else
			{
				//CRC 8
				status = 1 << 30;
				status |= 3 << 25;
				status |= 8 << 17; //ID
				status |= 0;
				input[1] = status;
			}
		}
		//Key 9
		result = checkKeyExists();
		status = 1 << 30;
		status |= 3 << 25;
		status |= 9 << 17; //ID
		status |= result;
		input[2] = status;
	}
}

//This function returns number of keys present in EMMC i.e. /tmp/emmc directory
Word32 SelfTestCcpHigh::returnNumberOfKeysInEmmc()
{
    struct dirent *dir;
    Word32 numberOfKeys = 0;
    DIR *directory;
    Word8 *filenamePtr = 0,*extensionPtr = 0;
    Word32 extensionComparison;
    directory = opendir("/tmp/emmc/");
    if(directory)
    {
        //printf("\033[2J");
        //printf("------------\n");
        while((dir = readdir(directory)) != NULL)
        {
            filenamePtr = strtok(dir->d_name,".");
            extensionPtr = strtok(NULL,".");
            if(extensionPtr != NULL)
            {
                extensionComparison = strcmp(extensionPtr,"key");
                if(extensionComparison == 0)
                {
                    numberOfKeys++;
                }
            }

        }
        closedir(directory);
    }
    return numberOfKeys;
}

//This function returns if any Key exists in emmc
Word32 SelfTestCcpHigh::checkKeyExists()
{
	if (returnNumberOfKeysInEmmc() <= 0)
	{
		//printf("No Key Found\n");
		return 1;
	}
	else
	{
		printf("Key Exists\n");
		return 0;
	}
}

//This is the main function of Modular Self Test done on CCP
//It checks CF Card, EMMC, free space in both, read write of both,
//TL and Key presence in EMMC and eventually temperature of SoC
void SelfTestCcpHigh::testAllModulesCCP(UWord32 *input, bool tlFailure,
		XadcTemperatureHigh &xadcTemperatureHigh, CfCard &cfCard)
{
	UWord8 ccpTemperatureWarning = 0;
	input[0] = checkCfCard(cfCard);
	input[1] = checkEmmc();
	input[2] = findFreeSpaceCfCard(cfCard);
	input[3] = findFreeSpaceEmmc();
	input[4] = readWriteTestCfCard(cfCard);
	input[5] = readWriteTestEmmc();
	checkThreatLibraryFaults(&input[6], tlFailure);

	ccpTemperatureWarning = xadcTemperatureHigh.checkXadcTemperature();
	input[10] = 1 << 30;
	input[10] |= 3 << 25;
	input[10] |= 11 << 17; //ID
	input[10] |= ccpTemperatureWarning;
}
