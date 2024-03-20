/*
 * trackaudioclass.h
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#ifndef TRACKAUDIOCLASS_H_
#define TRACKAUDIOCLASS_H_

#include "../common/common.h"
#include <boost/circular_buffer.hpp>

using namespace boost;

class TrackAudioClass {
public:
	uint8_t flagNewGuyAudioPlayed;

	TrackAudioClass();
	virtual ~TrackAudioClass();
};


#endif /* TRACKAUDIOCLASS_H_ */
