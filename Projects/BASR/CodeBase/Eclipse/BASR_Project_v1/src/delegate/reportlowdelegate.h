/*
 * reportlowdelegate.h
 *
 *  Created on: Oct 11, 2019
 *      Author: gmd
 */

#ifndef REPORTLOWDELEGATE_H_
#define REPORTLOWDELEGATE_H_
#include <iomanip>
#include "../common/common.h"
#include "../model/model.h"
#include "../utility/constant.h"
#include "../vo/dataanalysisclass.h"
#include "../delegate/analysistracklowdelegate.h"
#include "../delegate/hardwaredelegate.h"
#include "../delegate/analysisspurrejectiondelegate.h"
#include "../delegate/ethernettrackconversiondelegate.h"
#include "BasrTrackJsonConversionDelegate.h"

void reportAnalysisObjectToCpu(DataAnalysisClass & analysisObj,DataSimulationLowClass &simLowData, uint64_t currSystemIteration);

#endif /* REPORTLOWDELEGATE_H_ */
