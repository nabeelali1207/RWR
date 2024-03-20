/*
 * analysistrackingdelegate.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#define ANALYSISTRACKLOWDELEGATE_CPP_

#include "analysistracklowdelegate.h"

int16_t trackMeanValueScoring(uint16_t mean, uint16_t queryValue,
		uint16_t minBoundry, int16_t stdDevSlope, int16_t priscore)
{
	// assumes that quantities are normally distributed so
	// if query value is within 1 Standard Deviation of the mean then
	// a score of
	int32_t diff = mean;
	diff = diff - queryValue;
	int32_t absDiff = abs(diff);
	int32_t score = 0;

	stdDevSlope = (stdDevSlope < 1 ? 1 : stdDevSlope); // to compensate for divide by zero

	if (absDiff < minBoundry)
	{
		score = 100;
	}
	else if (priscore > 0)
	{
		absDiff = max(0, absDiff - minBoundry);
		score = 100 - ((absDiff * 100) / (minBoundry + 2 * stdDevSlope));
	}
	else
	{
		absDiff = max(0, absDiff - minBoundry);
		score = 100 - ((absDiff * 100) / minBoundry);
	}

	score = min(score, 100);
	score = max(0, score);
	return score;
	// if the query value is at 1std deviation boundary then 100-32=68 score is assigned
	// because in normal distribution 68% of the samples lie in the 1 standard deviation range
	// if the query value has difference greater then 3 std Dev. from mean 0 score is assigned.

}
int16_t trackPdwPowerScore_v2(PdwDefaultClass &pdw, TrackPowerClass &trackPow,
		int16_t priScore)
{
	if (pdw.antennaCode == trackPow.dominantAntenna)
	{

	}
	else if (trackPow.weightsVector[trackPow.dominantAntenna] < 0.6)
	{
		if (trackPow.secondaryAntenna == pdw.antennaCode)
		{
			if (pdw.pxAntenna == trackPow.pxAntenna
					&& pdw.pyAntenna == trackPow.pyAntenna)
			{

			}
			else
				return 0;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

	int16_t finalScore = 0;
	int16_t scoreUsingLastPowers = 0;
	int16_t scoreUsingAvgPowers = 0;

	int16_t allowedDeviation = 0;

	float diff = 0;
	diff += trackPow.weightsVector[0] * abs(pdw.powerA - trackPow.lastPa);
	diff += trackPow.weightsVector[1] * abs(pdw.powerB - trackPow.lastPb);
	diff += trackPow.weightsVector[2] * abs(pdw.powerC - trackPow.lastPc);
	diff += trackPow.weightsVector[3] * abs(pdw.powerD - trackPow.lastPd);

	diff = abs(diff);
	allowedDeviation = ConstLow::asPowerDefualtVariation;
	if (diff < allowedDeviation)
	{
		scoreUsingLastPowers = 100;
	}
	else
	{
		diff = diff - allowedDeviation;
		if (priScore > 0)
		{
			allowedDeviation = 2 * allowedDeviation;
		}
		scoreUsingLastPowers = 100 - 100 * (diff / allowedDeviation);
	}

	diff = 0;
	diff += trackPow.weightsVector[0] * abs(pdw.powerA - trackPow.avgPA);
	diff += trackPow.weightsVector[1] * abs(pdw.powerB - trackPow.avgPB);
	diff += trackPow.weightsVector[2] * abs(pdw.powerC - trackPow.avgPC);
	diff += trackPow.weightsVector[3] * abs(pdw.powerD - trackPow.avgPD);

	diff = abs(diff);
	allowedDeviation = ConstLow::asPowerDefualtVariation;
	if (diff < allowedDeviation)
	{
		scoreUsingAvgPowers = 100;
	}
	else
	{
		diff = diff - allowedDeviation;
		if (priScore > 0)
		{
			allowedDeviation = 2 * allowedDeviation;
		}
		scoreUsingAvgPowers = 100 - 100 * (diff / allowedDeviation);
	}
	scoreUsingLastPowers = min((int16_t) 100, scoreUsingLastPowers);
	scoreUsingAvgPowers = min((int16_t) 100, scoreUsingAvgPowers);

	finalScore = max(scoreUsingLastPowers, scoreUsingAvgPowers);
//	finalScore = 0.5 * scoreUsingLastPowers + 0.5 * scoreUsingAvgPowers;

	finalScore = max((int16_t) 0, finalScore);

	return finalScore;
}
int8_t trackCompareBands(BandCodeEnum pdwBand, BandCodeEnum trackBand)
{
	if (pdwBand == trackBand)
	{
		return 1;
	}
	return -1;
}
int8_t trackCompareAngleLowerWindow(uint16_t aoa1, uint16_t aoa2)
{
	uint16_t diff = (aoa2 > aoa1) ? aoa2 - aoa1 : aoa1 - aoa2;
	diff = (diff > 180 ? 360 - diff : diff);
	if (diff <= ConstLow::asTrackingAoaLowerWindow)
		return 1;
	return -1;
}
int8_t trackCompareAngleWindow(uint16_t aoa1, uint16_t aoa2)
{
	uint16_t diff = (aoa2 > aoa1) ? aoa2 - aoa1 : aoa1 - aoa2;
	diff = (diff > 180 ? 360 - diff : diff);
	if (diff <= ConstLow::asTrackingAoaWindow)
		return 1;
	return -1;
}
int8_t trackCompareBandWidth(uint16_t rf, uint16_t trackRf)
{
	uint16_t diff = (trackRf > rf) ? trackRf - rf : rf - trackRf;

	if (diff <= ConstLow::asTrackingRfWindow)
		return 1;
	return -1;
}
bool trackCompareGroundIllumination(PdwDefaultClass &pdw,
		TrackPowerClass &trackPow)
{
	GroundIlluminationEnum groundIlluminationInPulse =
			GroundIlluminationEnum::no_axis;
	bool groundIlluminationMatch = false;

	if (pdw.powerA > pdw.powerC && pdw.powerC > pdw.powerB
			&& pdw.powerC > pdw.powerD)
		groundIlluminationInPulse = GroundIlluminationEnum::x_axis;

	else if (pdw.powerC > pdw.powerA && pdw.powerA > pdw.powerB
			&& pdw.powerA > pdw.powerD)
		groundIlluminationInPulse = GroundIlluminationEnum::x_axis;

	else if (pdw.powerD > pdw.powerB && pdw.powerB > pdw.powerA
			&& pdw.powerB > pdw.powerC)
		groundIlluminationInPulse = GroundIlluminationEnum::y_axis;

	else if (pdw.powerB > pdw.powerD && pdw.powerD > pdw.powerA
			&& pdw.powerD > pdw.powerC)
		groundIlluminationInPulse = GroundIlluminationEnum::y_axis;

	else
		groundIlluminationInPulse = GroundIlluminationEnum::no_axis;

	// compare track's and pulse's ground illumination status
	groundIlluminationMatch =
			((groundIlluminationInPulse == GroundIlluminationEnum::x_axis
					&& trackPow.groundIlluminationAxis
							== GroundIlluminationEnum::x_axis)
					|| (groundIlluminationInPulse
							== GroundIlluminationEnum::y_axis
							&& trackPow.groundIlluminationAxis
									== GroundIlluminationEnum::y_axis));
	return groundIlluminationMatch;

}
int16_t trackPdwPowerScore(PdwDefaultClass &pdw, TrackPowerClass &trackPow,
		int16_t priScore)
//gives Power score in range [-30 to 100] depending upon degree of match
{	// matches all 4 powers
	int score = 0;
	score += 0.5 * trackPow.weightsVector[0]
			* trackMeanValueScoring(trackPow.lastPa, pdw.powerA,
					ConstLow::asPowerDefualtVariation, trackPow.stdPA,
					priScore);
	score += 0.5 * trackPow.weightsVector[1]
			* trackMeanValueScoring(trackPow.lastPb, pdw.powerB,
					ConstLow::asPowerDefualtVariation, trackPow.stdPB,
					priScore);
	score += 0.5 * trackPow.weightsVector[2]
			* trackMeanValueScoring(trackPow.lastPc, pdw.powerC,
					ConstLow::asPowerDefualtVariation, trackPow.stdPC,
					priScore);
	score += 0.5 * trackPow.weightsVector[3]
			* trackMeanValueScoring(trackPow.lastPd, pdw.powerD,
					ConstLow::asPowerDefualtVariation, trackPow.stdPD,
					priScore);
	score += 0.5 * trackPow.weightsVector[0]
			* trackMeanValueScoring(trackPow.avgPA, pdw.powerA,
					ConstLow::asPowerDefualtVariation, trackPow.stdPA,
					priScore);
	score += 0.5 * trackPow.weightsVector[1]
			* trackMeanValueScoring(trackPow.avgPB, pdw.powerB,
					ConstLow::asPowerDefualtVariation, trackPow.stdPB,
					priScore);
	score += 0.5 * trackPow.weightsVector[2]
			* trackMeanValueScoring(trackPow.avgPC, pdw.powerC,
					ConstLow::asPowerDefualtVariation, trackPow.stdPC,
					priScore);
	score += 0.5 * trackPow.weightsVector[3]
			* trackMeanValueScoring(trackPow.avgPD, pdw.powerD,
					ConstLow::asPowerDefualtVariation, trackPow.stdPD,
					priScore);
	return score;
}
void getParameterWeightageInTracking(float *weights,
		uint8_t priTrackingPossible, uint8_t rfTrackingPossible,
		uint8_t pwTrackingPossible)
{
	uint8_t trackingScoreIndex = ((priTrackingPossible & 0x1) << 2)
			+ ((rfTrackingPossible & 0x1) << 1) + (pwTrackingPossible & 0x1);

	switch (trackingScoreIndex)
	{
	case 0:
		weights[0] = 0.00;
		weights[1] = 0.00;
		weights[2] = 0.00;
		weights[3] = 0.50;
		break;
	case 1:
		weights[0] = 0.00;
		weights[1] = 0.00;
		weights[2] = 0.50;
		weights[3] = 0.50;
		break;
	case 2:
		weights[0] = 0.00;
		weights[1] = 0.50;
		weights[2] = 0.00;
		weights[3] = 0.50;
		break;
	case 3:
		weights[0] = 0.00;
		weights[1] = 0.33;
		weights[2] = 0.33;
		weights[3] = 0.33;
		break;
	case 4:
		weights[0] = 0.50;
		weights[1] = 0.00;
		weights[2] = 0.00;
		weights[3] = 0.50;
		break;
	case 5:
		weights[0] = 0.33;
		weights[1] = 0.00;
		weights[2] = 0.33;
		weights[3] = 0.33;
		break;
	case 6:
		weights[0] = 0.33;
		weights[1] = 0.33;
		weights[2] = 0.00;
		weights[3] = 0.33;
		break;
	case 7:
		weights[0] = 0.25;
		weights[1] = 0.25;
		weights[2] = 0.25;
		weights[3] = 0.25;
		break;
	}
}
int16_t trackGetIndexOfBestMatch(PdwDefaultClass &pdw,
		vector<TrackDefaultClass> &tracks)
{ /* This function calculates scores of the incoming pulse with all
 * active tracks and returns the track id with best match/ max score
 * */
	int16_t maxScore = -255;
	int16_t maxScoreIndex = 0;
	int16_t rfScore = 0, pwScore = 0, powerScore = 0, priScore = 0;
	int16_t rfScoreMx = 0, pwScoreMx = 0, powerScoreMx = 0, priScoreMx = 0;
	bool trackAndPulseInGroundIllum = 0;
