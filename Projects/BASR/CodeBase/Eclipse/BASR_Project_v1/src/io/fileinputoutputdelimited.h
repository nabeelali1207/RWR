/*
 * fileinputoutputdelimited.h
 *
 *  Created on: Oct 9, 2019
 *      Author: gmd
 */

#include "../common/common.h"
#include "../io/serialbuffer.h"
#include "../utility/constant.h"

#include <fstream>
#include <string>

#ifndef FILEINPUTOUTPUTDELIMITED_H_
#define FILEINPUTOUTPUTDELIMITED_H_

class FileOutputDelimitedClass
{
private:
	unsigned long maxAllowedFileSize;
	fstream fileStream;
	bool validFileOpen;
	//uint8_t * bufferPtr;

	//uint64_t lastFileWriteTime;
	//SerialBufferClass buffer;
public:
	FileOutputDelimitedClass();
	void openFile(std::string & fileName);
	bool isValidFileOpen();
	bool writeDelimitedDataToDisk(uint8_t * dataPtr, uint32_t size);
	void flushToDisk()
	{
		std::flush (fileStream);
	}
};
class FileInputDelimitedClass
{
private:
	uint64_t delimiter;
	uint64_t fileSize;
	uint64_t lastDelimPosition;
	ifstream fileStream;
	bool fileFinishedFlag;
	bool validFileOpen;

	uint64_t findNextDelimiterPosition(uint64_t startPosition, uint32_t offset =
			0, uint32_t step = 64);
public:
	FileInputDelimitedClass();
	void openFile(std::string & fileName);
	bool isValidFileOpen();
	uint64_t readDelimitedDataFromDisk(uint8_t * dataPtr,
			uint32_t maxBufferSize);
};

#endif /* FILEINPUTOUTPUTDELIMITED_H_ */
