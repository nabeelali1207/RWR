/*
 * analysistrackmergehighdelegate.cpp
 *
 *  Created on: Nov 20, 2018
 *      Author: hassan
 */

#define ANALYSISTRACKMERGEHIGHDELEGATE_CPP_

#include "analysistrackmergehighdelegate.h"

void mergeTracksAllHigh(DataSensorClass &low, DataSensorClass &high)
{

	int16_t foundPair = 0;
	uint16_t sourceTrackIndex = 0;
	uint16_t targetTrackIndex = 0;

	//analysisVariableMergeAngle = 35;
	//analysisVariableMergeAngleQuadrantSwap = 15;
	while (foundPair != -1)
	{
		foundPair = -1;
		for (sourceTrackIndex = 0; sourceTrackIndex < high.tracks.size();
				sourceTrackIndex++)
		{
			if (high.tracks[sourceTrackIndex].general.flagDeletionHigh == 1)
			{
				continue;
			}
			for (targetTrackIndex = 0; targetTrackIndex < high.tracks.size();
					targetTrackIndex++)
			{
				if (sourceTrackIndex == targetTrackIndex)
				{
					continue;
				}
				if ((high.tracks[sourceTrackIndex].general.toaLast
						== 1816750000)
						&& (high.tracks[targetTrackIndex].general.toaLast
								== 1809411000))
					high.tracks[targetTrackIndex].general.toaLast = 1809411000;
				if ((high.tracks[sourceTrackIndex].general.toaLast
						== 202899746120)
						&& (high.tracks[targetTrackIndex].general.toaLast
								== 202897683320))
					high.tracks[sourceTrackIndex].general.toaLast =
							202899746120;

				if (high.tracks[targetTrackIndex].general.flagDeletionHigh == 1)
				{
					continue;
				}
				if (high.tracks[sourceTrackIndex].general.flagDeletionHigh == 1)
				{
					continue;
				}
				if (high.compareHighHigh(sourceTrackIndex, targetTrackIndex).parameterInitialStatus
						== 1)
				{
					continue;
				}
				//for debugging purposes
				//important 8th may 2600 8395490218
				//if((high.tracks[sourceTrackIndex].last[0].general.toaLast == 35001060681) &&
				//		(high.tracks[targetTrackIndex].last[0].general.toaLast == 35005202515)) {
				//	foundPair = -1;
				//}
				//if((high.tracks[targetTrackIndex].last[0].general.toaLast == 35001060681) &&
				//		(high.tracks[sourceTrackIndex].last[0].general.toaLast == 35005202515)) {
				//	foundPair = -1;
				//}
				//end of debugging
				//prune tracks here via a criteria
				if ((high.tracks[sourceTrackIndex].last[0].pri.flagPriAnalysisLimitedLast
						== 1)
						|| (high.tracks[targetTrackIndex].last[0].pri.flagPriAnalysisLimitedLast
								== 1))
				{
					foundPair = mergeTracksWithUncertainPri(low, high,
							high.tracks[sourceTrackIndex],
							high.tracks[targetTrackIndex], sourceTrackIndex,
							targetTrackIndex);
				}
				else
				{
					foundPair = mergeTracksWithNormalPri(low, high,
							high.tracks[sourceTrackIndex],
							high.tracks[targetTrackIndex], sourceTrackIndex,
							targetTrackIndex);
				}
				/*if(foundPair != -1) {
				 if(ConstHigh::simLogIterationHigh == 1) {
				 cout << "Merging Last Toas H: " <<
				 high.tracks[sourceTrackIndex].general.toaLast <<
				 " Q: " << (int)high.tracks[sourceTrackIndex].power.quadrantCodeMax<<
				 " AOA: " << high.tracks[sourceTrackIndex].angle.bearingLast<<
				 " H: " << high.tracks[targetTrackIndex].general.toaLast <<
				 " Q: " << (int)high.tracks[targetTrackIndex].power.quadrantCodeMax<<
				 " AOA: " << high.tracks[targetTrackIndex].angle.bearingLast<<endl;
				 }
				 }*/
			}
			if (foundPair != -1)
			{
				break;
			}
		}
	}
}

