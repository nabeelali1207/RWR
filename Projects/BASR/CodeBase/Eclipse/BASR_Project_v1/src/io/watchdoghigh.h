/*
 * watchdoghigh.h
 *
 *  Created on: Feb 26, 2021
 *      Author: saad
 */

#ifndef WATCHDOGHIGH_H_
#define WATCHDOGHIGH_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include "../common/common.h"

class WatchdogHigh {
private:
	UWord32 watchdogControl;
public:
	/*
	 * Function Name:   initializeWatchdogHigh
	 * Purpose:			Initializing the watchdog
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	UWord8 initializeWatchdogHigh(void);
	/*
	 * Function Name:   resetWatchdogHigh
	 * Purpose:			Resets the watchdog, if watchdog is initialized
	 * 					and not being reset, board will be restarted
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void resetWatchdogHigh(void);
};

#endif /* WATCHDOGHIGH_H_ */
