/*
 * analysisrfdelegate.h
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#ifndef ANALYSISRFDELEGATE_H_
#define ANALYSISRFDELEGATE_H_

#include "../common/common.h"
#include "../vo/pdwsequenceclass.h"
#include "../vo/trackdefaultclass.h"
#include "../utility/constant.h"

void rfUpdateActiveSpotInfo(vector<TrackDefaultClass> &tracks);
void rfCopyResultsFromTrackingClass(TrackDefaultClass &track);
BandCodeEnum getBandFromRf(uint32_t rfValue);
//All definitions
#ifdef ANALYSISRFDELEGATE_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion

#endif /* ANALYSISRFDELEGATE_H_ */