int16_t mergeTracksWithNormalPri(DataSensorClass &low, DataSensorClass &high,
		TrackDefaultClass &trackOne, TrackDefaultClass &trackTwo,
		uint16_t indexTrackOne, uint16_t indexTrackTwo)
{

	uint16_t reportSourceActualIndex = 0;
	uint16_t reportTargetActualIndex = 0;
	uint16_t angleDiff = 0;
	uint16_t angleDiffQuadrantSwap = 0;
	uint8_t bandCompatible = 0;
	uint16_t powerDiff = 0;
	uint16_t pwDiff = 0;
	uint16_t pwDiffAccumulate = 0;
	uint16_t rfStatus = 0;
	uint8_t parameterExclusionStatus = 0;
	uint8_t parameterSpecialPermission = 0;
	uint8_t libraryMatchStatus = 0;
	uint8_t priInitialScore = 0;
	uint16_t priInitialScoreAccumulate = 0;
	//uint8_t  priDetailScore = 0;
	uint16_t mergeBearing = 0;
	uint16_t mergeBearingQuadrantSwap = 0;
	uint16_t mergeSourceIndex = 0;
	uint16_t mergeTargetIndex = 0;
	//uint32_t totalPulseOverlap = 0;
	//uint32_t totalToaOverlap = 0;
	//uint32_t totalPulseCount = 0;
	//uint32_t totalToaCount = 0;
	//float pulseOverlapPercent = 1;

	uint64_t toaDifference;

	if (high.compareHighHigh(indexTrackOne, indexTrackTwo).parameterInitialStatus
			== 0)
	{
		high.compareHighHigh(indexTrackOne, indexTrackTwo).parameterInitialStatus =
				1;
		high.compareHighHigh(indexTrackTwo, indexTrackOne).parameterInitialStatus =
				1;
	}
	bandCompatible = 1;
	if (bandCompatible == 0)
	{
		return -1;
	}
	//merge angle change
	if (trackOne.rf.bandCode == BandCodeEnum::band_e)
	{
		mergeBearing = ConstHigh::mergeLimitMergeLowBandBearingPrimary;
		mergeBearingQuadrantSwap =
				ConstHigh::mergeLimitMergeLowBandBearingSecondary;
	}
	else if ((trackOne.rf.bandCode != BandCodeEnum::band_i)
			|| (trackOne.last[0].pri.accumulatePriStaggerCount > 0)
			|| (trackTwo.last[0].pri.accumulatePriStaggerCount > 0))
	{
		mergeBearing = ConstHigh::mergeLimitMergePriStaggerBearingPrimary;
		mergeBearingQuadrantSwap =
				ConstHigh::mergeLimitMergePriStaggerBearingSecondary;
	}
	else
	{
		mergeBearing = ConstHigh::mergeLimitMergeDefaultBearingPrimary;
		mergeBearingQuadrantSwap =
				ConstHigh::mergeLimitMergeDefaultBearingSecondary;
	}
	//end of merge angle change
	mergeCalcBearingDifferenceHighHigh(trackOne, trackTwo, angleDiff,
			angleDiffQuadrantSwap);
	if ((angleDiff > mergeBearing)
			&& (angleDiffQuadrantSwap > mergeBearingQuadrantSwap))
	{
		return -1;
	}
	if ((trackOne.history.highToaStartUpdateConsecutiveCount > 2)
			&& (trackTwo.history.highToaStartUpdateConsecutiveCount > 2)
			&& ((trackOne.last[0].pri.accumulatePriJitterCount == 0)
					&& (trackOne.last[0].pri.accumulatePriComplexCount == 0))
			&& ((trackTwo.last[0].pri.accumulatePriJitterCount == 0)
					&& (trackTwo.last[0].pri.accumulatePriComplexCount == 0)))
	{
		return -1;
	}
	if ((trackOne.history.highToaStartUpdateConsecutiveCount > 3)
			&& (trackTwo.history.highToaStartUpdateConsecutiveCount > 3))
	{
		return -1;
	}
	if ((trackOne.general.timeIterCountLifetime > 2)
			&& (trackOne.general.timeIterCountLifetimeGlobal > 10)
			&& (trackTwo.general.timeIterCountLifetime == 0))
	{
		if (((trackOne.libmatch.active.flagKnownLibrary == 1)
				&& (trackTwo.libmatch.active.flagKnownLibrary == 1)
				&& (trackOne.libmatch.active.highestMatchList[0].ID
						== trackTwo.libmatch.active.highestMatchList[0].ID))
				|| (trackOne.libmatch.active.flagKnownLibrary == 0)
				|| (trackTwo.libmatch.active.flagKnownLibrary == 0))
		{
			pwDiff = mergeDifferencePw(trackOne.last[0], trackTwo.last[0]);
			rfStatus = mergeCalcStatusStringRf(trackOne.last[0],
					trackTwo.last[0]);
			priInitialScore = mergeCalcScoreStringPri(trackOne.last[0],
					trackTwo.last[0]);
			powerDiff = labs(
					(int16_t) trackOne.last[0].power.sumPxPyMax
							- (int16_t) trackTwo.last[0].power.sumPxPyMax);
			if ((pwDiff < ConstHigh::mergeDeltaMergePwSmall) && (rfStatus != 3)
					&& (priInitialScore > 5)
					&& (powerDiff < ConstHigh::mergeDeltaMergePowerMedium)
					&& (angleDiff < ConstHigh::mergeDeltaMergeBearingSmall))
			{
				parameterSpecialPermission = 1;
			}
		}
	}
	else if ((trackTwo.general.timeIterCountLifetime > 2)
			&& (trackTwo.general.timeIterCountLifetimeGlobal > 10)
			&& (trackOne.general.timeIterCountLifetime == 0))
	{
		if (((trackOne.libmatch.active.flagKnownLibrary == 1)
				&& (trackTwo.libmatch.active.flagKnownLibrary == 1)
				&& (trackOne.libmatch.active.highestMatchList[0].ID
						== trackTwo.libmatch.active.highestMatchList[0].ID))
				|| (trackOne.libmatch.active.flagKnownLibrary == 0)
				|| (trackTwo.libmatch.active.flagKnownLibrary == 0))
		{
			pwDiff = mergeDifferencePw(trackOne.last[0], trackTwo.last[0]);
			rfStatus = mergeCalcStatusStringRf(trackOne.last[0],
					trackTwo.last[0]);
			priInitialScore = mergeCalcScoreStringPri(trackOne.last[0],
					trackTwo.last[0]);
			powerDiff = labs(
					(int16_t) trackOne.last[0].power.sumPxPyMax
							- (int16_t) trackTwo.last[0].power.sumPxPyMax);
			if ((pwDiff < ConstHigh::mergeDeltaMergePwSmall) && (rfStatus != 3)
					&& (priInitialScore > 5)
					&& (powerDiff < ConstHigh::mergeDeltaMergePowerMedium)
					&& (angleDiff < ConstHigh::mergeDeltaMergeBearingSmall))
			{
				parameterSpecialPermission = 1;
			}
		}
	}
	else if ((trackOne.general.timeIterCountLifetimeGlobal > 10)
			&& (trackTwo.general.timeIterCountLifetimeGlobal > 10))
	{
		if (((trackOne.libmatch.active.flagKnownLibrary == 1)
				&& (trackTwo.libmatch.active.flagKnownLibrary == 1)
				&& (trackOne.libmatch.active.highestMatchList[0].ID
						== trackTwo.libmatch.active.highestMatchList[0].ID))
				&& (trackOne.history.stitchedUpdateNextCount == 0)
				&& (trackTwo.history.stitchedUpdateNextCount == 0))
		{
			//The previous code also allowed this condition if both tracks were unknown, so be careful
			//|| ((trackOne.libmatch.current.flagKnownLibrary == 0) && (trackTwo.libmatch.current.flagKnownLibrary == 0)
			parameterSpecialPermission = 1;
		}
	}
	if (((trackOne.libmatch.active.flagKnownLibrary == 1)
			&& (trackTwo.libmatch.active.flagKnownLibrary == 1)
			&& (trackOne.libmatch.active.highestMatchList[0].ID
					== trackTwo.libmatch.active.highestMatchList[0].ID))
			&& (trackOne.libmatch.active.flagAerialOnlyLibrary == 0)
			&& (trackTwo.libmatch.active.flagAerialOnlyLibrary == 0))
	{
		parameterSpecialPermission = 1;
	}
	if ((trackOne.last[0].pri.accumulatePriStaggerCount > 0)
			|| (trackTwo.last[0].pri.accumulatePriStaggerCount > 0))
	{
		if ((trackOne.libmatch.active.flagKnownLibrary == 1)
				&& (trackTwo.libmatch.active.flagKnownLibrary == 1)
				&& (trackOne.libmatch.active.highestMatchList[0].emitterBatchId
						!= trackTwo.libmatch.active.highestMatchList[0].emitterBatchId))
		{
			priInitialScore = 0;
		}
		else if (((trackOne.last[0].pri.accumulatePriDwellSwitchCount > 0)
				&& (trackOne.last[0].pri.accumulatePriFixedCount == 0))
				|| ((trackTwo.last[0].pri.accumulatePriDwellSwitchCount > 0)
						&& (trackTwo.last[0].pri.accumulatePriFixedCount == 0)))
		{
			priInitialScore = 0;
		}
		else
		{
			priInitialScore = mergeCalcScoreStringPri(trackOne.last[0],
					trackTwo.last[0]);
		}
		if (priInitialScore >= 10)
		{
			parameterSpecialPermission = 1;
		}
	}
	if (parameterSpecialPermission == 0)
	{
		if ((trackOne.general.timeIterCountLifetimeGlobal > 0)
				&& (trackTwo.general.timeIterCountLifetimeGlobal > 0))
		{
			parameterExclusionStatus = 1;
		}
		if ((trackOne.history.stitchedUpdateCount > 0)
				&& (trackTwo.history.stitchedUpdateCount > 0))
		{
			reportSourceActualIndex =
					trackOne.history.indexAssociateTracksIterLow[0];	//review
			reportTargetActualIndex =
					trackTwo.history.indexAssociateTracksIterLow[0];	//review
			if (((trackOne.general.flagProbation == 1)
					&& (low.tracks[reportSourceActualIndex].history.lowToaStartUpdateFlag
							== 1))
					|| ((trackTwo.general.flagProbation == 1)
							&& (low.tracks[reportTargetActualIndex].history.lowToaStartUpdateFlag
									== 1)))
			{
				parameterExclusionStatus = 0;
			}
			else
			{
				if ((trackOne.last[0].pri.accumulatePriJitterCount == 0)
						&& (trackOne.last[0].pri.accumulatePriComplexCount == 0)
						&& (trackTwo.last[0].pri.accumulatePriComplexCount == 0)
						&& (trackTwo.last[0].pri.accumulatePriJitterCount == 0))
				{
					parameterExclusionStatus = 1;
				}
			}
		}
		if ((trackOne.history.stitchedUpdateNextCount > 0)
				&& (trackTwo.history.stitchedUpdateNextCount > 0))
		{
			//if number of updates exceeds a certain number, deny merge and null special permissions. Maybe compare future pulse widths as well
			if ((trackOne.last[0].pri.accumulatePriJitterCount == 0)
					&& (trackOne.last[0].pri.accumulatePriComplexCount == 0)
					&& (trackTwo.last[0].pri.accumulatePriComplexCount == 0)
					&& (trackTwo.last[0].pri.accumulatePriJitterCount == 0))
			{
				parameterExclusionStatus = 1;
			}
		}
		//excluse based on tl category
		if ((trackOne.libmatch.active.flagKnownLibrary == 1)
				&& (trackTwo.libmatch.active.flagKnownLibrary == 1))
		{
			if (trackOne.libmatch.active.highestMatchList[0].emitterBatchId
					!= trackTwo.libmatch.active.highestMatchList[0].emitterBatchId)
			{
				if ((trackOne.rf.getBandCodeLegacy() == 2)
						&& (trackTwo.last[0].rf.getBandCodeLegacy() == 2))
				{
					if (trackOne.libmatch.active.highestMatchList[0].dwellCount
							> 0
							&& trackTwo.libmatch.active.highestMatchList[0].dwellCount
									> 0)
					{
						if (trackOne.libmatch.active.highestMatchList[0].noOfSpotsMatched
								== trackOne.libmatch.active.highestMatchList[0].libraryPriCount
								|| trackOne.libmatch.active.highestMatchList[0].noOfSpotsMatched
										>= 3)
						{
							parameterExclusionStatus = 1;
						}
					}
					else
					{
						parameterExclusionStatus = 1;
					}
				}
				else
				{
					parameterExclusionStatus = 1;
				}
			}
//			if (trackOne.libmatch.active.highestMatchList[0].emitterBatchId
//					!= trackTwo.libmatch.active.highestMatchList[0].emitterBatchId) {
//				parameterExclusionStatus = 1;
		}
		else if ((trackOne.libmatch.active.highestMatchList[0].ID
				== trackTwo.libmatch.active.highestMatchList[0].ID)
				&& (angleDiff < ConstHigh::mergeDeltaMergeBearingSmall))
		{
			if (trackOne.libmatch.active.flagKnownLibrary == 1
					&& trackTwo.libmatch.active.flagKnownLibrary == 1)
				libraryMatchStatus = 1;			//review
		}
	}

	if ((libraryMatchStatus == 0) && (parameterExclusionStatus == 0)
			&& (parameterSpecialPermission == 0))
	{
		if (trackOne.general.toaFirst > trackTwo.general.toaLast)
		{
			toaDifference = trackOne.general.toaFirst
					- trackTwo.general.toaLast;
		}
		else if (trackTwo.general.toaFirst > trackOne.general.toaLast)
		{
			toaDifference = trackTwo.general.toaFirst
					- trackOne.general.toaLast;
		}
		else
		{
			toaDifference = 2100000;
		}

		pwDiff = mergeDifferencePw(trackOne.last[0], trackTwo.last[0]);
		rfStatus = mergeCalcStatusStringRf(trackOne.last[0], trackTwo.last[0]);
		if (pwDiff > ConstHigh::mergeDeltaMergePwSmall)
		{			//stt rule here for variable limit
			if (priInitialScore == 0)
			{
				priInitialScore = mergeCalcScoreStringPri(trackOne.last[0],
						trackTwo.last[0]);
				if (priInitialScore < 10 && (angleDiff < 20))
				{
					if (trackOne.rf.getBandCodeLegacy() == 1
							|| trackOne.rf.getBandCodeLegacy() == 0)
					{
						priInitialScoreAccumulate = mergeCalcScoreStringPri(
								trackOne, trackTwo);
						pwDiffAccumulate = mergeDifferencePwAccumulate(trackOne,
								trackTwo);
						priInitialScore = priInitialScoreAccumulate;
						pwDiff = pwDiffAccumulate;
					}
				}
			}
			if (priInitialScore < 5)
			{
				if (rfStatus == 3)
				{
					parameterExclusionStatus = 1;
				}
			}
			//TBD
			if (pwDiff > ConstHigh::mergeDeltaMergePwLarge)
			{
				if (priInitialScore == 0)
				{
					if (toaDifference < 1200000 && (angleDiff < 20))
					{

					}
					else
					{
						parameterExclusionStatus = 1;
					}
				}
			}
			//if((pTrackSource->trackProbation == 1) && (emitterLibraryRecordSource->overlapPotentialPw == 1))
			//	parameterExclusionStatus = 1;
			//if((pTrackTarget->trackProbation == 1) && (emitterLibraryRecordTarget->overlapPotentialPw == 1))
			//	parameterExclusionStatus = 1;
		}
		if (rfStatus == 3)
		{
			parameterExclusionStatus = 1;
		}
		powerDiff = labs(
				(int16_t) trackOne.last[0].power.sumPxPyMax
						- (int16_t) trackTwo.last[0].power.sumPxPyMax);
		if (trackOne.last[0].rf.getBandCodeLegacy() == 4)
		{
			if (powerDiff > 400) // low value for cd band due to aoa not available
			{
				if (priInitialScore == 0)
				{
					priInitialScore = mergeCalcScoreStringPri(trackOne.last[0],
							trackTwo.last[0]);
				}
				if (priInitialScore < 10)
				{
					parameterExclusionStatus = 1;
				}
			}
		}
		else
		{
			if (powerDiff > ConstHigh::mergeDeltaMergePowerExcludePriCertain)
			{
				if (priInitialScore == 0)
				{
					priInitialScore = mergeCalcScoreStringPri(trackOne.last[0],
							trackTwo.last[0]);
				}
				if (priInitialScore < 10)
				{
					parameterExclusionStatus = 1;
				}
			}
		}
		if ((trackOne.last[0].pri.accumulatePriStaggerCount > 0)
				&& (trackOne.last[0].pri.accumulatePriDwellSwitchCount == 0)
				&& (trackTwo.last[0].pri.accumulatePriStaggerCount == 0)
				&& (trackTwo.last[0].pri.accumulatePriDwellSwitchCount > 0)
				&& ((pwDiff > ConstHigh::mergeDeltaMergePwSmall)
						|| (powerDiff > ConstHigh::mergeDeltaMergePowerSmall))
				&& (parameterExclusionStatus == 0))
		{
			if (priInitialScore == 0)
			{
				priInitialScore = mergeCalcScoreStringPri(trackOne.last[0],
						trackTwo.last[0]);
			}
			if (trackTwo.last[0].pri.accumulatePriDwellSwitchCount > 2)
			{
				parameterExclusionStatus = 1;
			}
			if ((priInitialScore == 0)
					&& (trackTwo.last[0].pri.priTypeFutureDominant
							!= PriTypeEnum::pri_stagger))
			{
				parameterExclusionStatus = 1;
			}
		}
		if ((trackTwo.last[0].pri.accumulatePriStaggerCount > 0)
				&& (trackTwo.last[0].pri.accumulatePriDwellSwitchCount == 0)
				&& (trackOne.last[0].pri.accumulatePriStaggerCount == 0)
				&& (trackOne.last[0].pri.accumulatePriDwellSwitchCount > 0)
				&& ((pwDiff > ConstHigh::mergeDeltaMergePwSmall)
						|| (powerDiff > ConstHigh::mergeDeltaMergePowerSmall))
				&& (parameterExclusionStatus == 0))
		{
			if (priInitialScore == 0)
			{
				priInitialScore = mergeCalcScoreStringPri(trackOne.last[0],
						trackTwo.last[0]);
			}
			if (trackOne.last[0].pri.accumulatePriDwellSwitchCount > 2)
			{
				parameterExclusionStatus = 1;
			}
			if ((priInitialScore == 0)
					&& (trackOne.last[0].pri.priTypeFutureDominant
							!= PriTypeEnum::pri_stagger))
			{
				parameterExclusionStatus = 1;
			}
		}
	}
	if ((parameterExclusionStatus == 0) || (parameterSpecialPermission == 1))
	{
		//which track to eliminate and which to keep?
		if (trackOne.history.stitchedUpdateNextCount
				> trackTwo.history.stitchedUpdateNextCount)
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		else if (trackTwo.history.stitchedUpdateNextCount
				> trackOne.history.stitchedUpdateNextCount)
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		else if (trackOne.last[0].history.lowToaStartUpdateCount
				> trackTwo.last[0].history.lowToaStartUpdateCount)
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		else if (trackTwo.last[0].history.lowToaStartUpdateCount
				> trackOne.last[0].history.lowToaStartUpdateCount)
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		else if (trackOne.history.stitchedUpdateCount
				> trackTwo.history.stitchedUpdateCount)
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		else if (trackTwo.history.stitchedUpdateCount
				> trackOne.history.stitchedUpdateCount)
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		//this is a new rule
		else if ((trackOne.libmatch.active.flagKnownLibrary == 1)
				&& (trackTwo.libmatch.active.flagKnownLibrary == 0))
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		//this is a new rule
		else if ((trackTwo.libmatch.active.flagKnownLibrary == 1)
				&& (trackOne.libmatch.active.flagKnownLibrary == 0))
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		//add a rule here about tracking emitters based on low and high processing
		else if ((trackOne.last[0].pri.accumulatePriStaggerCount > 0)
				&& (trackOne.last[0].pri.accumulatePriDwellSwitchCount == 0)
				&& (trackTwo.last[0].pri.accumulatePriStaggerCount == 0)
				&& (trackTwo.last[0].pri.accumulatePriDwellSwitchCount > 0))
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		else if ((trackTwo.last[0].pri.accumulatePriStaggerCount > 0)
				&& (trackTwo.last[0].pri.accumulatePriDwellSwitchCount == 0)
				&& (trackOne.last[0].pri.accumulatePriStaggerCount == 0)
				&& (trackOne.last[0].pri.accumulatePriDwellSwitchCount > 0))
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		else if (trackOne.general.timeIterCountLifetime
				< trackTwo.general.timeIterCountLifetime)
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		else if (trackTwo.general.timeIterCountLifetime
				< trackOne.general.timeIterCountLifetime)
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		else if (trackOne.general.toaFirst < trackTwo.general.toaFirst)
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		else
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		if (parameterExclusionStatus == 0)
		{
			if (ConstHigh::simLogIterationHigh == 1)
			{
				cout << "Merging Last Toas H: "
						<< high.tracks[mergeSourceIndex].general.toaLast
						<< " Q: "
						<< (int) high.tracks[mergeSourceIndex].power.quadrantCodeMax
						<< " AOALast: "
						<< high.tracks[mergeSourceIndex].last[0].angle.bearingLast
						<< " H: "
						<< high.tracks[mergeTargetIndex].general.toaLast
						<< " Q: "
						<< (int) high.tracks[mergeTargetIndex].power.quadrantCodeMax
						<< " AOALast: "
						<< high.tracks[mergeTargetIndex].last[0].angle.bearingLast
						<< endl;
			}
			//if overlap is less than a threshold. merge tracks, force updates and assign one lib match to the other, cfd and audio status of older track
			//how to merge?
			//add/check overlap and update counts on successful merge
			//take lowest start toa and merge. How to use probation flag?
			//maybe address t2 tracks here as well
			//what about stt flag?
			//deactivate one track and ignore its reports
			if ((high.tracks[mergeSourceIndex].libmatch.active.flagKnownLibrary
					== 0)
					&& (high.tracks[mergeTargetIndex].libmatch.active.flagKnownLibrary
							== 1))
			{
				pwDiff = mergeDifferencePw(
						high.tracks[mergeSourceIndex].last[0],
						high.tracks[mergeTargetIndex].last[0]);
				if ((pwDiff < ConstHigh::mergeDeltaMergePwSmall)
						&& (high.tracks[mergeTargetIndex].general.flagProbation
								== 0))
				{
					//These rules are problematic, preserve integrity of
					//..flagIterCurrentMatchLibrary
					high.tracks[mergeSourceIndex].general.priorityDynamic =
							high.tracks[mergeTargetIndex].general.priorityDynamic;
					high.tracks[mergeSourceIndex].libmatch.active =
							high.tracks[mergeTargetIndex].libmatch.active;

					//some variables are pending, sort them out
					/*pTrackKeep->LibMatchIndex = pTrackDestroy->LibMatchIndex;
					 pTrackKeep->LibMatchScore = pTrackDestroy->LibMatchScore;
					 pTrackKeep->PriMatchScore = pTrackDestroy->PriMatchScore;
					 pTrackKeep->CatNumber = pTrackDestroy->CatNumber;
					 pTrackKeep->SignatureCode = pTrackDestroy->SignatureCode;
					 pTrackKeep->ModeCode = pTrackDestroy->ModeCode;
					 pTrackKeep->OuterCode = pTrackDestroy->OuterCode;
					 pTrackKeep->Priority = pTrackDestroy->Priority;
					 pTrackKeep->LibErp = pTrackDestroy->LibErp;
					 pTrackKeep->LibNatureThreat = pTrackDestroy->LibNatureThreat;
					 pTrackKeep->LibDisplayThreat = pTrackDestroy->LibDisplayThreat;
					 pTrackKeep->LibMissileLaunch = pTrackDestroy->LibMissileLaunch;
					 pTrackKeep->LibTypeThreat = pTrackDestroy->LibTypeThreat;
					 pTrackKeep->LibUnknownCount = pTrackDestroy->LibUnknownCount;
					 pTrackKeep->ChaffIssunaceStatus = pTrackDestroy->ChaffIssunaceStatus;
					 pTrackKeep->ChaffLibraryIndex = pTrackDestroy->ChaffLibraryIndex;
					 pTrackKeep->FlareIssunaceStatus = pTrackDestroy->FlareIssunaceStatus;
					 pTrackKeep->FlareLibraryIndex = pTrackDestroy->FlareLibraryIndex;
					 pTrackKeep->CDCWCorr = pTrackDestroy->CDCWCorr;
					 pTrackKeep->AudioIndex = pTrackDestroy->AudioIndex;
					 pTrackKeep->AudioWaitIterations = pTrackDestroy->AudioWaitIterations;
					 pTrackKeep->AudioWaitIterationsLimit = pTrackDestroy->AudioWaitIterationsLimit;
					 pTrackKeep->effectiveRadiatedPower = pTrackDestroy->effectiveRadiatedPower;
					 */
				}
			}
			if (high.tracks[mergeTargetIndex].history.highTrackContinuationIterFlag
					> 0)
			{
				high.tracks[mergeSourceIndex].history.highTrackContinuationIterFlag =
						high.tracks[mergeTargetIndex].history.highTrackContinuationIterFlag;
			}
			//library updation rate merge logic
			if (high.tracks[mergeSourceIndex].libmatch.timeIterCountLibMatchExec
					< high.tracks[mergeTargetIndex].libmatch.timeIterCountLibMatchExec)
			{
				high.tracks[mergeSourceIndex].libmatch.timeIterCountLibMatchExec =
						high.tracks[mergeTargetIndex].libmatch.timeIterCountLibMatchExec;
			}
			//supress track merge logic
			if ((high.tracks[mergeSourceIndex].general.timeIterCountDemodSupprSet
					> high.tracks[mergeTargetIndex].general.timeIterCountDemodSupprSet)
					&& (high.tracks[mergeSourceIndex].general.flagDemod == 1)
					&& (high.tracks[mergeTargetIndex].general.flagDemod == 1))
			{
				high.tracks[mergeSourceIndex].general.timeIterCountDemodSupprSet =
						high.tracks[mergeTargetIndex].general.timeIterCountDemodSupprSet;
			}
			//angle and amplitude merge logic (needs review)
			if (high.tracks[mergeSourceIndex].general.timeIterCountLifetimeGlobal
					< high.tracks[mergeTargetIndex].general.timeIterCountLifetimeGlobal)
			{

				high.tracks[mergeSourceIndex].range.amplitudeCountDelta =
						high.tracks[mergeTargetIndex].range.amplitudeCountDelta;
				high.tracks[mergeSourceIndex].range.amplitudeCountMax =
						high.tracks[mergeTargetIndex].range.amplitudeCountMax;
				high.tracks[mergeSourceIndex].range.amplitudeStepDisplay =
						high.tracks[mergeTargetIndex].range.amplitudeStepDisplay;
				high.tracks[mergeSourceIndex].range.timeIterCountRangeCalc =
						high.tracks[mergeTargetIndex].range.timeIterCountRangeCalc;
				high.tracks[mergeSourceIndex].range.timeIterLimitRangeCalc =
						high.tracks[mergeTargetIndex].range.timeIterLimitRangeCalc;

				high.tracks[mergeSourceIndex].angle.bearingLast =
						high.tracks[mergeTargetIndex].angle.bearingLast;
				high.tracks[mergeSourceIndex].angle.bearingPreviousLast =
						high.tracks[mergeTargetIndex].angle.bearingPreviousLast;
				high.tracks[mergeSourceIndex].angle.bearingDisplay =
						high.tracks[mergeTargetIndex].angle.bearingDisplay;
				high.tracks[mergeSourceIndex].angle.bearingRemainderLast =
						high.tracks[mergeTargetIndex].angle.bearingRemainderLast;
				high.tracks[mergeSourceIndex].angle.timeIterCountBearingCalc =
						high.tracks[mergeTargetIndex].angle.timeIterCountBearingCalc;
				high.tracks[mergeSourceIndex].angle.timeIterLimitBearingCalc =
						high.tracks[mergeTargetIndex].angle.timeIterLimitBearingCalc;
				//the followig flag we are not using, hence it has not been copied either
				//pTrackKeep->TrackAgeAngleExpiry = pTrackDestroy->TrackAgeAngleExpiry;
			}
			//cfd merge logic, this is pending!
			/*if((pTrackDestroy->ChaffIssunaceStatus != NoChaffDefined) && (pTrackKeep->ChaffIssunaceStatus == NoChaffDefined))
			 {
			 pTrackKeep->ChaffIssunaceStatus = pTrackDestroy->ChaffIssunaceStatus;
			 pTrackKeep->ChaffLibraryIndex = pTrackDestroy->ChaffLibraryIndex;
			 }
			 else if((pTrackDestroy->ChaffIssunaceStatus != NoChaffDefined) && (pTrackKeep->ChaffIssunaceStatus != NoChaffDefined))
			 {
			 if(pTrackDestroy->ChaffIssunaceStatus == ChaffIssued)
			 {
			 pTrackKeep->ChaffIssunaceStatus = pTrackDestroy->ChaffIssunaceStatus;
			 pTrackKeep->ChaffLibraryIndex = pTrackDestroy->ChaffLibraryIndex;
			 }
			 }
			 if((pTrackDestroy->FlareIssunaceStatus != NoFlareDefined) && (pTrackKeep->FlareIssunaceStatus == NoFlareDefined))
			 {
			 pTrackKeep->FlareIssunaceStatus = pTrackDestroy->FlareIssunaceStatus;
			 pTrackKeep->FlareLibraryIndex = pTrackDestroy->FlareLibraryIndex;
			 }
			 else if((pTrackDestroy->FlareIssunaceStatus != NoFlareDefined) && (pTrackKeep->FlareIssunaceStatus != NoFlareDefined))
			 {
			 if(pTrackDestroy->FlareIssunaceStatus == FlareIssued)
			 {
			 pTrackKeep->FlareIssunaceStatus = pTrackDestroy->FlareIssunaceStatus;
			 pTrackKeep->FlareLibraryIndex = pTrackDestroy->FlareLibraryIndex;
			 }
			 }
			 */
			//audio merge logic
			if ((high.tracks[mergeSourceIndex].audio.flagNewGuyAudioPlayed == 1)
					|| (high.tracks[mergeTargetIndex].audio.flagNewGuyAudioPlayed
							== 1))
			{
				high.tracks[mergeSourceIndex].audio.flagNewGuyAudioPlayed = 1;
			}
			else
			{
				high.tracks[mergeSourceIndex].audio.flagNewGuyAudioPlayed = 0;
			}
			//stt merge logic
			if ((high.tracks[mergeSourceIndex].power.flagTrackingModeHigh == 0)
					&& (high.tracks[mergeTargetIndex].power.flagTrackingModeHigh
							== 1))
			{
				high.tracks[mergeSourceIndex].power.flagTrackingModeHigh =
						high.tracks[mergeTargetIndex].power.flagTrackingModeHigh;
				high.tracks[mergeSourceIndex].power.countTrackingModeIterHigh =
						high.tracks[mergeTargetIndex].power.countTrackingModeIterHigh;
			}
			if ((high.tracks[mergeSourceIndex].power.flagTrackingModeHigh == 0)
					&& (high.tracks[mergeTargetIndex].power.flagTrackingModeHigh
							== 0))
			{
				if ((high.tracks[mergeSourceIndex].history.flagUpdateOnceIterHigh
						== 0)
						&& (high.tracks[mergeTargetIndex].history.flagUpdateOnceIterHigh
								== 1))
				{
					high.tracks[mergeSourceIndex].history.flagUpdateOnceIterHigh =
							high.tracks[mergeTargetIndex].history.flagUpdateOnceIterHigh;
				}
				if (high.tracks[mergeSourceIndex].power.countTrackingModeIterHigh
						< high.tracks[mergeTargetIndex].power.countTrackingModeIterHigh)
				{
					high.tracks[mergeSourceIndex].power.countTrackingModeIterHigh =
							high.tracks[mergeTargetIndex].power.countTrackingModeIterHigh;
				}
				if (high.tracks[mergeSourceIndex].power.countTrackingModeLowIterHigh
						< high.tracks[mergeTargetIndex].power.countTrackingModeLowIterHigh)
				{
					high.tracks[mergeSourceIndex].power.countTrackingModeLowIterHigh =
							high.tracks[mergeTargetIndex].power.countTrackingModeLowIterHigh;
				}
			}
			//this clause is again tricky, needs to be handled carefully
			if (high.tracks[mergeSourceIndex].general.timeIterCountLifetimeGlobal
					< high.tracks[mergeTargetIndex].general.timeIterCountLifetimeGlobal)
			{
				high.tracks[mergeSourceIndex].general.timeIterCountLifetimeGlobal =
						high.tracks[mergeTargetIndex].general.timeIterCountLifetimeGlobal;
				high.tracks[mergeSourceIndex].general.timeIterCountLifetime =
						high.tracks[mergeTargetIndex].general.timeIterCountLifetime;
				high.tracks[mergeSourceIndex].history.highToaStartUpdateTotalCount =
						high.tracks[mergeTargetIndex].history.highToaStartUpdateTotalCount;
			}
			if ((high.tracks[mergeTargetIndex].general.flagProbation == 0)
					&& (high.tracks[mergeTargetIndex].general.timeIterCountLifetime
							< high.tracks[mergeSourceIndex].general.timeIterCountLifetime))
			{
				high.tracks[mergeSourceIndex].general.timeIterCountLifetime =
						high.tracks[mergeTargetIndex].general.timeIterCountLifetime;
			}
			//this rule seems problematic
			//if(high.tracks[mergeSourceIndex].general.toaFirst > high.tracks[mergeTargetIndex].general.toaFirst) {
			//	high.tracks[mergeSourceIndex].general.toaFirst = high.tracks[mergeTargetIndex].general.toaFirst;
			//}
			if ((high.tracks[mergeSourceIndex].general.flagProbation == 0)
					|| (high.tracks[mergeTargetIndex].general.flagProbation == 0))
			{
				high.tracks[mergeSourceIndex].general.flagProbation = 0;
				high.tracks[mergeSourceIndex].general.timeIterCountProbationSet =
						0;
			}

			// CFD related merging here
			high.tracks[mergeSourceIndex].cfd.flagCfdProgramFired = high.tracks[mergeTargetIndex].cfd.flagCfdProgramFired;

			//mark this track as inactive and bound to be deleted later
			high.tracks[mergeTargetIndex].general.flagDeletionHigh = 1;

			high.tracks[mergeSourceIndex].power.flagScanAnalysisRequested=1;
			IlluminationCycleInfo::takeUnion(high.tracks[mergeSourceIndex].power.illuminationInfo,
					high.tracks[mergeTargetIndex].power.illuminationInfo);

			return 1;
		}
	}
	return -1;
}
int16_t mergeTracksWithUncertainPri(DataSensorClass &low, DataSensorClass &high,
		TrackDefaultClass &trackOne, TrackDefaultClass &trackTwo,
		uint16_t indexTrackOne, uint16_t indexTrackTwo)
{

	//uint16_t reportSourceActualIndex = 0;
	//uint16_t reportTargetActualIndex = 0;
	uint16_t angleDiff = 0;
	uint16_t angleDiffQuadrantSwap = 0;
	uint8_t bandCompatible = 0;
	uint16_t powerDiff = 0;
	uint16_t pwDiff = 0;
	uint16_t rfStatus = 0;
	uint8_t parameterExclusionStatus = 0;
	uint8_t parameterSpecialPermission = 0;
	uint8_t libraryMatchStatus = 0;
	//uint8_t  priInitialScore = 0;
	//uint8_t  priDetailScore = 0;
	uint16_t mergeBearing = 0;
	uint16_t mergeBearingQuadrantSwap = 0;
	uint16_t mergeSourceIndex = 0;
	uint16_t mergeTargetIndex = 0;
	//uint32_t totalPulseOverlap = 0;
	//uint32_t totalToaOverlap = 0;
	//uint32_t totalPulseCount = 0;
	//uint32_t totalToaCount = 0;
	//float pulseOverlapPercent = 1;

	if (high.compareHighHigh(indexTrackOne, indexTrackTwo).parameterInitialStatus
			== 0)
	{
		high.compareHighHigh(indexTrackOne, indexTrackTwo).parameterInitialStatus =
				1;
		high.compareHighHigh(indexTrackTwo, indexTrackOne).parameterInitialStatus =
				1;
	}
	bandCompatible = 1;
	if (bandCompatible == 0)
	{
		return -1;
	}
	//merge angle change
	if (trackOne.rf.bandCode == BandCodeEnum::band_e)
	{
		mergeBearing = ConstHigh::mergeLimitMergeLowBandBearingPrimary;
		mergeBearingQuadrantSwap =
				ConstHigh::mergeLimitMergeLowBandBearingSecondary;
	}
	else if ((trackOne.rf.bandCode != BandCodeEnum::band_i)
			|| (trackOne.last[0].pri.accumulatePriStaggerCount > 0)
			|| (trackTwo.last[0].pri.accumulatePriStaggerCount > 0))
	{
		mergeBearing = ConstHigh::mergeLimitMergePriStaggerBearingPrimary;
		mergeBearingQuadrantSwap =
				ConstHigh::mergeLimitMergePriStaggerBearingSecondary;
	}
	else
	{
		mergeBearing = ConstHigh::mergeLimitMergeDefaultBearingPrimary;
		mergeBearingQuadrantSwap =
				ConstHigh::mergeLimitMergeDefaultBearingSecondary;
	}
	//end of merge angle change
	mergeCalcBearingDifferenceHighHigh(trackOne, trackTwo, angleDiff,
			angleDiffQuadrantSwap);
	if ((angleDiff > mergeBearing)
			&& (angleDiffQuadrantSwap > mergeBearingQuadrantSwap))
	{
		return -1;
	}
	if ((trackOne.history.highToaStartUpdateConsecutiveCount > 3)
			&& (trackTwo.history.highToaStartUpdateConsecutiveCount > 3))
	{
		parameterExclusionStatus = 1;
	}
	else if ((trackOne.history.highToaStartUpdateTotalCount > 10)
			&& (trackTwo.history.highToaStartUpdateTotalCount > 10))
	{
		parameterExclusionStatus = 1;
	}
	else if (((trackOne.history.stitchedUpdateNextCount > 0)
			|| (trackOne.history.stitchedUpdateNextDiffToaCount > 0))
			&& ((trackTwo.history.stitchedUpdateNextCount > 0)
					|| (trackTwo.history.stitchedUpdateNextDiffToaCount > 0))
			&& ((trackOne.history.highToaStartUpdateConsecutiveCount > 2)
					&& (trackTwo.history.highToaStartUpdateConsecutiveCount > 2)))
	{
		parameterExclusionStatus = 1;
	}

	//excluse based on tl category
//	if ((trackOne.libmatch.active.flagKnownLibrary == 1)
//			&& (trackTwo.libmatch.active.flagKnownLibrary == 1)
//			&& (trackOne.libmatch.active.highestMatchList[0].emitterBatchId
//					!= trackTwo.libmatch.active.highestMatchList[0].emitterBatchId)) {
//		parameterExclusionStatus = 1;
//	}
	if ((trackOne.libmatch.active.flagKnownLibrary == 1)
			&& (trackTwo.libmatch.active.flagKnownLibrary == 1))
	{
		if (trackOne.libmatch.active.highestMatchList[0].emitterBatchId
				!= trackTwo.libmatch.active.highestMatchList[0].emitterBatchId)
		{
			if ((trackOne.rf.getBandCodeLegacy() == 2)
					&& (trackTwo.last[0].rf.getBandCodeLegacy() == 2))
			{
				if (trackOne.libmatch.active.highestMatchList[0].dwellCount > 0
						&& trackTwo.libmatch.active.highestMatchList[0].dwellCount
								> 0)
				{
					if (trackOne.libmatch.active.highestMatchList[0].noOfSpotsMatched
							== trackOne.libmatch.active.highestMatchList[0].libraryPriCount
							|| trackOne.libmatch.active.highestMatchList[0].noOfSpotsMatched
									>= 3)
					{
						parameterExclusionStatus = 1;
					}
				}
				else
				{
					parameterExclusionStatus = 1;
				}
			}
			else
			{
				parameterExclusionStatus = 1;
			}
//			if (trackOne.libmatch.active.highestMatchList[0].emitterBatchId
//					!= trackTwo.libmatch.active.highestMatchList[0].emitterBatchId) {
//				parameterExclusionStatus = 1;
		}
	}
	if ((libraryMatchStatus == 0) && (parameterExclusionStatus == 0))
	{
		pwDiff = mergeDifferencePw(trackOne.last[0], trackTwo.last[0]);
		rfStatus = mergeCalcStatusStringRf(trackOne.last[0], trackTwo.last[0]);
		if ((pwDiff > ConstHigh::mergeDeltaStringPwLarge)
				&& ((trackOne.last[0].pri.accumulatePriJitterCount == 0)
						&& (trackOne.last[0].pri.accumulatePriComplexCount == 0)
						&& (trackTwo.last[0].pri.accumulatePriJitterCount == 0)
						&& (trackTwo.last[0].pri.accumulatePriComplexCount == 0)))
		{
			if (((trackOne.rf.bandCode == BandCodeEnum::band_e)
					|| (trackTwo.rf.bandCode == BandCodeEnum::band_e))
					|| (((trackOne.rf.bandCode == BandCodeEnum::band_g)
							|| (trackTwo.rf.bandCode == BandCodeEnum::band_g))
							&& ((trackOne.libmatch.active.flagKnownLibrary == 0)
									|| (trackTwo.libmatch.active.flagKnownLibrary
											== 0))))
			{
				powerDiff = labs(
						(int16_t) trackOne.last[0].power.sumPxPyMax
								- (int16_t) trackTwo.last[0].power.sumPxPyMax);
				//do nothing yet
			}
			else
			{
				powerDiff = labs(
						(int16_t) trackOne.last[0].power.sumPxPyMax
								- (int16_t) trackTwo.last[0].power.sumPxPyMax);
				if (powerDiff > ConstHigh::mergeDeltaMergePowerLarge)
				{
					parameterExclusionStatus = 1;
				}
				if (rfStatus != 1)
				{
					parameterExclusionStatus = 1;
				}
			}
		}
		if (rfStatus == 3)
		{
			parameterExclusionStatus = 1;
		}
		//exclude threat mergers based on tl rules for comparison between stt and search
		if ((trackOne.general.flagProbation == 1)
				&& (trackOne.libmatch.active.flagKnownLibrary == 1)
				&& (trackTwo.libmatch.active.flagKnownLibrary == 0))
		{
			if ((trackTwo.history.stitchedUpdateNextCount > 0)
					|| (trackTwo.history.stitchedUpdateNextDiffToaCount > 0))
			{
				parameterExclusionStatus = 1;
			}
		}
		if ((trackTwo.general.flagProbation == 1)
				&& (trackTwo.libmatch.active.flagKnownLibrary == 1)
				&& (trackOne.libmatch.active.flagKnownLibrary == 0))
		{
			if ((trackOne.history.stitchedUpdateNextCount > 0)
					|| (trackOne.history.stitchedUpdateNextDiffToaCount > 0))
			{
				parameterExclusionStatus = 1;
			}
		}
	}
	if ((parameterExclusionStatus == 0) || (parameterSpecialPermission == 1))
	{
		//which track to eliminate and which to keep?
		if (trackOne.history.stitchedUpdateNextCount
				> trackTwo.history.stitchedUpdateNextCount)
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		else if (trackTwo.history.stitchedUpdateNextCount
				> trackOne.history.stitchedUpdateNextCount)
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		else if (trackOne.history.stitchedUpdateNextCount
				> trackTwo.history.stitchedUpdateNextDiffToaCount)
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		else if (trackTwo.history.stitchedUpdateNextCount
				> trackOne.history.stitchedUpdateNextDiffToaCount)
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		else if ((trackOne.last[0].pri.flagPriAnalysisLimitedLast == 0)
				&& (trackTwo.last[0].pri.flagPriAnalysisLimitedLast == 1))
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		else if ((trackOne.last[0].pri.flagPriAnalysisLimitedLast == 1)
				&& (trackTwo.last[0].pri.flagPriAnalysisLimitedLast == 0))
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		else if (trackOne.last[0].history.lowToaStartUpdateCount
				> trackTwo.last[0].history.lowToaStartUpdateCount)
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		else if (trackTwo.last[0].history.lowToaStartUpdateCount
				> trackOne.last[0].history.lowToaStartUpdateCount)
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		else if (trackOne.history.stitchedUpdateCount
				> trackTwo.history.stitchedUpdateCount)
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		else if (trackTwo.history.stitchedUpdateCount
				> trackOne.history.stitchedUpdateCount)
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		else if (trackOne.history.stitchedUpdateDiffToaCount
				> trackTwo.history.stitchedUpdateDiffToaCount)
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		else if (trackTwo.history.stitchedUpdateDiffToaCount
				> trackOne.history.stitchedUpdateDiffToaCount)
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		//this is a new rule
		else if ((trackOne.libmatch.active.flagKnownLibrary == 1)
				&& (trackTwo.libmatch.active.flagKnownLibrary == 0))
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		//this is a new rule
		else if ((trackTwo.libmatch.active.flagKnownLibrary == 1)
				&& (trackOne.libmatch.active.flagKnownLibrary == 0))
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		//add a rule here about tracking emitters based on low and high processing
		else if ((trackOne.last[0].pri.accumulatePriStaggerCount > 0)
				&& (trackOne.last[0].pri.accumulatePriDwellSwitchCount == 0)
				&& (trackTwo.last[0].pri.accumulatePriStaggerCount == 0)
				&& (trackTwo.last[0].pri.accumulatePriDwellSwitchCount > 0))
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		else if ((trackTwo.last[0].pri.accumulatePriStaggerCount > 0)
				&& (trackTwo.last[0].pri.accumulatePriDwellSwitchCount == 0)
				&& (trackOne.last[0].pri.accumulatePriStaggerCount == 0)
				&& (trackOne.last[0].pri.accumulatePriDwellSwitchCount > 0))
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		else if (trackOne.general.timeIterCountLifetime
				< trackTwo.general.timeIterCountLifetime)
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		else if (trackTwo.general.timeIterCountLifetime
				< trackOne.general.timeIterCountLifetime)
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		else if (trackOne.general.toaFirst < trackTwo.general.toaFirst)
		{
			mergeSourceIndex = indexTrackOne;
			mergeTargetIndex = indexTrackTwo;
		}
		else
		{
			mergeSourceIndex = indexTrackTwo;
			mergeTargetIndex = indexTrackOne;
		}
		if (parameterExclusionStatus == 0)
		{
			if (ConstHigh::simLogIterationHigh == 1)
			{
				cout << "Merging Toa H: "
						<< high.tracks[mergeSourceIndex].general.toaFirst << "-"
						<< high.tracks[mergeSourceIndex].general.toaLast
						<< " Q: "
						<< (int) high.tracks[mergeSourceIndex].power.quadrantCodeMax
						<< " AOA: "
						<< high.tracks[mergeSourceIndex].angle.bearingLast
						<< " H: "
						<< high.tracks[mergeTargetIndex].general.toaFirst << "-"
						<< high.tracks[mergeTargetIndex].general.toaLast
						<< " Q: "
						<< (int) high.tracks[mergeTargetIndex].power.quadrantCodeMax
						<< " AOA: "
						<< high.tracks[mergeTargetIndex].angle.bearingLast
						<< endl;
			}
			//if overlap is less than a threshold. merge tracks, force updates and assign one lib match to the other, cfd and audio status of older track
			//how to merge?
			//add/check overlap and update counts on successful merge
			//take lowest start toa and merge. How to use probation flag?
			//maybe address t2 tracks here as well
			//what about stt flag?
			//deactivate one track and ignore its reports
			if ((high.tracks[mergeSourceIndex].libmatch.active.flagKnownLibrary
					== 0)
					&& (high.tracks[mergeTargetIndex].libmatch.active.flagKnownLibrary
							== 1))
			{
				pwDiff = mergeDifferencePw(
						high.tracks[mergeSourceIndex].last[0],
						high.tracks[mergeTargetIndex].last[0]);
				if ((pwDiff < ConstHigh::mergeDeltaMergePwSmall)
						&& (high.tracks[mergeTargetIndex].general.flagProbation
								== 0))
				{
					//These rules are problematic, preserve integrity of
					//..flagIterCurrentMatchLibrary
					high.tracks[mergeSourceIndex].general.priorityDynamic =
							high.tracks[mergeTargetIndex].general.priorityDynamic;
					high.tracks[mergeSourceIndex].libmatch.active =
							high.tracks[mergeTargetIndex].libmatch.active;
					//some variables are pending, sort them out
					/*pTrackKeep->LibMatchIndex = pTrackDestroy->LibMatchIndex;
					 pTrackKeep->LibMatchScore = pTrackDestroy->LibMatchScore;
					 pTrackKeep->PriMatchScore = pTrackDestroy->PriMatchScore;
					 pTrackKeep->CatNumber = pTrackDestroy->CatNumber;
					 pTrackKeep->SignatureCode = pTrackDestroy->SignatureCode;
					 pTrackKeep->ModeCode = pTrackDestroy->ModeCode;
					 pTrackKeep->OuterCode = pTrackDestroy->OuterCode;
					 pTrackKeep->Priority = pTrackDestroy->Priority;
					 pTrackKeep->LibErp = pTrackDestroy->LibErp;
					 pTrackKeep->LibNatureThreat = pTrackDestroy->LibNatureThreat;
					 pTrackKeep->LibDisplayThreat = pTrackDestroy->LibDisplayThreat;
					 pTrackKeep->LibMissileLaunch = pTrackDestroy->LibMissileLaunch;
					 pTrackKeep->LibTypeThreat = pTrackDestroy->LibTypeThreat;
					 pTrackKeep->LibUnknownCount = pTrackDestroy->LibUnknownCount;
					 pTrackKeep->ChaffIssunaceStatus = pTrackDestroy->ChaffIssunaceStatus;
					 pTrackKeep->ChaffLibraryIndex = pTrackDestroy->ChaffLibraryIndex;
					 pTrackKeep->FlareIssunaceStatus = pTrackDestroy->FlareIssunaceStatus;
					 pTrackKeep->FlareLibraryIndex = pTrackDestroy->FlareLibraryIndex;
					 pTrackKeep->CDCWCorr = pTrackDestroy->CDCWCorr;
					 pTrackKeep->AudioIndex = pTrackDestroy->AudioIndex;
					 pTrackKeep->AudioWaitIterations = pTrackDestroy->AudioWaitIterations;
					 pTrackKeep->AudioWaitIterationsLimit = pTrackDestroy->AudioWaitIterationsLimit;
					 pTrackKeep->effectiveRadiatedPower = pTrackDestroy->effectiveRadiatedPower;
					 */
				}
			}
			if (high.tracks[mergeTargetIndex].history.highTrackContinuationIterFlag
					> 0)
			{
				high.tracks[mergeSourceIndex].history.highTrackContinuationIterFlag =
						high.tracks[mergeTargetIndex].history.highTrackContinuationIterFlag;
			}
			//library updation rate merge logic
			if (high.tracks[mergeSourceIndex].libmatch.timeIterCountLibMatchExec
					< high.tracks[mergeTargetIndex].libmatch.timeIterCountLibMatchExec)
			{
				high.tracks[mergeSourceIndex].libmatch.timeIterCountLibMatchExec =
						high.tracks[mergeTargetIndex].libmatch.timeIterCountLibMatchExec;
			}
			//supress track merge logic
			if ((high.tracks[mergeSourceIndex].general.timeIterCountDemodSupprSet
					> high.tracks[mergeTargetIndex].general.timeIterCountDemodSupprSet)
					&& (high.tracks[mergeSourceIndex].general.flagDemod == 1)
					&& (high.tracks[mergeTargetIndex].general.flagDemod == 1))
			{
				high.tracks[mergeSourceIndex].general.timeIterCountDemodSupprSet =
						high.tracks[mergeTargetIndex].general.timeIterCountDemodSupprSet;
			}
			//angle and amplitude merge logic (needs review)
			if (high.tracks[mergeSourceIndex].general.timeIterCountLifetimeGlobal
					< high.tracks[mergeTargetIndex].general.timeIterCountLifetimeGlobal)
			{

				high.tracks[mergeSourceIndex].range.amplitudeCountDelta =
						high.tracks[mergeTargetIndex].range.amplitudeCountDelta;
				high.tracks[mergeSourceIndex].range.amplitudeCountMax =
						high.tracks[mergeTargetIndex].range.amplitudeCountMax;
				high.tracks[mergeSourceIndex].range.amplitudeStepDisplay =
						high.tracks[mergeTargetIndex].range.amplitudeStepDisplay;
				high.tracks[mergeSourceIndex].range.timeIterCountRangeCalc =
						high.tracks[mergeTargetIndex].range.timeIterCountRangeCalc;
				high.tracks[mergeSourceIndex].range.timeIterLimitRangeCalc =
						high.tracks[mergeTargetIndex].range.timeIterLimitRangeCalc;

				high.tracks[mergeSourceIndex].angle.bearingLast =
						high.tracks[mergeTargetIndex].angle.bearingLast;
				high.tracks[mergeSourceIndex].angle.bearingPreviousLast =
						high.tracks[mergeTargetIndex].angle.bearingPreviousLast;
				high.tracks[mergeSourceIndex].angle.bearingDisplay =
						high.tracks[mergeTargetIndex].angle.bearingDisplay;
				high.tracks[mergeSourceIndex].angle.bearingRemainderLast =
						high.tracks[mergeTargetIndex].angle.bearingRemainderLast;
				high.tracks[mergeSourceIndex].angle.timeIterCountBearingCalc =
						high.tracks[mergeTargetIndex].angle.timeIterCountBearingCalc;
				high.tracks[mergeSourceIndex].angle.timeIterLimitBearingCalc =
						high.tracks[mergeTargetIndex].angle.timeIterLimitBearingCalc;
				//the followig flag we are not using, hence it has not been copied either
				//pTrackKeep->TrackAgeAngleExpiry = pTrackDestroy->TrackAgeAngleExpiry;
			}
			//cfd merge logic, this is pending!
			/*if((pTrackDestroy->ChaffIssunaceStatus != NoChaffDefined) && (pTrackKeep->ChaffIssunaceStatus == NoChaffDefined))
			 {
			 pTrackKeep->ChaffIssunaceStatus = pTrackDestroy->ChaffIssunaceStatus;
			 pTrackKeep->ChaffLibraryIndex = pTrackDestroy->ChaffLibraryIndex;
			 }
			 else if((pTrackDestroy->ChaffIssunaceStatus != NoChaffDefined) && (pTrackKeep->ChaffIssunaceStatus != NoChaffDefined))
			 {
			 if(pTrackDestroy->ChaffIssunaceStatus == ChaffIssued)
			 {
			 pTrackKeep->ChaffIssunaceStatus = pTrackDestroy->ChaffIssunaceStatus;
			 pTrackKeep->ChaffLibraryIndex = pTrackDestroy->ChaffLibraryIndex;
			 }
			 }
			 if((pTrackDestroy->FlareIssunaceStatus != NoFlareDefined) && (pTrackKeep->FlareIssunaceStatus == NoFlareDefined))
			 {
			 pTrackKeep->FlareIssunaceStatus = pTrackDestroy->FlareIssunaceStatus;
			 pTrackKeep->FlareLibraryIndex = pTrackDestroy->FlareLibraryIndex;
			 }
			 else if((pTrackDestroy->FlareIssunaceStatus != NoFlareDefined) && (pTrackKeep->FlareIssunaceStatus != NoFlareDefined))
			 {
			 if(pTrackDestroy->FlareIssunaceStatus == FlareIssued)
			 {
			 pTrackKeep->FlareIssunaceStatus = pTrackDestroy->FlareIssunaceStatus;
			 pTrackKeep->FlareLibraryIndex = pTrackDestroy->FlareLibraryIndex;
			 }
			 }
			 */
			//audio merge logic
			if ((high.tracks[mergeSourceIndex].audio.flagNewGuyAudioPlayed == 1)
					|| (high.tracks[mergeTargetIndex].audio.flagNewGuyAudioPlayed
							== 1))
			{
				high.tracks[mergeSourceIndex].audio.flagNewGuyAudioPlayed = 1;
			}
			else
			{
				high.tracks[mergeSourceIndex].audio.flagNewGuyAudioPlayed = 0;
			}
			//stt merge logic
			if ((high.tracks[mergeSourceIndex].power.flagTrackingModeHigh == 0)
					&& (high.tracks[mergeTargetIndex].power.flagTrackingModeHigh
							== 1))
			{
				high.tracks[mergeSourceIndex].power.flagTrackingModeHigh =
						high.tracks[mergeTargetIndex].power.flagTrackingModeHigh;
				high.tracks[mergeSourceIndex].power.countTrackingModeIterHigh =
						high.tracks[mergeTargetIndex].power.countTrackingModeIterHigh;
			}
			if ((high.tracks[mergeSourceIndex].power.flagTrackingModeHigh == 0)
					&& (high.tracks[mergeTargetIndex].power.flagTrackingModeHigh
							== 0))
			{
				if ((high.tracks[mergeSourceIndex].history.flagUpdateOnceIterHigh
						== 0)
						&& (high.tracks[mergeTargetIndex].history.flagUpdateOnceIterHigh
								== 1))
				{
					high.tracks[mergeSourceIndex].history.flagUpdateOnceIterHigh =
							high.tracks[mergeTargetIndex].history.flagUpdateOnceIterHigh;
				}
				if (high.tracks[mergeSourceIndex].power.countTrackingModeIterHigh
						< high.tracks[mergeTargetIndex].power.countTrackingModeIterHigh)
				{
					high.tracks[mergeSourceIndex].power.countTrackingModeIterHigh =
							high.tracks[mergeTargetIndex].power.countTrackingModeIterHigh;
				}
				if (high.tracks[mergeSourceIndex].power.countTrackingModeLowIterHigh
						< high.tracks[mergeTargetIndex].power.countTrackingModeLowIterHigh)
				{
					high.tracks[mergeSourceIndex].power.countTrackingModeLowIterHigh =
							high.tracks[mergeTargetIndex].power.countTrackingModeLowIterHigh;
				}
			}
			//this clause is again tricky, needs to be handled carefully
			if (high.tracks[mergeSourceIndex].general.timeIterCountLifetimeGlobal
					< high.tracks[mergeTargetIndex].general.timeIterCountLifetimeGlobal)
			{
				high.tracks[mergeSourceIndex].general.timeIterCountLifetimeGlobal =
						high.tracks[mergeTargetIndex].general.timeIterCountLifetimeGlobal;
				high.tracks[mergeSourceIndex].general.timeIterCountLifetime =
						high.tracks[mergeTargetIndex].general.timeIterCountLifetime;
				high.tracks[mergeSourceIndex].history.highToaStartUpdateTotalCount =
						high.tracks[mergeTargetIndex].history.highToaStartUpdateTotalCount;
			}
			if ((high.tracks[mergeTargetIndex].general.flagProbation == 0)
					&& (high.tracks[mergeTargetIndex].general.timeIterCountLifetime
							< high.tracks[mergeSourceIndex].general.timeIterCountLifetime))
			{
				high.tracks[mergeSourceIndex].general.timeIterCountLifetime =
						high.tracks[mergeTargetIndex].general.timeIterCountLifetime;
			}
			//this rule seems problematic
			//if(high.tracks[mergeSourceIndex].general.toaFirst > high.tracks[mergeTargetIndex].general.toaFirst) {
			//	high.tracks[mergeSourceIndex].general.toaFirst = high.tracks[mergeTargetIndex].general.toaFirst;
			//}
			if ((high.tracks[mergeSourceIndex].general.flagProbation == 0)
					|| (high.tracks[mergeTargetIndex].general.flagProbation == 0))
			{
				high.tracks[mergeSourceIndex].general.flagProbation = 0;
				high.tracks[mergeSourceIndex].general.timeIterCountProbationSet =
						0;
			}

			// CFD related merging here
			high.tracks[mergeSourceIndex].cfd.flagCfdProgramFired = high.tracks[mergeTargetIndex].cfd.flagCfdProgramFired;

			//mark this track as inactive and bound to be deleted later
			high.tracks[mergeTargetIndex].general.flagDeletionHigh = 1;
			return 1;
		}
	}
	return -1;
}

