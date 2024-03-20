/*
 * systemtime.h
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#ifndef SYSTEMTIME_H_
#define SYSTEMTIME_H_

#include <ctime>
#include <unistd.h>
#include "../common/common.h"

uint64_t getEpochTimeMilliseconds();
void setSystemDelayMicroseconds(uint64_t delay);

//All definitions
#ifdef SYSTEMTIME_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion



#endif /* SYSTEMTIME_H_ */
