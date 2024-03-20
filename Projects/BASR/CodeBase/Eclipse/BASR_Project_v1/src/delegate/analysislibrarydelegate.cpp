/*
 * analysislibrarydelegate.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#define ANALYSISLIBRARYDELEGATE_CPP_

#include "../delegate/analysislibrarydelegate.h"

void libraryAssignSymbolEmitterAll(vector<TrackDefaultClass> &tracks,
		LibraryStruct &library)
{
	uint8_t eUkCounter = 0, gUkCounter = 0, iUkCounter = 0, jUkCounter = 0;
	uint16_t i = 0;

	for (i = 0; i < tracks.size(); i++)
	{
		if (tracks[i].libmatch.active.flagKnownLibrary == 1)
		{
			tracks[i].libmatch.active.highestMatchList[0].symbolOne =
					library.Emitters[tracks[i].libmatch.active.highestMatchList[0].ID].SymbolCode1;
			tracks[i].libmatch.active.highestMatchList[0].symbolTwo =
					library.Emitters[tracks[i].libmatch.active.highestMatchList[0].ID].SymbolCode2;
			if (tracks[i].libmatch.active.highestMatchListCount > 1)
			{
				tracks[i].libmatch.active.highestMatchList[1].symbolOne =
						library.Emitters[tracks[i].libmatch.active.highestMatchList[1].ID].SymbolCode1;
				tracks[i].libmatch.active.highestMatchList[1].symbolTwo =
						library.Emitters[tracks[i].libmatch.active.highestMatchList[1].ID].SymbolCode2;
			}
			if (tracks[i].libmatch.active.highestMatchListCount > 2)
			{
				tracks[i].libmatch.active.highestMatchList[2].symbolOne =
						library.Emitters[tracks[i].libmatch.active.highestMatchList[2].ID].SymbolCode1;
				tracks[i].libmatch.active.highestMatchList[2].symbolTwo =
						library.Emitters[tracks[i].libmatch.active.highestMatchList[2].ID].SymbolCode2;
			}
		}
		else
		{
			if (tracks[i].rf.getBandCodeLegacy() == 0)
			{
				tracks[i].libmatch.active.highestMatchList[0].symbolOne =
						0xC320;	// E
				if (eUkCounter > 8)
				{
					tracks[i].libmatch.active.highestMatchList[0].symbolTwo =
							UnknownModeCodes[8];
				}
				else
				{
					tracks[i].libmatch.active.highestMatchList[0].symbolTwo =
							UnknownModeCodes[eUkCounter];
					eUkCounter++;
				}
			}
			else if (tracks[i].rf.getBandCodeLegacy() == 1)
			{
				tracks[i].libmatch.active.highestMatchList[0].symbolOne =
						0xC390;	// G
				if (gUkCounter > 8)
				{
					tracks[i].libmatch.active.highestMatchList[0].symbolTwo =
							UnknownModeCodes[8];
				}
				else
				{
					tracks[i].libmatch.active.highestMatchList[0].symbolTwo =
							UnknownModeCodes[gUkCounter];
					gUkCounter++;
				}
			}
			else if (tracks[i].rf.getBandCodeLegacy() == 2)
			{
				tracks[i].libmatch.active.highestMatchList[0].symbolOne =
						0xC410;	// I
				if (iUkCounter > 8)
				{
					tracks[i].libmatch.active.highestMatchList[0].symbolTwo =
							UnknownModeCodes[8];
				}
				else
				{
					tracks[i].libmatch.active.highestMatchList[0].symbolTwo =
							UnknownModeCodes[iUkCounter];
					iUkCounter++;
				}
			}
			else if (tracks[i].rf.getBandCodeLegacy() == 3)
			{
				tracks[i].libmatch.active.highestMatchList[0].symbolOne =
						0xC440;	// J
				if (jUkCounter > 8)
				{
					tracks[i].libmatch.active.highestMatchList[0].symbolTwo =
							UnknownModeCodes[8];
				}
				else
				{
					tracks[i].libmatch.active.highestMatchList[0].symbolTwo =
							UnknownModeCodes[jUkCounter];
					jUkCounter++;
				}
			}
		}

		if ((tracks[i].libmatch.active.flagTrackingOnlyLibrary == 0)
				&& (tracks[i].libmatch.active.flagTrackingOnlyLibrary == 0))
		{
			if (tracks[i].power.flagTrackingModeHigh == 1)
			{
				if (tracks[i].libmatch.active.flagAerialOnlyLibrary == 1)
				{
					tracks[i].libmatch.active.highestMatchList[0].symbolOuter =
							0xCA60;
				}
				else
				{
					tracks[i].libmatch.active.highestMatchList[0].symbolOuter =
							0xC900;
				}
			}
		}
		if (tracks[i].libmatch.active.flagTrackingOnlyLibrary == 1)
		{
			if (tracks[i].libmatch.active.flagAerialOnlyLibrary == 1)
			{
				tracks[i].libmatch.active.highestMatchList[0].symbolOuter =
						0xCA60;
			}
			else
			{
				tracks[i].libmatch.active.highestMatchList[0].symbolOuter =
						0xC900;
			}
		}

		if (tracks[i].libmatch.active.flagMissileOnlyLibrary == 1)
		{
			tracks[i].libmatch.active.highestMatchList[0].symbolOuter =
					(tracks[i].libmatch.active.highestMatchList[0].symbolOuter)
							| (1 << 13);
			tracks[i].power.flagMissileLaunchHigh = 1;
		}
	}
}
void libraryCalculatePriorityOfTracks(vector<TrackDefaultClass> &tracks,
		LibraryStruct &library)
{
	uint16_t i = 0;
	for (i = 0; i < tracks.size(); i++)
	{
		if (tracks[i].libmatch.active.flagKnownLibrary == 1)
		{
			uint16_t emitterID =
					tracks[i].libmatch.active.highestMatchList[0].ID;
			if (library.Emitters[emitterID].emitterFunction == 13) // missile guidance has max priority
			{
				tracks[i].general.priority = 255;
			}
			tracks[i].general.priority = (library.Emitters[emitterID].TTE * 16)
					+ library.Emitters[emitterID].TPR;
		}
		else
		{
			if (tracks[i].rf.getBandCodeLegacy() == 2
					|| tracks[i].rf.getBandCodeLegacy() == 3)
			{
				if (tracks[i].power.flagTrackingModeHigh == 1)
				{
					tracks[i].general.priority = 255;
				}
				else
				{
					tracks[i].general.priority = 151;
				}
			}
			else
			{
				if (tracks[i].power.flagTrackingModeHigh == 1)
				{
					tracks[i].general.priority = 171;
				}
				else
				{
					tracks[i].general.priority = 101;
				}
			}
		}
		tracks[i].general.priorityDynamic = (uint8_t) tracks[i].general.priority
				/ 8;
	}
}
void libraryHideTracksWithSameDemodPtr(vector<TrackDefaultClass> &tracks,
		LibraryStruct &library)
{
	uint16_t i = 0, j = 0;
	int8_t checkforHighestPriorityTrack = 0;
	if (Model::ptyEnabled == 1)
	{
		for (i = 0; i < tracks.size(); i++)
		{
			tracks[i].general.flagVisiblePriority = 1;
			if (tracks[i].libmatch.active.flagKnownLibrary == 1)
			{
				for (j = 0; j < tracks.size(); j++)
				{
					if (tracks[j].libmatch.active.flagKnownLibrary == 1
							&& i != j)
					{
						uint16_t emitterIdRefTrack =
								tracks[i].libmatch.active.highestMatchList[0].ID;
						uint16_t emitterIdTargetTrack =
								tracks[j].libmatch.active.highestMatchList[0].ID;
						checkforHighestPriorityTrack = 0;
						if (library.Emitters[emitterIdRefTrack].demod_pointer
								== library.Emitters[emitterIdTargetTrack].demod_pointer)
						{
							checkforHighestPriorityTrack = 1;
						}
						if (checkforHighestPriorityTrack == 1)
						{
							uint16_t priorityOfRefTrack =
									library.Emitters[emitterIdRefTrack].TTE * 16
											+ library.Emitters[emitterIdRefTrack].TPR;
							uint16_t priorityOfTargetTrack =
									library.Emitters[emitterIdTargetTrack].TTE
											* 16
											+ library.Emitters[emitterIdRefTrack].TPR;
							if (priorityOfRefTrack > priorityOfTargetTrack)
							{
								tracks[j].general.flagVisiblePriority = 0;
								tracks[i].general.flagVisiblePriority = 1;
							}
							else
							{
								tracks[i].general.flagVisiblePriority = 0;
								tracks[j].general.flagVisiblePriority = 1;
							}
						}
					}
				}
			}
		}
	}
}
void libraryMatchEmitterPreprocess(TrackDefaultClass &track,
		LibraryStruct &library, LibraryMetaClass &libraryMeta)
{

	//strategy
	//add clear library parameters flag, how to clear without affecting string/merge?
	//locations for request lib match and force unknowns
	//pre and post processing conditions that work for track creation and updates
	//set unknown symbols (stable), priority, category if required
	//merge integrity of library mmatch parameters
	//create symbols, audio, cfd and surpression function
	//flight record flag for integrity of library match parameters

	//determine the library track flag and ground/air flag
	//these flags may need to be moved from here

	if (track.libmatch.flagRequestLibraryMatch == 1)
	{
		//track.libmatch.timeIterCountLibMatchExec = 0;
		track.libmatch.flagIterAttemptMatchLibrary = 1;

		track.power.flagMissileLaunchHigh = 0;
		track.libmatch.active.flagMissileOnlyLibrary = 0;
		track.general.flagSuppressEmitter = 0;
	}
	else
	{
		return;
	}
}

void libraryMatchEmitterPostprocess(TrackDefaultClass &track,
		LibraryStruct &library, LibraryMetaClass &libraryMeta,
		CfdLibrary &cfdLib)
{

	uint8_t flagSetKnownMatch = 0;
	uint8_t flagCfdFoundInLibrary = 0;

	//exit function if library match was not requested
	if (track.libmatch.flagRequestLibraryMatch == 1)
	{
		track.libmatch.flagRequestLibraryMatch = 0;
	}
	else
	{
		return;
	}
	//set the known/unknown library flag in track
	if (track.libmatch.last.highestMatchListCount > 0)
	{
		if (track.libmatch.last.highestMatchList[0].Total
				>= track.libmatch.last.thresholdScoreKnown)
		{
			track.libmatch.last.flagKnownLibrary = 1;
			track.libmatch.last.flagKnownPartialLibrary = 1;
		}
		else
		{
			track.libmatch.last.flagKnownLibrary = 0;
			track.libmatch.last.flagKnownPartialLibrary = 1;
		}
	}
	else
	{
		track.libmatch.last.flagKnownLibrary = 0;
		track.libmatch.last.flagKnownPartialLibrary = 0;
	}
	//set certain flags and variables if the emitter is known
	if (track.libmatch.last.flagKnownLibrary > 0)
	{
		if (library.Emitters[track.libmatch.last.highestMatchList[0].ID].platform
						== 3)	// SHIP
		{
			track.libmatch.last.flagShipOnlyLibrary = 1;
		}

		if (library.Emitters[track.libmatch.last.highestMatchList[0].ID].platform
				== 2)	// AIR
		{
			track.libmatch.last.flagAerialOnlyLibrary = 1;
		}
		if ((library.Emitters[track.libmatch.last.highestMatchList[0].ID].platform
				== 4)	// SINGLE CANNON
				|| (library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
						== 5)) // DOUBLE CANNON
		{
			track.libmatch.last.flagSamOnlyLibrary = 1;
		}
		if ((library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
				== 6)	// FIRE CONTROL
				|| (library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
						== 12)	// TARGET TRACKING
				|| (library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
						== 14)) // UNDEFINED LOCK_ON
		{
			track.libmatch.last.flagTrackingOnlyLibrary = 1;
			track.libmatch.last.flagScanningOnlyLibrary = 0;
		}
		if ((library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
				== 1) // IFF
				|| (library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
						== 2) // HEIGHT FINDER
				|| (library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
						== 3) // EARLY WARNING
				|| (library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
						== 4) // NAVIGATION
				|| (library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
						== 7) // SURFACE SEARCH
				|| (library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
						== 9) // AIRBORNE BOMBSEARCH
				|| (library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
						== 10) // BEACON
				|| (library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
						== 11))	// DATA TRANSMISSION
		{
			track.libmatch.last.flagScanningOnlyLibrary = 1;
			track.libmatch.last.flagTrackingOnlyLibrary = 0;
		}
		if ((library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
				== 13)) // MISSILE GUIDANCE
		{
			track.libmatch.last.flagMissileOnlyLibrary = 1;
		}
		if ((library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
				== 0) // NOT DEFINED
				|| (library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
						== 5) // AIRBORNE RADAR
				|| (library.Emitters[track.libmatch.last.highestMatchList[0].ID].emitterFunction
						== 8))	// TARGET ACQUISITION
		{
			track.libmatch.last.flagScanningOnlyLibrary = 0;
			track.libmatch.last.flagTrackingOnlyLibrary = 0;
		}

		// CFD Related info
		for (auto i = 0; i < cfdLib.cfdHeader.totalSequences; i++)
		{
			if ((cfdLib.cfdProgram[i].programId
					== library.Emitters[track.libmatch.last.highestMatchList[0].ID].cmSuggested))
			{
				flagCfdFoundInLibrary = 1;
				break;
			}
		}

		if (flagCfdFoundInLibrary == 0)
		{
			for (auto i = 0; i < cfdLib.cfdHeader.totalSequences; i++)
			{
				if ((cfdLib.cfdProgram[i].programId
						== library.Emitters[track.libmatch.last.highestMatchList[0].ID].cmAlternative))
				{
					flagCfdFoundInLibrary = 1;
					break;
				}
			}
		}

		if (flagCfdFoundInLibrary == 1)
		{
			if ((library.Emitters[track.libmatch.last.highestMatchList[0].ID].cmSuggested
					> 0)
					|| (library.Emitters[track.libmatch.last.highestMatchList[0].ID].cmAlternative
							> 0))
			{
				track.libmatch.last.cfdSuggestedId =
						library.Emitters[track.libmatch.last.highestMatchList[0].ID].cmSuggested;
				track.libmatch.last.cfdAlternativeId =
						library.Emitters[track.libmatch.last.highestMatchList[0].ID].cmAlternative;

				track.cfd.validCfdThreat = 1;
			}
			else
			{
				track.cfd.validCfdThreat = 0;
			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////

		track.libmatch.last.countPriRequiredLibrary =
				library.Emitters[track.libmatch.last.highestMatchList[0].ID].scores.priCountSTTReq;
		track.libmatch.last.countPriPresentLibrary =
				library.Emitters[track.libmatch.last.highestMatchList[0].ID].pri.priCount;
		if ((track.libmatch.last.highestMatchList[0].dwellCount > 0)
				&& (track.libmatch.last.countPriRequiredLibrary > 0)
				&& (track.libmatch.last.countPriRequiredLibrary < 6)
				&& (track.libmatch.last.countPriRequiredLibrary
						> ConstHigh::libraryCountPriDefaultThreshold))
		{
			track.libmatch.last.countPriRequiredKnown =
					track.libmatch.last.countPriRequiredLibrary;
		}
		else
		{
			track.libmatch.last.countPriRequiredKnown =
					ConstHigh::libraryCountPriDefaultThreshold;
		}
//		if (track.general.flagVisibleHigh == 1)

		if (track.general.timeIterCountLifetimeGlobal > 2)
		{
			if (track.libmatch.previous.highestMatchList[0].Total
					>= track.libmatch.previous.thresholdScoreKnown)
			{
				track.libmatch.last.countCategoryConsecutiveKnown =
						track.libmatch.previous.countCategoryConsecutiveKnown;
				if (track.libmatch.last.highestMatchList[0].emitterBatchId
						== track.libmatch.previous.highestMatchList[0].emitterBatchId)
				{
					track.libmatch.last.countCategoryConsecutiveKnown++;
				}
				else
				{
					track.libmatch.last.countCategoryConsecutiveKnown = 0;
				}
			}
			else
			{
				track.libmatch.last.countCategoryConsecutiveKnown = 0;
			}
		}
		if (library.Emitters[track.libmatch.last.highestMatchList[0].ID].erp.erp
				> 0)
		{
			track.libmatch.last.effectiveRadiatedPower =
					library.Emitters[track.libmatch.last.highestMatchList[0].ID].erp.erp;
		}
		track.libmatch.last.priorityLibrary =
				library.Emitters[track.libmatch.last.highestMatchList[0].ID].TPR
						* library.Emitters[track.libmatch.last.highestMatchList[0].ID].TTE;
		track.libmatch.last.flagDisplayLibrary = 1;	//this flag needs to be set from the library
//		track.libmatch.last.symbolOne =
//				library.Emitters[track.libmatch.last.highestMatchList[0].ID].SymbolCode1;
//		track.libmatch.last.symbolTwo =
//				library.Emitters[track.libmatch.last.highestMatchList[0].ID].SymbolCode2;
//		track.libmatch.last.symbolOuter = 0;//this will be set in a function that determines modes
	}
	//to display the emitter as known or not required further rules
	if (track.libmatch.last.flagKnownLibrary > 0)
	{
		flagSetKnownMatch = 1;

		if (((track.libmatch.last.flagAerialOnlyLibrary == 1)
				&& (track.rf.getBandCodeLegacy() == 2))
				|| ((track.libmatch.last.highestMatchList[0].RFScore == 0)
						&& (track.rf.getBandCodeLegacy() == 2))
				|| (track.libmatch.last.flagMissileOnlyLibrary == 1))
		{
			if ((track.libmatch.last.highestMatchList[0].dwellCount > 0)
					&& (track.libmatch.last.highestMatchList[0].dwellCount
							< track.libmatch.last.countPriRequiredKnown))
			{
				flagSetKnownMatch = 0;
				track.libmatch.last.flagKnownLibrary = 0;
			}
//			if ((track.general.flagVisibleHigh == 1)
//					&& (track.libmatch.last.countCategoryConsecutiveKnown > 1)
//					&& (track.libmatch.last.highestMatchList[0].dwellCount > 0)
//					&& ((track.libmatch.last.highestMatchList[0].dwellCount
//							>= track.libmatch.last.countPriRequiredKnown)
//							|| (track.libmatch.last.highestMatchList[0].dwellCount
//									>= track.libmatch.last.countPriPresentLibrary)))
//			{
//				flagSetKnownMatch = 1;
//				track.libmatch.last.flagKnownLibrary = 1;
//			}
			if ((track.general.timeIterCountLifetimeGlobal > 2)
					&& (track.libmatch.last.countCategoryConsecutiveKnown > 1)
					&& (track.libmatch.last.highestMatchList[0].dwellCount > 0)
					&& ((track.libmatch.last.highestMatchList[0].dwellCount
							>= track.libmatch.last.countPriRequiredKnown)
							|| (track.libmatch.last.highestMatchList[0].dwellCount
									>= track.libmatch.last.countPriPresentLibrary)))
			{
				flagSetKnownMatch = 1;
				track.libmatch.last.flagKnownLibrary = 1;
			}

		}
	}
	//if we decide to prceed with using this known match
	if(track.libmatch.last.flagKnownLibrary == 1)
	{
		if(library.Emitters[track.libmatch.last.highestMatchList[0].ID].suppressEmitter == 1)
		{
			track.libmatch.last.suppressEmitter = 1;
			track.general.flagSuppressEmitter = 1;
		}
		else
		{
			track.libmatch.last.suppressEmitter = 0;
			track.general.flagSuppressEmitter = 0;
		}
		uint32_t expiryTime =
				library.Emitters[track.libmatch.last.highestMatchList[0].ID].arp.arpUpper
						/ (200); // b/c one report is of 200ms
		if (expiryTime > track.general.timeIterLimitLifetime)
			track.general.timeIterLimitLifetime = expiryTime;
	}
	//if we decide to proceed with using this known match
	if ((track.libmatch.last.flagKnownLibrary > 0) && (flagSetKnownMatch == 1))
	{
		//if previous match was not the same as current one
		if (track.libmatch.previous.flagKnownLibrary == 1)
		{
			if (track.libmatch.last.highestMatchList[0].ID
					!= track.libmatch.previous.highestMatchList[0].ID)
			{
				//invalidate chaff and flare sequence
			}
		}
		track.libmatch.countConsecutiveUnknownLibrary = 0;
		track.libmatch.active = track.libmatch.last;
	}
	//if the computed match was unknown
	if (track.libmatch.last.flagKnownLibrary == 0)
	{

		track.libmatch.countConsecutiveUnknownLibrary++;
		if ((track.libmatch.countConsecutiveUnknownLibrary > 6)
				|| ((track.libmatch.countConsecutiveUnknownLibrary > 3)
						&& ((track.rf.getBandCodeLegacy() == 0)
								|| (track.rf.getBandCodeLegacy() == 1)))
				|| ((track.libmatch.countConsecutiveUnknownLibrary > 1)
						&& (track.libmatch.active.flagKnownLibrary == 1)
						&& (track.libmatch.active.flagAerialOnlyLibrary == 0)
						&& (track.rf.getBandCodeLegacy() == 2))
				|| (track.libmatch.flagSetUnknownLibrary == 1))
		{

			//mark cfd as invalid
			//pTrack->ChaffIssunaceStatus = NoChaffDefined;
			//pTrack->FlareIssunaceStatus = NoFlareDefined;
			if (track.general.trackType == TrackTypeEnum::track_cw)
			{
				track.libmatch.last.flagAerialOnlyLibrary = 0;
				track.libmatch.last.flagMissileOnlyLibrary = 0;
				track.libmatch.last.flagSamOnlyLibrary = 0;
				track.libmatch.last.flagScanningOnlyLibrary = 0;
				track.libmatch.last.flagTrackingOnlyLibrary = 1;
				track.libmatch.last.effectiveRadiatedPower = 120;

			}
			else if (track.rf.getBandCodeLegacy() == 0)
			{
				track.libmatch.last.flagAerialOnlyLibrary = 0;
				track.libmatch.last.flagMissileOnlyLibrary = 0;
				track.libmatch.last.flagSamOnlyLibrary = 0;
				track.libmatch.last.flagScanningOnlyLibrary = 1;
				track.libmatch.last.flagTrackingOnlyLibrary = 0;
				track.libmatch.last.effectiveRadiatedPower = 110;
			}
			else if (track.rf.getBandCodeLegacy() == 1)
			{
				track.libmatch.last.flagAerialOnlyLibrary = 0;
				track.libmatch.last.flagMissileOnlyLibrary = 0;
				track.libmatch.last.flagSamOnlyLibrary = 0;
				track.libmatch.last.flagScanningOnlyLibrary = 1;
				track.libmatch.last.flagTrackingOnlyLibrary = 0;
				track.libmatch.last.effectiveRadiatedPower = 120;
			}
			else if (track.rf.getBandCodeLegacy() == 2)
			{
				if (track.pri.accumulatePriStaggerCount > 0)
				{
					track.libmatch.last.flagAerialOnlyLibrary = 0;
					track.libmatch.last.flagMissileOnlyLibrary = 0;
					track.libmatch.last.flagSamOnlyLibrary = 0;
					track.libmatch.last.flagScanningOnlyLibrary = 0;
					track.libmatch.last.flagTrackingOnlyLibrary = 0;
					track.libmatch.last.effectiveRadiatedPower = 120;
				}
				else
				{
					track.libmatch.last.flagAerialOnlyLibrary = 1;
					track.libmatch.last.flagMissileOnlyLibrary = 0;
					track.libmatch.last.flagSamOnlyLibrary = 0;
					track.libmatch.last.flagScanningOnlyLibrary = 0;
					track.libmatch.last.flagTrackingOnlyLibrary = 0;
					track.libmatch.last.effectiveRadiatedPower = 110;
				}
			}
			else if (track.rf.getBandCodeLegacy() == 3)
			{
				track.libmatch.last.flagAerialOnlyLibrary = 0;
				track.libmatch.last.flagMissileOnlyLibrary = 0;
				track.libmatch.last.flagSamOnlyLibrary = 1;
				track.libmatch.last.flagScanningOnlyLibrary = 0;
				track.libmatch.last.flagTrackingOnlyLibrary = 1;
				track.libmatch.last.effectiveRadiatedPower = 130;
			}
			else if (track.rf.getBandCodeLegacy() == 4)
			{
				track.libmatch.last.flagAerialOnlyLibrary = 0;
				track.libmatch.last.flagMissileOnlyLibrary = 0;
				track.libmatch.last.flagSamOnlyLibrary = 1;
				track.libmatch.last.flagScanningOnlyLibrary = 0;
				track.libmatch.last.flagTrackingOnlyLibrary = 1;
				track.libmatch.last.effectiveRadiatedPower = 110;
			}
			else if (track.rf.getBandCodeLegacy() == 5)
			{
				track.libmatch.last.flagAerialOnlyLibrary = 0;
				track.libmatch.last.flagMissileOnlyLibrary = 0;
				track.libmatch.last.flagSamOnlyLibrary = 1;
				track.libmatch.last.flagScanningOnlyLibrary = 0;
				track.libmatch.last.flagTrackingOnlyLibrary = 1;
				track.libmatch.last.effectiveRadiatedPower = 130;
			}
			track.libmatch.active = track.libmatch.last;
			//set previous library match here for unknown category number

			track.libmatch.last.countCategoryConsecutiveKnown = 0;
			track.libmatch.countConsecutiveUnknownLibrary = 0;
		}
	}
	if (track.libmatch.active.flagScanningOnlyLibrary == 1)
	{
		track.audio.flagNewGuyAudioPlayed = 1;
	}
	if ((track.libmatch.active.flagKnownLibrary == 0)
			&& (track.rf.getBandCodeLegacy() == 0))
	{
		track.audio.flagNewGuyAudioPlayed = 1;
	}
	//update the chaff staus
	/*if(pTrack->ChaffIssunaceStatus == NoChaffDefined)
	 {
	 ChaffLibIndex = 0xFF;
	 if(pTrack->LibMatchIndex < MAX_LIB_ENTRIES)
	 {
	 ChaffLibIndex = pCurrentLibrary->EmittersExtendedData[pTrack->LibMatchIndex].ChaffInst;
	 }
	 if(ChaffLibIndex == 0xFF)
	 pTrack->ChaffIssunaceStatus = NoChaffDefined;
	 else
	 {
	 pTrack->ChaffLibraryIndex = ChaffLibIndex;
	 pTrack->ChaffIssunaceStatus = ChaffNotIssued;
	 }
	 }*/
	//update the Flare status
	/*if(pTrack->FlareIssunaceStatus == NoFlareDefined)
	 {
	 FlareLibIndex = 0xFF;
	 if(pTrack->LibMatchIndex < MAX_LIB_ENTRIES)
	 {
	 FlareLibIndex = pCurrentLibrary->EmittersExtendedData[pTrack->LibMatchIndex].FlareInst;
	 }
	 if(FlareLibIndex == 0xFF)
	 pTrack->FlareIssunaceStatus = NoFlareDefined;
	 else
	 {
	 pTrack->FlareLibraryIndex = FlareLibIndex;
	 pTrack->FlareIssunaceStatus = FlareNotIssued;
	 }
	 }*/
	track.libmatch.flagSetUnknownLibrary = 0;
	track.libmatch.active = track.libmatch.last; // temporary
	track.libmatch.previous = track.libmatch.last;
	//this needs to move to an appropriate location, need them during track creation time
	/*
	 track.libmatch.current.flagAerialOnlyLibrary = 0;
	 track.libmatch.current.flagKnownLibrary = 0;
	 track.libmatch.current.flagKnownPartialLibrary = 0;
	 track.libmatch.current.flagTrackingOnlyLibrary = 0;
	 track.libmatch.current.flagScanningOnlyLibrary = 0;
	 track.libmatch.current.flagMissileOnlyLibrary = 0;
	 track.libmatch.current.flagSamOnlyLibrary = 0;
	 track.libmatch.current.flagDisplayLibrary = 0;
	 track.libmatch.current.priorityLibrary = 0;
	 track.libmatch.current.countCategoryConsecutiveKnown = 0;
	 track.libmatch.current.countPriPresentLibrary = 0;
	 track.libmatch.current.countPriRequiredKnown = ConstHigh::libraryCountPriDefaultThreshold;
	 track.libmatch.current.countPriRequiredLibrary = 0;
	 track.libmatch.current.effectiveRadiatedPower = 0;
	 track.libmatch.current.highestMatchListCount = 0;
	 track.libmatch.current.thresholdScoreKnown = ConstHigh::libraryScoreDefaultThreshold;
	 */
}

