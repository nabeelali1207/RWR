/*
 * reporthighdelegate.h
 *
 *  Created on: Oct 11, 2019
 *      Author: gmd
 */

#ifndef REPORTHIGHDELEGATE_H_
#define REPORTHIGHDELEGATE_H_

#include "../common/common.h"
#include "../model/model.h"
#include "../utility/constant.h"
#include "../vo/dataanalysisclass.h"

#include <iomanip>

bool reportReadFromDSP(DataAnalysisClass & analysisObj,
		uint8_t &dataFinishedFlag);
void reportInsertCircularBuffer(DataAnalysisClass & analysisObj,
		circular_buffer<DataAnalysisClass> & analysisBufferObj,
		bool dspStatus);
void printHighEmitters(DataAnalysisClass &analysisObj, uint64_t currSystemIteration);
void printHighCwEmitters(vector<TrackDefaultClass> &tracks);
#endif /* REPORTHIGHDELEGATE_H_ */