//		float TRACKING_SCORES[4][4] =
//		{
//		{ 0, 0, 0.5, 0.5 },
//		{ 0, 0.5, 0.25, 0.25 },
//		{ 0.5, 0, 0.25, 0.25 },
//		{ 0.5, 0.25, 0.12, 0.13 } };
	float trackingWeights[4];
	float &priWeight = trackingWeights[0];
	float &rfWeight = trackingWeights[1];
	float &pwWeight = trackingWeights[2];
	float &powerWeight = trackingWeights[3];
	/* ------ */
//	float TRACKING_SCORES[4][4] =
//	{
//	{ 0, 0, 0.5, 0.5 },		// when only PW and Pow could be used for tracking
//			{ 0, 0, 0.5, 0.5 },	// when RF,PW and Power could be used for tracking
//			{ 0.33, 0, 0.33, 0.33 },	// when PRI,PW and power could be used
//			{ 0.33, 0, 0.33, 0.33 }	// when all parameters could be used
//	};
	if (trackAndPulseInGroundIllum == 0)
	{
		trackAndPulseInGroundIllum = 0;	//remove compiler warning
	}
	for (uint16_t i = 0; i < tracks.size(); i++)
	{
		if (tracks[i].general.dspTrackingEnabled == false)
		{
			continue;
		}
		else
		{
			uint64_t timePassed = pdw.timeOfArrivalStart
					- tracks[i].general.timeToaLastUpdate;

			//time passed is in toa time (10ns counter)
			if (timePassed > 3 * tracks[i].pri.maxPriSpot)
			{
				tracks[i].pri.priTrackingObj[0].priTrackingPossibleFlag = 0;

				//it means that when pri-tracking is not possible
				//		==> pw-tracking will not be possible also?
//				tracks[i].pw.pwTrackingObj[0].pwTrackingPossibleFlag = 0;

			}
			if (pdw.timeOfArrivalStart < tracks[i].general.timeToaLastUpdate)
			{
				continue;
			}
//			if (timePassed > 15 * tracks[i].pri.maxPriSpot)
//			{
//				tracks[i].pri.priTrackingObj[0].priTrackingPossibleFlag = 0;
//				tracks[i].general.dspTrackingEnabled = false; // No update to the track for a while
//				continue;
//			}

			//convert toa time to microsecond time
			timePassed = timePassed / ConstLow::toaCountInMicros;

			if (timePassed > tracks[i].general.timeToDeclareSilent)
			{
				tracks[i].general.dspTrackingEnabled = false; // No update to the track for a while
				continue;
			}
		}
		if (tracks[i].general.flagSelfTest != pdw.selfTestFlag)
			continue;
		int16_t currScore = 0;
		int8_t trackIsInAngleWindow = trackCompareAngleWindow(
				pdw.angleOfArrival, tracks[i].angle.curAngle); // see if AOA of pulse is within range of the candidate track
		int8_t trackIsInRfWindow = trackCompareBandWidth(pdw.frequency,
				tracks[i].rf.rfTrackingObj[0].rfMedian);

		if (tracks[i].power.groundIlluminationFlag == 1)
			trackAndPulseInGroundIllum = trackCompareGroundIllumination(pdw,
					tracks[i].power);

		if (trackIsInAngleWindow == 1
				&& trackIsInRfWindow
						== 1/*|| trackAndPulseInGroundIllum == true*/)
		{
			uint8_t isPriTrackingPossible =
					tracks[i].pri.priTrackingObj[0].CanGuessNextTOA(); // if pri information available ?
			uint8_t isRfTrackingPossible =
					tracks[i].rf.rfTrackingObj[0].rfTrackingPossibleFlag
							&& pdw.frequencyValidFlag; // Is RF info available and RF of incoming pulse valid?

			uint8_t isPwTrackingPossible =
					tracks[i].pw.pwTrackingObj[0].pwTrackingPossibleFlag;

			getParameterWeightageInTracking(trackingWeights,
					isPriTrackingPossible, isRfTrackingPossible,
					isPwTrackingPossible);

			if (isPriTrackingPossible == 1)
			{
				uint8_t priMatch =
						tracks[i].pri.priTrackingObj[0].DoesToaBelongToThisTrack(
								pdw.timeOfArrivalStart);	// Pri matching
				if (priMatch == 1)
				{
					priScore = 100;	// pri matched
				}
				else
				{
					priScore = 0;	// pri not matched
				}
			}
			else
			{ // pri tracking not possible
				priScore = 0;
			}
			if (isRfTrackingPossible == 1)
			{
				rfScore = tracks[i].rf.rfTrackingObj[0].GetRfScore(
						pdw.frequency, pdw.frequencyValidFlag);	// RF matching
			}
			else
			{
				rfScore = 0;
			}
			pwScore = tracks[i].pw.pwTrackingObj[0].GetPwScore(
					(int32_t) pdw.pulseWidth);		// pw matching
//			powerScore = trackPdwPowerScore(pdw, tracks[i].power, priScore);// power matching
			powerScore = trackPdwPowerScore_v2(pdw, tracks[i].power, priScore);	// power matching

			/* special scenario handling for jitter pri added on 15.06.2020 */
			if (tracks[i].pri.priTrackingObj[0].currPriType
					== SequenceTypesEnum::seq_jitter)
			{ // if pri type is jitter, assign more weight to PW
				if (isRfTrackingPossible)
				{
					trackingWeights[0] = 0.2;
					trackingWeights[1] = 0.1;
					trackingWeights[2] = 0.45;
					trackingWeights[3] = 0.25;
				}
				else
				{
					trackingWeights[0] = 0.2;
					trackingWeights[1] = 0;
					trackingWeights[2] = 0.5;
					trackingWeights[3] = 0.3;
				}
			}

			/*if(trackAndPulseInGroundIllum)
			 {
			 if (isRfTrackingPossible)
			 {
			 trackingWeights[0] = 0.35;   // pri
			 trackingWeights[1] = 0.1;   // rf
			 trackingWeights[2] = 0.4;  // PW
			 trackingWeights[3] = 0.15;	// Power
			 }
			 else
			 {
			 trackingWeights[0] = 0.35;   // pri
			 trackingWeights[1] = 0;   // rf
			 trackingWeights[2] = 0.5;  // PW
			 trackingWeights[3] = 0.15;	// Power
			 }
			 }*/
			// Assign weights to all of the scores
			currScore = priWeight * priScore + rfWeight * rfScore
					+ pwWeight * pwScore + powerWeight * powerScore;
			// Special checks to handle multi-agility of stagger in both pri and pw
			if (currScore < 60
					&& tracks[i].pri.curPriType == PriTypeEnum::pri_stagger)
			{

				if (tracks[i].pri.lastPriAnalysisSpotCount == 2
						&& tracks[i].pw.pwTrackingObj[0].pwSpots.size() > 1)
				{
					int8_t trackIsInAngleWithinLowerWindow =
							trackCompareAngleLowerWindow(pdw.angleOfArrival,
									tracks[i].angle.curAngle); // see if AOA of pulse is very close
					if (powerScore >= 80
							&& trackIsInAngleWithinLowerWindow == true)
					{
						currScore = ConstLow::asTrackingMinScoreThreshold + 1; // min score
					}
				}
			}
		}
		if (abs((int16_t) currScore - (int16_t) maxScore) < 5)
		{
			if (priScore == 0 && priScoreMx > 0)
			{
				continue;
			}
			else if (priScore > 0 && priScoreMx == 0)
			{
				maxScore = currScore;
				maxScoreIndex = i;
				rfScoreMx = rfScore;
				pwScoreMx = pwScore;
				powerScoreMx = powerScore;
				priScoreMx = priScore;
			}
			else if (currScore > maxScore)
			{
				maxScore = currScore;
				maxScoreIndex = i;
				rfScoreMx = rfScore;
				pwScoreMx = pwScore;
				powerScoreMx = powerScore;
				priScoreMx = priScore;
			}
		}
		else if (currScore > maxScore) // Find the track with max score
		{
			maxScore = currScore;
			maxScoreIndex = i;
			rfScoreMx = rfScore;
			pwScoreMx = pwScore;
			powerScoreMx = powerScore;
			priScoreMx = priScore;
		}
	}
	// store individual scores of pulse with best matched track, used for debugging
	pdw.trackingRfScore = rfScoreMx;
	pdw.trackingPwScore = pwScoreMx;
	pdw.trackingPowerScore = powerScoreMx;
	pdw.trackingPriScore = priScoreMx;
	//maxScore = ConstLow::asTrackingMinScoreThreshold + 10; // forced merging of pulse DEBUGGING ONLY
	if (maxScore > ConstLow::asTrackingMinScoreThreshold)
	{ // a match was found
		return maxScoreIndex;	// best match track
	}

	pdw.trackingRfScore = 0;
	pdw.trackingPwScore = 0;
	pdw.trackingPowerScore = 0;
	pdw.trackingPriScore = 0;

	return -1;	// no match found
}

