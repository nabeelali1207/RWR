/*
 * angledelegate.cpp
 *
 *  Created on: Oct 29, 2018
 *      Author: hassan
 */

#define ANALYSISANGLEDELEGATE_CPP_

#include "../delegate/analysisangledelegate.h"

vector<PdwDefaultClass> doaGetPdwsForAnalysis(TrackDefaultClass &track)
{
	int32_t inputSize = track.angle.pdwCountSinceLastAoaAnlaysis;
	int32_t bufferSize = track.general.dspPdwBuffer.size();
	inputSize = min(inputSize, bufferSize);

	vector<PdwDefaultClass> pdwInput(inputSize);

	int32_t startPoint = bufferSize - inputSize;
	int32_t endPoint = bufferSize;

	for (uint16_t i = startPoint, j = 0; i < endPoint; i++, j++)
	{
		pdwInput[j] = track.general.dspPdwBuffer[i];
	}

	return pdwInput;
}

void doaAnalysisExecute(TrackDefaultClass &track)
{
	vector<PdwDefaultClass> pdwInput = doaGetPdwsForAnalysis(track); // pick AOAs from buffer
	track.angle.pdwCountSinceLastAoaAnlaysis = 0;

	int32_t avgAOA = 0, avgCount = 0;
	int16_t referenceAngle = pdwInput[0].angleOfArrival;
	uint16_t minAOA = pdwInput[0].angleOfArrival, maxAOA =
			pdwInput[0].angleOfArrival;

	for (uint16_t i = 0; i < pdwInput.size(); i++)
	{
		int16_t diff = ((int16_t) pdwInput[i].angleOfArrival - referenceAngle);
		diff = (diff > 180 ? 360 - diff : diff);
		diff = (diff < -180 ? 360 + diff : diff);
		avgAOA += diff;
		avgCount++;
		if (pdwInput[i].angleOfArrival > maxAOA)
			maxAOA = pdwInput[i].angleOfArrival;
		if (pdwInput[i].angleOfArrival < minAOA)
			minAOA = pdwInput[i].angleOfArrival;
	}
	avgAOA = referenceAngle + (avgAOA / avgCount);
	avgAOA = (avgAOA > 360 ? avgAOA - 360 : avgAOA);
	track.angle.curAngle = avgAOA;
	track.angle.timeToaAngleUpdate = pdwInput.back().timeOfArrivalStart;
	track.angle.minimumAngle = minAOA;
	track.angle.maximumAngle = maxAOA;
}

