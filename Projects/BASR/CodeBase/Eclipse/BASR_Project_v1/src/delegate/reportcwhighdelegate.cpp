/*
 * cwpreportdelegate.cpp
 *
 *  Created on: Jan 14, 2021
 *      Author: saad
 */

#define REPORTCWHIGHDELEGATE_CPP_

#include "reportcwhighdelegate.h"

//This function is main logic of CW processing. CW reports are read from
//CSI link with CWP, are associated with old CW tracks, new CW tracks are
//created for new threats, Spur CW threats are rejected, matching, distance
//calculation is done and finally tracks to be expired are expired

void reportCwCompleteIterationHigh(uint32_t simulatedThreats,
		DataAnalysisClass &analysisCwpHistory,
		DataAnalysisClass &analysisCwpCurrent, LibraryStruct &library,
		CfdLibrary &cfdLib)
{
	uint8_t i;
	if (simulatedThreats != 1)
	{
		//Here CW report is read, Self test emitters are filtered
		//and CW report is copied to our own Track structure
		reportCwReadFromCwp(analysisCwpHistory);
	}
	else
	{
		for (i = 0; i < analysisCwpHistory.bandAll.tracks.size(); i++)
		{
			analysisCwpHistory.bandAll.tracks[i].history.activeFlag = 1;
			analysisCwpHistory.bandAll.tracks[i].rf.accumulateRf =
					analysisCwpHistory.bandAll.tracks[i].rf.rfCircular;
		}
	}

	//CW tracks that are received are checked with existing CW tracks
	//All the received CW tracks having same RF as old CW track is
	//associated with old CW track
	reportCwAssociateTracksHigh(analysisCwpHistory.bandAll.tracks,
			analysisCwpCurrent.bandAll.tracks);

	//All those received CW tracks which are not assocaited
	//to existing CW tracks gets a new CW track made for itself
	reportCwCreateTracksHigh(analysisCwpHistory.bandAll.tracks,
			analysisCwpCurrent);

	//Spur CW threats are deleted here, i.e. if there is 2500
	//and 5000 RF threats, 5000 RF track would be deleted as
	//it would be a spur
	reportCwDeleteSpursTracks(analysisCwpCurrent.bandAll.tracks);

	//Matching is done on each CW track
	reportCwMatchTracksHighLogic(analysisCwpCurrent.bandAll.tracks, library,
			cfdLib);

	//Distance is calculated based on Free Space Path Loss Formula
	reportCwCalculateDistanceHigh(analysisCwpCurrent.bandAll.tracks, library);

	//Simple averaging filter is applied to avoid jumps in distance
	reportCwSmoothDistanceWithAveraging(analysisCwpCurrent.bandAll.tracks);

	//All the lifetime counters are incremented in this function
	reportCwPostProcessIterHigh(analysisCwpCurrent.bandAll.tracks);

	//If a track has completed its lifespan, it would be deleted in this function
	reportCwExpireTracksHigh(analysisCwpCurrent.bandAll.tracks);
}

//A simple averaging filter is applied on every iteration to avoid jumps
void reportCwSmoothDistanceWithAveraging(
		vector<TrackDefaultClass> &tracksCwReported)
{
	UWord32 i;
	UWord64 range;
	for (i = 0; i < tracksCwReported.size(); i++)
	{
		range = tracksCwReported[i].range.rangeMeters;
		if (tracksCwReported[i].range.previousRangeMeters != 0)
		{
			tracksCwReported[i].range.rangeMeters =
					(tracksCwReported[i].range.rangeMeters
							+ tracksCwReported[i].range.previousRangeMeters)
							/ 2;
		}
		tracksCwReported[i].range.previousRangeMeters = range;
	}
}

