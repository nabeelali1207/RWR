/*
 * fileinputoutputdelimited.cpp
 *
 *  Created on: Oct 14, 2019
 *      Author: gmd
 */

#include "fileinputoutputdelimited.h"

FileOutputDelimitedClass::FileOutputDelimitedClass()
{
	validFileOpen = false;
	maxAllowedFileSize = 512 * (1 << 20);
	maxAllowedFileSize = 1024 * (1 << 20);
}
void FileOutputDelimitedClass::openFile(std::string & fileName)
{

	fileStream.open(fileName,
			std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
	validFileOpen = fileStream.is_open();
	if (validFileOpen)
	{
		std::cout << "File: " << fileName << " Opened For Writing!" << endl;
	}
	else
	{
		std::cout << "Unable To open: " << fileName << " for Writing!" << endl;
	}

}
bool FileOutputDelimitedClass::isValidFileOpen()
{
	return validFileOpen;
}
bool FileOutputDelimitedClass::writeDelimitedDataToDisk(uint8_t * dataPtr,
		uint32_t size)
{
	if (validFileOpen)
	{
		//std::cerr<<"Error code"<<strerr(err)<<std::endl
		if (((unsigned long) fileStream.tellp() + (unsigned long) size)
				> maxAllowedFileSize)
		{
			fileStream.seekp(0, ios_base::beg);
		}
		fileStream.write((char *) dataPtr, size);
		return true;
	}
	else
	{
		cout << "Writing to File Failed" << endl;
	}
	return false;
}

uint64_t FileInputDelimitedClass::findNextDelimiterPosition(
		uint64_t startPosition, uint32_t offset, uint32_t step)
// step size is defined by message alignment
// by default a delimited message is assumed to be 64 byte aligned
{
	uint64_t originalPosition = fileStream.tellg();

	// find starting point of the search
	uint64_t foundPosition = startPosition;
	startPosition = startPosition + (offset * step);

	for (uint64_t i = startPosition; i < fileSize - sizeof(delimiter); i +=
			step)
	{
		fileStream.seekg(i, ios::beg);
		uint64_t tempDelim = 0;
		fileStream.read((char *) &tempDelim, sizeof(delimiter));
		if (tempDelim == delimiter)
		{
			foundPosition = i;
			break;
		}
	}
	fileStream.seekg(originalPosition, ios::beg);
	return foundPosition;
}
FileInputDelimitedClass::FileInputDelimitedClass()
{
	fileFinishedFlag = true;
	validFileOpen = false;
	lastDelimPosition = 0;
	delimiter = 0xdeadbeefbeeffeed;
	fileSize = 0;
}
void FileInputDelimitedClass::openFile(std::string & fileName)
{
	lastDelimPosition = 0;
	delimiter = 0xdeadbeefbeeffeed;
	fileStream.open(fileName, ios::in | ios::binary);

	validFileOpen = fileStream.is_open();
	if (validFileOpen)
	{
		fileFinishedFlag = false;
		fileStream.seekg(0, ios::end);
		fileSize = fileStream.tellg();
		fileStream.seekg(0, ios::beg);

		// to find start point of first message
		if (fileSize > 8)
		{
			std::cout << "File: " << fileName << " Opened For Reading!" << endl;
			lastDelimPosition = findNextDelimiterPosition(0); // find first delim position
		}
		else
		{
			std::cout << "File: " << fileName << " Does not contain any info!"
					<< endl;
			fileFinishedFlag = true;
		}
	}
	else
	{
		fileFinishedFlag = true;
		std::cout << "Unable To open: " << fileName << " for Reading!" << endl;
	}

}
bool FileInputDelimitedClass::isValidFileOpen()
{
	return validFileOpen;
}
uint64_t FileInputDelimitedClass::readDelimitedDataFromDisk(uint8_t * dataPtr,
		uint32_t maxBufferSize)
{
	if (fileFinishedFlag)
		return 0;

	uint64_t nextDelimPosition = findNextDelimiterPosition(lastDelimPosition,
			1);
	if (nextDelimPosition == lastDelimPosition)
	{ // in case of last message the delimiter will not be found as there is no next message
	  // in this case we go to end of the file
		nextDelimPosition = fileSize;
		fileFinishedFlag = true;
	}

	int64_t bytesRead = nextDelimPosition - lastDelimPosition;

	if (bytesRead > 0 && bytesRead <= maxBufferSize)
	{
		fileStream.seekg(lastDelimPosition, ios::beg);
		fileStream.read((char *) dataPtr, bytesRead);
	}
	else
	{
		bytesRead = 0;
	}

	lastDelimPosition = nextDelimPosition;
	return bytesRead;
}