void mergeInitializeScoreStructureLowLow(DataSensorClass &low)
{

	low.compareLowLow.resize(0, 0, false);			//can remove later
	low.compareLowLow.resize(low.tracks.size(), low.tracks.size(), false);
}

void mergeInitializeScoreStructureHighLow(DataSensorClass &low,
		DataSensorClass &high)
{

	high.compareHighLow.resize(0, 0, false);			//can remove later
	high.compareHighLow.resize(low.tracks.size(),
			ConstHigh::trackLimitMaxEmittersHigh, false);
}

void mergeInitializeScoreStructureHighHigh(DataSensorClass &high)
{

	high.compareHighHigh.resize(0, 0, false);			//can remove later
	high.compareHighHigh.resize(high.tracks.size(), high.tracks.size(), false);
}

void mergeCompareInitialAllLowLow(DataSensorClass &low)
{
	uint8_t i = 0;
	uint8_t j = 0;
	uint16_t angleDiff = 360;
	uint16_t angleDiffQuadrantSwap = 360;
	//uint8_t stitchAngleLimit = 25;
	//uint8_t stitchAngleLimitAlternative = 15;
	uint16_t pwDiff;
	uint8_t overlapPriTypeFlag = 0;
	uint8_t overlapPriTypeCount = 0;
	uint8_t overlapPotential = 0;
	uint8_t overlapPulseWidthCount = 0;
	uint8_t overlapTrackCount = 0;
	uint32_t currentPulseOverlap = 0;
	uint64_t currentToaOverlap = 0;
	uint32_t maxPulseOverlap = 0;
	uint64_t maxToaOverlap = 0;

	for (i = 0; i < low.tracks.size(); i++)
	{
		maxPulseOverlap = 0;
		maxToaOverlap = 0;
		overlapPotential = 0;
		overlapPulseWidthCount = 0;
		overlapPriTypeCount = 0;
		overlapTrackCount = 0;
		for (j = 0; j < low.tracks.size(); j++)
		{
			//for debugging purposes
			if ((low.tracks[i].general.toaLast == 202899746120)
					&& (low.tracks[j].general.toaLast == 202897683320))
			{
				low.tracks[i].general.toaLast = 202899746120;
			}
			if ((low.tracks[j].general.toaLast == 202899746120)
					&& (low.tracks[i].general.toaLast == 202897683320))
			{
				low.tracks[j].general.toaLast = 202899746120;
			}
			//end of debugging
			//disable this block if required
			/*if(low.tracks[j].general.processingHighFlag == 1)
			 {
			 low.compareLowLow(i,j).parameterInitialStatus = 0;
			 low.compareLowLow(i,j).parameterCompareStatus = 0;
			 low.compareLowLow(i,j).overallScore = 0;
			 continue;
			 }*/
			//end of optional disable of block
			low.compareLowLow(i, j).parameterInitialStatus = 1;
			low.compareLowLow(i, j).parameterCompareStatus = 0;
			low.compareLowLow(i, j).overallScore = 0;
			if (i == j)
			{
				continue;
			}
			if (low.tracks[i].rf.bandCode != low.tracks[j].rf.bandCode)
			{
				low.compareLowLow(i, j).bandCompatible = 0;
				low.compareLowLow(i, j).stitchCompatible = 0;
				low.compareLowLow(i, j).mergeCompatible = 0;
				continue;
			}
			low.compareLowLow(i, j).bandCompatible = 1;
			mergeCalcBearingDifferenceLowLow(low.tracks[i], low.tracks[j],
					angleDiff, angleDiffQuadrantSwap);
			low.compareLowLow(i, j).angleDiff = angleDiff;
			low.compareLowLow(i, j).angleDiffQuadrantSwap =
					angleDiffQuadrantSwap;

			if ((angleDiff < ConstHigh::mergeLimitStringBearingPrimaryLarge)
					|| (angleDiffQuadrantSwap
							< ConstHigh::mergeLimitStringBearingSecondaryLarge))
			{

				overlapPotential = 1;
				overlapTrackCount++;
				low.compareLowLow(i, j).angleCompatible = 1;
				low.compareLowLow(i, j).mergeCompatible = 1;

				//if((low.tracks[i].pw.accumulatePw.size() > 0) &&
				//		(low.tracks[j].pw.accumulatePw.size() > 0)) {
				//	if((low.tracks[i].rf.bandCode == BandCodeEnum::band_i) ||
				//			(low.tracks[i].rf.bandCode ==
				//					BandCodeEnum::band_ij)) {
				//		pwDiff =
				//				labs((int32_t)low.tracks[i].pw.accumulatePw[0].pwLow -
				//						(int32_t)low.tracks[j].pw.accumulatePw[0].pwLow);
				//		if(pwDiff < ConstHigh::mergeDeltaStringPwSmall)
				//			overlapPulseWidthCount = overlapPulseWidthCount + 1;
				//	}
				//}
				pwDiff = mergeDifferencePw(low.tracks[i], low.tracks[j]);
				if (pwDiff < ConstHigh::mergeDeltaStringPwMedium)
				{
					overlapPulseWidthCount = overlapPulseWidthCount + 1;
				}
				overlapPriTypeFlag = mergeDifferencePriType(low.tracks[i],
						low.tracks[j]);
				if (overlapPriTypeFlag > 0)
				{
					overlapPriTypeCount = overlapPriTypeCount + 1;
				}
			}
			else
			{
				low.compareLowLow(i, j).angleCompatible = 0;
				low.compareLowLow(i, j).stitchCompatible = 0;
				low.compareLowLow(i, j).mergeCompatible = 0;
				continue;
			}
			if (low.tracks[i].general.toaFirst > low.tracks[j].general.toaLast)
			{
				low.compareLowLow(i, j).stitchCompatible = 1;
				low.compareLowLow(i, j).mergeCompatible = 1;
				low.compareLowLow(i, j).toaDiffFirstLast =
						low.tracks[i].general.toaFirst
								- low.tracks[j].general.toaLast;
				low.compareLowLow(i, j).pulseDiffOverlap = 0;
				low.compareLowLow(i, j).toaDiffOverlap = 0;
			}
			else if (low.tracks[j].general.toaFirst
					> low.tracks[i].general.toaLast)
			{
				low.compareLowLow(i, j).stitchCompatible = 0;
				low.compareLowLow(i, j).mergeCompatible = 1;
				low.compareLowLow(i, j).toaDiffFirstLast = 0;
				low.compareLowLow(i, j).pulseDiffOverlap = 0;
				low.compareLowLow(i, j).toaDiffOverlap = 0;
				continue;
			}
			else
			{
				low.compareLowLow(i, j).stitchCompatible = 0;
				low.compareLowLow(i, j).mergeCompatible = 1;
				low.compareLowLow(i, j).toaDiffFirstLast = 0;
				//overlap analysis
				mergeCalcOverlapLowLow(low.tracks[i], low.tracks[j],
						currentPulseOverlap, currentToaOverlap);
				low.compareLowLow(i, j).pulseDiffOverlap = currentPulseOverlap;
				low.compareLowLow(i, j).toaDiffOverlap = currentToaOverlap;
				if (currentPulseOverlap > maxPulseOverlap)
					maxPulseOverlap = currentPulseOverlap;
				if (currentToaOverlap > maxToaOverlap)
					maxToaOverlap = currentToaOverlap;
				/*if((pActiveTagsList->ActiveTags[index].pulseWidth > 0) && (pActiveTagsList->ActiveTags[innerIndex].pulseWidth > 0))
				 {
				 if((pActiveTagsList->ActiveTags[index].Band == 2) || (pActiveTagsList->ActiveTags[index].Band == 5))
				 {
				 pwDiff = (UWord16)ABSOLUTE_VAL(pActiveTagsList->ActiveTags[index].pulseWidth - pActiveTagsList->ActiveTags[innerIndex].pulseWidth);
				 if(pwDiff < 6)
				 overlapPulseWidthCount = overlapPulseWidthCount + 1;
				 }
				 }*/
				continue;
			}
			//scoring is not currently done here to save processor cycles. We will compare reports on the go later when required
		}
		low.tracks[i].history.countPulseOverlapMaxLow = maxPulseOverlap;
		low.tracks[i].history.durationToaOverlapMaxLow = maxToaOverlap;
		low.tracks[i].history.countPulseWidthOverlapMaxLow =
				overlapPulseWidthCount;
		low.tracks[i].history.countPriTypeOverlapMaxLow = overlapPriTypeCount;
		low.tracks[i].history.countTrackOverlapMaxLow = overlapTrackCount;
		low.tracks[i].history.flagOverlapPotential = overlapPotential;
	}
}

