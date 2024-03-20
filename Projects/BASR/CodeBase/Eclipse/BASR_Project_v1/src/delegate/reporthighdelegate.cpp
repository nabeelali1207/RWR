/*
 * reporthighdelegate.cpp
 *
 *  Created on: Oct 11, 2019
 *      Author: gmd
 */

#define REPORTHIGHDELEGATE_CPP_

#include "../delegate/reporthighdelegate.h"

bool reportReadFromDSP(DataAnalysisClass &analysisObj,
		uint8_t &dataFinishedFlag)
{

	//Model::pcieThreatProcessor.getThreatProcessorReport();
	//insert GMD Logic
	uint64_t bytesReceived = Model::cpuIoHandle.recMessageFromTP(
			Model::classObjectSerializer.getSerializedMessagePointer(),
			Model::classObjectSerializer.getMaxBufferSize());

	bool status = Model::classObjectSerializer.deSerializeClass(analysisObj,
			"THTDATA", bytesReceived);

	dataFinishedFlag = (status == true ? 0 : 1);
	return status;
}

void reportInsertCircularBuffer(DataAnalysisClass &analysisObj,
		circular_buffer<DataAnalysisClass> &analysisBufferObj,
		bool dspStatus)
{
	if (dspStatus == true)
	{
		vector<TrackDefaultClass>::iterator tIter;
		for (tIter = analysisObj.bandEJ.tracks.begin();
				tIter != analysisObj.bandEJ.tracks.end();)
		{
			//delete low track if it has no pri
			if (tIter->general.flagSelfTest == 1)
			{
				tIter = analysisObj.bandEJ.tracks.erase(tIter);
			}
			else
			{
				tIter++;
			}
		}
		analysisBufferObj.push_back(analysisObj);
	}
	//this is disabled until we learn more about dsp status variable
	else
	{		//if there is a timeout, insert empty report in circular buffer
		DataAnalysisClass analysisObjEmpty;
		analysisBufferObj.push_back(analysisObjEmpty);
	}
	//this is temporary
	//Model::analysisHigh = analysisBufferObj[1];
}
void printHighEmitters(DataAnalysisClass &analysisObj,
		uint64_t currSystemIteration)
{
//	if ((currSystemIteration % 10) != 0)
//	{
//		return;
//	}
	if (analysisObj.bandEJ.tracks.size() > 0
			|| analysisObj.bandCD.tracks.size() > 0
			|| analysisObj.bandK.tracks.size() > 0)
	{
		printf("Emitters\n");
		printf(
				"ToaFirst\tToaLast\t\tBand\tAngle\tPRIType\tPRI\tPW\tRF\tLibID\n");
	}
	for (uint16_t i = 0; i < analysisObj.bandEJ.tracks.size(); i++)
	{
		TrackDefaultClass &track = analysisObj.bandEJ.tracks[i];
		printf("%05.08f \t",
				((double) track.general.toaFirst) / (double) 1.0e8);
		printf("%05.08f \t", ((double) track.general.toaLast) / (double) 1.0e8);

		printf("EJ%02d\t%d\t%lu\t", i, track.angle.bearingLast,
				track.pri.accumulatePri.size());
		if (track.pri.accumulatePri.size() > 0)
		{
			printf("%d\t", track.pri.accumulatePri.front().priMean);
		}
		else
		{
			printf("0\t");
		}
		if (track.pw.accumulatePw.size() > 0)
			printf("%d\t", track.pw.accumulatePw.front().pwAvg);
		else
			printf("0\t");

		if (track.rf.accumulateRf.size() > 0)
			printf("%d\t", track.rf.accumulateRf.front().rfAvg);
		else
			printf("0\t");
		if(track.libmatch.active.flagKnownLibrary==1)
		{
			printf("%d\n", track.libmatch.active.highestMatchList[0].ID);
		}
		else
		{
			printf("%d\n", 0);
		}


	}
	for (uint16_t i = 0; i < analysisObj.bandCD.tracks.size(); i++)
	{
		TrackDefaultClass &track = analysisObj.bandCD.tracks[i];
		printf("%05.08f \t",
				((double) track.general.toaFirst) / (double) 1.0e8);
		printf("%05.08f \t", ((double) track.general.toaLast) / (double) 1.0e8);

		printf("CD%02d\t%d\t%lu\t", i, track.angle.bearingLast,
						track.pri.accumulatePri.size());
				if (track.pri.accumulatePri.size() > 0)
				{
					printf("%d\t", track.pri.accumulatePri.front().priMean);
				}
				else
				{
					printf("0\t");
				}
				if (track.pw.accumulatePw.size() > 0)
					printf("%d\t", track.pw.accumulatePw.front().pwAvg);
				else
					printf("0\t");

				if (track.rf.accumulateRf.size() > 0)
					printf("%d\t", track.rf.accumulateRf.front().rfAvg);
				else
					printf("0\t");
				if(track.libmatch.active.flagKnownLibrary==1)
				{
					printf("%d\n", track.libmatch.active.highestMatchList[0].ID);
				}
				else
				{
					printf("%d\n", 0);
				}

	}

	for (uint16_t i = 0; i < analysisObj.bandK.tracks.size(); i++)
	{
		TrackDefaultClass &track = analysisObj.bandK.tracks[i];
		printf("%05.08f \t",
				((double) track.general.toaFirst) / (double) 1.0e8);
		printf("%05.08f \t", ((double) track.general.toaLast) / (double) 1.0e8);

		printf("_K%02d\t%d\t%lu\t", i, track.angle.bearingLast,
						track.pri.accumulatePri.size());
				if (track.pri.accumulatePri.size() > 0)
				{
					printf("%d\t", track.pri.accumulatePri.front().priMean);
				}
				else
				{
					printf("0\t");
				}
				if (track.pw.accumulatePw.size() > 0)
					printf("%d\t", track.pw.accumulatePw.front().pwAvg);
				else
					printf("0\t");

				if (track.rf.accumulateRf.size() > 0)
					printf("%d\t", track.rf.accumulateRf.front().rfAvg);
				else
					printf("0\t");
				if(track.libmatch.active.flagKnownLibrary==1)
				{
					printf("%d\n", track.libmatch.active.highestMatchList[0].ID);
				}
				else
				{
					printf("%d\n", 0);
				}

	}
	if (analysisObj.bandEJ.tracks.size() > 0
			|| analysisObj.bandCD.tracks.size() > 0
			|| analysisObj.bandK.tracks.size() > 0)
	{
		printf("\n");
	}
//	if (analysisObj.bandEJ.tracks.size() > 0)
//	{
//		if (analysisObj.bandEJ.tracks[0].pri.priCircular.size() > 0)
//		{
//			cout << "Pri: "
//					<< analysisObj.bandEJ.tracks[0].pri.priCircular.front().priMean
//					<< endl;
//		}
//	}
}

void printHighCwEmitters(vector<TrackDefaultClass> &tracks)
{
	if (tracks.size() > 0)
	{
		printf("Band\tAngle\tQuad\tHeadng\tRF\tGS\tA\tB\tC\tD\n");
	}
	for (uint32_t i = 0; i < tracks.size(); i++)
	{
		TrackDefaultClass &track = tracks[i];
		printf("CW%02d\t%d\t%d\t%d\t%d\t", i, track.angle.curAngle,
				track.angle.curQuadrant, track.angle.bearingDisplay,
				track.rf.accumulateRf[0].rfAvg);
		printf("%d\t%d\t%d\t%d\t%d\n", track.power.cwGainStage,track.power.avgPA,track.power.avgPB,track.power.avgPC,track.power.avgPD);
	}
	if (tracks.size() > 0)
	{
		printf("\n");
	}
}
