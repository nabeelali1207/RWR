/*
 * threadmainhighdelegate.cpp
 *
 *  Created on: Sep 26, 2019
 *      Author: hassan
 */

#define THREADMAINHIGHDELEGATE_CPP_

#include "threadmainhighdelegate.h"
#include "BasrTrackJsonConversionDelegate.h"

void* threadMainHighDelegate(void*)
{
	uint64_t threadMainHighDelegateLastTime, threadMainHighDelegateCurrentTime;
	Word32 timeDifference = 0;
	//all the couts of the code are redirected to a file in /output
	redirectStandardOutput();

	//a counter to count itration number to help in debugging
	initThreadMainIterCountHigh(Model::countThreadMainIterHigh);

	cfdUnitStatusCheck(1);
	cfdSendAutoPrograms(Model::cfdLibrary);

	//these 2 timers are used to calculate 220 msecs difference between 2 Tracking
	//iteration i.e. in case TP is not UP and we are running on normal iterations
	//so there is a difference of 220msecs between each iteration
	threadMainHighDelegateLastTime = Utility::getSystemTime();
	threadMainHighDelegateCurrentTime = Utility::getSystemTime();

	uint64_t lastLogTime = Utility::getSystemTime();
	uint64_t maxIterationTime = 0;
	uint64_t minIterationTime = (uint64_t) -1;
	uint64_t thisIterationTime, currTime = lastLogTime, lastTime = lastLogTime;
	uint64_t maxTrackCountEJ, maxTrackCountCD, maxTrackCountK, maxTrackCountCW;
	maxTrackCountEJ = maxTrackCountCD = maxTrackCountK = maxTrackCountCW = 0;

	//SPI option is the option or mode selected by Trenz to operate on
	Model::trenzSpiLink.spiOption = 10;
	if (ConstHigh::simOnActualHwFlag == 1)
	{
		//Built In Test is the basic self test protocol where each and every
		//board and communication protocol is tested. It is also done
		//at the start of operation to check for any discrepancies at
		//start of program

		Model::stDelegate.selfTestType = 3;
		Model::stDelegate.selfTestActiveFlag = 1;
		Model::stDelegate.selfTestBuiltIn();
	}
	while (1)
	{
		if (ConstHigh::simOnActualHwFlag == 1)
		{
			//watchdog is disabled for debugging purposes
			//enable it while running it in normal mode
			Model::watchdogHigh.resetWatchdogHigh();

			//zeroize signal comes from VDM through PCIe
			//on zeroize signal, we clear all Keys from CF Card
			Model::pcieThreatProcessor.checkZeroizeSignal(Model::cfCard);

			//All the communication between Trenz and CCP is done using
			//SPI link, this function gets the commands from Trenz and
			//takes actions on it
			Model::trenzSpiLink.interProcessorCommunicationLogic();
//			if (ConstHigh::cfdInit == 0)
//			{
//				cfdUnitStatusCheck(1);
//			}
			cfdPeriodicStatusCheck();
			cfdPeriodicBitRequest();
		}

		bool reportRec = Model::pcieThreatProcessor.getThreatProcessorReport(
				Model::classObjectSerializer.getSerializedMessagePointer());
		//checking if SPI option is 10 or Function Self Test is currently
		//running so that tracking and other logic may be done on threats
		if ((Model::trenzSpiLink.spiOption == 10)
				|| (ConstHigh::FstInProgress == 1))
		{

			threadMainHighDelegateCurrentTime = Utility::getSystemTime();

			timeDifference = (threadMainHighDelegateCurrentTime
					- threadMainHighDelegateLastTime) / 1000;
			//checking if TP is up OR its been 230 msecs between the previous
			//iteration and this one
			if ((reportRec) || (timeDifference > 300))
			{
				//updating the timer for next iteration
				threadMainHighDelegateLastTime = Utility::getSystemTime();
				//checking if TP is up and ready to send reports through PCIe
				Model::pcieThreatProcessor.checkTpUp(
						Model::threatProcessorBandSwitchingTimeClass.sendBandSwitchDataToTp);

				//It is CF Card main logic, it checks if CF card is present, if yes then
				//it mounts it. If CF Card is not present, it resets CF Card Controller every
				//5 seconds so that CF Card maintains Hot Swapibility functionality
				//For Hot Swap CF Card, the controllers power should not be external though
				Model::cfCard.mainProcessingCfCardCheckLogic();
				ConstHigh::flightRecordPathCurrent = "/tmp/"
						+ string((char*) Model::cfCard.cfCardName) + "/";
				//write low flight record

				//clearing the Flight Record structure that was filled in previous iteration
				initFlightRecordStructure();

				//Reads TP report containing Pulsed Threats
				//Report is stored in pointer returned by
				//Model::classObjectSerializer.getSerializedMessagePointer()
				bool reportValid = false;
				if (reportRec == 1)
				{
					//Here TP Report is being serialized into its Class
					reportValid = Model::classObjectSerializer.deSerializeClass(
							Model::analysisLow, "THTDATA", 204800);
//					printf("EJ-R: %d \t", Model::analysisLow.bandEJ.tracks.size());
//					printf("CD-R: %d \t", Model::analysisLow.bandCD.tracks.size());
//					printf("K-R: %d \n", Model::analysisLow.bandK.tracks.size());
				}

				if (reportValid && Model::analysisLow.ethernetThreats == 1)
				{
					Model::analysisCwpHistory[0].bandAll =
							Model::analysisLow.bandAll;
					Model::analysisHigh.ethernetThreats =
							Model::analysisLow.ethernetThreats;
					Model::analysisHigh.failureIDs =
							Model::analysisLow.failureIDs;
//					Model::analysisLow.bandAll.tracks.clear();
				}

				fillPacketInfofindTpSelfTestEmitters(Model::stDelegate,
						Model::analysisLow.bandEJ);
				//TP reports are copied to Circular Buffer here
				reportInsertCircularBuffer(Model::analysisLow,
						Model::analysisHistory, reportValid);

				//Here all the CW processing gets done.
				//CW reports are recieved, extracted, copied, matched and
				//processed here
				reportCwCompleteIterationHigh(
						Model::analysisLow.ethernetThreats,
						Model::analysisCwpHistory[0], Model::analysisCwp,
						Model::library, Model::cfdLibrary);

				//Band switching time is sent to TP when TP asks for it
				Model::threatProcessorBandSwitchingTimeClass.sendAntennaSwitchingTimeStructureToThreatProcessorLogic();

				threadCompleteIterationHigh(Model::analysisHistory[0].bandEJ,
						Model::analysisHistory[1].bandEJ,
						Model::analysisHigh.bandEJ);
				threadCompleteIterationHigh(Model::analysisHistory[0].bandCD,
						Model::analysisHistory[1].bandCD,
						Model::analysisHigh.bandCD);
				threadCompleteIterationHigh(Model::analysisHistory[0].bandK,
						Model::analysisHistory[1].bandK,
						Model::analysisHigh.bandK);
//			logWriteLowTracks(Model::analysisHistory[0].bandEJ.tracks,
//					Model::analysisHistory[0].bandCD.tracks,
//					Model::analysisHistory[0].bandK.tracks,
//					Model::analysisCwp.bandAll.tracks, &Model::library,
//					&Model::libraryMeta, ConstHigh::flightRecordPathCurrent);
				logWriteHighTracks(Model::analysisHigh.bandEJ.tracks,
						Model::analysisHigh.bandCD.tracks,
						Model::analysisHigh.bandK.tracks,
						Model::analysisCwp.bandAll.tracks, &Model::library,
						&Model::libraryMeta,
						ConstHigh::flightRecordPathCurrent);

				jsonReportBasrHighTracks();

				printHighEmitters(Model::analysisHigh, 1);
				printHighCwEmitters(Model::analysisCwp.bandAll.tracks);
				sendTrackingDataToSpi(ConstHigh::FstInProgress,
						ConstHigh::spiAudioRequest,
						Model::analysisHigh.bandK.tracks,
						Model::analysisCwp.bandAll.tracks,
						Model::analysisHigh.bandEJ.tracks,
						Model::analysisHigh.bandCD.tracks, Model::trenzSpiLink,
						Model::library, ConstHigh::sopMissileLaunch,
						ConstHigh::sopThreatFound, ConstHigh::sopBeingTracked,
						Model::pcieThreatProcessor.tpUp);

				// send threat message to CFD here
				cfdFillThreatData(Model::analysisHigh.bandEJ.tracks,
						Model::analysisHigh.bandCD.tracks,
						Model::analysisHigh.bandK.tracks,
						Model::analysisCwp.bandAll.tracks);

				Model::stDelegate.selfTestHandleFstMfl();

				Model::stDelegate.selfTestPeriodic(
						Model::analysisHigh.ethernetThreats);
				Model::stDelegate.selfTestSynthetic(
						Model::analysisHigh.failureIDs);

				checkAudioAsyncSopNeeded();

				Model::fileoutputflightrecord.flightRecordAircraftNavigationInformationWritten =
						0;

				incrementThreadMainIterHigh(Model::countThreadMainIterHigh);
				Model::analysisLow.bandEJ.tracks.resize(0);
				Model::analysisLow.bandK.tracks.resize(0);
				Model::analysisLow.bandCD.tracks.resize(0);
				Model::analysisCwpHistory[0].bandAll.tracks.resize(0);
				Model::analysisHigh.ethernetThreats = 0;
				Model::analysisHigh.failureIDs = 0;
				//technically last report time will be updated even when no reports
				//were received in the last iteration
			}
		}
		if (Model::trenzSpiLink.spiOption == 12)
		{
			startInitiatedBuiltInSelfTestProcess();
		}
		else if (Model::trenzSpiLink.spiOption == 13)
		{
			startMaintainanceBuiltInSelfTestProcess();
		}
		else if (Model::trenzSpiLink.spiOption == 16)
		{
			Model::cfCard.copyAndDecryptLogic();
			libraryLoading(Model::library, Model::libraryMeta,
					Model::cfdLibrary, Model::cfdLibraryMeta, Model::cfCard);
			cfdSendAutoPrograms(Model::cfdLibrary);
			maintOut.loadStatus.status = 0;
			fillEwm1MaintBuffer(&maintOut, 5);
			Model::trenzSpiLink.writeReplyToTrenz(3, 29, 0);
			Model::trenzSpiLink.spiOption = -1;
		}
		if (ConstHigh::simOnActualHwFlag == 1)
		{
			if (ConstHigh::sendShutdown == 1)
			{
				Model::trenzSpiLink.writeReplyToTrenz(20, 0, 0);
				ConstHigh::sendShutdown = 0;
			}
		}

		lastTime = currTime;
		currTime = Utility::getSystemTime();
		thisIterationTime = (currTime > lastTime ? currTime - lastTime : 1);
		minIterationTime = min(minIterationTime, thisIterationTime);
		maxIterationTime = max(maxIterationTime, thisIterationTime);

		maxTrackCountEJ = (
				maxTrackCountEJ > Model::analysisHigh.bandEJ.tracks.size() ?
						maxTrackCountEJ :
						Model::analysisHigh.bandEJ.tracks.size());
		maxTrackCountCD = (
				maxTrackCountCD > Model::analysisHigh.bandCD.tracks.size() ?
						maxTrackCountCD :
						Model::analysisHigh.bandCD.tracks.size());
		maxTrackCountK =
				(maxTrackCountK > Model::analysisHigh.bandI.tracks.size() ?
						maxTrackCountK : Model::analysisHigh.bandI.tracks.size());
		maxTrackCountCW = (
				maxTrackCountCW > Model::analysisCwp.bandAll.tracks.size() ?
						maxTrackCountCW :
						Model::analysisCwp.bandAll.tracks.size());

		if ((currTime - lastLogTime) > 10000000)
		{
			cout << "Systime: ";
			Utility::printTime();
			cout << " Iter: " << Model::countThreadMainIterHigh;

			cout << " Response Time(min,max): (" << minIterationTime << ","
					<< maxIterationTime << ")\n";
			cout << "Max Track Count(EJ,CD,K,Cw): (" << maxTrackCountEJ << " , "
					<< maxTrackCountCD << " , " << maxTrackCountK << " , "
					<< maxTrackCountCW << ")";
			cout << endl;

			maxTrackCountEJ = maxTrackCountCD = maxTrackCountK =
					maxTrackCountCW = 0;
			minIterationTime = (uint64_t) -1;
			maxIterationTime = 0;
			lastLogTime = currTime;
		}
	}
	restoreStandardOutput();
	return NULL;
}

