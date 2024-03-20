/*
 * BasrTrackJsonConversionDelegate.cpp
 *
 *  Created on: Jan 23, 2023
 *      Author: gmd
 */

#include "BasrTrackJsonConversionDelegate.h"
#include "../utility/VectorAndStringUtils.h"

void jsonReportBasrLowTracks()
{
	DataAnalysisClass &lowObj = Model::analysisLow;
	vector<TrackDefaultClass>& cdTracks = lowObj.bandCD.tracks;
	vector<TrackDefaultClass>& ejTracks = lowObj.bandEJ.tracks;
	vector<TrackDefaultClass>& kTracks = lowObj.bandK.tracks;

	string message =
			"{\"MessageName\": \"BasrTpInterfaceV100\",  \"MessageType\": \"LowTracks\"";
	message.reserve(20000);

	message += (",\"SystemItr\":" + to_string(Model::countThreadMainIterLow));
	message += (",\"TracksCD\":" + to_string(cdTracks.size()));
	message += (",\"TracksEJ\":" + to_string(ejTracks.size()));
	message += (",\"TracksK\":" + to_string(kTracks.size()));

	message += ",\"Tracks\":[";

	uint32_t eBandID = 6; //output format 0:E,1:G,2:I,3:J,4:CD,5:K,6:EJ
	uint32_t cdBandID = 4; //output format 0:E,1:G,2:I,3:J,4:CD,5:K,6:EJ
	uint32_t kBandID = 5; //output format 0:E,1:G,2:I,3:J,4:CD,5:K,6:EJ

	addBasrLowTracksToString(message, cdTracks, cdBandID);

	if (cdTracks.size() > 0)
	{
		if (ejTracks.size() > 0 || kTracks.size() > 0)
		{
			message += ",";
		}
	}
	addBasrLowTracksToString(message, ejTracks, eBandID);

	if (ejTracks.size() > 0)
	{
		if (kTracks.size() > 0)
		{
			message += ",";
		}
	}
	addBasrLowTracksToString(message, kTracks, kBandID);
	message += "]"; // end of tracks array

	message += "}"; //end of json marker

	LowNetworkInterface &networkInterface = Model::lowNetworkInterface;

	networkInterface.submitMessageForSending(message,
			LowNetworkInterface::BASR_TP_INTERFACE_MESSAGE_TYPE);
}

void jsonReportBasrHighTracks()
{

	DataAnalysisClass &highObj = Model::analysisHigh;
	DataAnalysisClass &highCwObj = Model::analysisCwp;
	vector<TrackDefaultClass>& cdTracks = highObj.bandCD.tracks;
	vector<TrackDefaultClass>& ejTracks = highObj.bandEJ.tracks;
	vector<TrackDefaultClass>& kTracks = highObj.bandK.tracks;
	vector<TrackDefaultClass>& cwTracks = highCwObj.bandAll.tracks;

	string message =
			"{\"MessageName\": \"BasrCCPInterfaceV100\",  \"MessageType\": \"HighTracks\"";
	message.reserve(20000);

	message += (",\"SystemItr\":" + to_string(Model::countThreadMainIterHigh));
	message += (",\"TracksCD\":" + to_string(cdTracks.size()));
	message += (",\"TracksEJ\":" + to_string(ejTracks.size()));
	message += (",\"TracksK\":" + to_string(kTracks.size()));
	message += (",\"TracksCW\":" + to_string(cwTracks.size()));

	message += ",\"Tracks\":[";

	uint32_t eBandID = 6; //output format 0:E,1:G,2:I,3:J,4:CD,5:K,6:EJ,7:CW
	uint32_t cdBandID = 4; //output format 0:E,1:G,2:I,3:J,4:CD,5:K,6:EJ,7:CW
	uint32_t kBandID = 5; //output format 0:E,1:G,2:I,3:J,4:CD,5:K,6:EJ,7:CW
	uint32_t cwBandID = 7; //output format 0:E,1:G,2:I,3:J,4:CD,5:K,6:EJ,7:CW

	addBasrHighTracksToString(message, cdTracks, cdBandID);

	if (cdTracks.size() > 0)
	{
		if (ejTracks.size() > 0 || kTracks.size() > 0 || cwTracks.size() > 0)
		{
			message += ",";
		}
	}
	addBasrHighTracksToString(message, ejTracks, eBandID);

	if (ejTracks.size() > 0)
	{
		if (kTracks.size() > 0 || cwTracks.size() > 0)
		{
			message += ",";
		}
	}
	addBasrHighTracksToString(message, kTracks, kBandID);

	if (kTracks.size() > 0)
	{
		if (cwTracks.size() > 0)
		{
			message += ",";
		}
	}
	addBasrHighTracksToString(message, cwTracks, cwBandID);

	message += "]"; // end of tracks array

	message += "}"; //end of json marker

	HighNetworkInterface &networkInterface = Model::highNetworkInterface;

	networkInterface.submitMessageForSending(message,
			HighNetworkInterface::BASR_CCP_INTERFACE_MESSAGE_TYPE);
}

