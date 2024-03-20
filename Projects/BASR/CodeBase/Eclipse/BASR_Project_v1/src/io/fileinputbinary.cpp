/*
 * inputbinary.cpp
 *
 *  Created on: Oct 15, 2018
 *      Author: hassan
 */

#define FILEINPUTBINARY_CPP_

#include "../io/fileinputbinary.h"

UWord8 readFileBinary(Word8* filename, L32 allowedSize, UWord8* buffer)
{
	FILE* file;
    L32 lSize;

	file = fopen(filename, "rb");
	if(file == NULL)
	{
		return 0;
	}
	else
	{
        fseek(file, 0, SEEK_END);
        lSize = ftell(file);

        //if file size is greater than normal file, we won't read or use it
        if(lSize > allowedSize)
        {
            fclose(file);
    		return 0;
        }

        rewind(file);
        memset((void*)buffer, 0, sizeof(buffer));
        if(1 != fread(buffer, lSize, 1, file))
        {
            fclose(file);
        	return 0;
        }

        fclose(file);
	}

	return 1;
}
