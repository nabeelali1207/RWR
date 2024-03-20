/*
 * libraryclass.cpp
 *
 *  Created on: Nov 23, 2018
 *      Author: hassan
 */

#define LIBRARYCLASS_CPP_

#include "../vo/libraryclass.h"

LibraryMetaClass::LibraryMetaClass() {
	numberOfEmmiters = 0;
	numberofPris = 0;
	numberofStaggerPRIs = 0;
	numberofComplexPRIs = 0;
	numberofJitterPRIs = 0;
	numberofSliderPRIs = 0;
	numberofPWs = 0;
	numberofRFs = 0;

    memset(PRIs, 0, sizeof(PRIs));
    memset(PRIcount, 0, sizeof(PRIcount));
    memset(RFs, 0, sizeof(RFs));
    memset(RFcount, 0, sizeof(RFcount));
    memset(PWs, 0, sizeof(PWs));
    memset(PWcount, 0, sizeof(PWcount));

    memset(keyDigest, 0, sizeof(keyDigest));
    memset(bufferForUse1, 0, sizeof(bufferForUse1));
    memset(bufferForUse2, 0, sizeof(bufferForUse2));
    memset(bufferForKey, 0, sizeof(bufferForKey));
}

LibraryMetaClass::~LibraryMetaClass() {
}

//void libraryInitStructure(LibraryStruct& library)
//{
//    memset(&library, 0, sizeof(LibraryStruct));
//}