//Saad's logic and code. Change with caution! xD
//This function matches a single Track to library, sequence of operation is as:
//First, we extract PRIs, PWs, RFs from track irrespective of their types
//Then we extract Library Emmiter IDs from PRI,RF,PW hash of library
//After extraction, track is matched with all library emmiters and scored
//Like for like types are matched in PRI,RF anfd PW. In all these, there are
//3 types of matching, points on points, points on ranges and ranges on ranges
//overlap. In all these, percentage of overlap is multiplied by total score
//In the end, top 3 scorers are copied to track as library match

//need to add unknown flag, id, category in track library match. Also need a
//way to track a weak library match across iterations
void libraryMatchEmitter(TrackDefaultClass &track, LibraryStruct &library,
		LibraryMetaClass &libraryMeta)
{

	uint8_t PRIConstant = 0, PWConstant = 0, RFConstant = 0, lpiConstant = 0;
	uint8_t PRITypeConstant = 0, PWTypeConstant = 0, RFTypeConstant = 0;
	uint16_t sumLibScore = 0;
	uint16_t returnIDsPRIs[1000], returnIDsRFs[1000], returnIDsPWs[1000];
	uint16_t returnIDsPRInRF[2000], returnIDsIntersect[2000];
	uint16_t returnIDsAll[2000];
	uint16_t returnIDsAllSameBand[2000];
	uint32_t localCountSlidingPri = 0, localCountJitterPri = 0;
	uint32_t localCountComplexPri = 0, upper, lower, sizeOfnonRepeat;
	uint32_t tIter = 0, lIter = 0, ptrCount = 0, i = 0, j = 0;
	int32_t numberOfIDsOfPRIs = 0, numberOfIDsOfRFs = 0, numberOfIDsOfPWs = 0;
	uint32_t PRIcount = 0, RFcount = 0, PWcount = 0, PRInRFcount = 0;
	uint32_t IntersectCount = 0, TotalCount = 0;
	F32 DwellMatched = 0, SliderMatched = 0, SliderMatchedMax = 0;
	F32 JitterMatched = 0, JitterMatchedMax = 0, staggerMatched = 0,
			diffToaMatched = 0;
	F32 FixedTypeFound = 0, DwellTypeFound = 0, DiffToaTypeFound = 0,
			RFMatched = 0, PWMatched = 0;
	F32 ComplexMatched = 0, ComplexMatchedMax = 0, RFComplex = 0;
	F32 rfSpotCount = 0, RFComplexPrevious = 0, priCandidateScore = 0;
	F32 pwSpotCount = 0;
	vector<uint32_t> localPriSpots;
	vector<LibraryMatchScore> scoresTotal;
	F32 NoOfSpotsMatched = 0;
	if (track.libmatch.flagRequestLibraryMatch == 0)
	{
		return;
	}

	memset(returnIDsPRIs, 0, sizeof(returnIDsPRIs));
	memset(returnIDsRFs, 0, sizeof(returnIDsRFs));
	memset(returnIDsPWs, 0, sizeof(returnIDsPWs));
	memset(returnIDsPRInRF, 0, sizeof(returnIDsPRInRF));
	memset(returnIDsIntersect, 0, sizeof(returnIDsIntersect));
	memset(returnIDsAll, 0, sizeof(returnIDsAll));
	memset(returnIDsAllSameBand, 0, sizeof(returnIDsAllSameBand));


	PRIConstant = library.header.tlScores.priScore;
	PRITypeConstant = library.header.tlScores.priTypeScore;
	PWConstant = library.header.tlScores.pwScore;
	PWTypeConstant = library.header.tlScores.pwTypeScore;
	RFConstant = library.header.tlScores.rfScore;
	RFTypeConstant = library.header.tlScores.rfTypeScore;
	track.libmatch.last.thresholdScoreKnown =
			library.header.tlScores.globalMatchPercentRequired;
	PWTypeConstant = 0;
	RFTypeConstant = 0;
	if (track.rf.accumulateRf.size() == 0 && RFConstant!=0)
	{
		float multiplier = 100/(100.0f-RFConstant);
		PRIConstant = round(multiplier * (float)PRIConstant);
		PRITypeConstant =round( multiplier *(float)PRITypeConstant);
		PWConstant = round(multiplier * (float)PWConstant);
		sumLibScore += PRIConstant;
		sumLibScore += PRITypeConstant;
		sumLibScore += PWConstant;
		RFConstant = 0;
		if(PRITypeConstant > 0)
		{
			PRITypeConstant = PRITypeConstant + (100-sumLibScore);
		}
		else if(PWConstant > 0)
		{
			PWConstant = PWConstant + (100-sumLibScore);
		}
		track.libmatch.last.thresholdScoreKnown = PRIConstant + PRITypeConstant +1;
	}

//	track.libmatch.active.thresholdScoreKnown = library.header.tlScores.globalMatchPercentRequired;
//	track.libmatch.previous.thresholdScoreKnown = library.header.tlScores.globalMatchPercentRequired;

	//This function finds number of PRI spots/ranges for each type of PRI in track	
	libraryCountTrackPriTypes(track.pri);
	//This function finds number of RF spots/ranges for each type of RF in track
	libraryCountTrackRfTypes(track.rf);
	//Total number of RF in track
	RFcount = libraryNumberOfRf(track.rf);
	//Total number of PW in track
	PWcount = libraryNumberOfPw(track.pw);
	//Total number of PRI in track
	PRIcount = libraryNumberOfPri(track.pri, track.libmatch);
	localPriSpots.resize(PRIcount, 0);

	//extract all PRI spots from the input emitter across all types and ranges
	//at a resolution of ?? us
	//tracklibrary would subsitute for extEmitter while count of each pri type
	//would available
	//in trackpri structure
	//libraryPrintHighEmitter(track);

	if (track.pri.accumulatePriFixedCount > 0)
	{
		for (i = 0; i < track.pri.accumulatePri.size(); i++)
		{
			if ((track.pri.accumulatePri[i].priType == PriTypeEnum::pri_fixed)
					|| (track.pri.accumulatePri[i].priType
							== PriTypeEnum::pri_dwellswitch))
			{
				lower = track.pri.accumulatePri[i].priLow;
				localPriSpots[ptrCount] = lower / 100;
				ptrCount++;
			}
		}
	}
	if (track.pri.accumulatePriDwellSwitchCount > 0)
	{
		for (i = 0; i < track.pri.accumulatePri.size(); i++)
		{
			if ((track.pri.accumulatePri[i].priType == PriTypeEnum::pri_fixed)
					|| (track.pri.accumulatePri[i].priType
							== PriTypeEnum::pri_dwellswitch))
			{
				lower = track.pri.accumulatePri[i].priLow;
				localPriSpots[ptrCount] = lower / 100;
				ptrCount++;
			}
		}
	}
	if (track.pri.accumulatePriStaggerCount > 0)
	{
		for (i = 0; i < track.pri.accumulatePri.size(); i++)
		{
			if (track.pri.accumulatePri[i].priType == PriTypeEnum::pri_stagger)
			{
				lower = track.pri.accumulatePri[i].priLow;
				localPriSpots[ptrCount] = lower / 100;
				ptrCount++;
			}
		}
	}
	if (track.pri.accumulatePriDiffToaCount > 0)
	{
		for (i = 0; i < track.pri.accumulatePri.size(); i++)
		{
			if (track.pri.accumulatePri[i].priType == PriTypeEnum::pri_difftoa)
			{
				lower = track.pri.accumulatePri[i].priLow;
				localPriSpots[ptrCount] = lower / 100;
				ptrCount++;
			}
		}
	}
	//add loop for multiple ranges
	if (track.pri.accumulatePriJitterCount > 0)
	{
		for (i = 0; i < track.pri.accumulatePri.size(); i++)
		{
			if (track.pri.accumulatePri[i].priType == PriTypeEnum::pri_jitter)
			{
				upper = track.pri.accumulatePri[i].priHigh;
				lower = track.pri.accumulatePri[i].priLow;
				upper = upper / 100;
				lower = lower / 100;
				localCountJitterPri = upper - lower + 1;
				if (upper == lower)
				{
					localPriSpots[ptrCount] = upper;
					ptrCount++;
				}
				else
				{
					for (j = 0; j < localCountJitterPri; j++)
					{
						localPriSpots[ptrCount] = lower;
						ptrCount++;
						lower++;
					}
				}
			}
		}
	}
	//add loop for multiple ranges
	if (track.pri.accumulatePriSlidingCount > 0)
	{
		for (i = 0; i < track.pri.accumulatePri.size(); i++)
		{
			if (track.pri.accumulatePri[i].priType == PriTypeEnum::pri_sliding)
			{
				upper = track.pri.accumulatePri[i].priHigh;
				lower = track.pri.accumulatePri[i].priLow;
				upper = upper / 100;
				lower = lower / 100;
				localCountSlidingPri = upper - lower + 1;
				if (upper == lower)
				{
					localPriSpots[ptrCount] = upper;
					ptrCount++;
				}
				else
				{
					for (j = 0; j < localCountSlidingPri; j++)
					{
						localPriSpots[ptrCount] = lower;
						ptrCount++;
						lower++;
					}
				}
			}
		}
	}
	//add loop for multiple ranges
	//add loop for multiple ranges
	if (track.pri.accumulatePriComplexCount > 0)
	{
		for (i = 0; i < track.pri.accumulatePri.size(); i++)
		{
			if (track.pri.accumulatePri[i].priType == PriTypeEnum::pri_complex)
			{
				upper = track.pri.accumulatePri[i].priHigh;
				lower = track.pri.accumulatePri[i].priLow;
				upper = upper / 100;
				lower = lower / 100;
				localCountSlidingPri = upper - lower + 1;
				if (upper == lower)
				{
					localPriSpots[ptrCount] = upper;
					ptrCount++;
				}
				else
				{
					for (j = 0; j < localCountComplexPri; j++)
					{
						localPriSpots[ptrCount] = lower;
						ptrCount++;
						lower++;
					}
				}
			}
		}
	}

	//lookup library emitters ids that have matching (pri or rf) and pw with
	//our given emitter
	numberOfIDsOfPRIs = libraryPriLookup(localPriSpots, ptrCount, returnIDsPRIs,
			libraryMeta);
	//libraryRadixSort(returnIDsPRIs,numberOfIDsOfPRIs);
	quickSort(returnIDsPRIs, 0, numberOfIDsOfPRIs - 1);
	sizeOfnonRepeat = libraryRemoveDuplicateFromArray(returnIDsPRIs,
			numberOfIDsOfPRIs);
	numberOfIDsOfPRIs = sizeOfnonRepeat;

	numberOfIDsOfRFs = libraryRfLookup(track.rf.accumulateRf, RFcount,
			returnIDsRFs, libraryMeta);
	//libraryRadixSort(returnIDsRFs,RFcount);
	//libraryRadixSort(returnIDsRFs,numberOfIDsOfRFs);//review later
	quickSort(returnIDsRFs, 0, numberOfIDsOfRFs - 1);
	sizeOfnonRepeat = libraryRemoveDuplicateFromArray(returnIDsRFs,
			numberOfIDsOfRFs);
	RFcount = sizeOfnonRepeat;
	numberOfIDsOfPWs = libraryPwLookup(track.pw.accumulatePw, PWcount,
			returnIDsPWs, libraryMeta);
	//libraryRadixSort(returnIDsPWs,PWcount);
	//libraryRadixSort(returnIDsPWs,numberOfIDsOfPWs);//review later
	quickSort(returnIDsPWs, 0, numberOfIDsOfPWs - 1);
	sizeOfnonRepeat = libraryRemoveDuplicateFromArray(returnIDsPWs,
			numberOfIDsOfPWs);
	PWcount = sizeOfnonRepeat;

	PRInRFcount = libraryUnion(returnIDsRFs, returnIDsPRIs, returnIDsPRInRF,
			RFcount, numberOfIDsOfPRIs);
	//libraryRadixSort(returnIDsPRInRF,PRInRFcount);
	quickSort(returnIDsPRInRF, 0, PRInRFcount - 1);
	sizeOfnonRepeat = libraryRemoveDuplicateFromArray(returnIDsPRInRF,
			PRInRFcount);
	PRInRFcount = sizeOfnonRepeat;

	IntersectCount = libraryIntersection(returnIDsPRInRF, returnIDsPWs,
			returnIDsIntersect, PRInRFcount, PWcount);
	//libraryRadixSort(returnIDsIntersect,IntersectCount);
	quickSort(returnIDsIntersect, 0, IntersectCount - 1);
	sizeOfnonRepeat = libraryRemoveDuplicateFromArray(returnIDsIntersect,
			IntersectCount);
	IntersectCount = sizeOfnonRepeat;

	TotalCount = libraryUnion(returnIDsIntersect, returnIDsPRInRF, returnIDsAll,
			IntersectCount, PRInRFcount);
	// libraryRadixSort(returnIDsAll,TotalCount);
	quickSort(returnIDsAll, 0, TotalCount - 1);
	sizeOfnonRepeat = libraryRemoveDuplicateFromArray(returnIDsAll, TotalCount);
	TotalCount = sizeOfnonRepeat;

	sizeOfnonRepeat = libraryRemoveDifferentBandsFromArray(track, libraryMeta,
			returnIDsAllSameBand, returnIDsAll, TotalCount);
	TotalCount = sizeOfnonRepeat;

	//printf("TC: %d\n",TotalCount);
	//scoresTotal = (LibraryMatchScore *)calloc(TotalCount,sizeof(LibraryMatchScore));//turn into vector
	scoresTotal.resize(TotalCount);

	libraryScoreInit(scoresTotal, TotalCount);

	//assign scores to each emitter that has been found from hash lookup of pri
	//rf,pw
	//returnIDsAllSameBand has all the emitter IDs needed for matching
	for (j = 0; j < TotalCount; j++)
	{
		FixedTypeFound = 0, DwellTypeFound = 0;
		//if library programmed emitter type is cw, ignore it
		if (library.Emitters[returnIDsAllSameBand[j]].emitterType != 1)
		{
			continue;
		}

		//assign pri type score, fix this later to handle multiple pri types in
		//the emitter

		//library has fixed pri and emitter has fixed pri
		if ((track.pri.accumulatePriFixedCount > 0)
				&& (library.Emitters[returnIDsAllSameBand[j]].pri.priType
						== TL_PRI_TYPES_FIXED))
		{
			scoresTotal[j].PRItypeScore = PRITypeConstant;
			FixedTypeFound = 1;
		}
		//library has dwell pri and emitter has fixed pri
		if ((track.pri.accumulatePriFixedCount > 0) && (FixedTypeFound == 0))
		{
			if (library.Emitters[returnIDsAllSameBand[j]].pri.priType
					== TL_PRI_TYPES_DWELL)
			{
				scoresTotal[j].PRItypeScore = PRITypeConstant / 2;
			}
		}
		//library has dwell pri and emitter has dwell pri
		if ((track.pri.accumulatePriDwellSwitchCount > 1)
				&& (library.Emitters[returnIDsAllSameBand[j]].pri.priType
						== TL_PRI_TYPES_DWELL))
		{
			scoresTotal[j].PRItypeScore = PRITypeConstant;
			DwellTypeFound = 1;
		}
		//library has fixed pri and emitter has dwell pri
		if ((track.pri.accumulatePriDwellSwitchCount > 0)
				&& (DwellTypeFound == 0) && (FixedTypeFound == 0))
		{
			if (library.Emitters[returnIDsAllSameBand[j]].pri.priType
					== TL_PRI_TYPES_FIXED)
			{
				scoresTotal[j].PRItypeScore = PRITypeConstant / 2;
			}
		}
		//library has stagger pri and emitter has stagger pri
		if ((track.pri.accumulatePriStaggerCount > 0)
				&& (library.Emitters[returnIDsAllSameBand[j]].pri.priType
						== TL_PRI_TYPES_STAGGER))
		{
			scoresTotal[j].PRItypeScore = PRITypeConstant;
		}
		//library has difftoa pri and emitter has difftoa pri
		if ((track.pri.accumulatePriDiffToaCount > 0)
				&& (library.Emitters[returnIDsAllSameBand[j]].pri.priType
						== TL_PRI_TYPES_DIFFTOA))
		{
			scoresTotal[j].PRItypeScore = PRITypeConstant;
			DiffToaTypeFound = 1;
		}
		if ((track.pri.accumulatePriDiffToaCount > 0) && (DiffToaTypeFound == 0)
				&& (DwellTypeFound == 0) && (FixedTypeFound == 0))
		{
			if (library.Emitters[returnIDsAllSameBand[j]].pri.priType
					== TL_PRI_TYPES_FIXED
					|| library.Emitters[returnIDsAllSameBand[j]].pri.priType
							== TL_PRI_TYPES_DWELL)
			{
				scoresTotal[j].PRItypeScore = PRITypeConstant / 2;
			}
		}
		//library has sliding pri and emitter has sliding pri
		if ((track.pri.accumulatePriSlidingCount > 0)
				&& (library.Emitters[returnIDsAllSameBand[j]].pri.priType
						== TL_PRI_TYPES_SLIDER))
		{
			scoresTotal[j].PRItypeScore = PRITypeConstant;
		}
		//library has jitter pri and emitter has jitter pri
		if ((track.pri.accumulatePriJitterCount > 0)
				&& (library.Emitters[returnIDsAllSameBand[j]].pri.priType
						== TL_PRI_TYPES_JITTER))
		{
			scoresTotal[j].PRItypeScore = PRITypeConstant;
		}
		//library has complex pri and emitter has complex pri
		if ((track.pri.accumulatePriComplexCount > 0)
				&& (library.Emitters[returnIDsAllSameBand[j]].pri.priType
						== TL_PRI_TYPES_COMPLEX))
		{
			scoresTotal[j].PRItypeScore = PRITypeConstant;
		}
		//now perform pri value matching

		//where to reset these variables?
		DwellMatched = 0;
		NoOfSpotsMatched = 0;
		staggerMatched = 0;
		diffToaMatched = 0;
		SliderMatchedMax = 0;
		SliderMatched = 0;
		JitterMatchedMax = 0;
		JitterMatched = 0;
		ComplexMatchedMax = 0;
		ComplexMatched = 0;
		vector<uint16_t> dwellMatchedIndices;
		for (tIter = 0; tIter < track.pri.accumulatePri.size(); tIter++)
		{
			//first match dwell and switch spots
			if (((track.pri.accumulatePri[tIter].priType
					== PriTypeEnum::pri_fixed)
					|| (track.pri.accumulatePri[tIter].priType
							== PriTypeEnum::pri_dwellswitch))
					&& ((library.Emitters[returnIDsAllSameBand[j]].pri.priType
							== TL_PRI_TYPES_FIXED)
							|| (library.Emitters[returnIDsAllSameBand[j]].pri.priType
									== TL_PRI_TYPES_DWELL)))
			{
				dwellMatchedIndices.resize(
						library.Emitters[returnIDsAllSameBand[j]].pri.priCount,
						0);
				for (lIter = 0;
						lIter
								< library.Emitters[returnIDsAllSameBand[j]].pri.priCount;
						lIter++)
				{
					if ((library.Emitters[returnIDsAllSameBand[j]].pri.priSpots[lIter].priLower
							<= track.pri.accumulatePri[tIter].priLow)
							&& (library.Emitters[returnIDsAllSameBand[j]].pri.priSpots[lIter].priUpper
									>= track.pri.accumulatePri[tIter].priLow))
					{
						DwellMatched++;
						if (dwellMatchedIndices[lIter] == 0)
						{
							NoOfSpotsMatched++;
							dwellMatchedIndices[lIter] = 1;
						}
						else
							dwellMatchedIndices[lIter]++;
						//DwellTypeMatched = PRITypeConstant;
						//PRIDwellFound = 1;
						break;
					}
				}
			}
			if ((track.pri.accumulatePri[tIter].priType
					== PriTypeEnum::pri_stagger)
					&& (library.Emitters[returnIDsAllSameBand[j]].pri.priType
							== TL_PRI_TYPES_STAGGER))
			{
				for (lIter = 0;
						lIter
								< library.Emitters[returnIDsAllSameBand[j]].pri.priCount;
						lIter++)
				{
					if ((library.Emitters[returnIDsAllSameBand[j]].pri.priSpots[lIter].priLower
							<= track.pri.accumulatePri[tIter].priLow)
							&& (library.Emitters[returnIDsAllSameBand[j]].pri.priSpots[lIter].priUpper
									>= track.pri.accumulatePri[tIter].priLow))
					{

						staggerMatched++;
						break;
					}
				}
			}
			if ((track.pri.accumulatePri[tIter].priType
					== PriTypeEnum::pri_difftoa)
					&& (library.Emitters[returnIDsAllSameBand[j]].pri.priType
							== TL_PRI_TYPES_DIFFTOA
							|| library.Emitters[returnIDsAllSameBand[j]].pri.priType
									== TL_PRI_TYPES_FIXED
							|| library.Emitters[returnIDsAllSameBand[j]].pri.priType
									== TL_PRI_TYPES_DWELL))
			{
				for (lIter = 0;
						lIter
								< library.Emitters[returnIDsAllSameBand[j]].pri.priCount;
						lIter++)
				{
					if ((library.Emitters[returnIDsAllSameBand[j]].pri.priSpots[lIter].priLower
							<= track.pri.accumulatePri[tIter].priLow)
							&& (library.Emitters[returnIDsAllSameBand[j]].pri.priSpots[lIter].priUpper
									>= track.pri.accumulatePri[tIter].priLow))
					{

						diffToaMatched++;
						break;
					}
				}
			}
			if ((track.pri.accumulatePri[tIter].priType
					== PriTypeEnum::pri_sliding)
					&& (library.Emitters[returnIDsAllSameBand[j]].pri.priType
							== TL_PRI_TYPES_SLIDER))
			{
				for (lIter = 0;
						lIter
								< library.Emitters[returnIDsAllSameBand[j]].pri.priCount;
						lIter++)
				{

					SliderMatched =
							(F32) libraryFindOverlapRegionPercentage(
									library.Emitters[returnIDsAllSameBand[j]].pri.priSpots[lIter].priLower,
									library.Emitters[returnIDsAllSameBand[j]].pri.priSpots[lIter].priUpper,
									track.pri.accumulatePri[tIter].priLow,
									track.pri.accumulatePri[tIter].priHigh);
					if (SliderMatched > SliderMatchedMax)
					{
						SliderMatchedMax = SliderMatched;
					}
				}
			}
			if ((track.pri.accumulatePri[tIter].priType
					== PriTypeEnum::pri_jitter)
					&& (library.Emitters[returnIDsAllSameBand[j]].pri.priType
							== TL_PRI_TYPES_JITTER))
			{
				for (lIter = 0;
						lIter
								< library.Emitters[returnIDsAllSameBand[j]].pri.priCount;
						lIter++)
				{

					JitterMatched =
							(F32) libraryFindOverlapRegionPercentage(
									library.Emitters[returnIDsAllSameBand[j]].pri.priSpots[lIter].priLower,
									library.Emitters[returnIDsAllSameBand[j]].pri.priSpots[lIter].priUpper,
									track.pri.accumulatePri[tIter].priLow,
									track.pri.accumulatePri[tIter].priHigh);
					if (JitterMatched > JitterMatchedMax)
					{
						JitterMatchedMax = JitterMatched;
					}
				}
			}
			if ((track.pri.accumulatePri[tIter].priType
					== PriTypeEnum::pri_complex)
					&& (library.Emitters[returnIDsAllSameBand[j]].pri.priType
							== TL_PRI_TYPES_COMPLEX))
			{
				for (lIter = 0;
						lIter
								< library.Emitters[returnIDsAllSameBand[j]].pri.priCount;
						lIter++)
				{
					ComplexMatched =
							(F32) libraryFindOverlapRegionPercentage(
									library.Emitters[returnIDsAllSameBand[j]].pri.priSpots[lIter].priLower,
									library.Emitters[returnIDsAllSameBand[j]].pri.priSpots[lIter].priUpper,
									track.pri.accumulatePri[tIter].priLow,
									track.pri.accumulatePri[tIter].priHigh);
					if (ComplexMatched > ComplexMatchedMax)
					{
						ComplexMatchedMax = ComplexMatched;
					}
				}
			}
		}

		//assign score to fixed and dwell and switch spots
		if (DwellMatched > 0)
		{
			priCandidateScore = (DwellMatched
					/ track.pri.accumulatePriDwellSwitchCount) * PRIConstant;
			if (priCandidateScore > scoresTotal[j].PRIScore)
			{
				scoresTotal[j].PRIScore = priCandidateScore;
				scoresTotal[j].noOfSpotsMatched = NoOfSpotsMatched;
				scoresTotal[j].libraryPriCount =
						library.Emitters[returnIDsAllSameBand[j]].pri.priCount;
				scoresTotal[j].dwellCount = DwellMatched;
			}
		}
		//assign score to stagger spots
		if (staggerMatched > 0)
		{
			priCandidateScore = (staggerMatched
					/ track.pri.accumulatePriStaggerCount) * PRIConstant;
			if (priCandidateScore > scoresTotal[j].PRIScore)
			{
				scoresTotal[j].PRIScore = priCandidateScore;
				scoresTotal[j].staggerCount = staggerMatched;
			}
		}
		if (diffToaMatched > 0)
		{
			priCandidateScore = (diffToaMatched
					/ track.pri.accumulatePriDiffToaCount) * PRIConstant;
			if (priCandidateScore > scoresTotal[j].PRIScore)
			{
				scoresTotal[j].PRIScore = priCandidateScore;
				scoresTotal[j].diffToaCount = diffToaMatched;
			}
		}
		//assign score to sliding range
		if (SliderMatchedMax > 0)
		{
			priCandidateScore = (SliderMatchedMax * PRIConstant / 100);
			if (priCandidateScore > scoresTotal[j].PRIScore)
			{
				scoresTotal[j].PRIScore = priCandidateScore;
				scoresTotal[j].slidingCount = 1;
			}
		}
		//assign score to jitter range
		if (JitterMatchedMax > 0)
		{
			priCandidateScore = (JitterMatchedMax * PRIConstant / 100);
			if (priCandidateScore > scoresTotal[j].PRIScore)
			{
				scoresTotal[j].PRIScore = priCandidateScore;
				scoresTotal[j].jitterCount = 1;
			}
		}
		//assign score to complex range
		if (ComplexMatchedMax > 0)
		{
			priCandidateScore = (ComplexMatchedMax * PRIConstant / 100);
			if (priCandidateScore > scoresTotal[j].PRIScore)
			{
				scoresTotal[j].PRIScore = priCandidateScore;
				scoresTotal[j].complexCount = 1;
			}
		}

		RFMatched = 0;
		rfSpotCount = 0;
		RFComplex = 0;
		RFComplexPrevious = 0;
		//calculate rf value score for this emitter
		for (tIter = 0; tIter < track.rf.accumulateRf.size(); tIter++)
		{

			if ((track.rf.accumulateRf[tIter].rfType == RfTypeEnum::rf_agile)
					|| (track.rf.accumulateRf[tIter].rfType
							== RfTypeEnum::rf_complex)
					|| (track.rf.accumulateRf[tIter].rfType
							== RfTypeEnum::rf_slide)
					|| (track.rf.accumulateRf[tIter].rfType
							== RfTypeEnum::rf_sweep)
					|| (track.rf.accumulateRf[tIter].rfType
							== RfTypeEnum::rf_sweepsin)
					|| (track.rf.accumulateRf[tIter].rfType
							== RfTypeEnum::rf_sweeptriangle))
			{

				for (lIter = 0;
						lIter
								< library.Emitters[returnIDsAllSameBand[j]].rf.rfCount;
						lIter++)
				{

					if (library.Emitters[returnIDsAllSameBand[j]].rf.rfRanges[lIter].rfLower
							< library.Emitters[returnIDsAllSameBand[j]].rf.rfRanges[lIter].rfUpper)
					{

						RFComplex =
								libraryFindOverlapRegionPercentage(
										library.Emitters[returnIDsAllSameBand[j]].rf.rfRanges[lIter].rfLower,
										library.Emitters[returnIDsAllSameBand[j]].rf.rfRanges[lIter].rfUpper,
										track.rf.accumulateRf[tIter].rfLow,
										track.rf.accumulateRf[tIter].rfHigh);

						if (RFComplex > RFComplexPrevious)
						{
							RFComplexPrevious = RFComplex;
						}
					}
					else
					{
						//do not match
					}
				}
			}
			if ((track.rf.accumulateRf[tIter].rfType == RfTypeEnum::rf_fixed)
					|| (track.rf.accumulateRf[tIter].rfType
							== RfTypeEnum::rf_jump)
					|| (track.rf.accumulateRf[tIter].rfType
							== RfTypeEnum::rf_switch))
			{

				rfSpotCount++;
				for (lIter = 0;
						lIter
								< library.Emitters[returnIDsAllSameBand[j]].rf.rfCount;
						lIter++)
				{

					if ((library.Emitters[returnIDsAllSameBand[j]].rf.rfRanges[lIter].rfLower
							<= track.rf.accumulateRf[tIter].rfLow)
							&& (library.Emitters[returnIDsAllSameBand[j]].rf.rfRanges[lIter].rfUpper
									>= track.rf.accumulateRf[tIter].rfLow))
					{

						RFMatched++;
						break;
					}
				}
			}
		}
		//assign rf range score and type score
		if (RFComplexPrevious > 0)
		{
			scoresTotal[j].RFScore = (F32) (RFComplexPrevious / 100)
					* RFConstant;
			scoresTotal[j].RFtypeScore = RFTypeConstant;
		}
		//assign rf spot score and type score
		if (RFMatched > 0)
		{
			if (((RFMatched / rfSpotCount) * RFConstant)
					> scoresTotal[j].RFScore)
			{

				scoresTotal[j].RFScore = (RFMatched / rfSpotCount) * RFConstant;
				scoresTotal[j].RFtypeScore = RFTypeConstant;
			}
		}

		PWMatched = 0;
		pwSpotCount = 0;
		//calculate pw value score for this emitter
		for (tIter = 0; tIter < track.pw.accumulatePw.size(); tIter++)
		{

			pwSpotCount++;
			for (lIter = 0;
					lIter < library.Emitters[returnIDsAllSameBand[j]].pw.pwCount;
					lIter++)
			{

				if ((library.Emitters[returnIDsAllSameBand[j]].pw.pwRanges[lIter].pwLower
						<= track.pw.accumulatePw[tIter].pwLow)
						&& (library.Emitters[returnIDsAllSameBand[j]].pw.pwRanges[lIter].pwUpper
								>= track.pw.accumulatePw[tIter].pwLow))
				{

					PWMatched++;
					break;
				}
			}
		}
		//assign pw value score to this iteration library emitter
		if (PWMatched > 0)
		{
			scoresTotal[j].PWScore = (PWMatched / pwSpotCount) * PWConstant;
			//assign pw type score to this iteration library emitter
			scoresTotal[j].PWtypeScore = PWTypeConstant;
		}
		//assign lpi pmop score to this iteration library emitter
		if ((track.intrapulse.pmopPresent == true)
				&& (library.Emitters[returnIDsAllSameBand[j]].lpi.PMOP == 1))
		{
			scoresTotal[j].LpiScore = lpiConstant;
		}
		//assign lpi fmop score to this iteration library emitter
		else if ((track.intrapulse.fmopPresent == true)
				&& (library.Emitters[returnIDsAllSameBand[j]].lpi.FMOP == 1))
		{
			scoresTotal[j].LpiScore = lpiConstant;
		}
		//assign lpi amop score to this iteration library emitter
		else if ((track.intrapulse.amopPresent == true)
				&& (library.Emitters[returnIDsAllSameBand[j]].lpi.AMOP == 1))
		{
			scoresTotal[j].LpiScore = lpiConstant;
		}
		libraryCalculateTotal(&scoresTotal[j]);
		//printf("MSC: %f\n", scoresTotal[j].Total );

		scoresTotal[j].ID = returnIDsAllSameBand[j];
		scoresTotal[j].emitterBatchId =
				library.Emitters[scoresTotal[j].ID].batchNumber;
	}
	//Top 3 scorers are copied
	libraryCopyTopScores(scoresTotal, track.libmatch, TotalCount);
}

