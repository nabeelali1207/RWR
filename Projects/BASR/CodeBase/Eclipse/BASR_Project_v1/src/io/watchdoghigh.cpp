/*
 * watchdoghigh.cpp
 *
 *  Created on: Feb 26, 2021
 *      Author: saad
 */

#include "watchdoghigh.h"

UWord8 WatchdogHigh::initializeWatchdogHigh(void)
{
	watchdogControl = open("/dev/watchdog0", O_WRONLY);
    if(watchdogControl < 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


void WatchdogHigh::resetWatchdogHigh(void)
{
    write(watchdogControl, "\0", 1);
}

