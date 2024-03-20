/*
 * liveplaybackpdwprovider.cpp
 *
 *  Created on: Jan 18, 2023
 *      Author: gmd
 */
#include "liveplaybackpdwprovider.h"

void PlaybackPdwProvider::initialize()
{
	pdwsSinceLastRead.resize(0);
	playbackPdwsTotal.resize(0);
	newPdwsToPushInSpsc.resize(0);
	newPdwsToPushInSpsc.reserve(SPSC_CAPACITY);
	pdwsSinceLastRead.reserve(MAX_PDWS);

	spsc_queue.reset();

	systemTimeToToaOffsetMicros = Utility::getSystemTime()-Utility::getSystemTimeFromBoot();
	playbackCaseDuration = 0;

	playbackTotalPdwCnt = 0;
	playbackedPdwCnt = 0;

	playbackCyclicFlag = false;
	playbackRunningFlag = false;
	playbackCompletedFlag = false;

	validFileLoadedFlag = false;

	lastUpdateTime = 0;
	playbackStartTimeMicros = 0;
	toaOffset = 0;

	lastSentToa = 0;
	//playbackMode = PlaybackMode::TimeStepDataSync;
	playbackMode = PlaybackMode::TimeStepDuration;

}

void PlaybackPdwProvider::addPdwsForPlayback(vector<PdwSmall32>& newPdws)
{
	if (playbackRunningFlag == false)
	{
		if (playbackPdwsTotal.capacity()
				< (playbackPdwsTotal.size() + newPdws.size()))
		{
			playbackPdwsTotal.reserve(
					playbackPdwsTotal.size() + newPdws.size());
		}
		playbackPdwsTotal.insert(playbackPdwsTotal.end(), newPdws.begin(),
				newPdws.end());
	}

	std::sort(playbackPdwsTotal.begin(), playbackPdwsTotal.end(),PdwSmall32::toaComparison);

	playbackTotalPdwCnt = playbackPdwsTotal.size();

}
void PlaybackPdwProvider::clearPdws()
{
	playbackPdwsTotal.resize(0);
	spsc_queue.reset();

}

void PlaybackPdwProvider::startPlayback(bool cyclicMode)
{
	if (playbackRunningFlag == false && playbackPdwsTotal.size() > 0)
	{
		playbackIterationNumber = 0;
		playbackedPdwCnt = 0;
		playbackTotalPdwCnt = playbackPdwsTotal.size();
		playbackRunningFlag = true;
		playbackCompletedFlag = false;
		playbackCyclicFlag = cyclicMode;

		playbackStartTimeMicros = Utility::getSystemTime();

		uint64_t timeDiff = (playbackStartTimeMicros
				- systemTimeToToaOffsetMicros);

		toaOffset = ((timeDiff + 999999) / 1000000) * 1000000; // round to next second
		toaOffset = toaOffset * 100; // conversion of micros to 100Mhz based clock counter

		lastSentToa = toaOffset;
		playbackCaseDuration = (playbackPdwsTotal.back().TOA
				- playbackPdwsTotal.front().TOA) / 100;

	}
}
void PlaybackPdwProvider::stopPlayback()
{
	spsc_queue.reset();
	playbackRunningFlag = false;
	playbackCompletedFlag = false;
}

void PlaybackPdwProvider::setPlaybackOffsetForToa(uint64_t offset)
{
	systemTimeToToaOffsetMicros = offset;
}

void PlaybackPdwProvider::update()
{

	uint64_t currTime = Utility::getSystemTime();

	if ((currTime - lastUpdateTime) < ITERATION_TIMESTEP)
	{
		return;
	}

	lastUpdateTime = currTime;

	if (playbackCompletedFlag == true)
	{
		playbackRunningFlag = false;
		if (playbackCyclicFlag == true)
		{
			if (currTime - playbackCompletionTimeStamp > 1000000)
			{
				startPlayback(playbackCyclicFlag);
			}
		}
	}

	if (playbackRunningFlag == false)
		return;

	playbackIterationNumber++;

	switch (playbackMode)
	{
	case PlaybackMode::TimeStepDuration:
	{
		uint64_t newLastToa = (currTime - playbackStartTimeMicros) * 100;
		lastSentToa = newLastToa;
		sendNewPdwsToSharedBuffer(lastSentToa);
		break;
	}
	case PlaybackMode::TimeStepIteration:
	{
		uint64_t newLastToa = (currTime - playbackStartTimeMicros) * 100;
		lastSentToa = newLastToa;
		fillOutputBufferWithSpill(lastSentToa);
		break;
	}
	case PlaybackMode::TimeStepDataSync:
	{
		uint64_t newLastToa = (currTime - playbackStartTimeMicros) * 100;
		lastSentToa = newLastToa;
		fillOutputBufferWithoutSpill(ITERATION_TIMESTEP * 100);
		break;
	}
	default:

		break;
	}


}

