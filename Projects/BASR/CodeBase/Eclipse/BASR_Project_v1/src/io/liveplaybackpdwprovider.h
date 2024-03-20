/*
 * liveplaybackpdwprovider.h
 *
 *  Created on: Jan 18, 2023
 *      Author: gmd
 */

#ifndef LIVEPLAYBACKPDWPROVIDER_H_
#define LIVEPLAYBACKPDWPROVIDER_H_

#include <iostream>
#include "../vo/pdwdefaultclass.h"
#include "../utility/utility.h"
#include <boost/lockfree/spsc_queue.hpp>

class PlaybackPdwProvider
{
	enum class PlaybackMode
	{
		TimeStepIteration,			// outputs new data of fixed time step based on iteration number
		TimeStepDataSync,			// outputs new data such that the consumer does not miss anything
		TimeStepDuration,			// outputs new data based on elapsed time
	};
	static const uint16_t MAX_PDWS = 512;
	static const uint16_t PDW_SIZE = 32;

	static const uint16_t SPSC_CAPACITY=3000;
	static const uint64_t ITERATION_TIMESTEP = 10*1000;// micros after which pdws will be sent to spsc queue

	vector<PdwSmall32> pdwsSinceLastRead;
	vector<PdwSmall32> newPdwsToPushInSpsc;
	vector<PdwSmall32> playbackPdwsTotal;
	uint64_t lastSentToa;

	boost::lockfree::spsc_queue<PdwSmall32, boost::lockfree::capacity<SPSC_CAPACITY> > spsc_queue;

	int64_t systemTimeToToaOffsetMicros;
	uint64_t toaOffset;							//in 10nano seconds
	uint64_t playbackCaseDuration;				//in micros

	uint64_t playbackCompletionTimeStamp;
	uint32_t playbackTotalPdwCnt;
	uint32_t playbackedPdwCnt;


	bool playbackCyclicFlag;
	bool playbackRunningFlag;
	bool playbackCompletedFlag;

	bool validFileLoadedFlag;

	uint64_t lastUpdateTime;
	int64_t playbackStartTimeMicros;
	uint64_t playbackIterationNumber;

	void fillOutputBufferWithSpill(uint64_t windowSize);
	void fillOutputBufferWithoutSpill(uint64_t windowSize);
	void sendNewPdwsToSharedBuffer(uint64_t lastToa);
	
	PlaybackMode playbackMode;
public:



	void initialize();



	void startPlayback(bool cyclicMode);
	void stopPlayback();


	void addPdwsForPlayback(vector<PdwSmall32>& newPdws);
	void clearPdws();


	void setPlaybackOffsetForToa(uint64_t);

	void update();

	vector<PdwSmall32> & getReceivedPdws();

	bool isPlaybackCompleted()
	{
		return playbackCompletedFlag;
	}
	bool isPlaybackRunning()
	{
		return playbackRunningFlag;
	}
};

#endif /* LIVEPLAYBACKPDWPROVIDER_H_ */
