/*
 * analysistrackingdelegate.h
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#ifndef ANALYSISTRACKLOWDELEGATE_H_
#define ANALYSISTRACKLOWDELEGATE_H_

#include "../common/common.h"
#include "../model/model.h"
#include "../vo/dataanalysisclass.h"
#include "../vo/pdwdefaultclass.h"
#include "../utility/constant.h"
#include "../delegate/analysispowerdelegate.h"
#include "../delegate/analysisangledelegate.h"
#include "../vo/loglowclass.h"
#include "../delegate/analysistoadelegate.h"
#include "../delegate/analysispwdelegate.h"
#include "../delegate/analysislpidelegate.h"
//notes
//out tracking would be a simple score based algorithm that each pulse best
//..fits, we have not yet decided if sinking same pulse in multiple tracks is
//..allowed.
//..We need to determine when to update pri,rf,pw,scan analysis (major analysis
//..vs minor updates we would perform during sinking each pulse)
//Track can accept multiple update mechanisms, including pulses or other tracks
//..calling pri,rf,pw,scan,match analysis would be different for each mechanism
//..furthermore we would divide mechanisms as scan update, string and merge
//end of notes

int16_t trackMeanValueScoring(uint16_t mean, uint16_t queryValue,
		uint16_t minBoundry, int16_t stdDevSlope, int16_t trackingScorePri);
int8_t trackCompareBands(BandCodeEnum pdwBand, BandCodeEnum trackBand);
int8_t trackCompareAngleLowerWindow(uint16_t aoa1, uint16_t aoa2);
int8_t trackCompareAngleWindow(uint16_t aoa1, uint16_t aoa2);
int8_t trackCompareBandWidth(uint16_t rf, uint16_t trackRf);
bool trackCompareGroundIllumination(PdwDefaultClass& pdw,
		TrackPowerClass &trackPow);
int16_t trackPdwPowerScore(PdwDefaultClass &pdw, TrackPowerClass &trackPow);
int16_t trackPdwCompareMaxScoreIndex(PdwDefaultClass &pdw,
		vector<TrackDefaultClass>& tracks);
void trackPdwIntoExistingTracks(vector<TrackDefaultClass>& tracks,
		vector<PdwDefaultClass> &iterationPdws, uint64_t currSystemIteration);
void trackAssociateNewPdwWithTrack(TrackDefaultClass & track,
		PdwDefaultClass & pdw, uint64_t currSystemIteration);
void trackAddPulseToPriPwBuffer(TrackDefaultClass &track, PdwDefaultClass &pdw);
void trackAssociateNewPdwWithTrackV2(TrackDefaultClass & track,
		PdwDefaultClass & pdw, uint64_t currSystemIteration);
void trackProcessOnIterationEnd(vector<TrackDefaultClass>& tracks,
		uint64_t currIteration);

void trackDeleteInactiveTracks(vector<TrackDefaultClass>& tracks);
void trackCountStringableTracks(vector<TrackDefaultClass>& tracks,uint32_t &stringableTrackCount);

void trackStopUpdatesOfSilentTracks(vector<TrackDefaultClass>& tracks,
		uint64_t currSystemIteration);

void trackUpdateParametersBeforeReporting(vector<TrackDefaultClass>& tracks,
		uint64_t currSystemIteration);

void trackUpdateParametersAfterReporting(vector<TrackDefaultClass>& tracks,
		uint64_t currSystemIteration,bool reportSentFlag);
void trackDeleteTracksWithNoPri(vector<TrackDefaultClass> &tracks);
int16_t trackGetIndexOfBestMatch(PdwDefaultClass& pdw,
		vector<TrackDefaultClass>& tracks);

//All definitions
#ifdef ANALYSISTRACKLOWDELEGATE_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion

#endif /* ANALYSISTRACKLOWDELEGATE_H_ */
