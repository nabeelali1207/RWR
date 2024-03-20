/*
 * manageinputdelegate.h
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#ifndef DATAINPUTDELEGATE_H_
#define DATAINPUTDELEGATE_H_

#include "../common/common.h"
#include "../delegate/timedelegate.h"
#include "../io/fileinputmatlab.h"
#include "../io/systemstorage.h"
#include "../model/model.h"
#include "../utility/constant.h"
#include "../vo/dataanalysisclass.h"
#include "../vo/pdwdefaultclass.h"
#include "../vo/loglowclass.h"
#include "../delegate/datascaledelegate.h"

void inputReadPdwSimulationFile(queue<PdwDefaultClass>& inputPdwList);
void readPdwAllMatFile(const char * matInFileName, const char * csvOutFileName,
		queue<PdwDefaultClass>& inputPdwList, uint8_t writeOutputFlag);
void inputInsertPdwBandBuffer(PdwDefaultClass &pdwObj,
		DataAnalysisClass &analysisObj);

void inputGetPdwSimIteration(uint64_t startTime, uint64_t endTime,
		uint64_t mainThreadIterCount, uint8_t &dataFinishedFlag,
		queue<PdwDefaultClass>& bandInputPdwBuffer,
		DataAnalysisClass &analysisObj);
void inputGetPdwLiveIteration(uint8_t &dataFinishedFlag,
		LivePdwReceiver & pdwReceiver, DataAnalysisClass &analysisObj);
void inputGetPdwLiveIteration(uint8_t &dataFinishedFlag,
		LivePdwReceiver & pdwReceiver,PlaybackPdwProvider& playbackData, DataAnalysisClass &analysisObj);

void inputClearIterationBandPdws(vector<PdwDefaultClass> &currentPdws);
void inputExpireIterationBandPdws(vector<PdwDefaultClass> &currentPdws,
		uint64_t &timeToaReference);

void inputCleanPdwSimData(queue<PdwDefaultClass>& inputPdwList);
void inputLimitPdwSimData(queue<PdwDefaultClass>& inputPdwList);

void inputMountHardwareStorage();

//functions, includes and defines
#ifdef DATAINPUTDELEGATE_CPP_
//global variables
#else
//externs
#endif
//end of conditional exclusion

#endif /* DATAINPUTDELEGATE_H_ */
