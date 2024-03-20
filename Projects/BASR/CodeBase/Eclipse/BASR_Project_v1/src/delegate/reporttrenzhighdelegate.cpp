/*
 * reporttrenzhighdelegate.cpp
 *
 *  Created on: Feb 12, 2021
 *      Author: saad
 */

#define REPORTTRENZHIGHDELEGATE_CPP_

#include "reporttrenzhighdelegate.h"

//This function is used to copy tracking data from Track structure to
//SPI data structure and then sending this structure to Trenz using SPI
void sendTrackingDataToSpi(UWord8 FstInProgress, UWord8 spiAudioRequest,
		vector<TrackDefaultClass> &kBandTracks,
		vector<TrackDefaultClass> &cwTracks,
		vector<TrackDefaultClass> &ejTracks,
		vector<TrackDefaultClass> &cdTracks, TrenzSpiLink trenzSpiLink,
		LibraryStruct &library, UWord8 &sopMissileLaunch,
		UWord8 &sopThreatFound, UWord8 &sopBeingTracked, UWord8 tpUp)
{
	if (FstInProgress == 1)
	{
		return;
	}
	//An array to sort on priority
	UWord16 priority[256] =
	{ 0 };
	UWord32 count = 0;
	SpiTrackReply spiTrack;
	UWord8 numberOfTracksLocal = 0, numberOfKTracks = 0, numberOfCwTracks = 0,
			numberOfEjTracks = 0, numberOfCdTracks = 0;

	Word32 timeDifference = 0;
	UWord8 i = 0;
	UWord8 trackType = 0;
	uint64_t currentTime;

	sopThreatFound = sopBeingTracked = sopMissileLaunch = 0;

	//This check ensures that Trenz get data not before passing of 250 msecs
	//So it checks if TP is not up and running, i.e. if TP is running each pass
	//is of 220 msecs, then checks if 250 msecs have passed, if no, no data would be
	//sent to Trenz
	if (tpUp != 1)
	{
		currentTime = Utility::getSystemTime();
		timeDifference =
				(currentTime - Model::lastTrackingDataSentTime)
								/ 1000;

		//			printf("time Difference  %lu \n \n",timeDifference);
		if (timeDifference < 250)
		{
			return;
		}
	}

//	printf("STD1\n");
	Model::lastTrackingDataSentTime = Utility::getSystemTime();
	//If the system is running on Hardware, only then we need to send data to Trenz
	if (ConstHigh::simOnActualHwFlag == 1)
	{
		memset(&spiTrack, 0, sizeof(SpiTrackReply));
		//numberOfTracksLocal will be the count of Total tracks across bands to be sent
		numberOfTracksLocal = 0;
//		printf("STD2\n");

		if (FstInProgress == 0)
		{
			trackType = 1;
			// K-Band parameters
			//All priorities are extracted in this loop for K band
			for (i = 0; i < kBandTracks.size(); i++)
			{
				if (kBandTracks[i].general.flagVisibleHigh == 1)
				{
					kBandTracks[i].general.flagTrackSentToTrenz = 0;
					priority[count] = kBandTracks[i].general.priorityDynamic;
					count = count + 1;
				}
			}
//			if (kBandTracks.size() > 0)
//			{
//				printf("\nCount K: %d\n", count);
//			}
			//Priorities are sorted from lower to higher
			radixSortPriorities(priority, count);
			//Priorities are reversed from higher to lower
			reverseArrayOrder(priority, count);
			//A nested loop to copy all K band tracks according to their priority
			while (numberOfTracksLocal != count)
			{
				//total 24 threats can be reported to Trenz in any iteration
				if (numberOfTracksLocal >= 24)
				{
					break;
				}
				for (i = 0; i < kBandTracks.size(); i++)
				{
					if (kBandTracks[i].general.flagTrackSentToTrenz == 0
							&& kBandTracks[i].general.flagVisibleHigh == 1)
					{
						if (kBandTracks[i].general.priorityDynamic
								== priority[numberOfTracksLocal])
						{
							kBandTracks[i].general.flagTrackSentToTrenz = 1;
							//All individual track parameters are copied from Tracks to
							//SPI structure
							copyIndividualParameterOfTracksForSpi(
									kBandTracks[i],
									spiTrack.trackingData[numberOfTracksLocal],
									library, trackType, sopMissileLaunch,
									sopThreatFound, sopBeingTracked);
							numberOfTracksLocal = numberOfTracksLocal + 1;
							numberOfKTracks = numberOfKTracks + 1;
							break;
						}
					}
				}

			}
			trackType = 2;
			// CW-Band parameters
			//Same as above loop but for CW tracks
			if (numberOfTracksLocal < 24)
			{
				memset(priority, 0, sizeof(UWord16) * 256);
				count = 0;
				for (i = 0; i < cwTracks.size(); i++)
				{
					if (cwTracks[i].general.flagVisibleHigh == 1)
					{
						cwTracks[i].general.flagTrackSentToTrenz = 0;
						priority[count] = cwTracks[i].general.priorityDynamic;
						count = count + 1;
					}
				}
//				if (cwTracks.size() > 0)
//				{
//					printf("\nCount CW: %d\n", count);
//				}
				radixSortPriorities(priority, count);
				reverseArrayOrder(priority, count);
				while (numberOfCwTracks != count)
				{
					if (numberOfTracksLocal >= 24)
					{
						break;
					}
					for (i = 0; i < cwTracks.size(); i++)
					{
						if (cwTracks[i].general.flagTrackSentToTrenz == 0
								&& cwTracks[i].general.flagVisibleHigh == 1)
						{
							if (cwTracks[i].general.priorityDynamic
									== priority[numberOfCwTracks])
							{
								cwTracks[i].general.flagTrackSentToTrenz = 1;
								copyIndividualParameterOfTracksForSpi(
										cwTracks[i],
										spiTrack.trackingData[numberOfTracksLocal],
										library, trackType, sopMissileLaunch,
										sopThreatFound, sopBeingTracked);
								numberOfTracksLocal = numberOfTracksLocal + 1;
								numberOfCwTracks = numberOfCwTracks + 1;
								break;
							}
						}
					}

				}
			}
			trackType = 3;
			//EJ-Band parameters
			//Same as above loop but for EJ tracks
			if (numberOfTracksLocal < 24)
			{
				memset(priority, 0, sizeof(UWord16) * 256);
				count = 0;
				for (i = 0; i < ejTracks.size(); i++)
				{
					if (ejTracks[i].general.flagVisibleHigh == 1)
					{
						ejTracks[i].general.flagTrackSentToTrenz = 0;
						priority[count] = ejTracks[i].general.priorityDynamic;
						count = count + 1;
					}
				}
//				if (ejTracks.size() > 0)
//				{
//					printf("\nCount EJ: %d\n", count);
//				}
				radixSortPriorities(priority, count);
				reverseArrayOrder(priority, count);
				while (numberOfEjTracks != count)
				{
					if (numberOfTracksLocal >= 24)
					{
						break;
					}
					for (i = 0; i < ejTracks.size(); i++)
					{
						if (ejTracks[i].general.flagTrackSentToTrenz == 0
								&& ejTracks[i].general.flagVisibleHigh == 1)
						{
							if (ejTracks[i].general.priorityDynamic
									== priority[numberOfEjTracks])
							{
								ejTracks[i].general.flagTrackSentToTrenz = 1;
								copyIndividualParameterOfTracksForSpi(
										ejTracks[i],
										spiTrack.trackingData[numberOfTracksLocal],
										library, trackType, sopMissileLaunch,
										sopThreatFound, sopBeingTracked);
								numberOfTracksLocal = numberOfTracksLocal + 1;
								numberOfEjTracks = numberOfEjTracks + 1;
								break;
							}
						}
					}

				}
			}
			trackType = 4;
			// CD-Band parameters
			//Same as above loop but for CD tracks
			if (numberOfTracksLocal < 24)
			{
				memset(priority, 0, sizeof(UWord16) * 256);
				count = 0;
				for (i = 0; i < cdTracks.size(); i++)
				{
					if (cdTracks[i].general.flagVisibleHigh == 1)
					{
						cdTracks[i].general.flagTrackSentToTrenz = 0;
						priority[count] = cdTracks[i].general.priorityDynamic;
						count = count + 1;
					}
				}
//				if (cdTracks.size() > 0)
//				{
//					printf("\nCount CD: %d\n", count);
//				}
				radixSortPriorities(priority, count);
				reverseArrayOrder(priority, count);
				while (numberOfCdTracks != count)
				{
					if (numberOfTracksLocal >= 24)
					{
						break;
					}
					for (i = 0; i < cdTracks.size(); i++)
					{
						if (cdTracks[i].general.flagTrackSentToTrenz == 0
								&& cdTracks[i].general.flagVisibleHigh == 1)
						{
							if (cdTracks[i].general.priorityDynamic
									== priority[numberOfCdTracks])
							{
								cdTracks[i].general.flagTrackSentToTrenz = 1;
								copyIndividualParameterOfTracksForSpi(
										cdTracks[i],
										spiTrack.trackingData[numberOfTracksLocal],
										library, trackType, sopMissileLaunch,
										sopThreatFound, sopBeingTracked);
								numberOfTracksLocal = numberOfTracksLocal + 1;
								numberOfCdTracks = numberOfCdTracks + 1;
								break;
							}
						}
					}

				}
			}

		}
		//Filling the SPI structures header
		spiTrack.delimeter = 0xdeadbeef;
		spiTrack.messageCode = 9;
		spiTrack.numberOfTracks = numberOfTracksLocal;
		if (spiAudioRequest == 1)
		{
			//Finding what Audio is to be played at this iteration
			spiTrack.audioID = trackingAudioMainLogic(kBandTracks, cwTracks,
					ejTracks, cdTracks);
		}
		//Calculating CRC
		spiTrack.CRC = trenzSpiLink.calculateCRC((UWord8*) &spiTrack,
				sizeof(spiTrack), 8);

		//This function sends data in chunks of 4 4 bytes
		trenzSpiLink.writeTrackingDataToSpi(&spiTrack);
//		printf("STD3\n");
		spiAudioRequest = 0;
	}
}

