/*
 * angledelegate.h
 *
 *  Created on: Oct 29, 2018
 *      Author: hassan
 */

#ifndef ANALYSISANGLEDELEGATE_H_
#define ANALYSISANGLEDELEGATE_H_

#include "../common/common.h"
#include "../vo/angletableoneband.h"
#include "../vo/angletabletwoband.h"
#include "../vo/angletablefourband.h"
#include "../vo/pdwdefaultclass.h"
#include "../vo/trackdefaultclass.h"
#include "../utility/constant.h"

vector<PdwDefaultClass> doaGetPdwsForAnalysis(TrackDefaultClass &track);
void doaAnalysisExecute(TrackDefaultClass &track);
uint16_t doaCalculateAngle(uint16_t px, uint16_t py, uint8_t quadrantCode,
		uint8_t bandCode, uint8_t bandDivision);
uint16_t doaCalculateAngle_Low(uint16_t pa, uint16_t pb, uint16_t pc,
		uint16_t pd, uint8_t quadrantCode, uint8_t bandCode,
		uint8_t bandDivision);
uint16_t doaRoundAngleDifference(uint16_t angleInput);
uint8_t doaFlagPxPyCompatibleCheck(uint16_t pxOne, uint16_t pxTwo,
		uint16_t pyOne, uint16_t pyTwo);
uint8_t doaFlagQuadrantAdjacencyCheck(uint8_t qCodeOne, uint8_t qCodeTwo);

//functions, includes and defines
#ifdef ANALYSISANGLEDELEGATE_CPP_
//global variables
#else
//externs
#endif

#endif /* ANALYSISANGLEDELEGATE_H_ */
