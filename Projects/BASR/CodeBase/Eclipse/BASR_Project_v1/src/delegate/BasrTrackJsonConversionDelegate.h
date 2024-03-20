/*
 * BasrTrackJsonConversionDelegate.h
 *
 *  Created on: Jan 23, 2023
 *      Author: gmd
 */

#ifndef BASRTRACKJSONCONVERSIONDELEGATE_H_
#define BASRTRACKJSONCONVERSIONDELEGATE_H_
#include "../model/model.h"

void jsonReportBasrLowTracks();
void jsonReportBasrHighTracks();

void addBasrLowTracksToString(string &t,vector<TrackDefaultClass> &tracks,uint32_t band);
void addBasrLowTrackToString(string &t,TrackDefaultClass &track);

void addBasrHighTracksToString(string &t,vector<TrackDefaultClass> &tracks,uint32_t band);
void addBasrHighTrackToString(string &t,TrackDefaultClass &track);
void addBasrHighCWTrackToString(string& t, TrackDefaultClass& track);

#endif /* BASRTRACKJSONCONVERSIONDELEGATE_H_ */
