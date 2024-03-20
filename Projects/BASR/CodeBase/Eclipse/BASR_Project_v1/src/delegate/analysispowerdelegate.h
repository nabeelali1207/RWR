/*
 * analysispower.h
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#ifndef ANALYSISPOWERDELEGATE_H_
#define ANALYSISPOWERDELEGATE_H_

#include "../common/common.h"
#include "../vo/pdwsequenceclass.h"
#include "../vo/trackdefaultclass.h"
#include "../utility/utility.h"
#include "../utility/constant.h"

void powerDetermineWeights(uint32_t pa, uint32_t pb, uint32_t pc, uint32_t pd,
		float* destination);
void powerDetermineWeights_V2(TrackDefaultClass & track);
vector<PdwDefaultClass> powerGetPdwsForAnalysis(TrackDefaultClass & track);
void powerUpdateTrackPowersOnPulseAssociation(TrackDefaultClass &track,PdwDefaultClass & newPdw);
void powerUpdateGroundIlluminationFlags(TrackDefaultClass &track);
void powerUpdateDominantAntennas(TrackDefaultClass &track);
void resetDominantAntennaCounts(TrackDefaultClass & track);
void powerUpdatePowerFlagsAfterReporting(TrackDefaultClass &track);
void powerUpdatePowerFlagsBeforeReporting(TrackDefaultClass &track);
void powerUpdateMaximumPowers(TrackDefaultClass &track);
void powerScanAnalysis(TrackDefaultClass &track);

void powerUpdateLastStateOfPowerVariables(TrackDefaultClass & track);
void powerUpdateFirstStateOfPowerVariables(TrackDefaultClass & track);

void powerAnalysisExecute(TrackDefaultClass & track);

//All definitions
#ifdef ANALYSISPOWERDELEGATE_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion

#endif /* ANALYSISPOWERDELEGATE_H_ */
