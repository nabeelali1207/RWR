/*
 * trackrangeclass.h
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#ifndef TRACKRANGECLASS_H_
#define TRACKRANGECLASS_H_

#include "../common/common.h"
#include "../utility/consthigh.h"
#include <boost/circular_buffer.hpp>

using namespace boost;

class TrackRangeClass {
public:
	uint16_t amplitudeCountMax;
	uint16_t amplitudeCountDelta;
	uint16_t amplitudeStepDisplay;

	uint32_t timeIterCountRangeCalc;
	uint32_t timeIterLimitRangeCalc;

	uint64_t rangeMeters;
	uint64_t previousRangeMeters;

	TrackRangeClass();
	virtual ~TrackRangeClass();
};


#endif /* TRACKRANGECLASS_H_ */
