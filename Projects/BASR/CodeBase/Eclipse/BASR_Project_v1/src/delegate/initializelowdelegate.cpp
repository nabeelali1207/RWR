/*
 * initializelowdelegate.cpp
 *
 *  Created on: Oct 15, 2018
 *      Author: hassan
 */

#define INITIALIZELOWDELEGATE_CPP_

#include "initializelowdelegate.h"

void initializeLowDelegate()
{

	LogLowClass::logProgramStartup();
	simLowSetConstants();
	simLowSetVariables();
	outputCreateDebugFiles();
	scaleSetSystemConstants();
	hardwareMountUsbDrive();
//	Model::ethernetlinktp.initializeEthernetLink(ConstLow::readThreatDataFromEthernet ==1);
	if (ConstLow::simOnActualHwFlag == 1)
	{
		SerialLinkLowToHigh::checkKernelPciMod();
	}

	Model::dspIoHandle.initializeIoHandler(ConstLow::logDspReportFlag,
			ConstLow::simOnActualHwFlag == 1);

	if (ConstLow::simIterationTypeFlag
			== LowIterationTypeFlags::simulationModeDisabled && ConstLow::simOnActualHwFlag == 1)
	{
		Model::dspLivePdwReceiver.initialize(ConstLow::logRecordLivePdws,
				ConstLow::dspPdwFlushFileName);

		Model::dspSelfTestController.initializeSelfTestLow();
	}

	if (ConstLow::simIterationTypeFlag
			== LowIterationTypeFlags::simulationModeDisabled && ConstLow::simOnActualHwFlag == 1)
	{

		launchNewThread(dspPdwsThread, dspPdwsThreadAttrib, dspPdwsThreadCpuSet,
				1, threadIoHandlerLow, NULL);

		launchNewThread(dspNetworkInterfaceThread,
				dspNetworkInterfaceThreadAttrib,
				dspNetworkInterfaceThreadCpuSet, 2, threadLowNetworkInterface,
				NULL);

		getCurrentThreadIdAndAttrib(dspMainThread, dspMainThreadAttrib);
		setCurrentThreadAffinity(dspMainThread, dspMainThreadCpuSet, 3);
		cout << "DSP Thread:" << getMyProcessorCore() << endl;
		threadMainLowDelegate(NULL);
	}
	else
	{
		if(Constant::configureToReadPdwsFromFileFlag == 0)
		{


			launchNewThread(dspNetworkInterfaceThread,
					dspNetworkInterfaceThreadAttrib,
					dspNetworkInterfaceThreadCpuSet, 2, threadLowNetworkInterface,
					NULL);
		}


		getCurrentThreadIdAndAttrib(dspMainThread, dspMainThreadAttrib);
		setCurrentThreadAffinity(dspMainThread, dspMainThreadCpuSet, 3);
		cout << "DSP Thread:" << getMyProcessorCore() << endl;
		threadMainLowDelegate(NULL);
	}
}

