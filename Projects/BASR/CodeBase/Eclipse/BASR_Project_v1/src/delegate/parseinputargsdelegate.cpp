/*
 * parseinputargsdelegate.cpp
 *
 *  Created on: Sep 28, 2022
 *      Author: gmd
 */

#include "parseinputargsdelegate.h"

void setSimulationOnPcConstants()
{ // this is read pdws from file mode, with cpu tracks from file
	ConstLow::watchDogEnabledFlag = 0;
//	ConstLow::matInFileName = "input/Pdws_Flight-178_00M_To_20M.mat";
//	ConstLow::matInFileName = "input/Pdws_Flight47_05M_To_10M.mat";
//	ConstLow::matInFileName = "input/Pdws_CompassFlight47_15M_To_30M.mat";
	//ConstLow::matInFileName = "input/RF_Jump_4.mat";
	ConstLow::matInFileName = "input/62_PRI_Complex_1.bin";
//	ConstLow::matInFileName = "input/scan_circular_PUP.mat";
//	ConstLow::matInFileName = "input/SectorScanRT.mat";
//	ConstLow::matInFileName = "input/LockonJan13RT.mat";
//    ConstLow::matInFileName = "input/TWS10Aug22RT.mat"; //TWS6sec_PUP
//    ConstLow::matInFileName = "input/TWS_PUP.mat";
//    ConstLow::matInFileName = "input/TWS6sec_PUP.mat";
	//ConstLow::matInFileName = "input/TWSRT.mat";
//	ConstLow::matInFileName = "input/TWSWithScanRT.mat";
//	ConstLow::matInFileName = "input/Conical_PUP.mat";
//	ConstLow::matInFileName = "input/PRI_DnS_1_PUP.mat";
//	ConstLow::matInFileName = "input/scanCircular_PUP.mat";
//	ConstLow::matInFileName = "input/sector_PUP.mat";

	ConstLow::simIterationTypeFlag =
			LowIterationTypeFlags::fixedTimeIterationFastSimulation;

	ConstLow::logRecordLivePdws = false;
	ConstLow::logPrintEmitters = true;
	ConstLow::logDspReportFlag = true;
	ConstLow::logDspCsvReportFlag = true;
	ConstLow::logHexFileTimeBenchmarks = false;
	ConstLow::logHexFileClusteringData = false;
	ConstLow::logHexFilePriData = false;
	ConstLow::logHexFileTrackData = false;
	ConstLow::logHexFileIterationInfo = false;
	ConstLow::logHexFile = false;

	ConstLow::simTimeLimitFlag = 0;
	ConstLow::simTimeStartToa = 900;
	ConstLow::simTimeEndToa = 1400;

	ConstLow::simTimeStartToa *= 100000000;
	ConstLow::simTimeEndToa *= 100000000;

	ConstHigh::simOnActualHwFlag = 0;
	ConstHigh::simTimeSyntheticFlag = 1;
	Model::cfCard.foundCfFlag = 1;
}
void setSimulationOnHwConstants()
{ // this is read pdws from file mode, with cpu tracks from dsp

	ConstLow::watchDogEnabledFlag = 0;
	//ConstLow::matInFileName = "input/RF_Agile_1_PUP.mat";

	ConstLow::simIterationTypeFlag =
			LowIterationTypeFlags::fixedTimeIterationFastSimulation;

	ConstLow::logPrintEmitters = true;
	ConstLow::logDspReportFlag = true;
	ConstLow::logDspCsvReportFlag = true;
}
void setLiveModeConstants()
{ // this is live mode with no debug logs

	ConstLow::watchDogEnabledFlag = 0;

	ConstLow::simIterationTypeFlag =
			LowIterationTypeFlags::simulationModeDisabled;

	ConstHigh::simTimeSyntheticFlag = 0;

}
bool setupSystemConstantsFromCommandLine(int argumentCount,
		char **argumentValue)
{

	ConstLow::asClusterMaxTimeWindowMillis = 500;
	ConstLow::asClusterMinInput = 16;

	bool parametersCorrectFlag = true;

	if (argumentCount == 1)
	{
		// code is being run directly, so no need to setup
		Constant::simParametersByScript = 0;
		if (Constant::configureToReadPdwsFromFileFlag == 0)
		{
			string dspDefaultDirectory = "/" + Constant::parameterFileName;
			ifstream file1;
			file1.open(dspDefaultDirectory, ios::in | ios::binary);
			if (file1.is_open())
			{
				ConstLow::simOnActualHwFlag = 1;
				ConstLow::simIterationTypeFlag =
						LowIterationTypeFlags::simulationModeDisabled;
				cout << "Reading From DSP directory" << endl;
				Constant::simParametersByScript = 1;
				return readParametersFromFile(file1);

			}
			else
			{
				cout << "No Config file present, using default settings"
						<< endl;
				ConstLow::watchDogEnabledFlag = 0;

				ConstLow::simIterationTypeFlag =
						LowIterationTypeFlags::simulationModeDisabled;

				ConstLow::logPrintEmitters = false;
				ConstLow::logDspReportFlag = false;
				ConstLow::logDspCsvReportFlag = false;
			}
		}

		if (Constant::configureToRunOnHwFlag == 1
				&& Constant::configureToReadPdwsFromFileFlag == 0)
		{
			setLiveModeConstants();
		}
		else if (Constant::configureToRunOnHwFlag == 1
				&& Constant::configureToReadPdwsFromFileFlag == 1)
		{
			setSimulationOnHwConstants();
		}

		else if (Constant::configureToRunOnHwFlag == 0
				&& Constant::configureToReadPdwsFromFileFlag == 1)
		{
			setSimulationOnPcConstants();
		}
	}
	else if (argumentCount == 2)
	{
		Constant::systemdelegateSelectLowHigh = atoi(argumentValue[1]);
		Constant::simParametersByScript = 1;
		if (Constant::configureToRunOnHwFlag == 0
				&& Constant::configureToReadPdwsFromFileFlag == 1)
		{
			setSimulationOnPcConstants();
		}
	}
	else if (argumentCount == 3)
	{
		Constant::systemdelegateSelectLowHigh = atoi(argumentValue[1]);
		Constant::simParametersByScript = 1;

	}
	else
	{

	}
	return parametersCorrectFlag;
}
template<class T>
bool parseParameter(istringstream &stream, T &ref)
{
	bool res = true;
	int64_t n;
	if (stream >> n)
	{
		ref = (T) n;
	}
	else
	{
		res = false;
	}
	return res;
}