Word32 libraryRemoveDifferentBandsFromArray(TrackDefaultClass &track,
		LibraryMetaClass &libraryMeta, UWord16 outputArr[], UWord16 arr[],
		UWord32 size)
{
	UWord8 bandOfTrack = track.rf.getBandCodeLegacy();
	UWord32 i = 0;
	UWord32 countOfOutput = 0;
	for (i = 0; i < size; i++)
	{
		if (libraryMeta.rfBandExistsInLibraryFlags[arr[i]][bandOfTrack] == 1)
		{
			outputArr[countOfOutput] = arr[i];
			countOfOutput++;
		}
	}
	return countOfOutput;
}

void libraryCountTrackPriTypes(TrackPriClass &pri)
{

	uint8_t i = 0;

	pri.accumulatePriFixedCount = 0;
	pri.accumulatePriDwellSwitchCount = 0;
	pri.accumulatePriStaggerCount = 0;
	pri.accumulatePriJitterCount = 0;
	pri.accumulatePriSlidingCount = 0;
	pri.accumulatePriComplexCount = 0;
	pri.accumulatePriDiffToaCount = 0;

	for (i = 0; i < pri.accumulatePri.size(); i++)
	{
		//fixed pri is a single dwell
		if (pri.accumulatePri[i].priType == PriTypeEnum::pri_fixed)
		{
			pri.accumulatePriFixedCount++;
			pri.accumulatePriDwellSwitchCount++;
		}
		//fixed pri is a single dwell
		else if (pri.accumulatePri[i].priType == PriTypeEnum::pri_dwellswitch)
		{
			pri.accumulatePriFixedCount++;
			pri.accumulatePriDwellSwitchCount++;
		}
		else if (pri.accumulatePri[i].priType == PriTypeEnum::pri_stagger)
			pri.accumulatePriStaggerCount++;
		else if (pri.accumulatePri[i].priType == PriTypeEnum::pri_jitter)
			pri.accumulatePriJitterCount++;
		else if (pri.accumulatePri[i].priType == PriTypeEnum::pri_sliding)
			pri.accumulatePriSlidingCount++;
		else if (pri.accumulatePri[i].priType == PriTypeEnum::pri_complex)
			pri.accumulatePriComplexCount++;
		else if (pri.accumulatePri[i].priType == PriTypeEnum::pri_difftoa)
			pri.accumulatePriDiffToaCount++;
	}
	//resovle conflict between fixed and dwell pri types
	if ((pri.accumulatePriDwellSwitchCount > 0)
			|| (pri.accumulatePriFixedCount > 0))
	{
		if ((pri.accumulatePriDwellSwitchCount > 1)
				|| (pri.accumulatePriFixedCount > 1))
			pri.accumulatePriFixedCount = 0;
		//else
		//	pri.accumulatePriDwellSwitchCount = 0;
	}
}
void libraryCountTrackRfTypes(TrackRfClass &rf)
{

	uint8_t i = 0;

	rf.accumulateFixedRfCount = 0;
	rf.accumulateAgileRfCount = 0;
	rf.accumulateSwitchRfCount = 0;
	rf.accumulateJumpRfCount = 0;
	rf.accumulateComplexRfCount = 0;

	for (i = 0; i < rf.accumulateRf.size(); i++)
	{
		//fixed Rf is a single dwell
		if (rf.accumulateRf[i].rfType == RfTypeEnum::rf_fixed)
		{
			rf.accumulateFixedRfCount++;
			rf.accumulateSwitchRfCount++;
		}
		//fixed Rf is a single dwell
		else if (rf.accumulateRf[i].rfType == RfTypeEnum::rf_switch)
		{
			rf.accumulateFixedRfCount++;
			rf.accumulateSwitchRfCount++;
		}
		else if (rf.accumulateRf[i].rfType == RfTypeEnum::rf_jump)
		{
			rf.accumulateJumpRfCount++;
		}
		else if (rf.accumulateRf[i].rfType == RfTypeEnum::rf_complex)
		{
			rf.accumulateComplexRfCount++;
		}
		else if (rf.accumulateRf[i].rfType == RfTypeEnum::rf_agile)
			rf.accumulateAgileRfCount++;
	}
	//resovle conflict between fixed and dwell Rf types
	if ((rf.accumulateSwitchRfCount > 0) || (rf.accumulateFixedRfCount > 0))
	{
		if ((rf.accumulateSwitchRfCount > 1) || (rf.accumulateFixedRfCount > 1))
			rf.accumulateFixedRfCount = 0;
		//else
		//	rf.accumulateRfDwellSwitchCount = 0;
	}
}
//add function to return pri ranges for evaluation
uint16_t libraryNumberOfPri(TrackPriClass &pri, TrackLibraryMatchClass &match)
{

	uint16_t number = 0, i = 0, j = 0;

	if (pri.accumulatePriFixedCount > 0)
	{
		number = number + 1;
	}
	if (pri.accumulatePriDwellSwitchCount > 0)
	{
		for (i = 0; i < pri.accumulatePri.size(); i++)
		{
			if ((pri.accumulatePri[i].priType == PriTypeEnum::pri_fixed)
					|| (pri.accumulatePri[i].priType
							== PriTypeEnum::pri_dwellswitch))
			{
				number = number + 1;
			}
		}
	}
	if (pri.accumulatePriStaggerCount > 0)
	{
		for (i = 0; i < pri.accumulatePri.size(); i++)
		{
			if (pri.accumulatePri[i].priType == PriTypeEnum::pri_stagger)
			{
				number = number + 1;
			}
		}
	}
	if (pri.accumulatePriDiffToaCount > 0)
	{
		for (i = 0; i < pri.accumulatePri.size(); i++)
		{
			if (pri.accumulatePri[i].priType == PriTypeEnum::pri_difftoa)
			{
				number = number + 1;
			}
		}
	}
	if (pri.accumulatePriJitterCount > 0)
	{
		for (i = 0; i < pri.accumulatePri.size(); i++)
		{
			if (pri.accumulatePri[i].priType == PriTypeEnum::pri_jitter)
			{
				j = (pri.accumulatePri[i].priHigh / 100)
						- (pri.accumulatePri[i].priLow / 100) + 1;
				number = number + j;
			}
		}
	}
	if (pri.accumulatePriSlidingCount > 0)
	{
		for (i = 0; i < pri.accumulatePri.size(); i++)
		{
			if (pri.accumulatePri[i].priType == PriTypeEnum::pri_sliding)
			{
				j = (pri.accumulatePri[i].priHigh / 100)
						- (pri.accumulatePri[i].priLow / 100) + 1;
				number = number + j;
			}
		}
	}
	if (pri.accumulatePriComplexCount > 0)
	{
		for (i = 0; i < pri.accumulatePri.size(); i++)
		{
			if (pri.accumulatePri[i].priType == PriTypeEnum::pri_complex)
			{
				j = (pri.accumulatePri[i].priHigh / 100)
						- (pri.accumulatePri[i].priLow / 100) + 1;
				number = number + j;
			}
		}
	}
	//simple diff toas are not counted for now
	return number;
}

