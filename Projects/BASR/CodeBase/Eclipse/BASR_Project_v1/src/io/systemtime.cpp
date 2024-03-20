/*
 * systeminputtime.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#define SYSTEMTIME_CPP_

#include "../io/systemtime.h"

uint64_t getEpochTimeMilliseconds() {
	uint64_t msSinceEpoch=0;
//	struct timespec spec;
//
//	clock_gettime(CLOCK_REALTIME, &spec);
//	msSinceEpoch =
//	    (uint64_t)(spec.tv_sec) * 1000 +
//	    (uint64_t)(spec.tv_nsec) / 1000000;
	//cout << "System Time: " << msSinceEpoch << endl;
	//usleep(1000);
	return msSinceEpoch;
}

void setSystemDelayMicroseconds(uint64_t delay)
{
	usleep(delay);
}
