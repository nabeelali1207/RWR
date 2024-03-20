/*
 * trackcomparelass.h
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#ifndef TRACKCOMPARECLASS_H_
#define TRACKCOMPARECLASS_H_

#include "../common/common.h"
#include <boost/circular_buffer.hpp>

using namespace boost;

//need to add low side comparison variables

class TrackCompareClass {
public:

	//legacy high string comparison variables
	uint8_t parameterInitialStatus;//---
	uint8_t parameterCompareStatus;//---
	uint8_t parameterExclusionStatus;//
	uint8_t stitchCompatible;//---
	uint8_t mergeCompatible;//---
	uint8_t	angleDiff;//---/
	uint8_t	angleDiffQuadrantSwap;//---/
	uint8_t	angleCompatible;//---
	uint8_t	angleScore;//
	uint8_t	bandCompatible;//---
	uint8_t	bandScore;//
	uint32_t pulseDiffOverlap;//--/
	uint8_t	toaScore;//
	int64_t	toaDiffFirstLast;//--/
	uint64_t toaDiffOverlap;//--/
	uint8_t	pxDiff;
	uint8_t	pyDiff;
	uint8_t	pxScore;//
	uint8_t	pyScore;//
	uint8_t	pwScoreThree;//
	uint8_t	pwScoreTen;//
	uint16_t pwDiff;
	uint8_t	powerScore;//
	uint16_t powerDiff;
	uint8_t	rfScore;//
	uint8_t	threatLibraryScoreLowBar;//
	uint8_t	threatLibraryScoreHighBar;//
	uint8_t priDetailCompareStatus;//
	uint8_t	priTypeCompatible;//
	uint8_t	priTypeScore;//
	uint8_t priFailedScore;//
	uint8_t priInitialScore;//
	uint8_t priDetailScore;//
	uint32_t overallScore;

	//legacy low string comparison variables
	//add variables at their primary locations as well

	TrackCompareClass();
	virtual ~TrackCompareClass();
};


#endif /* TRACKCOMPARECLASS_H_ */