//-----------------------------------------------------------------------------------------------------------

uint16_t libraryNumberOfRf(TrackRfClass &rf)
{
	rf.accumulateRfCount = rf.accumulateRf.size();
	return rf.accumulateRfCount;
}

uint16_t libraryNumberOfPw(TrackPwClass &pw)
{
	pw.accumulatePwCount = pw.accumulatePw.size();
	return pw.accumulatePwCount;
}

void libraryScoreInit(vector<LibraryMatchScore> &scoresTotal, Word32 number)
{
	Word32 i = 0;
	for (i = 0; i < number; i++)
	{
		scoresTotal[i].ID = 0;
		scoresTotal[i].PRIScore = 0;
		scoresTotal[i].PRItypeScore = 0;
		scoresTotal[i].PWScore = 0;
		scoresTotal[i].PWtypeScore = 0;
		scoresTotal[i].RFScore = 0;
		scoresTotal[i].RFtypeScore = 0;
		scoresTotal[i].LpiScore = 0;
		scoresTotal[i].Total = 0;
		scoresTotal[i].dwellCount = 0;
		scoresTotal[i].staggerCount = 0;
		scoresTotal[i].slidingCount = 0;
		scoresTotal[i].jitterCount = 0;
		scoresTotal[i].complexCount = 0;
		scoresTotal[i].diffToaCount = 0;
		scoresTotal[i].emitterBatchId = 0;
	}
}

