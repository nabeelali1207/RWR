/*
 * simulationlowdelegate.h
 *
 *  Created on: Nov 3, 2018
 *      Author: test
 */

#ifndef SIMULATIONLOWDELEGATE_H_
#define SIMULATIONLOWDELEGATE_H_

#include "../common/common.h"
#include "../model/model.h"
#include "../utility/constant.h"

void simLowInitializeParameters(DataSimulationLowClass &simLowData);
void simLowUpdateSystemToToaOffset(DataSimulationLowClass &simLowData);
uint64_t simLowConvertSystemTimeToToaTime(uint64_t systemTime,
		DataSimulationLowClass &simLowData);
uint64_t simLowGetIterationToaStartTime(DataSimulationLowClass &simLowData);
uint64_t simLowGetIterationToaEndTime(DataSimulationLowClass &simLowData);
void simLowUpdateToaStartTime(uint64_t startToa,
		DataSimulationLowClass &simLowData);
void simLowUpdateIterationTime(DataSimulationLowClass &simLowData);

void simLowSetConstants();
void simLowSetVariables();

//All definitions
#ifdef SIMULATIONLOWDELEGATE_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion



#endif /* SIMULATIONLOWDELEGATE_H_ */
