/*
 * threadmainlowdelegate.h
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#ifndef THREADMAINLOWDELEGATE_H_
#define THREADMAINLOWDELEGATE_H_

#include "../common/common.h"
#include "../delegate/analysisgeneraldelegate.h"
#include "../delegate/timedelegate.h"
#include "../delegate/datainputdelegate.h"
#include "../delegate/datascaledelegate.h"
#include "../delegate/simulationlowdelegate.h"
#include "../model/model.h"
#include "../utility/constant.h"
//#include "../vo/pdwdefaultclass.h"
#include "../vo/loglowclass.h"
#include "reportlowdelegate.h"
#include "threaditerationlowdelegate.h"

void * threadMainLowDelegate(void *);
void configureThreadMainConstants();
void incrementThreadMainIterLow(uint64_t &countThreadMainIteration);
void initThreadMainIterCountLow(queue<PdwDefaultClass>& inputPdwList,
		uint64_t &countThreadMainIteration);

//functions, includes and defines
#ifdef THREADMAINLOWDELEGATE_CPP_
//global variables
#else
//externs
#endif
//end of conditional exclusion

#endif /* THREADMAINLOWDELEGATE_H_ */