void* threadMainHighDelegateSimulator(void*)
{
	//bool dspStatus = 0;
	uint8_t dataFinishedFlag = 0;
	uint64_t lastReportTime = Utility::getSystemTime();
	//redirectStandardOutput();
	initThreadMainIterCountHigh(Model::countThreadMainIterHigh);


	uint64_t lastLogTime = Utility::getSystemTime();
	uint64_t maxIterationTime = 0;
	uint64_t minIterationTime = (uint64_t) -1;
	uint64_t thisIterationTime, currTime = lastLogTime, lastTime = lastLogTime;

	uint64_t maxTrackCountEJ, maxTrackCountCD, maxTrackCountK, maxTrackCountCW;
	maxTrackCountEJ = maxTrackCountCD = maxTrackCountK = maxTrackCountCW = 0;

	Model::cpuIoHandle.initializeIoHandler(true,false);
	while (dataFinishedFlag == 0)
	{

		bool reportAvailableFlag = reportReadFromDSP(Model::analysisLow,
				dataFinishedFlag);

		temporaryProcessing();

		if ((reportAvailableFlag == true)
				|| ((Utility::getSystemTime() - lastReportTime) > 250000)
				|| (dataFinishedFlag == 1))
		{

			//maintain dsp data one iteration behind
			reportInsertCircularBuffer(Model::analysisLow,
					Model::analysisHistory, reportAvailableFlag);

			threadCompleteIterationHigh(Model::analysisHistory[0].bandEJ,
					Model::analysisHistory[1].bandEJ,
					Model::analysisHigh.bandEJ);
			threadCompleteIterationHigh(Model::analysisHistory[0].bandCD,
					Model::analysisHistory[1].bandCD,
					Model::analysisHigh.bandCD);
			threadCompleteIterationHigh(Model::analysisHistory[0].bandK,
					Model::analysisHistory[1].bandK, Model::analysisHigh.bandK);

			initFlightRecordStructure();
			//					logWriteLowTracks(Model::analysisHistory[0].bandEJ.tracks,
			//					Model::analysisHistory[0].bandCD.tracks,
			//					Model::analysisHistory[0].bandK.tracks,
			//					Model::analysisCwp.bandAll.tracks, &Model::library,
			//					&Model::libraryMeta, ConstHigh::flightRecordPathCurrent);
			logWriteHighTracks(Model::analysisHigh.bandEJ.tracks,
					Model::analysisHigh.bandCD.tracks,
					Model::analysisHigh.bandK.tracks,
					Model::analysisCwp.bandAll.tracks, &Model::library,
					&Model::libraryMeta, ConstHigh::flightRecordPathCurrent);

			jsonReportBasrHighTracks();

			printHighEmitters(Model::analysisHigh, 1);
			printHighCwEmitters(Model::analysisCwp.bandAll.tracks);
			//			logWriteLowTracks(Model::analysisHistory[0].bandEJ.tracks,
			//					Model::analysisHistory[0].bandCD.tracks,
			//					Model::analysisHistory[0].bandK.tracks,
			//					Model::analysisCwp.bandAll.tracks, &Model::library,
			//					&Model::libraryMeta, ConstHigh::flightRecordPathCurrent);
			logWriteHighTracks(Model::analysisHigh.bandEJ.tracks,
					Model::analysisHigh.bandCD.tracks,
					Model::analysisHigh.bandK.tracks,
					Model::analysisCwp.bandAll.tracks, &Model::library,
					&Model::libraryMeta, ConstHigh::flightRecordPathCurrent);

			Model::fileoutputflightrecord.flightRecordAircraftNavigationInformationWritten =
					0;

			incrementThreadMainIterHigh(Model::countThreadMainIterHigh);
			Model::analysisLow.bandEJ.tracks.resize(0);
			Model::analysisLow.bandK.tracks.resize(0);
			Model::analysisLow.bandCD.tracks.resize(0);
			Model::analysisCwpHistory[0].bandAll.tracks.resize(0);
			Model::analysisHigh.ethernetThreats = 0;
			Model::analysisHigh.failureIDs = 0;
		}

		//status logs
		lastTime = currTime;
		currTime = Utility::getSystemTime();
		thisIterationTime = (currTime > lastTime ? currTime - lastTime : 1);
		minIterationTime = min(minIterationTime, thisIterationTime);
		maxIterationTime = max(maxIterationTime, thisIterationTime);

		maxTrackCountEJ = (
				maxTrackCountEJ > Model::analysisHigh.bandEJ.tracks.size() ?
						maxTrackCountEJ :
						Model::analysisHigh.bandEJ.tracks.size());
		maxTrackCountCD = (
				maxTrackCountCD > Model::analysisHigh.bandCD.tracks.size() ?
						maxTrackCountCD :
						Model::analysisHigh.bandCD.tracks.size());
		maxTrackCountK =
				(maxTrackCountK > Model::analysisHigh.bandI.tracks.size() ?
						maxTrackCountK : Model::analysisHigh.bandI.tracks.size());
		maxTrackCountCW = (
				maxTrackCountCW > Model::analysisCwp.bandAll.tracks.size() ?
						maxTrackCountCW :
						Model::analysisCwp.bandAll.tracks.size());

		if ((currTime - lastLogTime) > 10000000)
		{
			cout << "Systime: ";
			Utility::printTime();
			cout << " Iter: " << Model::countThreadMainIterHigh;

			cout << " Response Time(min,max): (" << minIterationTime << ","
					<< maxIterationTime << ")\n";
			cout << "Max Track Count(EJ,CD,K,Cw): (" << maxTrackCountEJ << " , "
					<< maxTrackCountCD << " , " << maxTrackCountK << " , "
					<< maxTrackCountCW << ")";
			cout << endl;

			maxTrackCountEJ = maxTrackCountCD = maxTrackCountK =
					maxTrackCountCW = 0;
			minIterationTime = (uint64_t) -1;
			maxIterationTime = 0;
			lastLogTime = currTime;
		}

	}
	restoreStandardOutput();
	return NULL;
}
void temporaryProcessing()
{
	uint16_t i = 0;
	circular_buffer<LimitRfClass>::iterator tIter;

	//this is temporary, band code must be fixed at low side
	/* update 15.02.2021 removing this since band code has been fixed at low side*/

	/*for(i = 0; i < Model::analysisLow.bandE.tracks.size(); i++) {
	 Model::analysisLow.bandE.tracks[i].rf.bandCode = BandCodeEnum::band_e;
	 }
	 for(i = 0; i < Model::analysisLow.bandG.tracks.size(); i++) {
	 Model::analysisLow.bandG.tracks[i].rf.bandCode = BandCodeEnum::band_g;
	 }
	 for(i = 0; i < Model::analysisLow.bandI.tracks.size(); i++) {
	 Model::analysisLow.bandI.tracks[i].rf.bandCode = BandCodeEnum::band_i;
	 }
	 for(i = 0; i < Model::analysisLow.bandJ.tracks.size(); i++) {
	 Model::analysisLow.bandJ.tracks[i].rf.bandCode = BandCodeEnum::band_j;
	 }*/

// removing if invalid RF spot was recieved
	for (i = 0; i < Model::analysisLow.bandEJ.tracks.size(); i++)
	{
		for (tIter = Model::analysisLow.bandEJ.tracks[i].rf.rfCircular.begin();
				tIter != Model::analysisLow.bandEJ.tracks[i].rf.rfCircular.end();
				)
		{
			//tIter->rfType = RfTypeEnum::rf_switch;
			if (tIter->rfLow == 0)
			{
				tIter = Model::analysisLow.bandEJ.tracks[i].rf.rfCircular.erase(
						tIter);
			}
			else
			{
				tIter++;
			}
		}
	}
	for (i = 0; i < Model::analysisLow.bandCD.tracks.size(); i++)
	{
		for (tIter = Model::analysisLow.bandCD.tracks[i].rf.rfCircular.begin();
				tIter != Model::analysisLow.bandCD.tracks[i].rf.rfCircular.end();
				)
		{
			//tIter->rfType = RfTypeEnum::rf_switch;
			if (tIter->rfLow == 0)
			{
				tIter = Model::analysisLow.bandCD.tracks[i].rf.rfCircular.erase(
						tIter);
			}
			else
			{
				tIter++;
			}
		}
	}
	for (i = 0; i < Model::analysisLow.bandK.tracks.size(); i++)
	{
		for (tIter = Model::analysisLow.bandK.tracks[i].rf.rfCircular.begin();
				tIter != Model::analysisLow.bandK.tracks[i].rf.rfCircular.end();
				)
		{
			//tIter->rfType = RfTypeEnum::rf_switch;
			if (tIter->rfLow == 0)
			{
				tIter = Model::analysisLow.bandK.tracks[i].rf.rfCircular.erase(
						tIter);
			}
			else
			{
				tIter++;
			}
		}
	}
}