void mergeCompareInitialAllHighLow(DataSensorClass &low, DataSensorClass &high)
{

	uint16_t i = 0;
	uint16_t j = 0;
	uint16_t angleDiff = 0;
	uint16_t angleDiffQuadrantSwap = 0;
	uint16_t pwDiff = 0;
	uint8_t overlapPriTypeFlag = 0;
	uint8_t overlapPriTypeCount = 0;
	uint8_t overlapPulseWidthCount = 0;
	uint8_t overlapPotential = 0;
	uint8_t overlapTrackCount = 0;
	uint32_t currentPulseOverlap = 0;
	uint64_t currentToaOverlap = 0;
	uint32_t maxPulseOverlap = 0;
	uint64_t maxToaOverlap = 0;

	for (i = 0; i < low.tracks.size(); i++)
	{
		//if this low track does not overlap with any other low track in this iteration

		//for debugging purposes
		//if(low.tracks[i].general.toaLast == 162076970510) {
		//	low.tracks[i].general.toaLast = 162076970510;
		//}
		//end of debugging

		if (low.tracks[i].history.flagOverlapPotential == 0)
		{
			maxPulseOverlap = 0;
			maxToaOverlap = 0;
			overlapPotential = 0;
			overlapPulseWidthCount = 0;
			overlapPriTypeCount = 0;
			overlapTrackCount = 0;
			for (j = 0; j < high.tracks.size(); j++)
			{
				mergeCalcBearingDifferenceHighLow(low.tracks[i], high.tracks[j],
						angleDiff, angleDiffQuadrantSwap);
				//this condition has been replaced with standard angle comparison
				//predicate used for lowlow, highlow and highhigh
				//if(((angleDiff < 30) &&
				//		(low.tracks[i].rf.getBandCodeLegacy() == 2)) ||
				//		((angleDiff < 40) &&
				//				(low.tracks[i].rf.getBandCodeLegacy() != 2))) {
				if ((angleDiff < ConstHigh::mergeLimitStringBearingPrimary)
						|| (angleDiffQuadrantSwap
								< ConstHigh::mergeLimitStringBearingSecondary))
				{
					overlapPotential = 1;
					overlapTrackCount++;
					//do we need to compute this matrix here?
					//low.compareHighLow(i,j).angleCompatible = 1;
					//low.compareHighLow(i,j).mergeCompatible = 1;
					mergeCalcOverlapHighLow(low.tracks[i], high.tracks[j],
							currentPulseOverlap, currentToaOverlap);
					pwDiff = mergeDifferencePw(low.tracks[i], high.tracks[j]);
					if (pwDiff < ConstHigh::mergeDeltaStringPwMedium)
					{
						overlapPulseWidthCount = overlapPulseWidthCount + 1;
					}
					overlapPriTypeFlag = mergeDifferencePriType(low.tracks[i],
							high.tracks[j]);
					if (overlapPriTypeFlag > 0)
					{
						overlapPriTypeCount = overlapPriTypeCount + 1;
					}
					if (currentPulseOverlap > maxPulseOverlap)
					{
						maxPulseOverlap = currentPulseOverlap;
					}
					if (currentToaOverlap > maxToaOverlap)
					{
						maxToaOverlap = currentToaOverlap;
					}
				}
			}
			low.tracks[i].history.countPulseOverlapMaxLow = maxPulseOverlap;
			low.tracks[i].history.durationToaOverlapMaxLow = maxToaOverlap;
			low.tracks[i].history.countPulseWidthOverlapMaxLow =
					overlapPulseWidthCount;
			low.tracks[i].history.countPriTypeOverlapMaxLow =
					overlapPriTypeCount;
			low.tracks[i].history.countTrackOverlapMaxLow = overlapTrackCount;
			low.tracks[i].history.flagOverlapPotential = overlapPotential;
		}
	}
}

