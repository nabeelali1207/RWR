/*
 * hardwaredelegate.cpp
 *
 *  Created on: Jan 1, 2019
 *      Author: test
 */

#define HARDWAREDELEGATE_CPP_

#include "../delegate/hardwaredelegate.h"

void hardwareMountUsbDrive()
{
	if (ConstLow::simOnActualHwFlag == 1)
	{
//		mountUsbDrive();
	}
}

void hardwareMountSsdDrive()
{
	if (ConstHigh::simOnActualHwFlag == 1)
	{
		mountSsdDrive();
	}
}
