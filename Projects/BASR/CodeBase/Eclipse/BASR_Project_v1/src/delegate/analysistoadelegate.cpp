/*
 * analysistoadelegate.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#define ANALYSISTOADELEGATE_CPP_

#include "../delegate/analysistoadelegate.h"
void priUpdatePriActiveSpotInfo(vector<TrackDefaultClass> &tracks,
		uint32_t &stringableTrackCount)
{/* This function is only used to get pri analysis results from tracking class
 *   as PRI analysis is automatically called if required, during tracking
 *   when pulses associate successfully
 */
	for (uint16_t i = 0; i < tracks.size(); i++)
	{
		tracks[i].pri.maxPriSpot = tracks[i].pri.priTrackingObj[0].GetMaxPri();

		if (tracks[i].pri.priTrackingObj[0].newResultsAvailableForCopyFlag == 1)
		{
			if(tracks[i].pri.priFirst.size() == 0)
			{
				stringableTrackCount++;
			}
			priCopyResultFromFromTrackingObject(tracks[i]);
		}

	}
}
void priCopyResultFromFromTrackingObject(TrackDefaultClass &track)
{
	AnalysisTrackPriLowClass &trackingObj = track.pri.priTrackingObj[0];

	LimitPriClass tempPriValue;
	TrackPriClass &priObj = track.pri;

	switch (trackingObj.currPriType)
	{
	case SequenceTypesEnum::seq_unknown:
		priObj.curPriType = PriTypeEnum::pri_uncomputed;	// unknown pri
		break;
	default:
		break;
	}
	priObj.maxPriSpot = trackingObj.GetMaxPri();
	if (priObj.maxPriSpot > 0)
	{
		uint64_t emitterSilenceTime = 0;
		if (priObj.maxPriSpot > 100000)
		{
			emitterSilenceTime = 8 * priObj.maxPriSpot
					/ ConstLow::toaCountInMicros;
		}
		else
		{
			emitterSilenceTime = 20 * priObj.maxPriSpot
					/ ConstLow::toaCountInMicros;
		}
		track.general.timeToDeclareSilent = min((uint64_t) (50000),
				emitterSilenceTime);
	}
//		if(track.general.timeToDeclareSilent == 5755)
//			track.general.timeToDeclareSilent = track.general.timeToDeclareSilent+1;
	if (trackingObj.lastAnalysisWasSuccessfulFlag == 1)
	{
		priObj.lastPriAnalysisPulseCount = trackingObj.lastAnalysisPulseCount;

		//priObj.maxPriSpot = trackingObj.
		switch (trackingObj.currPriType)
		{
		case SequenceTypesEnum::seq_stagger:
		{
			priObj.curPriType = PriTypeEnum::pri_stagger;
			vector<int32_t> &staggerSpots =
					priObj.priTrackingObj[0].GetStaggerSpots();
			tempPriValue.frame = priObj.priTrackingObj[0].GetStaggerFrame();
			tempPriValue.priType = priObj.curPriType;
			priObj.lastPriAnalysisSpotCount = staggerSpots.size();
			for (uint16_t i = 0; i < staggerSpots.size(); i++)
			{
				tempPriValue.priHigh = staggerSpots[i];
				tempPriValue.priMean = staggerSpots[i];
				tempPriValue.priLow = staggerSpots[i];
				tempPriValue.pulseCount = 0;
				priObj.priCircular.push_front(tempPriValue);
			}
			break;
		}
		case SequenceTypesEnum::seq_dwell:
		{
			vector<int32_t> &dwellSpots =
					priObj.priTrackingObj[0].GetDwellSpots();

			if (dwellSpots.size() == 1)
				priObj.curPriType = PriTypeEnum::pri_fixed;
			else
				priObj.curPriType = PriTypeEnum::pri_dwellswitch;

			tempPriValue.priType = priObj.curPriType;
			priObj.lastPriAnalysisSpotCount = dwellSpots.size();
			for (uint16_t i = 0; i < dwellSpots.size(); i++)
			{
				tempPriValue.priHigh = dwellSpots[i];
				tempPriValue.priMean = dwellSpots[i];
				tempPriValue.priLow = dwellSpots[i];
				tempPriValue.pulseCount = 0;
				priObj.priCircular.push_front(tempPriValue);
			}
			break;
		}
		case SequenceTypesEnum::seq_difftoa:
		{
			priObj.curPriType = PriTypeEnum::pri_difftoa;
			vector<int32_t> &priSpots = priObj.priTrackingObj[0].GetPriSpots();
			vector<int32_t> &priRangesHigh =
					priObj.priTrackingObj[0].GetUpperRange();
			vector<int32_t> &priRangesLow =
					priObj.priTrackingObj[0].GetLowerRange();
			tempPriValue.priType = PriTypeEnum::pri_difftoa;
			priObj.lastPriAnalysisSpotCount = priSpots.size();
			for (uint16_t i = 0; i < priSpots.size(); i++)
			{
				tempPriValue.priHigh = priRangesHigh[i];
				tempPriValue.priMean = priSpots[i];
				tempPriValue.priLow = priRangesLow[i];
				tempPriValue.pulseCount = 0;
				priObj.priCircular.push_front(tempPriValue);
			}
			break;
		}
		case SequenceTypesEnum::seq_slideNeg:
		{
			priObj.curPriType = PriTypeEnum::pri_sliding;

			tempPriValue.priType = priObj.curPriType;
			priObj.lastPriAnalysisSpotCount = 1;
			tempPriValue.pulseCount = 0;
			tempPriValue.priLow = priObj.priTrackingObj[0].GetNegSlideStartPri();
			tempPriValue.priMean = priObj.priTrackingObj[0].GetNegSlideRate();
			tempPriValue.priHigh = priObj.priTrackingObj[0].GetNegSlideEndPri();
			priObj.priCircular.push_front(tempPriValue);
			break;
		}
		case SequenceTypesEnum::seq_slidePos:
		{
			priObj.curPriType = PriTypeEnum::pri_sliding;

			tempPriValue.priType = priObj.curPriType;
			priObj.lastPriAnalysisSpotCount = 1;
			tempPriValue.pulseCount = 0;
			tempPriValue.priLow = priObj.priTrackingObj[0].GetPosSlideStartPri();
			tempPriValue.priMean = priObj.priTrackingObj[0].GetPosSlideRate();
			tempPriValue.priHigh = priObj.priTrackingObj[0].GetPosSlideEndPri();
			priObj.priCircular.push_front(tempPriValue);
			break;
		}
		case SequenceTypesEnum::seq_slideTri:
		{
			priObj.curPriType = PriTypeEnum::pri_sliding;

			tempPriValue.priType = priObj.curPriType;
			priObj.lastPriAnalysisSpotCount = 1;
			tempPriValue.pulseCount = 0;
			tempPriValue.priLow = priObj.priTrackingObj[0].GetTriSlideStartPri();
			tempPriValue.priMean = priObj.priTrackingObj[0].GetTriSlideRate();
			tempPriValue.priHigh = priObj.priTrackingObj[0].GetTriSlideEndPri();
			priObj.priCircular.push_front(tempPriValue);
			break;
		}
		case SequenceTypesEnum::seq_slideSine:
			{
				priObj.curPriType = PriTypeEnum::pri_sliding;

				tempPriValue.priType = priObj.curPriType;
				priObj.lastPriAnalysisSpotCount = 1;
				tempPriValue.pulseCount = 0;
				tempPriValue.priLow = priObj.priTrackingObj[0].GetSineSlideStartPri();
				tempPriValue.priMean = priObj.priTrackingObj[0].GetSineSlideStartPri();
				tempPriValue.priHigh = priObj.priTrackingObj[0].GetSineSlideEndPri();
				priObj.priCircular.push_front(tempPriValue);
				break;
			}
		case SequenceTypesEnum::seq_jitter:
		{
			priObj.curPriType = PriTypeEnum::pri_jitter;
			vector<int32_t> &priSpots = priObj.priTrackingObj[0].GetPriSpots();
			vector<int32_t> &priRangesHigh =
					priObj.priTrackingObj[0].GetUpperRange();
			vector<int32_t> &priRangesLow =
					priObj.priTrackingObj[0].GetLowerRange();
			tempPriValue.priType = PriTypeEnum::pri_jitter;
			priObj.lastPriAnalysisSpotCount = priSpots.size();
			for (uint16_t i = 0; i < priSpots.size(); i++)
			{
				tempPriValue.priHigh = priRangesHigh[i];
				tempPriValue.priMean = priSpots[i];
				tempPriValue.priLow = priRangesLow[i];
				tempPriValue.pulseCount = 0;
				priObj.priCircular.push_front(tempPriValue);
			}
			break;
		}
		case SequenceTypesEnum::seq_complex:
		{
			priObj.curPriType = PriTypeEnum::pri_complex;
			vector<int32_t> &priSpots = priObj.priTrackingObj[0].GetPriSpots();
			vector<int32_t> &priRangesHigh =
					priObj.priTrackingObj[0].GetUpperRange();
			vector<int32_t> &priRangesLow =
					priObj.priTrackingObj[0].GetLowerRange();
			tempPriValue.priType = PriTypeEnum::pri_complex;
			priObj.lastPriAnalysisSpotCount = priSpots.size();
			for (uint16_t i = 0; i < priSpots.size(); i++)
			{
				tempPriValue.priHigh = priRangesHigh[i];
				tempPriValue.priMean = priSpots[i];
				tempPriValue.priLow = priRangesLow[i];
				tempPriValue.pulseCount = 0;
				priObj.priCircular.push_front(tempPriValue);
			}
			break;
		}
		default:
			priObj.curPriType = PriTypeEnum::pri_uncomputed;
			priObj.lastPriAnalysisSpotCount = 0;
			break;

		}
		if (priObj.priFirst.size() == 0)
		{
			//after first pri analysis passes for a track, we mark it as a candidate for stringing
			track.general.dspCandidateForStringingFlag = true;
			if(track.pw.pwAnalysisDoneOnPriBuffer == 0) // Reset PW info if it was not done on pulses that made pri analysis successful
			{
				track.pw.pwCircular.resize(0);
				track.pw.lastAnalysisSpotCount = 0;
				track.pw.pwFirst.resize(0);
			}
			int32_t diffTOA = 0;
			for (int16_t i = 0;
					i < (int16_t) (track.general.dspPdwBuffer.size() - 1); i++)
			{
				diffTOA = track.general.dspPdwBuffer[i + 1].timeOfArrivalStart
						- track.general.dspPdwBuffer[i].timeOfArrivalStart;

				int32_t minimumJumpAllowed = 0;
				if (priObj.maxPriSpot > 100000)
				{
					minimumJumpAllowed = 8 * priObj.maxPriSpot;
				}
				else
				{
					minimumJumpAllowed = 20 * priObj.maxPriSpot;
				}
				if (diffTOA > minimumJumpAllowed)
					trackingObj.firstLockedTOA = track.general.dspPdwBuffer[i
							+ 1].timeOfArrivalStart;
			}
			if (trackingObj.firstLockedTOA > 0)
			{
				track.general.toaFirst = trackingObj.firstLockedTOA;
				track.general.toaFirstUpdatedFlag = 1;
			}
			priObj.toaBufferSizeFirst = priObj.lastPriAnalysisPulseCount;
			priObj.lastPriAnalysistime =
					track.general.dspPdwBuffer.back().timeOfArrivalStart;
			for (uint16_t i = 0; i < priObj.priCircular.size(); i++)
			{
				priObj.priFirst.push_back(priObj.priCircular[i]);
			}
		}
	}
	trackingObj.lastAnalysisWasSuccessfulFlag = 0;
	trackingObj.newResultsAvailableForCopyFlag = 0;

}
//bool priComputeNewValue_Version2(vector<PdwDefaultClass> pdwInput,
//		TrackPriClass& priObj)
//{
//	vector<int32_t> diffToas(pdwInput.size() - 1);
//	vector<int32_t> diffPRIs(pdwInput.size() - 1);
//
//	for (uint16_t i = 0; i < pdwInput.size() - 1; i++)
//	{
//		diffToas[i] = pdwInput[i + 1].timeOfArrivalStart
//				- pdwInput[i].timeOfArrivalStart;
//	}
//	for (uint16_t i = 0; i < diffToas.size() - 1; i++)
//	{
//		diffPRIs[i] = diffToas[i + 1] - diffToas[i];
//	}
//
////	if (priObj.seqSearcher[0].searchForPattern_v2(diffToas, diffPRIs) == true)
////	{
//	if (priObj.seqSearcher[0].searchForPattern_v1(diffToas) == true)
//	{
//		vector<SeqDescriptor> &patternDescription =
//				priObj.seqSearcher[0].getFoundSequenceDescription();
//
//		if (patternDescription.size() == 0)
//			return false;
//
//		switch (priObj.seqSearcher[0].getSequenceType())
//		{
//		case SequenceTypeEnum::seq_dwell:
//		{
//			if (patternDescription.size() == 1)
//				priObj.curPriType = PriTypeEnum::pri_fixed;
//			else
//				priObj.curPriType = PriTypeEnum::pri_dwellswitch;
//			break;
//		}
//		case SequenceTypeEnum::seq_stagger:
//		{
//			if (patternDescription.size() == 1)
//				priObj.curPriType = PriTypeEnum::pri_fixed;
//			else
//				priObj.curPriType = PriTypeEnum::pri_stagger;
//			break;
//		}
//		case SequenceTypeEnum::seq_slideNeg:
//		case SequenceTypeEnum::seq_slidePosOrNeg:
//		case SequenceTypeEnum::seq_slidePos:
//		case SequenceTypeEnum::seq_slideTri:
//		{
//			priObj.curPriType = PriTypeEnum::pri_sliding;
//			break;
//		}
//		case SequenceTypeEnum::seq_jitter:
//		{
//			priObj.curPriType = PriTypeEnum::pri_jitter;
//			break;
//		}
//		default:
//		{
//			break;
//		}
//		}
//		LimitPriClass tempPriValue;
//		tempPriValue.priType = priObj.curPriType;
//		priObj.lastPriAnalysisSpotCount = patternDescription.size();
//		priObj.maxPriSpot = patternDescription.back().maxValue;
//
//		// ----- to not repeat the last fixed pri analysis result if spots match
//		if (priObj.curPriType == PriTypeEnum::pri_fixed)
//		{
//			if (priObj.priCircular.size() > 0)
//			{
//				if (priObj.priCircular.back().priType == PriTypeEnum::pri_fixed
//						|| priObj.priCircular.back().priType
//								== PriTypeEnum::pri_dwellswitch)
//				{
//					int64_t diff = priObj.priCircular.back().priMean
//							- patternDescription[0].meanValue;
//
//					if (abs(diff) < 25)
//					{
//
//						priObj.priCircular.back().pulseCount =
//								patternDescription[0].freq;
//						return true;
//					}
//				}
//			}
//		}
//
//		//
//
//		for (uint16_t i = 0; i < patternDescription.size(); i++)
//		{
//			tempPriValue.priHigh = patternDescription[i].maxValue;
//			tempPriValue.priMean = patternDescription[i].meanValue;
//			tempPriValue.priLow = patternDescription[i].minValue;
//			tempPriValue.pulseCount = patternDescription[i].freq;
//			if (tempPriValue.priMean == 0)
//			{
//				cout << "here" << endl;
//				//TODO gmd-> find reason of zero mean spots
//			}
//			priObj.priCircular.push_front(tempPriValue);
//		}
//		return true;
//	}
//	return false;
//}