void mergeCompareInitialAllHighHigh(DataSensorClass &high)
{
	uint16_t i, j;
	for (i = 0; i < high.tracks.size(); i++)
	{
		for (j = 0; j < high.tracks.size(); j++)
		{
			if (i == j)
			{
				continue;
			}

			if (high.tracks[i].last[0].general.toaFirst
					> high.tracks[j].last[0].general.toaLast)
			{
				high.compareHighHigh(i, j).toaDiffFirstLast =
						high.tracks[i].last[0].general.toaFirst
								- high.tracks[i].last[0].general.toaLast;
			}
			else
			{
				high.compareHighHigh(i, j).toaDiffFirstLast = 2100000;
			}
		}
	}

}
uint8_t mergeCompareFinalizeHighHigh(TrackDefaultClass &track,
		DataSensorClass &high)
{
	uint16_t i = 0;
	uint16_t angleDiff = 0;
	uint16_t angleDiffQuadrantSwap = 0;
	uint8_t rfStatus = 0;
	for (i = 0; i < high.tracks.size(); i++)
	{
		if ((track.general.flagProbation == 1)
				&& (high.tracks[i].general.flagProbation == 0)
				&& (track.general.flagDeletionHigh == 0)
				&& (high.tracks[i].general.flagDeletionHigh == 0))
		{
			mergeCalcBearingDifferenceLowLow(track.last[0],
					high.tracks[i].last[0], angleDiff, angleDiffQuadrantSwap);
			if ((angleDiff < ConstHigh::mergeLimitStringBearingPrimary)
					|| (angleDiffQuadrantSwap
							< ConstHigh::mergeLimitStringBearingSecondary))
			{
				rfStatus = mergeCalcStatusStringRf(track.last[0],high.tracks[i].last[0]);
				if(rfStatus !=3)
					return 1;
			}
		}
	}
	return 0;
}
uint8_t mergeComparePriHighHigh(TrackDefaultClass &newTrack,
		DataSensorClass &high)
{
	uint16_t i = 0;
	uint16_t j = 0;

	for (i = 0; i < high.tracks.size(); i++)
	{
		TrackDefaultClass &comparisonTarget = high.tracks[i];
		if ((newTrack.general.flagProbation == 1)
				&& (comparisonTarget.general.flagProbation == 0)
				&& (newTrack.general.flagDeletionHigh == 0)
				&& (comparisonTarget.general.flagDeletionHigh == 0))
		{
			for (j = 0; j < newTrack.pri.accumulatePri.size(); j++)
			{
				if ((newTrack.pri.accumulatePri[j].priType
						== PriTypeEnum::pri_fixed
						|| newTrack.pri.accumulatePri[j].priType
								== PriTypeEnum::pri_dwellswitch
						|| newTrack.pri.accumulatePri[j].priType
								== PriTypeEnum::pri_stagger)
						&& comparisonTarget.last[0].pri.priTypeCurrentDominant
								== PriTypeEnum::pri_dwellswitch)
				{
					for (uint16_t k = 0;
							k < comparisonTarget.last[0].pri.priCircular.size();
							k++)
					{
						if (findIfNthMultiple(
								newTrack.last[0].pri.accumulatePri[j].priMean,
								comparisonTarget.last[0].pri.priCircular[k].priMean,
								(int16_t) 16))
						{

							return 1;
						}
					}
				}
			}
		}
	}
	return 0;
}
bool findIfNthMultiple(uint32_t queryValue, uint32_t spotValue,
		int16_t devAllowed)
{
	devAllowed = max(3, 2 * devAllowed);
	if (queryValue < 0 || spotValue < 1)
	{
		return false;
	}
	int32_t remainderValue = queryValue % spotValue;
	int32_t nthMultiple = queryValue / spotValue;
	if (nthMultiple > 10)
	{
		return false;
	}
	else if (remainderValue <= (devAllowed))
	{
		return true;
	}
	else if (labs((int32_t) remainderValue - (int32_t) spotValue)
			<= (devAllowed))
	{
		return true;
	}
	return false;

}
uint8_t mergeFindUnknownsWithinAngleWindow(TrackDefaultClass &track,
		DataSensorClass &high)
{
	uint16_t i = 0;
	uint16_t angleDiff = 0;
	uint16_t angleDiffQuadrantSwap = 0;
	uint8_t unknownsInAngleWindow = 0;
	for (i = 0; i < high.tracks.size(); i++)
	{
		if (high.tracks[i].libmatch.active.flagKnownLibrary == 1)
		{
			continue;
		}
		else if ((track.general.flagProbation == 1)
				&& (high.tracks[i].general.flagProbation == 0)
				&& (track.general.flagDeletionHigh == 0)
				&& (high.tracks[i].general.flagDeletionHigh == 0))
		{
			mergeCalcBearingDifferenceLowLow(track.last[0],
					high.tracks[i].last[0], angleDiff, angleDiffQuadrantSwap);
			if ((angleDiff < ConstHigh::mergeLimitStringBearingPrimary)
					|| (angleDiffQuadrantSwap
							< ConstHigh::mergeLimitStringBearingSecondary))
			{
				unknownsInAngleWindow++;
			}
		}
	}
	if (unknownsInAngleWindow > 1)
		return 1;
	else
		return 0;
}
void mergeCompareInitialHighLow(DataSensorClass &low, DataSensorClass &high,
		uint16_t lowIndex, uint16_t highIndex)
{
	uint16_t angleDiff;
	uint16_t angleDiffQuadrantSwap;

	high.compareHighLow(lowIndex, highIndex).parameterInitialStatus = 1;
	if (low.tracks[lowIndex].rf.bandCode
			== high.tracks[highIndex].last[0].rf.bandCode)
	{
		high.compareHighLow(lowIndex, highIndex).bandCompatible = 1;
	}
	else
	{
		high.compareHighLow(lowIndex, highIndex).bandCompatible = 0;
		high.compareHighLow(lowIndex, highIndex).stitchCompatible = 0;
		high.compareHighLow(lowIndex, highIndex).angleCompatible = 0;
		high.compareHighLow(lowIndex, highIndex).mergeCompatible = 0;
		return;
	}
	mergeCalcBearingDifferenceHighLow(low.tracks[lowIndex],
			high.tracks[highIndex], angleDiff, angleDiffQuadrantSwap);
	high.compareHighLow(lowIndex, highIndex).angleDiff = angleDiff;
	high.compareHighLow(lowIndex, highIndex).angleDiffQuadrantSwap =
			angleDiffQuadrantSwap;
	if ((angleDiff < ConstHigh::mergeLimitStringBearingPrimaryLarge)
			|| (angleDiffQuadrantSwap
					< ConstHigh::mergeLimitStringBearingSecondaryLarge))
	{
		high.compareHighLow(lowIndex, highIndex).stitchCompatible = 0;
		high.compareHighLow(lowIndex, highIndex).angleCompatible = 1;
		high.compareHighLow(lowIndex, highIndex).mergeCompatible = 1;
		high.compareHighLow(lowIndex, highIndex).angleDiff = angleDiff;
		high.compareHighLow(lowIndex, highIndex).angleDiffQuadrantSwap =
				angleDiffQuadrantSwap;
	}
	else
	{
		high.compareHighLow(lowIndex, highIndex).stitchCompatible = 0;
		high.compareHighLow(lowIndex, highIndex).angleCompatible = 0;
		high.compareHighLow(lowIndex, highIndex).mergeCompatible = 0;
		return;
	}
	if (low.tracks[lowIndex].general.toaFirst
			> high.tracks[highIndex].last[0].general.toaLast)
	{
		high.compareHighLow(lowIndex, highIndex).stitchCompatible = 1;
		high.compareHighLow(lowIndex, highIndex).toaDiffFirstLast =
				low.tracks[lowIndex].general.toaFirst
						- high.tracks[highIndex].last[0].general.toaLast;
	}
}

uint8_t mergeCalcStatusStringRf(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo)
{
	uint8_t rfStatus = 0;
	uint32_t rfMinSource = 0xFFFFFFFF, rfMinOrgSource = 0xFFFFFFFF;
	uint32_t rfMaxSource = 0, rfMaxOrgSource = 0;
	uint32_t rfMinTarget = 0xFFFFFFFF, rfMinOrgTarget = 0xFFFFFFFF;
	uint32_t rfMaxTarget = 0, rfMaxOrgTarget = 0;
	uint16_t analysisRfExclusionDeltaCalc =
			ConstHigh::mergeDeltaStringRfExcludeLarge;

//	if (trackOne.rf.getBandCodeLegacy() != 2)
	{
		analysisRfExclusionDeltaCalc =
				ConstHigh::mergeDeltaStringRfExcludeMedium;
	}
	//Find max and minimum rfs of both tracks
	for (uint8_t i = 0; i < trackOne.rf.accumulateRf.size(); i++)
	{
		rfMinOrgSource = min(rfMinOrgSource, trackOne.rf.accumulateRf[i].rfLow);
		rfMaxOrgSource = max(rfMaxOrgSource, trackOne.rf.accumulateRf[i].rfLow);
	}
	for (uint8_t i = 0; i < trackTwo.rf.accumulateRf.size(); i++)
	{
		rfMinOrgTarget = min(rfMinOrgTarget, trackTwo.rf.accumulateRf[i].rfLow);
		rfMaxOrgTarget = max(rfMaxOrgTarget, trackTwo.rf.accumulateRf[i].rfLow);
	}
	if ((trackOne.rf.accumulateRf.size() > 0)
			&& (trackTwo.rf.accumulateRf.size() > 0))
	{
		rfMinSource = rfMinOrgSource;
		if (rfMinSource > ConstHigh::mergeDeltaStringRf)
		{
			rfMinSource = rfMinSource - ConstHigh::mergeDeltaStringRf;
		}
		else
		{
			rfMinSource = 0;
		}
		rfMaxSource = rfMaxOrgSource + ConstHigh::mergeDeltaStringRf;

		rfMinTarget = rfMinOrgTarget;
		rfMaxTarget = rfMaxOrgTarget;
		if ((rfMaxSource >= rfMinTarget) && (rfMinSource <= rfMaxTarget))
		{
			rfStatus = 1;
		}
		else
		{
			rfMinSource = rfMinOrgSource;
			if (rfMinSource > analysisRfExclusionDeltaCalc)
			{
				rfMinSource = rfMinSource - analysisRfExclusionDeltaCalc;
			}
			else
			{
				rfMinSource = 0;
			}
			rfMaxSource = rfMaxOrgSource + analysisRfExclusionDeltaCalc;
			if ((rfMaxSource >= rfMinTarget) && (rfMinSource <= rfMaxTarget))
			{
				rfStatus = 2;
			}
			else
			{
				rfStatus = 3;
			}
		}
	}
	return rfStatus;
}
uint8_t mergeCalcScoreStringPri(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo)
{
	uint8_t foundFlag = 0;
	uint16_t indexSource;
	uint16_t indexTarget;
	uint32_t priDifference = 0;
	uint8_t totalScore = 0;
	uint32_t priMinSource = 0;
	uint32_t priMaxSource;
	uint32_t priMinTarget = 0;
	uint32_t priMaxTarget;
	//review this, difftoa compatibility with stagger and dwell will require spot comparison
	if ((trackOne.pri.priTypeCurrentDominant == PriTypeEnum::pri_difftoa)
			|| (trackOne.pri.priTypeCurrentDominant == PriTypeEnum::pri_complex)
			|| (trackTwo.pri.priTypeCurrentDominant == PriTypeEnum::pri_difftoa)
			|| (trackTwo.pri.priTypeCurrentDominant == PriTypeEnum::pri_complex))
	{
		totalScore = totalScore + ConstHigh::mergeScoreStringPriUncertain;
	}
	//give some score for jitter and dwell compatibility
	if (((trackOne.pri.priTypeCurrentDominant == PriTypeEnum::pri_fixed)
			|| (trackOne.pri.priTypeCurrentDominant
					== PriTypeEnum::pri_dwellswitch)
			|| (trackOne.pri.priTypeCurrentDominant == PriTypeEnum::pri_jitter))
			&& ((trackTwo.pri.priTypeCurrentDominant == PriTypeEnum::pri_fixed)
					|| (trackTwo.pri.priTypeCurrentDominant
							== PriTypeEnum::pri_dwellswitch)
					|| (trackTwo.pri.priTypeCurrentDominant
							== PriTypeEnum::pri_jitter)))
	{
		if ((trackOne.pri.priTypeCurrentDominant == PriTypeEnum::pri_jitter)
				|| (trackTwo.pri.priTypeCurrentDominant
						== PriTypeEnum::pri_jitter))
		{
			totalScore = totalScore + 5;
		}
	}
	if ((trackOne.pri.accumulatePriFixedCount > 0)
			&& (trackTwo.pri.accumulatePriFixedCount > 0))
	{
		priDifference =
				labs(
						(int32_t) (trackOne.pri.getDwellSwitchLastSpot().priLow)
								- (int32_t) (trackTwo.pri.getDwellSwitchLastSpot().priLow));
		if (priDifference < ConstHigh::mergeDeltaStringPriFixedSmall)
			totalScore = totalScore + ConstHigh::mergeScoreStringPriFixedSmall;
		else if (priDifference < ConstHigh::mergeDeltaStringPriFixedMedium)
			totalScore = totalScore + ConstHigh::mergeScoreStringPriFixedMedium;
		else if (priDifference < ConstHigh::mergeDeltaStringPriFixedLarge)
			totalScore = totalScore + ConstHigh::mergeScoreStringPriFixedLarge;
	}
	else if ((trackOne.pri.accumulatePriDwellSwitchCount > 0)
			&& (trackTwo.pri.accumulatePriDwellSwitchCount > 0))
	{
		priMinSource = trackOne.pri.getDwellSwitchMinSpot().priLow;
		if (priMinSource > ConstHigh::mergeDeltaStringPriDwellSwitchSmall)
		{
			priMinSource = priMinSource
					- ConstHigh::mergeDeltaStringPriDwellSwitchSmall;
		}
		else
		{
			priMinSource = 0;
		}
		priMaxSource = trackOne.pri.getDwellSwitchMaxSpot().priLow
				+ ConstHigh::mergeDeltaStringPriDwellSwitchSmall;
		priMinTarget = trackTwo.pri.getDwellSwitchMinSpot().priLow;
		priMaxTarget = trackTwo.pri.getDwellSwitchMaxSpot().priLow;
		if ((priMaxSource >= priMinTarget) && (priMinSource <= priMaxTarget))
		{
			totalScore = totalScore
					+ ConstHigh::mergeScoreStringPriDwellSwitchSmall;
		}
		else
		{
			priMinSource = trackOne.pri.getDwellSwitchMinSpot().priLow;
			if (priMinSource > ConstHigh::mergeDeltaStringPriDwellSwitchMedium)
			{
				priMinSource = priMinSource
						- ConstHigh::mergeDeltaStringPriDwellSwitchMedium;
			}
			else
			{
				priMinSource = 0;
			}
			priMaxSource = trackOne.pri.getDwellSwitchMaxSpot().priLow
					+ ConstHigh::mergeDeltaStringPriDwellSwitchMedium;
			if ((priMaxSource >= priMinTarget)
					&& (priMinSource <= priMaxTarget))
			{
				totalScore = totalScore
						+ ConstHigh::mergeScoreStringPriDwellSwitchMedium;
			}
			else
			{
				priMinSource = trackOne.pri.getDwellSwitchMinSpot().priLow;
				if (priMinSource
						> ConstHigh::mergeDeltaStringPriDwellSwitchLarge)
				{
					priMinSource = priMinSource
							- ConstHigh::mergeDeltaStringPriDwellSwitchLarge;
				}
				else
				{
					priMinSource = 0;
				}
				priMaxSource = trackOne.pri.getDwellSwitchMaxSpot().priLow
						+ ConstHigh::mergeDeltaStringPriDwellSwitchLarge;
				if ((priMaxSource >= priMinTarget)
						&& (priMinSource <= priMaxTarget))
				{
					totalScore = totalScore
							+ ConstHigh::mergeScoreStringPriDwellSwitchLarge;
				}
			}
		}
	}

	//match stagger phases
	foundFlag = 0;
	for (indexSource = 0; indexSource < trackOne.pri.accumulatePri.size();
			indexSource++)
	{
		if (trackOne.pri.accumulatePri[indexSource].priType
				== PriTypeEnum::pri_stagger)
		{
			if (foundFlag == 1)
			{
				break;
			}
			for (indexTarget = 0;
					indexTarget < trackTwo.pri.accumulatePri.size();
					indexTarget++)
			{
				if (trackTwo.pri.accumulatePri[indexTarget].priType
						== PriTypeEnum::pri_stagger)
				{
					priMinSource =
							trackOne.pri.accumulatePri[indexSource].priLow;
					priMaxSource =
							trackOne.pri.accumulatePri[indexSource].priLow;
					priMinTarget =
							trackTwo.pri.accumulatePri[indexTarget].priLow;
					if (priMinTarget
							> ConstHigh::mergeDeltaStringPriStaggerPhase)
					{
						priMinTarget =
								trackTwo.pri.accumulatePri[indexTarget].priLow
										- ConstHigh::mergeDeltaStringPriStaggerPhase;
					}
					else
					{
						priMinTarget = 0;
					}
					priMaxTarget =
							trackTwo.pri.accumulatePri[indexTarget].priLow
									+ ConstHigh::mergeDeltaStringPriStaggerPhase;
					if ((priMaxSource >= priMinTarget)
							&& (priMinSource <= priMaxTarget))
					{
						totalScore = totalScore
								+ ConstHigh::mergeScoreStringPriStaggerPhase;
						foundFlag = 1;
						break;
					}
				}
			}
		}
	}
	//match stagger frames
	foundFlag = 0;
	for (indexSource = 0; indexSource < trackOne.pri.accumulatePri.size();
			indexSource++)
	{
		if (trackOne.pri.accumulatePri[indexSource].priType
				== PriTypeEnum::pri_stagger)
		{
			if (foundFlag == 1)
			{
				break;
			}
			for (indexTarget = 0;
					indexTarget < trackTwo.pri.accumulatePri.size();
					indexTarget++)
			{
				if (trackTwo.pri.accumulatePri[indexTarget].priType
						== PriTypeEnum::pri_stagger)
				{
					priMinSource =
							trackOne.pri.accumulatePri[indexSource].frame;
					priMaxSource =
							trackOne.pri.accumulatePri[indexSource].frame;
					priMinTarget =
							trackTwo.pri.accumulatePri[indexTarget].frame;
					if ((priMinSource == 0) && (priMaxSource == 0)
							&& (priMinTarget == 0))
					{
						continue;
					}
					if (priMinTarget
							> ConstHigh::mergeDeltaStringPriStaggerPhase)
					{
						priMinTarget =
								trackTwo.pri.accumulatePri[indexTarget].frame
										- ConstHigh::mergeDeltaStringPriStaggerPhase;
					}
					else
					{
						priMinTarget = 0;
					}
					priMaxTarget = trackTwo.pri.accumulatePri[indexTarget].frame
							+ ConstHigh::mergeDeltaStringPriStaggerPhase;
					if ((priMaxSource >= priMinTarget)
							&& (priMinSource <= priMaxTarget))
					{
						totalScore = totalScore
								+ ConstHigh::mergeScoreStringPriStaggerPhase;
						foundFlag = 1;
						break;
					}
				}
			}
		}
	}
	//cross match stagger frames and fixed pri
	foundFlag = 0;
	if ((trackOne.pri.accumulatePriStaggerCount > 0)
			&& (trackTwo.pri.accumulatePriDwellSwitchCount < 4))
	{
		for (indexSource = 0; indexSource < trackOne.pri.accumulatePri.size();
				indexSource++)
		{
			if (trackOne.pri.accumulatePri[indexSource].priType
					== PriTypeEnum::pri_stagger)
			{
				if (foundFlag == 1)
					break;

				for (indexTarget = 0;
						indexTarget < trackTwo.pri.accumulatePri.size();
						indexTarget++)
				{
					if (trackTwo.pri.accumulatePri[indexTarget].priType
							== PriTypeEnum::pri_dwellswitch)
					{
						priMinSource =
								trackOne.pri.accumulatePri[indexSource].frame;
						priMaxSource =
								trackOne.pri.accumulatePri[indexSource].frame;
						priMinTarget =
								trackTwo.pri.accumulatePri[indexTarget].priLow;
						if (priMinTarget
								> ConstHigh::mergeDeltaStringPriStaggerFrameDwellSwitch)
						{
							priMinTarget =
									trackTwo.pri.accumulatePri[indexTarget].priLow
											- ConstHigh::mergeDeltaStringPriStaggerFrameDwellSwitch;
						}
						else
						{
							priMinTarget = 0;
						}
						priMaxTarget =
								trackTwo.pri.accumulatePri[indexTarget].priLow
										+ ConstHigh::mergeDeltaStringPriStaggerFrameDwellSwitch;
						if ((priMaxSource >= priMinTarget)
								&& (priMinSource <= priMaxTarget))
						{
							totalScore =
									totalScore
											+ ConstHigh::mergeScoreStringPriStaggerFrameDwellSwitch;
							foundFlag = 1;
							break;
						}
					}
				}
			}
		}
	}
	//cross match stagger frames and fixed pri
	foundFlag = 0;
	if ((trackOne.pri.accumulatePriDwellSwitchCount < 4)
			&& (trackTwo.pri.accumulatePriStaggerCount > 0))
	{
		for (indexSource = 0; indexSource < trackOne.pri.accumulatePri.size();
				indexSource++)
		{
			if (trackOne.pri.accumulatePri[indexSource].priType
					== PriTypeEnum::pri_dwellswitch)
			{
				if (foundFlag == 1)
					break;

				for (indexTarget = 0;
						indexTarget < trackTwo.pri.accumulatePri.size();
						indexTarget++)
				{
					if (trackTwo.pri.accumulatePri[indexTarget].priType
							== PriTypeEnum::pri_stagger)
					{
						priMinSource =
								trackOne.pri.accumulatePri[indexSource].priLow;
						priMaxSource =
								trackOne.pri.accumulatePri[indexSource].priLow;
						priMinTarget =
								trackTwo.pri.accumulatePri[indexTarget].frame;
						if (priMinTarget
								> ConstHigh::mergeDeltaStringPriStaggerFrameDwellSwitch)
						{
							priMinTarget =
									trackTwo.pri.accumulatePri[indexTarget].frame
											- ConstHigh::mergeDeltaStringPriStaggerFrameDwellSwitch;
						}
						else
						{
							priMinTarget = 0;
						}
						priMaxTarget =
								trackTwo.pri.accumulatePri[indexTarget].frame
										+ ConstHigh::mergeDeltaStringPriStaggerFrameDwellSwitch;
						if ((priMaxSource >= priMinTarget)
								&& (priMinSource <= priMaxTarget))
						{
							totalScore =
									totalScore
											+ ConstHigh::mergeScoreStringPriStaggerFrameDwellSwitch;
							foundFlag = 1;
							break;
						}
					}
				}
			}
		}
	}
	return totalScore;
}
uint16_t mergeDifferencePw(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo)
{

	uint16_t i = 0;
	uint16_t j = 0;
	uint32_t tempPwDifference = 0;
	uint32_t minPwDifference = 4096;

	for (i = 0; i < trackOne.pw.pwCircular.size(); i++)
	{
		for (j = 0; j < trackTwo.pw.pwCircular.size(); j++)
		{
			tempPwDifference = labs(
					(int32_t) trackOne.pw.pwCircular[i].pwLow
							- (int32_t) trackTwo.pw.pwCircular[j].pwLow);
			if (tempPwDifference < minPwDifference)
			{
				minPwDifference = tempPwDifference;
			}
		}
	}
	return ((uint16_t) minPwDifference);

	/*uint16_t pwDifferenceOne = 4096;
	 uint16_t pwDifferenceTwo = 4096;

	 if((trackOne.pw.accumulatePw.size() > 0) &&
	 (trackTwo.pw.accumulatePw.size() > 0)) {
	 pwDifferenceOne = labs((int32_t)trackOne.pw.accumulatePw.back().pwLow -
	 (int32_t)trackTwo.pw.accumulatePw.back().pwLow);
	 }
	 if((trackOne.pw.accumulatePw.size() > 1) &&
	 (trackTwo.pw.accumulatePw.size() > 1)) {
	 pwDifferenceOne = labs((int32_t)trackOne.pw.accumulatePw.front().pwLow -
	 (int32_t)trackTwo.pw.accumulatePw.front().pwLow);
	 }
	 if(pwDifferenceOne <= pwDifferenceTwo)
	 return pwDifferenceOne;
	 return pwDifferenceTwo;
	 */
}
uint16_t mergeDifferencePwAccumulate(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo)
{

	uint16_t i = 0;
	uint16_t j = 0;
	uint32_t tempPwDifference = 0;
	uint32_t minPwDifference = 4096;

	for (i = 0; i < trackOne.pw.accumulatePw.size(); i++)
	{
		for (j = 0; j < trackTwo.pw.accumulatePw.size(); j++)
		{
			tempPwDifference = labs(
					(int32_t) trackOne.pw.accumulatePw[i].pwLow
							- (int32_t) trackTwo.pw.accumulatePw[j].pwLow);
			if (tempPwDifference < minPwDifference)
			{
				minPwDifference = tempPwDifference;
			}
		}
	}
	return ((uint16_t) minPwDifference);
}
uint16_t mergeDifferencePriType(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo)
{

	uint16_t i = 0;
	uint16_t j = 0;

	for (i = 0; i < trackOne.pri.accumulatePri.size(); i++)
	{
		for (j = 0; j < trackTwo.pri.accumulatePri.size(); j++)
		{
			if ((trackOne.pri.accumulatePri[i].priType
					== PriTypeEnum::pri_complex)
					&& (trackTwo.pri.accumulatePri[j].priType
							== PriTypeEnum::pri_complex))
			{
				return 1;
			}
			if ((trackOne.pri.accumulatePri[i].priType
					== PriTypeEnum::pri_sliding)
					&& (trackTwo.pri.accumulatePri[j].priType
							== PriTypeEnum::pri_sliding))
			{
				return 1;
			}
			if ((trackOne.pri.accumulatePri[i].priType
					== PriTypeEnum::pri_difftoa)
					&& (trackTwo.pri.accumulatePri[j].priType
							== PriTypeEnum::pri_difftoa))
			{
				return 1;
			}
			if ((trackOne.pri.accumulatePri[i].priType
					== PriTypeEnum::pri_jitter)
					&& (trackTwo.pri.accumulatePri[j].priType
							== PriTypeEnum::pri_jitter))
			{
				return 1;
			}
			if ((trackOne.pri.accumulatePri[i].priType
					== PriTypeEnum::pri_stagger)
					&& (trackTwo.pri.accumulatePri[j].priType
							== PriTypeEnum::pri_stagger))
			{
				return 1;
			}
			if ((trackOne.pri.accumulatePri[i].priType == PriTypeEnum::pri_fixed)
					&& (trackTwo.pri.accumulatePri[j].priType
							== PriTypeEnum::pri_fixed))
			{
				return 1;
			}
			if ((trackOne.pri.accumulatePri[i].priType
					== PriTypeEnum::pri_dwellswitch)
					&& (trackTwo.pri.accumulatePri[j].priType
							== PriTypeEnum::pri_dwellswitch))
			{
				return 1;
			}
			if ((trackOne.pri.accumulatePri[i].priType
					== PriTypeEnum::pri_dwellswitch)
					&& (trackTwo.pri.accumulatePri[j].priType
							== PriTypeEnum::pri_fixed))
			{
				return 1;
			}
			if ((trackOne.pri.accumulatePri[i].priType == PriTypeEnum::pri_fixed)
					&& (trackTwo.pri.accumulatePri[j].priType
							== PriTypeEnum::pri_dwellswitch))
			{
				return 1;
			}
		}
	}
	return 0;
}