void libraryCalculateTotal(LibraryMatchScore *sc)
{
	sc->Total = sc->PRIScore + sc->PRItypeScore + sc->PWScore + sc->PWtypeScore
			+ sc->RFScore + sc->RFtypeScore + sc->LpiScore;

	if (sc->Total > 100)
	{
		sc->Total *= 1;
	}
}

void libraryTopThreeIds(vector<LibraryMatchScore> &sc, uint32_t n,
		vector<uint8_t> &indexes)
{

	Word32 first, second, third;
	indexes[0] = 0;
	indexes[1] = 0;
	indexes[2] = 0;
	third = first = second = INT_MIN;
	for (uint32_t i = 0; i < n; i++)
	{
		// If current element is smaller than first
		if (sc[i].Total > first)
		{
			third = second;
			second = first;
			first = sc[i].Total;
			indexes[2] = indexes[1];
			indexes[1] = indexes[0];
			indexes[0] = i;
		}

		// If arr[i] is in between first and second then update second
		else if (sc[i].Total > second)
		{
			third = second;
			second = sc[i].Total;
			indexes[2] = indexes[1];
			indexes[1] = i;
		}

		else if (sc[i].Total > third)
		{
			third = sc[i].Total;
			indexes[2] = i;
		}
	}
}

//review this!!!
void libraryCopyTopScores(vector<LibraryMatchScore> &sc,
		TrackLibraryMatchClass &match, UWord32 n)
{
	UWord32 i = 0, nonZeroIndexes = 0;
	vector<uint8_t> indexes;
	match.last.clear();
	indexes.resize(n, 0);

	for (i = 0; i < n; i++)
	{
		if (sc[i].Total > 0)
		{
			nonZeroIndexes++;
		}
	}

	if (nonZeroIndexes <= 0)
	{
		return;
	}
	libraryTopThreeIds(sc, n, indexes);
	if (nonZeroIndexes < 3)
	{
		if (nonZeroIndexes < 2)
		{
			match.last.highestMatchListCount = 1;
			match.last.highestMatchList[0] = sc[indexes[0]];
		}
		else
		{
			match.last.highestMatchListCount = 2;

			match.last.highestMatchList[0] = sc[indexes[0]];
			match.last.highestMatchList[1] = sc[indexes[1]];
		}
	}
	else
	{
		match.last.highestMatchListCount = 3;
		match.last.highestMatchList[0] = sc[indexes[0]];
		match.last.highestMatchList[1] = sc[indexes[1]];
		match.last.highestMatchList[2] = sc[indexes[2]];
		//memcpy(&extEmitter->TopScorers[0], &sc[indexes[0]], sizeof(LibraryMatchScore));
		//memcpy(&extEmitter->TopScorers[1], &sc[indexes[1]], sizeof(LibraryMatchScore));
		//memcpy(&extEmitter->TopScorers[2], &sc[indexes[2]], sizeof(LibraryMatchScore));
	}
}