void addBasrLowTracksToString(string &t, vector<TrackDefaultClass> &tracks,
		uint32_t band)
{
	uint32_t lastTrackId = tracks.size() - 1;
	for (uint32_t i = 0; i < tracks.size(); i++)
	{
		TrackDefaultClass track = tracks[i];
//		if(track.pri.curPriType != PriTypeEnum::pri_uncomputed)
//		{
//
//		}

		t += "{\"Band\":" + to_string(band);
		addBasrLowTrackToString(t, track);
		bool isLastTrack = (i == lastTrackId);
		if (isLastTrack == false)
		{
			t += "},";
		}
		else
		{
			t += "}";
		}
	}
}

string getPriTypeString(TrackPriClass &pri)
{
	string res = "0";
	switch (pri.priTrackingObj[0].GetCurrPriType())
	{
	case SequenceTypesEnum::seq_dwell:
		res[0] = '2';
		break;
	case SequenceTypesEnum::seq_stagger:
		res[0] = '3';
		break;
	case SequenceTypesEnum::seq_difftoa:
		res[0] = '7';
		break;
	case SequenceTypesEnum::seq_slidePos:
		res[0] = '4';
		break;
	case SequenceTypesEnum::seq_slideNeg:
		res[0] = '4';
		break;
	case SequenceTypesEnum::seq_slideTri:
		res[0] = '4';
		break;
	case SequenceTypesEnum::seq_slideSine:
		res[0] = '4';
		break;
	case SequenceTypesEnum::seq_jitter:
		res[0] = '5';
		break;
	case SequenceTypesEnum::seq_complex:
		res[0] = '6';
		break;
	default:
		break;
	}
	return res;
}
string getRfVectorString(TrackRfClass &rf)
{
	string res = "";
	circular_buffer<LimitRfClass> &rfSpots = rf.rfCircular;
	for (uint32_t i = 0; i < rfSpots.size(); i++)
	{
		res += to_string(rfSpots[i].rfAvg);
		res += ",";
	}
	if (res.length() > 0 && res[res.length() - 1] == ',')
	{
		res[res.length() - 1] = ' ';
	}
	return res;
}
string getRfVectorString2(TrackRfClass &rf)
{
	string res = "";
	circular_buffer<LimitRfClass> &rfSpots = rf.accumulateRf;
	for (uint32_t i = 0; i < rfSpots.size(); i++)
	{
		res += to_string(rfSpots[i].rfAvg);
		res += ",";
	}
	if (res.length() > 0 && res[res.length() - 1] == ',')
	{
		res[res.length() - 1] = ' ';
	}
	return res;
}
string getPwVectorString(TrackPwClass &pw)
{
	string res = "";
	vector<uint32_t> &pwSpots = pw.pwTrackingObj[0].pwSpots;
	for (uint32_t i = 0; i < pwSpots.size(); i++)
	{
		res += to_string(pwSpots[i]);
		res += ",";
	}
	if (res.length() > 0 && res[res.length() - 1] == ',')
	{
		res[res.length() - 1] = ' ';
	}
	return res;
}
string getPwVectorString2(TrackPwClass &pw)
{
	string res = "";
	circular_buffer<LimitPwClass> pwSpots = pw.accumulatePw;
	res.reserve(pwSpots.size() * 10);
	for (uint32_t i = 0; i < pwSpots.size(); i++)
	{
		res += to_string(pwSpots[i].pwLow);
		res += ",";
		res += to_string(pwSpots[i].pwHigh);
		res += ",";
	}
	if (res.length() > 0 && res[res.length() - 1] == ',')
	{
		res[res.length() - 1] = ' ';
	}
	return res;
}
string getPriVectorString(TrackPriClass &pri)
{
	string res = "";
	switch (pri.priTrackingObj[0].GetCurrPriType())
	{
	case SequenceTypesEnum::seq_stagger:
	{
		vector<int32_t> &staggerSpots = pri.priTrackingObj[0].GetStaggerSpots();

		res.reserve(staggerSpots.size() * 5);
		for (uint16_t i = 0; i < staggerSpots.size(); i++)
		{
			res += to_string(staggerSpots[i]);
			res += ",";
		}
		break;
	}
	case SequenceTypesEnum::seq_dwell:
	{
		vector<int32_t> &dwellSpots = pri.priTrackingObj[0].GetDwellSpots();

		res.reserve(dwellSpots.size() * 5);
		for (uint16_t i = 0; i < dwellSpots.size(); i++)
		{
			res += to_string(dwellSpots[i]);
			res += ",";
		}
		break;
	}
	case SequenceTypesEnum::seq_jitter:
	case SequenceTypesEnum::seq_complex:
	case SequenceTypesEnum::seq_difftoa:
	{
		vector<int32_t> &priSpots = pri.priTrackingObj[0].GetPriSpots();
		res.reserve(priSpots.size() * 5);
		for (uint16_t i = 0; i < priSpots.size(); i++)
		{
			res += to_string(priSpots[i]);
			res += ",";
		}
		break;
	}
	case SequenceTypesEnum::seq_slidePos:
	{

		res += to_string(pri.priTrackingObj[0].GetPosSlideStartPri());
		res += ",";
		res += to_string(pri.priTrackingObj[0].GetPosSlideRate());
		res += ",";
		res += to_string(pri.priTrackingObj[0].GetPosSlideEndPri());
		res += ",";

		break;
	}
	case SequenceTypesEnum::seq_slideNeg:
	{
		res += to_string(pri.priTrackingObj[0].GetNegSlideStartPri());
		res += ",";
		res += to_string(pri.priTrackingObj[0].GetNegSlideRate());
		res += ",";
		res += to_string(pri.priTrackingObj[0].GetNegSlideEndPri());
		res += ",";

		break;
	}
	case SequenceTypesEnum::seq_slideTri:
	{
		res += to_string(pri.priTrackingObj[0].GetTriSlideStartPri());
		res += ",";
		res += to_string(pri.priTrackingObj[0].GetTriSlideRate());
		res += ",";
		res += to_string(pri.priTrackingObj[0].GetTriSlideEndPri());
		res += ",";
		break;
	}

	case SequenceTypesEnum::seq_slideSine:
	{

		res += to_string(pri.priTrackingObj[0].GetSineSlideStartPri());
		res += ",";
		res += to_string(pri.priTrackingObj[0].GetSineSlideStartPri());
		res += ",";
		res += to_string(pri.priTrackingObj[0].GetSineSlideEndPri());
		res += ",";
		break;
	}
	default:
		res += ",";
		break;
	}
	if (res.length() > 0 && res[res.length() - 1] == ',')
	{
		res[res.length() - 1] = ' ';
	}
	return res;

}

