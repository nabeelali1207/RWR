/*
 * trackintrapulseclass.h
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#ifndef TRACKINTRAPULSECLASS_H_
#define TRACKINTRAPULSECLASS_H_

#include "../common/common.h"
#include <boost/circular_buffer.hpp>
#include "../io/serialbuffer.h"

using namespace boost;

enum class IntraPulseType :uint8_t
{
	UMOP = 0,
	FMOP,
	PMOP,
	AMOP,
};

class TrackIntraPulseClass {
public:
	//we may need further intra pulse containers if required similar to limit
	//how else would be create a circular buffer here?
	uint64_t timeToaIntraAnl;
	bool amopPresent;
	bool pmopPresent;
	bool fmopPresent;
	bool umopPresent;
	IntraPulseType trackLpiType;
	uint16_t pulseCountSinceLastLpiAnalysis;
	TrackIntraPulseClass();
	~TrackIntraPulseClass();
	void serialize(SerialBufferClass & buffer);
};


#endif /* TRACKINTRAPULSECLASS_H_ */