//where is this called from?
void libraryRequestMatch(TrackDefaultClass &track)
{
	//track.libmatch.current.flagKnownLibrary = 0;
	//track.libmatch.current.flagKnownPartialLibrary = 0;
	track.libmatch.flagRequestLibraryMatch = 1;
	//track.libmatch.current.flagAerialOnlyLibrary = 0;
	//track.libmatch.current.flagTrackingOnlyLibrary = 0;
	//track.libmatch.current.flagScanningOnlyLibrary = 0;
	//track.libmatch.current.flagMissileOnlyLibrary = 0;
	//track.libmatch.current.highestMatchListCount = 0;
	//track.libmatch.timeIterCountLibMatchExec = 0;
}

void libraryRequestForceUnknown(TrackDefaultClass &track)
{
	track.libmatch.flagSetUnknownLibrary = 1;
}

void libraryRequestResetTrackParameters(TrackDefaultClass &track)
{
	track.libmatch.flagRequestResetTrackParameters = 1;
}

void libraryResetTrackParameters(TrackDefaultClass &track)
{
	if (track.libmatch.flagRequestResetTrackParameters == 1)
	{
		track.libmatch.flagRequestResetTrackParameters = 0;
		track.pri.accumulatePri.clear();
		track.pw.accumulatePw.clear();
		track.rf.accumulateRf.clear();

		track.pri.accumulatePriComplexCount = 0;
		track.pri.accumulatePriDiffToaCount = 0;
		track.pri.accumulatePriDwellSwitchCount = 0;
		track.pri.accumulatePriFixedCount = 0;
		track.pri.accumulatePriJitterCount = 0;
		track.pri.accumulatePriSlidingCount = 0;
		track.pri.accumulatePriStaggerCount = 0;
		track.rf.accumulateFixedRfCount = 0;
		track.rf.accumulateAgileRfCount = 0;
		track.rf.accumulateSwitchRfCount = 0;
		track.rf.accumulateJumpRfCount = 0;
		track.rf.accumulateComplexRfCount = 0;
	}
}

void libraryInitStructure(LibraryStruct &library)
{
	memset(&library, 0, sizeof(LibraryStruct));
}

void libraryLoading(LibraryStruct &library, LibraryMetaClass &libraryMeta,
		CfdLibraryStruct &cfdLibrary, CfdLibraryMetaClass &cfdLibraryMeta,
		CfCard &cfCard)
{
	string libraryPath;
	string libraryKeyPath;
	string libraryKeyDecryptPath;
	string libraryTlePath;
	string libraryCfdPath;
	Word8 filenameWithPath[100] =
	{ ' ' };
	Word8 keyFilenameWithPath[100] =
	{ ' ' };
	Word8 cfdFilenameWithPath[100] =
	{ ' ' };
	uint8_t status = 0;
	uint8_t *password, j = 0, i = 0;
	uint32_t crc = 0, m = 0, k = 0, l = 0;
	uint32_t rk[60];
	Word8 filename[100] =
	{ ' ' };
	SHA256_CTX ctx;
	uint8_t *buffer2;
	LibraryStruct *tl;
	CfdLibraryStruct *cfd;
	uint8_t decryptionKeyName[32];
	ThreatLibraryUnencryptedHeader *tlHeader;
	UWord8 success = 0;
	if (ConstHigh::simOnActualHwFlag == 1)
	{
		cfCard.extractKeyNamesFromEmmc();
		cfCard.calculateHashOfKeysInEmmc();
		cfCard.readThreatLibraryNamesFromEmmc();

		cfCard.readCfdLibraryName();
	}
	else
	{
		cfCard.threatLibraryPresentInEmmc[0]=1;

		char* fileName = "library";
		char* kFileName = "key";
		strcpy(&cfCard.threatLibraryFilenamesInEmmc[i][0],fileName);
		strcpy(&cfCard.keyFilenamesInEmmc[i][0],kFileName);
	}
	for (i = 0; i < 6; i++)
	{
		if (cfCard.threatLibraryPresentInEmmc[i] == 1)
		{
			if (ConstHigh::simOnActualHwFlag == 1)
			{
				sprintf(filenameWithPath, "/tmp/emmc/%s.tle",
						&cfCard.threatLibraryFilenamesInEmmc[i][0]);
				sprintf(keyFilenameWithPath, "/tmp/emmc/%s.key",
						&cfCard.keyFilenamesInEmmc[i][0]);

				sprintf(cfdFilenameWithPath, "/tmp/emmc/%s.cfd",
						&cfCard.cfdLibraryName[0]);
			}
			else
			{
				//for offline set path here
				sprintf(filenameWithPath, "./input/%s.tle",
						&cfCard.threatLibraryFilenamesInEmmc[i][0]);
				sprintf(keyFilenameWithPath, "./input/%s.key",
						&cfCard.keyFilenamesInEmmc[i][0]);

				sprintf(cfdFilenameWithPath, "./input/%s.cfd",
						&cfCard.cfdLibraryName[0]);
			}
			libraryKeyPath = keyFilenameWithPath;
			libraryTlePath = filenameWithPath;
			libraryCfdPath = cfdFilenameWithPath;

			sprintf(filename, "%s", libraryCfdPath.c_str());
			status = readFileBinary(filename,
					cfdLibraryMeta.SIZE_OF_CFD_LIBRARY,
					cfdLibraryMeta.bufferForCfdLibrary);
			if (status)
			{
				cfd = (CfdLibraryStruct*) cfdLibraryMeta.bufferForCfdLibrary;
				memcpy(&cfdLibrary, cfd, sizeof(CfdLibraryStruct));
			}

			sprintf(filename, "%s", libraryKeyPath.c_str());
			//sprintf(filename, "input/key.key");
			status = readFileBinary(filename, libraryMeta.SIZE_OF_KEY,
					libraryMeta.bufferForKey);

			if (status)
			{
				password = libraryMeta.bufferForKey;
				for (j = 0; j < sizeof(decryptionKeyName); j++)
				{
					decryptionKeyName[j] = *password != 0 ? *password++ : 0;
				}

				sha256_init(&ctx);
				sha256_update(&ctx, decryptionKeyName,
						sizeof(decryptionKeyName));
				sha256_final(&ctx, (uint8_t*) &libraryMeta.keyDigest);
			}

			sprintf(filename, "%s", libraryTlePath.c_str());
			//sprintf(filename, "input/library.tle");
			status = readFileBinary(filename,
					libraryMeta.SIZE_OF_THREAT_LIBRARY,
					libraryMeta.bufferForUse1);

			if (status)
			{
				tlHeader =
						(ThreatLibraryUnencryptedHeader*) libraryMeta.bufferForUse1;
				if (tlHeader->delimeter != 0xdeadbeefbeeffeed)
				{
					Model::tlFailure = true;
					printf("TL delimeter Fail\n");
				}
				else
				{
					for (k = 0; k < 32; k++)
					{
						if (libraryMeta.keyDigest[k] != tlHeader->hash[k])
						{
							l++;
						}
					}

					if (l == 0)
					{
						crc = 0;
						printf("Key Hash Matched for TL \n");

						aesSetupDecrypt(rk, libraryMeta.bufferForKey);

						buffer2 = libraryMeta.bufferForUse1
								+ sizeof(ThreatLibraryUnencryptedHeader);
						memset(libraryMeta.bufferForUse2, 0,
								libraryMeta.SIZE_OF_THREAT_LIBRARY
										- sizeof(ThreatLibraryUnencryptedHeader));

						cbcDecryptData(buffer2, rk, tlHeader->iv,
								libraryMeta.bufferForUse2,
								libraryMeta.SIZE_OF_THREAT_LIBRARY
										- sizeof(ThreatLibraryUnencryptedHeader));

						tl = (LibraryStruct*) libraryMeta.bufferForUse2;

						for (m = 0; m < sizeof(LibraryStruct) - 5; m++)
						{
							crc +=
									(int32_t) libraryMeta.bufferForUse2[m]
											^ (int32_t) libraryMeta.bufferForUse2[m
													+ 1];
						}

						if (crc == tl->constants.CRC)
						{
							memcpy(&library, tl, sizeof(LibraryStruct));
							printf("Decrypt Success \n");
							Model::tlFailure = false;
							success = 1;
						}
						else
						{
							Model::tlFailure = true;
							printf("Decrypt Unsuccess \n");
							success = 0;
						}
					}
				}
			}
			break;
		}
	}
	if (success == 0)
	{
		Model::tlFailure = true;
	}
	if (Model::tlFailure == false)
	{

		printf("Valid Library Found (Major.Minor): %c%c.%c%c\n",
				library.header.majorVersion[0], library.header.majorVersion[1],
				library.header.minorVersion[0], library.header.minorVersion[1]);
	}
	else
	{
		loadDefaultLibrary(library, libraryMeta);
	}
	libraryHashing(library, libraryMeta);
	libraryBandQuery(library, libraryMeta);

	for (uint16_t i = 0; i < libraryMeta.numberOfEmmiters; i++)
	{
		if (library.Emitters[i].erp.erp == 0)
		{
			if (libraryMeta.rfBandExistsInLibraryFlags[i][0] == 1)
			{
				library.Emitters[i].erp.erp = 110;
			}
			else if (libraryMeta.rfBandExistsInLibraryFlags[i][1] == 1)
			{
				library.Emitters[i].erp.erp = 120;
			}
			else if (libraryMeta.rfBandExistsInLibraryFlags[i][2] == 1)
			{
				if (library.Emitters[i].pri.priType == 3)
				{
					library.Emitters[i].erp.erp = 120;
				}
				else
					library.Emitters[i].erp.erp = 110;

			}
			else if (libraryMeta.rfBandExistsInLibraryFlags[i][3] == 1)
			{

				library.Emitters[i].erp.erp = 130;
			}
		}
		for (uint16_t j = 0; j < library.Emitters[i].rf.rfCount; j++)
		{
			library.Emitters[i].rf.rfRanges[j].rfLower =
					library.Emitters[i].rf.rfRanges[j].rfLower - 10;
			library.Emitters[i].rf.rfRanges[j].rfUpper =
					library.Emitters[i].rf.rfRanges[j].rfUpper + 10;
		}
	}
	uint16_t sumLibScore = 0;
	sumLibScore += library.header.tlScores.priScore;
	sumLibScore += library.header.tlScores.priTypeScore;
	sumLibScore += library.header.tlScores.pwScore;
	sumLibScore += library.header.tlScores.pwTypeScore;
	sumLibScore += library.header.tlScores.rfScore;
	sumLibScore += library.header.tlScores.rfTypeScore;
	if (sumLibScore > 75
			&& sumLibScore > library.header.tlScores.globalMatchPercentRequired
			&& library.header.tlScores.globalMatchPercentRequired > 50)
	{
		// lib scores valid/sane
		library.header.tlScores.pwTypeScore = ConstHigh::libraryPwTypeScore;
		library.header.tlScores.rfTypeScore = ConstHigh::libraryRfTypeScore;
		// b/c both these scores are not being calculated in lib matching
	}
	else
	{
		library.header.tlScores.priScore = ConstHigh::libraryPriScore;
		library.header.tlScores.priTypeScore = ConstHigh::libraryPriTypeScore;
		library.header.tlScores.pwScore = ConstHigh::libraryPwScore;
		library.header.tlScores.pwTypeScore = ConstHigh::libraryPwTypeScore;
		library.header.tlScores.rfTypeScore = ConstHigh::libraryRfTypeScore;
		library.header.tlScores.rfScore = ConstHigh::libraryRfScore;
		library.header.tlScores.globalMatchPercentRequired =
				ConstHigh::libraryScoreDefaultThreshold;
	}

//	for (uint16_t i = 0; i < libraryMeta.numberOfEmmiters; i++)
//	{
//		if (library.Emitters[i].erp.erp == 0)
//		{
//			if (libraryMeta.rfBandExistsInLibraryFlags[i][0] == 1)
//			{
//				library.Emitters[i].erp.erp = 110;
//			}
//			else if (libraryMeta.rfBandExistsInLibraryFlags[i][1] == 1)
//			{
//				library.Emitters[i].erp.erp = 120;
//			}
//			else if (libraryMeta.rfBandExistsInLibraryFlags[i][2] == 1)
//			{
//				if(library.Emitters[i].pri.priType == 3)
//				{
//				library.Emitters[i].erp.erp = 120;
//				}
//				else
//					library.Emitters[i].erp.erp = 110;
//
//			}
//			else if (libraryMeta.rfBandExistsInLibraryFlags[i][3] == 1)
//			{
//
//				library.Emitters[i].erp.erp = 130;
//			}
//		}
//	}

	//logic to remove k band rss entry
	uint8_t deleteRssEntryFlag[256];
	memset(deleteRssEntryFlag,0,sizeof(deleteRssEntryFlag));
	uint32_t deleteS1EntryCount=0;
	for (uint16_t i = 0; i < library.constants.mdf.numOfS1; i++)
	{
//		if(library.constants.rss.s1Block[i].band==4 || library.constants.rss.s1Block[i].band==5)
//		{
//			deleteRssEntryFlag[i]=1;
//			deleteS1EntryCount++;
//		}
		library.constants.rss.s1Block[i]=library.constants.rss.s1Block[i+deleteS1EntryCount];
	}
	library.constants.mdf.numOfS1 = library.constants.mdf.numOfS1 -deleteS1EntryCount;
}
void loadDefaultLibrary(LibraryStruct &library, LibraryMetaClass &libraryMeta)
{
	library.header.LibraryID = 11;
	library.header.majorVersion[0] = 'L';
	library.header.majorVersion[1] = 'T';
	library.header.minorVersion[0] = 'D';
	library.header.minorVersion[1] = 'F';

	library.header.NumberOfThreats = 5;

	library.header.tlScores.priScore = 45;
	library.header.tlScores.priTypeScore = 25;
	library.header.tlScores.pwScore = 30;
	library.header.tlScores.pwTypeScore = 0;
	library.header.tlScores.rfTypeScore = 0;
	library.header.tlScores.rfScore = 0;
	library.header.tlScores.globalMatchPercentRequired = 73;

	for (uint16_t i = 0; i < library.header.NumberOfThreats; i++)
	{

		library.Emitters[i].TTE = 14;
		library.Emitters[i].TPR = 5;
		library.Emitters[i].platform = 3;
		library.Emitters[i].attribute = 2;
		library.Emitters[i].emitterName[0] = 141;
		library.Emitters[i].emitterName[1] = i+1;
		library.Emitters[i].modeID[0] = (uint8_t)(10);
		library.Emitters[i].modeID[0] = (uint8_t)(10);
		library.Emitters[i].emitterFunction = 0;
		library.Emitters[i].erp.erp = 110;

		library.Emitters[i].SymbolCode1 =(uint16_t)(49216);
		//pri info
		library.Emitters[i].pri.priCount = 1;
		library.Emitters[i].pri.priType = 1;
		library.Emitters[i].pri.priSpots[0].priLower = 78000;
		library.Emitters[i].pri.priSpots[0].priUpper = 82000;

		//pw info

		library.Emitters[i].pw.pwCount = 1;
		library.Emitters[i].pw.pwType = 1;
		library.Emitters[i].pw.pwRanges[0].pwLower = 120;
		library.Emitters[i].pw.pwRanges[0].pwUpper = 170;

		//rf info
		library.Emitters[i].rf.rfCount = 1;
		library.Emitters[i].rf.rfType = 1;
		library.Emitters[i].rf.rfRanges[0].rfLower = 6780;
		library.Emitters[i].rf.rfRanges[0].rfUpper = 6820;



		library.Emitters[i].arp.arpLower = 2000;
		library.Emitters[i].arp.arpLower = 3000;
		library.Emitters[i].arp.illuminationTime = 100;
		library.Emitters[i].arp.arpType = 1;
	}
	library.Emitters[1].SymbolCode1 = (uint16_t)(49264);
			//pri info

	library.Emitters[1].pri.priSpots[0].priLower = 100000;
	library.Emitters[1].pri.priSpots[0].priUpper = 110000;

	//pw info

	library.Emitters[1].pw.pwRanges[0].pwLower = 1500;
	library.Emitters[1].pw.pwRanges[0].pwUpper = 1800;

	//rf info

	library.Emitters[1].rf.rfRanges[0].rfLower = 16780;
	library.Emitters[1].rf.rfRanges[0].rfUpper = 16820;

	library.Emitters[1].arp.arpLower = 50000;
	library.Emitters[1].arp.arpLower = 60000;

	library.Emitters[2].SymbolCode1 = (uint16_t)(49328);
				//pri info

	library.Emitters[2].pri.priSpots[0].priLower = 10000;
	library.Emitters[2].pri.priSpots[0].priUpper = 11000;

	//pw info

	library.Emitters[2].pw.pwRanges[0].pwLower = 150;
	library.Emitters[2].pw.pwRanges[0].pwUpper = 200;

	//rf info

	library.Emitters[2].rf.rfRanges[0].rfLower = 11580;
	library.Emitters[2].rf.rfRanges[0].rfUpper = 11620;

	library.Emitters[2].arp.arpLower = 5000;
	library.Emitters[2].arp.arpLower = 6000;

	library.Emitters[3].SymbolCode1 = (uint16_t)(49392);
					//pri info

	library.Emitters[3].pri.priSpots[0].priLower = 20000;
	library.Emitters[3].pri.priSpots[0].priUpper = 21000;

	//pw info

	library.Emitters[3].pw.pwRanges[0].pwLower = 600;
	library.Emitters[3].pw.pwRanges[0].pwUpper = 700;

	//rf info

	library.Emitters[3].rf.rfRanges[0].rfLower = 3580;
	library.Emitters[3].rf.rfRanges[0].rfUpper = 3620;

	library.Emitters[3].arp.arpLower = 8000;
	library.Emitters[3].arp.arpLower = 9000;

	library.Emitters[4].SymbolCode1 = (uint16_t)(49440);
					//pri info

	library.Emitters[4].pri.priSpots[0].priLower = 44000;
	library.Emitters[4].pri.priSpots[0].priUpper = 45000;

	//pw info

	library.Emitters[4].pw.pwRanges[0].pwLower = 850;
	library.Emitters[4].pw.pwRanges[0].pwUpper = 900;

	//rf info

	library.Emitters[4].rf.rfRanges[0].rfLower = 5520;
	library.Emitters[4].rf.rfRanges[0].rfUpper = 6000;

	library.Emitters[4].arp.arpLower = 8000;
	library.Emitters[4].arp.arpLower = 9000;

}
int32_t libraryHashing(LibraryStruct &library, LibraryMetaClass &libraryMeta)
{
	printf("Hashing PRI \n");
	libraryPriHashing(library, libraryMeta);
	printf("Hashing PW \n");
	libraryPwHashing(library, libraryMeta);
	printf("Hashing RF \n");
	libraryRfHashing(library, libraryMeta);
	printf("Hashing Complete! \n");

	return 1;
}