void trackPdwIntoExistingTracks(vector<TrackDefaultClass> &tracks,
		vector<PdwDefaultClass> &iterationPdws, uint64_t currSystemIteration)
{ // This function does Tracking (pulse matching/scoring and association)
//	if (tracks.size() == 0)
//		return;
	for (uint16_t i = 0; i < iterationPdws.size(); i++)
	{
		if (iterationPdws[i].timeOfArrivalStart == 1338235700) // DEBUGGING ONLY
		{
			cout << "hi" << endl;
		}

		if (iterationPdws[i].powerThresholdFlag == false)
		{ //ignore pdws that are below power threshold
			continue;
		}
		//find matching track for this pdw and associate it with found track
		if (iterationPdws[i].pulseTrackedFlag == true)
		{	//ignore pdws that have already been tracked
			continue;
		}
		if (iterationPdws[i].clusterProcessFlag == true)
		{
			continue;
		}
		// Tracking starts
		// 1. Find the best match
		int16_t matchingTrackIndex = trackGetIndexOfBestMatch(iterationPdws[i],
				tracks);
		if(matchingTrackIndex == 2)
			matchingTrackIndex=matchingTrackIndex*1;
		if (matchingTrackIndex < 0)
		{
			continue;
		}
		else
		{		//2. Associate the pulse and update the track
//			if(tracks[matchingTrackIndex].general.trackID == 40595)
//				tracks[matchingTrackIndex].general.trackID = 40594+1;
			iterationPdws[i].pulseTrackedFlag = true;
			trackAssociateNewPdwWithTrack(tracks[matchingTrackIndex],
					iterationPdws[i], currSystemIteration);
			iterationPdws[i].trackProcessCount++;
		}
	}
}

