/*
 * inputbinary.h
 *
 *  Created on: Oct 12, 2018
 *      Author: hassan
 */

#ifndef FILEINPUTBINARY_H_
#define FILEINPUTBINARY_H_

#include "../common/common.h"

//functions
#ifdef FILEINPUTBINARY_CPP_
//global variables
#else
//externs
#endif

UWord8 readFileBinary(Word8* filename, L32 allowedSize, UWord8* buffer);
UWord8 writeFileBinary();

#endif /* FILEINPUTBINARY_H_ */
