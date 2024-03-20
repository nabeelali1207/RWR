/*
 * selftestmflhandler.h
 *
 *  Created on: Jan 28, 2021
 *      Author: saad
 */

#ifndef SELFTESTMFLHANDLER_H_
#define SELFTESTMFLHANDLER_H_

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utility/consthigh.h"
#include "../common/common.h"

class SelfTestMflHandlerHigh
{
private:
	Word8 command[80];
public:
	void createMflFiles(Word8* filename);
	void readMflFile(Word8 *filename, UWord8 size, UWord8 *mflData);
	void writeMflFile(Word8 *filename, UWord8 size, UWord8 *mflData);
	void writeCurrentMflFile(Word8 *filename, UWord8 id, UWord8 amount,
			UWord8 exists);
	void fillPflStructure(UWord8 index);
};

#endif /* SELFTESTMFLHANDLER_H_ */
