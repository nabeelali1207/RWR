/*
 * librarycfdclass.h
 *
 *  Created on: Jun 11, 2021
 *      Author: zumair
 */

#ifndef VO_LIBRARYCFDCLASS_H_
#define VO_LIBRARYCFDCLASS_H_

#include "../common/common.h"
#include "../utility/consthigh.h"

typedef struct CfdLibraryHeader
{
	uint8_t day;
	uint8_t month;
	uint8_t year;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t majorVersion;
	uint8_t minorVersion;
	uint8_t totalSequences;
} CfdLibraryHeader;

typedef struct CfdLibraryProgram
{
	uint8_t programId;
	uint8_t numChaffGroup;
	uint8_t intervalChaffGroup;
	uint8_t chaffPerGroup;
	uint8_t sglDblChaff;
	uint8_t intervalChaff;
	uint8_t numFlare;
	uint8_t intervalFlare;
	uint8_t sglDblFlare;
} CfdLibraryProgram;

typedef struct CfdLibraryStruct
{
	CfdLibraryHeader cfdHeader;
	CfdLibraryProgram cfdProgram[240];
} CfdLibrary;

class CfdLibraryMetaClass
{
public:
	static const int SIZE_OF_CFD_LIBRARY = sizeof(CfdLibraryStruct);
	uint8_t bufferForCfdLibrary[SIZE_OF_CFD_LIBRARY];

	CfdLibraryMetaClass();
	virtual ~CfdLibraryMetaClass();
};

#endif /* VO_LIBRARYCFDCLASS_H_ */
