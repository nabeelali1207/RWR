/*
 * analysispwdelegate.h
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#ifndef ANALYSISPWDELEGATE_H_
#define ANALYSISPWDELEGATE_H_

#include "../common/common.h"
#include "../utility/constlow.h"
#include "../vo/pdwsequenceclass.h"
#include "../vo/trackdefaultclass.h"
#include "../utility/utility.h"
#include "analysistrackpwlowdelegate.h"
void pwUpdateActiveSpotInfoForAllTracks(vector<TrackDefaultClass> &tracks);
void pwCopyResultsFromTrackingClass(TrackDefaultClass &track);
void pwUpdateActiveSpotInfo(TrackDefaultClass &track);
//All definitions
#ifdef ANALYSISPWDELEGATE_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion

#endif /* ANALYSISPWDELEGATE_H_ */
