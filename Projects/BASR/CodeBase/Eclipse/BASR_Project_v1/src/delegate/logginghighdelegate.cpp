/*
 * logginghighdelegate.cpp
 *
 *  Created on: Sep 25, 2019
 *      Author: hassan
 */

#define LOGGINGHIGHDELEGATE_CPP_

#include "logginghighdelegate.h"

//tasks: run this code and verify for high tracks
//complete the tp/low report writing function and test it as well
//initialize encryption key
//make sure to write the flight record in ssd and not emmc
//verify both low and high tracks in desktop java application
//remove all malloc/calloc from code and replace with fixed size structures
//put additional safeties in place before opening file for writing
//consult ali nagra for those additional safeties (i.e. ssd is mounted etc)
//comment this code properly
//test with large number of tracks

//this function writes tp/low tracks to flight record, complete it!
void logWriteLowTracks(vector<TrackDefaultClass> &tracksE,
		vector<TrackDefaultClass> &tracksG, vector<TrackDefaultClass> &tracksI,
		vector<TrackDefaultClass> &tracksJ, ThreatLibrary *inputLibrary,
		LibraryMetaClass *libraryMeta, string flighRecordPath)
{

	/*ThreatLibrary *libraryFr;
	libraryFr = inputLibrary;*/
	//initialize key here temporarily!!
	uint8_t i = 0;
	uint8_t numberOfTracks = 0;

	for (i = 0; i < tracksE.size(); i++)
	{
		if (numberOfTracks < 256)
		{
			singleLowEmitterCopy(&(Model::emitterLogLow.Flights[numberOfTracks]),
					tracksE[i]);
			numberOfTracks = numberOfTracks + 1;
		}
	}
	for (i = 0; i < tracksG.size(); i++)
	{
		if (numberOfTracks < 256)
		{
			singleLowEmitterCopy(&(Model::emitterLogLow.Flights[numberOfTracks]),
					tracksG[i]);
			numberOfTracks = numberOfTracks + 1;
		}
	}
	for (i = 0; i < tracksI.size(); i++)
	{
		if (numberOfTracks < 256)
		{
			singleLowEmitterCopy(&(Model::emitterLogLow.Flights[numberOfTracks]),
					tracksI[i]);
			numberOfTracks = numberOfTracks + 1;
		}
	}
	for (i = 0; i < tracksJ.size(); i++)
	{
		if (numberOfTracks < 256)
		{
			singleLowEmitterCopy(&(Model::emitterLogLow.Flights[numberOfTracks]),
					tracksJ[i]);
			numberOfTracks = numberOfTracks + 1;
		}
	}
	Model::fileoutputflightrecord.writeThreatProcessorReportToFlightRecord(numberOfTracks, libraryMeta,
			flighRecordPath,Model::emitterLogLow,Model::memoryFailure);
/*	globalSessionIdLowTracks++;
	if (globalSessionIdLowTracks == 256)
	{
		globalSessionIdLowTracks = 0;
	}*/
	//copyThreatProcessorReportToFlightRecord(pEmitterRecordReportCurrent, pExtendedAefReportCurrent);
	//writeThreatProcessorReportToFlightRecord(pExtendedAefReportCurrent);
}

