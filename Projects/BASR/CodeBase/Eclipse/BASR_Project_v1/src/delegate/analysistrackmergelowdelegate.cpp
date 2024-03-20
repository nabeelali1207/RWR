/*
 * analysistrackmergelowdelegate.cpp
 *
 *  Created on: Nov 20, 2018
 *      Author: hassan
 */

#define ANALYSISTRACKMERGELOWDELEGATE_CPP_

#include "analysistrackmergelowdelegate.h"

TrackTimeWindowCompatibilty checkTracksStatusInTime(TrackDefaultClass &source,
		TrackDefaultClass &target)
{

	TrackTimeWindowCompatibilty trackWindowType;
	int64_t timeDiff;
	uint32_t blankingWindow = source.pri.maxPriSpot * 1000;
	blankingWindow = max(blankingWindow, (uint32_t) 1000000);
	blankingWindow = min(blankingWindow, ConstLow::asStringingMaxTimeWindow);

	timeDiff = (int64_t) target.general.toaFirst
			- (int64_t) source.general.toaLast;

	if (timeDiff < 0)
	{
		if (abs(timeDiff) < source.pri.maxPriSpot * 5)
			trackWindowType = TrackTimeWindowCompatibilty::slight_overlap;
		else
			trackWindowType = TrackTimeWindowCompatibilty::overlapping;
	}
	else if (timeDiff < source.pri.maxPriSpot * 20)
	{
		trackWindowType = TrackTimeWindowCompatibilty::without_blanking;
	}

	else if (timeDiff < blankingWindow/*ConstLow::asStringingMaxTimeWindow*/)
	{
		trackWindowType = TrackTimeWindowCompatibilty::blanking;
	}
	else
	{
		trackWindowType = TrackTimeWindowCompatibilty::out_Of_Window;
	}
	return trackWindowType;
}