void PlaybackPdwProvider::fillOutputBufferWithSpill(uint64_t lastToa)
{
	uint32_t spscSpace = spsc_queue.write_available();
	uint32_t currIterationPdws = 0;
	newPdwsToPushInSpsc.resize(0);
	PdwSmall32 firstPdw;
	if (playbackedPdwCnt < playbackTotalPdwCnt)
	{
		firstPdw = playbackPdwsTotal[playbackedPdwCnt];
	}
	PdwSmall32 lastPdw;
	for (; playbackedPdwCnt < playbackTotalPdwCnt; playbackedPdwCnt++)
	{
		lastPdw = playbackPdwsTotal[playbackedPdwCnt];
		if (lastPdw.getTOA() < lastToa)
		{
			break;
		}
		if (currIterationPdws < spscSpace)
		{ //read only those pdws that are to be sent to other thread
			newPdwsToPushInSpsc.push_back(lastPdw);
			newPdwsToPushInSpsc[currIterationPdws].TOA += toaOffset;
			currIterationPdws++;
		}
	}

	if (playbackedPdwCnt < playbackTotalPdwCnt)
	{ // not the last iteration

	}
	else
	{ // last iteration
		playbackCompletedFlag = true;
		playbackCompletionTimeStamp = Utility::getSystemTime();
	}

	spsc_queue.push(&newPdwsToPushInSpsc[0], currIterationPdws);

}
void PlaybackPdwProvider::fillOutputBufferWithoutSpill(uint64_t windowSize)
{
	uint32_t spscSpace = spsc_queue.write_available();
	uint32_t currIterationPdws = 0;
	newPdwsToPushInSpsc.resize(0);
	PdwSmall32 firstPdw;
	if (playbackedPdwCnt < playbackTotalPdwCnt)
	{
		firstPdw = playbackPdwsTotal[playbackedPdwCnt];
	}
	PdwSmall32 lastPdw;
	for (; playbackedPdwCnt < playbackTotalPdwCnt; playbackedPdwCnt++)
	{
		lastPdw = playbackPdwsTotal[playbackedPdwCnt];
		uint64_t aa =firstPdw.getTOA();
		uint64_t bb =lastPdw.getTOA();
		if (currIterationPdws < spscSpace
				&& (bb-aa < windowSize))
		{ //read only those pdws that are to be sent to other thread
			newPdwsToPushInSpsc.push_back(lastPdw);
			newPdwsToPushInSpsc[currIterationPdws].TOA += toaOffset;
			currIterationPdws++;
		}
		else
		{
			break;
		}
	}

	if (playbackedPdwCnt < playbackTotalPdwCnt)
	{ // not the last iteration

	}
	else
	{ // last iteration
		playbackCompletedFlag = true;
		playbackCompletionTimeStamp = Utility::getSystemTime();
	}

	spsc_queue.push(&newPdwsToPushInSpsc[0], currIterationPdws);

}

void PlaybackPdwProvider::sendNewPdwsToSharedBuffer(uint64_t lastToa)
{
	uint32_t spscSpace = spsc_queue.write_available();
	uint32_t currIterationPdws = 0;
	newPdwsToPushInSpsc.resize(0);
	PdwSmall32 lastPdw;
	for (; playbackedPdwCnt < playbackTotalPdwCnt; playbackedPdwCnt++)
	{
		lastPdw = playbackPdwsTotal[playbackedPdwCnt];
		if (lastPdw.getTOA() > lastToa)
		{
			break;
		}
		if (currIterationPdws < spscSpace)
		{ //read only those pdws that are to be sent to other thread
			newPdwsToPushInSpsc.push_back(lastPdw);
			newPdwsToPushInSpsc[currIterationPdws].TOA += toaOffset;
			currIterationPdws++;
		}
	}

	if (playbackedPdwCnt < playbackTotalPdwCnt)
	{ // not the last iteration

	}
	else
	{ // last iteration
		playbackCompletedFlag = true;
		playbackCompletionTimeStamp = Utility::getSystemTime();
	}

	spsc_queue.push(&newPdwsToPushInSpsc[0], currIterationPdws);

}

vector<PdwSmall32> & PlaybackPdwProvider::getReceivedPdws()
{
	uint32_t availableCount = spsc_queue.read_available();
	uint32_t pdwsToRead = min((uint32_t) 256, availableCount);
	pdwsSinceLastRead.resize(pdwsToRead);
	if (availableCount > 0)
	{
		spsc_queue.pop(&pdwsSinceLastRead[0], pdwsToRead);
	}
	return pdwsSinceLastRead;
}