//main function to write high flight record for each band
void logWriteHighTracks(vector<TrackDefaultClass> &tracksE,
		vector<TrackDefaultClass> &tracksG, vector<TrackDefaultClass> &tracksI,
		vector<TrackDefaultClass> &tracksJ, ThreatLibrary *inputLibrary,
		LibraryMetaClass *libraryMeta, string flighRecordPath)
{

	uint8_t i = 0;
	uint8_t numberOfTracks = 0;
//	uint8_t ssdStatus = 1;
	/*ThreatLibrary *libraryFr;
	libraryFr = inputLibrary;*/
	//initialize key here temporarily!!
//
//	if (ConstHigh::simOnActualHwFlag == 1)
//	{
//		ssdStatus = (/*checkForSsdOnHardware()
//				&& */Model::fileoutputflightrecord.checkIfSSDIsMounted(ConstHigh::flightRecordPathHardware));
//	}

	if (Model::cfCard.foundCfFlag)
	{
		for (i = 0; i < tracksE.size(); i++)
		{
			if (numberOfTracks < 256)
			{
				singleFlightRecordCopy(&(Model::emitterLog.Flights[numberOfTracks]),
						tracksE[i]);
				numberOfTracks = numberOfTracks + 1;
			}
		}
		for (i = 0; i < tracksG.size(); i++)
		{
			if (numberOfTracks < 256)
			{
				singleFlightRecordCopy(&(Model::emitterLog.Flights[numberOfTracks]),
						tracksG[i]);
				numberOfTracks = numberOfTracks + 1;
			}
		}
		for (i = 0; i < tracksI.size(); i++)
		{
			if (numberOfTracks < 256)
			{
				singleFlightRecordCopy(&(Model::emitterLog.Flights[numberOfTracks]),
						tracksI[i]);
				numberOfTracks = numberOfTracks + 1;
			}
		}
		for (i = 0; i < tracksJ.size(); i++)
		{
			if (numberOfTracks < 256)
			{
				singleFlightRecordCopy(&(Model::emitterLog.Flights[numberOfTracks]),
						tracksJ[i]);
				numberOfTracks = numberOfTracks + 1;
			}
		}
		Model::fileoutputflightrecord.writeTrackingDataFlightRecordToFile(numberOfTracks, libraryMeta,
				flighRecordPath,Model::emitterLog,Model::memoryFailure);
/*		globalSessionId++;
		if (globalSessionId == 256)
		{
			globalSessionId = 0;
		}*/
	}
}

//enable this function in header file as well and fix it to work with tp reports
//void basicFlightRecordData(ExtendedAefReport* extendedAefReport)
//{
//    emitterLog.flightRecordHeader.Delimiter = 0xbeeffeeddeadbeef;
//    emitterLog.flightRecordHeader.structureCount = extendedAefReport->NumberofEmitters;
//}

//enable this function in header file as well and fix it to work with tp reports
//void copyThreatProcessorReportToFlightRecord(EmitterRecordReport* emitter, ExtendedAefReport* extEmitter)
//{
//    Word32 i=0;
//    basicFlightRecordData(extEmitter);
//    for(i=0;i<extEmitter->NumberofEmitters;i++)
//    {
//        singleFlightRecordCopy(&(emitterLog.Flights[i]), emitter+i, extEmitter->ExtEmitterData[i],0);
//    }
//}