uint16_t doaCalculateAngle(uint16_t px, uint16_t py, uint8_t quadrantCode,
		uint8_t bandCode, uint8_t bandDivision)
{

	bandDivision = 2;
	px = px / 16;
	py = py / 16;

	uint16_t angleOfArrival = 0;
	if (bandDivision == 1)
	{
		uint16_t delta;
		delta = (uint16_t) (px - py);
		delta = delta >> 4;
		angleOfArrival = AngleTableOneBand[(quadrantCode << 12) | delta];
	}
	else if (bandDivision == 2)
	{
		uint16_t r;
		uint8_t AOA;
		int16_t PxMinusPy;

		if ((px == 0) || (py == 0))
		{
			if (px > py)
			{  // If py is lower than the threshold
				if ((quadrantCode == 0) || (quadrantCode == 2)) /* QuadCode 0 or 2 */
					AOA = 0x20;  //32 units
				else
					/* QuadCode 1 or 3 */
					AOA = 0xA0;  //160 units
			}
			else
			{  // If px is lower than the threshold
				if ((quadrantCode == 2) || (quadrantCode == 3)) /* QuadCode 2 or 3 */
					AOA = 0xE0;  //224 units
				else
					/* QuadCode 0 or 1 */
					AOA = 0x60;	//96 units
			}
		}
		else
		{
			PxMinusPy = px - py;
			if (PxMinusPy > 63)
				PxMinusPy = 63;
			else if (PxMinusPy < -64)
				PxMinusPy = -64;
			PxMinusPy = (uint16_t) (PxMinusPy);
			r = (uint16_t) ((uint16_t) (PxMinusPy) & 0x7f); // 7 bits allowed
			AOA = AngleTableTwoBand[(quadrantCode << 7) | r];
		}
		angleOfArrival = AOA;
	}
	else if (bandDivision == 4)
	{

		uint8_t aoa = 0;
		uint16_t r;

		//px = px/16;
		//py = py/16;

		if ((px < 10) || (py < 10))
		{
			if (px > py)
			{  // If py is lower than the threshold
				if ((quadrantCode == 0) || (quadrantCode == 2)) /* QuadCode 0 or 2 */
					aoa = 0x20;  //32 units
				else
					/* QuadCode 1 or 3 */
					aoa = 0xA0;  //160 units
			}
			else
			{  // If px is lower than the threshold
				if ((quadrantCode == 2) || (quadrantCode == 3)) /* QuadCode 2 or 3 */
					aoa = 0xE0;  //224 units
				else
					/* QuadCode 0 or 1 */
					aoa = 0x60;	//96 units
			}
		}
		else
		{
			r = (UWord16) ((px - py) & 0xff);
			if (bandCode == 0)
				aoa = AngleTableFourBand[((quadrantCode << 8) | r)];
			else if (bandCode == 1)
				aoa = AngleTableFourBand[1024 + ((quadrantCode << 8) | r)];
			else if (bandCode == 2)
				aoa = AngleTableFourBand[2048 + ((quadrantCode << 8) | r)];
			else if (bandCode == 3)
				aoa = AngleTableFourBand[3072 + ((quadrantCode << 8) | r)];
		}
		angleOfArrival = aoa;
	}
	//return angleOfArrival;
	return (360 -((uint16_t) ((((float) angleOfArrival) / 255.0) * 360.0)));
}
uint16_t doaCalculateAngle_Low(uint16_t pa, uint16_t pb, uint16_t pc,
		uint16_t pd, uint8_t quadrantCode, uint8_t bandCode,
		uint8_t bandDivision)
{

	bandDivision = 2;

	uint16_t px = max(pa, pc) / 16;
	uint16_t py = max(pb, pd) / 16;
	uint16_t pz = 0;

	uint16_t angleOfArrival = 0;
	if (bandDivision == 1)
	{
		uint16_t delta;
		delta = (uint16_t) (px - py);
		delta = delta >> 4;
		angleOfArrival = AngleTableOneBand[(quadrantCode << 12) | delta];
	}
	else if (bandDivision == 2)
	{
		uint16_t r;
		uint8_t AOA;
		int16_t PxMinusPy;

		if ((px == 0) || (py == 0))
		{
			if (px > py)
			{  // If py is lower than the threshold
				if ((quadrantCode == 0) || (quadrantCode == 2)) /* QuadCode 0 or 2 */
					AOA = 0x20;  //32 units
				else
					/* QuadCode 1 or 3 */
					AOA = 0xA0;  //160 units
			}
			else
			{  // If px is lower than the threshold
				if ((quadrantCode == 2) || (quadrantCode == 3)) /* QuadCode 2 or 3 */
					AOA = 0xE0;  //224 units
				else
					/* QuadCode 0 or 1 */
					AOA = 0x60;	//96 units
			}
		}
		else
		{
			PxMinusPy = px - py;
			if (PxMinusPy > 63)
				PxMinusPy = 63;
			else if (PxMinusPy < -64)
				PxMinusPy = -64;
			PxMinusPy = (uint16_t) (PxMinusPy);
			r = (uint16_t) ((uint16_t) (PxMinusPy) & 0x7f); // 7 bits allowed
			AOA = AngleTableTwoBand[(quadrantCode << 7) | r];
		}
		angleOfArrival = AOA;
	}
	else if (bandDivision == 4)
	{

		uint8_t aoa = 0;
		uint16_t r;

		//px = px/16;
		//py = py/16;

		if ((px < 10) || (py < 10))
		{
			if (px > py)
			{  // If py is lower than the threshold
				if ((quadrantCode == 0) || (quadrantCode == 2)) /* QuadCode 0 or 2 */
					aoa = 0x20;  //32 units
				else
					/* QuadCode 1 or 3 */
					aoa = 0xA0;  //160 units
			}
			else
			{  // If px is lower than the threshold
				if ((quadrantCode == 2) || (quadrantCode == 3)) /* QuadCode 2 or 3 */
					aoa = 0xE0;  //224 units
				else
					/* QuadCode 0 or 1 */
					aoa = 0x60;	//96 units
			}
		}
		else
		{
			r = (UWord16) ((px - py) & 0xff);
			if (bandCode == 0)
				aoa = AngleTableFourBand[((quadrantCode << 8) | r)];
			else if (bandCode == 1)
				aoa = AngleTableFourBand[1024 + ((quadrantCode << 8) | r)];
			else if (bandCode == 2)
				aoa = AngleTableFourBand[2048 + ((quadrantCode << 8) | r)];
			else if (bandCode == 3)
				aoa = AngleTableFourBand[3072 + ((quadrantCode << 8) | r)];
		}
		angleOfArrival = aoa;
	}

	if (abs((int16_t) px - (int16_t) py) < 20)
	{
		uint16_t centreAngles[4] =
		{ 63, 126, 189, 0 };
		angleOfArrival = centreAngles[quadrantCode & 0x3];
	}
	else if (px > py)
	{
		pz = min(pb, pd) / 16;

		if (abs((int16_t) py - (int16_t) pz) < 20)
		{
			if (pa > pc)
			{
				angleOfArrival = 0x20;
			}
			else
			{
				angleOfArrival = 0xA0;
			}
		}
	}
	else
	{
		pz = min(pa, pc) / 16;

		if (abs((int16_t) px - (int16_t) pz) < 20)
		{
			if (pb > pd)
			{
				angleOfArrival = 0x60;
			}
			else
			{
				angleOfArrival = 0xE0;
			}
		}
	}

	//return angleOfArrival;
	return (/*360 -*/((uint16_t) ((((float) angleOfArrival) / 255.0) * 360.0)));
}