void libraryPriHashing(LibraryStruct &threatLibrary,
		LibraryMetaClass &libraryMeta)
{
	uint16_t id;
	uint32_t i = 0, j = 0, k = 0;
	uint32_t lower = 0, upper = 0, dif = 0, start;
	//printf("Initializing PRI arrays \n");
	libraryInitPriArray(libraryMeta);
	libraryMeta.numberOfEmmiters = threatLibrary.header.NumberOfThreats;
	for (i = 0; i < libraryMeta.numberOfEmmiters; i++)
	{
		k = 0;
		//find number of PRIs
		libraryMeta.numberofPris = threatLibrary.Emitters[i].pri.priCount;
		for (k = 0; k < libraryMeta.numberofPris; k++)
		{
			lower = threatLibrary.Emitters[i].pri.priSpots[k].priLower;
			upper = threatLibrary.Emitters[i].pri.priSpots[k].priUpper;
			if ((lower <= 2000000) && (upper <= 2000000))
			{
				lower = lower / 100;
				upper = upper / 100;
				if (upper == lower)
				{
					libraryMeta.PRIs[upper][libraryMeta.PRIcount[upper]] = i;
					libraryMeta.PRIcount[upper]++;
				}
				else
				{
					j = 0;
					dif = labs((int32_t)(upper - lower));
					
					start = lower;
					id = i;
					for (j = 0; j <= dif; j++)
					{
						libraryMeta.PRIs[start + j][libraryMeta.PRIcount[start
								+ j]] = id;
						libraryMeta.PRIcount[start + j]++;
					}
				}
			}
		}

	}
	//printf("PRI hashing complete \n");
}

void libraryRfHashing(LibraryStruct &threatLibrary,
		LibraryMetaClass &libraryMeta)
{
	uint16_t id;
	uint32_t i = 0, j = 0, k = 0;
	uint32_t dif = 0, start;
	uint8_t lower = 0, upper = 0;
	//printf("Initializing RF arrays \n");
	libraryInitRfArray(libraryMeta);
	libraryMeta.numberOfEmmiters = threatLibrary.header.NumberOfThreats;
	for (i = 0; i < libraryMeta.numberOfEmmiters; i++)
	{
		//find nnumber of RFs
		libraryMeta.numberofRFs = threatLibrary.Emitters[i].rf.rfCount;
		for (k = 0; k < libraryMeta.numberofRFs; k++)
		{
			lower = (uint8_t) (threatLibrary.Emitters[i].rf.rfRanges[k].rfLower
					/ 100);
			upper = (uint8_t) (threatLibrary.Emitters[i].rf.rfRanges[k].rfUpper
					/ 100);
			if (upper == lower)
			{
				libraryMeta.RFs[upper][libraryMeta.RFcount[upper]] = i;
				libraryMeta.RFcount[upper]++;
			}
			else
			{
				j = 0;
				dif = upper - lower;
				start = lower;
				id = i;
				for (j = 0; j <= dif; j++)
				{
					libraryMeta.RFs[start + j][libraryMeta.RFcount[start + j]] =
							id;
					libraryMeta.RFcount[start + j]++;
				}
			}
		}
		k = 0;
	}
	//printf("RF hashing complete \n");
}

void libraryBandQuery(LibraryStruct &threatLibrary,
		LibraryMetaClass &libraryMeta)
{
	UWord32 i = 0;
	for (i = 0; i < libraryMeta.numberOfEmmiters; i++)
	{
		libraryRFBandExists(threatLibrary.Emitters[i].rf, i, libraryMeta);
	}
}

void libraryRFBandExists(RF rf, uint32_t iterator,
		LibraryMetaClass &libraryMeta)
{
	uint32_t numberOfRfRangesInEmitter = 0;
	uint32_t i = 0;
	numberOfRfRangesInEmitter = rf.rfCount;
	uint32_t cdBandLow = 500, cdBandHigh = 2000;
	uint32_t eBandLow = 2000, eBandHigh = 6000;
	uint32_t gBandLow = 6000, gBandHigh = 10000;
	uint32_t iBandLow = 10000, iBandHigh = 14000;
	uint32_t jBandLow = 14000, jBandHigh = 18000;
	uint32_t kBandLow = 18000, kBandHigh = 30000;

	for (i = 0; i < numberOfRfRangesInEmitter; i++)
	{
		if ((rf.rfRanges[i].rfLower > eBandLow)
				&& (rf.rfRanges[i].rfLower <= eBandHigh))
		{
			libraryMeta.rfBandExistsInLibraryFlags[iterator][0] = 1;
		}
		if ((rf.rfRanges[i].rfLower > gBandLow)
				&& (rf.rfRanges[i].rfLower <= gBandHigh))
		{
			libraryMeta.rfBandExistsInLibraryFlags[iterator][1] = 1;
		}
		if ((rf.rfRanges[i].rfLower > iBandLow)
				&& (rf.rfRanges[i].rfLower <= iBandHigh))
		{
			libraryMeta.rfBandExistsInLibraryFlags[iterator][2] = 1;
		}
		if ((rf.rfRanges[i].rfLower > jBandLow)
				&& (rf.rfRanges[i].rfLower <= jBandHigh))
		{
			libraryMeta.rfBandExistsInLibraryFlags[iterator][3] = 1;
		}
		if ((rf.rfRanges[i].rfLower > cdBandLow)
				&& (rf.rfRanges[i].rfLower <= cdBandHigh))
		{
			libraryMeta.rfBandExistsInLibraryFlags[iterator][4] = 1;
		}
		if ((rf.rfRanges[i].rfLower > kBandLow)
				&& (rf.rfRanges[i].rfLower <= kBandHigh))
		{
			libraryMeta.rfBandExistsInLibraryFlags[iterator][5] = 1;
		}

		if ((rf.rfRanges[i].rfUpper > eBandLow)
				&& (rf.rfRanges[i].rfUpper <= eBandHigh))
		{
			libraryMeta.rfBandExistsInLibraryFlags[iterator][0] = 1;
		}
		if ((rf.rfRanges[i].rfUpper > gBandLow)
				&& (rf.rfRanges[i].rfUpper <= gBandHigh))
		{
			libraryMeta.rfBandExistsInLibraryFlags[iterator][1] = 1;
		}
		if ((rf.rfRanges[i].rfUpper > iBandLow)
				&& (rf.rfRanges[i].rfUpper <= iBandHigh))
		{
			libraryMeta.rfBandExistsInLibraryFlags[iterator][2] = 1;
		}
		if ((rf.rfRanges[i].rfUpper > jBandLow)
				&& (rf.rfRanges[i].rfUpper <= jBandHigh))
		{
			libraryMeta.rfBandExistsInLibraryFlags[iterator][3] = 1;
		}
		if ((rf.rfRanges[i].rfUpper > cdBandLow)
				&& (rf.rfRanges[i].rfUpper <= cdBandHigh))
		{
			libraryMeta.rfBandExistsInLibraryFlags[iterator][4] = 1;
		}
		if ((rf.rfRanges[i].rfUpper > kBandLow)
				&& (rf.rfRanges[i].rfUpper <= kBandHigh))
		{
			libraryMeta.rfBandExistsInLibraryFlags[iterator][5] = 1;
		}
	}
}

void libraryPwHashing(LibraryStruct &threatLibrary,
		LibraryMetaClass &libraryMeta)
{
	uint16_t id;
	uint32_t i = 0, j = 0, k = 0;
	uint32_t dif = 0, start;
	uint8_t lower = 0, upper = 0;
	//printf("Initializing PW arrays \n");
	libraryInitPwArray(libraryMeta);
	libraryMeta.numberOfEmmiters = threatLibrary.header.NumberOfThreats;
	for (i = 0; i < libraryMeta.numberOfEmmiters; i++)
	{
		//find nnumber of PWs
		libraryMeta.numberofPWs = threatLibrary.Emitters[i].pw.pwCount;
		for (k = 0; k < libraryMeta.numberofPWs; k++)
		{
			lower = (uint8_t) (threatLibrary.Emitters[i].pw.pwRanges[k].pwLower
					/ 100);
			upper = (uint8_t) (threatLibrary.Emitters[i].pw.pwRanges[k].pwUpper
					/ 100);
			if (upper == lower)
			{
				libraryMeta.PWs[upper][libraryMeta.PWcount[upper]] = i;
				libraryMeta.PWcount[upper]++;
			}
			else
			{
				j = 0;
				dif = upper - lower;
				start = lower;
				id = i;
				for (j = 0; j <= dif; j++)
				{
					libraryMeta.PWs[start + j][libraryMeta.PWcount[start + j]] =
							id;
					libraryMeta.PWcount[start + j]++;
				}
			}
		}
		k = 0;
	}
	//printf("PW hashing complete \n");
}

void libraryInitPriArray(LibraryMetaClass &libraryMeta)
{
	memset(libraryMeta.PRIs, 0, sizeof(libraryMeta.PRIs));
	memset(libraryMeta.PRIcount, 0, sizeof(libraryMeta.PRIcount));
}

