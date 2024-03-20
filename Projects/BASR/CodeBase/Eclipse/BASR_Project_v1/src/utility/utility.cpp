/*
 * utility.cpp
 *
 *  Created on: Oct 15, 2018
 *      Author: test
 */

#define UTILITY_CPP_

#include "../utility/utility.h"

uint64_t Utility::getSystemTime()
{ //returns time in microseconds
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (((uint64_t) tv.tv_sec) * 1000000 + (uint64_t) tv.tv_usec);

	//std::chrono::system_clock::now();
}
void Utility::printTime(uint64_t systemTime)
{
	if (systemTime == 0)
	{
		systemTime = Utility::getSystemTime();
	}
	time_t rawtime;
	struct tm * timeinfo;
	rawtime = systemTime / 1000000;
	timeinfo = localtime(&rawtime);
	char timeArray[30];
	sprintf(timeArray,"%04d/%02d/%02d %02d:%02d:%02d",
			timeinfo->tm_year + 1900, timeinfo->tm_mon, timeinfo->tm_mday,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	cout << timeArray;
//	cout << timeinfo->tm_year + 1900 << "/" << std::setfill('0') << std::setw(2)
//			<< timeinfo->tm_mon << "/" << std::setfill('0') << std::setw(2)
//			<< timeinfo->tm_mday << " " << std::setfill('0') << std::setw(2)
//			<< timeinfo->tm_hour << ":" << timeinfo->tm_min << ":"
//			<< std::setfill('0') << std::setw(2) << timeinfo->tm_sec;
}
uint64_t Utility::getSystemTimeFromBoot()
{ //returns time from boot, in microseconds
	uint64_t uptime = 100000000;
	double uptime_seconds;
	if (std::ifstream("/proc/uptime", std::ios::in) >> uptime_seconds)
	{
		uptime = static_cast<unsigned long long>(uptime_seconds * 1000000.0);
	}
	return uptime;
}

uint32_t Utility::calculateCRC32(uint8_t * ptr, uint32_t size)
{
	uint8_t * alignedPtr = ptr; // ptr must be aligned to 4 byte boundery or errors will occur

	uint32_t * newPtr = (uint32_t *) alignedPtr;

	uint32_t endPoint = (size - (size % sizeof(uint32_t))) / 4;

	uint32_t crc = 0xFFFFFFFF;
	for (uint32_t i = 0; i < endPoint; i++)
	{
		crc = crc ^ *(newPtr + i);
	}
	return crc;
}

void Utility::setDate(const char *dateString)
{
	uint32_t year = 0, month = 0, day = 0;
	uint32_t hour = 0, min = 0, second = 0;
	sscanf(dateString, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min,
			&second);
	char newDateString[32] =
	{ '\0' };
	sprintf(newDateString, "%d-%d-%d %d:%d:%d", year, month, day, hour, min,
			second);

	printf("Setting new Time to: %s\n", newDateString);

	char commandBuff[32];
	sprintf(commandBuff, "date -s \"%s\"", newDateString);
	system(commandBuff);

	sprintf(commandBuff, "hwclock -w");
	system(commandBuff);

	printf("Date Set\n");

}
uint32_t Utility::getIndexOfFirstSetBit(uint32_t n)
{
	return log2(n & -n);
}
Utility::Utility()
{

}

Utility::~Utility()
{
	// TODO Auto-generated destructor stub
}
