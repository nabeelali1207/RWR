/*
 * analysisrfdelegate.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#define ANALYSISRFDELEGATE_CPP_

#include "../delegate/analysisrfdelegate.h"

void rfUpdateActiveSpotInfo(vector<TrackDefaultClass> &tracks)
{/* This function is only used to get rf analysis results from tracking class
 *   as RF analysis is automatically called if required, during tracking
 *   when pulses associate successfully
 */
	// this function copies the updated rf spot info from rfTrackingObject to rfObject
	for (uint16_t i = 0; i < tracks.size(); i++)
	{

		if (tracks[i].rf.rfTrackingObj[0].newResultsAvailableForCopyFlag == 1)
		{
			rfCopyResultsFromTrackingClass(tracks[i]);

		}
		if (tracks[i].rf.rfTrackingObj.size())
		{
			tracks[i].rf.rfValue = tracks[i].rf.rfTrackingObj[0].rfMedian;
			if (tracks[i].rf.bandCode != BandCodeEnum::band_k)
			{
				tracks[i].rf.bandCode = getBandFromRf(tracks[i].rf.rfValue);
			}
		}

	}
}

void rfCopyResultsFromTrackingClass(TrackDefaultClass &track)
{
	LimitRfClass tempRfValue;
	AnalysisTrackRfLowClass &trackingObj = track.rf.rfTrackingObj[0];
	TrackRfClass &rfObj = track.rf;
	switch (trackingObj.currRfType)
	{
	case SequenceTypesEnum::seq_dwell:
		track.rf.curRfType = RfTypeEnum::rf_fixed;
		break;
	case SequenceTypesEnum::seq_difftoa:
		track.rf.curRfType = RfTypeEnum::rf_jump;
		break;
	case SequenceTypesEnum::seq_jitter:
		track.rf.curRfType = RfTypeEnum::rf_agile;
		break;
	case SequenceTypesEnum::seq_unknown:
		track.rf.curRfType = RfTypeEnum::rf_uncomputed;
		break;
	default:
		track.rf.curRfType = RfTypeEnum::rf_uncomputed;
		break;
	}

	if (trackingObj.lastAnalysisWasSuccessfulFlag == 1)
	{
		rfObj.lastAnalysisSpotCount = trackingObj.lastAnalysisPulseCount;

		switch (trackingObj.currRfType)
		{
		case SequenceTypesEnum::seq_dwell:
		{
			vector<uint32_t> &dwellSpots =
					rfObj.rfTrackingObj[0].GetDwellSpots();

			if (dwellSpots.size() == 1)
				rfObj.curRfType = RfTypeEnum::rf_fixed;
			else
				rfObj.curRfType = RfTypeEnum::rf_switch;

			tempRfValue.rfType = rfObj.curRfType;
			rfObj.lastAnalysisSpotCount = dwellSpots.size();
			for (uint16_t i = 0; i < dwellSpots.size(); i++)
			{
				tempRfValue.rfHigh = dwellSpots[i];
				tempRfValue.rfAvg = dwellSpots[i];
				tempRfValue.rfLow = dwellSpots[i];
				tempRfValue.pulseCount = 0;
				rfObj.rfCircular.push_front(tempRfValue);

			}

			break;
		}
		case SequenceTypesEnum::seq_difftoa:
		{
			rfObj.curRfType = RfTypeEnum::rf_jump;
			vector<uint32_t> &rfSpots = rfObj.rfTrackingObj[0].GetRfSpots();
			vector<uint32_t> &rfRangesHigh =
					rfObj.rfTrackingObj[0].GetUpperRange();
			vector<uint32_t> &rfRangesLow =
					rfObj.rfTrackingObj[0].GetLowerRange();
			tempRfValue.rfType = RfTypeEnum::rf_jump;
			rfObj.lastAnalysisSpotCount = rfSpots.size();
			for (uint16_t i = 0; i < rfSpots.size(); i++)
			{
				tempRfValue.rfHigh = rfRangesHigh[i];
				tempRfValue.rfAvg = rfSpots[i];
				tempRfValue.rfLow = rfRangesLow[i];
				tempRfValue.pulseCount = 0;
				rfObj.rfCircular.push_front(tempRfValue);
			}
			break;
		}
		case SequenceTypesEnum::seq_jitter:
		{
			rfObj.curRfType = RfTypeEnum::rf_agile;
			vector<uint32_t> &rfSpots = rfObj.rfTrackingObj[0].GetRfSpots();
			vector<uint32_t> &rfRangesHigh =
					rfObj.rfTrackingObj[0].GetUpperRange();
			vector<uint32_t> &rfRangesLow =
					rfObj.rfTrackingObj[0].GetLowerRange();
			tempRfValue.rfType = RfTypeEnum::rf_agile;
			rfObj.lastAnalysisSpotCount = rfSpots.size();
			for (uint16_t i = 0; i < rfSpots.size(); i++)
			{
				tempRfValue.rfHigh = rfRangesHigh[i];
				tempRfValue.rfAvg = rfSpots[i];
				tempRfValue.rfLow = rfRangesLow[i];
				tempRfValue.pulseCount = 0;
				rfObj.rfCircular.push_front(tempRfValue);
			}
			break;
		}
		case SequenceTypesEnum::seq_complex:
		{
			rfObj.curRfType = RfTypeEnum::rf_complex;
			vector<uint32_t> &rfSpots = rfObj.rfTrackingObj[0].GetRfSpots();
			vector<uint32_t> &rfRangesHigh =
					rfObj.rfTrackingObj[0].GetUpperRange();
			vector<uint32_t> &rfRangesLow =
					rfObj.rfTrackingObj[0].GetLowerRange();
			tempRfValue.rfType = RfTypeEnum::rf_complex;
			rfObj.lastAnalysisSpotCount = rfSpots.size();
			for (uint16_t i = 0; i < rfSpots.size(); i++)
			{
				tempRfValue.rfHigh = rfRangesHigh[i];
				tempRfValue.rfAvg = rfSpots[i];
				tempRfValue.rfLow = rfRangesLow[i];
				tempRfValue.pulseCount = 0;
				rfObj.rfCircular.push_front(tempRfValue);
			}
			break;
		}
		default:
			rfObj.curRfType = RfTypeEnum::rf_uncomputed;
			rfObj.lastAnalysisSpotCount = 0;
			break;

		}

		if (track.rf.rfFirst.size() == 0)
		{
			for (uint16_t i = 0; i < rfObj.rfCircular.size(); i++)
			{
				LimitRfClass &temp = rfObj.rfCircular[i];
				track.rf.rfFirst.push_back(temp);
			}

		}
	}
	trackingObj.lastAnalysisWasSuccessfulFlag = 0;
	trackingObj.newResultsAvailableForCopyFlag = 0;
}
BandCodeEnum getBandFromRf(uint32_t rfValue)
{
	uint32_t cdBandLow = 500, cdBandHigh = 2000;
	uint32_t eBandLow = 2000, eBandHigh = 6000;
	uint32_t gBandLow = 6001, gBandHigh = 10000;
	uint32_t iBandLow = 10001, iBandHigh = 14000;
	uint32_t jBandLow = 14001, jBandHigh = 18000;

	BandCodeEnum band = BandCodeEnum::band_uncomputed;

	if ((rfValue >= cdBandLow) && (rfValue <= cdBandHigh))
	{
		band = BandCodeEnum::band_cd;
	}
	if ((rfValue > eBandLow) && (rfValue <= eBandHigh))
	{
		band = BandCodeEnum::band_e;
	}
	if ((rfValue > gBandLow) && (rfValue <= gBandHigh))
	{
		band = BandCodeEnum::band_g;
	}
	if ((rfValue > iBandLow) && (rfValue <= iBandHigh))
	{
		band = BandCodeEnum::band_i;
	}
	if ((rfValue > jBandLow) && (rfValue <= jBandHigh))
	{
		band = BandCodeEnum::band_j;
	}
	return band;
}