//verify how this can be called for tracks that are slated to be updated in the next iteration
//that should not be allowed to happen
//create difftoa pri score predicates where required
uint16_t mergeCalcScoreStringHighLow(DataSensorClass &low,
		DataSensorClass &high, uint16_t lowIndex, uint16_t highIndex)
{

	uint8_t rfStatus;
	uint8_t priTypeCompatible = 0;
	uint16_t pxDiff = 0;
	uint16_t pyDiff = 0;
	uint16_t angleDiff = 0;
	uint16_t angleDiffQuadrantSwap = 0;
	uint16_t powerDiff = 0;
	uint16_t pwDiff = 0;
	int64_t toaDiffFirstLast = 0;
	uint8_t parameterExclusionStatus = 0;
	uint8_t angleScore = 0;
	uint8_t bandScore = 0;
	uint8_t toaScore = 0;
	uint8_t pxScore = 0;
	uint8_t pyScore = 0;
	uint8_t pwScoreThree = 0;
	uint8_t pwScoreTen = 0;
	uint8_t powerScore = 0;
	uint8_t rfScore = 0;
	uint8_t threatLibraryScoreLowBar = 0;
	uint8_t threatLibraryScoreHighBar = 0;
	uint8_t priTypeScore = 0;
	uint8_t priFailedScore = 0;
	uint8_t priInitialScore = 0;
	uint16_t overallScore = 0;
	uint8_t groundIlluminationStatus = 0;

	if (high.compareHighLow(lowIndex, highIndex).parameterCompareStatus == 1)
	{
		return high.compareHighLow(lowIndex, highIndex).overallScore;
	}
	if (high.tracks[highIndex].last[0].power.groundIlluminationFlag == 1
			|| low.tracks[lowIndex].power.groundIlluminationFlag == 1)
	{
		groundIlluminationStatus = 1;
	}
	angleDiff = high.compareHighLow(lowIndex, highIndex).angleDiff;
	angleDiffQuadrantSwap =
			high.compareHighLow(lowIndex, highIndex).angleDiffQuadrantSwap;
	powerDiff =
			labs(
					(int16_t) low.tracks[lowIndex].power.sumPxPyMax
							- (int16_t) high.tracks[highIndex].last[0].power.sumPxPyMax);
	pwDiff = mergeDifferencePw(low.tracks[lowIndex],
			high.tracks[highIndex].last[0]);
	pxDiff = labs(
			(int16_t) low.tracks[lowIndex].power.pxMax
					- (int16_t) high.tracks[highIndex].last[0].power.pxMax);
	pyDiff = labs(
			(int16_t) low.tracks[lowIndex].power.pyMax
					- (int16_t) high.tracks[highIndex].last[0].power.pyMax);
	toaDiffFirstLast =
			high.compareHighLow(lowIndex, highIndex).toaDiffFirstLast;

	high.compareHighLow(lowIndex, highIndex).powerDiff = powerDiff;
	high.compareHighLow(lowIndex, highIndex).priTypeCompatible = 0;

	if (high.tracks[highIndex].rf.bandCode == low.tracks[lowIndex].rf.bandCode)
	{
		bandScore = ConstHigh::mergeScoreStringBand;				//band score
	}
	rfStatus = mergeCalcStatusStringRf(low.tracks[lowIndex],
			high.tracks[highIndex].last[0]);
//	if (rfStatus == 1)
//	{
//		rfScore = ConstHigh::mergeScoreStringRf;
//	}
	//exclude based on rf
	if (rfStatus == 3)
	{
		parameterExclusionStatus = 1;
	}
	if ((high.tracks[highIndex].libmatch.active.flagKnownLibrary == 1)
			&& (low.tracks[lowIndex].libmatch.active.flagKnownLibrary == 1))
	{
		if (low.tracks[lowIndex].libmatch.active.highestMatchList[0].emitterBatchId
				!= high.tracks[highIndex].libmatch.active.highestMatchList[0].emitterBatchId)
		{
			if ((low.tracks[lowIndex].rf.getBandCodeLegacy() == 2)
					&& (high.tracks[highIndex].last[0].rf.getBandCodeLegacy()
							== 2))
			{
				if (low.tracks[lowIndex].libmatch.active.highestMatchList[0].dwellCount
						> 0
						&& high.tracks[highIndex].libmatch.active.highestMatchList[0].dwellCount
								> 0)
				{
					if (low.tracks[lowIndex].libmatch.active.highestMatchList[0].noOfSpotsMatched
							== low.tracks[lowIndex].libmatch.active.highestMatchList[0].libraryPriCount
							|| low.tracks[lowIndex].libmatch.active.highestMatchList[0].noOfSpotsMatched
									>= 3)
					{
						parameterExclusionStatus = 1;
					}
				}
				else
				{
					parameterExclusionStatus = 1;
				}
			}
			else
			{
				parameterExclusionStatus = 1;
			}
		}
	}
	//linear score for bearing
	if (angleDiff < ConstHigh::mergeScoreStringBearingMax)
	{
		angleScore = (ConstHigh::mergeScoreStringBearingMax - angleDiff);
	}
	//linear score for power
	if (powerDiff < ConstHigh::mergeDeltaStringPower)
	{
		powerScore = (uint16_t) ((((((float) ConstHigh::mergeDeltaStringPower)
				- ((float) powerDiff)))
				/ ((float) ConstHigh::mergeDeltaStringPower))
				* ((float) ConstHigh::mergeScoreStringPower));
	}
	//linear score for pulse width
	if (pwDiff < ConstHigh::mergeDeltaStringPwLarge)
	{
		pwScoreTen = (uint16_t) ((((((float) ConstHigh::mergeDeltaStringPwLarge)
				- ((float) pwDiff)))
				/ ((float) ConstHigh::mergeDeltaStringPwLarge))
				* ((float) ConstHigh::mergeScoreStringPwLarge));
	}
	//additional score for pulse width
	if (pwDiff < ConstHigh::mergeDeltaStringPwSmall)
	{
		pwScoreThree = ConstHigh::mergeScoreStringPwSmall;
	}
	if (pxDiff < ConstHigh::mergeDeltaStringPx)
	{
//		pxScore = ConstHigh::mergeScoreStringPx;					//px score
		if (pyDiff < ConstHigh::mergeDeltaStringPy)
		{
			pxScore = ConstHigh::mergeScoreStringPx;
			pyScore = ConstHigh::mergeScoreStringPy;				//py score
		}
	}
//	if (pyDiff < ConstHigh::mergeDeltaStringPy)
//	{
//		pyScore = ConstHigh::mergeScoreStringPy;					//py score
//	}
	if (toaDiffFirstLast < (int64_t) ConstHigh::mergeDeltaStringToaDiff)
	{
		toaScore = ConstHigh::mergeScoreStringToaDiff;				//toa score
	}
	if (toaDiffFirstLast < (int64_t) ConstHigh::mergeDeltaStringToaDiffSmall)
	{
		toaScore += ConstHigh::mergeScoreStringToaDiff;				//toa score
	}
	if (((low.tracks[lowIndex].pri.accumulatePriComplexCount > 0)
			|| (low.tracks[lowIndex].pri.accumulatePriDiffToaCount > 0)) && (//(high.tracks[highIndex].last[0].pri.accumulatePriComplexCount > 0) ||
			(high.tracks[highIndex].last[0].pri.accumulatePriDiffToaCount > 0)))
	{
		priFailedScore = ConstHigh::mergeScoreStringPriWeak;
	}
	/*if((emitterLibraryRecordSource->complexPriFlag == 0) && (emitterLibraryRecordTarget->complexPriFlag == 0))
	 {
	 if((emitterLibraryRecordSource->reportUpdated == 1) && (emitterLibraryRecordTarget->reportUpdated == 1))
	 {
	 parameterExclusionStatus = 1;//complex pri and updates exclusion
	 }
	 else if((emitterLibraryRecordSource->reportUpdatedNext == 1) && (emitterLibraryRecordTarget->reportUpdatedNext == 1))
	 {
	 parameterExclusionStatus = 1;//complex pri and updates exclusion
	 }
	 }*/
	if ((low.tracks[lowIndex].libmatch.active.flagKnownPartialLibrary == 1)
			&& (high.tracks[highIndex].last[0].libmatch.active.flagKnownPartialLibrary
					== 1))
	{
		if (low.tracks[lowIndex].libmatch.active.highestMatchList[0].emitterBatchId
				== high.tracks[highIndex].last[0].libmatch.active.highestMatchList[0].emitterBatchId)
		{
			threatLibraryScoreLowBar =
					ConstHigh::mergeScoreStringLibraryPartial;
		}
		if (low.tracks[lowIndex].libmatch.active.highestMatchList[0].ID
				== high.tracks[highIndex].last[0].libmatch.active.highestMatchList[0].ID)
		{
			threatLibraryScoreLowBar =
					ConstHigh::mergeScoreStringLibraryPartial;
		}
		if (low.tracks[lowIndex].libmatch.active.highestMatchListCount > 1)
		{
			if (low.tracks[lowIndex].libmatch.active.highestMatchList[1].ID
					== high.tracks[highIndex].last[0].libmatch.active.highestMatchList[0].ID)
			{
				threatLibraryScoreLowBar =
						ConstHigh::mergeScoreStringLibraryPartial;
			}
		}
		if (high.tracks[highIndex].last[0].libmatch.active.highestMatchListCount
				> 1)
		{
			if (low.tracks[lowIndex].libmatch.active.highestMatchList[0].ID
					== high.tracks[highIndex].last[0].libmatch.active.highestMatchList[1].ID)
			{
				threatLibraryScoreLowBar =
						ConstHigh::mergeScoreStringLibraryPartial;
			}
		}
		if ((low.tracks[lowIndex].libmatch.active.highestMatchListCount > 1)
				&& (high.tracks[highIndex].last[0].libmatch.active.highestMatchListCount
						> 1))
		{
			if (low.tracks[lowIndex].libmatch.active.highestMatchList[1].ID
					== high.tracks[highIndex].last[0].libmatch.active.highestMatchList[1].ID)
			{
				threatLibraryScoreLowBar =
						ConstHigh::mergeScoreStringLibraryPartial;
			}
		}
	}
	//pri type compatible
	priInitialScore = mergeCalcScoreStringPri(low.tracks[lowIndex],
			high.tracks[highIndex].last[0]);
	if ((low.tracks[lowIndex].pri.accumulatePriDwellSwitchCount > 0)
			&& (high.tracks[highIndex].last[0].pri.accumulatePriDwellSwitchCount
					> 0))
	{
		priTypeCompatible = 1;
		priTypeScore = ConstHigh::mergeScoreStringPriType;
	}
	else if ((low.tracks[lowIndex].pri.accumulatePriStaggerCount > 0)
			&& (high.tracks[highIndex].last[0].pri.accumulatePriStaggerCount > 0))
	{
		if (priInitialScore == 0)
		{
			priTypeCompatible = 0;
			priTypeScore = 0;
		}
		else
		{
			priTypeCompatible = 1;
			priTypeScore = ConstHigh::mergeScoreStringPriType;
		}
	}
	else if (((low.tracks[lowIndex].pri.accumulatePriStaggerCount > 0)
			&& (high.tracks[highIndex].last[0].pri.priTypeFutureDominant
					== PriTypeEnum::pri_stagger))
			|| ((low.tracks[lowIndex].pri.priTypeFutureDominant
					== PriTypeEnum::pri_stagger)
					&& (high.tracks[highIndex].last[0].pri.accumulatePriStaggerCount
							> 0)))
	{
		priTypeCompatible = 1;
		priTypeScore = ConstHigh::mergeScoreStringPriType;

	}
//	else if ((low.tracks[lowIndex].pri.accumulatePriStaggerCount > 0)
//			&& (high.tracks[highIndex].last[0].pri.accumulatePriFixedCount > 0))
//	{
//		priTypeCompatible = 1;
//		priTypeScore = ConstHigh::mergeScoreStringPriType;
//		//if((priInitialScore == 0) && (pwDiff > 10) && (powerDiff > 70))
//		//	parameterExclusionStatus = 1;
//	}
//	else if ((low.tracks[lowIndex].pri.accumulatePriFixedCount > 0)
//			&& (high.tracks[highIndex].last[0].pri.accumulatePriStaggerCount > 0))
//	{
//		priTypeCompatible = 1;
//		priTypeScore = ConstHigh::mergeScoreStringPriType;
//		//if((priInitialScore == 0) && (pwDiff > 10) && (powerDiff > 70))
//		//	parameterExclusionStatus = 1;
//	}
	else if ((low.tracks[lowIndex].pri.accumulatePriDiffToaCount > 0)
			&& (high.tracks[highIndex].last[0].pri.accumulatePriDiffToaCount > 0))
	{
		priTypeCompatible = 1;
		priTypeScore = ConstHigh::mergeScoreStringPriType;
	}
	else if ((low.tracks[lowIndex].pri.accumulatePriJitterCount > 0)
			|| (low.tracks[lowIndex].pri.accumulatePriSlidingCount > 0)
			|| (low.tracks[lowIndex].pri.accumulatePriComplexCount > 0))
	{
		priTypeCompatible = 1;
	}
	else
	{
		priTypeCompatible = 0;
		/*if(((emitterLibraryRecordSource->staggerPriSpotsCount > 0) && (emitterLibraryRecordSource->dwellPriCount == 0)) &&
		 ((emitterLibraryRecordTarget->staggerPriSpotsCount == 0) && (emitterLibraryRecordTarget->dwellPriCount > 1) &&  (emitterLibraryRecordTarget->fixedPriFlag == 0))
		 )
		 {
		 parameterExclusionStatus = 1;
		 }
		 else if(((emitterLibraryRecordTarget->staggerPriSpotsCount > 0) && (emitterLibraryRecordTarget->dwellPriCount == 0)) &&
		 ((emitterLibraryRecordSource->staggerPriSpotsCount == 0) && (emitterLibraryRecordSource->dwellPriCount > 1) &&  (emitterLibraryRecordSource->fixedPriFlag == 0))
		 )
		 {
		 parameterExclusionStatus = 1;
		 }*/
		//review this predicate, may not be needed with the new library structure
		/*if((emitterLibraryRecordSource->libraryMatchIndex != UNKNOWN_LIB_MATCH_INDEX) && (emitterLibraryRecordTarget->libraryMatchIndex != UNKNOWN_LIB_MATCH_INDEX))
		 {
		 if(emitterLibraryRecordSource->libraryMatchCategory == emitterLibraryRecordTarget->libraryMatchCategory)
		 {
		 if((pCurrentLibrary->EmittersExtendedData[emitterLibraryRecordSource->libraryMatchIndex].threatTypeIs == 2) &&
		 (pCurrentLibrary->EmittersExtendedData[emitterLibraryRecordTarget->libraryMatchIndex].threatTypeIs == 2))
		 {
		 if((pCurrentLibrary->Emitters[emitterLibraryRecordSource->libraryMatchIndex].PRI.StaggeredPRICount > 0) ||
		 (pCurrentLibrary->Emitters[emitterLibraryRecordTarget->libraryMatchIndex].PRI.StaggeredPRICount > 0))
		 {
		 priTypeCompatible = 1;
		 priInitialScore = 10;
		 }
		 }
		 }
		 }*/
		if (priTypeCompatible == 0)
		{
			if (priInitialScore == 0)
			{
				parameterExclusionStatus = 1;
			}
		}
	}
	//temporary addition, can be tightened additionally
//	if ((angleDiff > ConstHigh::mergeLimitStringBearingPrimary)
//			&& (angleDiffQuadrantSwap
//					> ConstHigh::mergeLimitStringBearingSecondary)
//			&& ((pwDiff > ConstHigh::mergeDeltaStringPwMedium)
//					|| (priInitialScore < 10) || (priTypeCompatible == 0)))
	if ((angleDiff > ConstHigh::mergeLimitStringBearingPrimary)
			&& (angleDiffQuadrantSwap
					> ConstHigh::mergeLimitStringBearingSecondary)
			&& ((pwDiff > ConstHigh::mergeDeltaStringPwMedium)
					|| (priInitialScore < 10) || (priTypeCompatible == 0))
			&& ((pwDiff > ConstHigh::mergeDeltaStringPwSmall)
					|| (priInitialScore < 5)))
	{
		parameterExclusionStatus = 1;
	}
	//end of temporary addition
	if ((priInitialScore == 0) && (pwDiff > ConstHigh::mergeDeltaStringPwMedium)
			&& (low.tracks[lowIndex].rf.getBandCodeLegacy() == 2)
			&& (high.tracks[highIndex].last[0].rf.getBandCodeLegacy() == 2)
			&& (low.tracks[lowIndex].pri.accumulatePriJitterCount == 0)
			&& (low.tracks[lowIndex].pri.accumulatePriComplexCount == 0)
			&& (low.tracks[lowIndex].pri.accumulatePriSlidingCount == 0)
			&& (high.tracks[highIndex].last[0].pri.accumulatePriJitterCount == 0)
			&& (high.tracks[highIndex].last[0].pri.accumulatePriComplexCount
					== 0)
			&& (high.tracks[highIndex].last[0].pri.accumulatePriSlidingCount
					== 0))
	{
		parameterExclusionStatus = 1;
	}
	if ((priInitialScore < 10) && (pwDiff > ConstHigh::mergeDeltaStringPwMedium)
			&& (low.tracks[lowIndex].rf.getBandCodeLegacy() == 2)
			&& (high.tracks[highIndex].last[0].rf.getBandCodeLegacy() == 2)
			&& (low.tracks[lowIndex].pri.accumulatePriJitterCount == 0)
			&& (low.tracks[lowIndex].pri.accumulatePriComplexCount == 0)
			&& (low.tracks[lowIndex].pri.accumulatePriSlidingCount == 0)
			&& (high.tracks[highIndex].last[0].pri.accumulatePriJitterCount == 0)
			&& (high.tracks[highIndex].last[0].pri.accumulatePriComplexCount
					== 0)
			&& (high.tracks[highIndex].last[0].pri.accumulatePriSlidingCount
					== 0)
			&& (low.tracks[lowIndex].pri.accumulatePriDwellSwitchCount > 0)
			&& (high.tracks[highIndex].last[0].pri.accumulatePriDwellSwitchCount
					> 0)
			&& (high.tracks[highIndex].general.timeIterCountLifetimeGlobal > 2))
	{
		parameterExclusionStatus = 1;
	}
	if ((priInitialScore < 10) && (pwDiff > ConstHigh::mergeDeltaStringPwMedium)
			&& (powerDiff > ConstHigh::mergeDeltaStringPowerExclusion)/* &&
			 (low.tracks[lowIndex].rf.getBandCodeLegacy() == 2) &&
			 (high.tracks[highIndex].last[0].rf.getBandCodeLegacy() == 2)*/)
	{
		parameterExclusionStatus = 1;
	}
	if ((priInitialScore == 10)
			&& (powerDiff > ConstHigh::mergeDeltaStringPowerExclusion)
			&& (low.tracks[lowIndex].rf.getBandCodeLegacy() == 2)
			&& (high.tracks[highIndex].last[0].rf.getBandCodeLegacy() == 2))
	{
		parameterExclusionStatus = 1;
	}
	if ((priInitialScore == 10)
			&& (pwDiff > ConstHigh::mergeDeltaStringPwMedium)
			&& (low.tracks[lowIndex].rf.getBandCodeLegacy() == 2)
			&& (high.tracks[highIndex].last[0].rf.getBandCodeLegacy() == 2)
			&& (high.tracks[highIndex].general.timeIterCountLifetimeGlobal > 1)
			&& (low.tracks[lowIndex].pri.accumulatePriDwellSwitchCount > 0)
			&& (high.tracks[highIndex].last[0].pri.accumulatePriDwellSwitchCount
					> 0)
			&& (low.tracks[lowIndex].pri.accumulatePriStaggerCount == 0)
			&& (high.tracks[highIndex].last[0].pri.accumulatePriStaggerCount
					== 0)
			&& (low.tracks[lowIndex].history.countPulseWidthOverlapMaxLow > 0))
	{
		parameterExclusionStatus = 1;
	}
	if ((groundIlluminationStatus == 0) && powerScore > 10 && angleScore == 0
			&& pxDiff > 320 && pyDiff > 320)
	{
		powerScore = 0;
	}

	overallScore = angleScore + bandScore + toaScore + pxScore + pyScore
			+ pwScoreThree + pwScoreTen + powerScore + rfScore
			+ threatLibraryScoreLowBar + threatLibraryScoreHighBar
			+ priTypeScore + priFailedScore + priInitialScore;

	high.compareHighLow(lowIndex, highIndex).rfScore = rfScore;
	high.compareHighLow(lowIndex, highIndex).parameterExclusionStatus =
			parameterExclusionStatus;
	high.compareHighLow(lowIndex, highIndex).angleScore = angleScore;
	high.compareHighLow(lowIndex, highIndex).powerScore = powerScore;
	high.compareHighLow(lowIndex, highIndex).pwScoreTen = pwScoreTen;
	high.compareHighLow(lowIndex, highIndex).pwScoreThree = pwScoreThree;
	high.compareHighLow(lowIndex, highIndex).pxScore = pxScore;
	high.compareHighLow(lowIndex, highIndex).pyScore = pyScore;
	high.compareHighLow(lowIndex, highIndex).toaScore = toaScore;
	high.compareHighLow(lowIndex, highIndex).bandScore = bandScore;
	high.compareHighLow(lowIndex, highIndex).priDetailCompareStatus = 0;
	high.compareHighLow(lowIndex, highIndex).priTypeCompatible =
			priTypeCompatible;
	high.compareHighLow(lowIndex, highIndex).priTypeScore = priTypeScore;
	high.compareHighLow(lowIndex, highIndex).priFailedScore = priFailedScore;
	high.compareHighLow(lowIndex, highIndex).priInitialScore = priInitialScore;
	high.compareHighLow(lowIndex, highIndex).priDetailScore = 0;
	high.compareHighLow(lowIndex, highIndex).threatLibraryScoreLowBar =
			threatLibraryScoreLowBar;
	high.compareHighLow(lowIndex, highIndex).threatLibraryScoreHighBar =
			threatLibraryScoreHighBar;
	high.compareHighLow(lowIndex, highIndex).overallScore = overallScore;
	high.compareHighLow(lowIndex, highIndex).parameterCompareStatus = 1;

	return overallScore;
}

