/*
 * cfcard.h
 *
 *  Created on: Feb 8, 2021
 *      Author: saad
 */

#ifndef CFCARD_H_
#define CFCARD_H_

#include "../common/common.h"
#include "../utility/consthigh.h"
#include "../utility/utility.h"
#include "../vo/libraryclass.h"
#include <time.h>
#include "../utility/sha256.h"
#include "../vo/trenzspidatastructures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

class CfCard {
private:
	static const unsigned int SIZE_OF_THREAT_LIBRARY = sizeof(ThreatLibrary)
			+ 1000;
	uint64_t endTimeCfCheck;
	uint64_t endTimeCfControllerReset;
	SpiFilenameReply filenameReply;
	uint64_t timeDifferenceCfCheck;
	uint64_t timeDifferenceCfControllerReset;
	UWord8 resetCfFlag;
	Word8 threatLibraryFilenamesInCfCard[6][100];
	Word8 keyFilenamesInCfCard[6][100];
	UWord8 keyDigestsInCfCard[6][SHA256_BLOCK_SIZE];
	UWord8 keyDigestsInEmmc[6][32];
	UWord8 keys[6][32];
	Word8 bufferForKey[33];
	UWord8 threatLibraryHashMatchesInCfCard[6];
	UWord8 keyHashMatchesInCfCard[6];
	UWord8 keyCorelatedToTlInCfCard[6];
	UWord8 threatLibraryPresentInCfCard[6];
	UWord8 cfdLibraryPresent;
	Word32 numberOfKeysInEmmc;
	UWord32 numberOfKeysCopiedToEmmc;
	UWord8 bufferForUse1[SIZE_OF_THREAT_LIBRARY];
	UWord8 keysCorelatedToTlInEmmc[6];
	Word8 threatLibraryFilenamesCopiedToEmmc[6][100];
	Word8 keyFilenamesCopiedToEmmc[6][100];
	UWord8 threatLibraryCopiedToEmmc[6];
	Word8 bufferForCopy[SIZE_OF_THREAT_LIBRARY];
	UWord8 cfCardMounted;
public:
	UWord8 cfCardName[5];
	UWord8 cfCardNumber[4];

	uint64_t startTimeCfControllerReset;
	uint64_t startTimeCfCheck;
	Word8 keyFilenamesInEmmc[6][100];
	Word8 threatLibraryFilenamesInEmmc[6][100];
	Word8 cfdLibraryName[100];
	UWord8 threatLibraryPresentInEmmc[6];
	UWord8 foundCfFlag;
	UWord8 numberOfKeysInCfCard;

	/*
	 * Function Name:   resetCfCardController
	 * Purpose:			This function is used to restart CF Card controller
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	UWord8 resetCfCardController();

	/*
	 * Function Name:   checkCfCardExistance
	 * Purpose:			This function checks if CF Card is connected with our system
	 * 					It goes as follows, checks for disk devices with FAT16 setting,
	 * 					as CF Card is the only device with FAT16 format, all others being
	 * 					ext4 format, we pluck its name out to mount it on a folder later on.
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	UWord8 checkCfCardExistance();

	/*
	 * Function Name:   mainProcessingCfCardCheckLogic
	 * Purpose:			This function is main CF Card logic, i.e. it makes CF Card
	 * 					hot-swappabale and enables system to access CF Card even if
	 * 					is connected after startup
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void mainProcessingCfCardCheckLogic();

	/*
	 * Function Name:   startUpCfCardCheckLogic
	 * Purpose:			This function checks if CF card is connected with system. If so,
	 * 					it mounts the CF Card and reads Keys from the CF Card
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void startUpCfCardCheckLogic();

	/*
	 * Function Name:   mountCfCard
	 * Purpose:			This function makes a directory as /tmp/cf and mounts CF Card
	 * 					to /tmp/cf. Later it makes a testing.txt file used to check
	 * 					CF Card during Self Tests
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void mountCfCard();

	/*
	 * Function Name:   unmountCfCard
	 * Purpose:			This function unmounts CF Card and deletes the directory
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void unmountCfCard();

	/*
	 * Function Name:   zeroizeKeys
	 * Purpose:			This function overwrites all Keys with 0's in CF Card and emmc
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void zeroizeKeys();

	/*
	 * Function Name:   returnNumberOfKeysInEmmc
	 * Purpose:			This function traverses through /tmp/emmc/ and returns number of Keys present
	 * Inputs:          none
	 * Outputs:         number of keys present
	 * Scope:           Can be called by any function
	 */
	Word32 returnNumberOfKeysInEmmc();

