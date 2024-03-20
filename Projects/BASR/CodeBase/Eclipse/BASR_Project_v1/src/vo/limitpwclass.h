/*
 * limitpwclass.h
 *
 *  Created on: Dec 30, 2018
 *      Author: test
 */

#ifndef LIMITPWCLASS_H_
#define LIMITPWCLASS_H_

#include "../common/common.h"
#include "../vo/limitdefaultclass.h"


enum class PwTypeEnum {pw_fixed, pw_agile, pw_uncomputed};

class LimitPwClass: public LimitDefaultClass {
public:
	LimitTypeEnum limitType;
	PwTypeEnum pwType;
	LimitRangeEnum rangeType;
	uint32_t pwLow;
	uint32_t pwAvg;
	uint32_t pwHigh;
	uint32_t pwStdDev;
	uint32_t preSilenceTime;
	uint32_t postSilenceTime;
	uint32_t pulseCount;
	uint32_t frame;
	uint32_t extIntraPulseMode;
	uint32_t extScanType;
	uint64_t toaFirst;
	uint64_t toaLast;

	LimitPwClass();
	~LimitPwClass();
};


#endif /* LIMITPWCLASS_H_ */