uint16_t mergeCalcScoreStringLowLow(DataSensorClass &low, DataSensorClass &high,
		uint16_t indexOne, uint16_t indexTwo)
{

	uint8_t rfStatus;
	uint8_t priTypeCompatible = 0;
	uint16_t pxDiff = 0;
	uint16_t pyDiff = 0;
	uint16_t angleDiff = 0;
	uint16_t angleDiffQuadrantSwap = 0;
	uint16_t powerDiff = 0;
	uint16_t pwDiff = 0;
	int64_t toaDiffFirstLast = 0;
	uint8_t parameterExclusionStatus = 0;
	uint8_t angleScore = 0;
	uint8_t bandScore = 0;
	uint8_t toaScore = 0;
	uint8_t pxScore = 0;
	uint8_t pyScore = 0;
	uint8_t pwScoreThree = 0;
	uint8_t pwScoreTen = 0;
	uint8_t powerScore = 0;
	uint8_t rfScore = 0;
	uint8_t threatLibraryScoreLowBar = 0;
	uint8_t threatLibraryScoreHighBar = 0;
	uint8_t priTypeScore = 0;
	uint8_t priFailedScore = 0;
	uint8_t priInitialScore = 0;
	uint16_t overallScore = 0;
	uint8_t groundIlluminationStatus = 0;

	if (low.compareLowLow(indexOne, indexTwo).parameterCompareStatus == 1)
	{
		return low.compareLowLow(indexOne, indexTwo).overallScore;
	}

	if (low.tracks[indexOne].power.groundIlluminationFlag == 1
			|| low.tracks[indexTwo].power.groundIlluminationFlag == 1)
	{
		groundIlluminationStatus = 1;
	}

	angleDiff = low.compareLowLow(indexOne, indexTwo).angleDiff;
	angleDiffQuadrantSwap =
			low.compareLowLow(indexOne, indexTwo).angleDiffQuadrantSwap;
	powerDiff = labs(
			(int16_t) low.tracks[indexOne].power.sumPxPyMax
					- (int16_t) low.tracks[indexTwo].power.sumPxPyMax);
	pwDiff = mergeDifferencePw(low.tracks[indexOne], low.tracks[indexTwo]);
	pxDiff = labs(
			(int16_t) low.tracks[indexOne].power.pxMax
					- (int16_t) low.tracks[indexTwo].power.pxMax);
	pyDiff = labs(
			(int16_t) low.tracks[indexOne].power.pyMax
					- (int16_t) low.tracks[indexTwo].power.pyMax);
	toaDiffFirstLast = low.compareLowLow(indexOne, indexTwo).toaDiffFirstLast;

	low.compareLowLow(indexOne, indexTwo).powerDiff = powerDiff;
	low.compareLowLow(indexOne, indexTwo).priTypeCompatible = 0;

	if (low.tracks[indexOne].rf.bandCode == low.tracks[indexTwo].rf.bandCode)
	{
		bandScore = ConstHigh::mergeScoreStringBand;		//band score
	}
	rfStatus = mergeCalcStatusStringRf(low.tracks[indexOne],
			low.tracks[indexTwo]);
//	if (rfStatus == 1)
//	{
//		rfScore = ConstHigh::mergeScoreStringRf;
//	}
	//exclude based on rf
	if (rfStatus == 3)
	{
		parameterExclusionStatus = 1;
	}
	//linear score for bearing
	if (angleDiff < ConstHigh::mergeScoreStringBearingMax)
	{
		angleScore = (ConstHigh::mergeScoreStringBearingMax - angleDiff);
	}
	//linear score for power
	if (powerDiff < ConstHigh::mergeDeltaStringPower)
	{
		powerScore = (uint16_t) ((((((float) ConstHigh::mergeDeltaStringPower)
				- ((float) powerDiff)))
				/ ((float) ConstHigh::mergeDeltaStringPower))
				* ((float) ConstHigh::mergeScoreStringPower));
	}
	//linear score for pulse width
	if (pwDiff < ConstHigh::mergeDeltaStringPwLarge)
	{
		pwScoreTen = (uint16_t) ((((((float) ConstHigh::mergeDeltaStringPwLarge)
				- ((float) pwDiff)))
				/ ((float) ConstHigh::mergeDeltaStringPwLarge))
				* ((float) ConstHigh::mergeScoreStringPwLarge));
	}
	//additional score for pulse width
	if (pwDiff < ConstHigh::mergeDeltaStringPwSmall)
	{
		pwScoreThree = ConstHigh::mergeScoreStringPwSmall;
	}
	if (pxDiff < ConstHigh::mergeDeltaStringPx)
	{
//		pxScore = ConstHigh::mergeScoreStringPx;					//px score
		if (pyDiff < ConstHigh::mergeDeltaStringPy)
		{
			pxScore = ConstHigh::mergeScoreStringPx;
			pyScore = ConstHigh::mergeScoreStringPy;				//py score
		}
	}
//	if (pyDiff < ConstHigh::mergeDeltaStringPy)
//	{
//		pyScore = ConstHigh::mergeScoreStringPy;		//py score
//	}
	if (toaDiffFirstLast < (int64_t) ConstHigh::mergeDeltaStringToaDiff)
	{
		toaScore = ConstHigh::mergeScoreStringToaDiff;		//toa score
	}
	if (toaDiffFirstLast < (int64_t) ConstHigh::mergeDeltaStringToaDiffSmall)
	{
		toaScore += ConstHigh::mergeScoreStringToaDiff;		//toa score
	}
	if (((low.tracks[indexOne].pri.accumulatePriComplexCount > 0)
			|| (low.tracks[indexOne].pri.accumulatePriDiffToaCount > 0))
			&& ((low.tracks[indexTwo].pri.accumulatePriComplexCount > 0)
					|| (low.tracks[indexTwo].pri.accumulatePriDiffToaCount > 0)))
	{
		priFailedScore = ConstHigh::mergeScoreStringPriWeak;
	}
	if ((low.tracks[indexOne].pri.accumulatePriComplexCount == 0)
			&& (low.tracks[indexOne].pri.accumulatePriDiffToaCount == 0)
			&& (low.tracks[indexTwo].pri.accumulatePriComplexCount == 0)
			&& (low.tracks[indexTwo].pri.accumulatePriDiffToaCount == 0))
	{
		if ((low.tracks[indexOne].history.lowToaStartUpdateFlag == 1)
				&& (low.tracks[indexTwo].history.lowToaStartUpdateFlag == 1))
		{
			return 1;
		}
		if ((low.tracks[indexOne].history.lowToaStartUpdateNextFlag == 1)
				&& (low.tracks[indexTwo].history.lowToaStartUpdateNextFlag == 1))
		{
			return 1;
		}
	}
	if ((low.tracks[indexOne].libmatch.active.flagKnownPartialLibrary == 1)
			&& (low.tracks[indexTwo].libmatch.active.flagKnownPartialLibrary
					== 1))
	{
		if (low.tracks[indexOne].libmatch.active.highestMatchList[0].emitterBatchId
				== low.tracks[indexTwo].libmatch.active.highestMatchList[0].emitterBatchId)
		{
			threatLibraryScoreLowBar =
					ConstHigh::mergeScoreStringLibraryPartial;
		}
		if (low.tracks[indexOne].libmatch.active.highestMatchList[0].ID
				== low.tracks[indexTwo].libmatch.active.highestMatchList[0].ID)
		{
			threatLibraryScoreLowBar =
					ConstHigh::mergeScoreStringLibraryPartial;
		}
		if (low.tracks[indexOne].libmatch.active.highestMatchListCount > 1)
		{
			if (low.tracks[indexOne].libmatch.active.highestMatchList[1].ID
					== low.tracks[indexTwo].libmatch.active.highestMatchList[0].ID)
			{
				threatLibraryScoreLowBar =
						ConstHigh::mergeScoreStringLibraryPartial;
			}
		}
		if (low.tracks[indexTwo].libmatch.active.highestMatchListCount > 1)
		{
			if (low.tracks[indexOne].libmatch.active.highestMatchList[0].ID
					== low.tracks[indexTwo].libmatch.active.highestMatchList[1].ID)
			{
				threatLibraryScoreLowBar =
						ConstHigh::mergeScoreStringLibraryPartial;
			}
		}
		if ((low.tracks[indexOne].libmatch.active.highestMatchListCount > 1)
				&& (low.tracks[indexTwo].libmatch.active.highestMatchListCount
						> 1))
		{
			if (low.tracks[indexOne].libmatch.active.highestMatchList[1].ID
					== low.tracks[indexTwo].libmatch.active.highestMatchList[1].ID)
			{
				threatLibraryScoreLowBar =
						ConstHigh::mergeScoreStringLibraryPartial;
			}
		}
	}
	//pri type compatible
	priInitialScore = mergeCalcScoreStringPri(low.tracks[indexOne],
			low.tracks[indexTwo]);
	if ((low.tracks[indexOne].pri.accumulatePriDwellSwitchCount > 0)
			&& (low.tracks[indexTwo].pri.accumulatePriDwellSwitchCount > 0))
	{
		priTypeCompatible = 1;
		priTypeScore = ConstHigh::mergeScoreStringPriType;
	}
	else if (((low.tracks[indexOne].pri.accumulatePriStaggerCount > 0)
			&& (low.tracks[indexTwo].pri.accumulatePriStaggerCount > 0))
			|| ((low.tracks[indexOne].pri.accumulatePriStaggerCount > 0)
					&& (low.tracks[indexTwo].pri.priTypeFutureDominant
							== PriTypeEnum::pri_stagger))
			|| ((low.tracks[indexOne].pri.priTypeFutureDominant
					== PriTypeEnum::pri_stagger)
					&& (low.tracks[indexTwo].pri.accumulatePriStaggerCount > 0)))
	{
		priTypeCompatible = 1;
		priTypeScore = ConstHigh::mergeScoreStringPriType;
	}
	else if ((low.tracks[indexOne].pri.accumulatePriStaggerCount > 0)
			&& (low.tracks[indexTwo].pri.accumulatePriFixedCount > 0))
	{
		priTypeCompatible = 1;
		priTypeScore = ConstHigh::mergeScoreStringPriType;
		//if((priInitialScore == 0) && (pwDiff > 10) && (powerDiff > 70))
		//	parameterExclusionStatus = 1;
	}
	else if ((low.tracks[indexOne].pri.accumulatePriFixedCount > 0)
			&& (low.tracks[indexTwo].pri.accumulatePriStaggerCount > 0))
	{
		priTypeCompatible = 1;
		priTypeScore = ConstHigh::mergeScoreStringPriType;
		//if((priInitialScore == 0) && (pwDiff > 10) && (powerDiff > 70))
		//	parameterExclusionStatus = 1;
	}
	else if ((low.tracks[indexOne].pri.accumulatePriDiffToaCount > 0)
			&& (low.tracks[indexTwo].pri.accumulatePriDiffToaCount > 0))
	{
		priTypeCompatible = 1;
		priTypeScore = ConstHigh::mergeScoreStringPriType;
	}
	else if ((low.tracks[indexOne].pri.accumulatePriJitterCount > 0)
			|| (low.tracks[indexOne].pri.accumulatePriSlidingCount > 0)
			|| (low.tracks[indexOne].pri.accumulatePriComplexCount > 0))
	{
		priTypeCompatible = 1;
	}
	else
	{
		priTypeCompatible = 0;
		if (priTypeCompatible == 0)
		{
			if (priInitialScore == 0)
			{
				parameterExclusionStatus = 1;
			}
		}
	}
	//temporary addition, can be tightened additionally
//	if ((angleDiff > ConstHigh::mergeLimitStringBearingPrimary)
//			&& (angleDiffQuadrantSwap
//					> ConstHigh::mergeLimitStringBearingSecondary)
//			&& ((pwDiff > ConstHigh::mergeDeltaStringPwMedium)
//					|| (priInitialScore < 10) || (priTypeCompatible == 0)))
	if ((angleDiff > ConstHigh::mergeLimitStringBearingPrimary)
			&& (angleDiffQuadrantSwap
					> ConstHigh::mergeLimitStringBearingSecondary)
			&& ((pwDiff > ConstHigh::mergeDeltaStringPwMedium)
					|| (priInitialScore < 10) || (priTypeCompatible == 0))
			&& ((pwDiff > ConstHigh::mergeDeltaStringPwSmall)
					|| (priInitialScore < 5)))
	{
		parameterExclusionStatus = 1;
	}
	//end of temporary addition
	if ((priInitialScore == 0) && (pwDiff > ConstHigh::mergeDeltaStringPwMedium)
			&& (low.tracks[indexOne].rf.getBandCodeLegacy() == 2)
			&& (low.tracks[indexTwo].rf.getBandCodeLegacy() == 2)
			&& (low.tracks[indexOne].pri.accumulatePriJitterCount == 0)
			&& (low.tracks[indexOne].pri.accumulatePriComplexCount == 0)
			&& (low.tracks[indexOne].pri.accumulatePriSlidingCount == 0)
			&& (low.tracks[indexTwo].pri.accumulatePriJitterCount == 0)
			&& (low.tracks[indexTwo].pri.accumulatePriComplexCount == 0)
			&& (low.tracks[indexTwo].pri.accumulatePriSlidingCount == 0))
	{
		parameterExclusionStatus = 1;
	}
	if ((priInitialScore < 10) && (pwDiff > ConstHigh::mergeDeltaStringPwMedium)
			&& (powerDiff > ConstHigh::mergeDeltaStringPowerExclusion)/* &&
			 (low.tracks[lowIndex].rf.getBandCodeLegacy() == 2) &&
			 (high.tracks[highIndex].last[0].rf.getBandCodeLegacy() == 2)*/)
	{
		parameterExclusionStatus = 1;
	}
	if ((priInitialScore == 10)
			&& (powerDiff > ConstHigh::mergeDeltaStringPowerExclusion)
			&& (low.tracks[indexOne].rf.getBandCodeLegacy() == 2)
			&& (low.tracks[indexTwo].rf.getBandCodeLegacy() == 2))
	{
		parameterExclusionStatus = 1;
	}

	if ((groundIlluminationStatus == 0) && powerScore > 10 && angleScore == 0
			&& pxDiff > 320 && pyDiff > 320)
	{
		powerScore = 0;
	}

	overallScore = angleScore + bandScore + toaScore + pxScore + pyScore
			+ pwScoreThree + pwScoreTen + powerScore + rfScore
			+ threatLibraryScoreLowBar + threatLibraryScoreHighBar
			+ priTypeScore + priFailedScore + priInitialScore;
	//if(overallScore >= ConstHigh::mergeScoreStringRequiredTotal) {
	//	overallScore = overallScore + 10;
	//}

	low.compareLowLow(indexOne, indexTwo).rfScore = rfScore;
	low.compareLowLow(indexOne, indexTwo).parameterExclusionStatus =
			parameterExclusionStatus;
	low.compareLowLow(indexOne, indexTwo).angleScore = angleScore;
	low.compareLowLow(indexOne, indexTwo).powerScore = powerScore;
	low.compareLowLow(indexOne, indexTwo).pwScoreTen = pwScoreTen;
	low.compareLowLow(indexOne, indexTwo).pwScoreThree = pwScoreThree;
	low.compareLowLow(indexOne, indexTwo).pxScore = pxScore;
	low.compareLowLow(indexOne, indexTwo).pyScore = pyScore;
	low.compareLowLow(indexOne, indexTwo).toaScore = toaScore;
	low.compareLowLow(indexOne, indexTwo).bandScore = bandScore;
	low.compareLowLow(indexOne, indexTwo).priDetailCompareStatus = 0;
	low.compareLowLow(indexOne, indexTwo).priTypeCompatible = priTypeCompatible;
	low.compareLowLow(indexOne, indexTwo).priTypeScore = priTypeScore;
	low.compareLowLow(indexOne, indexTwo).priFailedScore = priFailedScore;
	low.compareLowLow(indexOne, indexTwo).priInitialScore = priInitialScore;
	low.compareLowLow(indexOne, indexTwo).priDetailScore = 0;
	low.compareLowLow(indexOne, indexTwo).threatLibraryScoreLowBar =
			threatLibraryScoreLowBar;
	low.compareLowLow(indexOne, indexTwo).threatLibraryScoreHighBar =
			threatLibraryScoreHighBar;
	low.compareLowLow(indexOne, indexTwo).overallScore = overallScore;
	low.compareLowLow(indexOne, indexTwo).parameterCompareStatus = 1;

	return overallScore;
}