void incrementThreadMainIterHigh(uint64_t &countThreadMainIteration)
{
	if (ConstHigh::simLogIterationHigh == 1)
	{
//		cout << "L-Track count E: "
//				<< Model::analysisHistory[0].bandEJ.tracks.size() << " G: "
//				<< Model::analysisHistory[0].bandG.tracks.size() << " I: "
//				<< Model::analysisHistory[0].bandI.tracks.size() << " J: "
//				<< Model::analysisHistory[0].bandJ.tracks.size() << endl;
//		cout << "H-Track count E: " << Model::analysisHigh.bandEJ.tracks.size()
//				<< " G: " << Model::analysisHigh.bandG.tracks.size() << " I: "
//				<< Model::analysisHigh.bandI.tracks.size() << " J: "
//				<< Model::analysisHigh.bandJ.tracks.size() << endl;
		cout << countThreadMainIteration + 1 << "------------------" << endl;
	}
	countThreadMainIteration++;
}

void initThreadMainIterCountHigh(uint64_t &countThreadMainIteration)
{
	countThreadMainIteration = 0;
	if (ConstHigh::simLogIterationHigh == 1)
	{
		cout << countThreadMainIteration << "------------------" << endl;
	}
	//freopen("output/debugHigh.txt","w",stdout);
	//streambuf* stream_buffer_cin = cin.rdbuf();
}

void redirectStandardOutput()
{
	if (ConstHigh::simLogIterationHigh == 1)
	{
		Model::stdoutFile.open("output/debugHigh.txt", ios::out);
		Model::stream_buffer_cout = cout.rdbuf();
		Model::stream_buffer_file = Model::stdoutFile.rdbuf();
		cout.rdbuf(Model::stream_buffer_file);
	}
}

void restoreStandardOutput()
{
	if (ConstHigh::simLogIterationHigh == 1)
	{
		cout.rdbuf(Model::stream_buffer_cout);
		Model::stdoutFile.close();
	}
}
