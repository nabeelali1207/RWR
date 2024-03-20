/*
 * trackhistoryclass.h
 *
 *  Created on: Sep 26, 2019
 *      Author: hassan
 */

#ifndef TRACKHISTORYCLASS_H_
#define TRACKHISTORYCLASS_H_

#include "../common/common.h"
#include "../utility/consthigh.h"
#include <boost/circular_buffer.hpp>

using namespace boost;

class TrackHistoryClass {
public:

	//legacy high side track potential
	uint8_t lowHighFlag;
	uint32_t stitchedCount;
	uint32_t stitchedJitterAndComplexCount;
	uint32_t stitchedDiffToaCount;
	uint64_t totalToaOverlap;
	uint32_t totalPulseOverlap;
	uint32_t totalPulseCount;
	uint16_t stitchedUpdateCount;
	uint16_t stitchedUpdateNextCount;
	uint16_t stitchedUpdateDiffToaCount;
	uint16_t stitchedUpdateNextDiffToaCount;
	uint8_t activeFlag;//this is depriciated and marked for removal
	//low track ids of all updated and strung tracks in high
	circular_buffer<uint64_t> idsAssociateTracksLow;
	//low track indexs of all updated and strung tracks in high
	//this is reset every iteration
	vector <uint64_t> idsAssociateTracksIterLow;
	vector <uint64_t> indexAssociateTracksIterLow;


	//
	uint32_t countConsecutiveSameSpotFixedPri;

	//legacy high side stringing variables and structures
	//these variables need to be renamed properly
	uint32_t countPulseWidthOverlapMaxLow;
	uint32_t countPriTypeOverlapMaxLow;
	uint32_t countPulseOverlapMaxLow;
	uint32_t countTrackOverlapMaxLow;
	uint64_t durationToaOverlapMaxLow;
	uint8_t flagOverlapPotential;

	uint32_t lowToaStartUpdateCount;
	uint32_t highToaStartUpdateTotalCount;
	uint32_t highToaStartUpdateConsecutiveCount;
	uint8_t highToaStartUpdateFlag;//this has to be reset every iteration
	uint8_t highTrackContinuationIterFlag;
	uint8_t lowToaStartUpdateFlag;
	uint8_t lowToaStartUpdateNextFlag;
	uint16_t lowIndexUpdateNext;//the updated track index of low report in next track list

	uint8_t flagUpdateOnceIterHigh;
	TrackHistoryClass();
	~TrackHistoryClass();
};



#endif /* TRACKHISTORYCLASS_H_ */