//Spurs i.e. RF image CW tracks are found and deleted
//To be called a spur, both RFs should be in different bands
//and are multiples of eachother by +- 2 MHz
void reportCwDeleteSpursTracks(vector<TrackDefaultClass> &tracksCwReported)
{
	UWord32 i, j;
//	vector<TrackDefaultClass>::iterator tIter;
//	circular_buffer<vector<TrackDefaultClass>::iterator> trackToBeDeleted;
//	trackToBeDeleted.set_capacity(40);
//	printf("Size: %d", tracksCwReported.size());
	for (i = 0; i < tracksCwReported.size(); i++)
	{
		//If there is no RF in CW track, Spurs cant be detected
		if (tracksCwReported[i].rf.accumulateRf.size() == 0)
		{
			return;
		}
		//If RF is reported as 0, a bug which was solved but can recur.
		//The track is deleted for it
		if (tracksCwReported[i].rf.accumulateRf[0].rfAvg == 0)
		{
			tracksCwReported[i].general.dspDeleteTrack = true;
		}
		//So now here is the spur detection checks are being done
		//Spurs can occur for CW tracks with Gain Stage 2 or 3
		else if ((tracksCwReported[i].power.cwGainStage == 2)
				|| (tracksCwReported[i].power.cwGainStage == 3))
		{
			for (j = 0; j < tracksCwReported.size(); j++)
			{
				if ((tracksCwReported[j].rf.accumulateRf.size() != 0)
						&& (tracksCwReported[j].general.dspDeleteTrack == false))
				{
					//If a track has different Band Code than the other AND
					//its RF is a multiple of the other, it gets deleted
					//RF can be among +-2 MHz
					if ((tracksCwReported[i].rf.bandCode
							!= tracksCwReported[j].rf.bandCode)
							&& (((tracksCwReported[j].rf.accumulateRf[0].rfAvg
									- 2)
									% tracksCwReported[i].rf.accumulateRf[0].rfAvg
									== 0)
									|| ((tracksCwReported[j].rf.accumulateRf[0].rfAvg
											- 1)
											% tracksCwReported[i].rf.accumulateRf[0].rfAvg
											== 0)
									|| ((tracksCwReported[j].rf.accumulateRf[0].rfAvg)
											% tracksCwReported[i].rf.accumulateRf[0].rfAvg
											== 0)
									|| ((tracksCwReported[j].rf.accumulateRf[0].rfAvg
											+ 1)
											% tracksCwReported[i].rf.accumulateRf[0].rfAvg
											== 0)
									|| ((tracksCwReported[j].rf.accumulateRf[0].rfAvg
											+ 2)
											% tracksCwReported[i].rf.accumulateRf[0].rfAvg
											== 0)))
					{
						//Its a spur, DELETE
//					printf("\n Delete req CW: %d \n",
//							tracksCwReported[j].rf.accumulateRf[0].rfAvg);
						tracksCwReported[j].general.dspDeleteTrack = true;
					}
				}
			}
		}
	}
	//All tracks with DSP Delete Track = 1 gets deleted
	trackDeleteInactiveTracks(tracksCwReported);
}

//This function reads CW reports from CQSI, finds all self test emitters
//and copy those emitters to local variable
void reportCwReadFromCwp(DataAnalysisClass &analysisCwp)
{

	//CW reports are copied from a buffer and extracted to CWP Report Array
	Model::cwpCqsiLink.readDataFromCwpCqsi();
	//Self Test emitters are filtered and results are inserted for Self Test
	//results to be sent to CWP
	Model::cwpCqsiLink.findCwSelfTestEmitters(
			&Model::cwpCqsiLink.cwReportRecord.cwReportArray[0],
			&Model::stDelegate.cwSelfTestResults, ConstHigh::FstInProgress);
	//We copy CWP reports from its array of CW structures to track class
	reportCwCopyCwpReportToAnalysisObject(analysisCwp);
	//The array of CWP structure is cleared for next iteration
	Model::cwpCqsiLink.clean();
}