void audioAyncSopMessage()
{

}
//This function is used to calculated Quadrants according to AoA of tracks
UWord16 findQuadrantToAngleForAudio(UWord16 angleOfArrival)
{
	UWord16 returnQuadrant = 0;
	F64 floatAngle;
	floatAngle = (F64) angleOfArrival;
	returnQuadrant = round(floatAngle / 360 * 8);
	if (returnQuadrant > 7)
	{
		returnQuadrant = 0;
	}
	return returnQuadrant;
}

//This function determines if there is a missile launch in K, CW, EJ and CD
//respectively, if it is so, it returns audio code to play
UWord16 trackingLaunchAudio(vector<TrackDefaultClass> &kBandTracks,
		vector<TrackDefaultClass> &cwTracks,
		vector<TrackDefaultClass> &ejTracks,
		vector<TrackDefaultClass> &cdTracks)
{

	UWord16 returnQuadrant, returnAudioId, i = 0, LAUNCH_AUDIO_CODE = 3;
	//loop for K band
	for (i = 0; i < kBandTracks.size(); i++)
	{
		//checks if missile launch exists
		if (kBandTracks[i].libmatch.active.flagMissileOnlyLibrary == 1)
		{
			//finds quadrant of track
			returnQuadrant = findQuadrantToAngleForAudio(
					kBandTracks[i].angle.curAngle);
			returnAudioId = (10 * LAUNCH_AUDIO_CODE) + returnQuadrant;
			printf("LF: %d \n", returnAudioId);
			return returnAudioId;
		}
	}
	//same as above loop for CW band
	for (i = 0; i < cwTracks.size(); i++)
	{
		if (cwTracks[i].libmatch.active.flagMissileOnlyLibrary == 1)
		{

			returnQuadrant = findQuadrantToAngleForAudio(
					cwTracks[i].angle.curAngle);
			returnAudioId = (10 * LAUNCH_AUDIO_CODE) + returnQuadrant;
			printf("LF: %d \n", returnAudioId);
			return returnAudioId;
		}
	}
	//same as above loop for EJ band
	for (i = 0; i < ejTracks.size(); i++)
	{
		if (ejTracks[i].libmatch.active.flagMissileOnlyLibrary == 1)
		{

			returnQuadrant = findQuadrantToAngleForAudio(
					ejTracks[i].angle.curAngle);
			returnAudioId = (10 * LAUNCH_AUDIO_CODE) + returnQuadrant;
			printf("LF: %d \n", returnAudioId);
			return returnAudioId;
		}
	}
	//same as above loop for CD band
	for (i = 0; i < cdTracks.size(); i++)
	{
		if (cdTracks[i].libmatch.active.flagMissileOnlyLibrary == 1)
		{

			returnQuadrant = findQuadrantToAngleForAudio(
					cdTracks[i].angle.curAngle);
			returnAudioId = (10 * LAUNCH_AUDIO_CODE) + returnQuadrant;
			printf("LF: %d \n", returnAudioId);
			return returnAudioId;
		}
	}
	return 0;
}

