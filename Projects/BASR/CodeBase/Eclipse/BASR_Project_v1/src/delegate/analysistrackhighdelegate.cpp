/*
 * analysistrackhighdelegate.cpp
 *
 *  Created on: Sep 25, 2019
 *      Author: hassan
 */

#include "../delegate/analysistrackhighdelegate.h"

void trackPreProcessIterLow(vector<TrackDefaultClass> &tracks)
{
	uint16_t i = 0;

	//copy all reported pri circular buffer into accumulated circular buffer
	//and count all individual pri types
	for (i = 0; i < tracks.size(); i++)
	{
		//copy the buffer into accumulated
		if (tracks[i].general.toaLast == 156415200)
		{
			tracks[i].general.toaLast = 156415200;
		}
		//if (tracks[i].general.toaFirst == 83039621405)
		//{
		//	tracks[i].general.toaFirst = 83039621405;
		//}
		trackRemovePriDuplicates(tracks[i]);

		tracks[i].pri.accumulatePri = tracks[i].pri.priCircular;
		tracks[i].pw.accumulatePw = tracks[i].pw.pwCircular;
		tracks[i].rf.accumulateRf = tracks[i].rf.rfCircular;
		tracks[i].power.sumPxPyMax = tracks[i].power.pxMax
				+ tracks[i].power.pyMax;
		trackCountUpdatePriAccumulative(tracks[i]);
		trackCountUpdatePriDominantCurrent(tracks[i]);
		if (tracks[i].pw.pwCircular.size() == 0)
		{
			LimitPwClass placeHolderPw;
			tracks[i].pw.pwCircular.push_back(placeHolderPw);
		}
		//libraryMatchEmitter(tracks[i],Model::library,Model::libraryMeta);
		if ((tracks[i].pri.accumulatePriFixedCount == 0)
				&& (tracks[i].pri.accumulatePriDwellSwitchCount == 0)
				&& (tracks[i].pri.accumulatePriStaggerCount == 0)
				&& (tracks[i].pri.accumulatePriJitterCount == 0)
				&& (tracks[i].pri.accumulatePriSlidingCount == 0)
				&& (tracks[i].pri.accumulatePriComplexCount == 0)
				&& (tracks[i].pri.accumulatePriDiffToaCount == 0))
		{
			tracks[i].general.processingHighFlag = 1;
		}
		if ((tracks[i].pri.accumulatePriFixedCount > 0)
				|| (tracks[i].pri.accumulatePriDwellSwitchCount > 0)
				|| (tracks[i].pri.accumulatePriStaggerCount > 0)
				|| (tracks[i].pri.accumulatePriJitterCount > 0)
				|| (tracks[i].pri.accumulatePriSlidingCount > 0)
				|| (tracks[i].pri.accumulatePriComplexCount > 0))
		{
			circular_buffer<LimitPriClass>::iterator pIter;
			for (pIter = tracks[i].pri.accumulatePri.begin();
					pIter != tracks[i].pri.accumulatePri.end();)
			{
				//delete low track if it has no pri
				if (pIter->priType == PriTypeEnum::pri_difftoa)
				{
					pIter = tracks[i].pri.accumulatePri.erase(pIter);
				}
				else
				{
					pIter++;
				}
			}
			tracks[i].pri.accumulatePriDiffToaCount = 0;
		}
	}
	vector<TrackDefaultClass>::iterator tIter;
	for (tIter = tracks.begin(); tIter != tracks.end();)
	{
		//delete low track if it has no pri
		if (tIter->pri.priTypeCurrentDominant == PriTypeEnum::pri_uncomputed)
		{
			tIter = tracks.erase(tIter);
		}
		else
		{
			tIter++;
		}
	}

}
uint8_t trackComparePriLowLow(TrackDefaultClass &track1,
		TrackDefaultClass &track2)
{
	uint16_t i = 0;

	if (track1.pri.priTypeCurrentDominant == PriTypeEnum::pri_fixed
			&& track2.pri.priTypeCurrentDominant
					== PriTypeEnum::pri_dwellswitch)
	{
		for (i = 0; i < track1.pri.priCircular.size(); i++)
		{
			if (findIfNthMultiple(track1.pri.priCircular[i].priMean,
					track2.pri.priCircular[i].priMean, (int16_t) 16))
			{

				return 1;
			}
		}
	}
	return 0;
}
uint8_t trackComparePwLowLow(TrackDefaultClass &track1,
		TrackDefaultClass &track2)
{
	uint32_t minPW = min(track1.pw.pwCircular[0].pwLow,
			track2.pw.pwCircular[0].pwLow);
	uint32_t maxPW = max(track1.pw.pwCircular[0].pwLow,
			track2.pw.pwCircular[0].pwLow);
	if (maxPW < 2 * minPW)
	{
		return 1;
	}
	return 0;
}
uint8_t trackCompareAngleLowLow(TrackDefaultClass &track1,
		TrackDefaultClass &track2)
{
	uint16_t angleDiff = 0;
	uint16_t angleDiffQuadrantSwap = 0;

	mergeCalcBearingDifferenceLowLow(track1, track2, angleDiff,
			angleDiffQuadrantSwap);
	if ((angleDiff < ConstHigh::mergeLimitStringBearingPrimary)
			|| (angleDiffQuadrantSwap
					< ConstHigh::mergeLimitStringBearingSecondary))
	{
		//if(angleDiff < ConstHigh::mergeLimitStringBearingPrimary) {
		return 1;
	}
	return 0;
}
void trackPreProcessIterHigh(vector<TrackDefaultClass> &tracks)
{

	uint16_t i = 0;

	//update track lifetime counters and other variables
	for (i = 0; i < tracks.size(); i++)
	{

		tracks[i].history.lowHighFlag = 1;
		tracks[i].general.processingHighFlag = 0;
		tracks[i].history.idsAssociateTracksIterLow.clear();
		tracks[i].history.indexAssociateTracksIterLow.clear();
		//review where this variable is used,
		//legacy name: trackingTrackPotentialActiveFlag
		tracks[i].history.activeFlag = 1;
		tracks[i].history.countPulseOverlapMaxLow = 0;
		tracks[i].history.countPulseWidthOverlapMaxLow = 0;
		tracks[i].history.countPriTypeOverlapMaxLow = 0;
		tracks[i].history.countTrackOverlapMaxLow = 0;
		tracks[i].history.durationToaOverlapMaxLow = 0;
		tracks[i].history.flagOverlapPotential = 0;
		tracks[i].history.highTrackContinuationIterFlag = 2;
		//tracks[i].history.highToaStartUpdateCount = 0;
		tracks[i].history.highToaStartUpdateFlag = 0;//reconcile the name of this variable
		//tracks[i].history.lowIndexUpdateNext = 0;
		//tracks[i].history.lowToaStartUpdateCount = 0;
		//tracks[i].history.lowToaStartUpdateFlag = 0;
		//tracks[i].history.lowToaStartUpdateNextFlag = 0;
		tracks[i].history.stitchedDiffToaCount = 0;
		tracks[i].history.stitchedCount = 0;
		tracks[i].history.stitchedJitterAndComplexCount = 0;
		tracks[i].history.stitchedUpdateDiffToaCount = 0;
		tracks[i].history.stitchedUpdateCount = 0;
		tracks[i].history.stitchedUpdateNextDiffToaCount = 0;
		tracks[i].history.stitchedUpdateNextCount = 0;
		tracks[i].history.totalPulseCount = 0;
		tracks[i].history.totalPulseOverlap = 0;
		tracks[i].history.totalToaOverlap = 0;
		tracks[i].history.flagUpdateOnceIterHigh = 0;//reconcile the name of this variable
		tracks[i].power.countTrackingModeLowIterHigh = 0;
		tracks[i].power.countScanningModeLowIterHigh = 0;

		tracks[i].libmatch.flagIterAttemptMatchLibrary = 0;
		tracks[i].libmatch.flagSetUnknownLibrary = 0;

		//legacy
		/*trackingTrackPotentialList[globalTrackCountIndex].reportOrTrackFlag = 1;
		 trackingTrackPotentialList[globalTrackCountIndex].trackingTrackPotentialActiveFlag = 1;
		 trackingTrackPotentialList[globalTrackCountIndex].reportIndex = 0;
		 trackingTrackPotentialList[globalTrackCountIndex].trackingReportTotalToaOverlap = 0;
		 trackingTrackPotentialList[globalTrackCountIndex].trackingReportTotalPulseOverlap = 0;
		 trackingTrackPotentialList[globalTrackCountIndex].trackingReportTotalPulseCount = 0;
		 trackingTrackPotentialList[globalTrackCountIndex].trackingReportStitchedCount = 0;//the number of reports that were strung into actual and potential tracks
		 trackingTrackPotentialList[globalTrackCountIndex].trackingReportJitterCount = 0;
		 trackingTrackPotentialList[globalTrackCountIndex].trackingReportComplexCount = 0;
		 trackingTrackPotentialList[globalTrackCountIndex].trackingTrackPotentialUpdateCount = 0; //how many reports in this active or potential track have updates
		 trackingTrackPotentialList[globalTrackCountIndex].trackingTrackPotentialUpdateNextCount = 0; //how many reports in this active or potential track have updates in next iteration
		 trackingTrackPotentialList[globalTrackCountIndex].trackingTrackPotentialUpdateCountComplex = 0; //how many reports in this active or potential track have updates
		 trackingTrackPotentialList[globalTrackCountIndex].trackingTrackPotentialUpdateNextCountComplex = 0; //how many reports in this active or potential track have updates in next iteration
		 */
	}
}
void trackPostProcessIterHigh(vector<TrackDefaultClass> &tracks)
{
	uint16_t i = 0;

	for (i = 0; i < tracks.size(); i++)
	{
		if (tracks[i].general.timeIterCountLifetimeGlobal > 0)
			tracks[i].general.flagVisibleHigh = 1;
		tracks[i].general.timeIterCountLifetime++;
		tracks[i].general.timeIterCountLifetimeGlobal++;
		//tracks[i].power.timeIterCountQuadrantCodeCalc++;
		tracks[i].libmatch.timeIterCountLibMatchExec++;
		tracks[i].angle.timeIterCountBearingCalc++;
		tracks[i].range.timeIterCountRangeCalc++;
		if (tracks[i].general.flagProbation == 1)
		{
			tracks[i].general.timeIterCountProbationSet++;
		}
		trackCountUpdatePriDominantCurrent(tracks[i]);		//why?
		trackCountUpdateRfDominantCurrent(tracks[i]);
	}
}
void trackRemovePriDuplicates(TrackDefaultClass &track)
{

	uint8_t i = 0;
	uint8_t j = 0;
	uint8_t countStagger = 0;
	uint8_t countMultipleStagger = 0;
	bool flagMultipleStagger = false;
	uint32_t comparisonPhase = 0;
	uint32_t firstPhaseMultiple = 0;
	uint32_t secondPhaseMultiple = 0;
	uint32_t resultOne = 0;
	uint32_t resultTwo = 0;

	bool pairFound = false;

	//if(track.general.toaFirst == 221427398125) {
	//	track.general.toaFirst = 221427398125;
	//}

	for (i = 0; i < track.pri.priCircular.size(); i++)
	{
		if (track.pri.priCircular[i].priType == PriTypeEnum::pri_fixed)
		{
			track.pri.priCircular[i].priType = PriTypeEnum::pri_dwellswitch;
		}
	}
	//remove consecutive pri spots
	pairFound = true;
	while (pairFound == true)
	{
		pairFound = false;

		for (i = 0; i < track.pri.priCircular.size(); i++)
		{
			if (i == (track.pri.priCircular.size() - 1))
			{
				pairFound = false;
				break;
			}
			if ((track.pri.priCircular[i].priLow
					== track.pri.priCircular[i + 1].priLow)
					&& (track.pri.priCircular[i].priHigh
							== track.pri.priCircular[i + 1].priHigh)
					&& (track.pri.priCircular[i].priType
							== track.pri.priCircular[i + 1].priType))
			{
				track.pri.priCircular.erase(
						track.pri.priCircular.begin() + (i + 1));
				pairFound = true;
				break;
			}
		}
	}
	//convert stagger pri that are are all phase multiples into dwell and switch
	countMultipleStagger = 0;
	vector<uint8_t> isMultipleSpotFlag(track.pri.priCircular.size(), 0);
	circular_buffer<LimitPriClass> priCircularBackup;
	for (i = 0; i < track.pri.priCircular.size(); i++)
	{

		flagMultipleStagger = false;

		for (j = 0; j < track.pri.priCircular.size(); j++)
		{
			if (i == j)
			{
				continue;
			}
			if ((track.pri.priCircular[i].priType == PriTypeEnum::pri_stagger)
					&& (track.pri.priCircular[j].priType
							== PriTypeEnum::pri_stagger)
					&& (track.pri.priCircular[i].frame
							== track.pri.priCircular[j].frame))
			{

				if (track.pri.priCircular[i].priLow
						<= track.pri.priCircular[j].priLow)
				{
					comparisonPhase = track.pri.priCircular[j].priLow;
					firstPhaseMultiple = track.pri.priCircular[i].priLow * 2;
					secondPhaseMultiple = track.pri.priCircular[i].priLow * 3;
				}
				else
					continue;
//				else
//				{
//					comparisonPhase = track.pri.priCircular[i].priLow;
//					firstPhaseMultiple = track.pri.priCircular[j].priLow * 2;
//					secondPhaseMultiple = track.pri.priCircular[j].priLow * 3;
//				}
				resultOne = labs(
						(int32_t) firstPhaseMultiple
								- (int32_t) comparisonPhase);
				resultTwo = labs(
						(int32_t) secondPhaseMultiple
								- (int32_t) comparisonPhase);
				if ((resultOne < 10) || (resultTwo < 10))
				{
					flagMultipleStagger = true;
					isMultipleSpotFlag[j] = 1;
				}
			}
		}
		if (track.pri.priCircular[i].priType == PriTypeEnum::pri_stagger)
		{
			countStagger++;
			if (flagMultipleStagger == true)
			{
				countMultipleStagger++;
			}
		}
	}
	if ((countStagger > 0) && (countMultipleStagger > 0))
	{
		priCircularBackup = track.pri.priCircular;
		track.pri.priCircular.resize(0);
		for (i = 0; i < priCircularBackup.size(); i++)
		{
			if (priCircularBackup[i].priType == PriTypeEnum::pri_stagger)
			{
				priCircularBackup[i].priType = PriTypeEnum::pri_dwellswitch;
				if (isMultipleSpotFlag[i] == 0)
				{
					track.pri.priCircular.push_back(priCircularBackup[i]);
				}
			}

		}
	}
}
void trackCountUpdatePriAccumulative(TrackDefaultClass &track)
{
	uint16_t i = 0;

	track.pri.accumulatePriFixedCount = 0;
	track.pri.accumulatePriDwellSwitchCount = 0;
	track.pri.accumulatePriStaggerCount = 0;
	track.pri.accumulatePriJitterCount = 0;
	track.pri.accumulatePriSlidingCount = 0;
	track.pri.accumulatePriComplexCount = 0;
	track.pri.accumulatePriDiffToaCount = 0;

	for (i = 0; i < track.pri.accumulatePri.size(); i++)
	{
		//update accumulative counts
		if ((track.pri.accumulatePri[i].priType == PriTypeEnum::pri_fixed)
				|| (track.pri.accumulatePri[i].priType
						== PriTypeEnum::pri_dwellswitch))
		{
			track.pri.accumulatePriFixedCount = 1;
			track.pri.accumulatePriDwellSwitchCount++;
		}
		if (track.pri.accumulatePri[i].priType == PriTypeEnum::pri_stagger)
		{
			track.pri.accumulatePriStaggerCount++;
		}
		if (track.pri.accumulatePri[i].priType == PriTypeEnum::pri_jitter)
		{
			track.pri.accumulatePriJitterCount++;
		}
		if (track.pri.accumulatePri[i].priType == PriTypeEnum::pri_sliding)
		{
			track.pri.accumulatePriSlidingCount++;
		}
		if (track.pri.accumulatePri[i].priType == PriTypeEnum::pri_complex)
		{
			track.pri.accumulatePriComplexCount++;
		}
		if (track.pri.accumulatePri[i].priType == PriTypeEnum::pri_difftoa)
		{
			track.pri.accumulatePriDiffToaCount++;
		}
	}
	//make this logic more intelligent by comparing two close dwell switches
	if (track.pri.accumulatePriDwellSwitchCount == 1)
	{
		track.pri.accumulatePriFixedCount = 1;
		//mark all dwell types as fixed
		//for (i = 0; i < track.pri.accumulatePri.size(); i++) {
		//	if(track.pri.accumulatePri[i].priType == PriTypeEnum::pri_dwellswitch) {
		//		track.pri.accumulatePri[i].priType = PriTypeEnum::pri_fixed;
		//	}
		//}
	}
	if (track.pri.accumulatePriDwellSwitchCount > 1)
	{
		track.pri.accumulatePriFixedCount = 0;
	}
	track.pri.flagPriAnalysisLimitedLast = 0;
	if ((track.pri.accumulatePriDiffToaCount > 0)
			&& (track.pri.accumulatePriComplexCount == 0)
			&& (track.pri.accumulatePriDwellSwitchCount == 0)
			&& (track.pri.accumulatePriStaggerCount == 0)
			&& (track.pri.accumulatePriSlidingCount == 0)
			&& (track.pri.accumulatePriJitterCount == 0))
	{
		track.pri.flagPriAnalysisLimitedLast = 1;
	}
}
void trackCountUpdatePriDominantCurrent(TrackDefaultClass &track)
{
	track.pri.priTypeCurrentDominant = PriTypeEnum::pri_uncomputed;
	typedef std::pair<uint16_t, uint8_t> myPair;
	vector<myPair> priTypeCounts(6);

	priTypeCounts[0].first = track.pri.accumulatePriComplexCount;
	priTypeCounts[0].second = 0;
	priTypeCounts[1].first = track.pri.accumulatePriJitterCount;
	priTypeCounts[1].second = 1;
	priTypeCounts[2].first = track.pri.accumulatePriDwellSwitchCount;
	priTypeCounts[2].second = 2;
	priTypeCounts[3].first = track.pri.accumulatePriSlidingCount;
	priTypeCounts[3].second = 3;
	priTypeCounts[4].first = track.pri.accumulatePriStaggerCount;
	priTypeCounts[4].second = 4;
	priTypeCounts[5].first = track.pri.accumulatePriDiffToaCount;
	priTypeCounts[5].second = 5;

	sort(priTypeCounts.begin(), priTypeCounts.end(),
			[](const myPair &l, const myPair &r)
			{	return l.first>r.first;});

	switch (priTypeCounts[0].second)
	{
	case 0:
		track.pri.priTypeCurrentDominant = PriTypeEnum::pri_complex;
		break;
	case 1:
		track.pri.priTypeCurrentDominant = PriTypeEnum::pri_jitter;
		break;
	case 2:
		track.pri.priTypeCurrentDominant = PriTypeEnum::pri_dwellswitch;
		break;
	case 3:
		track.pri.priTypeCurrentDominant = PriTypeEnum::pri_sliding;
		break;
	case 4:
		track.pri.priTypeCurrentDominant = PriTypeEnum::pri_stagger;
		break;
	case 5:
		track.pri.priTypeCurrentDominant = PriTypeEnum::pri_difftoa;
		break;
	default:
		track.pri.priTypeCurrentDominant = PriTypeEnum::pri_uncomputed;
	}
}
void trackCountUpdateRfDominantCurrent(TrackDefaultClass &track)
{
	track.rf.curRfType = RfTypeEnum::rf_uncomputed;
	//if (track.rf.accumulateAgileRfCount > 0)
	//{
	//	track.rf.curRfType = RfTypeEnum::rf_agile;
	//}
	//if (track.rf.accumulateFixedRfCount > 0)
	//{
	//	track.rf.curRfType = RfTypeEnum::rf_fixed;
	//}
	//if (track.rf.accumulateSwitchRfCount > 0)
	//{
	//	track.rf.curRfType = RfTypeEnum::rf_switch;
	//}
	//if (track.rf.accumulateSwitchRfCount > 0)
	//{
	//	track.rf.curRfType = RfTypeEnum::rf_switch;
	//}

	typedef std::pair<uint16_t, uint8_t> myPair;
	vector<myPair> rfTypeCounts(5);

	rfTypeCounts[0].first = track.rf.accumulateComplexRfCount;
	rfTypeCounts[0].second = 0;
	rfTypeCounts[1].first = track.rf.accumulateAgileRfCount;
	rfTypeCounts[1].second = 1;
	rfTypeCounts[2].first = track.rf.accumulateSwitchRfCount;
	rfTypeCounts[2].second = 2;
	rfTypeCounts[3].first = track.rf.accumulateJumpRfCount;
	rfTypeCounts[3].second = 3;
	rfTypeCounts[4].first = track.rf.accumulateFixedRfCount;
	rfTypeCounts[4].second = 4;


	sort(rfTypeCounts.begin(), rfTypeCounts.end(),
			[](const myPair &l, const myPair &r)
			{	return l.first>r.first;});

	switch (rfTypeCounts[0].second)
	{
	case 0:
		track.rf.curRfType = RfTypeEnum::rf_complex;
		break;
	case 1:
		track.rf.curRfType = RfTypeEnum::rf_agile;
		break;
	case 2:
		track.rf.curRfType = RfTypeEnum::rf_switch;
		break;
	case 3:
		track.rf.curRfType = RfTypeEnum::rf_jump;
		break;
	case 4:
		track.rf.curRfType = RfTypeEnum::rf_fixed;
		break;
	default:
		track.rf.curRfType = RfTypeEnum::rf_uncomputed;
	}
	

}
void trackCountUpdatePriDominantFuture(TrackDefaultClass &track,
		TrackDefaultClass &trackNext)
{
	if (trackNext.pri.accumulatePriComplexCount > 0)
	{
		track.pri.priTypeFutureDominant = PriTypeEnum::pri_complex;
	}
	if (trackNext.pri.accumulatePriJitterCount > 0)
	{
		track.pri.priTypeFutureDominant = PriTypeEnum::pri_jitter;
	}
	if (trackNext.pri.accumulatePriDiffToaCount > 0)
	{
		track.pri.priTypeFutureDominant = PriTypeEnum::pri_difftoa;
	}
	if (trackNext.pri.accumulatePriSlidingCount > 0)
	{
		track.pri.priTypeFutureDominant = PriTypeEnum::pri_sliding;
	}
	if (trackNext.pri.accumulatePriDwellSwitchCount > 0)
	{
		track.pri.priTypeFutureDominant = PriTypeEnum::pri_dwellswitch;
	}
	if (trackNext.pri.accumulatePriStaggerCount > 0)
	{
		track.pri.priTypeFutureDominant = PriTypeEnum::pri_stagger;
	}
}
void trackExpireTracksHigh(vector<TrackDefaultClass> &tracks)
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

