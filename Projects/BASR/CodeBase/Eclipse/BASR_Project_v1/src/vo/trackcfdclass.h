/*
 * trackcfdclass.h
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#ifndef TRACKCFDCLASS_H_
#define TRACKCFDCLASS_H_

#include "../common/common.h"
#include <boost/circular_buffer.hpp>

using namespace boost;

class TrackCfdClass {
public:
	uint8_t validCfdThreat;
	uint8_t threatIndex;
	uint8_t cmProgramId;
	uint8_t altitudeValidity;
	uint8_t velocityValidity;
	uint8_t aoa;
	uint8_t status;
	int8_t altitude;
	uint8_t velocity;
	uint8_t flagCfdProgramFired;

	TrackCfdClass();
	virtual ~TrackCfdClass();
};

#endif /* TRACKCFDCLASS_H_ */