//This function determines if there is a new guy in K, CW, EJ and CD
//respectively, if it is so, it returns audio code to play
UWord16 trackingNewGuyAudio(vector<TrackDefaultClass> &kBandTracks,
		vector<TrackDefaultClass> &cwTracks,
		vector<TrackDefaultClass> &ejTracks,
		vector<TrackDefaultClass> &cdTracks)
{
	UWord16 returnQuadrant, returnAudioId, i = 0, NEW_GUY_AUDIO_CODE = 1;

	//now in the loops below, we traverse through K, CW, EJ and CD band
	//respectively and return the first new guy found code
	for (i = 0; i < kBandTracks.size(); i++)
	{
		if (kBandTracks[i].audio.flagNewGuyAudioPlayed == 0)
		{

			returnQuadrant = findQuadrantToAngleForAudio(
					kBandTracks[i].angle.curAngle);
			returnAudioId = (10 * NEW_GUY_AUDIO_CODE) + returnQuadrant;
			kBandTracks[i].audio.flagNewGuyAudioPlayed = 1;
			printf("NGF: %d \n", returnAudioId);
			return returnAudioId;
		}
	}
	for (i = 0; i < cwTracks.size(); i++)
	{
		if (cwTracks[i].audio.flagNewGuyAudioPlayed == 0)
		{

			returnQuadrant = findQuadrantToAngleForAudio(
					cwTracks[i].angle.curAngle);
			returnAudioId = (10 * NEW_GUY_AUDIO_CODE) + returnQuadrant;
			cwTracks[i].audio.flagNewGuyAudioPlayed = 1;
			printf("NGF: %d \n", returnAudioId);
			return returnAudioId;
		}
	}
	for (i = 0; i < ejTracks.size(); i++)
	{
		if (ejTracks[i].audio.flagNewGuyAudioPlayed == 0)
		{

			returnQuadrant = findQuadrantToAngleForAudio(
					ejTracks[i].angle.curAngle);
			returnAudioId = (10 * NEW_GUY_AUDIO_CODE) + returnQuadrant;
			ejTracks[i].audio.flagNewGuyAudioPlayed = 1;
			printf("NGF: %d \n", returnAudioId);
			return returnAudioId;
		}
	}
	for (i = 0; i < cdTracks.size(); i++)
	{
		if (cdTracks[i].audio.flagNewGuyAudioPlayed == 0)
		{

			returnQuadrant = findQuadrantToAngleForAudio(
					cdTracks[i].angle.curAngle);
			returnAudioId = (10 * NEW_GUY_AUDIO_CODE) + returnQuadrant;
			cdTracks[i].audio.flagNewGuyAudioPlayed = 1;
			printf("NGF: %d \n", returnAudioId);
			return returnAudioId;
		}
	}
	return 0;
}