void trackAssociateNewPdwWithTrack(TrackDefaultClass &track,
		PdwDefaultClass &pdw, uint64_t currSystemIteration)
{/* This function associates the pulse with its track and updates the track state
 * Note: if parameters of track have changed or updated the during pulse association,
 * if number of pulses that have belonged to the
 * track crosses parameters' analysis trigger (PRI,PW or RF), then corresponding
 *  parameter analysis are called i.e. RF Analysis, PRI analysis, PW analysis.
 */
	track.general.dspPdwBuffer.push_back(pdw);
	track.angle.pdwCountSinceLastAoaAnlaysis++;
	track.general.dspIterationPulseCount++;

	track.general.timeToaLastUpdate = pdw.timeOfArrivalStart;
	track.general.timeSysLastUpdate = Utility::getSystemTime();
	track.general.iterSysLastUpdate = currSystemIteration;

	track.general.toaLast = pdw.timeOfArrivalStart;

	trackAddPulseToPriPwBuffer(track, pdw);
	if (pdw.frequencyValidFlag == 1)
		track.rf.rfTrackingObj[0].AddNextRfValue(pdw.frequency,
				pdw.frequencyValidFlag);			// rf tracking and update

	if (track.angle.pdwCountSinceLastAoaAnlaysis >= ConstLow::asAoaPdwCountMin)
	{
		doaAnalysisExecute(track);	// AOA calculation and update
	}

	powerUpdateTrackPowersOnPulseAssociation(track, pdw);		// power update
	lpiUpdateTrackLpiOnPulseAssociation(track,pdw);
}
void trackAddPulseToPriPwBuffer(TrackDefaultClass &track, PdwDefaultClass &pdw)
{
	int8_t priTrackState = track.pri.priTrackingObj[0].PushToa(
			pdw.timeOfArrivalStart);		// Pri tracking and update

	if (priTrackState == 0
			&& track.pri.priTrackingObj[0].lastKnownPriType
					== SequenceTypesEnum::seq_unknown)
	{
		priTrackState = 1;
	}
	switch (priTrackState)
	{
	case 0:
	{
		break;
	}
	case 1:
	{
		track.pw.pwTrackingObj[0].AddNextPwValue(pdw.pulseWidth); // Pw tracking and update
		break;
	}
	case 2:
	{
		track.pw.pwTrackingObj[0].ResetPWAnalysisInfo();

		if (track.pri.priTrackingObj[0].priTypeSwitchFlag == 1)
		{
			track.pw.pwCircular.resize(0);
			track.pw.lastAnalysisSpotCount = 0;
			track.pw.pwFirst.resize(0);
		}
//
//		for (int32_t i = track.general.dspPdwBuffer.size()
//				- track.pri.priTrackingObj[0].lastAnalysisPulseCount; i>-1 &&
//				i < (int32_t) track.general.dspPdwBuffer.size(); i++)
//		{
//			track.pw.pwTrackingObj[0].AddNextPwValue(
//					track.general.dspPdwBuffer[i].pulseWidth);
//			if(track.pw.pwTrackingObj[0].lastAnalysisWasSuccessfulFlag == 1)
//			{
//				track.pw.pwAnalysisDoneOnPriBuffer = 1;
//			}
//		}
//		break;
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
				if (lockedToas[0]
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
			for (int32_t i = track.general.dspPdwBuffer.size()
					- track.pri.priTrackingObj[0].lastAnalysisPulseCount;
					i > -1 && i < (int32_t) track.general.dspPdwBuffer.size();
					i++)
			{
				track.pw.pwTrackingObj[0].AddNextPwValue(
						track.general.dspPdwBuffer[i].pulseWidth);
			}
			break;
		} //end default case
		} //end nested switch

		if (track.pw.pwTrackingObj[0].currPwType
				== SequenceTypesEnum::seq_unknown
				&& track.pw.pwTrackingObj[0].lastAnalysisWasSuccessfulFlag == 0)
		{
			track.pw.pwTrackingObj[0].TriggerPwAnalysis();
		}

		if (track.pw.pwTrackingObj[0].lastAnalysisWasSuccessfulFlag == 1)
		{
			track.pw.pwAnalysisDoneOnPriBuffer = 1;
		}
		else if (track.pw.pwFirst.size() == 0)
		{
			uint16_t tempPwValue =
					track.pw.pwTrackingObj[0].GetPeakPwFromHistogram();
			if (tempPwValue == 0)
			{
				tempPwValue =
						track.pw.pwTrackingObj[0].GetLastPwValueFromBuffer();
			}
			track.pw.pwAnalysisDoneOnPriBuffer = 1;
			LimitPwClass tempObj;
			tempObj.pwLow = tempPwValue;
			tempObj.pwHigh = tempPwValue;
			tempObj.pwAvg = tempPwValue;
			tempObj.pwType = PwTypeEnum::pw_fixed;
			track.pw.curPwType = PwTypeEnum::pw_fixed;
			track.pw.pwCircular.push_back(tempObj);
			track.pw.lastAnalysisSpotCount = 1;
			track.pw.pwFirst.resize(0);
			track.pw.pwFirst.push_back(tempObj);

		}
		break;
	} //end case

	} // end switch
}
void trackAssociateNewPdwWithTrackV2(TrackDefaultClass &track,
		PdwDefaultClass &pdw, uint64_t currSystemIteration)
{/* This function associates the pulse with its track and updates the track state
 * Note: if parameters of track have changed or updated the during pulse association,
 * if number of pulses that have belonged to the
 * track crosses parameters' analysis trigger (PRI,PW or RF), then corresponding
 *  parameter analysis are called i.e. RF Analysis, PRI analysis, PW analysis.
 */

	track.general.dspPdwBuffer.push_back(pdw);
	track.angle.pdwCountSinceLastAoaAnlaysis++;
	track.general.dspIterationPulseCount++;

	track.general.timeToaLastUpdate = pdw.timeOfArrivalStart;
	track.general.timeSysLastUpdate = Utility::getSystemTime();
	track.general.iterSysLastUpdate = currSystemIteration;

	track.general.toaLast = pdw.timeOfArrivalStart;

	track.pri.priTrackingObj[0].PushToa(pdw.timeOfArrivalStart); // Pri tracking and update

	track.pw.pwTrackingObj[0].AddNextPwValue(pdw.pulseWidth); // Pw tracking and update
	if (pdw.frequencyValidFlag == 1)
		track.rf.rfTrackingObj[0].AddNextRfValue(pdw.frequency,
				pdw.frequencyValidFlag);			// rf tracking and update

	if (track.angle.pdwCountSinceLastAoaAnlaysis >= ConstLow::asAoaPdwCountMin)
	{
		doaAnalysisExecute(track);	// AOA calculation and update
	}

	powerUpdateTrackPowersOnPulseAssociation(track, pdw);		// power update
}
void trackProcessOnIterationEnd(vector<TrackDefaultClass> &tracks,
		uint64_t currIteration)
{
	for (uint16_t i = 0; i < tracks.size(); i++)
	{
		tracks[i].general.dspIterationPulseCount = 0;
	}
}

