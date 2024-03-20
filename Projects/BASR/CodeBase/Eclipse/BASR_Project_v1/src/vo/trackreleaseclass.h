/*
 * trackreleaseclass.h
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#ifndef TRACKRELEASECLASS_H_
#define TRACKRELEASECLASS_H_

#include "../common/common.h"
#include <boost/circular_buffer.hpp>

using namespace boost;

class TrackReleaseClass {
public:
	//concept of this class needs review
	//pri, rf, pw, intrapulse lists
	//library match object and time for each of the above where relevant
	//for now visibility and recording might be managed from release class
	//releases tracks cannot be hidden again until at least expiry

	uint32_t timeIterCountReleaseSet;
	uint32_t timeIterLimitReleaseSet;

	TrackReleaseClass();
	~TrackReleaseClass();
};


#endif /* TRACKRELEASECLASS_H_ */