bool getPriVectorStringType2(TrackPriClass &pri, string& res, string& priType)
{
	priType = "0";
	PriTypeEnum trackPriType = pri.priTypeCurrentDominant;
	vector<uint32_t> priValues;
	vector<uint32_t> priValues2;

	switch (trackPriType)
	{
	case PriTypeEnum::pri_fixed:
		priType[0] = '1';

		break;
	case PriTypeEnum::pri_dwellswitch:
		priType[0] = '2';
		break;
	case PriTypeEnum::pri_stagger:
		priType[0] = '3';
		break;
	case PriTypeEnum::pri_difftoa:
		priType[0] = '7';
		break;
	case PriTypeEnum::pri_sliding:
		priType[0] = '4';
		break;
	case PriTypeEnum::pri_jitter:
		priType[0] = '5';
		break;
	case PriTypeEnum::pri_complex:
		priType[0] = '6';
		break;
	default:
		break;
	}
	priValues.reserve(pri.accumulatePri.size());
	priValues2.reserve(pri.accumulatePri.size());
	for (uint32_t i = 0; i < pri.accumulatePri.size(); i++)
	{
		LimitPriClass &priInfoObj = pri.accumulatePri[i];
		if (trackPriType != priInfoObj.priType)
		{
			continue;
		}
		switch (trackPriType)
		{
		case PriTypeEnum::pri_fixed:
		case PriTypeEnum::pri_dwellswitch:
		case PriTypeEnum::pri_stagger:
		case PriTypeEnum::pri_difftoa:
			priValues.push_back(priInfoObj.priLow);
			break;

		case PriTypeEnum::pri_sliding:
			priValues.push_back(priInfoObj.priLow);
			priValues2.push_back(priInfoObj.priHigh);

			break;
		case PriTypeEnum::pri_jitter:
		case PriTypeEnum::pri_complex:
			priValues.push_back(priInfoObj.priLow);
			priValues2.push_back(priInfoObj.priHigh);

			break;
		default:
			break;
		}

	}
	res = "";
	switch (trackPriType)
	{
	case PriTypeEnum::pri_fixed:
	case PriTypeEnum::pri_dwellswitch:
	case PriTypeEnum::pri_stagger:
	case PriTypeEnum::pri_difftoa:
		std::sort(priValues.begin(), priValues.end());
		VectorAndStringUtils::removeDuplicates(priValues, priValues2,
				(uint32_t) 10);

		res.reserve(priValues2.size() * 8);
		res = VectorAndStringUtils::vectorToString(priValues2, ',');
		break;

	case PriTypeEnum::pri_sliding:

	case PriTypeEnum::pri_jitter:
	case PriTypeEnum::pri_complex:

		res.reserve((priValues.size() + priValues2.size()) * 8);
		res = VectorAndStringUtils::vectorToString(priValues, ',');
		res = VectorAndStringUtils::vectorToString(priValues2, ',');
		break;
	default:
		break;
	}

	if (res.length() > 0 && res[res.length() - 1] == ',')
	{
		res[res.length() - 1] = ' ';
	}
	return true;

}
void addBasrLowTrackToString(string& t, TrackDefaultClass& track)
{
	t += ",\"ToaFirst\":" + to_string(track.general.toaFirst);
	t += ",\"ToaLast\":" + to_string(track.general.toaLast);
	t += ",\"TrackId\":" + to_string(track.general.trackID);
	t += ",\"PulseCount\":" + to_string(track.power.pulseCountSinceLastReport);
	t += ",\"AOA\":" + to_string(track.angle.curAngle);
	t += ",\"PA\":" + to_string(track.power.avgPA);
	t += ",\"PB\":" + to_string(track.power.avgPB);
	t += ",\"PC\":" + to_string(track.power.avgPC);
	t += ",\"PD\":" + to_string(track.power.avgPD);
	t += ",\"RfAmp\":"
			+ to_string(track.general.dspPdwBuffer.back().rfAmplitude);
	if (track.general.flagSelfTest == 1)
	{
		t += ",\"STFlag\": true";
	}
	else
	{
		t += ",\"STFlag\": false";
	}

	t += ",\"MoP\":" + to_string((int) track.intrapulse.trackLpiType);

	t += ",\"PwType\":" + to_string((int) track.pw.curPwType);

	t += ",\"PriType\":" + getPriTypeString(track.pri);

	t += ",\"RfType\":" + to_string((int) track.rf.curRfType);

	t += ",\"PRI\":[" + getPriVectorString(track.pri);
	t += "],\"PW\":[" + getPwVectorString(track.pw);
	t += "],\"RF\":[" + getRfVectorString(track.rf);
	t += "]";
}

