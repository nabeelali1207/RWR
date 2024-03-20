/*
 * dataenvironmentclass.h
 *
 *  Created on: Jan 1, 2019
 *      Author: hassan
 */

#ifndef DATAENVIRONMENTCLASS_H_
#define DATAENVIRONMENTCLASS_H_

#include "../common/common.h"
#include "../vo/dataenvironmentclass.h"
#include "../vo/limitpriclass.h"
#include "../vo/pdwdefaultclass.h"

//place the state of rf environment here, including pw,pri,rf,intrapulse
//memory pairs
class DataEnvironmentClass {
public:

	BandCodeEnum bandCode;
	uint16_t bearing;
	uint16_t updateCount;
	PriTypeEnum priType;
	uint16_t priCount;
	uint32_t pwLow;
	uint32_t pwHigh;
	uint32_t rfLow;
	uint32_t rfHigh;
	uint32_t priLow;
	uint32_t priHigh;
	uint32_t priSum;
	uint16_t lifeTime;
	uint64_t toaStart;
	uint64_t toaEnd;
	uint16_t rangePositionDisplay;
	uint16_t rangeMeters;

	DataEnvironmentClass();
	virtual ~DataEnvironmentClass();
};

#endif /* DATAENVIRONMENTCLASS_H_ */