//This function copies CW structure array to BandAll track class
void reportCwCopyCwpReportToAnalysisObject(DataAnalysisClass &analysisCwp)
{
	CwReportRecord &cwReport = Model::cwpCqsiLink.cwReportRecord;
	UWord8 numberOfCwTracks = cwReport.numberOfCWPs;
	UWord8 i = 0;
	UWord8 printHeader = 1;
	for (i = 0; i < numberOfCwTracks; i++)
	{
		TrackDefaultClass track;
		LimitRfClass rf;
		if (cwReport.cwReportArray[i].selfTestStatus != 1)
		{
			rf.rfAvg = cwReport.cwReportArray[i].frequency;
			rf.rfType = RfTypeEnum::rf_fixed;
			track.rf.rfCircular.push_back(rf);
			track.rf.accumulateRf.push_back(rf);
			track.angle.curAngle = cwReport.cwReportArray[i].angleOfArrival;
			track.angle.bearingLast = cwReport.cwReportArray[i].angleOfArrival;
			track.general.trackType = TrackTypeEnum::track_cw;
			track.history.activeFlag = cwReport.cwReportArray[i].trackUpdate;
			track.power.flagTrackingModeHigh = 1;
			track.power.avgPA = cwReport.cwReportArray[i].amplitudeA;
			track.power.avgPB = cwReport.cwReportArray[i].amplitudeB;
			track.power.avgPC = cwReport.cwReportArray[i].amplitudeC;
			track.power.avgPD = cwReport.cwReportArray[i].amplitudeD;
			track.power.pxMax = cwReport.cwReportArray[i].px;
			track.power.pyMax = cwReport.cwReportArray[i].py;
			if (cwReport.cwReportArray[i].bandCode == 0)
			{
				track.rf.bandCode = BandCodeEnum::band_e;
			}
			else if (cwReport.cwReportArray[i].bandCode == 1)
			{
				track.rf.bandCode = BandCodeEnum::band_g;
			}
			else if (cwReport.cwReportArray[i].bandCode == 2)
			{
				track.rf.bandCode = BandCodeEnum::band_i;
			}
			else if (cwReport.cwReportArray[i].bandCode == 3)
			{
				track.rf.bandCode = BandCodeEnum::band_j;
			}
			else if (cwReport.cwReportArray[i].bandCode == 4)
			{
				track.rf.bandCode = BandCodeEnum::band_cd;
				track.angle.curAngle = track.angle.curAngle + 360;
				track.angle.bearingLast = track.angle.bearingLast + 360;
			}
			track.rf.curRfType = RfTypeEnum::rf_fixed;
			track.angle.curQuadrant = cwReport.cwReportArray[i].quadrant;
			track.power.cwGainStage = cwReport.cwReportArray[i].gainFst;
			analysisCwp.bandAll.tracks.push_back(track);
//			track.rf.rfCircular.pop_back();
		}
		else
		{
			if (printHeader == 1)
			{
				printHeader = 0;
				printf("STE--CW\nAngle\tQuad\tRF\tGS\tA\tB\tC\tD\n");
			}
			printf("ID%02d\t%d\t%d\t%d\t", i, cwReport.cwReportArray[i].angleOfArrival,
					cwReport.cwReportArray[i].quadrant, cwReport.cwReportArray[i].frequency);
			printf("%d\t%d\t%d\t%d\t%d\n", cwReport.cwReportArray[i].gainFst,
					cwReport.cwReportArray[i].amplitudeA,cwReport.cwReportArray[i].amplitudeB,cwReport.cwReportArray[i].amplitudeC,cwReport.cwReportArray[i].amplitudeD);

		}
	}
// function to copy this cwp report to analysis cwp
}

//All the track lifetime counters are incremented in this function
void reportCwPostProcessIterHigh(vector<TrackDefaultClass> &tracks)
{
	uint16_t i = 0;

	for (i = 0; i < tracks.size(); i++)
	{
		tracks[i].general.timeIterCountLifetime++;
		tracks[i].general.timeIterCountLifetimeGlobal++;
		//tracks[i].power.timeIterCountQuadrantCodeCalc++;
		tracks[i].libmatch.timeIterCountLibMatchExec++;
		tracks[i].angle.timeIterCountBearingCalc++;
		tracks[i].range.timeIterCountRangeCalc++;
	}
}