//This function determines if there is a tracking emitter in K, CW, EJ and CD
//respectively, if it is so, it returns audio code to play
UWord16 trackingTrackAudio(vector<TrackDefaultClass> &kBandTracks,
		vector<TrackDefaultClass> &cwTracks,
		vector<TrackDefaultClass> &ejTracks,
		vector<TrackDefaultClass> &cdTracks)
{
	UWord16 returnQuadrant, returnAudioId, i = 0, RADAR_AUDIO_CODE = 2;
	//now in the loops below, we traverse through K, CW, EJ and CD band
	//respectively and return the first tracking emitter found code
	for (i = 0; i < kBandTracks.size(); i++)
	{
		if (kBandTracks[i].power.flagTrackingModeHigh == 1)
		{
			returnQuadrant = findQuadrantToAngleForAudio(
					kBandTracks[i].angle.curAngle);
			returnAudioId = (10 * RADAR_AUDIO_CODE) + returnQuadrant;
			//printf("TF: %d \n", returnAudioId);
			return returnAudioId;
		}
	}
	for (i = 0; i < cwTracks.size(); i++)
	{
		if (cwTracks[i].power.flagTrackingModeHigh == 1)
		{
			returnQuadrant = findQuadrantToAngleForAudio(
					cwTracks[i].angle.curAngle);
			returnAudioId = (10 * RADAR_AUDIO_CODE) + returnQuadrant;
			//printf("TF: %d \n", returnAudioId);
			return returnAudioId;
		}
	}
	for (i = 0; i < ejTracks.size(); i++)
	{
		if (ejTracks[i].power.flagTrackingModeHigh == 1)
		{
			returnQuadrant = findQuadrantToAngleForAudio(
					ejTracks[i].angle.curAngle);
			returnAudioId = (10 * RADAR_AUDIO_CODE) + returnQuadrant;
			//printf("TF: %d \n", returnAudioId);
			return returnAudioId;
		}
	}
	for (i = 0; i < cdTracks.size(); i++)
	{
		if (cdTracks[i].power.flagTrackingModeHigh == 1)
		{
			returnQuadrant = findQuadrantToAngleForAudio(
					cdTracks[i].angle.curAngle);
			returnAudioId = (10 * RADAR_AUDIO_CODE) + returnQuadrant;
			//printf("TF: %d \n", returnAudioId);
			return returnAudioId;
		}
	}
	return 0;
}