	/*
	 * Function Name:   showKeyInCfCard
	 * Purpose:			This function traverses through /tmp/cf/ and returns number of
	 * 					Keys present and their names are copied to filename array
	 * Inputs:          none
	 * Outputs:         number of keys present
	 * Scope:           Can be called by any function
	 */
	Word32 showKeyInCfCard();

	/*
	 * Function Name:   extractKeyNamesFromCfCard
	 * Purpose:			This function corelates Keynames to their numbers i.e.
	 * 					1 to 6 are key numbers and names are associated w.r.t.
	 * 					to their nomenclature
	 * Inputs:          none
	 * Outputs:         number of keys present
	 * Scope:           Can be called by any function
	 */
	void extractKeyNamesFromCfCard();

	/*
	 * Function Name:   findKeyInCfCard
	 * Purpose:			This function corelates number to its keyname as number is a part
	 * 					of Keyname
	 * Inputs:          Key number
	 * Outputs:         Name of Key file
	 * Scope:           Can be called by any function
	 */
	Word8* findKeyInCfCard(Word32 number);

	/*
	 * Function Name:   extractKeyNamesFromEmmc
	 * Purpose:			This function corelates Keynames to their numbers i.e.
	 * 					1 to 6 are key numbers and names are associated w.r.t.
	 * 					to their nomenclature
	 * Inputs:          none
	 * Outputs:         number of keys present
	 * Scope:           Can be called by any function
	 */
	void extractKeyNamesFromEmmc();

	/*
	 * Function Name:   findKeyInEmmc
	 * Purpose:			This function corelates number to its keyname as number is a part
	 * 					of Keyname
	 * Inputs:          Key number
	 * Outputs:         Name of Key file
	 * Scope:           Can be called by any function
	 */
	Word8* findKeyInEmmc(Word32 number);

	/*
	 * Function Name:   calculateHashOfKeysInEmmc
	 * Purpose:			This fuction calculates Hash of all the keys present in emmc
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void calculateHashOfKeysInEmmc();

	/*
	 * Function Name:   readThreatLibraryNamesFromEmmc
	 * Purpose:			This function finds all TLs in emmc, corelates them to their numbers
	 * 					and saves their name to threatLibraryFilenamesInEmmc array
	 * Inputs:          none
	 * Outputs:         Numbers of TL in emmc
	 * Scope:           Can be called by any function
	 */
	UWord8 readThreatLibraryNamesFromEmmc();

	/*
	 * Function Name:   showLibraryInEmmc
	 * Purpose:			This function return number of TL in emmc
	 * Inputs:          none
	 * Outputs:         Numbers of TL in emmc
	 * Scope:           Can be called by any function
	 */
	Word32 showLibraryInEmmc();

	/*
	 * Function Name:   findLibraryInEmmc
	 * Purpose:			This function corelates number to its TL as number is a part
	 * 					of TL filename
	 * Inputs:          TL number
	 * Outputs:         Name of TL file
	 * Scope:           Can be called by any function
	 */
	Word8* findLibraryInEmmc(Word32 number);

	/*
	 * Function Name:   corelateThreatLibraryToNumber
	 * Purpose:			A simple function to convert 1 to 6 char to int; itoa can be used in C++
	 * Inputs:          TL file name
	 * Outputs:         Number it responds to
	 * Scope:           Can be called by any function
	 */
	UWord8 corelateThreatLibraryToNumber(Word8 *filename);

	/*
	 * Function Name:   showLibraryInCfCard
	 * Purpose:			This function return number of TL in CF Card
	 * Inputs:          none
	 * Outputs:         Numbers of TL in CF Card
	 * Scope:           Can be called by any function
	 */
	Word32 showLibraryInCfCard();

