/*
 * trackpriclass.h
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#ifndef TRACKPRICLASS_H_
#define TRACKPRICLASS_H_

#include "../common/common.h"
#include "../utility/constlow.h"
#include "../utility/consthigh.h"
#include "limitpriclass.h"
#include <boost/circular_buffer.hpp>

#include "../delegate/analysistrackprilowdelegate.h"
#include "../io/serialbuffer.h"
using namespace boost;

class TrackPriClass {
public:
	//create pri limit circular buffer here
	//create simple difftoa limit circular buffer as well?
	//do we need pri hierarchy for different types of sliding such as sin,
	//..square,sawtooth, triangular and their deltas?
	//determine tolerance deltas for differnt pri types (i.e. fixed = 0.9-1.1us
	//..this is needed for pri analysis algo and possible mods to pri structs
	//important sequence would need separate structures to preserve information

	//low variables
	vector<AnalysisTrackPriLowClass> priTrackingObj;
	bool isSeqSearchObjectValid;

	circular_buffer<LimitPriClass> priCircular;//need first and last pri in seq
	vector<LimitPriClass> priFirst;//stringing usage
	//priLast stringing usage, can be gotten from priCircular
	//stringing usage, may also require last few pulses diff-toa
	uint16_t toaBufferSizeFirst;//stringing usage
	uint16_t toaBufferSizeLast;//stringing usage
	uint64_t lastPriAnalysistime;//stringing usage
	uint16_t lastPriAnalysisPulseCount;//stringing usage, redundant?
	uint32_t lastPriAnalysisSpotCount;
	//uint64_t toaLockedFirst;

	uint8_t priTrackingPossible;
	uint32_t curPriSpot;
	uint32_t maxPriSpot;
	uint32_t curExpectPriSpot;
	PriTypeEnum curPriType;

	uint16_t countPdwAnalysisLast;//must be incremented after each pdw tracking
	uint16_t countPdwAnalysisWindow;//must be computed using last known pri
	uint64_t timeToaAnalysisLast;//must be incremented after each pdw tracking
	uint64_t diffToaAnalysisWindow;//must be computed using last known pri

	//high variables
	PriTypeEnum priTypeCurrentDominant;
	PriTypeEnum priTypeFutureDominant;
	uint8_t flagPriAnalysisLimitedFirst;//has to be set initially on create, reverify!!
	uint8_t flagPriAnalysisLimitedLast;//has to be set initially on create
	circular_buffer<LimitPriClass> accumulatePri;//lib match usage
	uint8_t accumulatePriFixedCount;//lib match usage
	uint8_t accumulatePriDwellSwitchCount;//lib match usage
	uint8_t accumulatePriStaggerCount;//lib match usage
	uint8_t accumulatePriSlidingCount;//lib match usage
	uint8_t accumulatePriJitterCount;//lib match usage
	uint8_t accumulatePriComplexCount;//lib match usage
	uint8_t accumulatePriDiffToaCount;//lib match usage

	TrackPriClass();
	LimitPriClass getDwellSwitchFirstSpot();
	LimitPriClass getDwellSwitchLastSpot();
	LimitPriClass getDwellSwitchMinSpot();
	LimitPriClass getDwellSwitchMaxSpot();
	LimitPriClass getStaggerPhaseFirstSpot();
	LimitPriClass getStaggerPhaseLastSpot();
	LimitPriClass getStaggerPhaseMinSpot();
	LimitPriClass getStaggerPhaseMaxSpot();
	LimitPriClass getStaggerFrameFirstSpot();
	LimitPriClass getStaggerFrameLastSpot();
	LimitPriClass getStaggerFrameMinSpot();
	LimitPriClass getStaggerFrameMaxSpot();
	LimitPriClass getSlidingFirstRange();
	LimitPriClass getSlidingLastRange();
	LimitPriClass getSlidingMinRange();
	LimitPriClass getSlidingMaxRange();
	LimitPriClass getJitterFirstRange();
	LimitPriClass getJitterLastRange();
	LimitPriClass getJitterMinRange();
	LimitPriClass getJitterMaxRange();
	LimitPriClass getComplexFirstRange();
	LimitPriClass getComplexLastRange();
	LimitPriClass getComplexMinRange();
	LimitPriClass getComplexMaxRange();
	LimitPriClass getDiffToaFirstSpot();
	LimitPriClass getDiffToaLastSpot();
	LimitPriClass getDiffToaMinSpot();
	LimitPriClass getDiffToaMaxSpot();

	void allocateSequenceSearchObject();
	~TrackPriClass();
	void serialize(SerialBufferClass & buffer);
};


#endif /* TRACKPRICLASS_H_ */