bool parseParameter(istringstream &stream, string &ref)
{
	bool res = true;
//	char n[100];
	if (stream >> ref)
	{
//		ref = string(n);
		size_t findPos = ref.find('"');
		string space = "";
		if (findPos != string::npos)
		{
			ref.replace(findPos, 1, space);
		}
		findPos = ref.find('"');
		if (findPos != string::npos)
		{
			ref.replace(findPos, 1, space);
		}
	}
	else
	{
		res = false;
	}
	return res;
}
bool parseFromString(string &line)
{
	size_t findPos = line.find(';');
	string space = " ";
	if (findPos != string::npos)
	{
		line.replace(findPos, 1, space);
	}
	findPos = line.find('=');
	if (findPos != string::npos)
	{
		line.replace(findPos, 1, space);
	}
//	cout << line <<endl;
	istringstream stream(line);
	bool res = true;
	string p;
	if (stream >> p)
	{
//		std::for_each(p.begin(), p.end(), [](char & c)
//		{	c = ::tolower(c);});

		if (p == "Constant::systemdelegateSelectLowHigh")
		{
			res = parseParameter(stream, Constant::systemdelegateSelectLowHigh);
			if (Constant::systemdelegateSelectLowHigh == 2)
			{
				cout << "Auto Run Disabled!" << endl;

				exit(0);
			}
		}
		else if (p == "Constant::configureForTwoBandProcessingFlag")
		{
			res = parseParameter(stream,
					Constant::configureForTwoBandProcessingFlag);
		}

		else if (p == "Constant::configureToRunPlaybackFlag")
		{
			res = parseParameter(stream,
					Constant::configureToReadPdwsFromFileFlag);
		}
		else if (p == "Constant::configureToReadPdwsFromFileFlag")
		{
			res = parseParameter(stream,
					Constant::configureToReadPdwsFromFileFlag);
		}
		else if (p == "ConstLow::watchDogEnabledFlag")
		{
			res = parseParameter(stream, ConstLow::watchDogEnabledFlag);
		}
		else if (p == "ConstHigh::simLogIterationHigh")
		{
			res = parseParameter(stream, ConstHigh::simLogIterationHigh);
		}
		else if (p == "ConstLow::simIterationTypeFlag")
		{
			res = parseParameter(stream, ConstLow::simIterationTypeFlag);
			cout << "ConstLow::simIterationTypeFlag: "
					<< (uint32_t) ConstLow::simIterationTypeFlag << endl;
		}
		else if (p == "ConstLow::matInFileName")
		{
			res = parseParameter(stream, ConstLow::matInFileName);
		}
		else if (p == "ConstLow::logRecordLivePdws")
		{
			res = parseParameter(stream, ConstLow::logRecordLivePdws);
		}
		else if (p == "ConstLow::logPrintEmitters")
		{
			res = parseParameter(stream, ConstLow::logPrintEmitters);
		}
		else if (p == "ConstLow::logDspReportFlag")
		{
			res = parseParameter(stream, ConstLow::logDspReportFlag);
		}
		else if (p == "ConstLow::asPowerThreshBandCD")
		{
			res = parseParameter(stream, ConstLow::asPowerThreshBandCD);
		}
		else if (p == "ConstLow::asPowerThreshBandEJ")
		{
			res = parseParameter(stream, ConstLow::asPowerThreshBandEJ);
		}
		else if (p == "ConstLow::asPowerThreshBandK")
		{
			res = parseParameter(stream, ConstLow::asPowerThreshBandK);
		}
		else if (p == "ConstLow::asPowerThreshPxPyCombinedBandCD")
		{
			res = parseParameter(stream,
					ConstLow::asPowerThreshPxPyCombinedBandCD);
		}
		else if (p == "ConstLow::asPowerThreshPxPyCombinedBandEJ")
		{
			res = parseParameter(stream,
					ConstLow::asPowerThreshPxPyCombinedBandEJ);
		}
		else if (p == "ConstLow::asPowerThreshPxPyCombinedBandK")
		{
			res = parseParameter(stream,
					ConstLow::asPowerThreshPxPyCombinedBandK);
		}
		else if (p == "ConstLow::asPowerThreshPxPyClusteringBandCD")
		{
			res = parseParameter(stream,
					ConstLow::asPowerThreshPxPyClusteringBandCD);
		}
		else if (p == "ConstLow::asPowerThreshPxPyClusteringBandEJ")
		{
			res = parseParameter(stream,
					ConstLow::asPowerThreshPxPyClusteringBandEJ);
		}
		else if (p == "ConstLow::asPowerThreshPxPyClusteringBandK")
		{
			res = parseParameter(stream,
					ConstLow::asPowerThreshPxPyClusteringBandK);
		}
		else
		{
			cout << "Wrong Parameter" << p << endl;
//			res = false;
		}
	}
	return res;
}
bool readParametersFromFile(ifstream &iFile)
{
	if (iFile.is_open() == false)
	{
		return false;
	}

	string line;
	while (std::getline(iFile, line))
	{
		if (parseFromString(line) == false)
		{
			cout << "Invalid Parameters in File \n\n\n\n Exiting" << endl;
			exit(-1);
		}

	}

	return true;
}
