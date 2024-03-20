/*
 * trackgeneralclass.h
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#ifndef TRACKGENERALCLASS_H_
#define TRACKGENERALCLASS_H_

#include "../common/common.h"
#include "../vo/pdwdefaultclass.h"
#include <boost/circular_buffer.hpp>
#include "../utility/constlow.h"
#include "../io/serialbuffer.h"

using namespace boost;

enum class TrackTypeEnum
{
	track_pulse, track_cw, track_icw, track_unknown
};

class TrackGeneralClass
{
private:
	static uint64_t IDCounter;
public:
	static uint64_t getUniqueID();

	uint64_t trackID;
	TrackTypeEnum trackType;

	//low variables
	circular_buffer<PdwDefaultClass> dspPdwBuffer;
	uint32_t dspIterationPulseCount;
	bool dspTrackingEnabled;
	bool dspCandidateForStringingFlag; // a flag only to represent a target for stringing ( can still be source)
	bool dspDeleteTrack;
	bool updateOffFlag;//stringing usage
	uint64_t timeToDeclareSilent;//if no pulse is synched in this time, DSP tracking will be disabled
	uint64_t iterSysLastUpdate;//system iteration when track was updated
	uint64_t timeToaLastUpdate;//equivalent to toaLast
	uint64_t timeSysLastUpdate;//system time when track was updated
	uint64_t timeSysCreation;//system time when track was created
	uint64_t toaFirst;//stringing usage
	uint64_t toaLast;//stringing usage
	uint64_t toaFirstOverlapAnalysis;//specific use in overlap analysis
	uint8_t toaFirstUpdatedFlag; // flag is set on first successful pri analysis
	uint16_t totalStringTracksCount;
	uint16_t iterStringTracksCount;
	//high variables
	uint32_t trackPulseCountTotal;//stringing usage
	uint32_t trackPulseCountIterationLast;//stringing usage
	uint8_t priority;
	uint8_t priorityDynamic;
	uint8_t flagDeletionHigh;
	uint8_t flagVisibleHigh;
	uint8_t flagVisiblePriority;
	uint8_t flagSuppressEmitter;
	uint8_t flagProbation;
	uint8_t flagDemod;
	uint8_t flagSelfTest;
	uint8_t processingHighFlag;
	uint32_t timeIterCountLifetime;
	uint32_t timeIterLimitLifetime;
	uint64_t timeIterCountLifetimeGlobal;
	uint8_t flagTrackSentToTrenz;
	//not needed, reset lifetime when visibility is set to true
	//uint32_t timeIterVisibleSet;
	uint32_t timeIterCountDemodSupprSet;
	uint32_t timeIterLimitDemodSupprSet;
	uint32_t timeIterCountProbationSet;
	uint32_t timeIterLimitProbationSet;

	//potential variable list, not yet final
	//uint64_t toaStartOverlapAnalysis

	TrackGeneralClass();
	virtual ~TrackGeneralClass();
	void serialize(SerialBufferClass & buffer);
};

#endif /* TRACKGENERALCLASS_H_ */
