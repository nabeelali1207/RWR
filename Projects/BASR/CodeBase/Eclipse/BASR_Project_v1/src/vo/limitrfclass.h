/*
 * limitrfclass.h
 *
 *  Created on: Dec 30, 2018
 *      Author: test
 */

#ifndef LIMITRFCLASS_H_
#define LIMITRFCLASS_H_

#include "../common/common.h"
#include "../vo/limitdefaultclass.h"


//review rf types
enum class RfTypeEnum {rf_fixed, rf_switch, rf_jump, rf_slide,	rf_agile,
	rf_sweep, rf_sweepsin, rf_sweeptriangle, rf_complex, rf_uncomputed};

class LimitRfClass: public LimitDefaultClass {
public:
	//add any repetition delta in rf class for sin or triangular patterns
	LimitTypeEnum limitType;
	RfTypeEnum rfType;
	LimitRangeEnum rangeType;
	uint32_t rfLow;
	uint32_t rfAvg;
	uint32_t rfHigh;
	uint32_t rfStdDev;
	uint32_t preSilenceTime;
	uint32_t postSilenceTime;
	uint32_t pulseCount;
	uint32_t frame;
	uint32_t extIntraPulseMode;
	uint32_t extScanMode;
	uint64_t toaFirst;
	uint64_t toaLast;

	LimitRfClass();
	~LimitRfClass();
};


#endif /* LIMITRFCLASS_H_ */
