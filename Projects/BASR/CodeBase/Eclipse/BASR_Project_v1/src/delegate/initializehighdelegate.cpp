/*
 * initializehighdelegate.cpp
 *
 *  Created on: Sep 25, 2019
 *      Author: hassan
 */

#define INITIALIZEHIGHDELEGATE_CPP_

#include "initializehighdelegate.h"

//one time fuction to initialize all the IOs or files to read data
void initializeHighDelegate()
{
	ConstHigh::simLogIterationHigh = 0;
	ConstHigh::cwReportsLogOn = 0;
	ConstHigh::SW_VER_YEAR = 0x2022;
	ConstHigh::SW_VER_MONTH = 0x08;
	ConstHigh::SW_VER_DATE = 0x1;
	if (ConstHigh::simOnActualHwFlag == 0)
	{
		ConstHigh::libraryPathCurrent = ConstHigh::libraryPathDesktop;
		ConstHigh::libraryKeyPathCurrent = ConstHigh::libraryKeyPathDesktop;
		ConstHigh::flightRecordPathCurrent = ConstHigh::flightRecordPathDesktop;
		ConstHigh::recDataFromDspDumpFileNameCurrent =
				ConstHigh::recDataFromDspDumpFileNameDesktop;
		if (Constant::simParametersByScript == 0)
		{
			ConstHigh::dspReportFileNameCurrent =
					ConstHigh::dspReportFileNameDesktop;
		}
		//ConstHigh::dspReportFileNameCurrent = "output/130508_527_544_CompleteData_DspRep.hex";
	}
	else
	{
		ConstHigh::libraryPathCurrent = ConstHigh::libraryPathHardware;
		ConstHigh::libraryKeyPathCurrent = ConstHigh::libraryKeyPathHardware;
		ConstHigh::flightRecordPathCurrent =
				ConstHigh::flightRecordPathHardware;
		ConstHigh::recDataFromDspDumpFileNameCurrent =
				ConstHigh::recDataFromDspDumpFileNameHardware;
		ConstHigh::dspReportFileNameCurrent =
				ConstHigh::dspReportFileNameHardware;
	}
	scaleSetSystemConstants();

	//start with a size of at least one in analysis history
	//circular buffers of Pulsed threats i.e. analysisHistory initialized
	//by 2 empty DSP reports as its size is 2
	Model::analysisHistory.push_back(Model::analysisLow);
	Model::analysisHistory.push_back(Model::analysisLow);

	//circular buffers of CW threats i.e. analysisCwpHistory initialized
	//by 1 empty CW report as its size is 1
	Model::analysisCwpHistory.push_back(Model::analysisCwp);

	//timer for CF Card Controller is initialized.
	//If CF Card remains missing for a certain amount of
	//time we restart the controller to discover CF Card
	//This technique helps with Hot Swap functionality
	//of CF Card
	Model::cfCard.startTimeCfControllerReset = Utility::getSystemTime();
	//timer for sending Tracking data to Trenz is initialized
	//Trenz is expecting a report atleast by 250 msecs
	//This timer helps in this regard
	Model::lastTrackingDataSentTime = Utility::getSystemTime();

	//CF Card existance is checked every 5 seconds, for Hot Swapping
	//and mounting on Filesystem on the go
	//This timer is used for the above mentioned use
	Model::cfCard.startTimeCfCheck = Utility::getSystemTime();

	if (ConstHigh::simOnActualHwFlag == 1)
	{
		//Initializing SPI interface with Trenz that is used
		//for Communication
		Model::trenzSpiLink.initialize();

		//Initializing CQSI interface with CWP that is used
		//to recieve CW threats
		Model::cwpCqsiLink.initialize();

		//Startup CF Card logic is that first CF card's existance is
		//checked, if CF Card exists, it is mounted and all the
		//Keynames are extracted
		Model::cfCard.startUpCfCardCheckLogic();

		//Initializing CSI interface with DIFM that is used to
		//send MDF and RSS
		Model::difmCsiLink.initialize();
	}

//	Model::cpuIoHandle.initializeIoHandler(ConstLow::logDspReportFlag,
//			ConstHigh::readingFromLvds);

// library load, decrypt and hash here

	//Hashing structures are initialized
	libraryInitHashingStructure(Model::libraryMeta);

	//Library structure is initialized
	libraryInitStructure(Model::library);

	//Function goes to EMMC, finds the key, tl and decrypts it
	libraryLoading(Model::library, Model::libraryMeta, Model::cfdLibrary,
			Model::cfdLibraryMeta, Model::cfCard);

	if (ConstHigh::simOnActualHwFlag == 1)
	{
		//writing MDF to DIFM board using CSI link
		Model::difmCsiLink.sendResetToDifm();
		usleep(2000);
		Model::difmCsiLink.writeMdfToCsi(Model::tlFailure, Model::library);
		usleep(2000);
		//writing RSS to DIFM board using CSI link
		Model::difmCsiLink.writeRssToCsi(Model::tlFailure, Model::library);

		//MFLs are stored in EMMC, this functions checks for every MFL file.
		//If file does not exist, it creates one
		Model::selfTestMflHigh.initMflDirectory();

		//PCIe is the communication protocol between TP and CCP
		//All pulsed threats are reported to CCP using this PCIe
		Model::pcieThreatProcessor.initPcie();

		//Self Test is done by accessing 2 base addresses, one for TP
		//and other for all other boards through VDM. Both of these are
		//initialized in this function
		Model::stDelegate.selfTestInitialize();

		//xadc temperature i.e. temperature of the board is read from this link
		//following function initializes the link to xadc for communication of
		//temperature
		Model::xadcTemperatureHigh.initialize();
		Model::cfdUartLink.initializeCfdUart();

		//old logic, probably would be deleted as system waits for Trenz to start
//		Model::trenzSpiLink.checkTrenzUp();

		//Built In Test is the basic self test protocol where each and every
		//board and communication protocol is tested. It is also done
		//at the start of operation to check for any discrepancies at
		//start of program
		Model::stDelegate.selfTestBuiltIn();

		//Watchdog is initialized here. Commented for debug purposes, uncomment
		//for normal run
		Model::watchdogHigh.initializeWatchdogHigh();
	}

	if (1 == 1)
	{
		launchNewThread(cpuNetworkInterfaceThread,
				cpuNetworkInterfaceThreadAttrib,
				cpuNetworkInterfaceThreadCpuSet, 2, threadHighNetworkInterface,
				NULL);
	}
	//As all the initializations are completed, this function runs in a while(1)
	//and is the main logic of CCP code

	getCurrentThreadIdAndAttrib(cpuMainThread, cpuMainThreadAttrib);
	setCurrentThreadAffinity(cpuMainThread, cpuMainThreadCpuSet, 3);

	cout << "CPU Thread:" << getMyProcessorCore() << endl;
	if (ConstHigh::simOnActualHwFlag == 1)
	{


		threadMainHighDelegate(NULL);
	}
	else
	{
		threadMainHighDelegateSimulator(NULL);

	}
}