void trackCfdDataTracksHigh(vector<TrackDefaultClass> &tracks,
		LibraryStruct &library, CfdLibrary &cfdLib)
{
	uint8_t i = 0;
	for (i = 0; i < tracks.size(); i++)
	{
		tracks[i].cfd.threatIndex = tracks[i].general.trackID;
		if(tracks[i].libmatch.active.highestMatchListCount>0)
		{
			tracks[i].cfd.threatIndex = tracks[i].libmatch.active.highestMatchList[0].ID;
		}
//		tracks[i].cfd.threatIndex = i;

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

void trackSortTracksToaStartLow(DataSensorClass &dataSensor)
{

	vector<TrackDefaultClass> &v = dataSensor.tracks;
	vector<uint16_t> &idx = dataSensor.tracksIndexSorted;
	idx.resize(v.size());
	iota(idx.begin(), idx.end(), 0);
	stable_sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2)
	{
		return v[i1].general.toaFirst < v[i2].general.toaFirst;
	});
}
void trackProcessConsecutiveFirstToaLow(vector<TrackDefaultClass> &tracks,
		vector<TrackDefaultClass> &tracksNext)
{
	uint16_t i = 0;
	uint16_t j = 0;

	for (i = 0; i < tracks.size(); i++)
	{
		//set if the current low report has been updated from the previous one
		if (tracks[i].history.lowToaStartUpdateCount > 0)
			tracks[i].history.lowToaStartUpdateFlag = 1;
		//check if the next low report has been updated from the current one
		for (j = 0; j < tracksNext.size(); j++)
		{
			if (tracks[i].general.toaFirst == tracksNext[j].general.toaFirst)
			{
				uint64_t trackduration = tracksNext[j].general.toaLast
						- tracksNext[j].general.toaFirst;
				if (tracks[i].general.toaLast == tracksNext[j].general.toaLast)
				{
					tracksNext[j].general.processingHighFlag = 1;
				}
				else if (tracksNext[j].general.dspTrackingEnabled == false
						&& trackduration < 5000000)
				{
					tracks[i] = tracksNext[j];
//					tracks[i].pw.pwCircular = tracksNext[j].pw.pwCircular;
//					tracks[i].rf.rfCircular = tracksNext[j].rf.rfCircular;
//					tracks[i].power = tracksNext[j].power;
//					tracks[i].general.toaLast =tracksNext[j].general.toaLast;
//					tracks[i].general.trackPulseCountTotal += tracksNext[j].general.trackPulseCountTotal;
					tracksNext[j].general.processingHighFlag = 1;
				}
				else
				{
					tracks[i].history.lowToaStartUpdateNextFlag = 1;
					tracks[i].history.lowIndexUpdateNext = j;
					tracksNext[j].history.lowToaStartUpdateCount =
							tracks[i].history.lowToaStartUpdateCount + 1;

				}
			}
		}
	}
}
void trackProcessConsecutiveFirstToaCountAllHigh(DataSensorClass &high)
{

	uint16_t i = 0;

	for (i = 0; i < high.tracks.size(); i++)
	{
		if (high.tracks[i].history.highToaStartUpdateFlag == 0)
		{
			high.tracks[i].history.highToaStartUpdateConsecutiveCount = 0;
		}
	}
}
void trackProcessFutureParametersLow(vector<TrackDefaultClass> &tracks,
		vector<TrackDefaultClass> &tracksNext)
{
	uint16_t i = 0;
	uint16_t nextIndex = 0;

	//iterate over all low tracks to remove duplicate pri types
	//we have to follow the same step when updating or stringing to a high
	//track later
	for (i = 0; i < tracks.size(); i++)
	{
		//if this track has an update
		if (tracks[i].history.lowToaStartUpdateNextFlag == 1)
		{
			//get the index of the future update track
			nextIndex = tracks[i].history.lowIndexUpdateNext;
			//if the current low track has dwell and stagger pri types
			//resolve that into one type only
			if ((tracks[i].pri.accumulatePriDwellSwitchCount > 0)
					&& (tracks[i].pri.accumulatePriStaggerCount > 0))
			{
				//if this pri type gets resolved to one in the future track
				if ((tracksNext[nextIndex].pri.accumulatePriDwellSwitchCount > 0)
						&& (tracksNext[nextIndex].pri.accumulatePriStaggerCount
								== 0))
				{
					//remove all stagger pri from this low track
					trackRemovePriCircByType(tracks[i],
							PriTypeEnum::pri_stagger);
				}
				//if this pri type gets resolved to one in the future track
				if ((tracksNext[nextIndex].pri.accumulatePriDwellSwitchCount
						== 0)
						&& (tracksNext[nextIndex].pri.accumulatePriStaggerCount
								> 0))
				{
					//remove all dwell pri from this low track
					trackRemovePriCircByType(tracks[i],
							PriTypeEnum::pri_dwellswitch);
				}
			}
			//estimate the future pri type variable here, order is important
			trackCountUpdatePriDominantFuture(tracks[i], tracksNext[nextIndex]);
		}
	}
}
void trackRemovePriCircByType(TrackDefaultClass &track, PriTypeEnum priType)
{

	circular_buffer<LimitPriClass>::iterator tIter;

	for (tIter = track.pri.accumulatePri.begin();
			tIter != track.pri.accumulatePri.end();)
	{
		if (tIter->priType == priType)
		{

			tIter = track.pri.accumulatePri.erase(tIter);
		}

		else
		{
			tIter++;
		}
	}

	for (tIter = track.pri.priCircular.begin();
			tIter != track.pri.priCircular.end();)
	{
		if (tIter->priType == priType)
		{

			tIter = track.pri.priCircular.erase(tIter);
		}

		else
		{
			tIter++;
		}
	}
	trackCountUpdatePriAccumulative(track);
}
void trackProcessMaxToaLastIter(DataSensorClass &dataSensor)
{
	uint16_t i;

	for (i = 0; i < dataSensor.tracks.size(); i++)
	{
		if ((dataSensor.trackToaLastMax > 0)
				&& (dataSensor.tracks[i].history.lowToaStartUpdateFlag == 1)
				&& (dataSensor.trackToaLastMax
						< dataSensor.tracks[i].general.toaLast))
		{
			dataSensor.tracks[i].general.toaFirstOverlapAnalysis =
					dataSensor.trackToaLastMax;
		}
		else
		{
			dataSensor.tracks[i].general.toaFirstOverlapAnalysis =
					dataSensor.tracks[i].general.toaFirst;
		}
	}
}
void trackCalcMaxLastToaCurrentIter(DataSensorClass &dataSensor)
{
	uint16_t i = 0;
	uint64_t max = 0;

	for (i = 0; i < dataSensor.tracks.size(); i++)
	{
		if (dataSensor.tracks[i].general.toaLast > max)
		{
			max = dataSensor.tracks[i].general.toaLast;
		}
	}
	if (dataSensor.tracks.size() > 0)
	{
		dataSensor.trackToaLastMax = max;
	}
	else
	{
		dataSensor.trackToaLastMax = 0;
	}
}
void trackCalcOverlapParametersLow(vector<TrackDefaultClass> &tracks,
		vector<TrackDefaultClass> &tracksNext)
{

}
void trackUpdateTrackFirstToaHigh(vector<TrackDefaultClass> &tracksLow,
		vector<TrackDefaultClass> &tracksLowNext,
		vector<TrackDefaultClass> &tracksHigh)
{

	uint16_t i = 0;
	uint16_t j = 0;
	uint16_t k = 0;
	uint16_t lowNextIndex = 0;

	//iterate over all low tracks
	for (i = 0; i < tracksLow.size(); i++)
	{
		if (tracksLow[i].general.toaLast == 69027000)
			tracksLow[i].general.toaLast = tracksLow[i].general.toaLast * 1;
		//if this track has already been processed, continue
		if (tracksLow[i].general.processingHighFlag == 1)
			continue;
		//iterate over all high tracks
		for (j = 0; j < tracksHigh.size(); j++)
		{
			//if there is a firstToa match, perform an organic update of high
			if (((tracksLow[i].general.toaFirst
					== tracksHigh[j].general.toaFirst)
					|| (tracksLow[i].general.toaFirst
							== tracksHigh[j].last[0].general.toaFirst))
					&& (tracksLow[i].general.toaFirst != 0))
			{

				//if low report is not updated next, simply send its copy in
				//the slot reserved for the lownext report
				//And call the track organic update function
				if (tracksLow[i].history.lowToaStartUpdateNextFlag == 0)
				{
					trackUpdateParameterOrganic(tracksLow[i], tracksLow[i],
							tracksHigh[j], i, i, j);
					tracksLow[i].general.processingHighFlag = 1;
				}
				//if low report is updated in the next iteration, send that
				//along
				//And call the track organic update function

				else if ((tracksLow[i].history.lowToaStartUpdateNextFlag == 1)
						&& (tracksLow[i].history.lowIndexUpdateNext
								< tracksLowNext.size()))
				{
					lowNextIndex = tracksLow[i].history.lowIndexUpdateNext;
					trackUpdateParameterOrganic(tracksLow[i],
							tracksLowNext[lowNextIndex], tracksHigh[j], i, i,
							j);
					tracksLow[i].general.processingHighFlag = 1;
				}
				//if low report is updated in the next iteration, however
				//the updated report is unavailable, simply copy the low report
				//in the slot reserved for the lownext report
				//And call the track organic update function
				else
				{
					trackUpdateParameterOrganic(tracksLow[i], tracksLow[i],
							tracksHigh[j], i, i, j);
					tracksLow[i].general.processingHighFlag = 1;
				}
			}
			else if (tracksHigh[j].last.size() > 0)
			{
				for (k = 0; k < tracksHigh[j].last.size(); k++)
				{
					if ((tracksLow[i].general.toaFirst
							== tracksHigh[j].last[k].general.toaFirst)
							&& (tracksLow[i].general.toaFirst != 0))
					{
						//if low report is not updated next, simply send its copy in
						//the slot reserved for the lownext report
						//And call the track organic update function
						if (tracksLow[i].history.lowToaStartUpdateNextFlag == 0)
						{
							trackUpdateParameterOrganic(tracksLow[i],
									tracksLow[i], tracksHigh[j], i, i, j);
							tracksLow[i].general.processingHighFlag = 1;
						}
						//if low report is updated in the next iteration, send that
						//along
						//And call the track organic update function

						else if ((tracksLow[i].history.lowToaStartUpdateNextFlag
								== 1)
								&& (tracksLow[i].history.lowIndexUpdateNext
										< tracksLowNext.size()))
						{
							lowNextIndex =
									tracksLow[i].history.lowIndexUpdateNext;
							trackUpdateParameterOrganic(tracksLow[i],
									tracksLowNext[lowNextIndex], tracksHigh[j],
									i, i, j);
							tracksLow[i].general.processingHighFlag = 1;
						}
						//if low report is updated in the next iteration, however
						//the updated report is unavailable, simply copy the low report
						//in the slot reserved for the lownext report
						//And call the track organic update function
						else
						{
							trackUpdateParameterOrganic(tracksLow[i],
									tracksLow[i], tracksHigh[j], i, i, j);
							tracksLow[i].general.processingHighFlag = 1;
						}
					}
				}
			}
		}
	}
}
void trackUpdateParameterOrganic(TrackDefaultClass &low,
		TrackDefaultClass &lowNext, TrackDefaultClass &high, uint16_t lowIndex,
		uint16_t lowNextIndex, uint16_t highIndex)
{
	uint16_t i = 0;

	if (ConstHigh::simLogIterationHigh == 1)
	{
		cout << "Track updated Toa H: " << high.general.toaFirst << "-"
				<< high.general.toaLast << " L: " << low.general.toaFirst << "-"
				<< low.general.toaLast << endl;
	}
	//remove duplicate pri types from low report
	if ((low.pri.accumulatePriDwellSwitchCount > 0)
			&& (low.pri.accumulatePriStaggerCount > 0)
			&& (low.pri.accumulatePriFixedCount == 0))
	{
		if (high.last[0].pri.accumulatePriStaggerCount > 0)
		{
			trackRemovePriCircByType(low, PriTypeEnum::pri_dwellswitch);
			trackRemovePriCircByType(low, PriTypeEnum::pri_fixed);
		}
		else if ((high.last[0].pri.accumulatePriDwellSwitchCount > 0)
				&& (high.last[0].pri.accumulatePriFixedCount == 0))
		{
			trackRemovePriCircByType(low, PriTypeEnum::pri_stagger);
		}
		//else if(low.history.lowToaStartUpdateNextFlag == 0) {
		//	return; //deny update since this track will not update in the future anyway
		//}
	}
	//end of removing duplicate pri from low reports
	if (((low.pri.accumulatePriDwellSwitchCount > 0)
			&& (low.pri.accumulatePriFixedCount == 0)
			&& (high.last[0].pri.accumulatePriStaggerCount > 0))
			|| ((low.pri.accumulatePriStaggerCount > 0)
					&& (high.last[0].pri.accumulatePriDwellSwitchCount > 0)
					&& (high.last[0].pri.accumulatePriFixedCount == 0)))
	{
		libraryRequestResetTrackParameters(high);
		libraryResetTrackParameters(high);
		if (low.history.lowToaStartUpdateNextFlag == 1)
		{
			if (lowNext.pri.accumulatePriStaggerCount > 0)
			{
				trackRemovePriCircByType(low, PriTypeEnum::pri_dwellswitch);
				trackRemovePriCircByType(low, PriTypeEnum::pri_fixed);
			}
			else if ((lowNext.pri.accumulatePriDwellSwitchCount > 0)
					&& (lowNext.pri.accumulatePriFixedCount == 0))
			{
				trackRemovePriCircByType(low, PriTypeEnum::pri_stagger);
			}
		}
		libraryRequestMatch(high);
		libraryRequestForceUnknown(high);
	}
	else if (((low.pri.accumulatePriDwellSwitchCount > 0)
			/*&& (low.pri.accumulatePriFixedCount == 0)*/&& (high.last[0].pri.accumulatePriStaggerCount
					> 0))
			|| ((low.pri.accumulatePriStaggerCount > 0)
					&& (high.last[0].pri.accumulatePriDwellSwitchCount > 0) /*&&
					 (high.last[0].pri.accumulatePriFixedCount == 0)*/))
	{
		libraryRequestResetTrackParameters(high);
		libraryResetTrackParameters(high);
		if (low.history.lowToaStartUpdateNextFlag == 1)
		{
			if (lowNext.pri.accumulatePriStaggerCount > 0)
			{
				trackRemovePriCircByType(low, PriTypeEnum::pri_dwellswitch);
				trackRemovePriCircByType(low, PriTypeEnum::pri_fixed);
			}
			else if ((lowNext.pri.accumulatePriDwellSwitchCount > 0)
					&& (lowNext.pri.accumulatePriFixedCount == 0))
			{
				trackRemovePriCircByType(low, PriTypeEnum::pri_stagger);
			}
		}
		libraryRequestMatch(high);
		libraryRequestForceUnknown(high);
	}
	//invalidate previous high library match based on match category difference
	//between high and low. note batch id is also the name for category
	//there is a slight flaw: we need the library match to be solid so single
	//pri based matches need to be catered for
	else if ((high.libmatch.active.flagKnownLibrary == 1)
			&& (low.libmatch.active.flagKnownLibrary == 1))
	{
		//batch id comparison
		if ((high.libmatch.active.highestMatchList[0].emitterBatchId
				!= low.libmatch.active.highestMatchList[0].emitterBatchId))
		{
			libraryRequestResetTrackParameters(high);
			libraryResetTrackParameters(high);
			libraryRequestMatch(high);
			libraryRequestForceUnknown(high);
		}
	}
	if (high.general.timeIterCountLifetimeGlobal < 2)
	{
		libraryRequestMatch(high);
	}
	if ((high.rf.accumulateRf.size() == 0) && (low.rf.accumulateRf.size() > 0))
	{
		libraryRequestForceUnknown(high);
		libraryRequestMatch(high);
	}
	//resume
	/*if ((high.last[0].pri.accumulatePriDwellSwitchCount > 0)
	 && (high.last[0].pri.accumulatePriStaggerCount == 0)
	 && (low.pri.accumulatePriDwellSwitchCount == 0)
	 && (low.pri.accumulatePriStaggerCount > 0))
	 {
	 high.pri.accumulatePri.clear();
	 }
	 //empty high pri buffer if there was a pri type mismatch with previous
	 if ((high.last[0].pri.accumulatePriDwellSwitchCount == 0)
	 && (high.last[0].pri.accumulatePriStaggerCount > 0)
	 && (low.pri.accumulatePriDwellSwitchCount > 0)
	 && (low.pri.accumulatePriStaggerCount == 0))
	 {
	 high.pri.accumulatePri.clear();
	 }
	 //invalidate previous high library match based on certain pri conditions
	 //first check if the previous library match was valid
	 if (high.libmatch.current.flagKnownLibrary == 1)
	 {
	 //if the previous library match had dwell, while low has stagger
	 if ((high.libmatch.current.highestMatchList[0].dwellCount > 0)
	 && (low.pri.accumulatePriDwellSwitchCount == 0)
	 && (low.pri.accumulatePriStaggerCount > 0))
	 {
	 libraryRequestMatch(high);
	 }
	 //if the previous library match had stagger while low has dwell switch
	 if ((high.libmatch.current.highestMatchList[0].staggerCount > 0)
	 && (low.pri.accumulatePriStaggerCount == 0)
	 && (low.pri.accumulatePriDwellSwitchCount > 0))
	 {
	 libraryRequestMatch(high);
	 }
	 }*/

	high.general.toaLast = low.general.toaLast;
	high.general.timeIterCountLifetime = 0;
	high.history.highToaStartUpdateConsecutiveCount++;
	high.history.highToaStartUpdateTotalCount++;
	high.history.highToaStartUpdateFlag = 1;
	high.history.highTrackContinuationIterFlag = 1;
	high.history.idsAssociateTracksLow.push_back(low.general.trackID);
	high.history.idsAssociateTracksIterLow.push_back(low.general.trackID);
	high.history.indexAssociateTracksIterLow.push_back(lowIndex);
	//decide which ones to keep and update, in both organic and string versions
	if (						//(low.pri.accumulatePriComplexCount == 0) &&
	(low.pri.accumulatePriDiffToaCount == 0))
	{
		if (low.history.lowToaStartUpdateFlag == 1)
			high.history.stitchedUpdateCount = high.history.stitchedUpdateCount
					+ 1;
		if (low.history.lowToaStartUpdateNextFlag == 1)
			high.history.stitchedUpdateNextCount =
					high.history.stitchedUpdateNextCount + 1;
	}
	else
	{
		if (low.history.lowToaStartUpdateFlag == 1)
			high.history.stitchedUpdateDiffToaCount =
					high.history.stitchedUpdateDiffToaCount + 1;
		if (low.history.lowToaStartUpdateNextFlag == 1)
			high.history.stitchedUpdateNextDiffToaCount =
					high.history.stitchedUpdateNextDiffToaCount + 1;
		high.history.stitchedDiffToaCount = high.history.stitchedDiffToaCount
				+ 1;
	}
	if ((low.pri.accumulatePriJitterCount > 0)
			|| (low.pri.accumulatePriComplexCount > 0))
	{
		high.history.stitchedJitterAndComplexCount =
				high.history.stitchedJitterAndComplexCount + 1;
	}
	high.history.totalToaOverlap = high.history.totalToaOverlap
			+ low.history.durationToaOverlapMaxLow;
	high.history.totalPulseOverlap = high.history.totalPulseOverlap
			+ low.history.countPulseOverlapMaxLow;
	high.history.totalPulseCount = high.history.totalPulseCount
			+ low.power.pulseCountSinceLastReport;
	high.history.stitchedCount = high.history.stitchedCount + 1;
	high.history.activeFlag = 1;
	high.history.flagUpdateOnceIterHigh = 1;
	//high.history.stitchedCount = low.history.stitchedCount;
	//high.history.stitchedJitterCount = low.history.stitchedJitterCount;
	//high.history.stitchedComplexCount = low.history.stitchedComplexCount;
	//high.history.totalToaOverlap = low.history.totalToaOverlap;
	//high.history.totalPulseOverlap = low.history.totalPulseOverlap;
	//high.history.totalPulseCount = low.history.totalPulseCount;
	//high.history.stitchedUpdateCount = low.history.stitchedUpdateCount;
	//high.history.stitchedUpdateNextCount = low.history.stitchedUpdateNextCount;
	//high.history.stitchedUpdateComplexCount = low.history.stitchedUpdateComplexCount;
	//high.history.stitchedUpdateNextComplexCount = low.history.stitchedUpdateNextComplexCount;
	//review needed
	high.power.toaMinStartPower = low.power.toaMinStartPower;
	high.power.toaMinEndPower = low.power.toaMinEndPower;
	high.power.pxAntennaFirst = low.power.pxAntennaFirst;
	high.power.pxAntennaLast = low.power.pxAntennaLast;
	high.power.pyAntennaFirst = low.power.pyAntennaFirst;
	high.power.pyAntennaLast = low.power.pyAntennaLast;
	high.power.pxValueFirst = low.power.pxValueFirst;
	high.power.pxValueLast = low.power.pxValueLast;
	//high.power.pxMax = low.power.pxMax;//reported for bearing calculation at high
	//high.power.pyMax = low.power.pyMax;//reported for bearing calculation at high
	high.power.maxPowerToa = low.power.maxPowerToa;	//reported for arp calculation at high
	high.power.pyValueFirst = low.power.pyValueFirst;
	high.power.pyValueLast = low.power.pyValueLast;
	high.power.sumPxPyMax = low.power.sumPxPyMax;
	high.power.quadrantCodeFirst = low.power.quadrantCodeFirst;
	high.power.quadrantCodeLast = low.power.quadrantCodeLast;
	high.power.primaryAntennaFirst = low.power.primaryAntennaFirst;
	high.power.primaryAntennaLast = low.power.primaryAntennaLast;
	//high.power.groundIlluminationFlag = low.power.groundIlluminationFlag;
	//high.power.groundIlluminationAxis = low.power.groundIlluminationAxis;
	//high.power.scanTypeFirst = low.power.scanTypeFirst;
	//high.power.scanTypeLast = low.power.scanTypeLast;
	//high.power.scanIlluminationTimeFirst = low.power.scanIlluminationTimeFirst;
	//high.power.scanIlluminationTimeLast = low.power.scanIlluminationTimeLast;
	//high.power.scanPulseCountFirst = low.power.scanPulseCountFirst;
	//high.power.scanPulseCountLast = low.power.scanPulseCountLast;
	//high.power.arpFirst = low.power.arpFirst;
	//high.power.arpLast = low.power.arpLast;
	//high.power.timeToaScanAnalysisLast = low.power.timeToaScanAnalysisLast;
	//high.power.flagTrackingLowFirst = low.power.flagTrackingLowFirst;
	//high.power.flagTrackingLowLast = low.power.flagTrackingLowLast;
	//high.power.flagTrackingEstimateHigh = low.power.flagTrackingEstimateHigh;
	high.power.countQuadrantCodesIter[0] = high.power.countQuadrantCodesIter[0]
			+ low.power.countQuadrantCodesIter[0];
	high.power.countQuadrantCodesIter[1] = high.power.countQuadrantCodesIter[1]
			+ low.power.countQuadrantCodesIter[1];
	high.power.countQuadrantCodesIter[2] = high.power.countQuadrantCodesIter[2]
			+ low.power.countQuadrantCodesIter[2];
	high.power.countQuadrantCodesIter[3] = high.power.countQuadrantCodesIter[3]
			+ low.power.countQuadrantCodesIter[3];
	if (low.power.pxMax > high.power.pxMaxQuadrant[low.power.quadrantCodeMax])
	{
		high.power.pxMaxQuadrant[low.power.quadrantCodeMax] = low.power.pxMax;
	}
	if (low.power.pyMax > high.power.pyMaxQuadrant[low.power.quadrantCodeMax])
	{
		high.power.pyMaxQuadrant[low.power.quadrantCodeMax] = low.power.pyMax;
	}
	if (low.power.scanTypeLast == EmitterArpEnum::scan_lock)
	{
		high.power.countTrackingModeLowIterHigh++;
	}
	else if (low.power.scanTypeLast == EmitterArpEnum::scan_circular)
	{
		high.power.countScanningModeLowIterHigh++;
	}
	trackCalcQuadrantAndPxPyHigh(high);
	high.angle.bearingLast = doaCalculateAngle(high.power.pxMax,
			high.power.pyMax, high.power.quadrantCodeMax,
			high.rf.getBandCodeLegacy(), Constant::systemBandCountOperate);
	for (i = 0; i < 4; i++)
	{
		high.power.countQuadrantCodesIter[i] =
				high.power.countQuadrantCodesIter[i]
						+ low.power.countQuadrantCodesIter[i];
	}

	high.power.flagScanAnalysisRequested = 1;
	IlluminationCycleInfo::takeUnion(high.power.illuminationInfo,
			low.power.illuminationInfo);
	//copy pri list from low track into high track
	for (i = 0; i < low.pri.accumulatePri.size(); i++)
	{
		//if high pri buffer already has entries in it
		if (high.pri.accumulatePri.size() > 0)
		{
			uint16_t lastIndex = high.pri.accumulatePri.size() - 1;
			//if the last entry was not the same as this one
			if (!((labs(
					(int32_t) high.pri.accumulatePri[lastIndex].priLow
							- (int32_t) low.pri.accumulatePri[i].priLow) < 32)
					&& (high.pri.accumulatePri[lastIndex].priType
							== low.pri.accumulatePri[i].priType)))
			{
				//push value into high pri accumulative buffer
				high.pri.accumulatePri.push_back(low.pri.accumulatePri[i]);
				high.history.countConsecutiveSameSpotFixedPri = 0;
			}
			else
			{
				if (low.pri.curPriType == PriTypeEnum::pri_fixed)
				{
					high.history.countConsecutiveSameSpotFixedPri++;
				}
				if (high.history.countConsecutiveSameSpotFixedPri > 6)
				{
					high.history.countConsecutiveSameSpotFixedPri = 0;
					high.pri.accumulatePri.clear();
					high.pri.accumulatePri.push_back(low.pri.accumulatePri[i]);
					high.rf.accumulateRf.resize(0);
					high.rf.accumulateRfCount = 0;

					high.pw.accumulatePw.resize(0);
					high.pw.accumulatePwCount = 0;
				}
			}
		}
		//if the high pri buffer was empty
		else
		{
			//push value into high pri accumulative buffer
			high.pri.accumulatePri.push_back(low.pri.accumulatePri[i]);

			high.history.countConsecutiveSameSpotFixedPri = 0;
		}
	}
	trackCountUpdatePriAccumulative(high);//re count the accumulated pri buffer
	//copy pw list from low track into high track
	for (i = 0; i < low.pw.accumulatePw.size(); i++)
	{
		//if high pw buffer already has entries in it
		if (high.pw.accumulatePw.size() > 0)
		{
			uint16_t lastIndex = high.pw.accumulatePw.size() - 1;
			//if the last entry was not the same as this one
			if (!((high.pw.accumulatePw[lastIndex].pwLow
					== low.pw.accumulatePw[i].pwLow)
					&& (high.pw.accumulatePw[lastIndex].pwType
							== low.pw.accumulatePw[i].pwType)))
			{
				//push value into high pw accumulative buffer
				high.pw.accumulatePw.push_back(low.pw.accumulatePw[i]);
			}
		}
		//if the high pw buffer was empty
		else
		{
			//push value into high pw accumulative buffer
			high.pw.accumulatePw.push_back(low.pw.accumulatePw[i]);
		}
	}
	//copy rf list from low track into high track
	for (i = 0; i < low.rf.accumulateRf.size(); i++)
	{
		//if high rf buffer already has entries in it
		if (high.rf.accumulateRf.size() > 0)
		{
			uint16_t lastIndex = high.rf.accumulateRf.size() - 1;
			//if the last entry was not the same as this one
			if (!((high.rf.accumulateRf[lastIndex].rfLow
					== low.rf.accumulateRf[i].rfLow)
					&& (high.rf.accumulateRf[lastIndex].rfType
							== low.rf.accumulateRf[i].rfType)))
			{
				//push value into high rf accumulative buffer
				high.rf.accumulateRf.push_back(low.rf.accumulateRf[i]);
			}
		}
		//if the high rf buffer was empty
		else
		{
			//push value into high rf accumulative buffer
			high.rf.accumulateRf.push_back(low.rf.accumulateRf[i]);
		}
	}
	high.last.push_front(low);
	//trackUpdateAccumlatedPwInfo(high);
	//trackUpdateAccumlatedPriInfo(high);
	libraryMatchEmitterPreprocess(high, Model::library, Model::libraryMeta);
	libraryMatchEmitter(high, Model::library, Model::libraryMeta);
	libraryMatchEmitterPostprocess(high, Model::library, Model::libraryMeta,
			Model::cfdLibrary);
}
void trackUpdateParameterString(TrackDefaultClass &low,
		TrackDefaultClass &lowNext, TrackDefaultClass &high, uint16_t lowIndex,
		uint16_t lowNextIndex, uint16_t highIndex)
{
	uint16_t i = 0;
	high.general.toaLast = low.general.toaLast;
	high.general.timeIterCountLifetime = 0;
	//high.history.highToaStartUpdateConsecutiveCount++;
	//high.history.highToaStartUpdateTotalCount++;
	//high.history.highToaStartUpdateFlag = 1;
	if (high.history.highTrackContinuationIterFlag != 0)
	{
		high.history.highTrackContinuationIterFlag = 1;
	}
	high.history.idsAssociateTracksLow.push_back(low.general.trackID);
	high.history.idsAssociateTracksIterLow.push_back(low.general.trackID);
	high.history.indexAssociateTracksIterLow.push_back(lowIndex);
	//decide which ones to keep and update, in both organic and string versions
	if (	//(low.pri.accumulatePriComplexCount == 0) &&
	(low.pri.accumulatePriDiffToaCount == 0))
	{
		if (low.history.lowToaStartUpdateFlag == 1)
			high.history.stitchedUpdateCount = high.history.stitchedUpdateCount
					+ 1;
		if (low.history.lowToaStartUpdateNextFlag == 1)
			high.history.stitchedUpdateNextCount =
					high.history.stitchedUpdateNextCount + 1;
	}
	else
	{
		if (low.history.lowToaStartUpdateFlag == 1)
			high.history.stitchedUpdateDiffToaCount =
					high.history.stitchedUpdateDiffToaCount + 1;
		if (low.history.lowToaStartUpdateNextFlag == 1)
			high.history.stitchedUpdateNextDiffToaCount =
					high.history.stitchedUpdateNextDiffToaCount + 1;
		high.history.stitchedDiffToaCount = high.history.stitchedDiffToaCount
				+ 1;
	}
	if ((low.pri.accumulatePriJitterCount > 0)
			|| (low.pri.accumulatePriComplexCount > 0))
	{
		high.history.stitchedJitterAndComplexCount =
				high.history.stitchedJitterAndComplexCount + 1;
	}
	high.history.totalToaOverlap = high.history.totalToaOverlap
			+ low.history.durationToaOverlapMaxLow;
	high.history.totalPulseOverlap = high.history.totalPulseOverlap
			+ low.history.countPulseOverlapMaxLow;
	high.history.totalPulseCount = high.history.totalPulseCount
			+ low.power.pulseCountSinceLastReport;
	high.history.stitchedCount = high.history.stitchedCount + 1;
	high.history.activeFlag = 1;
	high.history.flagUpdateOnceIterHigh = 1;
	//high.history.stitchedCount = low.history.stitchedCount;
	//high.history.stitchedJitterCount = low.history.stitchedJitterCount;
	//high.history.stitchedComplexCount = low.history.stitchedComplexCount;
	//high.history.totalToaOverlap = low.history.totalToaOverlap;
	//high.history.totalPulseOverlap = low.history.totalPulseOverlap;
	//high.history.totalPulseCount = low.history.totalPulseCount;
	//high.history.stitchedUpdateCount = low.history.stitchedUpdateCount;
	//high.history.stitchedUpdateNextCount = low.history.stitchedUpdateNextCount;
	//high.history.stitchedUpdateComplexCount = low.history.stitchedUpdateComplexCount;
	//high.history.stitchedUpdateNextComplexCount = low.history.stitchedUpdateNextComplexCount;
	//review needed
	high.power.toaMinStartPower = low.power.toaMinStartPower;
	high.power.toaMinEndPower = low.power.toaMinEndPower;
	high.power.pxAntennaFirst = low.power.pxAntennaFirst;
	high.power.pxAntennaLast = low.power.pxAntennaLast;
	high.power.pyAntennaFirst = low.power.pyAntennaFirst;
	high.power.pyAntennaLast = low.power.pyAntennaLast;
	high.power.pxValueFirst = low.power.pxValueFirst;
	high.power.pxValueLast = low.power.pxValueLast;
	//high.power.pxMax = low.power.pxMax;//reported for bearing calculation at high
	//high.power.pyMax = low.power.pyMax;//reported for bearing calculation at high
	high.power.maxPowerToa = low.power.maxPowerToa;	//reported for arp calculation at high
	high.power.pyValueFirst = low.power.pyValueFirst;
	high.power.pyValueLast = low.power.pyValueLast;
	high.power.sumPxPyMax = low.power.sumPxPyMax;
	high.power.quadrantCodeFirst = low.power.quadrantCodeFirst;
	high.power.quadrantCodeLast = low.power.quadrantCodeLast;
	high.power.primaryAntennaFirst = low.power.primaryAntennaFirst;
	high.power.primaryAntennaLast = low.power.primaryAntennaLast;
	high.power.flagScanAnalysisRequested = 1;
	IlluminationCycleInfo::takeUnion(high.power.illuminationInfo,
			low.power.illuminationInfo);

	//high.power.groundIlluminationFlag = low.power.groundIlluminationFlag;
	//high.power.groundIlluminationAxis = low.power.groundIlluminationAxis;
	//high.power.scanTypeFirst = low.power.scanTypeFirst;
	//high.power.scanTypeLast = low.power.scanTypeLast;
	//high.power.scanIlluminationTimeFirst = low.power.scanIlluminationTimeFirst;
	//high.power.scanIlluminationTimeLast = low.power.scanIlluminationTimeLast;
	//high.power.scanPulseCountFirst = low.power.scanPulseCountFirst;
	//high.power.scanPulseCountLast = low.power.scanPulseCountLast;
	//high.power.arpFirst = low.power.arpFirst;
	//high.power.arpLast = low.power.arpLast;
	//high.power.timeToaScanAnalysisLast = low.power.timeToaScanAnalysisLast;
	//high.power.flagTrackingLowFirst = low.power.flagTrackingLowFirst;
	//high.power.flagTrackingLowLast = low.power.flagTrackingLowLast;
	//high.power.flagTrackingEstimateHigh = low.power.flagTrackingEstimateHigh;
	high.power.countQuadrantCodesIter[0] = high.power.countQuadrantCodesIter[0]
			+ low.power.countQuadrantCodesIter[0];
	high.power.countQuadrantCodesIter[1] = high.power.countQuadrantCodesIter[1]
			+ low.power.countQuadrantCodesIter[1];
	high.power.countQuadrantCodesIter[2] = high.power.countQuadrantCodesIter[2]
			+ low.power.countQuadrantCodesIter[2];
	high.power.countQuadrantCodesIter[3] = high.power.countQuadrantCodesIter[3]
			+ low.power.countQuadrantCodesIter[3];
	if ((low.power.pxMax + low.power.pyMax)
			> (high.power.pxMaxQuadrant[low.power.quadrantCodeMax]
					+ high.power.pyMaxQuadrant[low.power.quadrantCodeMax]))
	{
		high.power.pxMaxQuadrant[low.power.quadrantCodeMax] = low.power.pxMax;
		high.power.pyMaxQuadrant[low.power.quadrantCodeMax] = low.power.pyMax;
	}
//	if (low.power.pyMax > high.power.pyMaxQuadrant[low.power.quadrantCodeMax])
//	{
//		high.power.pyMaxQuadrant[low.power.quadrantCodeMax] = low.power.pyMax;
//	}
	if (low.power.scanTypeLast == EmitterArpEnum::scan_lock)
	{
		high.power.countTrackingModeLowIterHigh++;
	}
	else if (low.power.scanTypeLast == EmitterArpEnum::scan_circular)
	{
		high.power.countScanningModeLowIterHigh++;
	}
	trackCalcQuadrantAndPxPyHigh(high);
	high.angle.bearingLast = doaCalculateAngle(high.power.pxMax,
			high.power.pyMax, high.power.quadrantCodeMax,
			high.rf.getBandCodeLegacy(), Constant::systemBandCountOperate);
	for (i = 0; i < 4; i++)
	{
		high.power.countQuadrantCodesIter[i] =
				high.power.countQuadrantCodesIter[i]
						+ low.power.countQuadrantCodesIter[i];
	}
	//empty high pri buffer if there was a pri type mismatch with previous
	/*if ((high.last[0].pri.accumulatePriDwellSwitchCount > 0)
	 && (high.last[0].pri.accumulatePriStaggerCount == 0)
	 && (low.pri.accumulatePriDwellSwitchCount == 0)
	 && (low.pri.accumulatePriStaggerCount > 0))
	 {
	 high.pri.accumulatePri.clear();
	 }
	 //empty high pri buffer if there was a pri type mismatch with previous
	 if ((high.last[0].pri.accumulatePriDwellSwitchCount == 0)
	 && (high.last[0].pri.accumulatePriStaggerCount > 0)
	 && (low.pri.accumulatePriDwellSwitchCount > 0)
	 && (low.pri.accumulatePriStaggerCount == 0))
	 {
	 high.pri.accumulatePri.clear();
	 }*/
	//invalidate previous high library match based on certain pri conditions
	//first check if the previous library match was valid
	if (high.libmatch.active.flagKnownLibrary == 1)
	{
		//if the previous library match had dwell, while low has stagger
		if ((high.libmatch.active.highestMatchList[0].dwellCount > 0)
				&& (low.pri.accumulatePriDwellSwitchCount == 0)
				&& (low.pri.accumulatePriStaggerCount > 0))
		{
			libraryRequestMatch(high);
		}
		//if the previous library match had stagger while low has dwell switch
		if ((high.libmatch.active.highestMatchList[0].staggerCount > 0)
				&& (low.pri.accumulatePriStaggerCount == 0)
				&& (low.pri.accumulatePriDwellSwitchCount > 0))
		{
			libraryRequestMatch(high);
		}
	}
	//invalidate previous high library match based on match category difference
	//between high and low. note batch id is also the name for category
	//there is a slight flaw: we need the library match to be solid so single
	//pri based matches need to be catered for
	if ((high.libmatch.active.flagKnownLibrary == 1)
			&& (low.libmatch.active.flagKnownLibrary == 1))
	{
		//batch id comparison
		if ((high.libmatch.active.highestMatchList[0].emitterBatchId
				!= low.libmatch.active.highestMatchList[0].emitterBatchId))
		{
			libraryRequestMatch(high);
		}
	}
	if (high.general.timeIterCountLifetimeGlobal < 2)
	{
		libraryRequestMatch(high);
	}
	if ((high.rf.accumulateRf.size() == 0) && (low.rf.accumulateRf.size() > 0))
	{
		libraryRequestForceUnknown(high);
		libraryRequestMatch(high);
	}
	//copy pri list from low track into high track
	if ((high.libmatch.active.flagKnownLibrary == 1)
			&& (low.libmatch.active.flagKnownLibrary == 1))
	{
		//batch id comparison
		if ((high.libmatch.active.highestMatchList[0].emitterBatchId
				== low.libmatch.active.highestMatchList[0].emitterBatchId)
				&& (high.libmatch.active.highestMatchList[0].ID
						!= low.libmatch.active.highestMatchList[0].ID))
		{
			high.pri.accumulatePri.resize(0);
			high.rf.accumulateRf.resize(0);
			high.pw.accumulatePw.resize(0);
		}
	}
	for (i = 0; i < low.pri.accumulatePri.size(); i++)
	{
		//if high pri buffer already has entries in it
		if (high.pri.accumulatePri.size() > 0)
		{
			uint16_t lastIndex = high.pri.accumulatePri.size() - 1;
			//if the last entry was not the same as this one
			if (!((labs(
					(int32_t) high.pri.accumulatePri[lastIndex].priLow
							- (int32_t) low.pri.accumulatePri[i].priLow) < 32)
					&& (high.pri.accumulatePri[lastIndex].priType
							== low.pri.accumulatePri[i].priType)))
			{
				//push value into high pri accumulative buffer
				high.pri.accumulatePri.push_back(low.pri.accumulatePri[i]);
				high.history.countConsecutiveSameSpotFixedPri = 0;
			}
			else
			{
				if (low.pri.curPriType == PriTypeEnum::pri_fixed)
				{
					high.history.countConsecutiveSameSpotFixedPri++;
				}
				if (high.history.countConsecutiveSameSpotFixedPri > 6)
				{
					high.history.countConsecutiveSameSpotFixedPri = 0;
					high.pri.accumulatePri.clear();
					high.pri.accumulatePri.push_back(low.pri.accumulatePri[i]);
					high.rf.accumulateRf.resize(0);
					high.rf.accumulateRfCount = 0;

					high.pw.accumulatePw.resize(0);
					high.pw.accumulatePwCount = 0;
				}
			}
		}
		//if the high pri buffer was empty
		else
		{
			//push value into high pri accumulative buffer
			high.pri.accumulatePri.push_back(low.pri.accumulatePri[i]);

			high.history.countConsecutiveSameSpotFixedPri = 0;
		}
	}
	trackCountUpdatePriAccumulative(high);//re count the accumulated pri buffer

	//copy pw list from low track into high track
	for (i = 0; i < low.pw.accumulatePw.size(); i++)
	{
		//if high pw buffer already has entries in it
		if (high.pw.accumulatePw.size() > 0)
		{
			uint16_t lastIndex = high.pw.accumulatePw.size() - 1;
			//if the last entry was not the same as this one
			if (!((high.pw.accumulatePw[lastIndex].pwLow
					== low.pw.accumulatePw[i].pwLow)
					&& (high.pw.accumulatePw[lastIndex].pwType
							== low.pw.accumulatePw[i].pwType)))
			{
				//push value into high pw accumulative buffer
				high.pw.accumulatePw.push_back(low.pw.accumulatePw[i]);
			}
		}
		//if the high pw buffer was empty
		else
		{
			//push value into high pw accumulative buffer
			high.pw.accumulatePw.push_back(low.pw.accumulatePw[i]);
		}
	}

	//copy rf list from low track into high track
	for (i = 0; i < low.rf.accumulateRf.size(); i++)
	{
		//if high rf buffer already has entries in it
		if (high.rf.accumulateRf.size() > 0)
		{
			uint16_t lastIndex = high.rf.accumulateRf.size() - 1;
			//if the last entry was not the same as this one
			if (!((high.rf.accumulateRf[lastIndex].rfLow
					== low.rf.accumulateRf[i].rfLow)
					&& (high.rf.accumulateRf[lastIndex].rfType
							== low.rf.accumulateRf[i].rfType)))
			{
				//push value into high rf accumulative buffer
				high.rf.accumulateRf.push_back(low.rf.accumulateRf[i]);
			}
		}
		//if the high rf buffer was empty
		else
		{
			//push value into high rf accumulative buffer
			high.rf.accumulateRf.push_back(low.rf.accumulateRf[i]);
		}
	}
	//library match is not done here
	//libraryMatchEmitter(high,Model::library,Model::libraryMeta);

	high.last.push_front(low);
//	trackUpdateAccumlatedPwInfo(high);
//	trackUpdateAccumlatedPriInfo(high);
}

