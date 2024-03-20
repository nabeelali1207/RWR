/*
 * serialbuffer.cpp
 *
 *  Created on: Oct 8, 2019
 *      Author: gmd
 */

#include "serialbuffer.h"

SerialBufferClass::SerialBufferClass(SerialBufferStreamTypeEnum _streamType, int sz)
{
	streamValid = true;
	setStreamType(_streamType);
	maxBufferSize = sz;
	bufferPointer = new char[maxBufferSize];
	currPos = 0;
}

SerialBufferClass::SerialBufferClass(const SerialBufferClass & other)
{
	streamValid = other.streamValid;
	setStreamType(other.streamType);
	maxBufferSize = other.maxBufferSize;
	bufferPointer = new char[maxBufferSize];
	memcpy((void *) bufferPointer, (void *) other.bufferPointer, maxBufferSize);
	seekPosition(other.currPos);
}

SerialBufferClass::~SerialBufferClass()
{
	delete[] (bufferPointer);
}

char * SerialBufferClass::getRawBufferPointer()
{
	return bufferPointer;
}
bool SerialBufferClass::isConsistent()
{
	return streamValid;
}

unsigned int SerialBufferClass::getPosition()
{
	return currPos;
}

void SerialBufferClass::seekPosition(unsigned int n)
{
	currPos = n;
}
unsigned int SerialBufferClass::getMaxBufferSize()
{
	return maxBufferSize;
}
void SerialBufferClass::setStreamType(SerialBufferStreamTypeEnum st)
{
	streamType = st;
	currPos = 0;
	streamValid = true;
}

void SerialBufferClass::alignToNBytes(int n)
{
	if (streamValid == false)
		return;
	long writePosition = currPos;
	long paddingBytes = n - (writePosition % n);
	if (paddingBytes == n)
		return;
	for (int i = 0; i < paddingBytes; i++)
	{
		bufferPointer[currPos] = 'A';
		currPos++;
		if (currPos >= maxBufferSize)
		{
			streamValid = false;
			return;
		}
	}

}

void SerialBufferClass::writeToBuffer(char * data, int size)
{
	if (streamValid == false)
		return;
	for (int i = 0; i < size; i++)
	{
		bufferPointer[currPos] = data[i];
		currPos++;
		if (currPos >= maxBufferSize)
		{
			streamValid = false;
			return;
		}
	}
}

void SerialBufferClass::readFromBuffer(char * data, int size)
{
	if (streamValid == false)
		return;
	for (int i = 0; i < size; i++)
	{
		data[i] = bufferPointer[currPos];
		currPos++;
		if (currPos >= maxBufferSize)
		{
			streamValid = false;
			return;
		}
	}
}
void SerialBufferClass::writeToFile()
{
	std::ofstream fid;
	fid.open("output/DataAnalysis.hex",
			std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
	fid.write(this->bufferPointer, currPos);
	fid.close();
}