void addBasrHighTracksToString(string& t, vector<TrackDefaultClass>& tracks,
		uint32_t band)
{
	uint32_t lastTrackId = tracks.size() - 1;
	for (uint32_t i = 0; i < tracks.size(); i++)
	{
		TrackDefaultClass track = tracks[i];
//		if(track.pri.curPriType != PriTypeEnum::pri_uncomputed)
//		{
//
//		}

		t += "{\"Band\":" + to_string(band);
		if (band == 7)
		{
			addBasrHighCWTrackToString(t, track);
		}
		else
		{
			addBasrHighTrackToString(t, track);
		}
		bool isLastTrack = (i == lastTrackId);
		if (isLastTrack == false)
		{
			t += "},";
		}
		else
		{
			t += "}";
		}
	}
}

void addBasrHighTrackToString(string& t, TrackDefaultClass& track)
{
	t += ",\"ToaFirst\":" + to_string(track.general.toaFirst);
	t += ",\"ToaLast\":" + to_string(track.general.toaLast);
	t += ",\"TrackId\":" + to_string(track.general.trackID);
	t += ",\"AOA\":" + to_string(track.angle.bearingLast);
	t += ",\"ArpType\":" + to_string((uint32_t) track.power.scanTypeLast);
	t += ",\"PulseCount\":" + to_string(track.history.totalPulseCount);

	t += ",\"lPA\":" + to_string(track.last[0].power.avgPA);
	t += ",\"lPB\":" + to_string(track.last[0].power.avgPB);
	t += ",\"lPC\":" + to_string(track.last[0].power.avgPC);
	t += ",\"lPD\":" + to_string(track.last[0].power.avgPD);
	t += ",\"lAOA\":" + to_string(track.last[0].angle.curAngle);
	t += ",\"lPulseCount\":"
			+ to_string(track.last[0].power.pulseCountSinceLastReport);
	t += ",\"lMoP\":" + to_string((int) track.last[0].intrapulse.trackLpiType);

	string priType, priVec;
	getPriVectorStringType2(track.pri, priVec, priType);
	t += ",\"PriType\":" + priType;

	t += ",\"PwType\":" + to_string((int) track.pw.curPwType);

	t += ",\"RfType\":" + to_string((int) track.rf.curRfType);

	t += ",\"PRI\":[" + priVec;
	t += "],\"PW\":[" + getPwVectorString2(track.pw);
	t += "],\"RF\":[" + getRfVectorString2(track.rf);
	t += "]";
}
void addBasrHighCWTrackToString(string& t, TrackDefaultClass& track)
{
	t += ",\"TrackId\":" + to_string(track.general.trackID);
	t += ",\"AOA\":" + to_string(track.angle.bearingDisplay);

	t += ",\"PA\":" + to_string(track.power.avgPA);
	t += ",\"PB\":" + to_string(track.power.avgPB);
	t += ",\"PC\":" + to_string(track.power.avgPC);
	t += ",\"PD\":" + to_string(track.power.avgPD);
	t += ",\"GS\":" + to_string(track.power.cwGainStage);
	t += ",\"RF\":[" + getRfVectorString2(track.rf);
	t += "]";
}