//Main loop to find audio to be played.
//As only one audio can be played per iteration, we have to select based
//on priorities
//Prioirties are as follows in decreasing order, Launch, New Guy and Track
//In the priorities above, bands are prioritized in the following order:
//K, CD, EJ, CW
UWord16 trackingAudioMainLogic(vector<TrackDefaultClass> &kBandTracks,
		vector<TrackDefaultClass> &cwTracks,
		vector<TrackDefaultClass> &ejTracks,
		vector<TrackDefaultClass> &cdTracks)
{
	UWord16 newGuyFound = 0, missileLauch = 0, tracking = 0;
	//finds if there is a launch emitter
	missileLauch = trackingLaunchAudio(kBandTracks, cwTracks, ejTracks,
			cdTracks);
	if (missileLauch != 0)
	{
		return missileLauch;
	}
	else
	{
		//finds if there is a new emitter
		newGuyFound = trackingNewGuyAudio(kBandTracks, cwTracks, ejTracks,
				cdTracks);
		if (newGuyFound != 0)
		{
			return newGuyFound;
		}
		else
		{
			//finds if there is a tracking emitter
			tracking = trackingTrackAudio(kBandTracks, cwTracks, ejTracks,
					cdTracks);
			if (tracking != 0)
			{
				//printf("T: %d\n", tracking);
				return tracking;
			}
		}
	}
	return 0;
}

//This function copies all the individual parameters to SPI strucuture to be sent
//trackType = 1 K band 2 CW, 3 for EJ and 4 for CD band
void copyIndividualParameterOfTracksForSpi(TrackDefaultClass &track,
		SpiTrackingData &trackingData, LibraryStruct &library, UWord8 trackType,
		UWord8 &sopMissileLaunch, UWord8 &sopThreatFound,
		UWord8 &sopBeingTracked)
