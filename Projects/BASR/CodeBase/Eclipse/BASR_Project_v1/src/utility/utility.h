/*
 * utility.h
 *
 *  Created on: Oct 15, 2018
 *      Author: test
 */

#ifndef UTILITY_H_
#define UTILITY_H_
#include "../common/common.h"
#include <sys/time.h>
#include <iomanip>
#include <stdio.h>

#
class Utility
{

public:

	static uint64_t getSystemTime();
	static void printTime(uint64_t systemTime=0);
	static uint64_t getSystemTimeFromBoot();
	static uint32_t calculateCRC32(uint8_t * ptr, uint32_t size);
	static void setDate(const char *);

	static uint32_t getIndexOfFirstSetBit(uint32_t);
	Utility();
	virtual ~Utility();
};

template<typename T>
T medianFilter3(T* arr)
{
//	char a=(arr[0]>arr[1] && arr[0]<arr[2])||(arr[0]<arr[1] && arr[0]>arr[2]);
//	char b=(arr[1]>arr[0] && arr[1]<arr[2])||(arr[1]<arr[0] && arr[1]>arr[2]);
//	char c=(arr[2]>arr[0] && arr[2]<arr[1])||(arr[2]<arr[0] && arr[2]>arr[1]);
//	switch(a|(b<<1)|(c<<2))
//	{
//	case 1:
//		return arr[0];
//	case 2:
//		return arr[1];
//	case 4:
//		return arr[2];
//	default:
//		return arr[0];

	T temp;
	if (arr[2] > arr[1])
	{
		temp = arr[1];
		arr[1] = arr[2];
		arr[2] = temp;
	}
	if (arr[1] > arr[0])
	{
		temp = arr[0];
		arr[0] = arr[1];
		arr[1] = temp;
	}
	if (arr[2] > arr[1])
	{
		temp = arr[1];
		arr[1] = arr[2];
		arr[2] = temp;
	}
	return arr[1];

}
template<typename T>
T medianFilterN(T* arr, uint32_t elementCount)
{
	uint32_t i, j;
	T temp;
	for (i = 0; i < (elementCount - 1); ++i)
	{
		for (j = 0; j < elementCount - 1 - i; ++j)
		{
			if (arr[j] > arr[j + 1])
			{
				temp = arr[j + 1];
				arr[j + 1] = arr[j];
				arr[j] = temp;
			}
		}
	}
	return arr[elementCount / 2];
}
template<typename T, typename sumType>
struct SmartMean
{
public:
	T minValue;
	T maxValue;
	sumType stdValue; //standard deviation
	sumType avgValue;
	void computeNormalMean(vector<T> input)
	{
		uint32_t sz = input.size();
		if (sz == 0)
		{
			minValue = maxValue = avgValue = stdValue = 0;
			return;
		}
		minValue = input[0];
		maxValue = input[0];
		avgValue = input[0];
		stdValue = 0;
		if (sz == 1)
			return;
		for (uint32_t i = 1; i < sz; i++)
		{
			minValue = (input[i] < minValue ? input[i] : minValue);
			maxValue = (input[i] > maxValue ? input[i] : maxValue);
			avgValue += input[i];
		}
		avgValue = avgValue / sz;
		for (uint32_t i = 0; i < sz; i++)
		{
			sumType temp = (input[i] - avgValue);
			stdValue += (temp * temp);
		}
		stdValue = stdValue / sz;
		stdValue = sqrt(stdValue);
	}
};
//functions, includes and defines
#ifdef UTILITY_CPP_
//global variables
#else
//externs
#endif

#endif /* UTILITY_H_ */
