/*
 * analysisgeneraldelegate.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#define ANALYSISGENERALDELEGATE_CPP_

#include "../delegate/analysisgeneraldelegate.h"

UWord8 calculateAngle(UWord16 oldAngle)
{
	F32 newAngle = oldAngle/360.0;
	newAngle = newAngle * 255.0;
	return (UWord8) (newAngle);
}

void calculateAmplitude(TrackPowerClass &power)
{
	F32 newAmp;
	if(power.pxMax > power.pyMax)
	{
		power.curMaxPower = power.pxMax;
	}
	else
	{
		power.curMaxPower = power.pyMax;
	}

	newAmp = power.curMaxPower / 4096.0;
	newAmp = newAmp * 15;

	power.curMaxPower = 15 - newAmp;
}