void trackStopUpdatesOfSilentTracks(vector<TrackDefaultClass> &tracks,
		uint64_t currSystemIteration)
{	// silent the tracks with no activity for a while
	uint64_t timePassed = 0;
	for (uint16_t i = 0; i < tracks.size(); i++)
	{
		uint64_t timeToDeclareSilent = tracks[i].general.timeToDeclareSilent;
		if (tracks[i].general.dspTrackingEnabled == false)
		{
			continue;
		}
		if (ConstLow::simIterationTypeFlag
				== LowIterationTypeFlags::fixedTimeIterationFastSimulation)
		{
			uint64_t iterationDiff = currSystemIteration
					- tracks[i].general.iterSysLastUpdate;
			timePassed = iterationDiff * ConstLow::simIterationFixedTime * 1000;
		}
		else if (ConstLow::simIterationTypeFlag
				== LowIterationTypeFlags::readIterationTimeFromFile)
		{
			uint64_t currTime = Utility::getSystemTime();
			timePassed = currTime - tracks[i].general.timeSysLastUpdate;

			timeToDeclareSilent -= 1000;
			timeToDeclareSilent = min((uint64_t) 2000, timeToDeclareSilent);
			timeToDeclareSilent = max((uint64_t) 500, timeToDeclareSilent);
		}
		else
		{
			uint64_t currTime = Utility::getSystemTime();
			timePassed = currTime - tracks[i].general.timeSysLastUpdate;
			//timeToDeclareSilent = 50000;
			timeToDeclareSilent += 10000;
		}

		if (timePassed > timeToDeclareSilent)
		{
			tracks[i].general.dspTrackingEnabled = false;
		}
	}
}
void trackDeleteInactiveTracks(vector<TrackDefaultClass> &tracks)
{
//	//for performance boost we count number of tracks that are delete-able
	uint8_t deleteableCount = 0;
	for (uint16_t i = 0; i < tracks.size(); i++)
	{
		if (tracks[i].general.dspDeleteTrack == true)
		{
			deleteableCount++;
		}

	}
	if (deleteableCount == 0)
		return;

//	vector<TrackDefaultClass> newTracks;
//	newTracks.reserve(tracks.size() - deleteableCount);
//	for (uint16_t i = 0; i < tracks.size(); i++)
//	{
//		if (tracks[i].general.dspDeleteTrack == false)
//		{
//			newTracks.push_back(tracks[i]);
//		}
//	}
//	tracks = newTracks;
//
//	for (vector<TrackDefaultClass>::iterator i = tracks.begin();
//			i != tracks.end(); )
//	{
//		if (i->general.dspDeleteTrack == true)
//		{
//			i = tracks.erase(i);
//		}
//		else
//			i++;
//	}

	uint16_t validTrackCounter = 0;
	for (uint16_t i = 0; i < tracks.size(); i++)
	{
		if (tracks[i].general.dspDeleteTrack == true)
		{
			continue;
		}
		else
		{
			if (i != validTrackCounter)
			{
				tracks[validTrackCounter] = tracks[i];
//				std::copy_n(tracks.begin() + i, 1,
//						tracks.begin() + validTrackCounter);
			}
			validTrackCounter++;
		}
	}

	tracks.resize(validTrackCounter);
}
void trackCountStringableTracks(vector<TrackDefaultClass> &tracks,
		uint32_t &stringableTrackCount)
{
	//for performance boost we count number of tracks that are string-able

	uint32_t stringableCount = 0;
	for (uint16_t i = 0; i < tracks.size(); i++)
	{
		if (tracks[i].general.dspCandidateForStringingFlag == true)
		{
			stringableCount++;
		}
	}
	stringableTrackCount = stringableCount;
}
void trackUpdateParametersBeforeReporting(vector<TrackDefaultClass> &tracks,
		uint64_t currSystemIteration)
{
	for (uint16_t i = 0; i < tracks.size(); i++)
	{
		tracks[i].pri.priTrackingObj[0].CalledBeforeRporting();
		priCopyResultFromFromTrackingObject(tracks[i]);
		//priCopyResultFromFromTrackingObject(tracks[i]);
//		if (tracks[i].general.toaFirst == 140505046995) // DEBUGGING ONLY
//			tracks[i].general.toaFirst = 140505046995;
//		if (tracks[i].general.trackID == 729) // DEBUGGING ONLY
//			tracks[i].general.trackID = 729 * 1;
		powerUpdateLastStateOfPowerVariables(tracks[i]);
		if (tracks[i].power.groundIlluminationFlag == 0) // if ground illum not set but may be present
			powerUpdateGroundIlluminationFlags(tracks[i]);
		powerUpdatePowerFlagsBeforeReporting(tracks[i]);

//		if (ConstLow::simIterationTypeFlag
//				== LowIterationTypeFlags::readIterationTimeFromFile)
//		{
//			if ((currSystemIteration - tracks[i].general.iterSysLastUpdate)
//					> 100)
//			{
//				tracks[i].general.dspTrackingEnabled=false;
//			}
//		}
	}

}
void trackUpdateParametersAfterReporting(vector<TrackDefaultClass> &tracks,
		uint64_t currSystemIteration,bool reportSentFlag)
{
//	uint64_t timePassed;
	for (uint16_t i = 0; i < tracks.size(); i++)
	{

//		if (ConstLow::simIterationTypeFlag
//				== LowIterationTypeFlags::fixedTimeIterationFastSimulation)
//		{
//			uint64_t iterationDiff = currSystemIteration
//					- tracks[i].general.iterSysLastUpdate;
//			timePassed = iterationDiff * ConstLow::simIterationFixedTime * 1000;
//		}
//		else
//		{
//			uint64_t currTime = Utility::getSystemTime();
//			timePassed = currTime - tracks[i].general.timeSysLastUpdate;
//		}

		if (tracks[i].general.dspTrackingEnabled == false)
		{
			tracks[i].general.dspDeleteTrack = true;
		}
		if(tracks[i].general.flagSelfTest && reportSentFlag == false)
		{
			tracks[i].general.dspDeleteTrack = false;
		}
		tracks[i].general.iterStringTracksCount = 0;
		uint32_t priSpotsCount = tracks[i].pri.lastPriAnalysisSpotCount;
		if (priSpotsCount != 0)
		{
			tracks[i].pri.priCircular.resize(priSpotsCount);
		}
		uint32_t rfSpotCount = tracks[i].rf.lastAnalysisSpotCount;
		if (rfSpotCount != 0)
		{
			tracks[i].rf.rfCircular.resize(rfSpotCount);

		}
		uint32_t pwSpotCount = tracks[i].pw.lastAnalysisSpotCount;
		if (pwSpotCount != 0)
		{
//			if (tracks[i].pw.pwCircular.size() > pwSpotCount)
//			{
//				pwSpotCount = pwSpotCount * 1;
//				int kk = 0;
//			}
			tracks[i].pw.pwCircular.resize(pwSpotCount);

		}
		powerUpdatePowerFlagsAfterReporting(tracks[i]);
		tracks[i].general.dspCandidateForStringingFlag = false;
		tracks[i].pri.priTrackingObj[0].CalledOnReportingPeriodEnd();
	}

	trackDeleteInactiveTracks(tracks);
}
void trackDeleteTracksWithNoPri(vector<TrackDefaultClass> &tracks)
{
	for (uint16_t i = 0; i < tracks.size(); i++)
	{

		if (tracks[i].general.dspTrackingEnabled==false && tracks[i].pri.curPriType == PriTypeEnum::pri_uncomputed)
		{
			tracks[i].general.dspDeleteTrack = true;
		}
		tracks[i].general.dspCandidateForStringingFlag = false;
	}

	trackDeleteInactiveTracks(tracks);
}
