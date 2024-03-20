/*
 * ethernettrackconversiondelegate.cpp
 *
 *  Created on: Jun 28, 2021
 *      Author: irfan
 */

#include "ethernettrackconversiondelegate.h"

void ethernetTracksConversion(vector<TrackDefaultClass> &tracks,
		vector<TrackDefaultClass> &tracksCw, vector<TrackDefaultClass> &tracksK,
		vector<TrackDefaultClass> &tracksCd, Ethernetlinktp &ethernetTrackObj)
{
	tracks.resize(0);
	tracksCw.resize(0);
	for (uint16_t i = 0; i < ethernetTrackObj.threatMsgs->trackCount; i++)
	{
		if (ethernetTrackObj.threatMsgs->threatParam[i].priType == 0)
		{
			ethernetCwTracksConversion(tracksCw, ethernetTrackObj, i);
		}

		else
		{
			if (ethernetTrackObj.threatMsgs->threatParam[i].rfSpot < 2000)
			{
				ethernetNormalTracksConversion(tracksCd, ethernetTrackObj, i);
			}
			else if (ethernetTrackObj.threatMsgs->threatParam[i].rfSpot > 18000)
			{
				ethernetNormalTracksConversion(tracksK, ethernetTrackObj, i);
			}
			else
			{
				ethernetNormalTracksConversion(tracks, ethernetTrackObj, i);
			}

		}

	}
}

void ethernetNormalTracksConversion(vector<TrackDefaultClass> &tracks,
		Ethernetlinktp &ethernetTrackObj, int trackNum)
{
	TrackDefaultClass track;
	track.power.AllocateHistograms();
	track.pri.allocateSequenceSearchObject();
	track.rf.AllocateRfTrackingObject();
	track.pw.AllocatePwTrackingObject();

	PdwDefaultClass tempPdw;

	threatReportParameters &currThreat =
			ethernetTrackObj.threatMsgs->threatParam[trackNum];

	track.angle.curAngle = currThreat.aoa;
	track.general.toaFirst = currThreat.toaFirst;
	track.general.toaLast = currThreat.toaLast;
	track.general.dspTrackingEnabled = false;

	tempPdw.timeOfArrivalStartRaw = currThreat.toaLast;
	tempPdw.powerARaw = currThreat.PowerAntennaA;
	tempPdw.powerBRaw = currThreat.PowerAntennaB;
	tempPdw.powerCRaw = currThreat.PowerAntennaC;
	tempPdw.powerDRaw = currThreat.PowerAntennaD;
	tempPdw.pulseWidthRaw = currThreat.pwSpot;
	tempPdw.frequencyRaw = currThreat.rfSpot;
	tempPdw.bandRaw = 0;
	tempPdw.frequencyValidFlagRaw = 1;
	scaleSinglePdw(tempPdw, 1, 1, 1);
	for (uint16_t count = 0; count < 10; count++)
	{
		tempPdw.clusterProcessFlag = true; //required for power analysis
		trackAssociateNewPdwWithTrack(track, tempPdw,
				Model::countThreadMainIterLow);
	}

	track.pri.curPriType = PriTypeEnum::pri_fixed;
	LimitPriClass priSpot;
	priSpot.priType = PriTypeEnum::pri_fixed;
	priSpot.priHigh = ((currThreat.priSpot) * 100);
	priSpot.priLow = ((currThreat.priSpot) * 100);
	priSpot.priMean = ((currThreat.priSpot) * 100);

	track.pw.curPwType = PwTypeEnum::pw_fixed;
	LimitPwClass pwSpot;
	pwSpot.pwType = PwTypeEnum::pw_fixed;
	pwSpot.pwHigh = ((currThreat.pwSpot / 10));
	pwSpot.pwLow = ((currThreat.pwSpot / 10));
	pwSpot.pwAvg = ((currThreat.pwSpot / 10));

	track.rf.curRfType = RfTypeEnum::rf_fixed;
	LimitRfClass rfSpot;
	rfSpot.rfType = RfTypeEnum::rf_fixed;
	rfSpot.rfHigh = ((currThreat.rfSpot));
	rfSpot.rfLow = ((currThreat.rfSpot));
	rfSpot.rfAvg = ((currThreat.rfSpot));
	if (currThreat.rfSpot < 2001)
	{
		track.rf.bandCode = BandCodeEnum::band_cd;
	}
	else if (currThreat.rfSpot < 6001)
	{
		track.rf.bandCode = BandCodeEnum::band_e;
	}
	else if (currThreat.rfSpot < 10001)
	{
		track.rf.bandCode = BandCodeEnum::band_g;
	}
	else if (currThreat.rfSpot < 14001)
	{
		track.rf.bandCode = BandCodeEnum::band_i;
	}
	else if (currThreat.rfSpot < 18001)
	{
		track.rf.bandCode = BandCodeEnum::band_j;
	}
	else if (currThreat.rfSpot < 40001)
	{
		track.rf.bandCode = BandCodeEnum::band_k;
	}

	track.rf.lastAnalysisSpotCount = 1;
	track.pri.lastPriAnalysisPulseCount = 16;
	track.pri.lastPriAnalysisSpotCount = 1;
	track.pw.lastAnalysisSpotCount = 1;

	track.pri.priCircular.push_back(priSpot);
	track.pw.pwCircular.push_back(pwSpot);
	track.rf.rfCircular.push_back(rfSpot);

	tracks.push_back(track);
}