uint8_t mergeFlagStringExcludeHighLow(DataSensorClass &low,
		DataSensorClass &high, uint16_t lowIndex, uint16_t highIndex)
{

	uint16_t rfDiff = 0;
	uint16_t analysisRfExclusionDeltaCalc =
			ConstHigh::mergeDeltaStringRfExcludeLarge;

	if (high.tracks[highIndex].rf.getBandCodeLegacy() != 2)
	{
		analysisRfExclusionDeltaCalc =
				ConstHigh::mergeDeltaStringRfExcludeMedium;
	}

	//exclude based on last track valid rf
	if ((high.tracks[highIndex].rf.accumulateRf.size() > 0)
			&& (low.tracks[lowIndex].rf.accumulateRf.size() > 0))
	{
		rfDiff =
				(int16_t) labs(
						(int32_t) high.tracks[highIndex].rf.accumulateRf.back().rfLow
								- (int32_t) low.tracks[lowIndex].rf.accumulateRf[0].rfLow);
	}
	if (rfDiff > analysisRfExclusionDeltaCalc)
	{
		return 1;
	}
	//excluse based on tl category
	if ((high.tracks[highIndex].libmatch.active.flagKnownLibrary == 1)
			&& (low.tracks[lowIndex].libmatch.active.flagKnownLibrary == 1))
	{
		if (low.tracks[lowIndex].libmatch.active.highestMatchList[0].emitterBatchId
				!= high.tracks[highIndex].libmatch.active.highestMatchList[0].emitterBatchId)
		{
			if ((low.tracks[lowIndex].rf.getBandCodeLegacy() == 2)
					&& (high.tracks[highIndex].last[0].rf.getBandCodeLegacy()
							== 2))
			{
				if (low.tracks[lowIndex].libmatch.active.highestMatchList[0].dwellCount
						> 0
						&& high.tracks[highIndex].libmatch.active.highestMatchList[0].dwellCount
								> 0)
				{
					if (low.tracks[lowIndex].libmatch.active.highestMatchList[0].noOfSpotsMatched
							== low.tracks[lowIndex].libmatch.active.highestMatchList[0].libraryPriCount
							|| low.tracks[lowIndex].libmatch.active.highestMatchList[0].noOfSpotsMatched
									>= 3)
					{
						return 1;
					}
				}
				else
				{
					return 1;
				}
			}
			else
			{
				return 1;
			}
		}
	}
	//exclude based on number of track updates
	if ((low.tracks[lowIndex].pri.accumulatePriComplexCount == 0)
			&& (low.tracks[lowIndex].pri.accumulatePriDiffToaCount == 0))
	{
		if ((low.tracks[lowIndex].history.lowToaStartUpdateFlag == 1)
				&& (high.tracks[highIndex].history.stitchedUpdateCount > 0))
		{
			return 1;
		}
		if ((low.tracks[lowIndex].history.lowToaStartUpdateNextFlag == 1)
				&& (high.tracks[highIndex].history.stitchedUpdateNextCount > 0))
		{
			return 1;
		}
	}
	return 0;
}

void mergeCalcBearingDifferenceLowLow(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo, uint16_t &angleDiff,
		uint16_t &angleDiffQuadrantSwap)
{

	uint16_t bearingCalculatedQuadrantSwap;
	uint16_t smallerAngleValue;
	int16_t aoaDifference;
	uint16_t absDifferenceAoa;

	angleDiff = 360;
	angleDiffQuadrantSwap = 360;

	//calculate normal angle difference. Each report with its own respective quadrants
	aoaDifference = (trackOne.angle.bearingLast - trackTwo.angle.bearingLast);
	absDifferenceAoa = labs((int16_t) aoaDifference);
	smallerAngleValue = doaRoundAngleDifference(absDifferenceAoa);
	angleDiff = smallerAngleValue;

	//certain rules must be followed for us to have a valid quadrant shifted angle difference
	if (trackOne.power.quadrantCodeMax != trackTwo.power.quadrantCodeMax)
	{
		if (angleDiff < 92)
		{	//previous was 90
			if (doaFlagPxPyCompatibleCheck(trackOne.power.pxMax,
					trackTwo.power.pxMax, trackOne.power.pyMax,
					trackTwo.power.pyMax) == 1)
			{
				if (doaFlagQuadrantAdjacencyCheck(
						trackOne.power.quadrantCodeMax,
						trackTwo.power.quadrantCodeMax) == 1)
				{
					//use target report quadrant code to calculate angle for the source report
					bearingCalculatedQuadrantSwap = doaCalculateAngle(
							trackOne.power.pxMax, trackOne.power.pyMax,
							trackTwo.power.quadrantCodeMax,
							trackOne.rf.getBandCodeLegacy(),
							Constant::systemBandCountOperate);

					aoaDifference = (bearingCalculatedQuadrantSwap
							- trackTwo.angle.bearingLast);
					absDifferenceAoa = labs((int16_t) aoaDifference);
					smallerAngleValue = doaRoundAngleDifference(
							absDifferenceAoa);
					angleDiffQuadrantSwap = smallerAngleValue;
				}
			}
		}
	}
}

void mergeCalcBearingDifferenceHighLow(TrackDefaultClass &low,
		TrackDefaultClass &high, uint16_t &angleDiff,
		uint16_t &angleDiffQuadrantSwap)
{

	uint16_t bearingCalculatedQuadrantSwap;
	uint16_t smallerAngleValue;
	int16_t aoaDifference;
	uint16_t absDifferenceAoa;

	//if(low.general.toaLast == 130853151030) {
	//	low.general.toaLast = 130853151030;
	//}

	angleDiff = 360;
	angleDiffQuadrantSwap = 360;

	//calculate normal angle difference. Each report with its own respective quadrants
	aoaDifference = (low.angle.bearingLast - high.angle.bearingLast);
	absDifferenceAoa = labs((int16_t) aoaDifference);
	smallerAngleValue = doaRoundAngleDifference(absDifferenceAoa);
	angleDiff = smallerAngleValue;

	//certain rules must be followed for us to have a valid quadrant shifted angle difference
	if (low.power.quadrantCodeMax != high.power.quadrantCodeMax)
	{
		if (angleDiff < 92)
		{	//previous was 90
			if (doaFlagPxPyCompatibleCheck(low.power.pxMax, high.power.pxMax,
					low.power.pyMax, high.power.pyMax) == 1)
			{
				if (doaFlagQuadrantAdjacencyCheck(low.power.quadrantCodeMax,
						high.power.quadrantCodeMax) == 1)
				{
					//use target report quadrant code to calculate angle for the source report
					bearingCalculatedQuadrantSwap = doaCalculateAngle(
							high.power.pxMax, high.power.pyMax,
							low.power.quadrantCodeMax,
							low.rf.getBandCodeLegacy(),
							Constant::systemBandCountOperate);

					aoaDifference = (bearingCalculatedQuadrantSwap
							- low.angle.bearingLast);
					absDifferenceAoa = labs((int16_t) aoaDifference);
					smallerAngleValue = doaRoundAngleDifference(
							absDifferenceAoa);
					angleDiffQuadrantSwap = smallerAngleValue;
				}
			}
		}
	}
}

void mergeCalcBearingDifferenceHighHigh(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo, uint16_t &angleDiff,
		uint16_t &angleDiffQuadrantSwap)
{

	uint16_t bearingCalculatedQuadrantSwap;
	uint16_t smallerAngleValue;
	int16_t aoaDifference;
	uint16_t absDifferenceAoa;

	angleDiff = 360;
	angleDiffQuadrantSwap = 360;

	//calculate normal angle difference. Each report with its own respective quadrants
	aoaDifference = (trackOne.last[0].angle.bearingLast
			- trackTwo.last[0].angle.bearingLast);
	absDifferenceAoa = labs((int16_t) aoaDifference);
	smallerAngleValue = doaRoundAngleDifference(absDifferenceAoa);
	angleDiff = smallerAngleValue;

	//certain rules must be followed for us to have a valid quadrant shifted angle difference
	if (trackOne.power.quadrantCodeMax != trackTwo.power.quadrantCodeMax)
	{
		if (angleDiff < 90)
		{
			if (doaFlagPxPyCompatibleCheck(trackOne.last[0].power.pxMax,
					trackTwo.last[0].power.pxMax, trackOne.last[0].power.pyMax,
					trackTwo.last[0].power.pyMax) == 1)
			{
				if (doaFlagQuadrantAdjacencyCheck(
						trackOne.last[0].power.quadrantCodeMax,
						trackTwo.last[0].power.quadrantCodeMax) == 1)
				{

					if (trackTwo.general.toaLast > trackOne.general.toaLast)
					{
						//use target report quadrant code to calculate angle for the source report
						bearingCalculatedQuadrantSwap = doaCalculateAngle(
								trackOne.last[0].power.pxMax,
								trackOne.last[0].power.pyMax,
								trackTwo.last[0].power.quadrantCodeMax,
								trackOne.rf.getBandCodeLegacy(),
								Constant::systemBandCountOperate);

						aoaDifference = (bearingCalculatedQuadrantSwap
								- trackTwo.angle.bearingLast);
						absDifferenceAoa = labs((int16_t) aoaDifference);
						smallerAngleValue = doaRoundAngleDifference(
								absDifferenceAoa);
						angleDiffQuadrantSwap = smallerAngleValue;
					}
					else
					{
						//use source report quadrant code to calculate angle for the target report
						bearingCalculatedQuadrantSwap = doaCalculateAngle(
								trackTwo.last[0].power.pxMax,
								trackTwo.last[0].power.pyMax,
								trackOne.last[0].power.quadrantCodeMax,
								trackTwo.rf.getBandCodeLegacy(),
								Constant::systemBandCountOperate);

						aoaDifference = (bearingCalculatedQuadrantSwap
								- trackOne.angle.bearingLast);
						absDifferenceAoa = labs((int16_t) aoaDifference);
						smallerAngleValue = doaRoundAngleDifference(
								absDifferenceAoa);
						angleDiffQuadrantSwap = smallerAngleValue;
					}
				}
			}
		}
	}
}

void mergeCalcOverlapLowLow(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo, uint32_t &pulseOverlap,
		uint64_t &toaOverlap)
{

	//uint16_t pwDiff;
	uint64_t totalToa;

	if ((trackOne.general.toaFirstOverlapAnalysis
			< trackTwo.general.toaFirstOverlapAnalysis)
			&& (trackOne.general.toaLast > trackTwo.general.toaLast))
	{
		toaOverlap = trackTwo.general.toaLast
				- trackTwo.general.toaFirstOverlapAnalysis;
		pulseOverlap = trackTwo.power.pulseCountSinceLastReport;
	}
	else if ((trackOne.general.toaFirstOverlapAnalysis
			> trackTwo.general.toaFirstOverlapAnalysis)
			&& (trackOne.general.toaLast < trackTwo.general.toaLast))
	{
		toaOverlap = trackOne.general.toaLast
				- trackOne.general.toaFirstOverlapAnalysis;
		totalToa = trackTwo.general.toaLast
				- trackTwo.general.toaFirstOverlapAnalysis;
		pulseOverlap = (toaOverlap * trackTwo.power.pulseCountSinceLastReport)
				/ totalToa;
	}
	else if ((trackOne.general.toaLast
			> trackTwo.general.toaFirstOverlapAnalysis)
			&& (trackOne.general.toaLast < trackTwo.general.toaLast))
	{
		toaOverlap = trackOne.general.toaLast
				- trackTwo.general.toaFirstOverlapAnalysis;
		totalToa = trackTwo.general.toaLast
				- trackTwo.general.toaFirstOverlapAnalysis;
		pulseOverlap = (toaOverlap * trackTwo.power.pulseCountSinceLastReport)
				/ totalToa;
	}
	else if ((trackOne.general.toaFirstOverlapAnalysis
			< trackTwo.general.toaLast)
			&& (trackOne.general.toaLast > trackTwo.general.toaLast))
	{
		toaOverlap = trackTwo.general.toaLast
				- trackOne.general.toaFirstOverlapAnalysis;
		totalToa = trackTwo.general.toaLast
				- trackTwo.general.toaFirstOverlapAnalysis;
		pulseOverlap = (toaOverlap * trackTwo.power.pulseCountSinceLastReport)
				/ totalToa;
	}
	else
	{
		toaOverlap = 1;
		pulseOverlap = 0;
		//#if SIMULATION_MODE_DISABLED == 0
		//printf("Overlap Analysis Failure: %"PRIu64" %"PRIu64"\n",pEmitterTagSource->timeOfArrivalStartOverlap,pEmitterTagTarget->timeOfArrivalStartOverlap);
		//#endif
	}
}

void mergeCalcOverlapHighLow(TrackDefaultClass &trackOne,
		TrackDefaultClass &trackTwo, uint32_t &pulseOverlap,
		uint64_t &toaOverlap)
{

	if ((trackTwo.general.flagProbation == 0)
			&& (trackTwo.general.timeIterCountLifetimeGlobal > 0))
	{
		if (trackOne.rf.bandCode == trackTwo.rf.bandCode)
		{
			pulseOverlap = 0;	//this is temporary and calculation is pending
			toaOverlap = 1;		//this is temporary and calculation is pending
		}
	}
}