//If a track has completed its lifetime, it will be deleted in this function
void reportCwExpireTracksHigh(vector<TrackDefaultClass> &tracks)
{

	vector<TrackDefaultClass>::iterator tIter;

	for (tIter = tracks.begin(); tIter != tracks.end();)
	{
		//delete track if lifetime counter has reached a certain limit
		if (tIter->general.timeIterCountLifetime
				> tIter->general.timeIterLimitLifetime)
		{
			if (ConstHigh::simLogIterationHigh == 1)
			{
				cout << "Track Expired Last Toa: " << tIter->general.toaLast
						<< endl;
			}
			tIter = tracks.erase(tIter);
		}
		//delete track if the deletionHighFlag was set previously
		else if (tIter->general.flagDeletionHigh == 1)
		{
			tIter = tracks.erase(tIter);
		}
		else if (tIter->general.flagProbation == 1)
		{
			if (tIter->general.timeIterCountProbationSet
					>= tIter->general.timeIterLimitProbationSet)
			{
				tIter = tracks.erase(tIter);
			}
			else
			{
				tIter++;
			}
		}
		else
		{
			tIter++;
		}
	}
}

//CW tracks having same RF as CW received tracks are associated and all the latest values are updated
//to the CW track
void reportCwAssociateTracksHigh(vector<TrackDefaultClass> &tracksCwReported,
		vector<TrackDefaultClass> &tracksCwActual)
{
	uint16_t i = 0, j = 0;
	for (i = 0; i < tracksCwReported.size(); i++)
	{
		if (tracksCwReported[i].history.activeFlag == 1)
		{
			for (j = 0; j < tracksCwActual.size(); j++)
			{
				int32_t rfCurr = tracksCwActual[j].rf.accumulateRf[0].rfAvg;
				int32_t rfTest = tracksCwReported[i].rf.accumulateRf[0].rfAvg;
				if (abs(rfCurr - rfTest) <= 5)
				{
					//assoc success
					tracksCwActual[j].rf.accumulateRf[0] =
							tracksCwReported[i].rf.accumulateRf[0];
					tracksCwActual[j].angle.curAngle =
							tracksCwReported[i].angle.curAngle;
					tracksCwActual[j].angle.bearingLast =
							tracksCwReported[i].angle.bearingLast;
					tracksCwActual[j].power.avgPA =
							tracksCwReported[i].power.avgPA;
					tracksCwActual[j].power.avgPB =
							tracksCwReported[i].power.avgPB;
					tracksCwActual[j].power.avgPC =
							tracksCwReported[i].power.avgPC;
					tracksCwActual[j].power.avgPD =
							tracksCwReported[i].power.avgPD;
					tracksCwActual[j].power.pxMax =
							tracksCwReported[i].power.pxMax;
					tracksCwActual[j].power.pyMax =
							tracksCwReported[i].power.pyMax;
					tracksCwActual[j].rf.bandCode =
							tracksCwReported[i].rf.bandCode;
					tracksCwActual[j].angle.curQuadrant =
							tracksCwReported[i].angle.curQuadrant;
					tracksCwReported[i].history.activeFlag = 2;
					tracksCwActual[j].general.timeIterCountLifetime = 0;

					tracksCwActual[j].power.cwGainStage =
							tracksCwReported[i].power.cwGainStage;
//					printf("Assoc CW: %d\n",
//							tracksCwReported[i].rf.accumulateRf[0].rfAvg);
					break;
				}
			}
		}
	}
}