	/*
	 * Function Name:   findLibraryInCfCard
	 * Purpose:			This function corelates number to its TL as number is a part
	 * 					of TL filename
	 * Inputs:          TL number
	 * Outputs:         Name of TL file
	 * Scope:           Can be called by any function
	 */
	Word8* findLibraryInCfCard(Word32 number);

	/*
	 * Function Name:   readThreatLibraryNamesFromCfCard
	 * Purpose:			This function finds all TLs in CF Card, corelates them to their numbers
	 * 					and saves their name to threatLibraryFilenamesInCfCard array
	 * Inputs:          none
	 * Outputs:         Numbers of TL in CF Card
	 * Scope:           Can be called by any function
	 */
	void readThreatLibraryNamesFromCfCard();

	/*
	 * Function Name:   calculateHashOfKeysInCF
	 * Purpose:			This function calculates Hash of all the keys present in CF Card
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void calculateHashOfKeysInCF();

	/*
	 * Function Name:   corelateThreatLibraryToKeysInCfCard
	 * Purpose:			This function corelates TL to keys present in CF Card
	 * 					Sequence is as follows, TL is opened, delimeter is checked,
	 * 					Key Hash present in TL header is extracted and is matched
	 * 					with all hashes of keys present in CF Card, if it matches with
	 * 					a single one, TL is corelated with that key
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void corelateThreatLibraryToKeysInCfCard();

	/*
	 * Function Name:   corelateThreatLibraryToKeysInEmmc
	 * Purpose:			This function corelates TL to keys present in emmc
	 * 					Sequence is as follows, TL is opened, delimeter is checked,
	 * 					Key Hash present in TL header is extracted and is matched
	 * 					with all hashes of keys present in emmc, if it matches with
	 * 					a single one, TL is corelated with that key
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void corelateThreatLibraryToKeysInEmmc();

	/*
	 * Function Name:   copyAndDecryptLogic
	 * Purpose:			This function first checks all the TLs and Keys in CF Card
	 * 					and emmc, corelates them to numbers, the TL number and its corresponding
	 * 					Key which exists in CF Card and emmc gets deleted from emmc, all the TLs and
	 * 					their Keys are copied to emmc from CF Card
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void copyAndDecryptLogic();

	/*
	 * Function Name:   readFile
	 * Purpose:			This function reads the file named in filename and dumps it to buffer
	 * Inputs:          Filenames
	 * Outputs:         Status of copying
	 * Scope:           Can be called by any function
	 */
	int readFile(const char *fileName, char *buffer, int maxSize);

	/*
	 * Function Name:   writeFile
	 * Purpose:			This function opens filename and dumps size length from buffer to file
	 * Inputs:          Filenames
	 * Outputs:         Status of copying
	 * Scope:           Can be called by any function
	 */
	int writeFile(const char *filename, char *buffer, int size);

	/*
	 * Function Name:   copyFiles
	 * Purpose:			This function copies readFilename to writeFilename by reading the file
	 * 					to a buffer and dumping the buffer to the writeFilename
	 * Inputs:          Filenames
	 * Outputs:         Status of copying
	 * Scope:           Can be called by any function
	 */
	int copyFiles(char *readFilename, char *writeFilename);

	/*
	 * Function Name:   findCfdLibraryInCfCard
	 * Purpose:			This function finds any CFD Library in CF Card
	 * Inputs:          TL number
	 * Outputs:         Name of TL file
	 * Scope:           Can be called by any function
	 */
	Word8* findCfdLibraryInCfCard();

	/*
	 * Function Name:   readCfdLibraryName
	 * Purpose:			This function finds any CFD library in CF Card, sets cfdLibraryPresent flag high
	 *					and saves its name to cfdLibraryName variable
	 * Inputs:          none
	 * Outputs:         Name of CFD Library in CF Card
	 * Scope:           Can be called by any function
	 */
	void readCfdLibraryName();

	/*
	 * Function Name:   copyCfdLibraryToEmmc
	 * Purpose:			This function first checks if any CFD Library is present in CF Card
	 * 					if yes, it is copied to emmc from CF Card
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void copyCfdLibraryToEmmc();
};

#endif /* CFCARD_H_ */
