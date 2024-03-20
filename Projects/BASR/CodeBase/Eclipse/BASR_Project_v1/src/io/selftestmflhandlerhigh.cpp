/*
 * selftestmflhandler.cpp
 *
 *  Created on: Jan 28, 2021
 *      Author: saad
 */
#define SELFTESTMFLHANDLER_CPP_
#include "selftestmflhandlerhigh.h"


void SelfTestMflHandlerHigh::readMflFile(Word8 *filename, UWord8 size, UWord8 *mflData)
{
	FILE *ptr;

	ptr = fopen(filename, "rb");
	if (ptr == 0)
	{
		return;
	}
	fread(mflData, size, 1, ptr);
	fclose(ptr);
}

void SelfTestMflHandlerHigh::writeMflFile(Word8 *filename, UWord8 size, UWord8 *mflData)
{
	FILE *ptr;

	ptr = fopen(filename, "wb");
	if(ptr!=NULL)
	{
		fwrite(mflData, size, 1, ptr);
		fclose(ptr);
	}
}

void SelfTestMflHandlerHigh::writeCurrentMflFile(Word8 *filename, UWord8 id, UWord8 amount, UWord8 exists)
{
	FILE *ptr;

	if (amount > 0)
	{
		sprintf(filename, "/tmp/emmc/mfl/current.txt");
		ptr = fopen(filename, "r+b");
		if(ptr!=NULL)
		{
			fseek(ptr, (id-1) * 2, SEEK_SET);
			if(exists==1)
			{
				fwrite("1", 1, 1, ptr);
			}
//			else
//			{
//				fwrite("0", 1, 1, ptr);
//			}
			fclose(ptr);
		}
	}
}