//new tracks are created for those CW threats who are not associated
void reportCwCreateTracksHigh(vector<TrackDefaultClass> &tracksCwReported,
		DataAnalysisClass &analysisCwp)
{
	uint16_t i = 0;
	for (i = 0; i < tracksCwReported.size(); i++)
	{
		if (tracksCwReported[i].history.activeFlag == 1)
		{
			TrackDefaultClass track;
			LimitRfClass rf;
			//new cw found
//			printf("NT CW: %d\n", tracksCwReported[i].rf.accumulateRf[0].rfAvg);
			rf.rfAvg = tracksCwReported[i].rf.accumulateRf[0].rfAvg;
			rf.rfLow = tracksCwReported[i].rf.accumulateRf[0].rfAvg;
			rf.rfType = RfTypeEnum::rf_fixed;
			track.rf.accumulateRf.push_back(rf);
			track.general.trackType = TrackTypeEnum::track_cw;
			track.angle.curAngle = tracksCwReported[i].angle.curAngle;
			track.angle.bearingLast = tracksCwReported[i].angle.bearingLast;
			track.power.flagTrackingModeHigh = 1;
			track.power.avgPA = tracksCwReported[i].power.avgPA;
			track.power.avgPB = tracksCwReported[i].power.avgPB;
			track.power.avgPC = tracksCwReported[i].power.avgPC;
			track.power.avgPD = tracksCwReported[i].power.avgPD;
			track.power.pxMax = tracksCwReported[i].power.pxMax;
			track.power.pyMax = tracksCwReported[i].power.pyMax;
			track.rf.bandCode = tracksCwReported[i].rf.bandCode;
			track.rf.curRfType = RfTypeEnum::rf_fixed;
			track.angle.curQuadrant = tracksCwReported[i].angle.curQuadrant;
			tracksCwReported[i].history.activeFlag = 2;
			track.general.timeIterCountLifetime = 0;
			track.power.cwGainStage = tracksCwReported[i].power.cwGainStage;
			track.general.timeIterLimitLifetime =
					ConstHigh::timeIterEquivalent4000ms;
			track.power.scanTypeLast = EmitterArpEnum::scan_lock;
			track.last.set_capacity(1);
			track.last.push_front(track);
			track.range.previousRangeMeters = 0;
			analysisCwp.bandAll.tracks.push_back(track);
//			track.rf.accumulateRf.pop_back();
		}
	}
}

//All CW tracks are sent to be matched against CW threats from TL
void reportCwMatchTracksHighLogic(vector<TrackDefaultClass> &tracksCwActual,
		LibraryStruct &library, CfdLibrary &cfdLib)
{
	uint8_t i = 0;
	for (i = 0; i < tracksCwActual.size(); i++)
	{
		reportCwMatchTrackHigh(tracksCwActual[i], library);
		reportCwMatchTrackPostProcess(tracksCwActual[i], library, cfdLib);
		reportCwCfdDataTracksHigh(tracksCwActual, library, cfdLib);
		libraryCalculatePriorityOfTracks(tracksCwActual, library);
	}
}

void reportCwCfdDataTracksHigh(vector<TrackDefaultClass> &tracks,
		LibraryStruct &library, CfdLibrary &cfdLib)
{
	uint8_t i = 0;
	for (i = 0; i < tracks.size(); i++)
	{
		tracks[i].cfd.threatIndex = tracks[i].general.trackID;

		for (auto j = 0; j < cfdLib.cfdHeader.totalSequences; j++)
		{
			if ((cfdLib.cfdProgram[j].programId
					== tracks[i].libmatch.last.cfdSuggestedId))
			{
				tracks[i].cfd.cmProgramId =
						tracks[i].libmatch.last.cfdSuggestedId;

				break;
			}
			else if ((cfdLib.cfdProgram[j].programId
					== tracks[i].libmatch.last.cfdAlternativeId))
			{
				tracks[i].cfd.cmProgramId =
						tracks[i].libmatch.last.cfdAlternativeId;

				break;
			}
		}

		tracks[i].cfd.altitudeValidity = 0;
		tracks[i].cfd.velocityValidity = 0;

		if ((tracks[i].angle.bearingLast >= 0)
				&& (tracks[i].angle.bearingLast <= 90))
		{
			tracks[i].cfd.aoa = 0;
		}
		else if ((tracks[i].angle.bearingLast > 90)
				&& (tracks[i].angle.bearingLast <= 180))
		{
			tracks[i].cfd.aoa = 1;
		}
		else if ((tracks[i].angle.bearingLast > 180)
				&& (tracks[i].angle.bearingLast <= 270))
		{
			tracks[i].cfd.aoa = 2;
		}
		else if ((tracks[i].angle.bearingLast > 270)
				&& (tracks[i].angle.bearingLast <= 360))
		{
			tracks[i].cfd.aoa = 3;
		}

		if (tracks[i].general.timeIterCountLifetime <= 1)
		{
			tracks[i].cfd.status = 1;
		}
		else if (tracks[i].general.timeIterCountLifetime > 1)
		{
			tracks[i].cfd.status = 2;
		}

		tracks[i].cfd.altitude = 0;
		tracks[i].cfd.velocity = 0;
	}
}