void singleLowEmitterCopy(FlightRecord * fr, TrackDefaultClass &track)
{
	uint16_t i = 0;
	uint16_t j = 0;
	uint16_t k = 0;
	//uint16_t trackPriCount = 0;
	uint64_t millis = 0;
	struct tm tm;
	time_t T;

	if ((ConstHigh::simTimeSyntheticFlag == 1))
	{
		if (Model::startupSystemTimeIsSet == 0)
		{
			Model::startupSystemTime = time(NULL);

			Model::startupSystemTimeIsSet = 1;
		}

		tm = *localtime(&Model::startupSystemTime);
		millis = 200 * Model::countThreadMainIterHigh;
		T = mktime(&tm);
		T += (millis / 1000);
		tm = *localtime(&T);
	}
	else
	{
		T = time(NULL);
		tm = *localtime(&T);
	}

	fr->matchCount = track.libmatch.active.highestMatchListCount;
	fr->trackUpdateStatus = track.history.lowToaStartUpdateFlag;
	fr->priority = track.general.priorityDynamic;

	for (i = 0; i < 3; i++)
	{
		fr->MatchIndex[i] = 0;
		fr->emitterId[i] = 0;
		fr->batchId[i] = 0;
		fr->SymbolCode1[i] = 0;
		fr->SymbolCode2[i] = 0;
		fr->SymbolCode3[i] = 0;
		fr->SymbolCode4[i] = 0;
		fr->OuterCode[i] = 0;
		fr->modeId[i].modeID[0] = 0;
		fr->modeId[i].modeID[1] = 0;
		fr->MatchScore[i] = 0;
	}
	for (i = 0; i < track.libmatch.active.highestMatchListCount; i++)
	{

		fr->MatchIndex[i] = track.libmatch.active.highestMatchList[i].ID + 1;
		fr->emitterId[i] = track.libmatch.active.highestMatchList[i].ID + 1;
		//if(track.libmatch.current.highestMatchList[i].Total != 0) {
		//	fr->MatchIndex[i] = track.libmatch.current.highestMatchList[i].ID + 1;
		//	fr->emitterId[i] = track.libmatch.current.highestMatchList[i].ID + 1;
		//}
		fr->batchId[i] = track.libmatch.active.highestMatchList[i].emitterBatchId;
		fr->SymbolCode1[i] = track.libmatch.active.highestMatchList[i].symbolOne;
		fr->SymbolCode2[i] = track.libmatch.active.highestMatchList[i].symbolTwo;
		fr->SymbolCode3[i] = 0;
		fr->SymbolCode4[i] = 0;
		fr->OuterCode[i] = track.libmatch.active.highestMatchList[i].symbolOuter;
		fr->modeId[i].modeID[0] = 0;
		fr->modeId[i].modeID[1] = 0;
//		libraryFr->Emitters[track.libmatch.active.highestMatchList[i].ID].modeID[1]; // look up later, init to 0 for now
		fr->MatchScore[i] = (UWord32) track.libmatch.active.highestMatchList[i].Total;
	}
	if(track.libmatch.active.highestMatchListCount == 0)
	{
		fr->SymbolCode1[i] = track.libmatch.active.highestMatchList[i].symbolOne;
		fr->SymbolCode2[i] = track.libmatch.active.highestMatchList[i].symbolTwo;
		fr->SymbolCode3[i] = 0;
		fr->SymbolCode4[i] = 0;
		fr->OuterCode[i] = track.libmatch.active.highestMatchList[i].symbolOuter;
	}
	fr->time.hour = tm.tm_hour;
	fr->time.minute = tm.tm_min;
	fr->time.second = tm.tm_sec;
	fr->date.day = tm.tm_mday;
	fr->date.month = tm.tm_mon + 1;
	fr->date.year = tm.tm_year + 1900;
	//memcpy(&fr->EmitterData, emitter, sizeof(LegacyEmitterRecordReport));
	//fr->EmitterData.PxQuadrant1 = ptrTrack->maxPowerXLast;
	//fr->EmitterData.PxQuadrant2 = ptrTrack->Priority;
	//fr->EmitterData.PyQuadrant1 = ptrTrack->maxPowerYLast;
	//fr->EmitterData.PyQuadrant2 = ptrTrack->trackDistanceFiltered;//later to be done zero
	//fr->EmitterData.AngleOfArrival = ptrTrack->Bearing;
	//fr->EmitterData.ComplexPRI_QualityFlag = ptrTrack->emitterClosingInWithRespectToDistance;
	memset(&fr->EmitterData, 0, sizeof(LegacyEmitterRecordReport));

	fr->EmitterData.AngleOfArrival = track.angle.bearingLast;
	fr->EmitterData.Band = track.rf.getBandEnumeration();

	//trackPriCount = track.pri.accumulatePri.size();
	//if(trackPriCount > 0) {
	//	trackLastPriType = track.pri.accumulatePri[trackPriCount - 1].priType;
	//}

	j = 0;
	k = 0;
	//requires type checks, please fix
	for (i = 0; i < track.pri.accumulatePri.size(); i++)
	{
		if ((track.pri.accumulatePri[i].priType == PriTypeEnum::pri_dwellswitch)
				&& (track.pri.priTypeCurrentDominant
						== PriTypeEnum::pri_dwellswitch)
				&& (track.pri.accumulatePriFixedCount == 1))
		{
			fr->EmitterData.PRIType = 1;
			fr->EmitterData.DwellPRISpot[0] = track.pri.accumulatePri[i].priLow;
			fr->EmitterData.DwellPRI_QualityFlag[0] = 1;
		}
		else if ((track.pri.accumulatePri[i].priType
				== PriTypeEnum::pri_dwellswitch)
				&& (track.pri.priTypeCurrentDominant
						== PriTypeEnum::pri_dwellswitch))
		{
			fr->EmitterData.PRIType = 2;
			if (j < 32)
			{
				fr->EmitterData.DwellPRISpot[j] =
						track.pri.accumulatePri[i].priLow;
				fr->EmitterData.DwellPRI_QualityFlag[j] = 1;
				j++;
			}
		}
		else if ((track.pri.accumulatePri[i].priType == PriTypeEnum::pri_stagger)
				&& (track.pri.priTypeCurrentDominant == PriTypeEnum::pri_stagger))
		{
			fr->EmitterData.PRIType = 3;
			if (j < 64)
			{
				fr->EmitterData.StaggerPhase[j] =
						track.pri.accumulatePri[i].priLow;
				j++;
			}
			if (k < 6)
			{
				if (fr->EmitterData.StaggerFrame[k]
						!= track.pri.accumulatePri[i].frame)
				{
					fr->EmitterData.StaggerFrame[k] =
							track.pri.accumulatePri[i].frame;
					k++;
				}
			}
		}
		else if ((track.pri.accumulatePri[i].priType == PriTypeEnum::pri_jitter)
				&& (track.pri.priTypeCurrentDominant == PriTypeEnum::pri_jitter))
		{
			fr->EmitterData.PRIType = 5;
			fr->EmitterData.JitterPRIMin = track.pri.accumulatePri[i].priLow;
			fr->EmitterData.JitterPRIMax = track.pri.accumulatePri[i].priHigh;
		}
		else if ((track.pri.accumulatePri[i].priType == PriTypeEnum::pri_sliding)
				&& (track.pri.priTypeCurrentDominant == PriTypeEnum::pri_sliding))
		{
			fr->EmitterData.PRIType = 4;
			fr->EmitterData.SlidingPRIMin = track.pri.accumulatePri[i].priLow;
			fr->EmitterData.SlidingPRIMax = track.pri.accumulatePri[i].priHigh;
		}
		else if ((track.pri.accumulatePri[i].priType == PriTypeEnum::pri_complex)
				&& (track.pri.priTypeCurrentDominant == PriTypeEnum::pri_complex))
		{
			fr->EmitterData.PRIType = 6;
			fr->EmitterData.ComplexPRIMin = track.pri.accumulatePri[i].priLow;
			fr->EmitterData.ComplexPRIMax = track.pri.accumulatePri[i].priHigh;
		}
		else if ((track.pri.accumulatePri[i].priType == PriTypeEnum::pri_difftoa)
				&& (track.pri.priTypeCurrentDominant == PriTypeEnum::pri_difftoa))
		{
			fr->EmitterData.PRIType = 7;
			if (j < 32)
			{
				fr->EmitterData.DwellPRISpot[j] =
						track.pri.accumulatePri[i].priLow;
				fr->EmitterData.DwellPRI_QualityFlag[j] = 0;
				j++;
			}
		}
	}
	fr->EmitterData.SlideType = 0;
	fr->EmitterData.SweepType = 0;    //what is sweep type?
	fr->EmitterData.JitterPRI_QualityFlag = 0;
	fr->EmitterData.LPIType = 0;
	fr->EmitterData.New_Updated_Flag = track.history.lowToaStartUpdateFlag;

	if (track.pw.accumulatePw.size() > 0)
	{
		fr->EmitterData.PWType = 1;
	}
	else
	{
		fr->EmitterData.PWType = 0;
	}
	/*if(track.pw.curPwType==PwTypeEnum::pw_fixed)
	 fr->EmitterData.PWType = 1;
	 else if(track.pw.curPwType==PwTypeEnum::pw_agile)
	 fr->EmitterData.PWType = 2;
	 else
	 fr->EmitterData.PWType = 0;
	 */

	fr->EmitterData.PulseCountQuadrant1 = track.power.quadrantCodeMax;
	fr->EmitterData.PulseCountQuadrant2 = track.power.quadrantCodeMax;
	j = 0;
	for (i = 0; i < track.pw.accumulatePw.size(); i++)
	{
		if (j < 8)
		{
			fr->EmitterData.PulseWidthSpot[j] = track.pw.accumulatePw[i].pwLow;
			j++;
		}
	}
	for (i = 0; i < 8; i++)
	{
		fr->EmitterData.PW_QualityFlag[i] = 0;
	}
	fr->EmitterData.PxQuadrant1 = track.power.pxMax;
	fr->EmitterData.PxQuadrant2 = track.power.pxMax;
	fr->EmitterData.PyQuadrant1 = track.power.pyMax;
	fr->EmitterData.PyQuadrant2 = track.power.pyMax;
	fr->EmitterData.Quadrant1 = 0;
	fr->EmitterData.Quadrant2 = 0;

	//	if (track.rf.accumulateRf.size() > 0)
	//	{
	//		fr->EmitterData.RFType = 2;
	//	}
	//	else
	//	{
	//		fr->EmitterData.RFType = 0;
	//	}
	if (track.rf.curRfType == RfTypeEnum::rf_fixed)
		fr->EmitterData.RFType = 1;

	else if (track.rf.curRfType == RfTypeEnum::rf_switch)
		fr->EmitterData.RFType = 2;
	

	else if (track.rf.curRfType == RfTypeEnum::rf_agile)
		fr->EmitterData.RFType = 3;

	 else if(track.rf.curRfType==RfTypeEnum::rf_jump)
	 fr->EmitterData.RFType = 4;

	 else if(track.rf.curRfType==RfTypeEnum::rf_complex)
	 fr->EmitterData.RFType = 6;

	/* else if(track.rf.curRfType==RfTypeEnum::rf_slide)
	 fr->EmitterData.RFType = 7;

	 else if(track.rf.curRfType==RfTypeEnum::rf_sweep)
	 fr->EmitterData.RFType = 7;

	 else if(track.rf.curRfType==RfTypeEnum::rf_sweepsin)
	 fr->EmitterData.RFType = 7;

	 else if(track.rf.curRfType==RfTypeEnum::rf_sweeptriangle)
	 fr->EmitterData.RFType = 7;
	 else fr->EmitterData.RFType = 0;*/


	j = 0;
	for (i = 0; i < track.rf.rfCircular.size(); i++)
	{
		if (j < 8)
		{
			fr->EmitterData.RFSpots[j] = track.rf.rfCircular[i].rfLow;
			j++;
		}
	}
	for (i = 0; i < 16; i++)
	{
		fr->EmitterData.RF_QualityFlag[i] = 0;
	}

	fr->EmitterData.PulseCountQuadrant1=track.power.pulseCountSinceLastReport;

	if(track.power.scanTypeLast==EmitterArpEnum::scan_circular)
		fr->EmitterData.Scan_Track = 1;
	else if(track.power.scanTypeLast==EmitterArpEnum::scan_lock)
		fr->EmitterData.Scan_Track = 0;
	else
		fr->EmitterData.Scan_Track = 3;

	fr->EmitterData.ScanPeriod = 0;
	//fr->EmitterData.Scan_Track = track.general.trackingFlag;
//	fr->EmitterData.Scan_Track = 3;
	fr->EmitterData.SelfTest = 0;
	fr->EmitterData.TOAStart = track.general.toaFirst;
	fr->EmitterData.TOAEnd = track.general.toaLast;
	fr->EmitterData.EmitterID = Model::countThreadMainIterHigh;
	//fr->EmitterData.Range = track.range.rangeMeters;
	fr->EmitterData.Range = track.range.amplitudeStepDisplay;
}