void ethernetCwTracksConversion(vector<TrackDefaultClass> &tracksCw,
		Ethernetlinktp &ethernetTrackObj, int trackNum)
{
	TrackDefaultClass track;

	threatReportParameters &currThreat =
			ethernetTrackObj.threatMsgs->threatParam[trackNum];

	track.angle.curAngle = currThreat.aoa;
	track.angle.bearingLast = currThreat.aoa;
	track.history.activeFlag = 1;

	track.power.avgPA = currThreat.PowerAntennaA;
	track.power.avgPB = currThreat.PowerAntennaB;
	track.power.avgPC = currThreat.PowerAntennaC;
	track.power.avgPD = currThreat.PowerAntennaD;

	if (track.power.avgPA > track.power.avgPC)
	{
		track.power.pxMax = track.power.avgPA;
		track.power.pyMax = track.power.avgPA;
	}
	else
	{
		track.power.pxMax = track.power.avgPC;
		track.power.pyMax = track.power.avgPC;
	}
	if (track.power.avgPB > track.power.avgPD)
	{
		track.power.pxMax = track.power.avgPB;
		track.power.pyMax = track.power.avgPB;
	}
	else
	{
		track.power.pxMax = track.power.avgPD;
		track.power.pyMax = track.power.avgPD;
	}

	track.rf.curRfType = RfTypeEnum::rf_fixed;
	if ((currThreat.rfSpot >= 500) && (currThreat.rfSpot < 2000))
	{
		track.rf.bandCode = BandCodeEnum::band_cd;
	}
	else if ((currThreat.rfSpot >= 2000) && (currThreat.rfSpot < 6000))
	{
		track.rf.bandCode = BandCodeEnum::band_e;
	}
	else if ((currThreat.rfSpot >= 6000) && (currThreat.rfSpot < 10000))
	{
		track.rf.bandCode = BandCodeEnum::band_g;
	}
	else if ((currThreat.rfSpot >= 10000) && (currThreat.rfSpot < 14000))
	{
		track.rf.bandCode = BandCodeEnum::band_i;
	}
	else if ((currThreat.rfSpot >= 14000) && (currThreat.rfSpot <= 18000))
	{
		track.rf.bandCode = BandCodeEnum::band_j;
	}

	LimitRfClass rfSpot;
	rfSpot.rfType = RfTypeEnum::rf_fixed;
	rfSpot.rfHigh = ((currThreat.rfSpot));
	rfSpot.rfLow = ((currThreat.rfSpot));
	rfSpot.rfAvg = ((currThreat.rfSpot));

	track.rf.rfCircular.push_back(rfSpot);

	tracksCw.push_back(track);
}

