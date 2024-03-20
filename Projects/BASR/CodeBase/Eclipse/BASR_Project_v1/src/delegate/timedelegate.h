/*
 * timedelegate.h
 *
 *  Created on: Oct 18, 2018
 *      Author: hassan
 */

#ifndef TIMEDELEGATE_H_
#define TIMEDELEGATE_H_

#include "../common/common.h"
#include "../io/systemtime.h"
#include "../model/model.h"
#include "../utility/utility.h"
#include "../utility/constant.h"
#include "../vo/loglowclass.h"

uint64_t timeGetMicrosecDiffTimer1();
void timeSetMsDelay(uint64_t delay);
void timeSetSimulationIteration(uint64_t &simTimeMsIterationActual);
void timeSetToaReference(vector<PdwDefaultClass> &currentPdws,
		uint64_t &timeToaReference);

//functions, includes and defines
#ifdef TIMEDELEGATE_CPP_
//global variables
#else
//externs
#endif
//end of conditional exclusion

#endif /* TIMEDELEGATE_H_ */