bool doAnglesMatch(TrackDefaultClass &source, TrackDefaultClass &target)
{

	int16_t angleDiff = abs(
			(int16_t) source.angle.curAngle - (int16_t) target.angle.curAngle);
	angleDiff = (angleDiff > 180 ? 360 - angleDiff : angleDiff);

	if (angleDiff <= 12)
	{
		return true;
	}
	return false;
}
bool doPwMatch(TrackDefaultClass &source, TrackDefaultClass &target)
{
	int16_t matchingSpotCount = 0;

//	AnalysisTrackPwLowClass &sourcePwObj = source.pw.pwTrackingObj[0];
//	AnalysisTrackPwLowClass &targetPwObj = target.pw.pwTrackingObj[0];
//
//	if (sourcePwObj.currPwType != SequenceTypesEnum::seq_unknown
//			&& sourcePwObj.currPwType == targetPwObj.currPwType)
//	{
//
//		for (uint16_t i = 0; i < sourcePwObj.pwSpots.size(); i++)
//		{
//			for (uint16_t j = 0; j < targetPwObj.pwSpots.size(); j++)
//			{
//				int32_t diff = (int32_t) sourcePwObj.pwSpots[i]
//						- (int32_t) targetPwObj.pwSpots[j];
//				diff = abs(diff);
//				if (diff <= (int32_t) sourcePwObj.deviationAllowed[i])
//				{
//					matchingSpotCount++;
//				}
//			}
//		}
//	}
	TrackPwClass &sourcePwObj = source.pw;
	TrackPwClass &targetPwObj = target.pw;

	if (sourcePwObj.curPwType != PwTypeEnum::pw_uncomputed
			&& sourcePwObj.curPwType == targetPwObj.curPwType)
	{

		for (uint16_t i = 0; i < sourcePwObj.pwCircular.size(); i++)
		{
			for (uint16_t j = 0; j < targetPwObj.pwCircular.size(); j++)
			{
				int32_t diff = (int32_t) sourcePwObj.pwCircular[i].pwAvg
						- (int32_t) targetPwObj.pwCircular[j].pwAvg;
				diff = abs(diff);
				if (diff <= 20)
				{
					matchingSpotCount++;
				}
			}
		}
	}
	if (matchingSpotCount > 0)
	{
		return true;
	}
	return false;
}
bool doPriTypesMatch(TrackDefaultClass &source, TrackDefaultClass &target)
{
	AnalysisTrackPriLowClass &sourcePriObj = source.pri.priTrackingObj[0];
	AnalysisTrackPriLowClass &targetPriObj = target.pri.priTrackingObj[0];

	if (sourcePriObj.currPriType != SequenceTypesEnum::seq_unknown
			&& sourcePriObj.currPriType == targetPriObj.currPriType)
	{
		return true;
	}
	return false;
}
bool doPriSpotsMatch(TrackDefaultClass &source, TrackDefaultClass &target)
{
	int16_t matchingSpotCount = 0;

	AnalysisTrackPriLowClass &sourcePriObj = source.pri.priTrackingObj[0];
	AnalysisTrackPriLowClass &targetPriObj = target.pri.priTrackingObj[0];

	switch (sourcePriObj.currPriType)
	{
	case SequenceTypesEnum::seq_dwell:
	{
		if (targetPriObj.currPriType == SequenceTypesEnum::seq_dwell)
		{
			vector<int32_t> &sourceSpots = sourcePriObj.GetDwellSpots();
			vector<int32_t> &targetSpots = targetPriObj.GetDwellSpots();
			for (uint16_t i = 0; i < sourceSpots.size(); i++)
			{
				for (uint16_t j = 0; j < targetSpots.size(); j++)
				{
					int32_t diff = (int32_t) sourceSpots[i]
							- (int32_t) targetSpots[j];
					diff = abs(diff);
					if (diff <= 33)
					{
						matchingSpotCount++;
					}
				}
			}
		}
		break;
	}
	case SequenceTypesEnum::seq_stagger:
	{
		if (targetPriObj.currPriType == SequenceTypesEnum::seq_stagger)
		{
			vector<int32_t> &sourceSpots = sourcePriObj.GetStaggerSpots();
			vector<int32_t> &targetSpots = targetPriObj.GetStaggerSpots();
			for (uint16_t i = 0; i < sourceSpots.size(); i++)
			{
				for (uint16_t j = 0; j < targetSpots.size(); j++)
				{
					int32_t diff = (int32_t) sourceSpots[i]
							- (int32_t) targetSpots[j];
					diff = abs(diff);
					if (diff <= 33)
					{
						matchingSpotCount++;
					}
				}
			}
		}
		else if (targetPriObj.currPriType == SequenceTypesEnum::seq_difftoa)
		{
			vector<int32_t> &sourceSpots = sourcePriObj.GetPriSpots();
			vector<int32_t> &targetSpots = targetPriObj.GetPriSpots();
			for (uint16_t i = 0; i < sourceSpots.size(); i++)
			{
				for (uint16_t j = 0; j < targetSpots.size(); j++)
				{
					int32_t diff = (int32_t) sourceSpots[i]
							- (int32_t) targetSpots[j];
					diff = abs(diff);
					if (diff <= 33)
					{
						matchingSpotCount++;
					}
				}
			}
		}
		break;
	}
	case SequenceTypesEnum::seq_difftoa:
	{
		if (targetPriObj.currPriType == SequenceTypesEnum::seq_difftoa)
		{
			vector<int32_t> &sourceSpots = sourcePriObj.GetPriSpots();
			vector<int32_t> &targetSpots = targetPriObj.GetPriSpots();
			for (uint16_t i = 0; i < sourceSpots.size(); i++)
			{
				for (uint16_t j = 0; j < targetSpots.size(); j++)
				{
					int32_t diff = (int32_t) sourceSpots[i]
							- (int32_t) targetSpots[j];
					diff = abs(diff);
					if (diff <= 33)
					{
						matchingSpotCount++;
					}
				}
			}
		}
		else if (targetPriObj.currPriType == SequenceTypesEnum::seq_stagger)
		{
			vector<int32_t> &sourceSpots = sourcePriObj.GetPriSpots();
			vector<int32_t> &targetSpots = targetPriObj.GetStaggerSpots();
			for (uint16_t i = 0; i < sourceSpots.size(); i++)
			{
				for (uint16_t j = 0; j < targetSpots.size(); j++)
				{
					int32_t diff = (int32_t) sourceSpots[i]
							- (int32_t) targetSpots[j];
					diff = abs(diff);
					if (diff <= 33)
					{
						matchingSpotCount++;
					}
				}
			}
		}
		break;
	}
	case SequenceTypesEnum::seq_jitter:
	{

		if (targetPriObj.currPriType == SequenceTypesEnum::seq_jitter)
		{
			vector<int32_t> &sourceSpots = sourcePriObj.GetPriSpots();
			vector<int32_t> &targetSpots = targetPriObj.GetPriSpots();
			for (uint16_t i = 0; i < sourceSpots.size(); i++)
			{
				for (uint16_t j = 0; j < targetSpots.size(); j++)
				{
					int32_t diff = (int32_t) sourceSpots[i]
							- (int32_t) targetSpots[j];
					diff = abs(diff);
					if (diff <= 33)
					{
						matchingSpotCount++;
					}
				}
			}
		}
		break;
	}
	case SequenceTypesEnum::seq_complex:
	{
		if (targetPriObj.currPriType == SequenceTypesEnum::seq_complex)
		{
			vector<int32_t> &sourceSpots = sourcePriObj.GetPriSpots();
			vector<int32_t> &targetSpots = targetPriObj.GetPriSpots();
			for (uint16_t i = 0; i < sourceSpots.size(); i++)
			{
				for (uint16_t j = 0; j < targetSpots.size(); j++)
				{
					int32_t diff = (int32_t) sourceSpots[i]
							- (int32_t) targetSpots[j];
					diff = abs(diff);
					if (diff <= 33)
					{
						matchingSpotCount++;
					}
				}
			}
		}
		break;
	}
	default:
		break;
	}

	if (matchingSpotCount > 0)
		return true;
	return false;
}
bool doPowersOfDominantAntennasMatch(TrackDefaultClass &source,
		TrackDefaultClass &target)
{

	int16_t allowedDeviation = 0;
	uint16_t sourceAveragePowers[4] =
	{ source.power.avgPA, source.power.avgPB, source.power.avgPC,
			source.power.avgPD };
	uint16_t sourceLastPowers[4] =
	{ source.power.lastPa, source.power.lastPb, source.power.lastPc,
			source.power.lastPd };
	uint16_t targetAveragePowers[4] =
	{ target.power.avgPA, target.power.avgPB, target.power.avgPC,
			target.power.avgPD };

	if (source.power.dominantAntenna == target.power.dominantAntenna)
	{

	}
	else if (source.power.pxAntenna == target.power.pxAntenna
			&& source.power.pyAntenna == target.power.pyAntenna)
	{

	}
	else
	{
		return false;
	}
	float diff = 0;
	diff +=
			source.power.weightsVector[source.power.pxAntenna]
					* abs(
							(int16_t) sourceAveragePowers[source.power.pxAntenna]
									- (int16_t) targetAveragePowers[target.power.pxAntenna]);
	diff +=
			source.power.weightsVector[source.power.pyAntenna]
					* abs(
							(int16_t) sourceAveragePowers[source.power.pyAntenna]
									- (int16_t) targetAveragePowers[target.power.pyAntenna]);

	diff = abs(diff);
	allowedDeviation = 2 * ConstLow::asPowerDefualtVariation;
	if (diff < allowedDeviation)
	{
		return true;
	}
	diff = 0;
	diff += source.power.weightsVector[source.power.pxAntenna]
			* abs(
					(int16_t) sourceLastPowers[source.power.pxAntenna]
							- (int16_t) target.power.pxValueFirst);
	diff += source.power.weightsVector[source.power.pyAntenna]
			* abs(
					(int16_t) sourceLastPowers[source.power.pyAntenna]
							- (int16_t) target.power.pyValueFirst);

	diff = abs(diff);
	if (diff < allowedDeviation)
	{
		return true;
	}
	return false;
}
int16_t findIfTwoTracksShouldString(vector<TrackDefaultClass> &tracks,
		int16_t sourceIndex, int16_t targetIndex)
{
	int16_t matchIndex = -1;
	TrackDefaultClass &source = tracks[sourceIndex];
	TrackDefaultClass &target = tracks[targetIndex];
	TrackTimeWindowCompatibilty trackStatus = checkTracksStatusInTime(source,
			target);
	int8_t rfCompatibility = trackCompareBandWidth(source.rf.rfTrackingObj[0].rfMedian,
			target.rf.rfTrackingObj[0].rfMedian);
	if (rfCompatibility == -1)
		return -1;
	if (trackStatus == TrackTimeWindowCompatibilty::overlapping)
	{
		return -1;
	}
	else if (trackStatus == TrackTimeWindowCompatibilty::out_Of_Window)
	{
		return -1;
	}

	bool angleMatchFlag = false;
	bool powersMatchFlag = false;
	bool pwMatchFlag = false;
	bool priTypeMatchFlag = false;
	bool priSpotMatchFlag = false;

	angleMatchFlag = doAnglesMatch(source, target);
	powersMatchFlag = doPowersOfDominantAntennasMatch(source, target);
	if (angleMatchFlag || powersMatchFlag)
	{
		priTypeMatchFlag = doPriTypesMatch(source, target);
		priSpotMatchFlag = doPriSpotsMatch(source, target);
		pwMatchFlag = doPwMatch(source, target);
		if (priTypeMatchFlag == true)
		{
			if (priSpotMatchFlag == true)
			{
				return sourceIndex;
			}
			else if (pwMatchFlag == true)
			{
				return sourceIndex;
			}
			else if (angleMatchFlag && powersMatchFlag)
			{
				if (trackStatus == TrackTimeWindowCompatibilty::slight_overlap)
					return -1;
				matchIndex = findOtherStringingMatch(tracks, targetIndex,
						sourceIndex);
				return matchIndex;
			}
		}
	}
	return -1;
}
int16_t findOtherStringingMatch(vector<TrackDefaultClass> &tracks,
		int16_t targetIndex, int16_t sourceIndex)
{
	bool angleMatch = false;
	bool powersMatch = false;
	bool pwMatch = false;
	bool priTypeMatch = false;
	bool priSpotMatch = false;
	TrackDefaultClass &target = tracks[targetIndex];
	int16_t trackWithpriMatch = -1;
	int16_t trackWithpwMatch = -1;
	int16_t trackWithpriTypeMatch = -1;
	for (int16_t i = sourceIndex - 1; i >= 0; i--)
	{
		TrackDefaultClass &otherSource = tracks[i];
		bool otherSourcePossible = (otherSource.general.dspTrackingEnabled
				== false);
		otherSourcePossible = (otherSourcePossible
				&& (otherSource.pri.curPriType != PriTypeEnum::pri_uncomputed));
		//source is string-able only when its has become silent
		if (otherSource.general.dspDeleteTrack == true
				|| otherSourcePossible == false)
		{
			continue;
		}
		int8_t rfCompatibility = trackCompareBandWidth(otherSource.rf.rfTrackingObj[0].rfMedian,
			target.rf.rfTrackingObj[0].rfMedian);
		if (rfCompatibility == -1)
			continue;
		TrackTimeWindowCompatibilty OtherTrackStatus = checkTracksStatusInTime(
				otherSource, target);
		if (OtherTrackStatus == TrackTimeWindowCompatibilty::blanking
				|| OtherTrackStatus
						== TrackTimeWindowCompatibilty::without_blanking)
		{
			angleMatch = doAnglesMatch(otherSource, target);

			powersMatch = doPowersOfDominantAntennasMatch(otherSource, target);
			if (angleMatch || powersMatch)
			{
				priTypeMatch = doPriTypesMatch(otherSource, target);
				priSpotMatch = doPriSpotsMatch(otherSource, target);
				pwMatch = doPwMatch(otherSource, target);
				if (trackWithpriTypeMatch == -1)
				{
					if (priTypeMatch == true)
						trackWithpriTypeMatch = i;
				}
				if (trackWithpwMatch == -1)
				{
					if (pwMatch == true)
						trackWithpwMatch = i;
				}
				if (pwMatch == true && priSpotMatch == true)
				{
					trackWithpriMatch = i;
					trackWithpwMatch = i;
				}
				else if (pwMatch == true && priTypeMatch == true)
				{
					trackWithpwMatch = i;
					trackWithpriTypeMatch = i;
				}
				if (trackWithpriMatch == -1)
				{
					if (priSpotMatch == 1)
						trackWithpriMatch = i;
				}
			}
		}
	}
	if (trackWithpriMatch != -1)
	{
		return trackWithpriMatch;
	}
	else if (trackWithpwMatch != -1
			&& trackWithpwMatch == trackWithpriTypeMatch)
	{
		return trackWithpwMatch;
	}
	else if (trackWithpriTypeMatch != -1)
	{
		return -2;
	}
	else
	{
		return sourceIndex;
	}
}
void mergeAndUpdateTracks(TrackDefaultClass &source, TrackDefaultClass &target)
{

	source.angle = target.angle;
	source.audio = target.audio;
	source.cfd = target.cfd;
	source.history = target.history;
	source.intrapulse = target.intrapulse;
	source.jamming = target.jamming;
	source.libmatch = target.libmatch;
	source.platform = target.platform;
	source.range = target.range;
	source.release = target.release;

//low variables
	for (uint16_t i = 0; i < target.general.dspPdwBuffer.size(); i++)
	{
		PdwDefaultClass &tempPdw = target.general.dspPdwBuffer[i];
		if (tempPdw.timeOfArrivalStart >= target.general.toaFirst)
		{
			source.general.dspPdwBuffer.push_back(tempPdw);
			source.power.powerHistogram[0].addToHistogram(
					tempPdw.powerX + tempPdw.powerY);
			source.power.scanAnalysis[0].FillScanBuffer(tempPdw);
		}

	}
	source.general.dspIterationPulseCount =
			target.general.dspIterationPulseCount;
	source.general.dspTrackingEnabled = target.general.dspTrackingEnabled;
	source.general.dspCandidateForStringingFlag = false;
	source.general.dspDeleteTrack = target.general.dspDeleteTrack;
	source.general.updateOffFlag = target.general.updateOffFlag;

	source.general.timeToDeclareSilent = max(source.general.timeToDeclareSilent,
			target.general.timeToDeclareSilent);
	source.general.iterSysLastUpdate = target.general.iterSysLastUpdate;
	source.general.timeToaLastUpdate = target.general.timeToaLastUpdate;
	source.general.timeSysLastUpdate = target.general.timeSysLastUpdate;
	source.general.timeSysCreation = target.general.timeSysCreation;
	source.general.toaLast = target.general.toaLast;
	source.general.toaFirstOverlapAnalysis =
			target.general.toaFirstOverlapAnalysis;
	target.power.pulseCountSinceLastScanAnalysis =
			target.power.pulseCountSinceLastScanAnalysis
					+ source.power.pulseCountSinceLastScanAnalysis;
	if ((target.power.pxMax + target.power.pyMax)
			< (source.power.pxMax + source.power.pyMax))
	{
		target.power.pxMax = source.power.pxMax;
		target.power.pyMax = source.power.pyMax;
	}

	source.power.copyParametersFromStringedEmitter(target.power);

	source.pri.priTrackingObj[0] = target.pri.priTrackingObj[0];

	for (uint16_t i = 0; i < target.pri.priCircular.size(); i++)
	{
		source.pri.priCircular.push_back(target.pri.priCircular[i]);
	}
	source.pri.toaBufferSizeFirst = target.pri.toaBufferSizeFirst;
	source.pri.lastPriAnalysisPulseCount = target.pri.lastPriAnalysisPulseCount;
	source.pri.toaBufferSizeLast = target.pri.toaBufferSizeLast;
	source.pri.lastPriAnalysisSpotCount = target.pri.lastPriAnalysisSpotCount;
	source.pri.priTrackingPossible = target.pri.priTrackingPossible;
	source.pri.curPriSpot = target.pri.curPriSpot;
	source.pri.maxPriSpot = max(source.pri.maxPriSpot, target.pri.maxPriSpot);
	source.pri.curExpectPriSpot = target.pri.curExpectPriSpot;
	source.pri.curPriType = target.pri.curPriType;

	source.pri.countPdwAnalysisLast = target.pri.countPdwAnalysisLast;
	source.pri.countPdwAnalysisWindow = target.pri.countPdwAnalysisWindow;
	source.pri.timeToaAnalysisLast = target.pri.timeToaAnalysisLast;
	source.pri.diffToaAnalysisWindow = target.pri.diffToaAnalysisWindow;

	for (uint16_t i = 0; i < target.pw.pwCircular.size(); i++)
	{
		source.pw.pwCircular.push_back(target.pw.pwCircular[i]);
	}

	source.pw.lastAnalysisSpotCount = target.pw.lastAnalysisSpotCount;
	source.pw.curPwType = target.pw.curPwType;
	source.pw.pwTrackingObj[0] = target.pw.pwTrackingObj[0];
	source.pw.timeToaPwAnl = target.pw.timeToaPwAnl;

	for (uint16_t i = 0; i < target.rf.rfCircular.size(); i++)
	{
		source.rf.rfCircular.push_back(target.rf.rfCircular[i]);
	}
	source.rf.bandCode = target.rf.bandCode;
	source.rf.rfTrackingPossible = target.rf.rfTrackingPossible;
	source.rf.curRfType = target.rf.curRfType;
	source.rf.timeToaRfAnl = target.rf.timeToaRfAnl;
	source.rf.lastAnalysisSpotCount = target.rf.lastAnalysisSpotCount;

	source.rf.rfTrackingObj[0] = target.rf.rfTrackingObj[0];

}
void mergeAndUpdateTracksV2(TrackDefaultClass &source,
		TrackDefaultClass &target)
{
	EmitterArpEnum sourceScanType;
	source.general.dspTrackingEnabled = target.general.dspTrackingEnabled;
	source.general.dspCandidateForStringingFlag = false;
	source.general.dspDeleteTrack = false;
	source.general.updateOffFlag = target.general.updateOffFlag;
	sourceScanType = source.power.scanTypeLast;
	for (uint16_t i = 0; i < target.general.dspPdwBuffer.size(); i++)
	{

//		if (target.general.dspPdwBuffer[i].timeOfArrivalStart
//				<= source.general.dspPdwBuffer.back().timeOfArrivalStart)
//		{
//			cout << "hi -debug\n";
//			continue;
//		}
		if (target.general.dspPdwBuffer[i].timeOfArrivalStart
				>= target.general.toaFirst)
		{
			trackAssociateNewPdwWithTrack(source,
					target.general.dspPdwBuffer[i],
					Model::countThreadMainIterLow);
		}
	}
	if (source.power.scanTypeLast == EmitterArpEnum::scan_unknown)
		source.power.scanTypeLast = sourceScanType; // retain last scan type if new is unknown

}
void sortTracksBasedOnToa(vector<TrackDefaultClass> &tracks)
{
	vector<TrackDefaultClass> sortedTracks;
	sortedTracks.reserve(tracks.size());
	typedef std::pair<uint64_t, uint64_t> myPair;
	vector<myPair> powers(tracks.size());
	for (uint16_t i = 0; i < tracks.size(); i++)
	{
		powers[i].first = tracks[i].general.toaLast;
		powers[i].second = i;
	}
	sort(powers.begin(), powers.end(), [](const myPair &l, const myPair &r)
	{	return l.first<r.first;});
	for (uint16_t i = 0; i < tracks.size(); i++)
	{
		sortedTracks.push_back(tracks[powers[i].second]);
	}
	tracks = sortedTracks;
}
//int8_t trackCompareBandWidth(uint16_t rf, uint16_t trackRf)
//{
//	uint16_t diff = (trackRf > rf) ? trackRf - rf : rf - trackRf;
//
//	if (diff <= ConstLow::asTrackingRfWindow)
//		return 1;
//	return -1;
//}
void stringTracksAllLow(DataSensorClass &bandObj)
{
	/* This function does stringing only */
	/* 1. sort all tracks time wise
	 * 2. Find first track that can be strung
	 *		2.1 find target track
	 *		2.2 find source track (with which target track will be merged/strung)
	 * 3. Find AOA overlap
	 * 4. Check PW match (NxN)
	 *
	 */
	if (bandObj.stringableTrackCount < 1)
	{
		return;
	}
	vector<TrackDefaultClass> &tracks = bandObj.tracks;
	vector<int16_t> mergeFlag;
	mergeFlag.resize(tracks.size(), -1);

//1. sort tracks based on last toa
	//sort(tracks.begin(), tracks.end(),TrackDefaultClass::compareTrackToas);
	//return;
	sortTracksBasedOnToa(tracks);
//	sort(tracks.begin(), tracks.end(),
//			[](const TrackDefaultClass &first, const TrackDefaultClass &second)
//			{	return first.general.toaLast<second.general.toaLast;});

//2. select 2 targets for stringing
	uint16_t sourceTrackIndex = 0;
	uint16_t targetTrackIndex = 0;

	for (uint16_t i = 1; i < tracks.size(); i++)
	{
		targetTrackIndex = i;
		bool targetStringable =
				(tracks[targetTrackIndex].general.dspCandidateForStringingFlag
						== true);
//		if (tracks[targetTrackIndex].general.toaFirst == 151369285655)
//			tracks[targetTrackIndex].general.toaFirst =
//					tracks[targetTrackIndex].general.toaFirst * 1;
		if (mergeFlag[targetTrackIndex] != -1 || targetStringable == false)
		{
			continue;
		}
		if (tracks[targetTrackIndex].general.toaFirst == 1345137600)
			tracks[targetTrackIndex].general.toaFirst =
					tracks[targetTrackIndex].general.toaFirst * 1;
		for (int16_t j = targetTrackIndex - 1; j >= 0; j--)
		{
			sourceTrackIndex = j;
			bool sourceStringAble =
					(tracks[sourceTrackIndex].general.dspTrackingEnabled
							== false);
			sourceStringAble = (sourceStringAble
					&& (tracks[sourceTrackIndex].pri.curPriType
							!= PriTypeEnum::pri_uncomputed));
			//source is string-able only when its has become silent
			if (mergeFlag[sourceTrackIndex] != -1 || sourceStringAble == false)
			{
				continue;
			}

			int16_t mergeTrackId = findIfTwoTracksShouldString(tracks,
					sourceTrackIndex, targetTrackIndex);

			if (mergeTrackId > -1)
			{
//				cout << "stringing tracks "
//						<< tracks[sourceTrackIndex].general.trackID << " with "
//						<< tracks[targetTrackIndex].general.trackID << endl;
//				cout << tracks[sourceTrackIndex].general.toaFirst << " - "
//						<< tracks[sourceTrackIndex].general.toaLast;
//				cout << "  " << tracks[targetTrackIndex].general.toaFirst << " - "
//						<< tracks[targetTrackIndex].general.toaLast << endl;
				mergeFlag[targetTrackIndex] = mergeTrackId;
				mergeAndUpdateTracks(tracks[mergeTrackId],
						tracks[targetTrackIndex]);
//				mergeAndUpdateTracksV2(tracks[mergeTrackId],
//						tracks[targetTrackIndex]);
				tracks[sourceTrackIndex].general.totalStringTracksCount++;
				tracks[sourceTrackIndex].general.iterStringTracksCount++;
				tracks[targetTrackIndex].general.dspDeleteTrack = true;
				break;
			}
			else if (mergeTrackId == -2)
			{
				tracks[targetTrackIndex].general.dspCandidateForStringingFlag =
						false;
				break;
			}
		}
		if (mergeFlag[targetTrackIndex] == -1)
		{
			if ((tracks[targetTrackIndex].general.toaLast
					- tracks[targetTrackIndex].pri.lastPriAnalysistime)
					> 20 * tracks[targetTrackIndex].pri.maxPriSpot)
			{
				tracks[targetTrackIndex].general.dspCandidateForStringingFlag =
						false;
			}
		}
	}

	trackDeleteInactiveTracks(tracks);
	trackCountStringableTracks(tracks, bandObj.stringableTrackCount);

}
