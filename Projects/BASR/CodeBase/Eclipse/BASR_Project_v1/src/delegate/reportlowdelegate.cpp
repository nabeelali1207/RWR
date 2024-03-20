/*
 * reportlowdelegate.cpp
 *
 *  Created on: Oct 11, 2019
 *      Author: gmd
 */

#define REPORTLOWDELEGATE_CPP_

#include "../delegate/reportlowdelegate.h"

void printEmitters(DataAnalysisClass &analysisObj, uint64_t currSystemIteration)
{
//	if ((currSystemIteration % 10) != 0)
//	{
//		return;
//	}
	if (analysisObj.bandEJ.tracks.size() > 0
			|| analysisObj.bandCD.tracks.size() > 0
			|| analysisObj.bandK.tracks.size() > 0)
	{
		cout << "Emitters\n";
		cout
				<< "ToaFirst\tToaLast\t\tBand\tAngle\tPulsCnt\tPRIType\tPRICnt\tPRI\tPW\tRF\tVdmAmp\tRfAmp\tSTFlag\tLpiType\n";
	}
	for (uint16_t i = 0; i < analysisObj.bandEJ.tracks.size(); i++)
	{
		TrackDefaultClass &track = analysisObj.bandEJ.tracks[i];
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
		switch(track.intrapulse.trackLpiType)
		{
		case IntraPulseType::UMOP:
			cout << "UMOP\t";
			break;
		case IntraPulseType::AMOP:
			cout << "AMOP\t";
			break;
		case IntraPulseType::FMOP:
			cout << "FMOP\t";
			break;
		case IntraPulseType::PMOP:
			cout << "PMOP\t";
			break;
		}

//		cout << (int) track.power.quadrantCodeLast << "\t";
		cout << "\n";

	}
	for (uint16_t i = 0; i < analysisObj.bandCD.tracks.size(); i++)
	{
		TrackDefaultClass &track = analysisObj.bandCD.tracks[i];
		cout << std::setprecision(12)
				<< ((double) track.general.toaFirst) / (double) 1.0e8 << "\t";
		cout << std::setprecision(12)
				<< ((double) track.general.toaLast) / (double) 1.0e8 << "\t";

		cout << "CD" << (int) i << "\t";
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
		switch(track.intrapulse.trackLpiType)
		{
		case IntraPulseType::UMOP:
			cout << "UMOP\t";
			break;
		case IntraPulseType::AMOP:
			cout << "AMOP\t";
			break;
		case IntraPulseType::FMOP:
			cout << "FMOP\t";
			break;
		case IntraPulseType::PMOP:
			cout << "PMOP\t";
			break;
		}
		cout << "\n";

	}

	for (uint16_t i = 0; i < analysisObj.bandK.tracks.size(); i++)
	{
		TrackDefaultClass &track = analysisObj.bandK.tracks[i];
		cout << std::setprecision(12)
				<< ((double) track.general.toaFirst) / (double) 1.0e8 << "\t";
		cout << std::setprecision(12)
				<< ((double) track.general.toaLast) / (double) 1.0e8 << "\t";

		cout << "K" << (int) i << "\t";
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
		switch(track.intrapulse.trackLpiType)
		{
		case IntraPulseType::UMOP:
			cout << "UMOP\t";
			break;
		case IntraPulseType::AMOP:
			cout << "AMOP\t";
			break;
		case IntraPulseType::FMOP:
			cout << "FMOP\t";
			break;
		case IntraPulseType::PMOP:
			cout << "PMOP\t";
			break;
		}
		cout << "\n";

	}
	if (analysisObj.bandEJ.tracks.size() > 0
			|| analysisObj.bandCD.tracks.size() > 0
			|| analysisObj.bandK.tracks.size() > 0)
	{
		cout << endl;
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
void reportAnalysisObjectToCpu(DataAnalysisClass &analysisObj,
		DataSimulationLowClass &simLowData, uint64_t currSystemIteration)
{

	uint64_t currSystemTime = Utility::getSystemTime();
	uint64_t timeSinceLastReport = 0;

	if (ConstLow::simIterationTypeFlag
			== LowIterationTypeFlags::fixedTimeIterationFastSimulation)
	{
		uint64_t iterationDiff = currSystemIteration
				- analysisObj.lastReportingSystemIter;
		timeSinceLastReport = iterationDiff * ConstLow::simIterationFixedTime
				* 1000;
	}
	else if (ConstLow::simIterationTypeFlag
			== LowIterationTypeFlags::readIterationTimeFromFile)
	{
		if (simLowData.callReportingFlag == 1)
			timeSinceLastReport = 200000;
		else
			timeSinceLastReport = 0;
	}
	else
	{
		timeSinceLastReport = currSystemTime - analysisObj.lastReportingTime;
	}

	if (timeSinceLastReport >= 230 * 1000)
	{
//		if(Model::analysisLow.diagnosticCommandLow.ack ==1)
//		{
//			cout<< "Sending ST-Ack from Low" <<endl;
//		}
		analysisObj.ethernetThreats = 0;
		analysisObj.failureIDs = 0;
		if(Model::ethernetlinktp.checkPacketsAvailableOnEthernetLink(ConstLow::readThreatDataFromEthernet ==1) > 1)
		{
			Model::ethernetlinktp.ethernetPacketsRead(ConstLow::readThreatDataFromEthernet ==1);

			ethernetTracksConversion(analysisObj.bandEJ.tracks,analysisObj.bandAll.tracks,
						analysisObj.bandK.tracks,analysisObj.bandCD.tracks,Model::ethernetlinktp);
			analysisObj.ethernetThreats = 1;
			analysisObj.failureIDs  = Model::ethernetlinktp.threatMsgs->threatHeader.faultInjection;
//			cout << "Failures : " << analysisObj.failureIDs << endl;
		}


		analysisObj.lastReportingTime = currSystemTime;
		analysisObj.lastReportingSystemIter = currSystemIteration;

		trackUpdateParametersBeforeReporting(analysisObj.bandEJ.tracks,
				Model::countThreadMainIterLow);
		trackUpdateParametersBeforeReporting(analysisObj.bandCD.tracks,
				Model::countThreadMainIterLow);
//		analysisRfSpurRejection(analysisObj.bandEJ.tracks);
		trackUpdateParametersBeforeReporting(analysisObj.bandK.tracks,
				Model::countThreadMainIterLow);

		bool statusFlag = false;

//		Model::analysisLow.diagnosticCommandLow.ack = 0;
//		delegateDfSelfTestLow();

		analysisObj.RSSCRC = 0x12abcdef;
		statusFlag = Model::classObjectSerializer.serializeClass(analysisObj,
				"THTDATA");

		if (Model::classObjectSerializer.getSerializedMessageSize() > 8000)
		{
			if (analysisObj.bandEJ.tracks.size() > 30)
			{
				trackDeleteTracksWithNoPri(analysisObj.bandEJ.tracks);
			}

			statusFlag = Model::classObjectSerializer.serializeClass(
					analysisObj, "THTDATA");

			if (Model::classObjectSerializer.getSerializedMessageSize() > 16000)
			{
				if (analysisObj.bandEJ.tracks.size() > 30)
				{
					analysisObj.bandEJ.tracks.resize(30);
				}
				statusFlag = Model::classObjectSerializer.serializeClass(
						analysisObj, "THTDATA");
			}

		}
		if(ConstLow::logPrintEmitters==true)
		{
			printEmitters(analysisObj, currSystemIteration);
		}
		jsonReportBasrLowTracks();
		LogLowClass::writeReportCsv(analysisObj.bandEJ.tracks,
				ConstLow::csvOutFileName);
		LogLowClass::writeReportCsv(analysisObj.bandCD.tracks,
				ConstLow::csvOutFileName);
		LogLowClass::writeReportCsv(analysisObj.bandK.tracks,
				ConstLow::csvOutFileName);
		bool reportStatus = false;
		if (statusFlag == true)
		{
			reportStatus = Model::dspIoHandle.sendMessageToCPU(
					Model::classObjectSerializer.getSerializedMessagePointer(),
					Model::classObjectSerializer.getSerializedMessageSize());
//			bool reportStatus = Model::dspIoHandle.sendMessageToCPU(
//					Model::classObjectSerializer.getSerializedMessagePointer(),
//					max(Model::classObjectSerializer.getSerializedMessageSize(),(uint32_t)1024*16));
//			if (reportStatus == false)
//			{
//				cout << "Message Sending Failed! Size: "
//						<< Model::classObjectSerializer.getSerializedMessageSize()
//						<< " Track count (E,CD,K) : ("
//						<< analysisObj.bandEJ.tracks.size() << ","
//						<< analysisObj.bandCD.tracks.size() << ","
//						<< analysisObj.bandK.tracks.size() << ")" << endl;
//			}
		}
		else
		{
			cout << "Class Serialization Failed! Track count (EJ) : "
					<< analysisObj.bandEJ.tracks.size() << endl;
		}

		trackUpdateParametersAfterReporting(analysisObj.bandEJ.tracks,
				Model::countThreadMainIterLow,reportStatus);
		trackUpdateParametersAfterReporting(analysisObj.bandCD.tracks,
				Model::countThreadMainIterLow,reportStatus);
		trackUpdateParametersAfterReporting(analysisObj.bandK.tracks,
				Model::countThreadMainIterLow,reportStatus);

	}

}
