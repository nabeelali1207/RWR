/*
 * analysisspurrejectiondelegate.cpp
 *
 *  Created on: Mar 4, 2021
 *      Author: gmd
 */

#include "../delegate/analysisspurrejectiondelegate.h"
#define ANALYSISSPURREJECTIONDELEGATE_CPP_
bool rangeMatch(uint32_t a, uint32_t b, uint32_t x, uint32_t y,
		uint32_t devAllowed)
{
	return ((a - devAllowed) < x) && (y < (b + devAllowed));
}
bool detectSpurs(TrackDefaultClass &first, TrackDefaultClass &second)
{
//first track has to be of lower rf,
//second track will be tested as being Spur of first track
	uint16_t vdmPowerFirst = max(first.power.pxMax, first.power.pyMax);
	if (first.rf.rfValue > 10000)
	{
		return false;
	}
	if (vdmPowerFirst < 1900)
	{
		return false;
	}
	if (first.rf.curRfType == RfTypeEnum::rf_uncomputed)
	{
		return false;
	}
	if (second.rf.curRfType == RfTypeEnum::rf_uncomputed)
	{
		return false;
	}

	uint32_t devAllowed = 10;
	uint32_t minThresholdVal = first.rf.getRfMinSpotLow().rfLow;
	uint32_t maxThresholdVal = first.rf.getRfMaxSpotLow().rfHigh;

	uint32_t testMin = second.rf.getRfMinSpotLow().rfLow;
	uint32_t testMax = second.rf.getRfMaxSpotLow().rfHigh;
	bool isSecondMultiple = rangeMatch(2 * minThresholdVal, 2 * maxThresholdVal,
			testMin, testMax, devAllowed);
	bool isThirdMultiple = rangeMatch(3 * minThresholdVal, 3 * maxThresholdVal,
			testMin, testMax, devAllowed);

	if (isSecondMultiple || isThirdMultiple)
	{

	}
	else
	{
		return false;
	}

	return true;
}
void analysisRfSpurRejection(vector<TrackDefaultClass> &tracks)
{
	uint32_t spurCount = 0;
	for (uint16_t i = 0; i < tracks.size(); i++)
	{
		if (tracks[i].general.dspDeleteTrack == true)
		{
			continue;
		}
		for (uint16_t j = i + 1; j < tracks.size(); j++)
		{
			if (tracks[j].general.dspDeleteTrack == true)
			{
				continue;
			}

			if (tracks[i].rf.rfValue < tracks[j].rf.rfValue)
			{
				bool isSpurTrack = detectSpurs(tracks[i], tracks[j]);
				if (isSpurTrack)
				{
					tracks[j].general.dspDeleteTrack = true;
					spurCount++;
				}
			}
			else
			{
				bool isSpurTrack = detectSpurs(tracks[j], tracks[i]);
				if (isSpurTrack)
				{
					tracks[i].general.dspDeleteTrack = true;
					spurCount++;
				}
			}
		}
	}

	if (spurCount > 0)
	{
		cout
				<< "================================--------Deleted Tracks--------================================\n";
		cout
				<< "ToaFirst\tToaLast\t\tBand\tAngle\tPulsCnt\tPRIType\tPRICnt\tPRI\tPW\tRF\tVdmAmp\tRfAmp\tSTFlag\tQuad\n";
	}

	for (uint16_t i = 0; i < tracks.size(); i++)
	{
		TrackDefaultClass &track = tracks[i];
		if (track.general.dspDeleteTrack != true)
		{
			continue;
		}
		cout << std::setprecision(12)
				<< ((double) track.general.toaFirst) / (double) 1.0e8 << "\t";
		cout << std::setprecision(12)
				<< ((double) track.general.toaLast) / (double) 1.0e8 << "\t";

		cout << "EJ" << (int) i << "\t";
		cout << (int) track.angle.curAngle << "\t";
		cout << (int) track.power.pulseCountSinceLastReport << "\t";
		cout << (int) track.pri.curPriType << "\t";
		cout << (int) track.pri.priCircular.size() << "\t";
		if (track.pri.priCircular.size() > 0)
		{
			cout << track.pri.priCircular.front().priMean << "\t";
		}
		else
		{
			cout << 0 << "\t";
		}
		if (track.pw.pwCircular.size() > 0)
			cout << (int) track.pw.pwCircular.front().pwAvg << "\t";
		else
			cout << 0 << "\t";

		if (track.rf.rfCircular.size() > 0)
			cout << (int) track.rf.rfCircular.front().rfAvg << "\t";
		else
			cout << 0 << "\t";
		cout << (int) max(track.power.pxMax, track.power.pyMax) << "\t";
		cout << (int) track.general.dspPdwBuffer.back().rfAmplitude << "\t";

		cout << (int) track.general.flagSelfTest << "\t";
		cout << (int) track.power.quadrantCodeLast << "\t";
		cout << "\n";
	}

	if (spurCount > 0)
	{
		cout
				<< "================================------------------------------================================\n";
	}
	trackDeleteInactiveTracks(tracks);
}
