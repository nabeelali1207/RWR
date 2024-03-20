/*
 * threadmainhighdelegate.h
 *
 *  Created on: Sep 26, 2019
 *      Author: hassan
 */

#ifndef THREADMAINHIGHDELEGATE_H_
#define THREADMAINHIGHDELEGATE_H_

#include "../common/common.h"
#include "../model/model.h"
#include "../delegate/threaditerationhighdelegate.h"
#include "../delegate/logginghighdelegate.h"
#include "../io/systemtime.h"
#include "../delegate/cfduartdelegate.h"
#include "../delegate/reporthighdelegate.h"
#include "../delegate/hardwaredelegate.h"
#include "../delegate/reportcwhighdelegate.h"
#include "../delegate/reporttrenzhighdelegate.h"
#include "../delegate/threatprocessorselftestdelegate.h"
#include "../delegate/trenzspidelegate.h"
#include "../delegate/selftesthighdelegate.h"

void* threadMainHighDelegate(void*);
void* threadMainHighDelegateSimulator(void*);
void temporaryProcessing();
void incrementThreadMainIterHigh(uint64_t &countThreadMainIteration);
void initThreadMainIterCountHigh(uint64_t &countThreadMainIteration);
void redirectStandardOutput();
void restoreStandardOutput();
//functions, includes and defines
#ifdef THREADMAINHIGHDELEGATE_CPP_
//global variables

#else
//externs

#endif
//end of conditional exclusion

#endif /* THREADMAINHIGHDELEGATE_H_ */
