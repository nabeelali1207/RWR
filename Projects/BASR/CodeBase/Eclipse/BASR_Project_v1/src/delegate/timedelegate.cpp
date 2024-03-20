/*
 * timedelegate.cpp
 *
 *  Created on: Oct 18, 2018
 *      Author: hassan
 */

#define TIMEDELEGATE_CPP_

#include "../delegate/timedelegate.h"

uint64_t timeGetMicrosecDiffTimer1()
{
	uint64_t currentSystemTime = 0;
	uint64_t timeDifference = 0;

	currentSystemTime = Utility::getSystemTime();
	if ((Model::timer1EpochMicroseconds == 0)
			|| (Model::timer1EpochMicroseconds > currentSystemTime))
	{
		Model::timer1EpochMicroseconds = currentSystemTime;
	}
	else
	{
		timeDifference = currentSystemTime - Model::timer1EpochMicroseconds;
		Model::timer1EpochMicroseconds = currentSystemTime;
	}

	return timeDifference;
}
void timeSetMsDelay(uint64_t delay)
{
	if (delay > 0)
	{
		setSystemDelayMicroseconds(delay * 1000);
	}
}

void timeSetSimulationIteration(uint64_t &simTimeMicrosecIterationActual)
{

}
void timeSetToaReference(vector<PdwDefaultClass> &currentPdws,
		uint64_t &timeToaReference)
{
	if(currentPdws.size()>0)
	{
		timeToaReference=currentPdws.back().timeOfArrivalStart;
	}
}

