/*
 * analysisgeneraldelegate.h
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#ifndef ANALYSISGENERALDELEGATE_H_
#define ANALYSISGENERALDELEGATE_H_

#include "../common/common.h"
#include "../delegate/analysisdbscandelegate.h"


//All definitions
#ifdef ANALYSISGENERALDELEGATE_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion

UWord8 calculateAngle(UWord16 oldAngle);
void calculateAmplitude(TrackPowerClass &power);

#endif /* ANALYSISGENERALDELEGATE_H_ */