void singleFlightRecordCopy(FlightRecord * fr, TrackDefaultClass &track)
{
	uint16_t i = 0;
	uint16_t j = 0;
	uint16_t k = 0;
	//uint16_t trackPriCount = 0;
	uint64_t millis = 0;
	struct tm tm;
	time_t T;

	if ((ConstHigh::simTimeSyntheticFlag == 1))
	{
		if (Model::startupSystemTimeIsSet == 0)
		{
			Model::startupSystemTime = time(NULL);

			Model::startupSystemTimeIsSet = 1;
		}

		tm = *localtime(&Model::startupSystemTime);
		millis = 200 * Model::countThreadMainIterHigh;
		T = mktime(&tm);
		T += (millis / 1000);
		tm = *localtime(&T);
	}
	else
	{
		T = time(NULL);
		tm = *localtime(&T);
	}

	fr->matchCount = track.libmatch.active.highestMatchListCount;
	fr->trackUpdateStatus = track.history.highTrackContinuationIterFlag;
	fr->priority = track.general.priorityDynamic;
	//fr->priority =track.angle.bearingLast;
	for (i = 0; i < 3; i++)
	{
		fr->MatchIndex[i] = 0;
		fr->emitterId[i] = 0;
		fr->batchId[i] = 0;
		fr->SymbolCode1[i] = 0;
		fr->SymbolCode2[i] = 0;
		fr->SymbolCode3[i] = 0;
		fr->SymbolCode4[i] = 0;
		fr->OuterCode[i] = 0;
		fr->modeId[i].modeID[0] = 0;
		fr->modeId[i].modeID[1] = 0;
		fr->MatchScore[i] = 0;
	}
	for (i = 0; i < track.libmatch.active.highestMatchListCount; i++)
	{

		fr->MatchIndex[i] = track.libmatch.active.highestMatchList[i].ID + 1;
		fr->emitterId[i] = track.libmatch.active.highestMatchList[i].ID + 1;
		//if(track.libmatch.current.highestMatchList[i].Total != 0) {
		//	fr->MatchIndex[i] = track.libmatch.current.highestMatchList[i].ID + 1;
		//	fr->emitterId[i] = track.libmatch.current.highestMatchList[i].ID + 1;
		//}
		fr->batchId[i] = track.libmatch.active.highestMatchList[i].emitterBatchId;
		fr->SymbolCode1[i] = track.libmatch.active.highestMatchList[i].symbolOne;
		fr->SymbolCode2[i] = track.libmatch.active.highestMatchList[i].symbolTwo;
		fr->SymbolCode3[i] = 0;
		fr->SymbolCode4[i] = 0;
		fr->OuterCode[i] = track.libmatch.active.highestMatchList[i].symbolOuter;
		fr->modeId[i].modeID[0] = 0;
		fr->modeId[i].modeID[1] = 0;
//		libraryFr->Emitters[track.libmatch.active.highestMatchList[i].ID].modeID[1]; // look up later, init to 0 for now
		fr->MatchScore[i] = (UWord32) track.libmatch.active.highestMatchList[i].Total;
	}
	if(track.libmatch.active.highestMatchListCount == 0)
	{
		fr->SymbolCode1[i] = track.libmatch.active.highestMatchList[i].symbolOne;
		fr->SymbolCode2[i] = track.libmatch.active.highestMatchList[i].symbolTwo;
		fr->SymbolCode3[i] = 0;
		fr->SymbolCode4[i] = 0;
		fr->OuterCode[i] = track.libmatch.active.highestMatchList[i].symbolOuter;
	}
	fr->time.hour = tm.tm_hour;
	fr->time.minute = tm.tm_min;
	fr->time.second = tm.tm_sec;
	fr->date.day = tm.tm_mday;
	fr->date.month = tm.tm_mon + 1;
	fr->date.year = tm.tm_year + 1900;

	//memcpy(&fr->EmitterData, emitter, sizeof(LegacyEmitterRecordReport));
	//fr->EmitterData.PxQuadrant1 = ptrTrack->maxPowerXLast;
	//fr->EmitterData.PxQuadrant2 = ptrTrack->Priority;
	//fr->EmitterData.PyQuadrant1 = ptrTrack->maxPowerYLast;
	//fr->EmitterData.PyQuadrant2 = ptrTrack->trackDistanceFiltered;//later to be done zero
	//fr->EmitterData.AngleOfArrival = ptrTrack->Bearing;
	//fr->EmitterData.ComplexPRI_QualityFlag = ptrTrack->emitterClosingInWithRespectToDistance;
	memset(&fr->EmitterData, 0, sizeof(LegacyEmitterRecordReport));

	fr->EmitterData.AngleOfArrival = track.angle.bearingLast;
	fr->EmitterData.Band = track.rf.getBandEnumeration();

	//trackPriCount = track.pri.accumulatePri.size();
	//if(trackPriCount > 0) {
	//	trackLastPriType = track.pri.accumulatePri[trackPriCount - 1].priType;
	//}

	j = 0;
	k = 0;
	//requires type checks, please fix
	for (i = 0; i < track.pri.accumulatePri.size(); i++)
	{
		if ((track.pri.accumulatePri[i].priType == PriTypeEnum::pri_dwellswitch)
				&& (track.pri.priTypeCurrentDominant
						== PriTypeEnum::pri_dwellswitch)
				&& (track.pri.accumulatePriFixedCount == 1))
		{
			fr->EmitterData.PRIType = 1;
			fr->EmitterData.DwellPRISpot[0] = track.pri.accumulatePri[i].priLow;
			fr->EmitterData.DwellPRI_QualityFlag[0] = 1;
		}
		else if ((track.pri.accumulatePri[i].priType
				== PriTypeEnum::pri_dwellswitch)
				&& (track.pri.priTypeCurrentDominant
						== PriTypeEnum::pri_dwellswitch))
		{
			fr->EmitterData.PRIType = 2;
			if (j < 32)
			{
				fr->EmitterData.DwellPRISpot[j] =
						track.pri.accumulatePri[i].priLow;
				fr->EmitterData.DwellPRI_QualityFlag[j] = 1;
				j++;
			}
		}
		else if ((track.pri.accumulatePri[i].priType == PriTypeEnum::pri_stagger)
				&& (track.pri.priTypeCurrentDominant == PriTypeEnum::pri_stagger))
		{
			fr->EmitterData.PRIType = 3;
			if (j < 64)
			{
				fr->EmitterData.StaggerPhase[j] =
						track.pri.accumulatePri[i].priLow;
				j++;
			}
			if (k < 6)
			{
				if (fr->EmitterData.StaggerFrame[k]
						!= track.pri.accumulatePri[i].frame)
				{
					fr->EmitterData.StaggerFrame[k] =
							track.pri.accumulatePri[i].frame;
					k++;
				}
			}
		}
		else if ((track.pri.accumulatePri[i].priType == PriTypeEnum::pri_jitter)
				&& (track.pri.priTypeCurrentDominant == PriTypeEnum::pri_jitter))
		{
			fr->EmitterData.PRIType = 5;
			fr->EmitterData.JitterPRIMin = track.pri.accumulatePri[i].priLow;
			fr->EmitterData.JitterPRIMax = track.pri.accumulatePri[i].priHigh;
		}
		else if ((track.pri.accumulatePri[i].priType == PriTypeEnum::pri_sliding)
				&& (track.pri.priTypeCurrentDominant == PriTypeEnum::pri_sliding))
		{
			fr->EmitterData.PRIType = 4;
			fr->EmitterData.SlidingPRIMin = track.pri.accumulatePri[i].priLow;
			fr->EmitterData.SlidingPRIMax = track.pri.accumulatePri[i].priHigh;
		}
		else if ((track.pri.accumulatePri[i].priType == PriTypeEnum::pri_complex)
				&& (track.pri.priTypeCurrentDominant == PriTypeEnum::pri_complex))
		{
			fr->EmitterData.PRIType = 6;
			fr->EmitterData.ComplexPRIMin = track.pri.accumulatePri[i].priLow;
			fr->EmitterData.ComplexPRIMax = track.pri.accumulatePri[i].priHigh;
		}
		else if ((track.pri.accumulatePri[i].priType == PriTypeEnum::pri_difftoa)
				&& (track.pri.priTypeCurrentDominant == PriTypeEnum::pri_difftoa))
		{
			fr->EmitterData.PRIType = 7;
			if (j < 32)
			{
				fr->EmitterData.DwellPRISpot[j] =
						track.pri.accumulatePri[i].priLow;
				fr->EmitterData.DwellPRI_QualityFlag[j] = 0;
				j++;
			}
		}
	}
	fr->EmitterData.SlideType = 0;
	fr->EmitterData.SweepType = 0;    //what is sweep type?
	fr->EmitterData.JitterPRI_QualityFlag = 0;
	fr->EmitterData.LPIType = 0;
	fr->EmitterData.New_Updated_Flag =
			track.history.highTrackContinuationIterFlag;

//	if (track.pw.accumulatePw.size() > 0)
//	{
//		fr->EmitterData.PWType = 1;
//	}
//	else
//	{
//		fr->EmitterData.PWType = 0;
//	}
	if (track.pw.accumulatePw.size() > 0)
	{
		if (track.pw.accumulatePw[0].pwType == PwTypeEnum::pw_fixed)
			fr->EmitterData.PWType = 1;
		else if (track.pw.accumulatePw[0].pwType == PwTypeEnum::pw_agile)
			fr->EmitterData.PWType = 2;
		else
			fr->EmitterData.PWType = 0;
	}


	fr->EmitterData.PulseCountQuadrant1 = track.power.quadrantCodeMax;
	fr->EmitterData.PulseCountQuadrant2 = track.power.quadrantCodeMax;
	j = 0;
	for (i = 0; i < track.pw.accumulatePw.size(); i++)
	{
		if (j < 8)
		{
			fr->EmitterData.PulseWidthSpot[j] = track.pw.accumulatePw[i].pwLow;
			j++;
		}
	}
	for (i = 0; i < 8; i++)
	{
		fr->EmitterData.PW_QualityFlag[i] = 0;
	}
	fr->EmitterData.PxQuadrant1 = track.power.pxMax;
	fr->EmitterData.PxQuadrant2 = track.power.pxMax;
	fr->EmitterData.PyQuadrant1 = track.power.pyMax;
	fr->EmitterData.PyQuadrant2 = track.power.pyMax;
	fr->EmitterData.Quadrant1 = 0;
	fr->EmitterData.Quadrant2 = 0;

//	if (track.rf.accumulateRf.size() > 0)
//	{
//		fr->EmitterData.RFType = 2;
//	}
//	else
//	{
//		fr->EmitterData.RFType = 0;
//	}
	if (track.rf.curRfType == RfTypeEnum::rf_fixed)
		fr->EmitterData.RFType = 1;

	else if (track.rf.curRfType == RfTypeEnum::rf_switch)
		fr->EmitterData.RFType = 2;

	else if (track.rf.curRfType == RfTypeEnum::rf_agile)
		fr->EmitterData.RFType = 3;

	 else if(track.rf.curRfType==RfTypeEnum::rf_jump)
	 fr->EmitterData.RFType = 4;

	 else if(track.rf.curRfType==RfTypeEnum::rf_complex)
	 fr->EmitterData.RFType = 6;

	/* else if(track.rf.curRfType==RfTypeEnum::rf_slide)
	 fr->EmitterData.RFType = 7;

	 else if(track.rf.curRfType==RfTypeEnum::rf_sweep)
	 fr->EmitterData.RFType = 7;

	 else if(track.rf.curRfType==RfTypeEnum::rf_sweepsin)
	 fr->EmitterData.RFType = 7;

	 else if(track.rf.curRfType==RfTypeEnum::rf_sweeptriangle)
	 fr->EmitterData.RFType = 7;*/
	 else fr->EmitterData.RFType = 0;


	j = 0;
	for (i = 0; i < track.rf.accumulateRf.size(); i++)
	{
		if (j < 8)
		{
			fr->EmitterData.RFSpots[j] = track.rf.accumulateRf[i].rfAvg;
			j++;
		}
	}
	for (i = 0; i < 16; i++)
	{
		fr->EmitterData.RF_QualityFlag[i] = 0;
	}
	fr->EmitterData.ScanPeriod = 0;

	fr->EmitterData.PulseCountQuadrant1=track.history.totalPulseCount;

//	if(track.last[0].power.scanTypeLast == EmitterArpEnum::scan_circular)
//		fr->EmitterData.Scan_Track = 1;
//	else if(track.last[0].power.scanTypeLast==EmitterArpEnum::scan_lock)
//		fr->EmitterData.Scan_Track = 0;
//	else
//		fr->EmitterData.Scan_Track = 3;
	switch (track.power.scanTypeLast)
	{
		case EmitterArpEnum::scan_circular:
			fr->EmitterData.Scan_Track=1;
			break;
		case EmitterArpEnum::scan_sectorial:
			fr->EmitterData.Scan_Track=4;
			break;
		case EmitterArpEnum::scan_conical:
			fr->EmitterData.Scan_Track=5;
			break;
		case EmitterArpEnum::scan_lock:
			fr->EmitterData.Scan_Track=0;
			break;
		case EmitterArpEnum::scan_tws:
			fr->EmitterData.Scan_Track=6;
			break;
		case EmitterArpEnum::scan_raster:
			fr->EmitterData.Scan_Track = 7;
			break;
		case EmitterArpEnum::scan_complex:
			fr->EmitterData.Scan_Track = 8;
			break;
		default:
			fr->EmitterData.Scan_Track=3;
			break;
	}
//	if (track.power.flagTrackingModeHigh == 1)
//		fr->EmitterData.Scan_Track=0;
//	else if (track.power.flagScanningModeHigh == 1)
//		fr->EmitterData.Scan_Track = 1;
//	else
//		fr->EmitterData.Scan_Track = 3;

//	fr->EmitterData.Scan_Track = 3;
	fr->EmitterData.SelfTest = 0;
	fr->EmitterData.TOAStart = track.general.toaFirst;
	fr->EmitterData.TOAEnd = track.general.toaLast;
	fr->EmitterData.EmitterID = Model::countThreadMainIterHigh;
	//fr->EmitterData.TOAMaxPower = track.power.maxPowerToa;
	//fr->EmitterData.Range = track.range.rangeMeters;
	fr->EmitterData.Range = track.range.amplitudeStepDisplay;
}


void initFlightRecordStructure()
{
	memset(&Model::emitterLog, 0, sizeof(EmitterLog));
	memset(&Model::emitterLogLow, 0, sizeof(EmitterLog));
}