void trackUpdateAccumlatedPwInfo(TrackDefaultClass &high)
{
	// update pw info in accumulatedPw from stringed/updated low track history
	int16_t startPoint = 3;
	int16_t historySize = high.last.size();
	startPoint = (startPoint < historySize ? startPoint : historySize);
	high.pw.accumulatePw.resize(0);
	for (int16_t ii = startPoint - 1; ii > -1; ii--)
	{
		for (uint16_t i = 0; i < high.last[ii].pw.accumulatePw.size(); i++)
		{
//			if (low.pw.accumulatePw[i].pwAvg>300 &&low.pw.accumulatePw[i].pwAvg<610)
//			{
//				low.pw.accumulatePw[i].pwAvg=low.pw.accumulatePw[i].pwAvg*1;
//			}
			//if high pw buffer already has entries in it
			if (high.pw.accumulatePw.size() > 0)
			{
				uint16_t lastIndex = high.pw.accumulatePw.size() - 1;
//				if the last entry was not the same as this one
				if (!((high.pw.accumulatePw[lastIndex].pwLow
						== high.last[ii].pw.accumulatePw[i].pwLow)
						&& (high.pw.accumulatePw[lastIndex].pwType
								== high.last[ii].pw.accumulatePw[i].pwType)))
				{
					//push value into high pw accumulative buffer
					high.pw.accumulatePw.push_back(
							high.last[ii].pw.accumulatePw[i]);
				}
			}
			//if the high pw buffer was empty
			else
			{
				//push value into high pw accumulative buffer
				high.pw.accumulatePw.push_back(
						high.last[ii].pw.accumulatePw[i]);
			}
		}
	}

	high.pw.accumulatePwCount = high.pw.accumulatePw.size();
}

