/*
 * limitpriclass.h
 *
 *  Created on: Dec 30, 2018
 *      Author: test
 */

#ifndef LIMITPRICLASS_H_
#define LIMITPRICLASS_H_

#include "../common/common.h"
#include "../vo/limitdefaultclass.h"

enum class PriTypeEnum {pri_uncomputed,pri_fixed, pri_dwellswitch, pri_stagger,
	pri_sliding, pri_jitter, pri_complex, pri_difftoa};

class LimitPriClass: public LimitDefaultClass {
public:
	LimitTypeEnum limitType;
	PriTypeEnum priType;
	LimitRangeEnum rangeType;
	uint32_t priLow;
	uint32_t priMean;
	uint32_t priHigh;
	uint32_t pwLow;
	uint32_t pwHigh;
	uint32_t preSilenceTime;
	uint32_t postSilenceTime;
	uint32_t pulseCount;
	uint32_t frame;
	uint32_t extIntraPulseMode;
	uint32_t extScanType;
	uint64_t toaFirst;
	uint64_t toaLast;

	LimitPriClass();
	~LimitPriClass();
};


#endif /* LIMITPRICLASS_H_ */
