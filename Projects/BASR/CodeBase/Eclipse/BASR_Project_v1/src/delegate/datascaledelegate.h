/*
 * datascaledelegate.h
 *
 *  Created on: Oct 28, 2018
 *      Author: test
 */

#ifndef DATASCALEDELEGATE_H_
#define DATASCALEDELEGATE_H_

#include "../common/common.h"
#include "../vo/datasensorclass.h"
#include "../delegate/analysisangledelegate.h"
#include "../utility/constant.h"
#include "../vo/datasimulationlowclass.h"

void scaleSetSystemConstants();
void scalePreprocessPdwThreadMainData(list<PdwDefaultClass> &currentPdws);
bool scaleApplyPowerThreshold(PdwDefaultClass &currentPdw,
		const uint16_t &minSinglePower, const uint16_t &minCombinedPower);
void scaleSinglePdwSimulationData(PdwDefaultClass & pdw);
void scaleSinglePdwLiveData(PdwDefaultClass &pdw);
void scaleSinglePdw(PdwDefaultClass & pdw, uint16_t adcScale, uint16_t toaScale,
		uint16_t pwScale);
void scalePdwSimulationData(queue<PdwDefaultClass>& inputPdwList);
//functions, includes and defines
#ifdef DATASCALEDELEGATE_CPP_
//global variables
#else
//externs
#endif
//end of conditional exclusion

#endif /* DATASCALEDELEGATE_H_ */