void trackUpdateAccumlatedPriInfo(TrackDefaultClass &high)
{

	int16_t startPoint = 3;
	int16_t historySize = high.last.size();
	startPoint = (startPoint < historySize ? startPoint : historySize);
	high.pri.accumulatePri.resize(0);
	for (int16_t ii = startPoint - 1; ii > -1; ii--)
	{
		for (uint16_t i = 0; i < high.last[ii].pri.accumulatePri.size(); i++)
		{
			//if high pri buffer already has entries in it
			if (high.pri.accumulatePri.size() > 0)
			{
				uint16_t lastIndex = high.pri.accumulatePri.size() - 1;
				//if the last entry was not the same as this one
				if (!((high.pri.accumulatePri[lastIndex].priLow
						== high.last[ii].pri.accumulatePri[i].priLow)
						&& (high.pri.accumulatePri[lastIndex].priType
								== high.last[ii].pri.accumulatePri[i].priType)))
				{
					//push value into high pri accumulative buffer
					high.pri.accumulatePri.push_back(
							high.last[ii].pri.accumulatePri[i]);
				}
			}
			//if the high pri buffer was empty
			else
			{
				//push value into high pri accumulative buffer
				high.pri.accumulatePri.push_back(
						high.last[ii].pri.accumulatePri[i]);
			}
		}
	}

	trackCountUpdatePriAccumulative(high);//re count the accumulated pri buffer

}
void trackMatchLibraryTest(DataSensorClass &high)
{

	for (UWord32 index = 0; index < high.tracks.size(); index++)
	{
		//cout<<"RF Type"<<high.tracks[index].rf.curRf<<"\n";
		high.tracks[index].pri.accumulatePri =
				high.tracks[index].pri.priCircular;
		high.tracks[index].pw.accumulatePw = high.tracks[index].pw.pwCircular;
		high.tracks[index].rf.accumulateRf = high.tracks[index].rf.rfCircular;

		libraryMatchEmitter(high.tracks[index], Model::library,
				Model::libraryMeta);
	}
}
void trackMatchLibraryAllLow(DataSensorClass &low)
{
	for (uint32_t index = 0; index < low.tracks.size(); index++)
	{
		low.tracks[index].libmatch.flagRequestLibraryMatch = 1;
		libraryMatchEmitter(low.tracks[index], Model::library,
				Model::libraryMeta);
		low.tracks[index].libmatch.flagRequestLibraryMatch = 0;

		if (low.tracks[index].libmatch.last.highestMatchListCount > 0)
		{
			if (low.tracks[index].libmatch.last.highestMatchList[0].Total
					>= low.tracks[index].libmatch.last.thresholdScoreKnown)
			{
				low.tracks[index].libmatch.last.flagKnownLibrary = 1;
				low.tracks[index].libmatch.last.flagKnownPartialLibrary = 1;
			}
			else
			{
				low.tracks[index].libmatch.last.flagKnownLibrary = 0;
				low.tracks[index].libmatch.last.flagKnownPartialLibrary = 1;
			}
		}
		else
		{
			low.tracks[index].libmatch.last.flagKnownLibrary = 0;
			low.tracks[index].libmatch.last.flagKnownPartialLibrary = 0;
		}

		low.tracks[index].libmatch.active = low.tracks[index].libmatch.last;
	}
	libraryAssignSymbolEmitterAll(low.tracks, Model::library);
}
void trackMatchLibraryAllHigh(DataSensorClass &high)
{
	for (uint32_t index = 0; index < high.tracks.size(); index++)
	{
		if(high.tracks[index].general.toaLast == 207810680)
			high.tracks[index].general.toaLast=high.tracks[index].general.toaLast*1;
		if (high.tracks[index].libmatch.timeIterCountLibMatchExec
				>= high.tracks[index].libmatch.timeIterLimitLibMatchExec)
		{
			libraryRequestMatch(high.tracks[index]);
		}
		if (high.tracks[index].libmatch.flagRequestLibraryMatch == 1)
		{
			libraryMatchEmitterPreprocess(high.tracks[index], Model::library,
					Model::libraryMeta);
			libraryMatchEmitter(high.tracks[index], Model::library,
					Model::libraryMeta);
			libraryMatchEmitterPostprocess(high.tracks[index], Model::library,
					Model::libraryMeta, Model::cfdLibrary);
		}
	}
}
uint8_t trackCreateEntryParameterEnvironment(DataSensorClass &dataSensor,
		TrackDefaultClass &track)
{

	DataEnvironmentClass environment;

	if (dataSensor.environmentParameterHistory.size()
			>= ConstHigh::trackLimitMaxEnvironmentSize)
	{
		return 0;
	}
	environment.bandCode = track.rf.bandCode;
	environment.bearing = track.angle.bearingLast;
	environment.updateCount = 1;
	if (track.last[0].pri.accumulatePriStaggerCount > 0)
	{
		environment.priType = PriTypeEnum::pri_stagger;
		environment.priCount = track.last[0].pri.accumulatePriStaggerCount;
		environment.priSum = track.last[0].pri.getStaggerPhaseMinSpot().frame;
		environment.priLow = track.last[0].pri.getStaggerPhaseMinSpot().priLow;
		environment.priHigh = track.last[0].pri.getStaggerPhaseMaxSpot().priLow;
	}
	else if (track.last[0].pri.accumulatePriDwellSwitchCount > 0)
	{
		environment.priType = PriTypeEnum::pri_dwellswitch;
		environment.priCount = track.last[0].pri.accumulatePriDwellSwitchCount;
		environment.priLow = track.last[0].pri.getDwellSwitchMinSpot().priLow;
		environment.priHigh = track.last[0].pri.getDwellSwitchMaxSpot().priLow;
	}
	else if (track.last[0].pri.accumulatePriSlidingCount > 0)
	{
		environment.priType = PriTypeEnum::pri_sliding;
		environment.priCount = track.last[0].pri.accumulatePriSlidingCount;
		environment.priLow = track.last[0].pri.getSlidingMaxRange().priLow;
		environment.priHigh = track.last[0].pri.getSlidingMaxRange().priHigh;
	}
	else if (track.last[0].pri.accumulatePriJitterCount > 0)
	{
		environment.priType = PriTypeEnum::pri_jitter;
		environment.priCount = track.last[0].pri.accumulatePriJitterCount;
		environment.priLow = track.last[0].pri.getJitterMaxRange().priLow;
		environment.priHigh = track.last[0].pri.getJitterMaxRange().priHigh;
	}
	else if (track.last[0].pri.accumulatePriComplexCount > 0)
	{
		environment.priType = PriTypeEnum::pri_complex;
		environment.priCount = track.last[0].pri.accumulatePriComplexCount;
		environment.priLow = track.last[0].pri.getComplexMaxRange().priLow;
		environment.priHigh = track.last[0].pri.getComplexMaxRange().priHigh;
	}
	else if (track.last[0].pri.accumulatePriDiffToaCount > 0)
	{
		environment.priType = PriTypeEnum::pri_difftoa;
		environment.priCount = track.last[0].pri.accumulatePriDiffToaCount;
		environment.priLow = track.last[0].pri.getDiffToaMinSpot().priLow;
		environment.priHigh = track.last[0].pri.getDiffToaMaxSpot().priLow;
	}
	if (track.last[0].pw.accumulatePwCount > 0)
	{
		environment.pwLow = track.last[0].pw.getPwMinSpot().pwLow;
		environment.pwHigh = track.last[0].pw.getPwMaxSpot().pwLow;
	}
	else
	{
		environment.pwLow = 0;
		environment.pwHigh = 0;
	}
	if (track.last[0].rf.accumulateRfCount > 0)
	{
		environment.rfLow = track.last[0].rf.getRfMinSpot().rfLow;
		environment.rfHigh = track.last[0].rf.getRfMaxSpot().rfLow;
		;
	}
	else
	{
		environment.rfLow = 0;
		environment.rfHigh = 0;
	}
	environment.lifeTime = ConstHigh::trackLimitMaxEnvironmentIterLifetime;
	environment.toaStart = track.last[0].general.toaFirst;
	environment.toaEnd = track.last[0].general.toaLast;
	dataSensor.environmentParameterHistory.push_back(environment);
	return 1;
}
uint8_t trackRedeemEntryParameterEnvironment(DataSensorClass &dataSensor,
		TrackDefaultClass &track)
{
	uint8_t parameterExclusion;
	uint8_t index;
	uint8_t bearingDifference;
	uint16_t pwDifference;
	uint16_t rfDifference;
	uint16_t priCountDifference;

	uint8_t smallerAngleValue;
	int16_t aoaDifference;
	uint16_t absDifferenceAoa;

	uint32_t priDifferenceOne;
	uint32_t priDifferenceTwo;
	uint32_t priDifferenceThree;
	uint32_t priDifferenceFour;
	uint32_t priSumDifference;

	for (index = 0; index < dataSensor.environmentParameterHistory.size();
			index++)
	{
		if (dataSensor.environmentParameterHistory[index].lifeTime > 0)
		{
			if (dataSensor.environmentParameterHistory[index].bandCode
					== track.last[0].rf.bandCode)
			{
				pwDifference =
						(int16_t) labs(
								(int32_t) dataSensor.environmentParameterHistory[index].pwLow
										- (int32_t) track.last[0].pw.accumulatePw.back().pwLow);//needs review for variable pulse width
				if (pwDifference < ConstHigh::mergeDeltaStringPwMedium)
				{
					aoaDifference =
							(dataSensor.environmentParameterHistory[index].bearing
									- track.last[0].angle.bearingLast);
					absDifferenceAoa = labs((int16_t) aoaDifference);
					smallerAngleValue = doaRoundAngleDifference(
							absDifferenceAoa);
					bearingDifference = smallerAngleValue;
					if (bearingDifference
							< ConstHigh::mergeLimitEnvironmentRedeemBearing)
					{

						parameterExclusion = 1;

						if ((dataSensor.environmentParameterHistory[index].priType
								== PriTypeEnum::pri_stagger)
								&& (track.last[0].pri.accumulatePriStaggerCount
										> 0))
						{
							priDifferenceOne =
									labs(
											(int32_t) dataSensor.environmentParameterHistory[index].priLow
													- (int32_t) track.last[0].pri.getStaggerPhaseMinSpot().priLow);
							priDifferenceTwo =
									labs(
											(int32_t) dataSensor.environmentParameterHistory[index].priHigh
													- (int32_t) track.last[0].pri.getStaggerPhaseMaxSpot().priLow);
							priSumDifference =
									labs(
											(int32_t) dataSensor.environmentParameterHistory[index].priSum
													- (int32_t) track.last[0].pri.getStaggerPhaseMinSpot().frame);
							if ((priDifferenceOne < 5) || (priDifferenceTwo < 5)
									|| (priSumDifference < 5))
							{
								parameterExclusion = 0;
							}
						}
						else if ((dataSensor.environmentParameterHistory[index].priType
								== PriTypeEnum::pri_dwellswitch)
								&& (track.last[0].pri.accumulatePriDwellSwitchCount
										> 0))
						{
							priDifferenceOne =
									labs(
											(int32_t) dataSensor.environmentParameterHistory[index].priLow
													- (int32_t) track.last[0].pri.getDwellSwitchMinSpot().priLow);
							priDifferenceTwo =
									labs(
											(int32_t) dataSensor.environmentParameterHistory[index].priHigh
													- (int32_t) track.last[0].pri.getDwellSwitchMaxSpot().priLow);
							if (track.last[0].pri.accumulatePriDwellSwitchCount
									> 1)
							{
								priDifferenceThree =
										labs(
												(int32_t) dataSensor.environmentParameterHistory[index].priLow
														- (int32_t) track.last[0].pri.getDwellSwitchFirstSpot().priLow);
								priDifferenceFour =
										labs(
												(int32_t) dataSensor.environmentParameterHistory[index].priHigh
														- (int32_t) track.last[0].pri.getDwellSwitchLastSpot().priLow);
							}
							else
							{
								priDifferenceThree = 255;
								priDifferenceFour = 255;
							}
							if ((priDifferenceOne < 5) || (priDifferenceTwo < 5)
									|| (priDifferenceThree < 5)
									|| (priDifferenceFour < 5))
							{
								parameterExclusion = 0;
							}
							priCountDifference =
									labs(
											(int16_t) dataSensor.environmentParameterHistory[index].priCount
													- (int16_t) track.last[0].pri.accumulatePriDwellSwitchCount);
							if (priCountDifference > 2)
							{
								parameterExclusion = 1;
							}
						}
						else if ((dataSensor.environmentParameterHistory[index].priType
								== PriTypeEnum::pri_sliding)
								&& (track.last[0].pri.accumulatePriSlidingCount
										> 0))
						{
							if ((dataSensor.environmentParameterHistory[index].priHigh
									>= track.last[0].pri.getSlidingMaxRange().priLow)
									&& (dataSensor.environmentParameterHistory[index].priLow
											<= track.last[0].pri.getSlidingMaxRange().priHigh))
							{
								parameterExclusion = 0;
							}
						}
						else if ((dataSensor.environmentParameterHistory[index].priType
								== PriTypeEnum::pri_jitter)
								&& (track.last[0].pri.accumulatePriJitterCount
										> 0))
						{
							if ((dataSensor.environmentParameterHistory[index].priHigh
									>= track.last[0].pri.getJitterMaxRange().priLow)
									&& (dataSensor.environmentParameterHistory[index].priLow
											<= track.last[0].pri.getJitterMaxRange().priHigh))
							{
								parameterExclusion = 0;
							}
						}
						else if ((dataSensor.environmentParameterHistory[index].priType
								== PriTypeEnum::pri_complex)
								&& (track.last[0].pri.accumulatePriComplexCount
										> 0))
						{
							if ((dataSensor.environmentParameterHistory[index].priHigh
									>= track.last[0].pri.getComplexMaxRange().priLow)
									&& (dataSensor.environmentParameterHistory[index].priLow
											<= track.last[0].pri.getComplexMaxRange().priHigh))
							{
								parameterExclusion = 0;
							}
						}
						else if ((dataSensor.environmentParameterHistory[index].priType
								== PriTypeEnum::pri_difftoa)
								&& (track.last[0].pri.accumulatePriDwellSwitchCount
										> 0))
						{
							priDifferenceOne =
									labs(
											(int32_t) dataSensor.environmentParameterHistory[index].priLow
													- (int32_t) track.last[0].pri.getDiffToaMinSpot().priLow);
							priDifferenceTwo =
									labs(
											(int32_t) dataSensor.environmentParameterHistory[index].priHigh
													- (int32_t) track.last[0].pri.getDiffToaMaxSpot().priLow);
							if (track.last[0].pri.accumulatePriDwellSwitchCount
									> 1)
							{
								priDifferenceThree =
										labs(
												(int32_t) dataSensor.environmentParameterHistory[index].priLow
														- (int32_t) track.last[0].pri.getDiffToaFirstSpot().priLow);
								priDifferenceFour =
										labs(
												(int32_t) dataSensor.environmentParameterHistory[index].priHigh
														- (int32_t) track.last[0].pri.getDiffToaLastSpot().priLow);
							}
							else
							{
								priDifferenceThree = 255;
								priDifferenceFour = 255;
							}
							if ((priDifferenceOne < 5) || (priDifferenceTwo < 5)
									|| (priDifferenceThree < 5)
									|| (priDifferenceFour < 5))
							{
								parameterExclusion = 0;
							}
						}
						if (parameterExclusion == 0)
						{
							if ((dataSensor.environmentParameterHistory[index].rfLow
									> 0)
									&& (track.last[0].rf.accumulateRfCount > 0))
							{
								rfDifference =
										labs(
												(int16_t) dataSensor.environmentParameterHistory[index].rfLow
														- (int16_t) track.last[0].rf.getRfMinSpot().rfLow);
								if (rfDifference > 100)
								{
									parameterExclusion = 1;
								}
							}
						}
						if (parameterExclusion == 0)
						{
							dataSensor.environmentParameterHistory[index].updateCount =
									dataSensor.environmentParameterHistory[index].updateCount
											+ 1;
							if (dataSensor.environmentParameterHistory[index].updateCount
									> 1)
							{
								dataSensor.environmentParameterHistory[index].bandCode =
										BandCodeEnum::band_uncomputed;
								dataSensor.environmentParameterHistory[index].bearing =
										0;
								dataSensor.environmentParameterHistory[index].updateCount =
										0;
								dataSensor.environmentParameterHistory[index].priType =
										PriTypeEnum::pri_uncomputed;
								dataSensor.environmentParameterHistory[index].priCount =
										0;
								dataSensor.environmentParameterHistory[index].pwLow =
										0;
								dataSensor.environmentParameterHistory[index].pwHigh =
										0;
								dataSensor.environmentParameterHistory[index].rfLow =
										0;
								dataSensor.environmentParameterHistory[index].rfHigh =
										0;
								dataSensor.environmentParameterHistory[index].priLow =
										0;
								dataSensor.environmentParameterHistory[index].priHigh =
										0;
								dataSensor.environmentParameterHistory[index].priSum =
										0;
								dataSensor.environmentParameterHistory[index].toaStart =
										0;
								dataSensor.environmentParameterHistory[index].toaEnd =
										0;
								//why is this value 22?
								if (dataSensor.environmentParameterHistory[index].lifeTime
										< 22)
								{
									dataSensor.environmentParameterHistory[index].lifeTime =
											0;
									return 1;
								}
								else
								{
									dataSensor.environmentParameterHistory[index].lifeTime =
											0;
									return 1;
								}
							}
							else
							{
								return 0;
							}
						}
					}
				}
			}
		}
	}
	return 0;
}
void trackExpireHistoryParameterEnvironment(DataSensorClass &dataSensor)
{

	vector<DataEnvironmentClass>::iterator iter;

	for (iter = dataSensor.environmentParameterHistory.begin();
			iter != dataSensor.environmentParameterHistory.end();)
	{
		if (iter->lifeTime > 0)
		{
			iter->lifeTime = iter->lifeTime - 1;
		}
		//delete track if lifetime counter has reached a certain limit
		if (iter->lifeTime == 0)
		{
			iter = dataSensor.environmentParameterHistory.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}
void trackExpireHistoryRangeEnvironment(DataSensorClass &dataSensor)
{

	vector<DataEnvironmentClass>::iterator iter;

	for (iter = dataSensor.environmentRangeHistory.begin();
			iter != dataSensor.environmentRangeHistory.end();)
	{
		if (iter->lifeTime > 0)
		{
			iter->lifeTime = iter->lifeTime - 1;
		}
		//delete track if lifetime counter has reached a certain limit
		if (iter->lifeTime == 0)
		{
			iter = dataSensor.environmentRangeHistory.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}
void trackCalcBearingDisplayAllHigh(vector<TrackDefaultClass> &tracks)
{
	uint16_t i = 0;
	int16_t resultBearing = 0;
	uint16_t SmallerAngleValue = 0;
	int16_t DiffAOA = 0;
	int16_t shiftAoa = 0;
	uint16_t AbsDifferenceAOA = 0;
	int16_t filterConstant = 0;
	uint16_t remainderAbsolute = 0;

	for (i = 0; i < tracks.size(); i++)
	{

		if (tracks[i].angle.timeIterCountBearingCalc
				>= tracks[i].angle.timeIterLimitBearingCalc)
		{
			tracks[i].power.flagResetQuadrantCodes = 1;
			tracks[i].angle.timeIterCountBearingCalc = 0;
			filterConstant = tracks[i].angle.bearingFilterLimit;

			DiffAOA = (tracks[i].angle.bearingPreviousLast
					- tracks[i].angle.bearingLast);
			AbsDifferenceAOA = labs((int16_t) DiffAOA);
			SmallerAngleValue = doaRoundAngleDifference(AbsDifferenceAOA);

			if ((DiffAOA > 0) && (AbsDifferenceAOA <= 180))
				shiftAoa = (int16_t) (-SmallerAngleValue);
			else if ((DiffAOA < 0) && (AbsDifferenceAOA <= 180))
				shiftAoa = (int16_t) (SmallerAngleValue);
			else if ((DiffAOA < 0) && (AbsDifferenceAOA > 180))
				shiftAoa = (int16_t) (-SmallerAngleValue);
			else if ((DiffAOA > 0) && (AbsDifferenceAOA > 180))
				shiftAoa = (int16_t) (SmallerAngleValue);

			tracks[i].angle.bearingRemainderLast =
					tracks[i].angle.bearingRemainderLast + shiftAoa;
			resultBearing = tracks[i].angle.bearingDisplay;
			remainderAbsolute = labs(
					(int16_t) tracks[i].angle.bearingRemainderLast);

			if (remainderAbsolute > 5)
			{
				if (remainderAbsolute < filterConstant)
				{
					filterConstant = remainderAbsolute;
				}
				if (tracks[i].angle.bearingRemainderLast < 0)
				{
					resultBearing = resultBearing - filterConstant;
					if (resultBearing < 0)
					{
						resultBearing = 359 + resultBearing;
					}
					tracks[i].angle.bearingRemainderLast =
							tracks[i].angle.bearingRemainderLast
									+ filterConstant;
				}
				else if (tracks[i].angle.bearingRemainderLast > 0)
				{
					resultBearing = resultBearing + filterConstant;
					if (resultBearing > 359)
					{
						resultBearing = resultBearing - 359;
					}
					tracks[i].angle.bearingRemainderLast =
							tracks[i].angle.bearingRemainderLast
									- filterConstant;
				}
			}
			tracks[i].angle.bearingDisplay = (uint16_t) resultBearing;
			tracks[i].angle.curAngle = tracks[i].angle.bearingDisplay;
			tracks[i].angle.bearingPreviousLast = tracks[i].angle.bearingLast;
		}
	}
}
void trackCalcBearingLastAll(vector<TrackDefaultClass> &tracks)
{

	uint16_t i = 0;

	//this needs to be completed using proper iter counts
	for (i = 0; i < tracks.size(); i++)
	{
		//debugging
		//if (tracks[i].general.toaLast == 220523049510)
		//	tracks[i].general.toaLast = 220523049510;
		//end of debugging

		//just confirm that we do not update angle outside of track creation, organic and
		//string based updates
		tracks[i].angle.bearingLast = doaCalculateAngle(tracks[i].power.pxMax,
				tracks[i].power.pyMax, tracks[i].power.quadrantCodeMax,
				tracks[i].rf.getBandCodeLegacy(),
				Constant::systemBandCountOperate);
		tracks[i].angle.bearingDisplay = tracks[i].angle.bearingLast;
		tracks[i].angle.curAngle = tracks[i].angle.bearingLast;
	}
}
void trackCalcQuadrantAndPxPyAllLow(vector<TrackDefaultClass> &tracks)
{

	uint16_t i = 0;
	uint64_t totalPulseCount = 0;
	uint8_t tempQuadrantCode = 0;
//	if (tracks[i].general.toaLast == 151408262570)
//		tracks[i].general.toaLast = 151408262570;

	for (i = 0; i < tracks.size(); i++)
	{

		tempQuadrantCode = 4;

		/*if (tracks[i].general.toaLast == 53215725900)
		 tracks[i].general.toaLast = 53215725900;*/
		totalPulseCount = tracks[i].power.countQuadrantCodesIter[0]
				+ tracks[i].power.countQuadrantCodesIter[1]
				+ tracks[i].power.countQuadrantCodesIter[2]
				+ tracks[i].power.countQuadrantCodesIter[3];

		if (totalPulseCount == 0)
		{
			tracks[i].power.quadrantCodeMax = tracks[i].power.quadrantCodeLast;
		}
		else if ((tracks[i].power.countQuadrantCodesIter[0]
				>= tracks[i].power.countQuadrantCodesIter[1])
				&& (tracks[i].power.countQuadrantCodesIter[0]
						>= tracks[i].power.countQuadrantCodesIter[2])
				&& (tracks[i].power.countQuadrantCodesIter[0]
						>= tracks[i].power.countQuadrantCodesIter[3]))
		{
			tempQuadrantCode = 0;
		}
		else if ((tracks[i].power.countQuadrantCodesIter[1]
				>= tracks[i].power.countQuadrantCodesIter[0])
				&& (tracks[i].power.countQuadrantCodesIter[1]
						>= tracks[i].power.countQuadrantCodesIter[2])
				&& (tracks[i].power.countQuadrantCodesIter[1]
						>= tracks[i].power.countQuadrantCodesIter[3]))
		{
			tempQuadrantCode = 1;
		}
		else if ((tracks[i].power.countQuadrantCodesIter[2]
				>= tracks[i].power.countQuadrantCodesIter[0])
				&& (tracks[i].power.countQuadrantCodesIter[2]
						>= tracks[i].power.countQuadrantCodesIter[1])
				&& (tracks[i].power.countQuadrantCodesIter[2]
						>= tracks[i].power.countQuadrantCodesIter[3]))
		{
			tempQuadrantCode = 2;
		}
		else
		{
			tempQuadrantCode = 3;
		}

		if (tempQuadrantCode == tracks[i].power.quadrantCodeMaxPxPy)
		{
			tracks[i].power.quadrantCodeMax =
					tracks[i].power.quadrantCodeMaxPxPy;
		}
		else if (totalPulseCount != 0)
		{
			if ((((double) tracks[i].power.countQuadrantCodesIter[0]
					/ (double) totalPulseCount) >= 0.05)
					&& tracks[i].power.quadrantCodeMaxPxPy == 0)
			{
				tracks[i].power.quadrantCodeMax = 0;
				tracks[i].power.countQuadrantCodesIter[1] = 0;
				tracks[i].power.countQuadrantCodesIter[2] = 0;
				tracks[i].power.countQuadrantCodesIter[3] = 0;
			}
			else if ((((double) tracks[i].power.countQuadrantCodesIter[1]
					/ (double) totalPulseCount) >= 0.05)
					&& tracks[i].power.quadrantCodeMaxPxPy == 1)
			{
				tracks[i].power.quadrantCodeMax = 1;
				tracks[i].power.countQuadrantCodesIter[0] = 0;
				tracks[i].power.countQuadrantCodesIter[2] = 0;
				tracks[i].power.countQuadrantCodesIter[3] = 0;
			}
			else if ((((double) tracks[i].power.countQuadrantCodesIter[2]
					/ (double) totalPulseCount) >= 0.05)
					&& tracks[i].power.quadrantCodeMaxPxPy == 2)
			{
				tracks[i].power.quadrantCodeMax = 2;
				tracks[i].power.countQuadrantCodesIter[0] = 0;
				tracks[i].power.countQuadrantCodesIter[1] = 0;
				tracks[i].power.countQuadrantCodesIter[3] = 0;
			}
			else if ((((double) tracks[i].power.countQuadrantCodesIter[3]
					/ (double) totalPulseCount) >= 0.05)
					&& tracks[i].power.quadrantCodeMaxPxPy == 3)
			{
				tracks[i].power.quadrantCodeMax = 3;
				tracks[i].power.countQuadrantCodesIter[0] = 0;
				tracks[i].power.countQuadrantCodesIter[1] = 0;
				tracks[i].power.countQuadrantCodesIter[2] = 0;

			}
			else
				tracks[i].power.quadrantCodeMax = tempQuadrantCode;

		}
	}
	//px, py for low tracks are simply pxMax and pyMax
}
void trackCalcQuadrantAndPxPyAllHigh(vector<TrackDefaultClass> &tracks)
{

	uint16_t i = 0;

	for (i = 0; i < tracks.size(); i++)
	{
		if (tracks[i].power.flagResetQuadrantCodes == 1)
		{
			tracks[i].power.countQuadrantCodesIter[0] = 0;
			tracks[i].power.countQuadrantCodesIter[1] = 0;
			tracks[i].power.countQuadrantCodesIter[2] = 0;
			tracks[i].power.countQuadrantCodesIter[3] = 0;
			tracks[i].power.pxMaxQuadrant[0] = 0;
			tracks[i].power.pxMaxQuadrant[1] = 0;
			tracks[i].power.pxMaxQuadrant[2] = 0;
			tracks[i].power.pxMaxQuadrant[3] = 0;
			tracks[i].power.pyMaxQuadrant[0] = 0;
			tracks[i].power.pyMaxQuadrant[1] = 0;
			tracks[i].power.pyMaxQuadrant[2] = 0;
			tracks[i].power.pyMaxQuadrant[3] = 0;
			tracks[i].power.flagResetQuadrantCodes = 0;
		}
		if ((tracks[i].power.countQuadrantCodesIter[0] == 0)
				&& (tracks[i].power.countQuadrantCodesIter[1] == 0)
				&& (tracks[i].power.countQuadrantCodesIter[2] == 0)
				&& (tracks[i].power.countQuadrantCodesIter[3] == 0))
		{
			continue;
		}
		if ((tracks[i].power.countQuadrantCodesIter[0]
				>= tracks[i].power.countQuadrantCodesIter[1])
				&& (tracks[i].power.countQuadrantCodesIter[0]
						>= tracks[i].power.countQuadrantCodesIter[2])
				&& (tracks[i].power.countQuadrantCodesIter[0]
						>= tracks[i].power.countQuadrantCodesIter[3]))
		{
			tracks[i].power.quadrantCodeMax = 0;
		}
		else if ((tracks[i].power.countQuadrantCodesIter[1]
				>= tracks[i].power.countQuadrantCodesIter[0])
				&& (tracks[i].power.countQuadrantCodesIter[1]
						>= tracks[i].power.countQuadrantCodesIter[2])
				&& (tracks[i].power.countQuadrantCodesIter[1]
						>= tracks[i].power.countQuadrantCodesIter[3]))
		{
			tracks[i].power.quadrantCodeMax = 1;
		}
		else if ((tracks[i].power.countQuadrantCodesIter[2]
				>= tracks[i].power.countQuadrantCodesIter[0])
				&& (tracks[i].power.countQuadrantCodesIter[2]
						>= tracks[i].power.countQuadrantCodesIter[1])
				&& (tracks[i].power.countQuadrantCodesIter[2]
						>= tracks[i].power.countQuadrantCodesIter[3]))
		{
			tracks[i].power.quadrantCodeMax = 2;
		}
		else
		{
			tracks[i].power.quadrantCodeMax = 3;
		}
		tracks[i].power.pxMax =
				tracks[i].power.pxMaxQuadrant[tracks[i].power.quadrantCodeMax];
		tracks[i].power.pyMax =
				tracks[i].power.pyMaxQuadrant[tracks[i].power.quadrantCodeMax];
	}
}
void trackCalcQuadrantAndPxPyHigh(TrackDefaultClass &track)
{

	if ((track.power.countQuadrantCodesIter[0] == 0)
			&& (track.power.countQuadrantCodesIter[1] == 0)
			&& (track.power.countQuadrantCodesIter[2] == 0)
			&& (track.power.countQuadrantCodesIter[3] == 0))
	{
		return;
	}
	if ((track.power.countQuadrantCodesIter[0]
			>= track.power.countQuadrantCodesIter[1])
			&& (track.power.countQuadrantCodesIter[0]
					>= track.power.countQuadrantCodesIter[2])
			&& (track.power.countQuadrantCodesIter[0]
					>= track.power.countQuadrantCodesIter[3]))
	{
		track.power.quadrantCodeMax = 0;
	}
	else if ((track.power.countQuadrantCodesIter[1]
			>= track.power.countQuadrantCodesIter[0])
			&& (track.power.countQuadrantCodesIter[1]
					>= track.power.countQuadrantCodesIter[2])
			&& (track.power.countQuadrantCodesIter[1]
					>= track.power.countQuadrantCodesIter[3]))
	{
		track.power.quadrantCodeMax = 1;
	}
	else if ((track.power.countQuadrantCodesIter[2]
			>= track.power.countQuadrantCodesIter[0])
			&& (track.power.countQuadrantCodesIter[2]
					>= track.power.countQuadrantCodesIter[1])
			&& (track.power.countQuadrantCodesIter[2]
					>= track.power.countQuadrantCodesIter[3]))
	{
		track.power.quadrantCodeMax = 2;
	}
	else
	{
		track.power.quadrantCodeMax = 3;
	}
	track.power.pxMax = track.power.pxMaxQuadrant[track.power.quadrantCodeMax];
	track.power.pyMax = track.power.pyMaxQuadrant[track.power.quadrantCodeMax];
}
void trackCalcRangeDisplay(TrackDefaultClass &track)
{

//	float powerDecibels = 0;
	float powerDecibelsNormalized = 0;
	//float cableLosses = 6;
	float distance = 0;
	uint16_t newPosition = 0;
	uint16_t antennaPeak = 0;
	uint16_t maxPower = 0;
	uint16_t angleScaled = 0;
	uint16_t rfValue = 0;
//	if (track.general.toaLast == 203260391400)
//	{
//		track.general.toaLast = 203260391400;
//	}
//	track.range.rangeMeters = 15000;//this is temporary and should be removed

	if (track.power.pxMax > track.power.pyMax)
	{
		maxPower = track.power.pxMax;
	}
	else
	{
		maxPower = track.power.pyMax;
	}
	if (track.rf.accumulateRf.size() == 0)
	{
		if (track.rf.getBandCodeLegacy() == 0)
			rfValue = 3000;
		else if (track.rf.getBandCodeLegacy() == 1)
			rfValue = 6000;
		else if (track.rf.getBandCodeLegacy() == 2)
			rfValue = 9500;
		else if (track.rf.getBandCodeLegacy() == 3)
			rfValue = 12000;
		else if (track.rf.getBandCodeLegacy() == 4)
			rfValue = 1000;
		else if (track.rf.getBandCodeLegacy() == 5)
			rfValue = 24000;
	}
	else
	{
		rfValue = track.rf.accumulateRf.front().rfLow;
	}

	//angleScaled = (int)((((float)pTrack->BearingMaxPxPy)/(255.0)) * 360.0);
	//powerDecibels = (-52+((((float)maxPower)-65)/3.85));
	//antennaPeak = ABSOLUTE_VAL(45 - (angleScaled % 90));
	//powerDecibelsNormalized = powerDecibels + powerNormalizationTable[antennaPeak];
	//distance = ((((float)pTrack->effectiveRadiatedPower)-powerDecibelsNormalized-32.4-20.0*log10((float)rfValue))/20.0);
	//distance = (pow(10.0,distance))/1.852;
	F64 scaledInput1 = 0, scaledInput2 = 0, dbValue = 0;
	scaledInput1 = (((F64) maxPower) / 2048) / 0.37;
	scaledInput2 = pow(10, ((scaledInput1 - 3.704) / 0.539));
	dbValue = 10 * log10(scaledInput2);

	angleScaled = track.angle.bearingLast;	//we are already scaled to 0-359
//	powerDecibels = (-52 + ((((float) maxPower) - 1040) / 61.6));//everything multiplied by 16
	antennaPeak = labs((int16_t) (45 - (angleScaled % 90)));
	powerDecibelsNormalized = dbValue
			+ ConstHigh::rangePowerNormalizationTable[antennaPeak];
	distance = abs(
			(((float) track.libmatch.active.effectiveRadiatedPower)
					- powerDecibelsNormalized - 32.4
					- 20.0 * log10((float) rfValue)) / 20.0);
	distance = (pow(10.0, distance)) / 1.852;
	track.range.rangeMeters = (uint64_t) (distance * 1852.0); //this isn't used anywhere yet

	if (distance >= 42)
	{
		newPosition = 2;
		//pTrack->TrackAgeAngleWindowLimit = 8;
		track.angle.timeIterLimitBearingCalc = 8;
		track.range.timeIterLimitRangeCalc = 8;
	}
	else if (distance >= 39)
	{
		newPosition = 2;
		track.angle.timeIterLimitBearingCalc = 8;
		track.range.timeIterLimitRangeCalc = 8;
	}
	else if (distance >= 36)
	{
		newPosition = 2;
		track.angle.timeIterLimitBearingCalc = 8;
		track.range.timeIterLimitRangeCalc = 8;
	}
	else if (distance >= 33)
	{
		newPosition = 3;
		track.angle.timeIterLimitBearingCalc = 8;
		track.range.timeIterLimitRangeCalc = 8;
	}
	else if (distance >= 30)
	{
		newPosition = 4;
		track.angle.timeIterLimitBearingCalc = 8;
		track.range.timeIterLimitRangeCalc = 8;
	}
	else if (distance >= 27)
	{
		newPosition = 5;
		track.angle.timeIterLimitBearingCalc = 8;
		track.range.timeIterLimitRangeCalc = 8;
	}
	else if (distance >= 24)
	{
		newPosition = 6;
		track.angle.timeIterLimitBearingCalc = 8;
		track.range.timeIterLimitRangeCalc = 8;
	}
	else if (distance >= 21)
	{
		newPosition = 7;
		track.angle.timeIterLimitBearingCalc = 8;
		track.range.timeIterLimitRangeCalc = 8;
	}
	else if (distance >= 18)
	{
		newPosition = 8;
		track.angle.timeIterLimitBearingCalc = 8;
		track.range.timeIterLimitRangeCalc = 8;
	}
	else if (distance >= 15)
	{
		newPosition = 9;
		track.angle.timeIterLimitBearingCalc = 8;
		track.range.timeIterLimitRangeCalc = 8;
	}
	else if (distance >= 12)
	{
		newPosition = 10;
		track.angle.timeIterLimitBearingCalc = 8;
		track.range.timeIterLimitRangeCalc = 8;
	}
	else if (distance >= 9)
	{
		newPosition = 11;
		track.angle.timeIterLimitBearingCalc = 5;
		track.range.timeIterLimitRangeCalc = 5;
	}
	else if (distance >= 6)
	{
		newPosition = 12;
		track.angle.timeIterLimitBearingCalc = 5;
		track.range.timeIterLimitRangeCalc = 5;
	}
	else if (distance >= 3)
	{
		newPosition = 13;
		track.angle.timeIterLimitBearingCalc = 5;
		track.range.timeIterLimitRangeCalc = 5;
	}
	else if (distance >= 0)
	{
		newPosition = 14;
		track.angle.timeIterLimitBearingCalc = 5;
		track.range.timeIterLimitRangeCalc = 5;
	}
	if (newPosition >= track.range.amplitudeStepDisplay)
	{
		track.range.amplitudeStepDisplay = newPosition;
	}
	track.range.amplitudeCountMax = 80
			+ (track.range.amplitudeStepDisplay * 10);
	if ((track.rf.getBandCodeLegacy() == 0)
			&& (track.range.amplitudeCountMax == 100)
			&& (track.range.amplitudeStepDisplay == 2))
	{
		track.range.amplitudeCountMax = 90;
		track.range.amplitudeStepDisplay = 1;
	}
	//legacy code
	/*
	 Word16 amplitudeDifference = 0;
	 UWord8 newPosition = 0;

	 if((pTrack->TrackAmplitudeSttFlag == 1) && (pTrack->Band == 2) && (pTrack->staggerPriIterator == 0))
	 {
	 amplitudeDifference = pTrack->TrackAmplitudeLast - pTrack->TrackAmplitudeSttInitial;
	 if(amplitudeDifference > 4)
	 {
	 if(amplitudeDifference >= 164)
	 {
	 newPosition = 14;
	 pTrack->TrackAgeAngleWindowLimit = 1;
	 }
	 else if(amplitudeDifference >= 134)
	 {
	 newPosition = 13;
	 pTrack->TrackAgeAngleWindowLimit = 1;
	 }
	 else if(amplitudeDifference >= 108)
	 {
	 newPosition = 12;
	 pTrack->TrackAgeAngleWindowLimit = 1;
	 }
	 else if(amplitudeDifference >= 88)
	 {
	 newPosition = 11;
	 pTrack->TrackAgeAngleWindowLimit = 1;
	 }
	 else if(amplitudeDifference >= 72)
	 {
	 newPosition = 11;
	 pTrack->TrackAgeAngleWindowLimit = 2;
	 }
	 else if(amplitudeDifference >= 60)
	 {
	 newPosition = 11;
	 pTrack->TrackAgeAngleWindowLimit = 2;
	 }
	 else if(amplitudeDifference >= 50)
	 {
	 newPosition = 10;
	 pTrack->TrackAgeAngleWindowLimit = 2;
	 }
	 else if(amplitudeDifference >= 42)
	 {
	 newPosition = 9;
	 pTrack->TrackAgeAngleWindowLimit = 3;
	 }
	 else if(amplitudeDifference >= 36)
	 {
	 newPosition = 8;
	 pTrack->TrackAgeAngleWindowLimit = 3;
	 }
	 else if(amplitudeDifference >= 27)
	 {
	 newPosition = 7;
	 pTrack->TrackAgeAngleWindowLimit = 4;
	 }
	 else if(amplitudeDifference >= 21)
	 {
	 newPosition = 6;
	 pTrack->TrackAgeAngleWindowLimit = 4;
	 }
	 else if(amplitudeDifference >= 15)
	 {
	 newPosition = 5;
	 pTrack->TrackAgeAngleWindowLimit = 5;
	 }
	 else if(amplitudeDifference >= 9)
	 {
	 newPosition = 4;
	 pTrack->TrackAgeAngleWindowLimit = 5;
	 }
	 else if(amplitudeDifference >= 5)
	 {
	 newPosition = 3;
	 pTrack->TrackAgeAngleWindowLimit = 6;
	 }
	 else
	 {
	 newPosition = 2;
	 pTrack->TrackAgeAngleWindowLimit = 6;
	 }
	 if(newPosition > pTrack->TrackAmplitudePosition)
	 {
	 pTrack->TrackAmplitudePosition = newPosition;
	 }
	 }
	 }
	 pTrack->Max_Amp = 80 + (pTrack->TrackAmplitudePosition * 10);
	 */
	//end of legacy
}
void trackCalcRangeDisplayAllHigh(DataSensorClass &high)
{
	uint16_t i = 0;

	for (i = 0; i < high.tracks.size(); i++)
	{
		if (high.tracks[i].range.timeIterCountRangeCalc
				>= high.tracks[i].range.timeIterLimitRangeCalc)
		{
			high.tracks[i].range.timeIterCountRangeCalc = 0;
			trackCalcRangeDisplay(high.tracks[i]);
		}
	}
}
void tracksIlluminationBasedScanAnalysis(vector<TrackDefaultClass> &tracks)
{
	for (uint32_t i = 0; i < tracks.size(); i++)
	{
		TrackPowerClass &p = tracks[i].power;
		if (p.flagScanAnalysisRequested == 1)
		{
			p.flagScanAnalysisRequested = 0;
			ScanAnalysisHistoryClass analysisRes =
					trackIlluminationBasedScanAnalysis(tracks[i]);
			if (analysisRes.scanType != EmitterArpEnum::scan_unknown)
			{
				p.scanAnalysisHistory.push_back(analysisRes);
				using myPair=pair<uint16_t,EmitterArpEnum>;
				vector<myPair> v =
				{
				{ 0, EmitterArpEnum::scan_circular },
				{ 0, EmitterArpEnum::scan_sectorial },
				{ 0, EmitterArpEnum::scan_conical },
				{ 0, EmitterArpEnum::scan_lock },
				{ 0, EmitterArpEnum::scan_tws },
				{ 0, EmitterArpEnum::scan_raster },
 				{ 0, EmitterArpEnum::scan_complex },};
				uint16_t &scanCircScore = v[0].first;
				uint16_t &scanSectScore = v[1].first;
				uint16_t &scanConcScore = v[2].first;
				uint16_t &scanLockScore = v[3].first;
				uint16_t &scanTwssScore = v[4].first;
				uint16_t &scanRasterScore = v[5].first;
				uint16_t &scanComplexScore = v[6].first;
				for (uint32_t i = 0; i < p.scanAnalysisHistory.size(); i++)
				{
					switch (p.scanAnalysisHistory[i].scanType)
					{
					case EmitterArpEnum::scan_circular:
						scanCircScore += 3;
						break;
					case EmitterArpEnum::scan_sectorial:
						scanSectScore += 3;
						break;
					case EmitterArpEnum::scan_conical:
						scanConcScore += 3;
						break;
					case EmitterArpEnum::scan_lock:
						scanLockScore += 3;
						break;
					case EmitterArpEnum::scan_tws:
						scanTwssScore += 3;
						break;
					case EmitterArpEnum::scan_raster:
						scanRasterScore += 3;
						break;
					case EmitterArpEnum::scan_complex:
						scanComplexScore += 2;
						break;
					default:
						break;
					}
				}
				sort(v.begin(), v.end(), [](myPair& a,myPair&b)
				{
					if(a.first>b.first)
					{
						return true;
					}
					else if(a.first==b.first)
					{
						return((uint16_t)a.second>(uint16_t)b.second);
					}
					return false;
				});
				p.scanTypeFirst = p.scanTypeLast;
				p.scanTypeLast = v[0].second;
			}

		}
	}
}
ScanAnalysisHistoryClass trackIlluminationBasedScanAnalysis(
		TrackDefaultClass &track)
{
	if(track.general.toaLast == 156415200)
		track.general.toaLast = track.general.toaLast*1;
	circular_buffer<IlluminationCycleInfo> &lightUps =
			track.power.illuminationInfo;
	ScanAnalysisHistoryClass res;
	if (lightUps.size() == 0)
	{
		//set some flags
		return res;
	}
	IlluminationCycleInfo& firstCycle = lightUps[0];
	IlluminationCycleInfo& lastCycle = lightUps[lightUps.size() - 1];
	uint64_t endTime = lastCycle.cycleStartTime + lastCycle.cycleDuration;
	uint32_t cycleCount = lightUps.size();
	uint32_t arpMatch = 1;
	int64_t arp = 0;
	int64_t lastArp = 0;
	vector<uint64_t> peakToPeakPowerTimeDiff;
	peakToPeakPowerTimeDiff.reserve(cycleCount - 1);
	for (uint32_t i = 0; i < cycleCount - 1; i++)
	{
		uint64_t firstTime = lightUps[i].cycleStartTime
				+ lightUps[i].maxPowerTime;
		uint64_t secondTime = lightUps[i + 1].cycleStartTime
				+ lightUps[i + 1].maxPowerTime;
		arp = secondTime - firstTime;
		peakToPeakPowerTimeDiff.push_back(arp);
		if (abs(lastArp - arp) < 10000000 /*|| abs(lastArp - arp) < (0.01*arp)*/)
			arpMatch++;
		lastArp = arp;
	}

	uint64_t avgIlluminationTime = 0;
	uint64_t totalTime = endTime - firstCycle.cycleStartTime;
	uint64_t totalIlluminationTime = 0;
	uint32_t totalPulseCount = 0;

	int32_t avgPowerLvl = 0;
	int32_t avgDevInPowerLvl = 0;
	int32_t avgKurtosisLvl = 0;
	uint32_t avgPeakPowerDuration = 0;
	uint32_t peakPowerVariation = 0;
	uint32_t powerVariationCount = 0;
	for (uint32_t i = 0; i < cycleCount; i++)
	{
		totalIlluminationTime += lightUps[i].cycleDuration;
		totalPulseCount += lightUps[i].cyclePulseCount;
		avgPowerLvl += lightUps[i].meanPower;
		avgDevInPowerLvl += lightUps[i].variance;
		avgKurtosisLvl += lightUps[i].kurtosis;
		avgPeakPowerDuration += lightUps[i].peakPowerDuration;
	}
	if (totalIlluminationTime < 1000000)
	{
		return res;
	}
	for (uint32_t i = 0; i < (cycleCount-1); i++)
	{
		peakPowerVariation = abs(lightUps[i+1].maxPower - lightUps[i].maxPower);
		if(peakPowerVariation > 150)
			powerVariationCount++;
	}

	avgPowerLvl = avgPowerLvl / cycleCount;
	avgIlluminationTime = totalIlluminationTime / cycleCount;
	avgDevInPowerLvl = avgDevInPowerLvl / cycleCount;
	avgKurtosisLvl = avgKurtosisLvl / cycleCount;
	avgPeakPowerDuration = avgPeakPowerDuration / cycleCount;

	uint64_t percentOnTime = (totalIlluminationTime * 100) / totalTime;

	using myPair=pair<uint16_t,EmitterArpEnum>;
	vector<myPair> v =
	{
	{ 0, EmitterArpEnum::scan_circular },
	{ 0, EmitterArpEnum::scan_sectorial },
	{ 0, EmitterArpEnum::scan_conical },
	{ 0, EmitterArpEnum::scan_lock },
	{ 0, EmitterArpEnum::scan_tws },
	{0, EmitterArpEnum::scan_raster },};
	uint16_t &scanCircScore = v[0].first;
	uint16_t &scanSectScore = v[1].first;
	uint16_t &scanConcScore = v[2].first;
	uint16_t &scanLockScore = v[3].first;
	uint16_t &scanTwssScore = v[4].first;
	uint16_t &scanRasterScore = v[5].first;

	if(avgPeakPowerDuration > 40)
	{
		scanLockScore += 5;
		scanTwssScore += 5;
		scanConcScore += 5;
	}
	if (percentOnTime < 50 || avgDevInPowerLvl > 500)
	{
		if (avgIlluminationTime < 50000000)
		{
			if (arpMatch > (cycleCount / 2))
				scanCircScore += 8;
			else
			{
				scanSectScore += 8;
				scanRasterScore += 8;
			}
		}
//		if (avgKurtosisLvl > 200)
//		{
//			scanCircScore += 5;
//			scanSectScore += 5;
//		}
	}
	else
	{
		if (avgKurtosisLvl > 100 && avgKurtosisLvl < 200)
		{
			if(cycleCount > 1)
				scanConcScore += 15;
			else
				scanConcScore += 5;
		}
		else
		{
			if(percentOnTime > 75)
			{
				scanLockScore += 5;
			}
			else
				scanTwssScore += 5;
		}
	}
	if (avgDevInPowerLvl > 40)
	{
		scanCircScore += 5;
		scanSectScore += 5;
		scanRasterScore += 5;

		if(scanConcScore > 0)
			scanConcScore = 0;
		if(scanTwssScore > 0)
			scanTwssScore = 0;
		if(scanLockScore > 0)
			scanLockScore = 0;
	}
	else if (avgDevInPowerLvl > 0)
	{
		scanConcScore += 5;
	}
	else
		scanLockScore += 5;

	if(cycleCount > 1)
	{
		if (percentOnTime < 25)
		{
			scanCircScore += 5;
			scanSectScore += 5;
			scanRasterScore += 5;
		}
		else if (percentOnTime < 45)
		{
			scanSectScore += 5;
		}
		else if (percentOnTime < 75)
		{
			scanTwssScore += 5;
		}
		else
		{
			scanConcScore += 5;
			scanLockScore += 5;
		}
		if(powerVariationCount >= floor(cycleCount/2))
		{
			scanRasterScore += 10;
		}
		else if(scanRasterScore >=5)
			scanRasterScore = scanRasterScore - 5;
	}
	sort(v.begin(), v.end(), [](myPair& a,myPair&b)
	{
		if(a.first>b.first)
		{
			return true;
		}
		else if(a.first==b.first)
		{
			return((uint16_t)a.second>(uint16_t)b.second);
		}
		return false;
	});
	if( v[0].first >= 10)
	{
		res.scanType = v[0].second;
		res.score = v[0].first;
	}
	if( v[1].first >= 10)
	{
		res.scanTypeOther = v[1].second;
		res.scoreOther = v[1].first;
	}

	res.analysisTimestamp = Utility::getSystemTime();
	res.analysisToaMin = firstCycle.cycleStartTime;
	res.analysisToaMax = lastCycle.cycleStartTime + lastCycle.cycleDuration;
	res.analysisPulseCount = totalPulseCount;
	res.avgIlluminationTime = avgIlluminationTime;
	res.avgPower = avgPowerLvl;
	if (cycleCount > 2)
	{
		res.arpMin = peakToPeakPowerTimeDiff.front();
		res.arpMax = peakToPeakPowerTimeDiff.back();
	}
	if(res.scanType == EmitterArpEnum::scan_sectorial)
	{
		if(cycleCount < 3)
			res.scanType = EmitterArpEnum::scan_complex;
	}
//	if(res.scanTypeOther == EmitterArpEnum::scan_raster && cycleCount < 4)
//		res.scanType = EmitterArpEnum::scan_complex;
	return res;

}
void trackCalcTrackingModeAllHigh(vector<TrackDefaultClass> &tracks)
{
	uint16_t i = 0;

	//apply logic to incorporate dsp based tracking flag using count or update/track once
	for (i = 0; i < tracks.size(); i++)
	{
		//if the library (or default) says this radar can both scan and track
		if ((tracks[i].libmatch.active.flagScanningOnlyLibrary == 0)
				&& (tracks[i].libmatch.active.flagTrackingOnlyLibrary == 0))
		{
			if (tracks[i].history.flagUpdateOnceIterHigh == 1)
			{
				// scenario handled for low ARPs
				if (tracks[i].power.countScanningModeLowIterHigh > 0
						&& tracks[i].power.countScanningModeLowIterHigh
								> tracks[i].power.countTrackingModeLowIterHigh)
				{
					if (tracks[i].power.countScanningModeIterHigh
							< ConstHigh::trackLimitMaxConsecutiveUpdateIter)
					{
						tracks[i].power.countScanningModeIterHigh =
								tracks[i].power.countScanningModeIterHigh + 2;
					}
					if (tracks[i].power.countScanningModeIterHigh
							>= ConstHigh::trackLimitPartialConsecutiveUpdateIter)
					{
						tracks[i].power.flagTrackingModeHigh = 0;
						tracks[i].power.flagScanningModeHigh = 1;
					}
					//if (tracks[i].power.countTrackingModeIterHigh > 0) {
					//	tracks[i].power.countTrackingModeIterHigh =
					//			tracks[i].power.countTrackingModeIterHigh - 1;
					//}
				}
				else
				{
					if (tracks[i].power.countTrackingModeIterHigh
							< ConstHigh::trackLimitMaxConsecutiveUpdateIter)
					{
						tracks[i].power.countTrackingModeIterHigh =
								tracks[i].power.countTrackingModeIterHigh + 2;
					}
					if (tracks[i].power.countTrackingModeIterHigh
							>= ConstHigh::trackLimitPartialConsecutiveUpdateIter)
					{
						tracks[i].power.flagTrackingModeHigh = 1;
						tracks[i].power.flagScanningModeHigh = 0;
					}
					//if (tracks[i].power.countScanningModeIterHigh > 0) {
					//	tracks[i].power.countScanningModeIterHigh =
					//			tracks[i].power.countScanningModeIterHigh - 1;
					//}
				}
			}
			else if (tracks[i].power.countTrackingModeLowIterHigh > 0)
			{
				if (tracks[i].power.countTrackingModeIterHigh
						< ConstHigh::trackLimitMaxConsecutiveUpdateIter)
				{
					tracks[i].power.countTrackingModeIterHigh =
							tracks[i].power.countTrackingModeIterHigh + 2;
				}
				if (tracks[i].power.countTrackingModeIterHigh
						>= ConstHigh::trackLimitPartialConsecutiveUpdateIter)
				{
					tracks[i].power.flagTrackingModeHigh = 1;
					tracks[i].power.flagScanningModeHigh = 0;
				}
			}
			else
			{
				if (tracks[i].power.countTrackingModeIterHigh > 0)
				{
					tracks[i].power.countTrackingModeIterHigh =
							tracks[i].power.countTrackingModeIterHigh - 1;
				}
				if (tracks[i].power.countTrackingModeIterHigh < 4)
				{
					tracks[i].power.flagTrackingModeHigh = 0;
				}
			}
		}
		//if the library says this radar can only track
		if (tracks[i].libmatch.active.flagTrackingOnlyLibrary == 1)
		{
			if (tracks[i].power.countTrackingModeIterHigh
					< ConstHigh::trackLimitMaxConsecutiveUpdateIter)
			{
				tracks[i].power.countTrackingModeIterHigh =
						tracks[i].power.countTrackingModeIterHigh + 2;
			}
			tracks[i].power.flagTrackingModeHigh = 1;
			tracks[i].power.flagScanningModeHigh = 0;
		}
		//decrement at each iteration
		if (tracks[i].power.countTrackingModeIterHigh > 0)
		{
			tracks[i].power.countTrackingModeIterHigh =
					tracks[i].power.countTrackingModeIterHigh - 1;
		}
		if (tracks[i].power.countScanningModeIterHigh > 0)
		{
			tracks[i].power.countScanningModeIterHigh =
					tracks[i].power.countScanningModeIterHigh - 1;
		}
	}
}
void trackCreateTrackProbationHigh(DataSensorClass &dataSensor,
		TrackDefaultClass &low, TrackDefaultClass &lowNext, uint16_t lowIndex,
		uint16_t lowNextIndex)
{

	uint16_t i = 0;
	TrackDefaultClass high;

	//lifetime limit may need to be band specific
	high.general.timeIterCountLifetime = 0;
	high.general.timeIterLimitLifetime = ConstHigh::timeIterEquivalent4000ms;
	high.general.timeIterLimitDemodSupprSet =
			ConstHigh::timeIterEquivalent1000ms;
	high.general.timeIterLimitProbationSet = ConstHigh::timeIterEquivalent200ms;
	high.general.toaFirst = low.general.toaFirst;
	high.general.toaLast = low.general.toaLast;
	high.general.flagProbation = 1;
	high.general.flagVisibleHigh = 0;
	high.general.trackType = low.general.trackType;
	high.audio.flagNewGuyAudioPlayed = 0;

	//debug code
	if (high.general.toaLast == 210659859425)
	{
		high.general.toaLast = 210659859425;
	}
	//end of debug code

	high.history.idsAssociateTracksLow.set_capacity(100);	//this is temporary
	high.history.idsAssociateTracksLow.push_back(low.general.trackID);
	high.history.idsAssociateTracksIterLow.push_back(low.general.trackID);
	high.history.indexAssociateTracksIterLow.push_back(lowIndex);
	high.history.lowHighFlag = 1;
	//decide which ones to keep and update, in both organic and string versions
	if (	//(low.pri.accumulatePriComplexCount == 0) &&
	(low.pri.accumulatePriDiffToaCount == 0))
	{
		if (low.history.lowToaStartUpdateFlag == 1)
			high.history.stitchedUpdateCount = high.history.stitchedUpdateCount
					+ 1;
		if (low.history.lowToaStartUpdateNextFlag == 1)
			high.history.stitchedUpdateNextCount =
					high.history.stitchedUpdateNextCount + 1;
	}
	else
	{
		if (low.history.lowToaStartUpdateFlag == 1)
			high.history.stitchedUpdateDiffToaCount =
					high.history.stitchedUpdateDiffToaCount + 1;
		if (low.history.lowToaStartUpdateNextFlag == 1)
			high.history.stitchedUpdateNextDiffToaCount =
					high.history.stitchedUpdateNextDiffToaCount + 1;
		high.history.stitchedDiffToaCount = high.history.stitchedDiffToaCount
				+ 1;
	}
	if ((low.pri.accumulatePriJitterCount > 0)
			|| (low.pri.accumulatePriComplexCount > 0))
	{
		high.history.stitchedJitterAndComplexCount =
				high.history.stitchedJitterAndComplexCount + 1;
	}
	high.history.totalToaOverlap = high.history.totalToaOverlap
			+ low.history.durationToaOverlapMaxLow;
	high.history.totalPulseOverlap = high.history.totalPulseOverlap
			+ low.history.countPulseOverlapMaxLow;
	high.history.totalPulseCount = high.history.totalPulseCount
			+ low.power.pulseCountSinceLastReport;
	high.history.stitchedCount = high.history.stitchedCount + 1;
	high.history.activeFlag = 1;

	//review needed
	high.power.toaMinStartPower = low.power.toaMinStartPower;
	high.power.toaMinEndPower = low.power.toaMinEndPower;
	high.power.pxAntennaFirst = low.power.pxAntennaFirst;
	high.power.pxAntennaLast = low.power.pxAntennaLast;
	high.power.pyAntennaFirst = low.power.pyAntennaFirst;
	high.power.pyAntennaLast = low.power.pyAntennaLast;
	high.power.pxValueFirst = low.power.pxValueFirst;
	high.power.pxValueLast = low.power.pxValueLast;
	high.power.pxMax = low.power.pxMax;	//reported for bearing calculation at high
	high.power.pyMax = low.power.pyMax;	//reported for bearing calculation at high
	high.power.maxPowerToa = low.power.maxPowerToa;	//reported for arp calculation at high
	high.power.pyValueFirst = low.power.pyValueFirst;
	high.power.pyValueLast = low.power.pyValueLast;
	high.power.sumPxPyMax = low.power.sumPxPyMax;
	high.power.quadrantCodeFirst = low.power.quadrantCodeFirst;
	high.power.quadrantCodeLast = low.power.quadrantCodeLast;
	high.power.quadrantCodeMax = low.power.quadrantCodeLast;//this is a temporary solution
	high.power.primaryAntennaFirst = low.power.primaryAntennaFirst;
	high.power.primaryAntennaLast = low.power.primaryAntennaLast;
	high.power.illuminationInfo = low.power.illuminationInfo;
	//high.power.groundIlluminationFlag = low.power.groundIlluminationFlag;
	//high.power.groundIlluminationAxis = low.power.groundIlluminationAxis;
	//high.power.scanTypeFirst = low.power.scanTypeFirst;
	//high.power.scanTypeLast = low.power.scanTypeLast;
	//high.power.scanIlluminationTimeFirst = low.power.scanIlluminationTimeFirst;
	//high.power.scanIlluminationTimeLast = low.power.scanIlluminationTimeLast;
	//high.power.scanPulseCountFirst = low.power.scanPulseCountFirst;
	//high.power.scanPulseCountLast = low.power.scanPulseCountLast;
	//high.power.arpFirst = low.power.arpFirst;
	//high.power.arpLast = low.power.arpLast;
	//high.power.timeToaScanAnalysisLast = low.power.timeToaScanAnalysisLast;
	//high.power.flagTrackingLowFirst = low.power.flagTrackingLowFirst;
	//high.power.flagTrackingLowLast = low.power.flagTrackingLowLast;
	//high.power.flagTrackingEstimateHigh = low.power.flagTrackingEstimateHigh;
	high.power.countQuadrantCodesIter[0] = high.power.countQuadrantCodesIter[0]
			+ low.power.countQuadrantCodesIter[0];
	high.power.countQuadrantCodesIter[1] = high.power.countQuadrantCodesIter[1]
			+ low.power.countQuadrantCodesIter[1];
	high.power.countQuadrantCodesIter[2] = high.power.countQuadrantCodesIter[2]
			+ low.power.countQuadrantCodesIter[2];
	high.power.countQuadrantCodesIter[3] = high.power.countQuadrantCodesIter[3]
			+ low.power.countQuadrantCodesIter[3];
	if (low.power.pxMax > high.power.pxMaxQuadrant[low.power.quadrantCodeMax])
	{
		high.power.pxMaxQuadrant[low.power.quadrantCodeMax] = low.power.pxMax;
	}
	if (low.power.pyMax > high.power.pyMaxQuadrant[low.power.quadrantCodeMax])
	{
		high.power.pyMaxQuadrant[low.power.quadrantCodeMax] = low.power.pyMax;
	}
	if (low.power.scanTypeLast == EmitterArpEnum::scan_lock)
	{
		high.power.countTrackingModeLowIterHigh++;
	}
	else if (low.power.scanTypeLast == EmitterArpEnum::scan_circular)
	{
		high.power.countScanningModeLowIterHigh++;
	}
	trackCalcQuadrantAndPxPyHigh(high);
	high.angle.bearingLast = doaCalculateAngle(high.power.pxMax,
			high.power.pyMax, high.power.quadrantCodeMax,
			high.rf.getBandCodeLegacy(), Constant::systemBandCountOperate);
	high.angle.bearingFirst = high.angle.bearingLast;
	high.angle.bearingDisplay = high.angle.bearingLast;
	high.angle.bearingPreviousLast = high.angle.bearingLast;

	for (i = 0; i < 4; i++)
	{
		high.power.countQuadrantCodesIter[i] =
				high.power.countQuadrantCodesIter[i]
						+ low.power.countQuadrantCodesIter[i];
	}
	//copy pri list from low track into high track
	for (i = 0; i < low.pri.accumulatePri.size(); i++)
	{
		//if high pri buffer already has entries in it
		if (high.pri.accumulatePri.size() > 0)
		{
			uint16_t lastIndex = high.pri.accumulatePri.size() - 1;
			//if the last entry was not the same as this one
			if (!((labs(
					(int32_t) high.pri.accumulatePri[lastIndex].priLow
							- (int32_t) low.pri.accumulatePri[i].priLow) < 32)
					&& (high.pri.accumulatePri[lastIndex].priType
							== low.pri.accumulatePri[i].priType)))
			{
				//push value into high pri accumulative buffer
				high.pri.accumulatePri.push_back(low.pri.accumulatePri[i]);
			}
		}
		//if the high pri buffer was empty
		else
		{
			//push value into high pri accumulative buffer
			high.pri.accumulatePri.push_back(low.pri.accumulatePri[i]);
		}
	}
	trackCountUpdatePriAccumulative(high);//re count the accumulated pri buffer
	//copy pw list from low track into high track
	for (i = 0; i < low.pw.accumulatePw.size(); i++)
	{
		//if high pw buffer already has entries in it
		if (high.pw.accumulatePw.size() > 0)
		{
			uint16_t lastIndex = high.pw.accumulatePw.size() - 1;
			//if the last entry was not the same as this one
			if (!((high.pw.accumulatePw[lastIndex].pwLow
					== low.pw.accumulatePw[i].pwLow)
					&& (high.pw.accumulatePw[lastIndex].pwType
							== low.pw.accumulatePw[i].pwType)))
			{
				//push value into high pw accumulative buffer
				high.pw.accumulatePw.push_back(low.pw.accumulatePw[i]);
			}
		}
		//if the high pw buffer was empty
		else
		{
			//push value into high pw accumulative buffer
			high.pw.accumulatePw.push_back(low.pw.accumulatePw[i]);
		}
	}
	//copy rf list from low track into high track
	for (i = 0; i < low.rf.accumulateRf.size(); i++)
	{
		//if high rf buffer already has entries in it
		if (high.rf.accumulateRf.size() > 0)
		{
			uint16_t lastIndex = high.rf.accumulateRf.size() - 1;
			//if the last entry was not the same as this one
			if (!((high.rf.accumulateRf[lastIndex].rfLow
					== low.rf.accumulateRf[i].rfLow)
					&& (high.rf.accumulateRf[lastIndex].rfType
							== low.rf.accumulateRf[i].rfType)))
			{
				//push value into high rf accumulative buffer
				high.rf.accumulateRf.push_back(low.rf.accumulateRf[i]);
			}
		}
		//if the high rf buffer was empty
		else
		{
			//push value into high rf accumulative buffer
			high.rf.accumulateRf.push_back(low.rf.accumulateRf[i]);
		}
	}
	high.rf.bandCode = low.rf.bandCode;

	high.libmatch.timeIterLimitLibMatchExec =
			ConstHigh::timeIterEquivalent200ms;
	if ((high.rf.getBandCodeLegacy() == 2)
			&& (high.pri.accumulatePriStaggerCount == 0))
	{
		high.angle.timeIterLimitBearingCalc =
				ConstHigh::trackLimitBearingCalcIterAerial;
		high.range.timeIterLimitRangeCalc =
				ConstHigh::trackLimitRangeCalcIterAerial;
		high.range.amplitudeStepDisplay = 2;
		high.range.amplitudeCountMax = 100;
	}
	else
	{
		high.angle.timeIterLimitBearingCalc =
				ConstHigh::trackLimitBearingCalcIterDefault;
		high.range.timeIterLimitRangeCalc =
				ConstHigh::trackLimitRangeCalcIterDefault;
		high.range.amplitudeStepDisplay = 0;
		high.range.amplitudeCountMax = 80;
	}
	high.release.timeIterLimitReleaseSet = ConstHigh::timeIterEquivalent1000ms;
	libraryRequestMatch(high);
	libraryRequestForceUnknown(high);
	libraryMatchEmitterPreprocess(high, Model::library, Model::libraryMeta);
	libraryMatchEmitter(high, Model::library, Model::libraryMeta);
	libraryMatchEmitterPostprocess(high, Model::library, Model::libraryMeta,
			Model::cfdLibrary);
	trackCalcRangeDisplay(high);
	high.last.set_capacity(6);	//this is temporary
	high.last.push_front(low);
	//trackUpdateAccumlatedPwInfo(high);
	dataSensor.tracks.push_back(high);
}
void trackFinalizeTracksAllHigh(DataSensorClass &low, DataSensorClass &high)
{

	uint8_t trackRedeemStatus;
	uint8_t trackFinalizeFlag;
	uint16_t dspFirstReportIndex;
	uint16_t dspLastReportIndex;
	uint16_t trackIndex = 0;
	uint8_t categoryMatchCount;
	uint8_t indexMatchCount;
	uint8_t sttMatchCount;
	uint16_t minBearingDiff;
	uint8_t sttPwComboCount;
	uint8_t flagCompareHighHigh = 0;
	uint8_t flagComparePriHighHigh = 0;
	uint8_t flagCompareRFHighHigh = 0;
	uint8_t unknownTracksInAngleWindowExist = 0;

	//debug code
	//if(high.tracks.size() == 3) {
	//	if(high.tracks[10].general.toaLast == 162076970510) {
	//		high.tracks[10].general.toaLast = 162076970510;
	//	}
	//}
	//end of debug code

	//finalize new tracks
	for (trackIndex = 0; trackIndex < high.tracks.size(); trackIndex++)
	{
		trackFinalizeFlag = 0;

		if (high.tracks[trackIndex].general.toaLast == 248920000)
		{
			high.tracks[trackIndex].general.toaLast = 248920000;
		}
		//copy the buffer into accumulated

		//UpdateTrackLibraryAlternative(pTrack);//perform library match
		if ((high.tracks[trackIndex].general.flagProbation == 1)
				&& (high.tracks[trackIndex].general.flagDeletionHigh == 0))
		{
			dspFirstReportIndex =
					high.tracks[trackIndex].history.indexAssociateTracksIterLow[0];
			dspLastReportIndex =
					high.tracks[trackIndex].history.indexAssociateTracksIterLow.back();
			//modifying this condition with reservations
			//if((low.tracks[dspFirstReportIndex].history.flagOverlapPotential == 1) ||
			//		(low.tracks[dspLastReportIndex].history.flagOverlapPotential == 1)) {
			if (((low.tracks[dspFirstReportIndex].history.flagOverlapPotential
					== 1)
					&& (low.tracks[dspFirstReportIndex].history.durationToaOverlapMaxLow
							> 0))
					|| ((low.tracks[dspLastReportIndex].history.flagOverlapPotential
							== 1)
							&& (low.tracks[dspLastReportIndex].history.durationToaOverlapMaxLow
									> 0)))
			{

				flagCompareHighHigh = mergeCompareFinalizeHighHigh(
						high.tracks[trackIndex], high);
				flagComparePriHighHigh = mergeComparePriHighHigh(
						high.tracks[trackIndex], high);
				unknownTracksInAngleWindowExist =
						mergeFindUnknownsWithinAngleWindow(
								high.tracks[trackIndex], high);

				//old notes
				//will have to count the number of pulses
				//check overlap/min pulse count if any and make permanent
				//if multiple updates make permanent despite little overlap?
				//incase of long running probation, remove probation status for
				//tracks that have been organically updated
				//end of old notes
				//for debugging purposes
				//if(high.tracks[trackIndex].last[0].general.toaLast == 23539522760) {
				//	trackFinalizeFlag = 0;
				//}
				//end of debugging
				if (high.tracks[trackIndex].pw.accumulatePw.size() == 0)
				{
					trackFinalizeFlag = 0;
				}
//				else if ((high.tracks[trackIndex].rf.getBandCodeLegacy() > 3))
//				{
//					trackFinalizeFlag = 0;
//				}
				//else if((high.tracks[trackIndex].history.totalPulseOverlap > 200) &&
				//	(high.tracks[trackIndex].history.totalToaOverlap > 100000) &&
				//	(high.tracks[trackIndex].history.totalPulseCount > 500) &&
				//	(high.tracks[trackIndex].last[0].pri.flagPriAnalysisLimitedLast == 0)) {
				//	trackFinalizeTrackSingleHigh(high.tracks[trackIndex]);
				//	trackFinalizeFlag = 1;
				//}
				else if (high.tracks[trackIndex].libmatch.active.flagKnownLibrary
						== 1)
				{
					trackCalcFeasibilityFinalizeTrackHigh(high,
							high.tracks[trackIndex], categoryMatchCount,
							indexMatchCount, sttMatchCount, minBearingDiff,
							sttPwComboCount);
					if (((categoryMatchCount > 1)
							&& (high.tracks[trackIndex].libmatch.active.flagAerialOnlyLibrary
									== 1)) || (indexMatchCount > 1)
							|| ((indexMatchCount > 0)
									&& (high.tracks[trackIndex].libmatch.active.flagAerialOnlyLibrary
											== 0))
							|| ((indexMatchCount > 0) && (sttMatchCount > 0))
							|| ((indexMatchCount > 0) && (minBearingDiff < 20))
							|| ((sttPwComboCount > 0)
									&& (high.tracks[trackIndex].libmatch.active.flagAerialOnlyLibrary
											== 1)
									&& ((high.tracks[trackIndex].rf.getBandCodeLegacy()
											== 2))))
					{
						if (((high.tracks[trackIndex].history.stitchedUpdateCount
								> 0)
								&& (high.tracks[trackIndex].history.stitchedUpdateNextCount
										> 0))
								|| ((high.tracks[trackIndex].history.stitchedUpdateDiffToaCount
										> 0)
										&& (high.tracks[trackIndex].history.stitchedUpdateNextDiffToaCount
												> 0)))
						{
							trackFinalizeTrackHigh(high.tracks[trackIndex]);
							trackFinalizeFlag = 1;
						}
						else
						{
							if (ConstHigh::simLogIterationHigh == 1)
							{
								cout
										<< "Track Finalize Failed (Library) Last Toa: "
										<< high.tracks[trackIndex].general.toaLast
										<< endl;
							}
						}
					}
					else if ((low.tracks[dspFirstReportIndex].history.countPriTypeOverlapMaxLow
							== 0)
							&& (low.tracks[dspFirstReportIndex].history.countPulseWidthOverlapMaxLow
									== 0)
							&& (low.tracks[dspFirstReportIndex].history.countTrackOverlapMaxLow
									== 1)
							&& (low.tracks[dspLastReportIndex].history.countPriTypeOverlapMaxLow
									== 0)
							&& (low.tracks[dspLastReportIndex].history.countPulseWidthOverlapMaxLow
									== 0)
							&& (low.tracks[dspLastReportIndex].history.countTrackOverlapMaxLow
									== 1) && flagComparePriHighHigh == 0)
					{
						trackFinalizeTrackHigh(high.tracks[trackIndex]);
						trackFinalizeFlag = 1;
					}
					else
					{
						if (high.tracks[trackIndex].rf.getBandCodeLegacy() == 2
								|| high.tracks[trackIndex].rf.getBandCodeLegacy()
										== 3)
						{
							trackFinalizeTrackHigh(high.tracks[trackIndex]);
							trackFinalizeFlag = 1;
						}
					}
				}
				else if (unknownTracksInAngleWindowExist == 0
						|| high.tracks[trackIndex].rf.getBandCodeLegacy() == 2
						|| high.tracks[trackIndex].rf.getBandCodeLegacy() == 3)
				{
					//add condition if there being only 2 track overlap for this to execute
					if ((low.tracks[dspFirstReportIndex].history.countPriTypeOverlapMaxLow
							== 0)
							&& (low.tracks[dspFirstReportIndex].history.countPulseWidthOverlapMaxLow
									== 0)
							&& (low.tracks[dspFirstReportIndex].history.countTrackOverlapMaxLow
									== 1)
							&& (low.tracks[dspLastReportIndex].history.countPriTypeOverlapMaxLow
									== 0)
							&& (low.tracks[dspLastReportIndex].history.countPulseWidthOverlapMaxLow
									== 0)
							&& (low.tracks[dspLastReportIndex].history.countTrackOverlapMaxLow
									== 1) && flagComparePriHighHigh == 0)
					{
						trackFinalizeTrackHigh(high.tracks[trackIndex]);
						trackFinalizeFlag = 1;
						//if(ConstHigh::simLogIterationHigh == 1) {
						//	cout << "Environment Entry Redeemed" << endl;
						//}
					}
					else if (/*(high.tracks[trackIndex].rf.getBandCodeLegacy()
							== 0
							|| high.tracks[trackIndex].rf.getBandCodeLegacy()
									== 1)
							&& */(high.tracks[trackIndex].history.totalPulseCount
									> 25))
					{
						trackFinalizeTrackHigh(high.tracks[trackIndex]);
						trackFinalizeFlag = 1;
					}
					else if (flagCompareHighHigh == 0)
					{
						trackFinalizeTrackHigh(high.tracks[trackIndex]);
						trackFinalizeFlag = 1;
					}
					else if ((high.tracks[trackIndex].history.stitchedUpdateCount
							> 0)
							&& (high.tracks[trackIndex].history.stitchedUpdateNextCount
									> 0))
					{
						trackFinalizeTrackHigh(high.tracks[trackIndex]);
						trackFinalizeFlag = 1;
					}
					else if ((high.tracks[trackIndex].history.stitchedUpdateDiffToaCount
							> 0)
							&& (high.tracks[trackIndex].history.stitchedUpdateNextDiffToaCount
									> 0))
					{
						trackFinalizeTrackHigh(high.tracks[trackIndex]);
						trackFinalizeFlag = 1;
					}
					else if ((high.tracks[trackIndex].history.stitchedUpdateCount
							== 0)
							&& (high.tracks[trackIndex].history.stitchedUpdateNextCount
									== 0)
							&& (high.tracks[trackIndex].history.stitchedUpdateDiffToaCount
									== 0)
							&& (high.tracks[trackIndex].history.stitchedUpdateNextDiffToaCount
									== 0))
					{
						trackRedeemStatus =
								trackRedeemEntryParameterEnvironment(high,
										high.tracks[trackIndex]);
						if (trackRedeemStatus == 1)
						{
							trackFinalizeTrackHigh(high.tracks[trackIndex]);
							trackFinalizeFlag = 1;
							//if(ConstHigh::simLogIterationHigh == 1) {
							//	cout << "Environment Entry Redeemed" << endl;
							//}
						}
						else
						{
							trackCreateEntryParameterEnvironment(high,
									high.tracks[trackIndex]);
							//if(ConstHigh::simLogIterationHigh == 1) {
							//	cout << "Environment Entry Created" << endl;
							//}
						}
					}
				}
				if (trackFinalizeFlag == 1)
				{
					if (ConstHigh::simLogIterationHigh == 1)
					{
						cout << "Track Finalize Succeeded (Overlap) Last Toa: "
								<< high.tracks[trackIndex].general.toaLast
								<< endl;
					}
				}
				//else
				//{
				//trackCreateEntryEnvironment(high, high.tracks[trackIndex]);
				//if(ConstHigh::simLogIterationHigh == 1) {
				//	cout << "Environment Entry Created" << endl;
				//}
				//}
			}
//			else if (((low.tracks[dspFirstReportIndex].history.flagOverlapPotential
//					== 1))
//					|| ((low.tracks[dspLastReportIndex].history.flagOverlapPotential
//							== 1)))
//			{
//
//				flagCompareHighHigh = mergeCompareFinalizeHighHigh(
//						high.tracks[trackIndex], high);
//				//old notes
//				//will have to count the number of pulses
//				//check overlap/min pulse count if any and make permanent
//				//if multiple updates make permanent despite little overlap?
//				//incase of long running probation, remove probation status for
//				//tracks that have been organically updated
//				//end of old notes
//				//for debugging purposes
//				//if(high.tracks[trackIndex].last[0].general.toaLast == 23539522760) {
//				//	trackFinalizeFlag = 0;
//				//}
//				//end of debugging
//				if (high.tracks[trackIndex].pw.accumulatePw.size() == 0)
//				{
//					trackFinalizeFlag = 0;
//				}
//				else if ((high.tracks[trackIndex].rf.getBandCodeLegacy() > 3))
//				{
//					trackFinalizeFlag = 0;
//				}
//				//else if((high.tracks[trackIndex].history.totalPulseOverlap > 200) &&
//				//	(high.tracks[trackIndex].history.totalToaOverlap > 100000) &&
//				//	(high.tracks[trackIndex].history.totalPulseCount > 500) &&
//				//	(high.tracks[trackIndex].last[0].pri.flagPriAnalysisLimitedLast == 0)) {
//				//	trackFinalizeTrackSingleHigh(high.tracks[trackIndex]);
//				//	trackFinalizeFlag = 1;
//				//}
//				else if (high.tracks[trackIndex].libmatch.active.flagKnownLibrary
//						== 1)
//				{
//					trackCalcFeasibilityFinalizeTrackHigh(high,
//							high.tracks[trackIndex], categoryMatchCount,
//							indexMatchCount, sttMatchCount, minBearingDiff,
//							sttPwComboCount);
//					if (((categoryMatchCount > 1)
//							&& (high.tracks[trackIndex].libmatch.active.flagAerialOnlyLibrary
//									== 1)) || (indexMatchCount > 1)
//							|| ((indexMatchCount > 0)
//									&& (high.tracks[trackIndex].libmatch.active.flagAerialOnlyLibrary
//											== 0))
//							|| ((indexMatchCount > 0) && (sttMatchCount > 0))
//							|| ((indexMatchCount > 0) && (minBearingDiff < 20))
//							|| ((sttPwComboCount > 0)
//									&& (high.tracks[trackIndex].libmatch.active.flagAerialOnlyLibrary
//											== 1)
//									&& ((high.tracks[trackIndex].rf.getBandCodeLegacy()
//											== 2))))
//					{
//						if (((high.tracks[trackIndex].history.stitchedUpdateCount
//								> 0)
//								&& (high.tracks[trackIndex].history.stitchedUpdateNextCount
//										> 0))
//								|| ((high.tracks[trackIndex].history.stitchedUpdateDiffToaCount
//										> 0)
//										&& (high.tracks[trackIndex].history.stitchedUpdateNextDiffToaCount
//												> 0)))
//						{
//							trackFinalizeTrackHigh(high.tracks[trackIndex]);
//							trackFinalizeFlag = 1;
//						}
//						else
//						{
//							if (ConstHigh::simLogIterationHigh == 1)
//							{
//								cout
//										<< "Track Finalize Failed (Library) Last Toa: "
//										<< high.tracks[trackIndex].general.toaLast
//										<< endl;
//							}
//						}
//					}
//					else if ((low.tracks[dspFirstReportIndex].history.countPriTypeOverlapMaxLow
//							== 0)
//							&& (low.tracks[dspFirstReportIndex].history.countPulseWidthOverlapMaxLow
//									== 0)
//							&& (low.tracks[dspFirstReportIndex].history.countTrackOverlapMaxLow
//									== 1)
//							&& (low.tracks[dspLastReportIndex].history.countPriTypeOverlapMaxLow
//									== 0)
//							&& (low.tracks[dspLastReportIndex].history.countPulseWidthOverlapMaxLow
//									== 0)
//							&& (low.tracks[dspLastReportIndex].history.countTrackOverlapMaxLow
//									== 1))
//					{
//						trackFinalizeTrackHigh(high.tracks[trackIndex]);
//						trackFinalizeFlag = 1;
//					}
//					else
//					{
//						if(high.tracks[trackIndex].rf.getBandCodeLegacy() == 2)
//						{
//							trackFinalizeTrackHigh(high.tracks[trackIndex]);
//							trackFinalizeFlag = 1;
//						}
//					}
//				}
//				//add condition if there being only 2 track overlap for this to execute
//				else if ((low.tracks[dspFirstReportIndex].history.countPriTypeOverlapMaxLow
//						== 0)
//						&& (low.tracks[dspFirstReportIndex].history.countPulseWidthOverlapMaxLow
//								== 0)
//						&& (low.tracks[dspFirstReportIndex].history.countTrackOverlapMaxLow
//								== 1)
//						&& (low.tracks[dspLastReportIndex].history.countPriTypeOverlapMaxLow
//								== 0)
//						&& (low.tracks[dspLastReportIndex].history.countPulseWidthOverlapMaxLow
//								== 0)
//						&& (low.tracks[dspLastReportIndex].history.countTrackOverlapMaxLow
//								== 1))
//				{
//					trackFinalizeTrackHigh(high.tracks[trackIndex]);
//					trackFinalizeFlag = 1;
//				}
//				else if ((high.tracks[trackIndex].rf.getBandCodeLegacy() == 0
//						|| high.tracks[trackIndex].rf.getBandCodeLegacy() == 3)
//						&& (high.tracks[trackIndex].history.totalPulseCount > 25))
//				{
//					trackFinalizeTrackHigh(high.tracks[trackIndex]);
//					trackFinalizeFlag = 1;
//				}
//				else if (flagCompareHighHigh == 0)
//				{
//					trackFinalizeTrackHigh(high.tracks[trackIndex]);
//					trackFinalizeFlag = 1;
//				}
//				else if ((high.tracks[trackIndex].history.stitchedUpdateCount
//						> 0)
//						&& (high.tracks[trackIndex].history.stitchedUpdateNextCount
//								> 0))
//				{
//					trackFinalizeTrackHigh(high.tracks[trackIndex]);
//					trackFinalizeFlag = 1;
//				}
//				else if ((high.tracks[trackIndex].history.stitchedUpdateDiffToaCount
//						> 0)
//						&& (high.tracks[trackIndex].history.stitchedUpdateNextDiffToaCount
//								> 0))
//				{
//					trackFinalizeTrackHigh(high.tracks[trackIndex]);
//					trackFinalizeFlag = 1;
//				}
//				else if ((high.tracks[trackIndex].history.stitchedUpdateCount
//						== 0)
//						&& (high.tracks[trackIndex].history.stitchedUpdateNextCount
//								== 0)
//						&& (high.tracks[trackIndex].history.stitchedUpdateDiffToaCount
//								== 0)
//						&& (high.tracks[trackIndex].history.stitchedUpdateNextDiffToaCount
//								== 0))
//				{
//					trackRedeemStatus = trackRedeemEntryEnvironment(high,
//							high.tracks[trackIndex]);
//					if (trackRedeemStatus == 1)
//					{
//						trackFinalizeTrackHigh(high.tracks[trackIndex]);
//						trackFinalizeFlag = 1;
//						//if(ConstHigh::simLogIterationHigh == 1) {
//						//	cout << "Environment Entry Redeemed" << endl;
//						//}
//					}
//					else
//					{
//						trackCreateEntryEnvironment(high,
//								high.tracks[trackIndex]);
//						//if(ConstHigh::simLogIterationHigh == 1) {
//						//	cout << "Environment Entry Created" << endl;
//						//}
//					}
//				}
//				if (trackFinalizeFlag == 1)
//				{
//					if (ConstHigh::simLogIterationHigh == 1)
//					{
//						cout << "Track Finalize Succeeded (Overlap) Last Toa: "
//								<< high.tracks[trackIndex].general.toaLast
//								<< endl;
//					}
//				}
//				//else
//				//{
//				//trackCreateEntryEnvironment(high, high.tracks[trackIndex]);
//				//if(ConstHigh::simLogIterationHigh == 1) {
//				//	cout << "Environment Entry Created" << endl;
//				//}
//				//}
//			}
			else if (((low.tracks[dspFirstReportIndex].history.flagOverlapPotential
					== 1))
					|| ((low.tracks[dspLastReportIndex].history.flagOverlapPotential
							== 1)))
			{
				unknownTracksInAngleWindowExist =
						mergeFindUnknownsWithinAngleWindow(
								high.tracks[trackIndex], high);
				if (1/*unknownTracksInAngleWindowExist == 0
						|| high.tracks[trackIndex].rf.getBandCodeLegacy() == 2
						|| high.tracks[trackIndex].rf.getBandCodeLegacy() == 3*/)
				{
					trackFinalizeTrackHigh(high.tracks[trackIndex]);
					if (ConstHigh::simLogIterationHigh == 1)
					{
						cout
								<< "Track Finalize Succeeded (No Overlap) Last Toa: "
								<< high.tracks[trackIndex].general.toaLast
								<< endl;
					}
				}
			}
			else
			{
				//no overlap potential. Finalize new threat
				if ((high.tracks[trackIndex].history.totalPulseCount > 25)
						|| (high.tracks[trackIndex].libmatch.active.flagKnownLibrary
								== 1)
						|| (high.tracks[trackIndex].rf.getBandCodeLegacy() == 0))
				{
					trackFinalizeTrackHigh(high.tracks[trackIndex]);
					if (ConstHigh::simLogIterationHigh == 1)
					{
						cout
								<< "Track Finalize Succeeded (No Overlap) Last Toa: "
								<< high.tracks[trackIndex].general.toaLast
								<< endl;
					}
				}
			}
		}
	}
}

void trackFinalizeTrackHigh(TrackDefaultClass &track)
{
	track.general.flagProbation = 0;
	track.general.flagVisibleHigh = 0;
}
void trackCalcFeasibilityFinalizeTrackHigh(DataSensorClass &high,
		TrackDefaultClass &highTrack, uint8_t &categoryMatchCount,
		uint8_t &indexMatchCount, uint8_t &sttMatchCount,
		uint16_t &minBearingDiff, uint8_t &sttPwComboCount)
{

	uint8_t trackIndex = 0;
	uint8_t bearingComparisonDelta = ConstHigh::mergeLimitFinalizeTrackBearing;
	uint16_t angleDiff = 0;
	uint16_t angleDiffQuadrantSwap = 0;
	uint16_t pwDifference = 0;

	categoryMatchCount = 0;
	indexMatchCount = 0;
	sttMatchCount = 0;
	minBearingDiff = 360;
	sttPwComboCount = 0;

	for (trackIndex = 0; trackIndex < high.tracks.size(); trackIndex++)
	{
		if (high.tracks[trackIndex].rf.bandCode == highTrack.rf.bandCode)
		{
			if (high.tracks[trackIndex].general.flagProbation == 0)
			{
				if ((high.tracks[trackIndex].libmatch.active.flagKnownLibrary
						== 1)
						&& (highTrack.libmatch.active.flagKnownLibrary == 1))
				{
					if (high.tracks[trackIndex].libmatch.active.highestMatchList[0].emitterBatchId
							== highTrack.libmatch.active.highestMatchList[0].emitterBatchId)
					{
						mergeCalcBearingDifferenceHighHigh(
								high.tracks[trackIndex], highTrack, angleDiff,
								angleDiffQuadrantSwap);
						if (angleDiff < bearingComparisonDelta)
						{
							categoryMatchCount = categoryMatchCount + 1;
							if (angleDiff < minBearingDiff)
							{
								minBearingDiff = angleDiff;
							}
							if (high.tracks[trackIndex].libmatch.active.highestMatchList[0].ID
									== highTrack.libmatch.active.highestMatchList[0].ID)
							{
								indexMatchCount = indexMatchCount + 1;
							}
							if ((high.tracks[trackIndex].power.countTrackingModeIterHigh
									> ConstHigh::trackLimitPartialConsecutiveUpdateIter)
									|| (high.tracks[trackIndex].power.flagTrackingModeHigh
											== 1))
							{
								sttMatchCount = sttMatchCount + 1;
							}
						}
					}
				}
				if ((high.tracks[trackIndex].power.countTrackingModeIterHigh
						> ConstHigh::trackLimitPartialConsecutiveUpdateIter)
						|| (high.tracks[trackIndex].power.flagTrackingModeHigh
								== 1))
				{
					if ((high.tracks[trackIndex].libmatch.active.flagKnownLibrary
							== 0)
							|| ((high.tracks[trackIndex].libmatch.active.flagKnownLibrary
									== 1)
									&& (high.tracks[trackIndex].libmatch.active.flagAerialOnlyLibrary
											== 1)))
					{
						mergeCalcBearingDifferenceHighHigh(
								high.tracks[trackIndex], highTrack, angleDiff,
								angleDiffQuadrantSwap);
						if (angleDiff < bearingComparisonDelta)
						{
							pwDifference =
									labs(
											(int16_t) high.tracks[trackIndex].pw.accumulatePw.back().pwLow
													- (int16_t) highTrack.pw.accumulatePw.back().pwLow);
							//this previously was only 150ns so take care
							if (pwDifference
									< ConstHigh::mergeDeltaStringPwMedium)
							{
								sttPwComboCount = sttPwComboCount + 1;
							}
						}
					}
				}
			}
		}
	}
}
void trackReleaseTracksAllHigh(DataSensorClass &high)
{

	uint16_t i = 0;
	libraryAssignSymbolEmitterAll(high.tracks, Model::library);

	libraryCalculatePriorityOfTracks(high.tracks, Model::library);

	trackCfdDataTracksHigh(high.tracks, Model::library, Model::cfdLibrary);

	//libraryHideTracksWithSameDemodPtr(high.tracks, Model::library);

	if (ConstHigh::simLogIterationHigh == 1)
	{
		cout << "--" << endl;
	}
	for (i = 0; i < high.tracks.size(); i++)
	{
		if (ConstHigh::simLogIterationHigh == 1)
		{
			cout << "High[" << i << "] " << high.tracks[i].general.toaFirst
					<< " - " << high.tracks[i].general.toaLast << " - AOA "
					<< high.tracks[i].angle.bearingLast << " - QCode "
					<< (uint16_t) high.tracks[i].power.quadrantCodeMax << endl;
		}
	}
}
int16_t trackGetSortedTrackIndexLowNext(DataSensorClass &low)
{
	int16_t returnIndex;
	uint16_t index;
	uint16_t lowestIndex;
	uint64_t lowestStartToa = 0;

	for (index = 0; index < low.tracks.size(); index++)
	{
		if (low.tracks[index].general.processingHighFlag == 1)
		{
			continue;
		}
		if (lowestStartToa == 0)
		{
			lowestIndex = index;
			lowestStartToa = low.tracks[index].general.toaFirst;
		}
		else if (low.tracks[index].general.toaFirst < lowestStartToa)
		{
			lowestIndex = index;
			lowestStartToa = low.tracks[index].general.toaFirst;
		}
	}
	if (lowestStartToa > 0)
	{
		returnIndex = (int16_t) lowestIndex;
		return returnIndex;
	}
	return -1;
}
void trackStringAndInsert(DataSensorClass &low, DataSensorClass &high)
{

	float complexPercentage;
	int16_t lowIndex = 0;
	uint16_t maxScore = 0;
	int16_t maxScoreIndex = 0;
	uint16_t highIndex = 0;
	uint16_t trackLastLowIndex = 0;
	int16_t maxScoreLastLowIndex = 0;
	uint8_t trackStringOrUpdateFlag = 0;
	uint32_t newMaxScore = 0;
	while (lowIndex != -1)
	{
		maxScore = 0;
		newMaxScore = 0;
		maxScoreIndex = -1;
		maxScoreLastLowIndex = -1;
		lowIndex = trackGetSortedTrackIndexLowNext(low);

		if (lowIndex == -1)
			break;
		//debugging
		if(low.tracks[lowIndex].general.toaLast == 1347087600) {
			low.tracks[lowIndex].general.toaLast = 1347087600;
		}
		//end of debugging
		for (highIndex = 0; highIndex < high.tracks.size(); highIndex++)
		{
			//debugging
			if (
//					(high.tracks[highIndex].general.toaLast == 220276311290) &&
			(low.tracks[lowIndex].general.toaLast == 69027000))
			{
				cout << "hi" << endl;
			}
			//end of debugging
			trackStringOrUpdateFlag = 0;
			//if the high track already has had low reports in this iteration (organic or strung in this function)
			if (high.tracks[highIndex].history.indexAssociateTracksIterLow.size()
					> 0)
			{
				trackLastLowIndex =
						high.tracks[highIndex].history.indexAssociateTracksIterLow.back();
				trackStringOrUpdateFlag = 1;
			}
			//if this high track has no organic update or stitched report in the current iteration
			if (trackStringOrUpdateFlag == 0)
			{
				//initial comparison if not done
				if (high.compareHighLow(lowIndex, highIndex).parameterInitialStatus
						== 0)
					mergeCompareInitialHighLow(low, high, (uint16_t) lowIndex,
							highIndex);
				//if stitch compatible
				if (high.compareHighLow(lowIndex, highIndex).stitchCompatible
						== 1)
				{
					//score comparison if not done
					if (high.compareHighLow(lowIndex, highIndex).parameterCompareStatus
							== 0)
						mergeCalcScoreStringHighLow(low, high,
								(uint16_t) lowIndex, highIndex);
					if ((high.compareHighLow(lowIndex, highIndex).overallScore
							> 0)
							&& (high.compareHighLow(lowIndex, highIndex).parameterExclusionStatus
									== 0))
					{
						//additional exclude check
						if (mergeFlagStringExcludeHighLow(low, high, lowIndex,
								highIndex) == 0)
						{
							//replace last maxscore and index
							newMaxScore = high.compareHighLow(lowIndex,
									highIndex).overallScore;
							if (newMaxScore > maxScore)
							{
								if (maxScore
										> ConstHigh::mergeScoreStringRequired
										&& (maxScore - newMaxScore) < 5)
								{
									if (maxScoreLastLowIndex != -1)
									{
										if (low.compareLowLow(lowIndex,
												maxScoreLastLowIndex).priInitialScore
												> 0
												&& low.compareLowLow(lowIndex,
														maxScoreLastLowIndex).pwScoreTen
														> 0)
										{

										}
										else
										{
											maxScore = newMaxScore;
											maxScoreIndex = (int16_t) highIndex;
											maxScoreLastLowIndex = -1;
										}
									}
									else
									{
										if (high.compareHighLow(lowIndex,
												maxScoreIndex).priInitialScore
												> 0
												&& high.compareHighLow(lowIndex,
														maxScoreIndex).pwScoreTen
														> 0)
										{

										}
										else
										{
											maxScore = newMaxScore;
											maxScoreIndex = (int16_t) highIndex;
											maxScoreLastLowIndex = -1;
										}
									}
								}
								else
								{
									maxScore = newMaxScore;
									maxScoreIndex = (int16_t) highIndex;
									maxScoreLastLowIndex = -1;
								}
							}
						}
					}
				}
			}
			//we know the last low report that got stitched/updated into this high track in the current iteration
			else
			{
				//trackLastReportIndex = trackingTrackPotentialList[trackIndex].trackingReportStitched[trackingTrackPotentialList[trackIndex].trackingReportStitchedCount - 1];
				//if stitch compatible
				if (low.compareLowLow(lowIndex, trackLastLowIndex).stitchCompatible
						== 1)
				{
					if (low.compareLowLow(lowIndex, trackLastLowIndex).parameterCompareStatus
							== 0)				//score comparison if not done
						mergeCalcScoreStringLowLow(low, high, lowIndex,
								trackLastLowIndex);
					if ((low.compareLowLow(lowIndex, trackLastLowIndex).overallScore
							> 0)
							&& (low.compareLowLow(lowIndex, trackLastLowIndex).parameterExclusionStatus
									== 0))
					{
						//additional exclude check
						if (mergeFlagStringExcludeHighLow(low, high, lowIndex,
								highIndex) == 0)
						{
							//replace last maxscore and index
							newMaxScore = low.compareLowLow(lowIndex,
									trackLastLowIndex).overallScore;
							if (newMaxScore > maxScore)
							{
								if (maxScore
										> ConstHigh::mergeScoreStringRequired
										&& (maxScore - newMaxScore) < 5)
								{
									if (maxScoreLastLowIndex != -1)
									{
										if (low.compareLowLow(lowIndex,
												maxScoreLastLowIndex).priInitialScore
												> 0
												&& low.compareLowLow(lowIndex,
														maxScoreLastLowIndex).pwScoreTen
														> 0)
										{

										}
										else
										{
											maxScore = newMaxScore;
											maxScoreIndex = (int16_t) highIndex;
											maxScoreLastLowIndex =
													(int16_t) trackLastLowIndex;
										}
									}
									else
									{
										if (high.compareHighLow(lowIndex,
												maxScoreIndex).priInitialScore
												> 0
												&& high.compareHighLow(lowIndex,
														maxScoreIndex).pwScoreTen
														> 0)
										{

										}
										else
										{
											maxScore = newMaxScore;
											maxScoreIndex = (int16_t) highIndex;
											maxScoreLastLowIndex =
													(int16_t) trackLastLowIndex;
										}
									}
								}
								else
								{
									maxScore = newMaxScore;
									maxScoreIndex = (int16_t) highIndex;
									maxScoreLastLowIndex =
											(int16_t) trackLastLowIndex;
								}
							}
						}
					}
				}
				else
				{//if (mergeFlagStringExcludeHighLow(low, high, lowIndex, highIndex) == 1) {
					continue;
				}
				//also compare with the second last report that got stiched/updated into this high track
				if (high.tracks[highIndex].history.indexAssociateTracksIterLow.size()
						> 1)
				{
					trackLastLowIndex =
							high.tracks[highIndex].history.indexAssociateTracksIterLow.rbegin()[1];
					//if stitch compatible
					if (low.compareLowLow(lowIndex, trackLastLowIndex).stitchCompatible
							== 1)
					{
						if (low.compareLowLow(lowIndex, trackLastLowIndex).parameterCompareStatus
								== 0)			//score comparison if not done
							mergeCalcScoreStringLowLow(low, high, lowIndex,
									trackLastLowIndex);
						if ((low.compareLowLow(lowIndex, trackLastLowIndex).overallScore
								> 0)
								&& (low.compareLowLow(lowIndex,
										trackLastLowIndex).parameterExclusionStatus
										== 0))
						{
							//additional exclude check
							if (mergeFlagStringExcludeHighLow(low, high,
									lowIndex, highIndex) == 0)
							{
								//replace last maxscore and index
								newMaxScore = low.compareLowLow(lowIndex,
										trackLastLowIndex).overallScore;
								if (newMaxScore > maxScore)
								{
									if (maxScore
											> ConstHigh::mergeScoreStringRequired
											&& (maxScore - newMaxScore) < 5)
									{
										if (maxScoreLastLowIndex != -1)
										{
											if (low.compareLowLow(lowIndex,
													maxScoreLastLowIndex).priInitialScore
													> 0
													&& low.compareLowLow(
															lowIndex,
															maxScoreLastLowIndex).pwScoreTen
															> 0)
											{

											}
											else
											{
												maxScore = newMaxScore;
												maxScoreIndex =
														(int16_t) highIndex;
												maxScoreLastLowIndex =
														(int16_t) trackLastLowIndex;
											}
										}
										else
										{
											if (high.compareHighLow(lowIndex,
													maxScoreIndex).priInitialScore
													> 0
													&& high.compareHighLow(
															lowIndex,
															maxScoreIndex).pwScoreTen
															> 0)
											{

											}
											else
											{
												maxScore = newMaxScore;
												maxScoreIndex =
														(int16_t) highIndex;
												maxScoreLastLowIndex =
														(int16_t) trackLastLowIndex;
											}
										}
									}
									else
									{
										maxScore = newMaxScore;
										maxScoreIndex = (int16_t) highIndex;
										maxScoreLastLowIndex =
												(int16_t) trackLastLowIndex;
									}
								}
							}
						}
					}
				}
			}
		}
		//debugging
		//if(low.tracks[reportIndex].general.toaLast == 6266848848)
		//{
		//	low.tracks[reportIndex].general.toaLast = 6266848848;
		//}
		//end of debugging
		//stitch report
		if ((maxScoreIndex != -1)
				&& (maxScore > ConstHigh::mergeScoreStringRequired))
		{
			//maybe move this condition out of here
			//library request match
			if (((low.tracks[lowIndex].pri.accumulatePriDwellSwitchCount > 0)
					&& (high.tracks[maxScoreIndex].last[0].pri.accumulatePriStaggerCount
							> 0))
					|| ((low.tracks[lowIndex].pri.accumulatePriStaggerCount > 0)
							&& (high.tracks[maxScoreIndex].last[0].pri.accumulatePriDwellSwitchCount
									> 0)))
			{
				high.tracks[maxScoreIndex].libmatch.flagRequestLibraryMatch = 1;
			}
			if (ConstHigh::simLogIterationHigh == 1)
			{
				cout << "Stringing Toas H: "
						<< high.tracks[maxScoreIndex].general.toaFirst << "-"
						<< high.tracks[maxScoreIndex].general.toaLast << " L: "
						<< low.tracks[lowIndex].general.toaFirst << "-"
						<< low.tracks[lowIndex].general.toaLast << endl;
			}
			trackUpdateParameterString(low.tracks[lowIndex],
					low.tracks[lowIndex], high.tracks[maxScoreIndex], lowIndex,
					lowIndex, maxScoreIndex);
			low.tracks[lowIndex].general.processingHighFlag = 1;
		}
		else if (high.tracks.size() < ConstHigh::trackLimitMaxEmittersHigh)
		{	//create new track on probation status
			trackCreateTrackProbationHigh(high, low.tracks[lowIndex],
					low.tracks[lowIndex], lowIndex, lowIndex);
			if (ConstHigh::simLogIterationHigh == 1)
			{
				cout << "New track probation Band: "
						<< (int16_t) (low.tracks[lowIndex].rf.getBandCodeLegacy())
						<< " Toa: " << low.tracks[lowIndex].general.toaFirst
						<< "-" << low.tracks[lowIndex].general.toaLast << endl;
			}
			low.tracks[lowIndex].general.processingHighFlag = 1;
		}
	}
	//update the library match for each track based on certain conditions
	//update the complex pri flag for each track based on number of reports
	for (highIndex = 0; highIndex < high.tracks.size(); highIndex++)
	{
		//mmaybe move this condition out of here
		if ((high.tracks[highIndex].general.flagProbation == 1)
				|| (high.tracks[highIndex].general.timeIterCountLifetimeGlobal
						< 2))
		{
			//library request match
			high.tracks[highIndex].libmatch.flagRequestLibraryMatch = 1;
		}
		complexPercentage =
				((float) high.tracks[highIndex].history.stitchedDiffToaCount)
						/ ((float) high.tracks[highIndex].history.stitchedCount);
		if ((complexPercentage >= 0.50)
				&& (high.tracks[highIndex].pri.flagPriAnalysisLimitedFirst == 1))
		{
			high.tracks[highIndex].pri.flagPriAnalysisLimitedLast = 1;
		}
		else if (complexPercentage >= 0.70)
		{
			high.tracks[highIndex].pri.flagPriAnalysisLimitedLast = 1;
		}
		else if ((complexPercentage < 50.0)
				&& (high.tracks[highIndex].pri.flagPriAnalysisLimitedLast == 1))
		{
			high.tracks[highIndex].pri.flagPriAnalysisLimitedLast = 0;
		}
		if (ConstHigh::simLogIterationHigh == 1)
		{
			cout << "High[" << highIndex << "] "
					<< high.tracks[highIndex].general.toaFirst << " - "
					<< high.tracks[highIndex].general.toaLast << " - AOA "
					<< high.tracks[highIndex].angle.bearingLast << " - QCode "
					<< (uint16_t) high.tracks[highIndex].power.quadrantCodeMax
					<< endl;
		}
	}
}

