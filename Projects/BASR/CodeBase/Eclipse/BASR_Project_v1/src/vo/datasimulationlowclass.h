/*
 * datasimulationlowclass.h
 *
 *  Created on: Oct 18, 2018
 *      Author: hassan
 */

#ifndef DATASIMLUATIONLOWCLASS_H_
#define DATASIMLUATIONLOWCLASS_H_

#include "../common/common.h"
#include "../vo/pdwdefaultclass.h"
#include "../utility/utility.h"
#include "../vo/loglowclass.h"
#include <queue>
using std::queue;
class DataSimulationLowClass {
public:
	queue <PdwDefaultClass> bandAllPdwBuffer;
	uint64_t currSystemTime;
	uint64_t lastSystemTime;
	uint64_t pdwIterationStartTime;
	uint64_t pdwIterationEndTime;
	int64_t systemTimeToToaTimeOffset;

	uint64_t iterationTimeFromFile_ptr;
	uint64_t iterationTimeFromFile_ptrMaxSize;
	ifstream iterationTimeFromFile_fileStream;
	LogIterationInfoClass iterationTimeFromFile_lastReadObject;
	uint8_t callReportingFlag;
	DataSimulationLowClass();
	virtual ~DataSimulationLowClass();
};

#endif /* DATASIMULATIONLOWCLASS_H_ */
