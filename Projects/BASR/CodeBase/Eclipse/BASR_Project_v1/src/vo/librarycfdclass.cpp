/*
 * librarycfdclass.cpp
 *
 *  Created on: Jun 11, 2021
 *      Author: zumair
 */

#define LIBRARYCFDCLASS_CPP_

#include "../vo/librarycfdclass.h"

CfdLibraryMetaClass::CfdLibraryMetaClass() {
    memset(bufferForCfdLibrary, 0, sizeof(CfdLibraryStruct));
}

CfdLibraryMetaClass::~CfdLibraryMetaClass() {
}