//trackType = 1 K band 2 CW, 3 for EJ and 4 for CD band
{
//printf("sending\n");
	if (track.audio.flagNewGuyAudioPlayed == 0)
	{
		sopThreatFound = 1;
	}
//	sopThreatFound = 1;
	trackingData.angleOfArrival = track.angle.bearingLast;
//printf("Bearing: %d\n", trackingData->angleOfArrival);
	if (track.general.priorityDynamic > 250)
	{
		trackingData.maxPriorityThreat = 1;
	}
	else
	{
		trackingData.maxPriorityThreat = 0;
	}
//trackingData->priority = pTrack->Priority;
//printf("PR%d\n",trackingData->priority);
	trackingData.priority = track.general.priorityDynamic;
//printf("PR %d\n",trackingData->priority);

	if ((track.libmatch.active.flagKnownLibrary == 1)
			&& (track.libmatch.active.highestMatchListCount > 0))
	{
		switch (library.Emitters[track.libmatch.active.highestMatchList[0].ID].attribute)
		{
		case 1:	//friend
			trackingData.attribute = 3;
			break;
		case 2:	//foe
			trackingData.attribute = 1;
			break;
		case 3:	//ours
			trackingData.attribute = 2;
			break;
		default:
			trackingData.attribute = 0;
			break;
		}
		// trackingData.attribute =
		// 		library.Emitters[track.libmatch.active.highestMatchList[0].ID].attribute;
		trackingData.radarCategory =
				track.libmatch.active.highestMatchList[0].emitterBatchId;

		// trackingData.platform =
		// 		library.Emitters[track.libmatch.active.highestMatchList[0].ID].platform;
		switch (library.Emitters[track.libmatch.active.highestMatchList[0].ID].platform)
		{
		case 1:		//ground
			trackingData.platform = 2;
			break;
		case 2:		//air
			trackingData.platform = 1;
			break;
		case 3:		//ship
			trackingData.platform = 3;
			break;
		case 4:		//single cannon
			trackingData.platform = 4;
			break;
		case 5:		//double cannon
			trackingData.platform = 5;
			break;
		default:
			trackingData.platform = 0;
			break;
		}
		trackingData.radarID =
				library.Emitters[track.libmatch.active.highestMatchList[0].ID].radarName;
		if (track.libmatch.active.flagMissileOnlyLibrary == 1)
		{
			trackingData.status = 3;
			sopMissileLaunch = 1;
			ConstHigh::sopAsyncAudioFlag = 1;

		}
		else if (track.power.flagTrackingModeHigh == 1)
		{
			trackingData.status = 2;
			sopBeingTracked = 1;
			ConstHigh::sopAsyncAudioFlag = 1;
		}
		else if (track.power.flagTrackingModeHigh == 0)
		{
			trackingData.status = 1;
		}
		trackingData.threatPresent = 1;
		trackingData.unknownTarget = 0;
	}
	else
	{
		trackingData.radarCategory = 0;
		trackingData.attribute = 0;
		trackingData.platform = 0;
		trackingData.radarID = 0;
		trackingData.status = 1;
		trackingData.threatPresent = 0;
		trackingData.unknownTarget = 1;
	}

	if (track.power.flagTrackingModeHigh == 1)
	{
		trackingData.status = 2;
		sopBeingTracked = 1;
		ConstHigh::sopAsyncAudioFlag = 1;
	}
//	if (trackType == 2)
//	{
//		trackingData.status = 2;
//		sopBeingTracked = 1;
//		ConstHigh::sopAsyncAudioFlag = 1;
//	}

	if (trackType == 4)
	{
		trackingData.angleOfArrival = 360;
	}
	/*if (distanceType == 1) {
	 trackingData->distance = ceil(
	 ((15 - pTrack->dynamicPriority) * 100) / 15);
	 } else {*/
	if (track.range.rangeMeters / 1852 <= 255)
	{
		trackingData.distance = track.range.rangeMeters / 1852;
		trackingData.distance = max((UWord8) 10, trackingData.distance);
	}
	else
	{
		trackingData.distance = 255;
	}
	if (track.power.flagTrackingModeHigh == 1)
	{
		trackingData.status = 2;
		sopBeingTracked = 1;
	}

//	printf("AOA: %d, RF: %d,", track.angle.bearingLast,
//			track.rf.accumulateRf.front().rfAvg);
//}
//    printf("S: B-I %d, E-I %d\n",trackingData->radarID,trackingData->radarCategory);
//	printf(" Dist: %d\n", trackingData.distance);
}

// The main function to that sorts arr[] of size n using
// Radix Sort
void radixSortPriorities(UWord16 arr[], UWord32 n)
{
// Find the maximum number to know number of digits
	Word32 m = radixSortGetMax(arr, n);
	Word32 exp;
// Do counting sort for every digit. Note that instead
// of passing digit number, exp is passed. exp is 10^i
// where i is current digit number
	for (exp = 1; m / exp > 0; exp *= 10)
		countSort(arr, n, exp);
}

Word32 radixSortGetMax(UWord16 arr[], UWord32 n)
{
	Word32 mx = arr[0];
	UWord32 i;
	for (i = 1; i < n; i++)
		if (arr[i] > mx)
			mx = arr[i];
	return mx;
}

// A function to do counting sort of arr[] according to
// the digit represented by exp.
void countSort(UWord16 arr[], UWord32 n, UWord32 exp)
{
	Word32 output[n]; // output array
	Word32 i;
	Word32 count[10] =
	{ 0 };

// Store count of occurrences in count[]
	for (i = 0; i < n; i++)
	{
		count[(arr[i] / exp) % 10]++;
	}

// Change count[i] so that count[i] now contains actual
//  position of this digit in output[]
	for (i = 1; i < 10; i++)
	{
		count[i] += count[i - 1];
	}

// Build the output array
	for (i = n - 1; i >= 0; i--)
	{
		output[count[(arr[i] / exp) % 10] - 1] = arr[i];
		count[(arr[i] / exp) % 10]--;
	}

// Copy the output array to arr[], so that arr[] now
// contains sorted numbers according to current digit
	for (i = 0; i < n; i++)
	{
		arr[i] = output[i];
	}
}

//Reverses the array
void reverseArrayOrder(UWord16 array[], UWord32 count)
{
	UWord32 i = 0;
	UWord16 *reverseArray = (UWord16*) malloc(count * sizeof(UWord16));
	for (i = 0; i < count; i++)
	{
		reverseArray[i] = array[count - i - 1];
	}
	memcpy(array, reverseArray, count * sizeof(UWord16));
	free(reverseArray);
//return reverseArray;
}

