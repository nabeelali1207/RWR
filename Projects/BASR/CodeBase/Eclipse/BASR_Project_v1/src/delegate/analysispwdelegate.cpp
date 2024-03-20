/*
 * analysispwdelegate.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#define ANALYSISPWDELEGATE_CPP_

#include "../delegate/analysispwdelegate.h"

void pwUpdateActiveSpotInfoForAllTracks(vector<TrackDefaultClass> &tracks)
{
	// this function copies the updated pulse width spot info from pwTrackingObject to pwObject
	for (uint16_t i = 0; i < tracks.size(); i++)
	{
		pwUpdateActiveSpotInfo(tracks[i]);
	}
}
void pwUpdateActiveSpotInfo(TrackDefaultClass &track)
{
	if (track.pw.pwTrackingObj[0].newResultsAvailableForCopyFlag == 1)
	{
		pwCopyResultsFromTrackingClass(track);
	}
	if (track.general.toaFirstUpdatedFlag == 1)
	{
		track.pw.pwTrackingObj[0].ResetPWAnalysisInfo();

		switch (track.pri.priTrackingObj[0].currPriType)
		{
		case SequenceTypesEnum::seq_stagger:
		case SequenceTypesEnum::seq_dwell:
		{
			vector<uint64_t> &lockedToas =
					track.pri.priTrackingObj[0].GetSeqSearchBuffer();
			uint32_t bufferIndexOfFirstPulse = 0;
			for (int32_t i = track.general.dspPdwBuffer.size() - 1; i > -1; i--)
			{
				if (track.general.toaFirst
						== track.general.dspPdwBuffer[i].timeOfArrivalStart)
				{
					bufferIndexOfFirstPulse = i;
					break;
				}
			}
			uint32_t bufferIndex = bufferIndexOfFirstPulse;
			for (uint32_t i = 0;
					i < lockedToas.size()
							&& bufferIndex < track.general.dspPdwBuffer.size();)
			{
				uint64_t &bufferToa =
						track.general.dspPdwBuffer[bufferIndex].timeOfArrivalStart;
				uint64_t &lockedToa = lockedToas[i];
				if (bufferToa == lockedToa)
				{
					track.pw.pwTrackingObj[0].AddNextPwValue(
							track.general.dspPdwBuffer[bufferIndex].pulseWidth);
					bufferIndex++;
					i++;
				}
				else if (bufferToa > lockedToa)
				{
					i++;
				}
				else
				{
					bufferIndex++;
				}
			}
			break;
		}
		default:
		{
			int32_t startPointIndex = 0;
			for (uint32_t j = 0; j < track.general.dspPdwBuffer.size(); j++)
			{
				if (track.general.dspPdwBuffer[j].timeOfArrivalStart
						== track.general.toaFirst)
				{
					startPointIndex = j;
					break;
				}
			}
			int32_t lastTOAIndex =
					track.general.dspPdwBuffer.size()
							- track.pri.priTrackingObj[0].countSinceLastSuccessfulPriAnalysis;
			uint64_t lastTOAOfPriAnalysis =
					track.general.dspPdwBuffer[lastTOAIndex].timeOfArrivalStart;

			int32_t firstTOAIndex = lastTOAIndex
					- track.pri.priTrackingObj[0].lastAnalysisPulseCount;
			uint64_t firstTOAOfPriAnalysis =
					track.general.dspPdwBuffer[firstTOAIndex].timeOfArrivalStart;

			if (lastTOAOfPriAnalysis
					< track.general.dspPdwBuffer[startPointIndex].timeOfArrivalStart)
			{

			}
			else if (firstTOAOfPriAnalysis
					< track.general.dspPdwBuffer[startPointIndex].timeOfArrivalStart)
			{
				for (int32_t j = startPointIndex; j > -1 && j < lastTOAIndex;
						j++)
				{
					track.pw.pwTrackingObj[0].AddNextPwValue(
							track.general.dspPdwBuffer[j].pulseWidth);
				}

				if (track.pw.pwTrackingObj[0].newResultsAvailableForCopyFlag
						== 1)
				{
					track.pw.pwCircular.resize(0);
					pwCopyResultsFromTrackingClass(track);

				}
			}
			break;
		} //end case

		}
		//			int32_t lastTOAIndex =
		//					track.general.dspPdwBuffer.size()
		//							- track.pri.priTrackingObj[0].countSinceLastSuccessfulPriAnalysis;
		//			uint64_t lastTOAOfPriAnalysis =
		//					track.general.dspPdwBuffer[lastTOAIndex].timeOfArrivalStart;
		//
		//			int32_t firstTOAIndex = lastTOAIndex
		//					- track.pri.priTrackingObj[0].lastAnalysisPulseCount;
		//			uint64_t firstTOAOfPriAnalysis =
		//					track.general.dspPdwBuffer[firstTOAIndex].timeOfArrivalStart;
		//
		//			if (lastTOAOfPriAnalysis
		//					< track.general.dspPdwBuffer[startPointIndex].timeOfArrivalStart)
		//			{
		//
		//			}
		//			else if (firstTOAOfPriAnalysis
		//					< track.general.dspPdwBuffer[startPointIndex].timeOfArrivalStart)
		//			{
		//				for (int32_t j = startPointIndex; j > -1 && j < lastTOAIndex;
		//						j++)
		//				{
		//					track.pw.pwTrackingObj[0].AddNextPwValue(
		//							track.general.dspPdwBuffer[j].pulseWidth);
		//				}
		//
		//				if (track.pw.pwTrackingObj[0].newResultsAvailableForCopyFlag
		//						== 1)
		//				{
		//					track.pw.pwCircular.resize(0);
		//					pwCopyResultsFromTrackingClass(track);
		//
		//				}
		//			}
		//			else
		//			{
		//				for (uint32_t j = startPointIndex;
		//						j < track.general.dspPdwBuffer.size(); j++)
		//				{
		//					track.pw.pwTrackingObj[0].AddNextPwValue(
		//							track.general.dspPdwBuffer[j].pulseWidth);
		//				}
		//
		//				if (track.pw.pwTrackingObj[0].newResultsAvailableForCopyFlag
		//						== 1)
		//				{
		//					track.pw.pwCircular.resize(0);
		//					pwCopyResultsFromTrackingClass(track);
		//
		//				}
		//				track.general.toaFirstUpdatedFlag = 0;
		//			}
		track.general.toaFirstUpdatedFlag = 0;
	}
}
void pwCopyResultsFromTrackingClass(TrackDefaultClass &track)
{
	AnalysisTrackPwLowClass &trackingObj = track.pw.pwTrackingObj[0];
	switch (trackingObj.currPwType)
	{
	case SequenceTypesEnum::seq_dwell:
		track.pw.curPwType = PwTypeEnum::pw_fixed;
		break;
	case SequenceTypesEnum::seq_jitter:
		track.pw.curPwType = PwTypeEnum::pw_agile;
		break;
	case SequenceTypesEnum::seq_unknown:
		track.pw.curPwType = PwTypeEnum::pw_uncomputed;
		break;
	default:
		track.pw.curPwType = PwTypeEnum::pw_uncomputed;
		break;
	}

	if (trackingObj.lastAnalysisWasSuccessfulFlag == 1)
	{
		vector<uint32_t> &pwSpots = trackingObj.pwSpots;
		track.pw.lastAnalysisSpotCount = pwSpots.size();
		LimitPwClass temp;
		temp.pulseCount = track.pw.pwTrackingObj[0].lastAnalysisPulseCount;
		temp.limitType = LimitTypeEnum::type_pw;
		temp.pwType = track.pw.curPwType;
		for (uint16_t i = 0; i < pwSpots.size(); i++)
		{
			temp.pwLow = pwSpots[i];
			temp.pwAvg = pwSpots[i];
			temp.pwHigh = pwSpots[i];
			temp.pwStdDev = (uint32_t) trackingObj.deviationAllowed[i];
			track.pw.pwCircular.push_front(temp);
		}

		if (track.pw.pwFirst.size() == 0)
		{
			for (uint16_t i = 0; i < pwSpots.size(); i++)
			{
				temp.pwLow = pwSpots[i];
				temp.pwAvg = pwSpots[i];
				temp.pwHigh = pwSpots[i];
				temp.pwStdDev = (uint32_t) trackingObj.deviationAllowed[i];
				track.pw.pwFirst.push_back(temp);
			}

		}
	}
	trackingObj.lastAnalysisWasSuccessfulFlag = 0;
	trackingObj.newResultsAvailableForCopyFlag = 0;
}
