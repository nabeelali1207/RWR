/*
 * logginglowclass.cpp
 *
 *  Created on: Jan 23, 2019
 *      Author: hassan
 */

#define LOGLOWCLASS_CPP_

#include "loglowclass.h"

void LogLowClass::logClusterSize()
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}

}

void LogLowClass::logClusteringBenchmark(double start, double end)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "clustering: " << end - start << " sec" << endl;
}

void LogLowClass::logClusterNewCount(uint16_t clusterCount)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "new clusters: " << clusterCount << endl;
}

void LogLowClass::logClusterInput(double paramA, double paramB, double paramC,
		double paramD)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << paramA << " - ";
	cout << paramB << " - ";
	cout << paramC << " - ";
	cout << paramD << " - ";
	cout << endl;
}

void LogLowClass::logClusterInputSize(uint64_t clusterInputSize)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "dbscan input size: " << clusterInputSize << endl;
}

void LogLowClass::logPriAnalysisBenchmark(double start, double end)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "pri analysis: " << end - start << " sec" << endl;
}

void LogLowClass::logThreadMainIterInitCount(uint64_t toaStartFront,
		uint64_t toaStartBack, uint16_t size)
{

	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "Toa Start: ";
	cout << toaStartFront;
	cout << endl;
	cout << "Toa End: ";
	cout << toaStartBack;
	cout << endl;
	cout << "Data Rows: ";
	cout << size;
	cout << endl;
	cout << "Initialization Complete. Starting Main Loop" << endl;
	cout << "" << endl;
}

void LogLowClass::logThreadMainIterCount(uint64_t threadMainIterationCount)
{

	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	//cout << "Main Thread Iteration Count: " <<
	//		threadMainIterationCount << endl;
	cout << "." << endl;
	//cout << threadMainIterationCount << endl;
}

void LogLowClass::logThreadMainIteration()
{

	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
}

void LogLowClass::logThreadMainIterPenalty(uint64_t timeSinceLastIteration)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "Time Penalty: " << timeSinceLastIteration << endl;
}

void LogLowClass::logThreadMainIterSimTime(uint64_t timeSinceLastIteration,
		uint64_t timeToWait, uint64_t simTimeMsIterationActual)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "LastIt: " << timeSinceLastIteration << " Wait: " << timeToWait
			<< " TOABuff: " << simTimeMsIterationActual << endl;
}

void LogLowClass::logThreadMainIterSimInput(uint64_t lowerTime,
		uint64_t upperTime, uint64_t lowerActualTime, uint64_t upperActualTime,
		uint64_t actualTimeDiff)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "LowerLimit: " << lowerTime << " LowToa: " << lowerActualTime
			<< " UpperToa: " << upperActualTime << " UpperLimit: " << upperTime
			<< " ToaDiff: " << actualTimeDiff << endl;
}

void LogLowClass::logThreadMainIterSimRemain(uint64_t pdwListSize,
		uint32_t bandLowPdwSize, uint32_t bandHighPdwSize, uint64_t iterCount)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "Sim Pdw Buff: " << pdwListSize << endl;
	cout << "Band L: " << bandLowPdwSize << " Band H: " << bandHighPdwSize
			<< " Iter: " << iterCount << endl;
}

void LogLowClass::logThreadCompleteInputTotalPdw(uint64_t pdwTotal,
		uint64_t threadMainIterationCount)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "Total: " << pdwTotal << " Iter: " << threadMainIterationCount
			<< endl;
}

void LogLowClass::logLimitToaStart(uint64_t toaLowerLimit,
		uint64_t timeOfArrivalStart)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "Limit Start Toa: " << toaLowerLimit << " Actual: "
			<< timeOfArrivalStart << endl;
}

void LogLowClass::logLimitToaEnd(uint64_t toaUpperLimit,
		uint64_t timeOfArrivalStart)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "Limit End Toa: " << toaUpperLimit << " Actual: "
			<< timeOfArrivalStart << endl;
}

void LogLowClass::logLimitToaTotal(uint16_t listSize)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "Limit Toa Total: " << listSize << endl;
}

void LogLowClass::logCleanInputPdwStart()
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "Cleaning Input Pdw List" << endl;
}

void LogLowClass::logCleanInputPdwEnd()
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "End of Cleaning Pdw List" << endl;
}

