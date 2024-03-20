/*
 * cfduartdelegate.h
 *
 *  Created on: Mar 29, 2021
 *      Author: zumair
 */

#ifndef DELEGATE_CFDUARTDELEGATE_H_
#define DELEGATE_CFDUARTDELEGATE_H_

#include <algorithm>
#include "../common/common.h"
#include "../vo/trenzspidatastructures.h"
#include "../vo/cfdstructuresclass.h"
#include "../vo/trackdefaultclass.h"
#include "../vo/librarycfdclass.h"

#ifdef CFDUARTDELEGATE_CPP_

#else

#endif

void sendAsyncSop(int ackNack, int pb, int lb);
void cfdSendAutoPrograms(CfdLibrary &cfdLib);
void handleCfdCommunication(M1ewCfdProgram &cfdIn);
void handleCfdCommunication(M1ewCfdRequest &cfdIn, Ewm1CfdRequest &cfdOut);
void handleCfdListRequest(M1ewCfdRequest &cfdIn, Ewm1CfdProgram &cfdOut);
void handleCfdStatusRequest();
void cfdUnitStatusCheck(UWord8 code);
void cfdFillThreatData(vector<TrackDefaultClass> &ejTracks,
		vector<TrackDefaultClass> &cdTracks,
		vector<TrackDefaultClass> &kBandTracks,
		vector<TrackDefaultClass> &cwTracks);
void cfdPeriodicStatusCheck();
void cfdPeriodicBitRequest();
void sendRequestOutToTrenz(UWord8 code);

#endif /* DELEGATE_CFDUARTDELEGATE_H_ */