uint16_t doaRoundAngleDifference(uint16_t angleInput)
{
	if (angleInput > 359)
		angleInput = 359;
	if (angleInput > 180)
		return (359 - angleInput);
	return angleInput;
}

//fix multiplication values with constants
uint8_t doaFlagPxPyCompatibleCheck(uint16_t pxOne, uint16_t pxTwo,
		uint16_t pyOne, uint16_t pyTwo)
{
	uint16_t pxpyDifferenceOne;
	uint16_t pxpyDifferenceTwo;

	if (((pxOne >= pyOne) && (pxTwo >= pyTwo))
			|| ((pyOne >= pxOne) && (pyTwo >= pxTwo)))
	{
		pxpyDifferenceOne = labs((int16_t)pxOne - (int16_t)pyOne);
		pxpyDifferenceTwo = labs((int16_t)pxTwo - (int16_t)pyTwo);

		if ((pxpyDifferenceOne < (8 * 16)) && (pxpyDifferenceTwo > (60 * 16)))
			return 0;
		if ((pxpyDifferenceTwo < (8 * 16)) && (pxpyDifferenceOne > (60 * 16)))
			return 0;
		if(pxpyDifferenceTwo < 400 && pxpyDifferenceOne < 400)
			return 0;
		return 1;
	}
	return 0;
}

uint8_t doaFlagQuadrantAdjacencyCheck(uint8_t qCodeOne, uint8_t qCodeTwo)
{
	if (qCodeOne == 0)
	{
		if (qCodeTwo == 1)
			return 1;
		else if (qCodeTwo == 2)
			return 1;
	}
	else if (qCodeOne == 1)
	{
		if (qCodeTwo == 0)
			return 1;
		else if (qCodeTwo == 3)
			return 1;
	}
	else if (qCodeOne == 2)
	{
		if (qCodeTwo == 0)
			return 1;
		else if (qCodeTwo == 3)
			return 1;
	}
	else if (qCodeOne == 3)
	{
		if (qCodeTwo == 1)
			return 1;
		else if (qCodeTwo == 2)
			return 1;
	}
	return 0;
}

