/*
 * pdwdifferenceclass.h
 *
 *  Created on: Feb 19, 2019
 *      Author: hassan
 */

#ifndef PDWDIFFERENCECLASS_H_
#define PDWDIFFERENCECLASS_H_

#include "../common/common.h"

class PdwDifferenceClass {
public:
	uint16_t pdwIndex;
	int64_t diffToa;
	int64_t diffToaCompare;

	//uint64_t timeOfArrivalStart;
	//uint64_t timeOfArrivalEnd;
	//uint64_t timeOfArrivalStartRaw;
	//uint64_t timeOfArrivalEndRaw;

	static bool cmpDiffToa(const PdwDifferenceClass & a, const PdwDifferenceClass & b);
	PdwDifferenceClass();
	virtual ~PdwDifferenceClass();
};

#endif /* PDWDIFFERENCECLASS_H_ */