void libraryInitRfArray(LibraryMetaClass &libraryMeta)
{
	memset(libraryMeta.RFs, 0, sizeof(libraryMeta.RFs));
	memset(libraryMeta.RFcount, 0, sizeof(libraryMeta.RFcount));
	memset(libraryMeta.rfBandExistsInLibraryFlags, 0,
			sizeof(libraryMeta.rfBandExistsInLibraryFlags));
}
void libraryInitPwArray(LibraryMetaClass &libraryMeta)
{
	memset(libraryMeta.PWs, 0, sizeof(libraryMeta.PWs));
	memset(libraryMeta.PWcount, 0, sizeof(libraryMeta.PWcount));
}

void libraryInitHashingStructure(LibraryMetaClass &libraryMeta)
{
	libraryInitPriArray(libraryMeta);
	libraryInitRfArray(libraryMeta);
	libraryInitPwArray(libraryMeta);
}

uint32_t libraryPriLookup(vector<uint32_t> &spots, uint32_t num, uint16_t IDs[],
		LibraryMetaClass &libraryMeta)
{

	uint32_t i = 0, j = 0, count = 0, cur = 0;
	uint32_t countOfPRIs = 0, *PRIret;

	for (i = 0; i < num; i++)
	{
		if (count >= 500)
		{
			break;
		}
		else
		{
			cur = spots[i];
			countOfPRIs = libraryMeta.PRIcount[cur];
			PRIret = libraryMeta.PRIs[cur];
			for (j = 0; j < countOfPRIs; j++)
			{
				IDs[count] = PRIret[j];
				count++;
			}
		}
		j = 0;
	}
	return count;
}

uint32_t libraryRfLookup(circular_buffer<LimitRfClass> &spots, uint32_t num,
		uint16_t IDs[], LibraryMetaClass &libraryMeta)
{

	uint32_t i = 0, j = 0, count = 0;
	uint8_t cur = 0;
	uint16_t countOfRF = 0, *RFret;
	for (i = 0; i < num; i++)
	{
		if (count >= 200)
		{
			break;
		}
		else
		{
			cur = (uint8_t) (spots[i].rfLow / 100);
			//printf("I am here %d\n",cur);
			countOfRF = libraryMeta.RFcount[cur];
			RFret = libraryMeta.RFs[cur];
			for (j = 0; j < countOfRF; j++)
			{
				IDs[count] = RFret[j];
				count++;
			}
		}
		j = 0;
	}
	return count;
}

uint32_t libraryPwLookup(circular_buffer<LimitPwClass> &spots, uint32_t num,
		uint16_t IDs[], LibraryMetaClass &libraryMeta)
{

	uint32_t i = 0, j = 0, count = 0;
	uint8_t cur = 0;
	uint16_t countOfPW = 0, *PWret;
	for (i = 0; i < num; i++)
	{
		if (count >= 200)
		{
			break;
		}
		else
		{
			cur = (uint8_t) (spots[i].pwLow / 100);
			countOfPW = libraryMeta.PWcount[cur];
			PWret = libraryMeta.PWs[cur];
			for (j = 0; j < countOfPW; j++)
			{
				IDs[count] = PWret[j];
				count++;
			}
		}
		j = 0;
	}
	return count;
}

// A utility function to swap two elements
void swap(uint16_t *a, uint16_t *b)
{
	uint16_t t = *a;
	*a = *b;
	*b = t;
}

/* This function takes last element as pivot, places
 the pivot element at its correct position in sorted
 array, and places all smaller (smaller than pivot)
 to left of pivot and all greater elements to right
 of pivot */
int16_t partition(uint16_t arr[], int16_t low, int16_t high)
{
	uint16_t pivot = arr[high]; // pivot
	int16_t i = (low - 1); // Index of smaller element

	for (int16_t j = low; j <= high - 1; j++)
	{
		// If current element is smaller than the pivot
		if (arr[j] < pivot)
		{
			i++; // increment index of smaller element
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return (i + 1);
}

/* The main function that implements QuickSort
 arr[] --> Array to be sorted,
 low --> Starting index,
 high --> Ending index */
void quickSort(uint16_t arr[], int16_t low, int16_t high)
{
	if (low < high)
	{
		/* pi is partitioning index, arr[p] is now
		 at right place */
		int16_t pi = partition(arr, low, high);

		// Separately sort elements before
		// partition and after partition
		quickSort(arr, low, pi - 1);
		quickSort(arr, pi + 1, high);
	}
}

Word32 libraryGetMax(UWord16 arr[], UWord32 n)
{
	uint16_t mx = arr[0];
	uint16_t i = 0;
	for (i = 1; i < n; i++)
		if (arr[i] > mx)
			mx = arr[i];
	return mx;
}

// A function to do counting sort of arr[] according to
// the digit represented by exp.
void libraryCountSort(UWord16 arr[], UWord32 n, UWord32 exp)
{
	UWord16 output[n]; // output array
	UWord16 count[10] =
	{ 0 };
	UWord16 i;

	// Store count of occurrences in count[]
	for (i = 0; i < n; i++)
		count[(arr[i] / exp) % 10]++;

	// Change count[i] so that count[i] now contains actual
	//  position of this digit in output[]
	for (i = 1; i < 10; i++)
		count[i] += count[i - 1];

	// Build the output array
	for (i = n - 1; i >= 0; i--)
	{
		output[count[(arr[i] / exp) % 10] - 1] = arr[i];
		count[(arr[i] / exp) % 10]--;
	}

	// Copy the output array to arr[], so that arr[] now
	// contains sorted numbers according to current digit
	for (i = 0; i < n; i++)
		arr[i] = output[i];
}

// The main function to that sorts arr[] of size n using
// Radix Sort
void libraryRadixSort(UWord16 arr[], UWord32 n)
{
	// Find the maximum number to know number of digits
	UWord16 m = libraryGetMax(arr, n);
	UWord16 exp;
	// Do counting sort for every digit. Note that instead
	// of passing digit number, exp is passed. exp is 10^i
	// where i is current digit number
	for (exp = 1; m / exp > 0; exp *= 10)
		libraryCountSort(arr, n, exp);
}

// A utility function to print an array
void libraryPrintArray(UWord16 arr[], UWord32 n)
{
	UWord32 i;
	for (i = 0; i < n; i++)
	{
		printf("%d ", arr[i]);
	}
}

Word32 libraryRemoveDuplicateFromArray(UWord16 arr[], UWord32 size)
{
	UWord32 i, j, k, l, prev;
	prev = size;
	//printf("\nArray with Unique list  : ");
	for (i = 0; i < size; i++)
	{
		for (j = i + 1; j < size;)
		{
			if (arr[j] == arr[i])
			{
				for (k = j; k < size; k++)
				{
					arr[k] = arr[k + 1];
				}
				size--;
			}
			else
			{
				j++;
			}
		}
	}
	//for (i = 0; i < size; i++)
	//{
	// printf("%d ", arr[i]);
	//}
	l = prev - size;
	for (i = 0; i <= l; i++)
	{
		arr[size + i] = 0;
	}
	return size;
}

// Function prints union of arr1[] and arr2[]
//m is the number of elements in arr1[]
//n is the number of elements in arr2[]
Word32 libraryUnion(UWord16 arr1[], UWord16 arr2[], UWord16 arr3[], Word32 m,
		Word32 n)
{

	Word32 i = 0, j = 0, k = 0;
	while (i < m && j < n)
	{
		if (arr1[i] < arr2[j])
		{
			arr3[k] = arr1[i++];
			k++;
		}
		else if (arr2[j] < arr1[i])
		{
			arr3[k] = arr2[j++];
			k++;
		}
		else
		{
			arr3[k] = arr2[j++];
			k++;
			i++;
		}
	}

	while (i < m)
	{
		arr3[k] = arr1[i++];
		k++;
	}
	while (j < n)
	{
		arr3[k] = arr2[j++];
		k++;
	}
	return k;
}

// Function prints Intersection of arr1[] and arr2[]
//m is the number of elements in arr1[]
//n is the number of elements in arr2[]
Word32 libraryIntersection(UWord16 arr1[], UWord16 arr2[], UWord16 arr3[],
		Word32 m, Word32 n)
{
	Word32 i = 0, j = 0, k = 0;
	while (i < m && j < n)
	{
		if (arr1[i] < arr2[j])
		{
			i++;
		}
		else if (arr2[j] < arr1[i])
		{
			j++;
		}
		else // if arr1[i] == arr2[j]
		{
			arr3[k] = arr2[j++];
			k++;
			i++;
		}
	}
	return k;
}

F32 libraryFindOverlapRegionPercentage(UWord32 regionX1, UWord32 regionX2,
		UWord32 regionY1, UWord32 regionY2)
{
	F32 x1 = 0;
	F32 x2 = 0;
	F32 y1 = 0;
	F32 y2 = 0;
	F32 sourceDifference = 0;
	F32 overlapDifference = 0;
	F32 matchPercent = 0;

	x1 = regionX1;
	x2 = regionX2;
	y1 = regionY1;
	y2 = regionY2;

	sourceDifference = y2 - y1;

	if ((x1 > x2) || (y1 > y2))
	{
		matchPercent = 0;
		return matchPercent;
	}
	if ((x1 == x2) && (x1 > 0) && (x1 >= y1) && (x1 <= y2))
	{
		matchPercent = 100;
		return matchPercent;
	}
	else if ((y1 == y2) && (y1 > 0) && (y1 >= x1) && (y1 <= x2))
	{
		matchPercent = 100;
		return matchPercent;
	}
	else if (x1 == x2)
	{
		matchPercent = 0;
		return matchPercent;
	}
	else if ((x1 < y1) && (x2 > y1) && (x2 < y2)) //partial overlap left
	{
		overlapDifference = x2 - y1;
	}
	else if ((x1 >= y1) && (x2 <= y2)) //complete source overlap
	{
		overlapDifference = y2 - y1;
	}
	else if ((x1 > y1) && (x1 < y2) && (x2 > y2)) //partial overlap right
	{
		overlapDifference = y2 - x1;
	}
	else if ((x2 <= y1) || (x1 >= y2)) //no overlap
	{
		overlapDifference = 0;
	}
	else if ((x1 <= y1) && (x2 >= y2)) //complete destination overlap
	{
		overlapDifference = y2 - y1;
	}
	if (sourceDifference != 0)
	{
		matchPercent = (overlapDifference / sourceDifference) * 100;
	}
	//return (UWord32) libraryRoundFloat((F64)matchPercent);
	return matchPercent;
}

UWord16 libraryRoundFloat(F64 number)
{
	return (number >= 0) ? (UWord16) (number + 0.5) : (UWord16) (number - 0.5);
}

void libraryPrintHighEmitter(TrackDefaultClass &track)
{
	string RFTypes[] =
	{ "rf_fixed", "rf_switch", "rf_jump", "rf_slide", "rf_agile", "rf_sweep",
			"rf_sweepsin", "rf_sweeptriangle", "rf_complex", "rf_uncomputed" };

	string PRITypes[] =
	{ "pri_uncomputed", "pri_fixed", "pri_dwellswitch", "pri_stagger",
			"pri_sliding", "pri_jitter", "pri_complex", "pri_difftoa" };
	string PWTypes[] =
	{ "pw_fixed", "pw_agile", "pw_uncomputed" };

	cout << " Track ID: " << track.general.trackID << endl;
	cout << " TOA Start: " << track.general.toaFirst << " TOA End: "
			<< track.general.toaLast << endl;
	cout << " RF Type: " << RFTypes[static_cast<int>(track.rf.curRfType)]
			<< endl;
	cout << " RF Spots: ";

	for (uint32_t index = 0; index < track.rf.accumulateRfCount; index++)
	{
		cout << "\t" << track.rf.accumulateRf[index].rfHigh << ",";

	}
	cout << endl;

	cout << " PRI Type: " << PRITypes[static_cast<int>(track.pri.curPriType)]
			<< endl;
	cout << " PRI Spots: ";

	switch (track.pri.curPriType)
	{
	case PriTypeEnum::pri_uncomputed:
		cout << endl;
		break;

	case PriTypeEnum::pri_fixed:
		for (uint32_t index = 0; index < track.pri.accumulatePriFixedCount;
				index++)
		{
			cout << "\t" << track.pri.accumulatePri[index].priHigh << ",";
		}
		cout << endl;
		break;

	case PriTypeEnum::pri_dwellswitch:
		for (uint32_t index = 0;
				index < track.pri.accumulatePriDwellSwitchCount; index++)
		{
			cout << "\t" << track.pri.accumulatePri[index].priHigh << ",";
		}
		cout << endl;
		break;

	case PriTypeEnum::pri_stagger:
		for (uint32_t index = 0; index < track.pri.accumulatePriStaggerCount;
				index++)
		{
			cout << "\t" << track.pri.accumulatePri[index].priHigh << ",";
		}
		cout << endl;
		break;

	case PriTypeEnum::pri_sliding:
		for (uint32_t index = 0; index < track.pri.accumulatePriSlidingCount;
				index++)
		{
			cout << "\t" << track.pri.accumulatePri[index].priHigh << ",";
		}
		cout << endl;
		break;
	case PriTypeEnum::pri_jitter:
		for (uint32_t index = 0; index < track.pri.accumulatePriJitterCount;
				index++)
		{
			cout << "\t" << track.pri.accumulatePri[index].priHigh << ",";
		}
		cout << endl;
		break;
	case PriTypeEnum::pri_complex:
		for (uint32_t index = 0; index < track.pri.accumulatePriComplexCount;
				index++)
		{
			cout << "\t" << track.pri.accumulatePri[index].priHigh << ",";
		}
		cout << endl;
		break;
	case PriTypeEnum::pri_difftoa:
		//	cout << num1 << " / " << num2 << " = " << num1/num2;
		break;
	default:
		// operator is doesn't match any case constant (+, -, *, /)
		cout << "Error! operator is not correct";
		break;
	}
	//for(uint32_t index=0; index < track.pri.; index++)
	//	{
	//	cout<<"\t"<<track.rf.accumulateRf[1].rfHigh<<",";

	//	}
	//cout<<endl;

	cout << " PW Type: " << PWTypes[static_cast<int>(track.pw.curPwType)]
			<< endl;
	cout << " PW Spots: ";

	for (uint32_t index = 0; index < track.pw.accumulatePwCount; index++)
	{
		cout << "\t" << track.pw.accumulatePw[index].pwHigh << ",";

	}
	cout << endl;

}
