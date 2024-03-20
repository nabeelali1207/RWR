/*
 * threaditerationlowdelegate.h
 *
 *  Created on: Oct 19, 2018
 *      Author: hassan
 */

#ifndef THREADITERATIONLOWDELEGATE_H_
#define THREADITERATIONLOWDELEGATE_H_

#include "../common/common.h"
#include "../delegate/analysisclusterdelegate.h"
#include "../delegate/analysisgeneraldelegate.h"
#include "../delegate/analysistoadelegate.h"
#include "../delegate/analysisrfdelegate.h"
#include "../delegate/analysisangledelegate.h"
#include "../delegate/analysispwdelegate.h"
#include "../delegate/analysispowerdelegate.h"
#include "../delegate/datainputdelegate.h"
#include "../delegate/timedelegate.h"
#include "analysistracklowdelegate.h"
#include "logginglowdelegate.h"
#include "../delegate/analysistrackmergelowdelegate.h"

void threadCompleteIterationLow(DataSensorClass &bandObj);
void threadTrainingIterationLow(DataSensorClass &bandObj);


//functions, includes and defines
#ifdef THREADITERATIONLOWDELEGATE_CPP_
//global variables
#else
//externs
#endif
//end of conditional exclusion

#endif /* THREADITERATIONLOWDELEGATE_H_ */