void LogLowClass::logEraseBoundPdw(uint64_t deletionToaLower,
		uint64_t deletionToaUpper)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "Erase candidate bound toa low: " << deletionToaLower
			<< " toa high: " << deletionToaUpper << endl;
}

void LogLowClass::logEraseInputPdw(uint64_t timeOfArrivalStart)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "Erasing Pdw: " << timeOfArrivalStart << endl;
}

void LogLowClass::logProgramStartup()
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "Program Starting..." << endl;
	cout << "Starting Initializing Delegate..." << endl;
}

void LogLowClass::logProgramTerminate()
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << "End of Program." << endl;
}

void LogLowClass::logBreakLine()
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << endl;
}

void LogLowClass::logn(string input)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << input << endl;
}

void LogLowClass::logn(string message, uint64_t value)
{
	if (ConstLow::logOverallFlag == false)
	{
		return;
	}
	cout << message << " : " << value << endl;
}
void LogLowClass::writeReportCsv(vector<TrackDefaultClass> &tracks,
		std::string fileName)
{
	if (ConstLow::logDspCsvReportFlag == false)
	{
		return;
	}
	std::ofstream fidTP;
	fidTP.open(ConstLow::csvOutFileName, std::ios::out | std::ios::app);// std::ios::out | std::ios::app);// TP output report
	for (uint16_t i = 0; i < tracks.size(); i++)
	{
//		if (tracks[i].general.dspTrackingEnabled == false)
//		{
//			continue;
//		}
//		if(tracks[i].pri.priTrackingObj[0].GetCurrPriType() == SequenceTypesEnum::seq_unknown)
//			continue;
		TrackDefaultClass & src = tracks[i];
		if (!fidTP)
		{
			cout << "Error opening file" << endl;
			return;
		}
		fidTP << std::endl;
		fidTP << src.general.trackID;
		fidTP << ',';
		switch (src.general.dspPdwBuffer.back().band)
		{
		case BandCodeEnum::band_e:
			fidTP << "E";
			break;
		case BandCodeEnum::band_g:
			fidTP << "G";
			break;
		case BandCodeEnum::band_i:
			fidTP << "I";
			break;
		case BandCodeEnum::band_j:
			fidTP << "J";
			break;
		case BandCodeEnum::band_cd:
			fidTP << "CD";
			break;
		case BandCodeEnum::band_k:
			fidTP << "K";
			break;
		default:
			fidTP << "ERR";
		}

		fidTP << ',';
		fidTP << src.angle.curAngle;
		fidTP << ',';
		if (src.power.groundIlluminationFlag == 1)
			fidTP << "groundIllumPresent";
		else
			fidTP << "NoGroundIllum";
		fidTP << ',';
		fidTP << src.general.dspIterationPulseCount;
		fidTP << ',';
		switch (src.pw.curPwType)
		{
		case PwTypeEnum::pw_fixed:
			fidTP << "Fixed";
			break;
		case PwTypeEnum::pw_agile:
			fidTP << "Agile";
			break;
		case PwTypeEnum::pw_uncomputed:
			fidTP << "UNC";
			break;
		default:
			fidTP << "ERR";
		}
		fidTP << ',';
//		for(uint32_t i=0; i < src.pw.pwCircular.size() ; i++)
//		{
//			fidTP << src.pw.pwCircular[i].pwAvg;
//			fidTP << ' ';
//		}
//		if (src.pw.pwCircular.size() == 0)
//		{
//			fidTP << 0;
//		}
		for (uint32_t i = 0; i < src.pw.pwTrackingObj[0].pwSpots.size(); i++)
		{
			fidTP << src.pw.pwTrackingObj[0].pwSpots[i];
			fidTP << ' ';
		}
		if (src.pw.pwTrackingObj[0].pwSpots.size() == 0)
		{
			fidTP << 0;
		}
		fidTP << ',';
		fidTP << src.general.toaFirst;
		fidTP << ',';
		fidTP << src.general.toaLast;
		fidTP << ',';
		for (uint32_t i = 0; i < src.power.illuminationInfo.size(); i++)
		{
			fidTP << src.power.illuminationInfo[i].cycleStartTime;
			fidTP << ' ';
			fidTP << src.power.illuminationInfo[i].cycleDuration;
			fidTP << ' ';
			fidTP << src.power.illuminationInfo[i].cyclePulseCount;
			fidTP << ' ';
		}
		fidTP << ',';
		if (src.power.scanAnalysis[0].GetEmitterMode() == EmitterMode::scan)
			fidTP << "scan";
		else if (src.power.scanAnalysis[0].GetEmitterMode()
				== EmitterMode::track)
			fidTP << "track";
		else
			fidTP << "unknown";
		fidTP << ',';
//		if (src.power.avgPA > src.power.avgPC)
//			fidTP << src.power.avgPA;
//		else
//			fidTP << src.power.avgPC;
//		fidTP << ',';
//		if (src.power.avgPB > src.power.avgPD)
//			fidTP << src.power.avgPB;
//		else
//			fidTP << src.power.avgPD;
		fidTP << src.power.pxMax;
		fidTP << ',';
		fidTP << src.power.pyMax;
		fidTP << ',';
		switch (src.rf.curRfType)
		{
		case RfTypeEnum::rf_fixed:
			fidTP << "Fixed";
			break;
		case RfTypeEnum::rf_switch:
			fidTP << "dwell";
			break;
		case RfTypeEnum::rf_jump:
			fidTP << "Jump";
			break;
		case RfTypeEnum::rf_agile:
			fidTP << "Agile";
			break;
		case RfTypeEnum::rf_complex:
			fidTP << "Complex";
			break;
		case RfTypeEnum::rf_uncomputed:
			fidTP << "UNC";
			break;
		default:
			fidTP << "ERR";
		}
		fidTP << ',';
		switch (src.rf.rfTrackingObj[0].GetCurrRfType()) {

		case SequenceTypesEnum::seq_dwell: {
			vector<uint32_t> &dwellSpots =
					src.rf.rfTrackingObj[0].GetDwellSpots();
			fidTP << dwellSpots.size();
			fidTP << ',';
			for (uint16_t i = 0; i < dwellSpots.size(); i++) {
				fidTP << dwellSpots[i];
				fidTP << ',';
			}
			break;
		}
		case SequenceTypesEnum::seq_difftoa: {
			vector<uint32_t> &rfSpots = src.rf.rfTrackingObj[0].GetRfSpots();
			fidTP << rfSpots.size();
			fidTP << ',';
			for (uint16_t i = 0; i < rfSpots.size(); i++) {
				fidTP << rfSpots[i];
				fidTP << ',';
			}
			break;
		}

		case SequenceTypesEnum::seq_jitter: {
			vector<uint32_t> &rfSpots = src.rf.rfTrackingObj[0].GetRfSpots();
			fidTP << rfSpots.size();
			fidTP << ',';
			for (uint16_t i = 0; i < rfSpots.size(); i++) {
				fidTP << rfSpots[i];
				fidTP << ',';
			}
			break;
		}
		case SequenceTypesEnum::seq_complex: {
			vector<uint32_t> &rfSpots = src.rf.rfTrackingObj[0].GetRfSpots();
			fidTP << rfSpots.size();
			fidTP << ',';
			for (uint16_t i = 0; i < rfSpots.size(); i++) {
				fidTP << rfSpots[i];
				fidTP << ',';
			}
			break;
		}
		default:
			fidTP << 0;
			fidTP << ',';
			break;

		}

		fidTP << ',';
//		LimitPriClass &tempPriObj = src.pri.priCircular.back();

		//case tempPriObj.priType::pri_dwellswitch:
		switch (src.pri.priTrackingObj[0].GetCurrPriType())
		{
		case SequenceTypesEnum::seq_unknown:
			fidTP << "unknown";
			break;
		case SequenceTypesEnum::seq_dwell:
			fidTP << "Dwell";
			break;
		case SequenceTypesEnum::seq_stagger:
			fidTP << "Stagger";
			break;
		case SequenceTypesEnum::seq_difftoa:
			fidTP << "difftoapri";
			break;
		case SequenceTypesEnum::seq_slidePos:
			fidTP << "posSlide";
			break;
		case SequenceTypesEnum::seq_slideNeg:
			fidTP << "NegSlide";
			break;
		case SequenceTypesEnum::seq_slideTri:
			fidTP << "Tri";
			break;
		case SequenceTypesEnum::seq_slideSine:
			fidTP << "Sine";
			break;
		case SequenceTypesEnum::seq_jitter:
			fidTP << "jitter";
			break;
		case SequenceTypesEnum::seq_complex:
			fidTP << "complex";
			break;
		default:
			break;
		}
		fidTP << ',';
		switch (src.pri.priTrackingObj[0].GetCurrPriType())
		{
		case SequenceTypesEnum::seq_stagger:
		{
			vector<int32_t> &staggerSpots =
					src.pri.priTrackingObj[0].GetStaggerSpots();

			fidTP << staggerSpots.size();
			fidTP << ',';
			for (uint16_t i = 0; i < staggerSpots.size(); i++)
			{
				fidTP << staggerSpots[i];
				fidTP << ',';
			}
			break;
		}
		case SequenceTypesEnum::seq_dwell:
		{
			vector<int32_t> &dwellSpots =
					src.pri.priTrackingObj[0].GetDwellSpots();
			fidTP << dwellSpots.size();
			fidTP << ',';
			for (uint16_t i = 0; i < dwellSpots.size(); i++)
			{
				fidTP << dwellSpots[i];
				fidTP << ',';
			}
			break;
		}
		case SequenceTypesEnum::seq_difftoa:
		{
			vector<int32_t> &priSpots = src.pri.priTrackingObj[0].GetPriSpots();
			fidTP << priSpots.size();
			fidTP << ',';
			for (uint16_t i = 0; i < priSpots.size(); i++)
			{
				fidTP << priSpots[i];
				fidTP << ',';
			}
			break;
		}
		case SequenceTypesEnum::seq_slidePos:
		{
			fidTP << 3;
			fidTP << ',';

			fidTP << src.pri.priTrackingObj[0].GetPosSlideStartPri();
			fidTP << ',';

			fidTP << src.pri.priTrackingObj[0].GetPosSlideRate();
			fidTP << ',';
			fidTP << src.pri.priTrackingObj[0].GetPosSlideEndPri();
			fidTP << ',';

			break;
		}
		case SequenceTypesEnum::seq_slideNeg:
		{
			fidTP << 3;
			fidTP << ',';

			fidTP << src.pri.priTrackingObj[0].GetNegSlideStartPri();
			fidTP << ',';

			fidTP << src.pri.priTrackingObj[0].GetNegSlideRate();
			fidTP << ',';
			fidTP << src.pri.priTrackingObj[0].GetNegSlideEndPri();
			fidTP << ',';

			break;
		}
		case SequenceTypesEnum::seq_slideTri:
		{
			fidTP << 3;
			fidTP << ',';

			fidTP << src.pri.priTrackingObj[0].GetTriSlideStartPri();
			fidTP << ',';

			fidTP << src.pri.priTrackingObj[0].GetTriSlideRate();
			fidTP << ',';
			fidTP << src.pri.priTrackingObj[0].GetTriSlideEndPri();
			fidTP << ',';

			break;
		}

		case SequenceTypesEnum::seq_slideSine:
		{
			fidTP << 3;
			fidTP << ',';

			fidTP << src.pri.priTrackingObj[0].GetSineSlideStartPri();
			fidTP << ',';

			fidTP << src.pri.priTrackingObj[0].GetSineSlideStartPri();
			fidTP << ',';
			fidTP << src.pri.priTrackingObj[0].GetSineSlideEndPri();
			fidTP << ',';

			break;
		}
		case SequenceTypesEnum::seq_jitter:
		{
			vector<int32_t> &priSpots = src.pri.priTrackingObj[0].GetPriSpots();
			fidTP << priSpots.size();
			fidTP << ',';
			for (uint16_t i = 0; i < priSpots.size(); i++)
			{
				fidTP << priSpots[i];
				fidTP << ',';
			}
			break;
		}
		case SequenceTypesEnum::seq_complex:
		{
			vector<int32_t> &priSpots = src.pri.priTrackingObj[0].GetPriSpots();
			fidTP << priSpots.size();
			fidTP << ',';
			for (uint16_t i = 0; i < priSpots.size(); i++)
			{
				fidTP << priSpots[i];
				fidTP << ',';
			}
			break;
		}
		default:
			fidTP << 0;
			fidTP << ',';
			break;

		}
		fidTP << ',';
//		uint16_t tempArray[4] = {src.power.stdPA, src.power.stdPB, src.power.stdPC,src.power.stdPD};
//
//		fidTP << tempArray[src.power.dominantAntenna];
//		fidTP << ',';
		fidTP << (int32_t) src.pri.priTrackingObj[0].priTrackingPossibleFlag;
		fidTP << std::endl;
	}
	fidTP.close();
}
LogLowClass::LogLowClass()
{

}

LogLowClass::~LogLowClass()
{
	// TODO Auto-generated destructor stub
}
