/*
 * simulationlowdelegate.cpp
 *
 *  Created on: Nov 3, 2018
 *      Author: test
 */

#define SIMULATIONLOWDELEGATE_CPP_

#include "simulationlowdelegate.h"
#include <fstream>
#include "../vo/loglowclass.h"

void simLowInitializeParameters(DataSimulationLowClass &simLowData)
{
	if (ConstLow::simIterationTypeFlag
			!= LowIterationTypeFlags::simulationModeDisabled)
	{
		simLowUpdateToaStartTime(
				simLowData.bandAllPdwBuffer.front().timeOfArrivalStart,
				simLowData);
	}
}

void simLowUpdateSystemToToaOffset(DataSimulationLowClass &simLowData)
{
	// by comparing current toa time to current system time
	// we can compute an offset that transforms system time to TOA time and vice versa
	// offset is used by ConvertSystemTimeToToaTime(uint64_t) function
	simLowData.systemTimeToToaTimeOffset = simLowData.currSystemTime;
	simLowData.systemTimeToToaTimeOffset = simLowData.systemTimeToToaTimeOffset
			- (int64_t) simLowData.pdwIterationEndTime
					/ ConstLow::toaCountInMicros;
}
uint64_t simLowConvertSystemTimeToToaTime(uint64_t systemTime,
		DataSimulationLowClass &simLowData)
{
	// there is an offset that converts current system time to TOA time
	// this offset is computed by updateSystemToToaOffset() function
	return (uint64_t) ((int64_t) systemTime
			- simLowData.systemTimeToToaTimeOffset) * ConstLow::toaCountInMicros;
}
uint64_t simLowGetIterationToaStartTime(DataSimulationLowClass &simLowData)
{
	return simLowData.pdwIterationStartTime;
}
uint64_t simLowGetIterationToaEndTime(DataSimulationLowClass &simLowData)
{
	return simLowData.pdwIterationEndTime;
}
void simLowUpdateToaStartTime(uint64_t startToa,
		DataSimulationLowClass &simLowData)
{
	//in simulation mode this function will be called with toa of first input pdw
	//TODO gmd-> In realtime-pdws we need to periodically call this function to cater for clock drift
	simLowData.pdwIterationEndTime = simLowData.pdwIterationStartTime =
			startToa;
	simLowData.currSystemTime = simLowData.lastSystemTime =
			Utility::getSystemTime();
	simLowUpdateSystemToToaOffset(simLowData);
}
void simLowUpdateIterationTime(DataSimulationLowClass &simLowData)
{
	if (ConstLow::simIterationTypeFlag
			== LowIterationTypeFlags::simulationModeDisabled)
	{
		return;
	}
	//this function sets the TOA-start and TOA-end that will be processed in current iteration
	simLowData.lastSystemTime = simLowData.currSystemTime;
	simLowData.currSystemTime = Utility::getSystemTime();

	uint64_t lastIterationActualDuration = simLowData.currSystemTime
			- simLowData.lastSystemTime;
	uint64_t lastIterationEstimatedDuration = (simLowData.pdwIterationEndTime
			- simLowData.pdwIterationStartTime) / ConstLow::toaCountInMicros;

	uint64_t thisIterationEstimatedDuration = 0;
	int64_t timeToWait = 0;

	int64_t tempStartPoint = 0, tempEndTime = 0;

	if ((ConstLow::simIterationTypeFlag
			== LowIterationTypeFlags::fixedTimeIterationFastSimulation)
			|| (ConstLow::simIterationTypeFlag
					== LowIterationTypeFlags::fixedTimeIterationSlowSimulation))
	{
		// end toa-time is advanced by fixed amount in this mode

		thisIterationEstimatedDuration = 1000 * ConstLow::simIterationFixedTime;

		tempEndTime = simLowData.pdwIterationEndTime;
		simLowData.pdwIterationEndTime += thisIterationEstimatedDuration
				* ConstLow::toaCountInMicros;

		tempStartPoint = (int64_t) simLowData.pdwIterationEndTime;
		tempStartPoint = tempStartPoint
				- ((int64_t) thisIterationEstimatedDuration
						* ConstLow::toaCountInMicros);

		simLowData.pdwIterationStartTime = (
				tempStartPoint < 0 ? tempEndTime : tempStartPoint);

	}
	else if (ConstLow::simIterationTypeFlag
			== LowIterationTypeFlags::readIterationTimeFromFile)
	{

		if (Model::countThreadMainIterLow
				< Model::simLowData.iterationTimeFromFile_lastReadObject.header.systemIteration)
		{
			usleep(10);
		}
		else if (Model::countThreadMainIterLow
				== Model::simLowData.iterationTimeFromFile_lastReadObject.header.systemIteration)
		{

			simLowData.pdwIterationStartTime =
					Model::simLowData.iterationTimeFromFile_lastReadObject.iterationStartToa;
			simLowData.pdwIterationEndTime =
					Model::simLowData.iterationTimeFromFile_lastReadObject.iterationEndToa
							+ 1;
			simLowData.callReportingFlag =
					Model::simLowData.iterationTimeFromFile_lastReadObject.reportingFlag;

			if (Model::simLowData.iterationTimeFromFile_fileStream.is_open()
					== true)
			{
				LogIterationInfoClass tempObject;
				int64_t remainingBytes =
						Model::simLowData.iterationTimeFromFile_ptrMaxSize
								- Model::simLowData.iterationTimeFromFile_ptr;

				if (remainingBytes >= (int64_t) sizeof(LogIterationInfoClass))
				{
					tempObject.operator >>(
							Model::simLowData.iterationTimeFromFile_fileStream);
					Model::simLowData.iterationTimeFromFile_ptr =
							Model::simLowData.iterationTimeFromFile_fileStream.tellg();
					Model::simLowData.iterationTimeFromFile_lastReadObject =
							tempObject;
				}
				else
				{
					Model::simLowData.iterationTimeFromFile_fileStream.close();
				}
			}
			else
			{
				ConstLow::simIterationTypeFlag =
						LowIterationTypeFlags::fixedTimeIterationFastSimulation;
			}
		}
		else
		{
			Model::simLowData.iterationTimeFromFile_fileStream.close();
			ConstLow::simIterationTypeFlag =
					LowIterationTypeFlags::fixedTimeIterationFastSimulation;
		}

	}
	else
	{
		// a mode where iteration time is not fixed
		if (lastIterationActualDuration < ConstLow::simIterationMinTime * 1000)
		{ // incase of very quick iteration, sleep for 500 micro seconds
			setSystemDelayMicroseconds(
					(ConstLow::simIterationMinTime * 1000)
							- lastIterationActualDuration);
			thisIterationEstimatedDuration = ConstLow::simIterationMinTime
					* 1000;

			simLowData.pdwIterationStartTime = simLowData.pdwIterationEndTime;
			simLowData.pdwIterationEndTime = simLowData.pdwIterationStartTime
					+ thisIterationEstimatedDuration
							* ConstLow::toaCountInMicros;

		}
		else if (lastIterationActualDuration > lastIterationEstimatedDuration)
		{ //we are lagging behind
			tempEndTime = simLowData.pdwIterationEndTime;
			simLowData.pdwIterationStartTime = simLowData.pdwIterationEndTime;

			//allow lagging behind for a specified amount of time
			if (lastIterationActualDuration
					> (ConstLow::simIterationMaxTime * 1000))
			{ //we have to discard some data
			  // this also resets the start and end times
				thisIterationEstimatedDuration = 1000
						* ConstLow::simIterationFixedTime;

				simLowData.pdwIterationEndTime =
						simLowData.pdwIterationStartTime
								+ lastIterationActualDuration
										* ConstLow::toaCountInMicros;

				simLowData.pdwIterationStartTime =
						simLowData.pdwIterationEndTime
								- thisIterationEstimatedDuration
										* ConstLow::toaCountInMicros;

			}
			else
			{ // allow some extra data to be processed
				thisIterationEstimatedDuration = lastIterationActualDuration;
				simLowData.pdwIterationEndTime =
						simLowData.pdwIterationStartTime
								+ thisIterationEstimatedDuration
										* ConstLow::toaCountInMicros;
			}

//			tempStartPoint = (int64_t) simLowData.pdwIterationEndTime;
//			tempStartPoint = tempStartPoint
//					- ((int64_t) thisIterationEstimatedDuration);

			//start time of this iteration is tricky to set as
			//1. we may have to ignore some pulses due to system lagging behind
			//2. at start when endPoint is close to zero, overflow occurs due to minus operation
//			simLowData.pdwIterationStartTime = (
//					tempStartPoint < 0 ? tempEndTime : tempStartPoint);
		}
		else
		{
			thisIterationEstimatedDuration = lastIterationActualDuration;

			simLowData.pdwIterationStartTime = simLowData.pdwIterationEndTime;
			simLowData.pdwIterationEndTime = simLowData.pdwIterationStartTime
					+ thisIterationEstimatedDuration
							* ConstLow::toaCountInMicros;
		}

	}

	if (ConstLow::simIterationTypeFlag
			== LowIterationTypeFlags::fixedTimeIterationSlowSimulation)
	{ // to sleep or not
		if (lastIterationActualDuration > lastIterationEstimatedDuration)
		{ //no need to wait, we are already lagging behind

		}
		else
		{
			timeToWait = (int64_t) lastIterationEstimatedDuration
					- (int64_t) lastIterationActualDuration;
			setSystemDelayMicroseconds((uint64_t) timeToWait);
		}
	}
//	cout << "Last Iteration Time (Estimate: " << lastIteartionEstimatedDuration
//			<< " Actual: " << lastIterationActualDuration
//			<< ")\nThis Iteration Estimated Time: "
//			<< thisIteartionEstimatedDuration << " Sleep Time: " << timeToWait
//			<< endl;
	if (ConstLow::logOverallFlag == true)
	{
		cout << "Start Time : " << simLowData.pdwIterationStartTime
				<< " End Time : " << simLowData.pdwIterationEndTime
				<< " Duration : "
				<< simLowData.pdwIterationEndTime
						- simLowData.pdwIterationStartTime << endl;
	}
//	simLowData.lastSystemTime = simLowData.currSystemTime;
//	simLowData.currSystemTime = Utility::getSystemTime();


}
void simLowSetConstants()
{
//	ConstLow::hexDebugIterDataFileName = "output/130508_527_544_CompleteData_debugItrData.hex";

	if (ConstLow::simOnActualHwFlag == 1)
	{
//		if (ConstLow::simIterationTypeFlag
//				== LowIterationTypeFlags::simulationModeDisabled)
//		{// play back mode
//			ConstLow::matInFileName = "livePdwsFlush.hex";
//		}

		ConstLow::matInFileName = "/media/sda1/" + ConstLow::matInFileName;
		ConstLow::csvOutFileName = "/media/sda1/" + ConstLow::csvOutFileName;
		ConstLow::hexDebugFileName = "/media/sda1/" + ConstLow::hexDebugFileName;
		ConstLow::dspReportFileName = "/media/sda1/" + ConstLow::dspReportFileName;
		ConstLow::dspPdwFlushFileName = "/media/sda1/"
				+ ConstLow::dspPdwFlushFileName;
		ConstLow::hexDebugIterDataFileName = "/media/sda1/"
				+ ConstLow::hexDebugIterDataFileName;
		return;

	}
	if (Constant::simParametersByScript == 1)
	{
		return;
	}

//	ConstLow::matInFileName = "input/AOA_Variation.mat";
//	ConstLow::matInFileName = "input/Case_4.9_PUP1.5_Version2.mat";
//	ConstLow::matInFileName = "input/PRI_difftoa_RT.mat";
//	ConstLow::matInFileName = "input/PRI_DnS_1_PUP.mat";
//	ConstLow::matInFileName = "input/PRI_DnS_2_PUP.mat";
//	ConstLow::matInFileName = "input/PRI_DnS_3_PUP.mat";
//	ConstLow::matInFileName = "input/PRI_DnS_4.mat";
//	ConstLow::matInFileName = "input/PRI_difftoa_1.mat";
//	ConstLow::matInFileName = "input/PRI_Jitter_1_PUP.mat";
//	ConstLow::matInFileName = "input/PRI_Complex_2_PUP.mat";
//	ConstLow::matInFileName = "input/Atp/PRI_Stagger_1_PUP.mat";
//	ConstLow::matInFileName = "input/PRI_Stagger_1_20m.mat";
//	ConstLow::matInFileName = "input/PRI_Stagger_2.mat";
//	ConstLow::matInFileName = "input/PRI_Stagger_3.mat";
//	ConstLow::matInFileName = "input/PRI_Stagger_4.mat";
//	ConstLow::matInFileName = "input/PRI_Stagger_4_20m.mat";
//	ConstLow::matInFileName = "input/PRI_Stagger_5.mat";
//	ConstLow::matInFileName = "input/PRI_Stagger_6.mat";
//	ConstLow::matInFileName = "input/PRI_Stagger_7.mat";
//	ConstLow::matInFileName = "input/PRI_Stagger_8.mat";
//	ConstLow::matInFileName = "input/PRI_Stagger_9.mat";
//	ConstLow::matInFileName = "input/PRI_Slide_5_PUP.mat";
//	ConstLow::matInFileName = "input/RF_Agile_5_PUP.mat";
//	ConstLow::matInFileName = "input/Atp/MultiEmitters_10_PUP.mat";
//	ConstLow::matInFileName = "input/Atp/MultiEmitters_2_PUP.mat";
//	ConstLow::matInFileName = "input/PRI_Slide_1.mat";
//	ConstLow::matInFileName = "input/scan_circular_PUP.mat";
//	ConstLow::matInFileName = "input/Atp/RF_Step_1_PUP.mat";
//	ConstLow::matInFileName ="input/130719_Flt2of3_IBand_10F11_1578_1875secs.mat";
//	ConstLow::matInFileName ="input/MultiEmitters_6.mat";
//	ConstLow::matInFileName ="input/PRI_DnS_2.mat";
//	ConstLow::matInFileName = "input/PRI_Slide_1_10m.mat";
//	ConstLow::matInFileName = "input/PRI_Slide_6.mat";
//	ConstLow::matInFileName = "input/PRI_Slide_8.mat";
//	ConstLow::matInFileName = "input/PRI_Slide_10.mat";
//	ConstLow::matInFileName = "input/130508_Flt1_of_1_2100_2600secs_faultremoved.mat";

//	ConstLow::matInFileName = "input/livepdws.mat";
//	ConstLow::matInFileName = "input/CD_Band_1.mat";
//	ConstLow::matInFileName = "input/allBandCase_1.mat";

//	ConstLow::matInFileName = "input/MultipleDAsFMRTest_AllBands_1to1000pages.mat";
//	ConstLow::matInFileName = "input/MultipleDAsFMRTest_AllBands_106001to107000pages.mat";
//	ConstLow::matInFileName = "input/130508_Flt1_of_1_2100_2600secs.mat";
//	ConstLow::matInFileName = "input/130508_Flt1_of_1_2600_3000secs.mat";
//	ConstLow::matInFileName = "input/130508_Flt1_of_1_2100_2600secs3.mat";
//	ConstLow::matInFileName = "input/130508_Flt1_of_1_775_1600secs.mat";
//	ConstLow::matInFileName = "input/8thMay201376001to77000pages.mat";
//	ConstLow::matInFileName = "input/MultiStagger_8thMay_PUP1.mat";
//	ConstLow::matInFileName = "input/Agile_agile3.mat";
//	ConstLow::matInFileName = "input/GroundTest_49001to50000pages.mat";
//	ConstLow::matInFileName = "input/GroundTest_50001to60000pages.mat";
//	ConstLow::matInFileName = "input/MultipleDAsFMRTest_AllBands_126001to127000pages.mat";
//	ConstLow::matInFileName = "input/Jan13Filtered.mat";
//	ConstLow::matInFileName = "input/Jan13Filtered2.mat";
//	ConstLow::matInFileName = "input/PulseToPulseAgile.mat";
//	ConstLow::matInFileName = "input/pulse2pulseAgileFull.mat";
//	ConstLow::matInFileName = "input/TrackingPulse2pulseAgileFull.mat";
//	ConstLow::matInFileName =
//			"input/130719_Flt2of3_IBand_10F11_1578_1875secs.mat";
//	ConstLow::matInFileName = "input/130720_Flight2_AllBands_Chunk_71_80k.mat";

//	ConstLow::matInFileName = "input/PridifftoaJan13.mat";
//	ConstLow::matInFileName = "input/jan13_8msPri.mat";
//	ConstLow::matInFileName = "input/jitter_jan13_p2_PUP.mat";
//	ConstLow::matInFileName = "input/jitter_jan13_p4_PUP.mat";
//	ConstLow::matInFileName = "input/15levelstag_may.mat";
//	ConstLow::matInFileName = "input/775to1600RT_1.mat";
//	ConstLow::matInFileName = "input/775_1600BandE.mat";
//	ConstLow::matInFileName = "input/775to1600RT_2_975to1150.mat";
//	ConstLow::matInFileName = "input/775to1600RT_3_E4DWS83.mat";
//	ConstLow::matInFileName = "input/JanFiltered2Emit_BandG.mat";
//	ConstLow::matInFileName = "input/775to1600RT_bandG.mat";
//	ConstLow::matInFileName = "input/lowblowscntrack_may9.mat";
//	ConstLow::matInFileName = "input/Spada_775_1600_Band1_mode2.mat";
//	ConstLow::matInFileName = "input/130AS1Part1_21_25k.mat";
//	ConstLow::matInFileName = "input/RT1_130AS1_2E.mat";
//	ConstLow::matInFileName = "input/RT2_130AS1_2E.mat";

//	ConstLow::matInFileName = "input/130508_527_544_CompleteData.mat";
//	ConstLow::matInFileName = "input/130508_527_544_Data_EG.mat";
//	ConstLow::matInFileName = "input/SA_3_powAgile.mat";
//	ConstLow::matInFileName = "input/TrackingPulse2pulseAgileFull.mat";

//	ConstLow::matInFileName = "input/RF_Jump_4.mat";
//	ConstLow::matInFileName = "input/02_Frec_agile_1.mat"; //
//	ConstLow::matInFileName = "input/12_Pri_jitter_1.bin";
////	ConstLow::matInFileName = "input/RF_Switch_1.mat";
//	ConstLow::matInFileName = "input/test2Filtered500.mat";
//	ConstLow::matInFileName = "input/livePdws_sPDWs.mat";

//	ConstLow::matInFileName = "input/MultipleDAsFMRTest_AllBands_103001to104000pages.mat";
//	ConstLow::matInFileName = "input/TestingDataforReview1to1000pages_dwell.mat";
//	ConstLow::matInFileName = "input/TestingDataforReview38001to39000pages_dwellFixed.mat";
//	ConstLow::matInFileName ="input/Flight0_Complete.mat";
}

void simLowSetVariables()
{
	//we start off with each iteration time equal to maximum desired
	Model::simTimeMicrosecIterationActual = ConstLow::simIterationFixedTime;
}

