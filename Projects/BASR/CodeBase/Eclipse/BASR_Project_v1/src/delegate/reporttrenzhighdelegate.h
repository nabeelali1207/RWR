/*
 * reporttrenzhighdelegate.h
 *
 *  Created on: Feb 12, 2021
 *      Author: saad
 */

#ifndef REPORTTRENZHIGHDELEGATE_H_
#define REPORTTRENZHIGHDELEGATE_H_

#include "../vo/trenzspidatastructures.h"
#include "../utility/consthigh.h"
#include "../vo/trackdefaultclass.h"
#include "../model/model.h"

/*
 * Function Name:   sendTrackingDataToSpi
 * Purpose:         This function is used to copy tracking data from Track structure to
 * 					SPI data structure and then sending this structure to Trenz using SPI
 * Inputs:          All the flags required from SPI data
 * 					K, CD, EJ and CW tracks
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void sendTrackingDataToSpi(UWord8 FstInProgress, UWord8 spiAudioRequest,
		vector<TrackDefaultClass> &kBandTracks,
		vector<TrackDefaultClass> &cwTracks,
		vector<TrackDefaultClass> &ejTracks,
		vector<TrackDefaultClass> &cdTracks, TrenzSpiLink trenzSpiLink,
		LibraryStruct &library, UWord8 &sopMissileLaunch, UWord8 &sopThreatFound,
		UWord8 &sopBeingTracked, UWord8 tpUp);

/*
 * Function Name:   radixSortPriorities
 * Purpose:			The main function to that sorts arr[] of size n using
 * Inputs:          Array
 * 					Size of array
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void radixSortPriorities(UWord16 arr[], UWord32 n);

/*
 * Function Name:   radixSortGetMax
 * Purpose:			Returns max of an array
 * Inputs:          Array
 * 					Size of array
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
Word32 radixSortGetMax(UWord16 arr[], UWord32 n);

/*
 * Function Name:   radixSortGetMax
 * Purpose:			A function to do counting sort of arr[] according to
 *					the digit represented by exp.
 * Inputs:          Array
 * 					Exp at which it is to be sorted
 * 					Size of array
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void countSort(UWord16 arr[], UWord32 n, UWord32 exp);

/*
 * Function Name:   reverseArrayOrder
 * Purpose:			Reverses the array
 * Inputs:          Array
 * 					Size of array
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void reverseArrayOrder(UWord16 array[], UWord32 count);

/*
 * Function Name:   copyIndividualParameterOfTracksForSpi
 * Purpose:         This function is used to copy tracking data from Track structure to
 * 					SPI data structure
 * Inputs:          All the flags required from SPI data
 * 					K, CD, EJ and CW tracks
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void copyIndividualParameterOfTracksForSpi(TrackDefaultClass &track,
		SpiTrackingData &trackingData, LibraryStruct &library, UWord8 trackType,
		UWord8 &sopMissileLaunch, UWord8 &sopThreatFound,
		UWord8 &sopBeingTracked);

/*
 * Function Name:   findQuadrantToAngleForAudio
 * Purpose:			It returns quadrant wrt AoA
 * Inputs:          Angle of arrival of emitter
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
UWord16 findQuadrantToAngleForAudio(UWord16 angleOfArrival);

/*
 * Function Name:   trackingLaunchAudio
 * Purpose:			This function determines if there is a missile launch in K, CW, EJ and CD
 * 					respectively, if it is so, it returns audio code to play
 * Inputs:          K, CD, EJ and CW tracks
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
UWord16 trackingLaunchAudio(vector<TrackDefaultClass> &kBandTracks,
		vector<TrackDefaultClass> &cwTracks,
		vector<TrackDefaultClass> &ejTracks,
		vector<TrackDefaultClass> &cdTracks);

/*
 * Function Name:   trackingNewGuyAudio
 * Purpose:			This function determines if there is a new guy in K, CW, EJ and CD
 * 					respectively, if it is so, it returns audio code to play
 * Inputs:          K, CD, EJ and CW tracks
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
UWord16 trackingNewGuyAudio(vector<TrackDefaultClass> &kBandTracks,
		vector<TrackDefaultClass> &cwTracks,
		vector<TrackDefaultClass> &ejTracks,
		vector<TrackDefaultClass> &cdTracks);

/*
 * Function Name:   trackingTrackAudio
 * Purpose:			This function determines if there is a tracking emitter in K, CW, EJ and CD
 * 					respectively, if it is so, it returns audio code to play
 * Inputs:          K, CD, EJ and CW tracks
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
UWord16 trackingTrackAudio(vector<TrackDefaultClass> &kBandTracks,
		vector<TrackDefaultClass> &cwTracks,
		vector<TrackDefaultClass> &ejTracks,
		vector<TrackDefaultClass> &cdTracks);

/*
 * Function Name:   trackingAudioMainLogic
 * Purpose:			Main loop to find audio to be played.
 * 					As only one audio can be played per iteration, we have to select based
 * 					on priorities.
 * 					Prioirties are as follows in decreasing order, Launch, New Guy and Track
 * 					In the priorities above, bands are prioritized in the following order:
 * 					K, CD, EJ, CW
 * Inputs:          K, CD, EJ and CW tracks
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
UWord16 trackingAudioMainLogic(vector<TrackDefaultClass> &kBandTracks,
		vector<TrackDefaultClass> &cwTracks,
		vector<TrackDefaultClass> &ejTracks,
		vector<TrackDefaultClass> &cdTracks);

#endif /* REPORTTRENZHIGHDELEGATE_H_ */
