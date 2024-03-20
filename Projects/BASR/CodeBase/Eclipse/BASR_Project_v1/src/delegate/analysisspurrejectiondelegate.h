/*
 * analysisspurrejectiondelegate.h
 *
 *  Created on: Mar 4, 2021
 *      Author: gmd
 */

#ifndef ANALYSISSPURREJECTIONDELEGATE_H_
#define ANALYSISSPURREJECTIONDELEGATE_H_
#include "../delegate/analysistracklowdelegate.h"
#include <iomanip>

bool detectSpurs(TrackDefaultClass &first, TrackDefaultClass &second);
void analysisRfSpurRejection(vector<TrackDefaultClass> &tracks);

#ifdef ANALYSISSPURREJECTIONDELEGATE_CPP_
uint16_t spurTable[][4] =
{
{ 3700, 1200, 7400, 1550 },
{ 3700, 1200, 7400, 1550 },

};
#else
extern uint16_t spurTable[][4];
#endif
#endif /* ANALYSISSPURREJECTIONDELEGATE_H_ */
