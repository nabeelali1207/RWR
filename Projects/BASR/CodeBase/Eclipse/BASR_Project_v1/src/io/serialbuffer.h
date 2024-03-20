/*
 * serialbuffer.h
 *
 *  Created on: Oct 3, 2019
 *      Author: gmd
 */

#ifndef SERIALBUFFER_H_
#define SERIALBUFFER_H_

#include <boost/circular_buffer.hpp>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
using std::vector;
enum class SerialBufferStreamTypeEnum
{
	InputStream, OutputStream
};
class SerialBufferClass
{
private:
	bool streamValid;
	SerialBufferStreamTypeEnum streamType;
	char* bufferPointer;
	unsigned int currPos;
	unsigned int maxBufferSize;

public:

	SerialBufferClass(SerialBufferStreamTypeEnum _streamType =
			SerialBufferStreamTypeEnum::InputStream, int sz = 1000000);
	SerialBufferClass(const SerialBufferClass & other);
	~SerialBufferClass();
	char * getRawBufferPointer();
	bool isConsistent();
	unsigned int getPosition();
	void seekPosition(unsigned int n);
	unsigned int getMaxBufferSize();
	void setStreamType(SerialBufferStreamTypeEnum st);
	void alignToNBytes(int n);
	void writeToBuffer(char * data, int size);
	void readFromBuffer(char * data, int size);
	void writeToFile();

	template<class T>
	void doTransaction(T & var)
	{
		switch (streamType)
		{
		case SerialBufferStreamTypeEnum::InputStream:
			writeToBuffer((char *) &var, sizeof(T));
			break;
		case SerialBufferStreamTypeEnum::OutputStream:
			readFromBuffer((char *) &var, sizeof(T));
			break;
		}
	}

	template<class V>
	inline void serialize(V & var, decltype(&V::serialize))
	{
		var.serialize(*this);
	}
	template<class V>
	inline void serialize(V & var,...)
	{
		doTransaction(var);
	}

	template<class U>
	void serialize(std::vector<U> & var, decltype(&U::serialize))
	{
		unsigned vectorSize = var.size();
		doTransaction(vectorSize);
		if (streamType == SerialBufferStreamTypeEnum::OutputStream)
		{
			var.resize(vectorSize);
		}
		for (unsigned i = 0; i < vectorSize; i++)
		{
			var[i].serialize(*this);
		}
	}
	template<class U>
	void serialize(std::vector<U> & var,...)
	{
		unsigned vectorSize = var.size();
		doTransaction(vectorSize);
		if (streamType == SerialBufferStreamTypeEnum::OutputStream)
		{
			var.resize(vectorSize);
		}
		for (unsigned i = 0; i < vectorSize; i++)
		{
			doTransaction(var[i]);
		}
	}
	template<class T>
	void serialize(boost::circular_buffer<T> & var, decltype(&T::serialize))
	{
		unsigned circCapacity = var.capacity();
		unsigned circSize = var.size();
		doTransaction(circCapacity);
		doTransaction(circSize);

		if (streamType == SerialBufferStreamTypeEnum::OutputStream)
		{
			var.set_capacity(circCapacity);
			var.resize(circSize);
		}
		for (unsigned i = 0; i < circSize; i++)
		{
			var[i].serialize(*this);
		}
	}
	template<class T>
	void serialize(boost::circular_buffer<T> & var,...)
	{
		unsigned circCapacity = var.capacity();
		unsigned circSize = var.size();
		doTransaction(circCapacity);
		doTransaction(circSize);

		if (streamType == SerialBufferStreamTypeEnum::OutputStream)
		{
			var.set_capacity(circCapacity);
			var.resize(circSize);
		}
		for (unsigned i = 0; i < circSize; i++)
		{
			doTransaction(var[i]);
		}
	}
};

#endif /* SERIALBUFFER_H_ */
