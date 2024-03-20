/*
 * logginglowdelegate.h
 *
 *  Created on: Jan 15, 2019
 *      Author: hassan
 */

#ifndef LOGGINGLOWDELEGATE_H_
#define LOGGINGLOWDELEGATE_H_

#include "../common/common.h"
#include "../utility/constant.h"
#include "../vo/pdwdefaultclass.h"
#include "../vo/trackdefaultclass.h"
#include "../vo/loglowclass.h"
#include "../model/model.h"

void logLowIterationInfo(DataAnalysisClass &analysisObj,
		uint64_t lastReportingItr, uint64_t currSysIter);
void logCurrentIterationPulses(vector<PdwDefaultClass> & currentPdws);
void logActiveTracksInfo(vector<TrackDefaultClass> &tracks,
		int32_t inputPdwCount);

//All definitions
#ifdef LOGGINGLOWDELEGATE_CPP_
//All file scope variables

#else
//Global variables using extern
#endif
//End of conditional exclusion

#endif /* LOGGINGLOWDELEGATE_H_ */
