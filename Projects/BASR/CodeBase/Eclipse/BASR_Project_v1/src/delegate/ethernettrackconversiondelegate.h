/*
 * ethernettrackconversiondelegate.h
 *
 *  Created on: Jun 28, 2021
 *      Author: irfan
 */

#ifndef SRC_DELEGATE_ETHERNETTRACKCONVERSIONDELEGATE_H_
#define SRC_DELEGATE_ETHERNETTRACKCONVERSIONDELEGATE_H_

#include "../common/common.h"
#include "../model/model.h"
#include "../utility/constlow.h"
#include "../utility/sha256.h"
#include "../delegate/analysistracklowdelegate.h"
#include "../delegate/datascaledelegate.h"

void ethernetTracksConversion(vector<TrackDefaultClass> &tracks,vector<TrackDefaultClass> &tracksCw,
		vector<TrackDefaultClass> &tracksK,vector<TrackDefaultClass> &tracksCd,Ethernetlinktp &ethernetTrackObj);
void ethernetCwTracksConversion(vector<TrackDefaultClass> &tracksCw,Ethernetlinktp &ethernetTrackObj,int trackNum);
void ethernetNormalTracksConversion(vector<TrackDefaultClass> &tracks,Ethernetlinktp &ethernetTrackObj,int trackNum);
#endif /* SRC_DELEGATE_ETHERNETTRACKCONVERSIONDELEGATE_H_ */
