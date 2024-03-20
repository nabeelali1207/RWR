/*
 * outputdelegate.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#define DATAOUTPUTDELEGATE_CPP_

#include "../delegate/dataoutputdelegate.h"

void outputCreateDebugFiles()
{ // debug file and output csv file of low side

	if (ConstLow::simIterationTypeFlag
			== LowIterationTypeFlags::readIterationTimeFromFile)
	{
		if (ConstLow::logHexFileIterationInfo == true)
		{
			ConstLow::logHexFileIterationInfo = false;
		}
		Model::simLowData.iterationTimeFromFile_fileStream.open(
				ConstLow::hexDebugIterDataFileName,
				std::ifstream::in | std::ifstream::binary);

		if (Model::simLowData.iterationTimeFromFile_fileStream.is_open()
				== false)
		{
			ConstLow::simIterationTypeFlag =
					LowIterationTypeFlags::fixedTimeIterationFastSimulation;

		}
		else
		{
			Model::simLowData.iterationTimeFromFile_fileStream.seekg(0,
					std::ifstream::end);
			Model::simLowData.iterationTimeFromFile_ptrMaxSize =
					Model::simLowData.iterationTimeFromFile_fileStream.tellg();

			Model::simLowData.iterationTimeFromFile_fileStream.seekg(0,
					std::ifstream::beg);
			LogIterationInfoClass tempObject;
			if (Model::simLowData.iterationTimeFromFile_ptrMaxSize
					>= sizeof(LogIterationInfoClass))
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
	}

	if (ConstLow::logHexFile == true)
	{
		std::ofstream fid;
		fid.open(ConstLow::hexDebugFileName,
				std::ofstream::out | std::ofstream::binary
						| std::ofstream::trunc);
		fid.close();

		if (ConstLow::logHexFileIterationInfo == true)
		{
			std::ofstream fid;
			fid.open(ConstLow::hexDebugIterDataFileName,
					std::ofstream::out | std::ofstream::binary
							| std::ofstream::trunc);
			fid.close();
		}

		fid.open(ConstLow::csvOutFileName, std::ios::out | std::ios::trunc); // TP output report
		fid
				<< "EmitterID,Band,Bearing,GroundIllum,PulseCount, PWType,PWSpot,FirstTOA,LastTOA,ScanFlag,Px,Py,RF-Type,RF(MHz),PRI Type,NSpots,,,,,,,,,\n"; //write title line here too
		fid.close();
		LogSystemVariablesClass logObject;
		logObject.header.systemIteration = 0;
		logObject.header.systemTime = Utility::getSystemTime();
		strcpy(logObject.fileName, ConstLow::matInFileName.c_str());
		logObject.logToFile(ConstLow::hexDebugFileName);
	}
	if (ConstLow::logDspCsvReportFlag == true)
	{
		std::ofstream fid;
		fid.open(ConstLow::csvOutFileName, std::ios::out | std::ios::trunc); // TP output report
		fid
				<< "EmitterID,Band,Bearing,GroundIllum,PulseCount, PWType,PWSpot,FirstTOA,LastTOA,ScanFlag,Px,Py,RF-Type,RF(MHz),PRI Type,NSpots,,,,,,,,,\n"; //write title line here too
		fid.close();
	}

}
