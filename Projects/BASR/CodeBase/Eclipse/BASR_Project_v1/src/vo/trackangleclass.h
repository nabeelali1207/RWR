/*
 * trackangleclass.h
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#ifndef TRACKANGLECLASS_H_
#define TRACKANGLECLASS_H_

#include "../common/common.h"
#include <boost/circular_buffer.hpp>
#include "../io/serialbuffer.h"
#include "../utility/consthigh.h"

using namespace boost;

class TrackAngleClass {
public:
	uint8_t curQuadrant;
	uint16_t curAngle;//maybe redundant
	uint16_t minimumAngle;
	uint16_t maximumAngle;
	uint16_t pdwCountSinceLastAoaAnlaysis;
	uint64_t timeToaAngleUpdate;//maybe redundant
	uint64_t timeSysAngleUpdate;//maybe redundant

	uint16_t bearingFirst;//stringing usage, calculated on high side
	uint16_t bearingLast;//stringing usage, calculated on high side
	uint16_t bearingPreviousLast;
	int16_t bearingRemainderLast;
	uint16_t bearingFilterLimit;
	uint16_t bearingDisplay;
	uint32_t timeIterCountBearingCalc;
	uint32_t timeIterLimitBearingCalc;

	TrackAngleClass();
	virtual ~TrackAngleClass();
	void serialize(SerialBufferClass & buffer);
};


#endif /* TRACKANGLECLASS_H_ */