void reportCwMatchTrackPostProcess(TrackDefaultClass &track,
		LibraryStruct &library, CfdLibrary &cfdLib)
{
	uint8_t flagSetKnownMatch = 0;
	uint8_t flagCfdFoundInLibrary = 0;

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

		if (track.general.flagVisibleHigh == 1)
		{
			if (track.libmatch.previous.flagKnownLibrary == 1)
			{
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

//		if (((track.libmatch.last.flagAerialOnlyLibrary == 1)
//				&& (track.rf.getBandCodeLegacy() == 2))
//				|| ((track.libmatch.last.highestMatchList[0].RFScore == 0)
//						&& (track.rf.getBandCodeLegacy() == 2))
//				|| (track.libmatch.last.flagMissileOnlyLibrary == 1))
//		{
//			flagSetKnownMatch = 0;
//			track.libmatch.last.flagKnownLibrary = 0;
//
//			if ((track.general.flagVisibleHigh == 1)
//					&& (track.libmatch.last.countCategoryConsecutiveKnown > 1))
//			{
//				flagSetKnownMatch = 1;
//			}
//		}
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
			if (track.rf.getBandCodeLegacy() == 0)
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
//				if (track.pri.accumulatePriStaggerCount > 0)
//				{
//					track.libmatch.last.flagAerialOnlyLibrary = 0;
//					track.libmatch.last.flagMissileOnlyLibrary = 0;
//					track.libmatch.last.flagSamOnlyLibrary = 0;
//					track.libmatch.last.flagScanningOnlyLibrary = 0;
//					track.libmatch.last.flagTrackingOnlyLibrary = 0;
//					track.libmatch.last.effectiveRadiatedPower = 120;
//				}
//				else
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

//CW threats are matched with TL CW threats
void reportCwMatchTrackHigh(TrackDefaultClass &track, LibraryStruct &library)
{

	UWord32 i = 0, j = 0, k = 0, RFMatched = 0;
	UWord8 count = 0;
	vector<LibraryMatchScore> scoresTotal;
	for (i = 0; i < library.header.NumberOfThreats; i++)
	{
		RFMatched = 0;
		if ((library.Emitters[i].emitterType == 2)
				|| (library.Emitters[i].emitterType == 3))
		{
			for (k = 0; k < library.Emitters[i].rf.rfCount; k++)
			{
				if (library.Emitters[i].rf.rfRanges[k].rfLower
						== library.Emitters[i].rf.rfRanges[k].rfUpper)
				{
					if (library.Emitters[i].rf.rfRanges[k].rfLower
							== track.rf.accumulateRf[0].rfAvg)
					{
						RFMatched++;
						break;
					}
				}
				else
				{
					if (library.Emitters[i].rf.rfRanges[k].rfLower
							<= track.rf.accumulateRf[0].rfAvg)
					{
						if (library.Emitters[i].rf.rfRanges[k].rfUpper
								>= track.rf.accumulateRf[0].rfAvg)
						{
							RFMatched++;
							break;
						}
					}
				}
			}

			scoresTotal.resize(RFMatched);
			libraryScoreInit(scoresTotal, RFMatched);
			if (RFMatched > 0)
			{
				if (library.Emitters[i].emitterType == 2)
				{
					scoresTotal[j].Total = 100;
					scoresTotal[j].ID = i;
					j++;
				}
				else
				{
					scoresTotal[j].Total = 75;
					scoresTotal[j].ID = i;
					j++;
				}
				RFMatched = 0;
			}
		}
	}
	count = j;
	libraryCopyTopScores(scoresTotal, track.libmatch, count);
}

//Distance of each CW track is calculated here
void reportCwCalculateDistanceHigh(vector<TrackDefaultClass> &tracksCwActual,
		LibraryStruct &library)
{
	UWord16 erp = 100;
	UWord16 rf = 9;
	UWord16 pxMax, pyMax;
	UWord32 ap, dist;
	F64 otherCalculations = 0;
	F64 pMax, pMaxDbm;
	uint8_t i = 0;
	for (i = 0; i < tracksCwActual.size(); i++)
	{
		if (tracksCwActual[i].general.timeIterCountLifetime == 0)
		{
			if ((tracksCwActual[i].libmatch.active.flagKnownLibrary == 0)
					|| (library.Emitters[tracksCwActual[i].libmatch.active.highestMatchList[0].ID].erp.erp
							== 0)
					|| (tracksCwActual[i].libmatch.active.highestMatchListCount
							< 0))
			{
				//if erp is unknown, default erp is 125
				erp = 125;
			}
			else
			{
				//else erp is extracted from TL's matched emitter
				erp =
						library.Emitters[tracksCwActual[i].libmatch.active.highestMatchList[0].ID].erp.erp;
			}
			rf = tracksCwActual[i].rf.accumulateRf[0].rfAvg;

			pxMax = tracksCwActual[i].power.pxMax;
			pyMax = tracksCwActual[i].power.pyMax;
			//Max ADC count Power of threat is extracted here
			if (pxMax > pyMax)
			{
				pMax = (F64) pxMax;
			}
			else
			{
				pMax = (F64) pyMax;
			}

//                printf("P-Max: %f\n",pMax);

			//Max Power is converted to dBm from CWPs ADC counts
			pMaxDbm = reportCwTpAdctoDbm(pMax);

//            printf("P-Max-DB: %f\n",pMaxDbm);

			//If Max power in dBm is greater than erp than distance becomes 0
			if (pMaxDbm > erp)
			{
				tracksCwActual[i].range.rangeMeters = 0;
			}
			else
			{
				/*
				 fspl = erp - (pMaxDbm+4);
				 printf("fspl: %d\n",fspl);
				 printf("rf: %d\n",rf);
				 rf=rf/1000;
				 otherCalculations = (fspl - (20 * log10(rf))-92.45)/20;

				 printf("otherCalculations: %f\n",otherCalculations);
				 check=(pow(10.00,otherCalculations) * KILOMETER_TO_NAUTICAL_MILES);
				 printf("check: %llu \n",check);
				 pTrack->trackDistance=(UWord32)(pow(10.00,otherCalculations) * KILOMETER_TO_NAUTICAL_MILES);
				 */
				ap = abs(45 - (tracksCwActual[i].angle.curAngle % 90));
				pMaxDbm = pMaxDbm + ConstHigh::cwPowerCorrectionLookup[ap];
				otherCalculations =
						(erp - pMaxDbm + 4 - 32.4 - (20 * log10(rf))) / 20;
//                printf("rf: %d, ap: %d, pMaxDbm: %f\n",rf, ap, pMaxDbm);
//                printf("otherCalculations: %f, ERP: %d\n",otherCalculations, erp);
				dist = (UWord32) ((pow(10.00, otherCalculations)) / 1.852);
				tracksCwActual[i].range.rangeMeters = dist * 1852;
//                printf("Dis: %d \n",pTrack->trackDistance);

//GMD formula
//                otherCalculations = pow(10,(((F64)(erp-pMaxDbm+147))/20)-6);
//                otherCalculations = otherCalculations/((F64)rf);
//                otherCalculations = otherCalculations/(1000*1.852);
//
//                printf("rf: %d, ap: %d, pMaxDbm: %f\n",rf, ap, pMaxDbm);
//                printf("otherCalculations: %f, ERP: %d\n",otherCalculations, erp);
//                dist = (UWord32)(otherCalculations);
			}
			//printf("Dist: %d\n\n",pTrack->trackDistance);
			//printf("Top ID: %d\n",pTrack->lastEmitterExtended.TopScorers[0].ID);
		}
	}
//printf("\n");
}

//This function converts CWP ADC counts to dBms
F64 reportCwTpAdctoDbm(F64 adcCount)
{
	F64 dlva_d = 0, dlva = 0, p1 = 0, px = 0;
	dlva_d = adcCount / 2048;
	dlva = dlva_d / 0.37;
	p1 = pow(10, ((dlva - 3.704) / 0.539));
	px = 10 * log10(p1);
	return px;
}
