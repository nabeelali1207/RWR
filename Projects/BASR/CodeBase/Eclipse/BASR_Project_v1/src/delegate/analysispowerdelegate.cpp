/*
 * analysispowerdelegate.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#define ANALYSISPOWERDELEGATE_CPP_

#include "../delegate/analysispowerdelegate.h"

void powerDetermineWeights(uint32_t pa, uint32_t pb, uint32_t pc, uint32_t pd,
		float *destination)
{
	typedef std::pair<uint32_t, uint32_t> myPair;
	vector<myPair> powers(4);
	powers[0].first = pa;
	powers[0].second = 0;
	powers[1].first = pb;
	powers[1].second = 1;
	powers[2].first = pc;
	powers[2].second = 2;
	powers[3].first = pd;
	powers[3].second = 3;
	sort(powers.begin(), powers.end(), [](const myPair &l, const myPair &r)
	{	return l.first>r.first;});

	float weights[][4] =
	{
	{ 1, 0, 0, 0 },
	{ 0.6, 0.4, 0, 0 },
	{ 0.5, 0.25, 0.25, 0 },
	{ 0.5, 0.25, 0.25, 0 } };

	uint32_t nonzeroCount = 0;
	for (uint32_t i = 0; i < 4; i++)
	{
		if (powers[i].first > 120)
			nonzeroCount++;
	}
	nonzeroCount = (nonzeroCount > 0 ? nonzeroCount : 1); // such cases where all powers are zero
	uint32_t rowNumber = nonzeroCount - 1;

	for (uint32_t ii = 0; ii < 4; ii++)
	{
		destination[powers[ii].second] = weights[rowNumber][ii];
	}
}
void powerDetermineWeights_V2(TrackDefaultClass &track)
{
	int16_t avgAoa = track.angle.curAngle;
//    int16_t avgAoa = 355;

	int16_t oppositeAntenna[4] =
	{ 2, 3, 0, 1 };
	int16_t antennaAngles[4] =
	{ 315, 225, 135, 45 };

	int16_t dominantAntenna = avgAoa;
	dominantAntenna = 3 - dominantAntenna / 90;

	dominantAntenna = min((int16_t) 3, dominantAntenna);
	dominantAntenna = max((int16_t) 0, dominantAntenna);

	int16_t diffFromDominentAntenna = abs(
			antennaAngles[dominantAntenna] - avgAoa);

//	diffFromDominentAntenna = (diffFromDominentAntenna>180 ? 360 -diffFromDominentAntenna : diffFromDominentAntenna);
	int16_t secondaryAntenna = 0;

	if (antennaAngles[dominantAntenna] - avgAoa < 0)
	{
		secondaryAntenna = (dominantAntenna - 1 + 4) % 4;
	}
	else
	{
		secondaryAntenna = (dominantAntenna + 1) % 4;
	}

	int16_t fourthAntenna = oppositeAntenna[dominantAntenna];
	int16_t thirdAntenna = oppositeAntenna[secondaryAntenna];

	//constants
	float stepAngle = 5.0f;
	float stepCount = 45 / stepAngle;

	float domAntennaWeigthMax = 0.9;
	float domAntennaWeightMin = 0.5;
	float secAntennaWeigthMax = 0.5;
	float secAntennaWeightMin = 0.05;
	//  float thrdAntennaWeigthMax = 0.1;
	//  float thrdAntennaWeightMin = 0.0;

	int16_t stepDifference = abs(int16_t(diffFromDominentAntenna / stepAngle));
	float domSlope = (domAntennaWeigthMax - domAntennaWeightMin)
			/ (stepCount * stepCount);
	float secSlope = (secAntennaWeigthMax - secAntennaWeightMin)
			/ (stepCount * stepCount);
	// float thrdSlope = (thrdAntennaWeigthMax - thrdAntennaWeightMin) / stepCount;

	float domAntennaWeight = domAntennaWeigthMax
			- domSlope * stepDifference * stepDifference;
	float secAntennaWeight = secAntennaWeightMin
			+ secSlope * stepDifference * stepDifference;
	float thrdAntennaWeight = 1 - domAntennaWeight - secAntennaWeight;
	float fourthAntennaWeight = 0.0f;

	track.power.weightsVector[dominantAntenna] = domAntennaWeight;
	track.power.weightsVector[secondaryAntenna] = secAntennaWeight;
	track.power.weightsVector[thirdAntenna] = thrdAntennaWeight;
	track.power.weightsVector[fourthAntenna] = fourthAntennaWeight;

	//
	//    uint16_t avgPowers[4] = {0,0,0,0};
	//
	//    avgPowers[0]=track.power.avgPA;    avgPowers[1]=track.power.avgPB;
	//    avgPowers[2]=track.power.avgPC;    avgPowers[3]=track.power.avgPD;
	//
	//    if()
}
void powerUpdateTrackPowersOnPulseAssociation(TrackDefaultClass &track,
		PdwDefaultClass &newPdw)
{
	track.power.pulseCountSinceLastPowerAnalysis++;
	track.power.pulseCountSinceLastReport++;
	track.power.pulseCountSinceLastGroundIlluminationFlagsUpdate++;

//	track.power.trackPowerA.push_back(newPdw.powerA);
//	track.power.trackPowerB.push_back(newPdw.powerB);
//	track.power.trackPowerC.push_back(newPdw.powerC);
//	track.power.trackPowerD.push_back(newPdw.powerD);

	uint16_t avgPx = max(newPdw.powerA, newPdw.powerC);
	uint16_t avgPy = max(newPdw.powerB, newPdw.powerD);

	if ((newPdw.trackingPowerScore > 60 || newPdw.clusterProcessFlag == true)
			&& (avgPx + avgPy > track.power.pxMax + track.power.pyMax))
	{
//		if (track.general.trackID == 0)
//		{
//			track.general.trackID = 1 - 1;
//		}
		track.power.pxMax = avgPx;
		track.power.pyMax = avgPy;
		track.power.quadrantCodeMaxPxPy = newPdw.quadrantCode;
		track.power.maxPowerToa = newPdw.timeOfArrivalStart;
	}

	if (track.power.pulseCountSinceLastPowerAnalysis
			>= ConstLow::asPowerPdwCountMin)
	{
		powerAnalysisExecute(track);
		powerUpdateDominantAntennas(track);
		track.power.pulseCountSinceLastPowerAnalysis = 0;
	}
	if (track.power.pulseCountSinceLastGroundIlluminationFlagsUpdate
			>= ConstLow::asGroundIllumPdwCountMin)
	{
		// Ground illumination detection
		powerUpdateGroundIlluminationFlags(track);
		resetDominantAntennaCounts(track);
		track.power.pulseCountSinceLastGroundIlluminationFlagsUpdate = 0; // reset
	}
	track.power.lastPa = newPdw.powerA;
	track.power.lastPb = newPdw.powerB;
	track.power.lastPc = newPdw.powerC;
	track.power.lastPd = newPdw.powerD;

	track.power.countQuadrantCodesIter[newPdw.quadrantCode]++;

//	if (newPdw.powerA > newPdw.powerB && newPdw.powerA > newPdw.powerD
//			&& newPdw.powerA > newPdw.powerD)
//		track.power.countDominantAntenna[0]++;
//	else if (newPdw.powerB > newPdw.powerA && newPdw.powerB > newPdw.powerC
//			&& newPdw.powerB > newPdw.powerD)
//		track.power.countDominantAntenna[1]++;
//	else if (newPdw.powerC > newPdw.powerB && newPdw.powerC > newPdw.powerD
//			&& newPdw.powerC > newPdw.powerA)
//		track.power.countDominantAntenna[2]++;
//	else
//		track.power.countDominantAntenna[3]++;

	track.power.countDominantAntenna[newPdw.dominantAntennaCode]++;
	track.power.countSecDominantAntenna[newPdw.secondaryAntennaCode]++;

//	uint16_t pdwPowers[4] =
//	{ newPdw.powerA, newPdw.powerB, newPdw.powerC, newPdw.powerD };
//	track.power.powerHistogram[0].addToHistogram(
//			pdwPowers[track.power.dominantAntenna]+pdwPowers[track.power.secondaryAntenna]);

	track.power.powerHistogram[0].addToHistogram(newPdw.powerX + newPdw.powerY);
	track.power.scanAnalysis[0].FillScanBuffer(newPdw);
	track.power.scanAnalysis2[0].addNextPdw(newPdw);

}
void powerUpdateGroundIlluminationFlags(TrackDefaultClass &track)
{
	if ((track.power.countDominantAntenna[0] > 3
			&& track.power.countSecDominantAntenna[2] > 3)
			|| (track.power.countDominantAntenna[2] > 3
					&& track.power.countSecDominantAntenna[0] > 3)) // if both A and C have powers
	{
		track.power.groundIlluminationFlag = 1;
		track.power.groundIlluminationAxis = GroundIlluminationEnum::x_axis; // x-axis
	}
	else if ((track.power.countDominantAntenna[1] > 3
			&& track.power.countSecDominantAntenna[3] > 3)
			|| (track.power.countDominantAntenna[3] > 3
					&& track.power.countSecDominantAntenna[1] > 3)) // if both B and D have powers
	{
		track.power.groundIlluminationFlag = 1;
		track.power.groundIlluminationAxis = GroundIlluminationEnum::y_axis; // y axis
	}

}
void powerUpdateDominantAntennas(TrackDefaultClass &track)
{
	uint16_t countDominantAntenna[4]; // dominant antenna
	uint16_t countSecDominantAntenna[4]; // dominant antenna
	vector<PdwDefaultClass> pdwInput = powerGetPdwsForAnalysis(track);

	typedef std::pair<uint16_t, uint8_t> myPair;
	vector<myPair> powers(4);
	memset(countDominantAntenna, 0, sizeof(countDominantAntenna));
	memset(countSecDominantAntenna, 0, sizeof(countSecDominantAntenna));
	for (uint16_t i = 0; i < pdwInput.size(); i++)
	{

		countDominantAntenna[pdwInput[i].dominantAntennaCode]++;
		countSecDominantAntenna[pdwInput[i].secondaryAntennaCode]++;
	}
	// Dominant and secondary antenna calculation

	// Dominant antenna
	powers[0].first = countDominantAntenna[0];
	powers[0].second = 0;
	powers[1].first = countDominantAntenna[1];
	powers[1].second = 1;
	powers[2].first = countDominantAntenna[2];
	powers[2].second = 2;
	powers[3].first = countDominantAntenna[3];
	powers[3].second = 3;
	sort(powers.begin(), powers.end(), [](const myPair &l, const myPair &r)
	{	return l.first>r.first;});
	track.power.dominantAntenna = powers[0].second;

	// secondary antenna
	powers[0].first = countSecDominantAntenna[0];
	powers[0].second = 0;
	powers[1].first = countSecDominantAntenna[1];
	powers[1].second = 1;
	powers[2].first = countSecDominantAntenna[2];
	powers[2].second = 2;
	powers[3].first = countSecDominantAntenna[3];
	powers[3].second = 3;
	sort(powers.begin(), powers.end(), [](const myPair &l, const myPair &r)
	{	return l.first>r.first;});
	track.power.secondaryAntenna = powers[0].second;
	// px py antennas
	if (track.power.dominantAntenna == 0 || track.power.dominantAntenna == 2)
		track.power.pxAntenna = track.power.dominantAntenna;
	else
		track.power.pyAntenna = track.power.dominantAntenna;
	if (track.power.secondaryAntenna == 0 || track.power.secondaryAntenna == 2)
		track.power.pxAntenna = track.power.secondaryAntenna;
	else
		track.power.pyAntenna = track.power.secondaryAntenna;
}
void resetDominantAntennaCounts(TrackDefaultClass &track)
{
	memset(track.power.countDominantAntenna, 0,
			sizeof(track.power.countDominantAntenna));
	memset(track.power.countSecDominantAntenna, 0,
			sizeof(track.power.countSecDominantAntenna));

}


void powerUpdateLastStateOfPowerVariables(TrackDefaultClass &track)
{
	uint16_t medianArray[3];
	uint16_t lastIndex = track.general.dspPdwBuffer.size() - 1;
	medianArray[0] = track.general.dspPdwBuffer[lastIndex].powerX;
	medianArray[1] = track.general.dspPdwBuffer[lastIndex - 1].powerX;
	medianArray[2] = track.general.dspPdwBuffer[lastIndex - 2].powerX;
	track.power.pxValueLast = ::medianFilter3<uint16_t>(medianArray);

	medianArray[0] = track.general.dspPdwBuffer[lastIndex].powerY;
	medianArray[1] = track.general.dspPdwBuffer[lastIndex - 1].powerY;
	medianArray[2] = track.general.dspPdwBuffer[lastIndex - 2].powerY;
	track.power.pyValueLast = medianFilter3<uint16_t>(medianArray);

	medianArray[0] = track.general.dspPdwBuffer[lastIndex].rfAmplitude;
	medianArray[1] = track.general.dspPdwBuffer[lastIndex - 1].rfAmplitude;
	medianArray[2] = track.general.dspPdwBuffer[lastIndex - 2].rfAmplitude;
	track.rf.rfAmplitude = (uint32_t)::medianFilter3<uint16_t>(medianArray);

	PdwDefaultClass &currPdw = track.general.dspPdwBuffer.back();
	if (currPdw.powerA >= currPdw.powerC)
	{
		track.power.pxAntennaLast = 0;
	}
	else
	{
		track.power.pxAntennaLast = 2;
	}
	if (currPdw.powerB >= currPdw.powerD)
	{
		track.power.pyAntennaLast = 1;
	}
	else
	{
		track.power.pyAntennaLast = 2;
	}

	track.power.quadrantCodeLast = currPdw.quadrantCode;
	track.power.primaryAntennaLast = currPdw.antennaCode;
}
void powerUpdateFirstStateOfPowerVariables(TrackDefaultClass &track)
{
	uint16_t medianArray[3];

	medianArray[0] = track.general.dspPdwBuffer[0].powerX;
	medianArray[1] = track.general.dspPdwBuffer[1].powerX;
	medianArray[2] = track.general.dspPdwBuffer[2].powerX;
	track.power.pxValueFirst = medianFilter3<uint16_t>(medianArray);

	medianArray[0] = track.general.dspPdwBuffer[0].powerY;
	medianArray[1] = track.general.dspPdwBuffer[1].powerY;
	medianArray[2] = track.general.dspPdwBuffer[2].powerY;
	track.power.pyValueFirst = medianFilter3<uint16_t>(medianArray);

	PdwDefaultClass &currPdw = track.general.dspPdwBuffer.front();
	if (currPdw.powerA >= currPdw.powerC)
	{
		track.power.pxAntennaFirst = 0;
	}
	else
	{
		track.power.pxAntennaFirst = 2;
	}
	if (currPdw.powerB >= currPdw.powerD)
	{
		track.power.pyAntennaFirst = 1;
	}
	else
	{
		track.power.pyAntennaFirst = 2;
	}

	track.power.quadrantCodeFirst = currPdw.quadrantCode;
	track.power.primaryAntennaFirst = currPdw.antennaCode;
}
void powerUpdatePowerFlagsAfterReporting(TrackDefaultClass &track)
{
	track.power.pulseCountSinceLastReport = 0;
	PdwDefaultClass &lastPdw = track.general.dspPdwBuffer.back();
	track.power.pxMax = lastPdw.powerX;
	track.power.pyMax = lastPdw.powerY;
	track.power.maxPowerToa = lastPdw.timeOfArrivalStart;
	track.power.countQuadrantCodesIter[0] = 0;
	track.power.countQuadrantCodesIter[1] = 0;
	track.power.countQuadrantCodesIter[2] = 0;
	track.power.countQuadrantCodesIter[3] = 0;
	track.power.groundIlluminationFlag = 0;
	track.power.groundIlluminationAxis = GroundIlluminationEnum::no_axis; // no axis

}
void powerUpdatePowerFlagsBeforeReporting(TrackDefaultClass &track)
{
//	if (track.general.toaLast == 214160099685)
//		track.general.toaLast = track.general.toaLast * 1;
	powerUpdateMaximumPowers(track);
	powerScanAnalysis(track);
}
void powerUpdateMaximumPowers(TrackDefaultClass &track)
{
//	if (track.general.toaLast == 2180097845)
//		track.general.toaLast = 2180097845;
	if (track.power.powerHistogram[0].inputSampleCount > 8)
	{
		track.power.powerHistogram[0].findPeaks();
		if (track.power.powerHistogram[0].getNumberOfPeaksFound() > 0)
		{
			uint16_t maxPower =
					track.power.powerHistogram[0].findMaxPowerFromPeaks();
			PowerHistogramPeak &peakDescription =
					track.power.powerHistogram[0].getFoundPeak();

			int16_t bufferSize = track.general.dspPdwBuffer.size();
			uint16_t startIndex =
					max(0,
							bufferSize
									- (int16_t) track.power.powerHistogram[0].inputSampleCount);
			uint16_t endIndex = bufferSize;

			bool closedMatchValidFlag = false;
			uint16_t closestMatchDiff = 0xFFFF;
			uint16_t closestMatchPx = 0;
			uint16_t closestMatchPy = 0;
			uint16_t closestMatchQuad = 0;

			for (uint16_t i = startIndex; i < endIndex; i++)
			{
//				if ((track.general.dspPdwBuffer[i].powerX
//						+ track.general.dspPdwBuffer[i].powerY) == maxPower)
//				{
//					track.power.pxMax = track.general.dspPdwBuffer[i].powerX;
//					track.power.pyMax = track.general.dspPdwBuffer[i].powerY;
//					track.power.quadrantCodeMaxPxPy =
//							track.general.dspPdwBuffer[i].quadrantCode;
//					break;
//				}

				if (track.general.dspPdwBuffer[i].trackingPowerScore < 60)
					continue;

				uint16_t currSum = track.general.dspPdwBuffer[i].powerX
						+ track.general.dspPdwBuffer[i].powerY;
				uint16_t diffFromMaxPower;
				diffFromMaxPower = abs((int16_t) currSum - (int16_t) maxPower);

				if (currSum < peakDescription.rangeMinValue)
					continue;
				if (currSum > peakDescription.rangeMaxValue)
					continue;

				if (diffFromMaxPower < closestMatchDiff)
				{
					closedMatchValidFlag = true;
					closestMatchDiff = diffFromMaxPower;
					closestMatchPx = track.general.dspPdwBuffer[i].powerX;
					closestMatchPy = track.general.dspPdwBuffer[i].powerY;
					closestMatchQuad =
							track.general.dspPdwBuffer[i].quadrantCode;

				}
			}
			if (closedMatchValidFlag == true)
			{
				track.power.pxMax = closestMatchPx;
				track.power.pyMax = closestMatchPy;
				track.power.quadrantCodeMaxPxPy = closestMatchQuad;
			}
		}
	}
	track.power.powerHistogram[0].clearHistogram();

}
void powerScanAnalysis(TrackDefaultClass &track)
{
//	int32_t bufferSize = track.general.dspPdwBuffer.size();
//	uint32_t startIndex = max(0,
//			bufferSize - track.power.pulseCountSinceLastScanAnalysis);
//	uint32_t endIndex = bufferSize;
//	if (track.general.trackID == 104)	// DEBUGGING ONLY
//		track.general.trackID = 103 + 1;
//	if (track.general.toaLast == 221640071880)	// DEBUGGING ONLY
//			track.general.toaLast = 221640071880;
//	for (uint32_t i = startIndex; i < endIndex; i++)
//	{
//		track.power.scanAnalysis[0].FillScanBuffer(
//				track.general.dspPdwBuffer[i]);
//	}
	track.power.scanAnalysis2[0].performScanAnalysis();
	vector<IlluminationCycleInfo> & temp=track.power.scanAnalysis2[0].getCycleInfo();
	track.power.illuminationInfo.resize(0);
	track.power.illuminationInfo.insert(track.power.illuminationInfo.begin(),temp.begin(),temp.end());

	EmitterMode emitterMode, scanType;
	uint16_t maxPower = max(track.power.pxMax, track.power.pyMax);
	track.power.scanAnalysis[0].SetMaxPower(maxPower);
	track.power.scanAnalysis[0].ScanAnalysis(track.power.dominantAntenna);
	emitterMode = track.power.scanAnalysis[0].GetEmitterMode();
	if(emitterMode == EmitterMode::scan)
	{
		scanType = track.power.scanAnalysis[0].DetectScanType();
	}
	switch (scanType)
	{
	case EmitterMode::scan:
		track.power.scanTypeLast = EmitterArpEnum::scan_circular;
		break;
	case EmitterMode::track:
		track.power.scanTypeLast = EmitterArpEnum::scan_lock;
		break;
	case EmitterMode::sector:
			track.power.scanTypeLast = EmitterArpEnum::scan_sectorial;
			break;
	case EmitterMode::conical:
		track.power.scanTypeLast = EmitterArpEnum::scan_sectorial;
		break;
	case EmitterMode::unknown:
		track.power.scanTypeLast = EmitterArpEnum::scan_unknown;
		break;
	}
//track.power.pulseCountSinceLastScanAnalysis = 0;
}
vector<PdwDefaultClass> powerGetPdwsForAnalysis(TrackDefaultClass &track)
{
	int32_t inputSize = track.power.pulseCountSinceLastPowerAnalysis;
	int32_t bufferSize = track.general.dspPdwBuffer.size();

	vector<PdwDefaultClass> pdwInput(inputSize);

	int32_t startPoint = bufferSize - inputSize;
	int32_t endPoint = bufferSize;

	for (uint16_t i = startPoint, j = 0; i < endPoint; i++, j++)
	{
		pdwInput[j] = track.general.dspPdwBuffer[i];
	}
	return pdwInput;
}
void powerAnalysisExecute(TrackDefaultClass &track)
{

	vector<PdwDefaultClass> pdwInput = powerGetPdwsForAnalysis(track);
	uint16_t inputSize = pdwInput.size();

	if (inputSize < 3) //just a safety check, not bound to happen
		return;

	uint32_t avgPA = 0;
	uint32_t avgPB = 0;
	uint32_t avgPC = 0;
	uint32_t avgPD = 0;

	uint32_t validPdwCount = pdwInput.size() - 2;
	for (uint16_t i = 1; i < inputSize - 1; i++)
	{
		uint16_t lastPA[3] =
		{ pdwInput[i - 1].powerA, pdwInput[i].powerA, pdwInput[i + 1].powerA };
		avgPA += medianFilter3(lastPA);

		uint16_t lastPB[3] =
		{ pdwInput[i - 1].powerB, pdwInput[i].powerB, pdwInput[i + 1].powerB };
		avgPB += medianFilter3(lastPB);

		uint16_t lastPC[3] =
		{ pdwInput[i - 1].powerC, pdwInput[i].powerC, pdwInput[i + 1].powerC };
		avgPC += medianFilter3(lastPC);

		uint16_t lastPD[3] =
		{ pdwInput[i - 1].powerD, pdwInput[i].powerD, pdwInput[i + 1].powerD };
		avgPD += medianFilter3(lastPD);
	}

	avgPA = (avgPA / validPdwCount);
	avgPB = (avgPB / validPdwCount);
	avgPC = (avgPC / validPdwCount);
	avgPD = (avgPD / validPdwCount);

	uint32_t stdDevPA = 0;
	uint32_t stdDevPB = 0;
	uint32_t stdDevPC = 0;
	uint32_t stdDevPD = 0;

	for (uint16_t i = 1; i < inputSize - 1; i++)
	{

		int32_t temp;
		uint16_t lastPA[3] =
		{ pdwInput[i - 1].powerA, pdwInput[i].powerA, pdwInput[i + 1].powerA };
		temp = (medianFilter3(lastPA) - avgPA);
		stdDevPA += (temp * temp);

		uint16_t lastPB[3] =
		{ pdwInput[i - 1].powerB, pdwInput[i].powerB, pdwInput[i + 1].powerB };
		temp = (medianFilter3(lastPB) - avgPB);
		stdDevPB += (temp * temp);

		uint16_t lastPC[3] =
		{ pdwInput[i - 1].powerC, pdwInput[i].powerC, pdwInput[i + 1].powerC };
		temp = (medianFilter3(lastPC) - avgPC);
		stdDevPC += (temp * temp);

		uint16_t lastPD[3] =
		{ pdwInput[i - 1].powerD, pdwInput[i].powerD, pdwInput[i + 1].powerD };
		temp = (medianFilter3(lastPD) - avgPD);
		stdDevPD += (temp * temp);
	}

	stdDevPA = sqrt(stdDevPA / validPdwCount);
	stdDevPB = sqrt(stdDevPB / validPdwCount);
	stdDevPC = sqrt(stdDevPC / validPdwCount);
	stdDevPD = sqrt(stdDevPD / validPdwCount);

	stdDevPA = (stdDevPA < 1 ? 1 : stdDevPA);
	stdDevPB = (stdDevPB < 1 ? 1 : stdDevPB);
	stdDevPC = (stdDevPC < 1 ? 1 : stdDevPC);
	stdDevPD = (stdDevPD < 1 ? 1 : stdDevPD);

	track.power.avgPA = avgPA;
	track.power.avgPB = avgPB;
	track.power.avgPC = avgPC;
	track.power.avgPD = avgPD;

	track.power.stdPA = stdDevPA;
	track.power.stdPB = stdDevPB;
	track.power.stdPC = stdDevPC;
	track.power.stdPD = stdDevPD;

//find dominant antenna and find weights
//	powerDetermineWeights(avgPA, avgPB, avgPC, avgPD,
//			track.power.weightsVector);

	powerDetermineWeights_V2(track);

}
