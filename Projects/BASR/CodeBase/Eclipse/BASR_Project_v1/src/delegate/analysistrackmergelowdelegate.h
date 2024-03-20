/*
 * analysistrackmergelowdelegate.h
 *
 *  Created on: Nov 20, 2018
 *      Author: hassan
 */

#ifndef ANALYSISTRACKMERGELOWDELEGATE_H_
#define ANALYSISTRACKMERGELOWDELEGATE_H_

#include "../common/common.h"
#include "../vo/trackdefaultclass.h"
#include "../vo/datasensorclass.h"
//#include "../delegate/analysistrackpwlowdelegate.h"
#include "../delegate/analysistracklowdelegate.h"

enum class TrackTimeWindowCompatibilty
{
	slight_overlap, overlapping, without_blanking, blanking, out_Of_Window
};

TrackTimeWindowCompatibilty checkTracksStatusInTime(TrackDefaultClass &source,
		TrackDefaultClass &target);

bool doAnglesMatch(TrackDefaultClass &source, TrackDefaultClass &target);
bool doPwMatch(TrackDefaultClass &source, TrackDefaultClass &target);
bool doPriTypesMatch(TrackDefaultClass &source, TrackDefaultClass &target);
bool doPriSpotsMatch(TrackDefaultClass &source, TrackDefaultClass &target);
bool doPowersOfDominantAntennasMatch(TrackDefaultClass &source,
		TrackDefaultClass &target);
int16_t findIfTwoTracksShouldString(vector<TrackDefaultClass> &tracks,
		int16_t sourceIndex, int16_t targetIndex);
int16_t findOtherStringingMatch(vector <TrackDefaultClass> &tracks,
		int16_t targetIndex, int16_t sourceIndex);
void mergeAndUpdateTracks(TrackDefaultClass &source, TrackDefaultClass &target);
void mergeAndUpdateTracksV2(TrackDefaultClass &source, TrackDefaultClass &target);
void sortTracksBasedOnToa(vector <TrackDefaultClass> &tracks);
void stringTracksAllLow(DataSensorClass &bandObj);

//All definitions
#ifdef ANALYSISTRACKMERGELOWDELEGATE_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion

#endif /* ANALYSISTRACKMERGELOWDELEGATE_H_ */
