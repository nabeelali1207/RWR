/*
 * selftesthighdelegate.cpp
 *
 *  Created on: Jun 25, 2021
 *      Author: zumair
 */

#define SELFTESTHIGHDELEGATE_CPP_

#include "../delegate/selftesthighdelegate.h"

SelfTestControllerHigh stHigh;

void SelfTestHighDelegate::selfTestInitialize()
{
	if ((ConstHigh::simOnActualHwFlag == 1)
			&& (ConstHigh::readingFromLvds == 1))
	{
		selfTestTimeout = Utility::getSystemTime();
		stHigh.initializeTpSelfTest();
		stHigh.initializePpfSelfTest();
		stHigh.sendWordPpfSelfTest(SELFTEST_CONTROL, 1);
		stHigh.sendWordPpfSelfTest(SELFTEST_CONTROL, 0);
	}
}

void SelfTestHighDelegate::selfTestPpf()
{
	UWord8 status = 0;
	UWord32 timeDifference = 0;
	uint64_t initTime, currentTime;

	stHigh.ppfGetSelfTestResults(&stData[0]);
	memset(&stData, 0, sizeof(stData));

	// start modular test for Custom Zynq
	status = stHigh.ppfGetSelfTestStatus(1);
	if (status == 1)
	{
		printf("PPF ST Busy!\n");
		status = stHigh.ppfGetSelfTestStatus(2);

		if (status == 2)
		{
			printf("PPF ST Done!\n");

			initTime = Utility::getSystemTime();

			selfTestTimeout = Utility::getSystemTime();
			while (1)
			{
				readCount = stHigh.ppfGetSelfTestResults(&stData[0]);

				// check if fault packet, break
				if (readCount > 1)
				{
					break;
				}

				currentTime = Utility::getSystemTime();
				timeDifference = (currentTime - initTime) / 1000;
				if (timeDifference > 500)
				{
					break;
				}
			}

			selfTestUpdateMfl(readCount, 1);

			printf("PPF MFL Done!\n");
		}
		else
		{
			printf("PPF ST failed to end!\n");
		}
	}
	else
	{
		printf("PPF ST failed to start!\n");
	}
}

void SelfTestHighDelegate::selfTestCcp()
{
	memset(&stData, 0, sizeof(stData));
	Model::selfTestCcpHigh.testAllModulesCCP(stData, Model::tlFailure,
			Model::xadcTemperatureHigh, Model::cfCard);

	selfTestUpdateMfl(readCount, 4);

	printf("CCP Self Test Done!\n");
}

void SelfTestHighDelegate::selfTestTrenz()
{
	memset(&stData, 0, sizeof(stData));

	SpiSelftTestPbitLogic(stData);

	selfTestUpdateMfl(readCount, 3);

	printf("Trenz Self Test Done!\n");
}

void SelfTestHighDelegate::selfTestTp()
{
	UWord32 status = 0;

	status = stHigh.tpGetSelfTestStatus();
	printf("TP ST Busy!\n");

	memset(&stData, 0, sizeof(stData));
	readCount = 0;

	if ((status == 2) || (status == 32) || (status == 128))
	{
		readCount = stHigh.tpGetSelfTestResults(&stData[0]);

		selfTestUpdateMfl(readCount, 2);

		printf("TP Self Test Done!\n");
	}
}

void SelfTestHighDelegate::selfTestRfBoard()
{
	memset(&stData, 0, sizeof(stData));
	readCount = Model::difmCsiLink.rfBoardSelfTestLogic(stData, 0);

	selfTestUpdateMfl(readCount, 5);

	printf("RF Board Self Test Done!\n");
}

//This is the main logic of Self Test. All the Self Test steps are started i.e.
//firstly, all modular tests are completed and eventually functional self test
//gets started
void SelfTestHighDelegate::selfTestBuiltIn()
{
	if ((ConstHigh::simOnActualHwFlag == 1)
			&& (ConstHigh::readingFromLvds == 1))
	{

		selfTestTimeout = Utility::getSystemTime();
		Model::selfTestMflHigh.clearPflStruct();
		memset(Model::selfTestMflHigh.mflData, 0,
				sizeof(Model::selfTestMflHigh.mflData));

		memset(&Model::fileoutputflightrecord.flightRecordMaintainanceHeader, 0,
				sizeof(Model::fileoutputflightrecord.flightRecordMaintainanceHeader));

		Model::fileoutputflightrecord.fillMaintenaceFlightRecordHeader();

		if (ConstHigh::FstInProgress == 0)
		{
			ConstHigh::mStRetryCount = 0;
			ConstHigh::FstRetryCount = 0;
			ConstHigh::FstCurrTryDuration = 3 * 1000;
			ConstHigh::FstMaxDuration = 9 * 1000;
		}

		selfTestModular();
		selfTestFunctional();
	}
}

//This is the main logic of Modular Self Test of each LRU of BASR
//This function performs Modular self tests and collects their results one by one
//It starts with VDM, then TP, then CCP, then Trenz and eventually RF Board
void SelfTestHighDelegate::selfTestModular()
{
	selfTestPpf();
	selfTestTp();
	selfTestTrenz();
	selfTestCcp();
	selfTestRfBoard();
}

//This function sends start functional self test message to DIFM
void SelfTestHighDelegate::selfTestFunctional()
{
	if (ConstHigh::FstInProgress == 0)
	{
		ConstHigh::FstInProgress = 1;
		Model::difmCsiLink.rfBoardSelfTestLogic(stData, 1);
		printf("FST Start!\n");

		fstTimeout = Utility::getSystemTime();
		selfTestTimeout = Utility::getSystemTime();

		memset(&cwSelfTestResults, 0, sizeof(cwSelfTestResults));
		memset(&tpSelfTestResults, 0, sizeof(tpSelfTestResults));
	}
}

//This function gathers all the Self Test results at the end of Functional Self Test
//All the results are extracted and eventually stored in MFL files
void SelfTestHighDelegate::selfTestHandleFstMfl()
{
	UWord8 i = 0;
	UWord8 fId = 15, module = 4;
	UWord32 mflData = 0;
	UWord16 timeDifference;

	endTime = Utility::getSystemTime();

	timeDifference = (endTime - fstTimeout) / 1000;

	if (/*(Model::stDelegate.selfTestType>2) && */(ConstHigh::StopFst == 0) && (ConstHigh::FstInProgress == 1)
			&& (ConstHigh::FstRetryCount < 2)
			&& (timeDifference < ConstHigh::FstMaxDuration))
	{
		if (timeDifference > ConstHigh::FstCurrTryDuration)
		{
			bool retryFst = false;
			bool retryMst = false;
			
			retryMst |= Model::selfTestMflHigh.checkNewPfl3Existence();
			retryMst |= Model::selfTestMflHigh.checkNewPfl4Existence();

			if (retryMst == true && ConstHigh::FstRetryCount == 0)
			// Change checks for this block at your own risk ... otherwise
			// state machine variables like ConstHigh::FstRetryCount, ConstHigh::FstInProgress might not be preserved
			{
				selfTestBuiltIn(); // fst wont start but only modular tests will be done
			}
			retryFst = retryFst
					|| (tpSelfTestResults.tpPulseAmpResults != 0xffff);
			retryFst = retryFst || (tpSelfTestResults.tpRfAmpResults != 0xffff);
			retryFst = retryFst || (tpSelfTestResults.pulseParamResult < 32);
			retryFst = retryFst
					|| (tpSelfTestResults.tpDetectionResults != 0xffff);
			retryFst = retryFst
					|| (cwSelfTestResults.cwDetectionResults != 0x1ff);
			retryFst = retryFst || (cwSelfTestResults.cwAmpResults != 0xffff);
			retryFst = retryFst || (cwSelfTestResults.gainCtlrResult != 1);

//			if(ConstHigh::FstRetryCount==0 && tpSelfTestResults.tpDetectionResults==0 && retryFst==false)
			if (tpSelfTestResults.tpDetectionResults == 0)
			{
				printf("Sending reset to RF board!\n");
				Model::difmCsiLink.sendResetToDifm();
				usleep(4000);
				Model::difmCsiLink.writeMdfToCsi(Model::tlFailure,
						Model::library);
				usleep(4000);
				//writing RSS to DIFM board using CSI link
				Model::difmCsiLink.writeRssToCsi(Model::tlFailure,
						Model::library);
				usleep(10000);
			}
			if (retryFst)
			{
				printf("Retry FST!\n");
				Model::difmCsiLink.rfBoardSelfTestLogic(stData, 1);
				ConstHigh::FstRetryCount++;
			}

			if (retryFst == false)
			{
				ConstHigh::FstMaxDuration = ConstHigh::FstCurrTryDuration;
			}
			ConstHigh::FstCurrTryDuration += 3000;
		}
	}
	if ((ConstHigh::StopFst == 1)
			|| ((ConstHigh::FstInProgress == 1)
					&& (timeDifference > ConstHigh::FstMaxDuration)))
	{
		selfTestTimeout = Utility::getSystemTime();
		ConstHigh::FstInProgress = 0;
		printf("FST End! \n");

		printf("Detection :%.8x ", tpSelfTestResults.tpDetectionResults);
		printf("Amp: %.8x ", tpSelfTestResults.tpPulseAmpResults);
		printf("RfAmp: %.8x ", tpSelfTestResults.tpRfAmpResults);
		printf("Param: %d \n", tpSelfTestResults.pulseParamResult);
		printf("CWParam:0x%08x CWAmp: 0x%08x gainCtrl: %d \n",
				cwSelfTestResults.cwDetectionResults,
				cwSelfTestResults.cwAmpResults,
				cwSelfTestResults.gainCtlrResult);
		UWord32 compositDifmResult = tpSelfTestResults.tpDetectionResults
				& tpSelfTestResults.tpPulseAmpResults
				& tpSelfTestResults.tpRfAmpResults;

//
//        printf("\n%.8x ", cwSelfTestResults.cwDetectionResults);
//        printf("\n%.8x ", cwSelfTestResults.cwAmpResults);
//        printf("\n%.8x \n", cwSelfTestResults.gainCtlrResult);

		for (i = 0; i < 48; i++)
		{
			mflData = ((1) << 30);
			mflData |= (module << 25);
			mflData |= (fId << 17);
			if (i < 16)
			{
//				mflData |= (tpSelfTestResults.tpDetectionResults >> i) ^ 0x0001;
				mflData |= (compositDifmResult >> i) ^ 0x0001;
			}
			else if (i < 32)
			{
//				mflData |= (tpSelfTestResults.tpRfAmpResults >> (i - 16))
//						^ 0x0001;
				mflData |= (compositDifmResult >> (i - 16)) ^ 0x0001;
			}
			else
			{
//				mflData |= (tpSelfTestResults.tpPulseAmpResults >> (i - 32))
//						^ 0x0001;

				mflData |= (compositDifmResult >> (i - 32)) ^ 0x0001;
			}

			fillPacketInfo(mflData);
			Model::selfTestMflHigh.updateMflEntry(Model::selfTestMflHigh.fault,
					Model::fileoutputflightrecord.RTC_TIME,
					Model::fileoutputflightrecord.flightRecordMaintainanceHeader);

			fId++;
		}

		fId = 7;
		module = 5;

		for (i = 0; i < 18; i++)
		{
			mflData = ((1) << 30);
			mflData |= (module << 25);
			mflData |= (fId << 17);
			if (i < 9)
			{
				mflData |= (cwSelfTestResults.cwDetectionResults >> i) ^ 0x0001;
			}
			else
			{
				mflData |= (cwSelfTestResults.cwAmpResults >> (i - 9)) ^ 0x0001;
			}

			fillPacketInfo(mflData);
			Model::selfTestMflHigh.updateMflEntry(Model::selfTestMflHigh.fault,
					Model::fileoutputflightrecord.RTC_TIME,
					Model::fileoutputflightrecord.flightRecordMaintainanceHeader);

			fId++;
		}

		fId = 4;
		module = 6;

		mflData = ((1) << 30);
		mflData |= (module << 25);
		mflData |= (fId << 17);
		if (tpSelfTestResults.pulseParamResult >= 32)
		{
			mflData |= 0;
		}
		else
		{
			mflData |= 1;
		}

		fillPacketInfo(mflData);
		Model::selfTestMflHigh.updateMflEntry(Model::selfTestMflHigh.fault,
				Model::fileoutputflightrecord.RTC_TIME,
				Model::fileoutputflightrecord.flightRecordMaintainanceHeader);

		fId++;
		mflData = ((1) << 30);
		mflData |= (module << 25);
		mflData |= (fId << 17);
		mflData |= (cwSelfTestResults.gainCtlrResult ^ 0x0001);
		Model::selfTestMflHigh.fillSpecialPfls(compositDifmResult);

		fillPacketInfo(mflData);
		Model::selfTestMflHigh.updateMflEntry(Model::selfTestMflHigh.fault,
				Model::fileoutputflightrecord.RTC_TIME,
				Model::fileoutputflightrecord.flightRecordMaintainanceHeader);

		Model::fileoutputflightrecord.writeEncryptedMaintenanceFlightRecord(
				Model::selfTestMflHigh.mflData, &Model::libraryMeta,
				ConstHigh::flightRecordPathCurrent, Model::memoryFailure);
		memset(&mflData, 0, sizeof(mflData));
		memset(&Model::fileoutputflightrecord.flightRecordMaintainanceHeader, 0,
				sizeof(Model::fileoutputflightrecord.flightRecordMaintainanceHeader));


		onBitCompletionLogic();

		ConstHigh::StopFst = 0;
		fstTimeout = Utility::getSystemTime();
		selfTestTimeout = Utility::getSystemTime();
		lastSpecialPeriodicBitTime = Utility::getSystemTime();
		Model::stDelegate.selfTestType = 0;
		Model::stDelegate.selfTestActiveFlag = 0;
	}
}

//This is the main logic of Periodic Self Test
//This function performs Periodic self tests and collects their results one by one
//It starts with Trenz, then VDM, then TP, then CCP and eventually RF Board
void SelfTestHighDelegate::selfTestPeriodic(uint32_t syntheticReportFlag)
{
	UWord32 timeDifference;
	UWord32 timeDifference2;

	if ((ConstHigh::simOnActualHwFlag == 1) && (ConstHigh::readingFromLvds == 1)
			&& (ConstHigh::FstInProgress == 0) && (syntheticReportFlag == 0))
	{
		endTime = Utility::getSystemTime();

		timeDifference = (endTime - selfTestTimeout) / 1000;
		timeDifference2 = (endTime - lastSpecialPeriodicBitTime) / 1000;

		lastSpecialPeriodicBitTime = Utility::getSystemTime();

		/*if(timeDifference2 > 200*1000)
		{

			Model::stDelegate.selfTestType = 2;
			Model::stDelegate.selfTestActiveFlag = 1;
			selfTestBuiltIn();
		}
		else */if (timeDifference > 20000)
		{

			Model::stDelegate.selfTestType = 1;
			Model::stDelegate.selfTestActiveFlag = 1;

			Model::selfTestMflHigh.clearPflStruct();

			memset(Model::selfTestMflHigh.mflData, 0,
					sizeof(Model::selfTestMflHigh.mflData));
//			memset(&Model::selfTestMflHigh.pfl, 0, sizeof(PflStructure));
			memset(
					&Model::fileoutputflightrecord.flightRecordMaintainanceHeader,
					0,
					sizeof(Model::fileoutputflightrecord.flightRecordMaintainanceHeader));
			Model::fileoutputflightrecord.fillMaintenaceFlightRecordHeader();

			if (trenzStResults == 1)
			{
				// Trenz
				trenzStResults = 0;
				readCount = ConstHigh::CPF_ERROR_COUNT;
				selfTestUpdateMfl(readCount, 3);
			}

			// PPF
			memset(&stData, 0, sizeof(stData));
			readCount = 0;
			readCount = stHigh.ppfGetSelfTestResults(&stData[0]);
			selfTestUpdateMfl(readCount, 1);

			// TP
			memset(&stData, 0, sizeof(stData));
			readCount = 0;
			readCount = stHigh.tpGetSelfTestResults(&stData[0]);
			selfTestUpdateMfl(readCount, 2);

			// CCP
			memset(&stData, 0, sizeof(stData));
			readCount = 0;
			Model::selfTestCcpHigh.testAllModulesCCP(stData, Model::tlFailure,
					Model::xadcTemperatureHigh, Model::cfCard);
			selfTestUpdateMfl(readCount, 4);

			// RFB
			memset(&stData, 0, sizeof(stData));
			readCount = 0;
			readCount = stHigh.rfbGetSelfTestResults(&stData[0]);
			selfTestUpdateMfl(readCount, 5);

			Model::fileoutputflightrecord.writeEncryptedMaintenanceFlightRecord(
					Model::selfTestMflHigh.mflData, &Model::libraryMeta,
					ConstHigh::flightRecordPathCurrent, Model::memoryFailure);
			memset(Model::selfTestMflHigh.mflData, 0,
					sizeof(Model::selfTestMflHigh.mflData));
			memset(
					&Model::fileoutputflightrecord.flightRecordMaintainanceHeader,
					0,
					sizeof(Model::fileoutputflightrecord.flightRecordMaintainanceHeader));

			selfTestTimeout = Utility::getSystemTime();

			onBitCompletionLogic();
			Model::stDelegate.selfTestType = 0;
			Model::stDelegate.selfTestActiveFlag = 0;
		}
	}
}

void SelfTestHighDelegate::selfTestUpdateMfl(UWord8 count, UWord8 module)
{
	UWord8 type = 0;
	for (auto i = 0; i < count; i++)
	{
		type = fillPacketInfo(stData[i]);
		if (type)
		{
			if (module == 1)		// PPF
			{
				// Work around for toggling error CWP
				if (Model::selfTestMflHigh.fault.faultId == 11)
				{
					printf("fId: %d, st: %d\n",
							Model::selfTestMflHigh.fault.faultId,
							Model::selfTestMflHigh.fault.status);
					printf("PPF Error \n");
					continue;
				}
			}
			else if (module == 2)	// TP
			{
				// Work around for USB error
				if (Model::selfTestMflHigh.fault.faultId == 5)
				{
					continue;
				}
			}
			else if (module == 3)	// TRENZ
			{
				// Trenz FPGA XDC Temperature
				if ((Model::selfTestMflHigh.fault.module == 0)
						&& (Model::selfTestMflHigh.fault.faultId == 11)
						&& (Model::selfTestMflHigh.fault.status == 1))
				{
					ConstHigh::sendShutdown = 1;
				}
			}
			else if (module == 4)	// CCP
			{
				// VDM FPGA XDC Temperature
				if ((Model::selfTestMflHigh.fault.module == 3)
						&& (Model::selfTestMflHigh.fault.faultId == 11)
						&& (Model::selfTestMflHigh.fault.status == 1))
				{
					ConstHigh::sendShutdown = 1;
				}
			}
			else if (module == 5)	// RFB
			{
				// ADC IC Temperature
				if ((Model::selfTestMflHigh.fault.module == 4)
						&& (Model::selfTestMflHigh.fault.faultId == 12)
						&& (Model::selfTestMflHigh.fault.status == 1))
				{
					ConstHigh::sendShutdown = 1;
				}
				// ADC Card Temperature
				if ((Model::selfTestMflHigh.fault.module == 4)
						&& (Model::selfTestMflHigh.fault.faultId == 13)
						&& (Model::selfTestMflHigh.fault.status == 1))
				{
					ConstHigh::sendShutdown = 1;
				}
				// DIFM FPGA XDC Temperature
				if ((Model::selfTestMflHigh.fault.module == 4)
						&& (Model::selfTestMflHigh.fault.faultId == 14)
						&& (Model::selfTestMflHigh.fault.status == 1))
				{
					ConstHigh::sendShutdown = 1;
				}
				// CWP FPGA XDC Temperature
				if ((Model::selfTestMflHigh.fault.module == 5)
						&& (Model::selfTestMflHigh.fault.faultId == 6)
						&& (Model::selfTestMflHigh.fault.status == 1))
				{
					ConstHigh::sendShutdown = 1;
				}
				// DCM Temperature
				if ((Model::selfTestMflHigh.fault.module == 6)
						&& (Model::selfTestMflHigh.fault.faultId == 3)
						&& (Model::selfTestMflHigh.fault.status == 1))
				{
					ConstHigh::sendShutdown = 1;
				}

				// Work around for toggling error CWP
				if ((Model::selfTestMflHigh.fault.faultId == 3)
						&& (Model::selfTestMflHigh.fault.module == 5))
				{
					continue;
				}
			}

			Model::selfTestMflHigh.updateMflEntry(Model::selfTestMflHigh.fault,
					Model::fileoutputflightrecord.RTC_TIME,
					Model::fileoutputflightrecord.flightRecordMaintainanceHeader);
		}
	}
}

void SelfTestHighDelegate::selfTestSynthetic(uint32_t failures)
{
	if (failures == 0)
	{
		return;
	}

	SingleFaultData syntheticFailures;

	memset(Model::selfTestMflHigh.mflData, 0,
			sizeof(Model::selfTestMflHigh.mflData));
	memset(&Model::selfTestMflHigh.pfl, 0, sizeof(PflStructure));

	// TP ARM
	syntheticFailures.packetId = 1;
	syntheticFailures.module = 2;
	syntheticFailures.faultId = 1;
	syntheticFailures.count = 0;
	syntheticFailures.status = failures & 0x01;
	Model::selfTestMflHigh.updateMflEntry(syntheticFailures,
			Model::fileoutputflightrecord.RTC_TIME,
			Model::fileoutputflightrecord.flightRecordMaintainanceHeader);

	// TP DSP
	syntheticFailures.packetId = 1;
	syntheticFailures.module = 2;
	syntheticFailures.faultId = 2;
	syntheticFailures.count = 0;
	syntheticFailures.status = (failures >> 1) & 0x01;
	Model::selfTestMflHigh.updateMflEntry(syntheticFailures,
			Model::fileoutputflightrecord.RTC_TIME,
			Model::fileoutputflightrecord.flightRecordMaintainanceHeader);

	// TP Flash
	syntheticFailures.packetId = 1;
	syntheticFailures.module = 2;
	syntheticFailures.faultId = 2;
	syntheticFailures.count = 0;
	syntheticFailures.status = (failures >> 2) & 0x01;
	Model::selfTestMflHigh.updateMflEntry(syntheticFailures,
			Model::fileoutputflightrecord.RTC_TIME,
			Model::fileoutputflightrecord.flightRecordMaintainanceHeader);

	// CF Card Full
	syntheticFailures.packetId = 1;
	syntheticFailures.module = 3;
	syntheticFailures.faultId = 3;
	syntheticFailures.count = 0;
	syntheticFailures.status = (failures >> 3) & 0x01;
	Model::selfTestMflHigh.updateMflEntry(syntheticFailures,
			Model::fileoutputflightrecord.RTC_TIME,
			Model::fileoutputflightrecord.flightRecordMaintainanceHeader);

	// CQSI link fault - PPF
	syntheticFailures.packetId = 1;
	syntheticFailures.module = 1;
	syntheticFailures.faultId = 8;
	syntheticFailures.count = 0;
	syntheticFailures.status = (failures >> 4) & 0x01;
	Model::selfTestMflHigh.updateMflEntry(syntheticFailures,
			Model::fileoutputflightrecord.RTC_TIME,
			Model::fileoutputflightrecord.flightRecordMaintainanceHeader);

	// Aurora link
	syntheticFailures.packetId = 1;
	syntheticFailures.module = 1;
	syntheticFailures.faultId = 3;
	syntheticFailures.count = 0;
	syntheticFailures.status = (failures >> 5) & 0x01;
	Model::selfTestMflHigh.updateMflEntry(syntheticFailures,
			Model::fileoutputflightrecord.RTC_TIME,
			Model::fileoutputflightrecord.flightRecordMaintainanceHeader);

	// CWP FPGA Fatal Error
	syntheticFailures.packetId = 1;
	syntheticFailures.module = 5;
	syntheticFailures.faultId = 3;
	syntheticFailures.count = 0;
	syntheticFailures.status = (failures >> 6) & 0x01;
	Model::selfTestMflHigh.updateMflEntry(syntheticFailures,
			Model::fileoutputflightrecord.RTC_TIME,
			Model::fileoutputflightrecord.flightRecordMaintainanceHeader);

	// PCIe link fault
	syntheticFailures.packetId = 1;
	syntheticFailures.module = 1;
	syntheticFailures.faultId = 6;
	syntheticFailures.count = 0;
	syntheticFailures.status = (failures >> 7) & 0x01;
	Model::selfTestMflHigh.updateMflEntry(syntheticFailures,
			Model::fileoutputflightrecord.RTC_TIME,
			Model::fileoutputflightrecord.flightRecordMaintainanceHeader);

	// EMMC Full
	syntheticFailures.packetId = 1;
	syntheticFailures.module = 3;
	syntheticFailures.faultId = 4;
	syntheticFailures.count = 0;
	syntheticFailures.status = (failures >> 8) & 0x01;
	Model::selfTestMflHigh.updateMflEntry(syntheticFailures,
			Model::fileoutputflightrecord.RTC_TIME,
			Model::fileoutputflightrecord.flightRecordMaintainanceHeader);

	// EMMC Read Write
	syntheticFailures.packetId = 1;
	syntheticFailures.module = 3;
	syntheticFailures.faultId = 6;
	syntheticFailures.count = 0;
	syntheticFailures.status = (failures >> 9) & 0x01;
	Model::selfTestMflHigh.updateMflEntry(syntheticFailures,
			Model::fileoutputflightrecord.RTC_TIME,
			Model::fileoutputflightrecord.flightRecordMaintainanceHeader);

	//sendPeriodicAsyncSop();
}

//This function extracts results out of readData and returns the type of
//results it got
UWord8 SelfTestHighDelegate::fillPacketInfo(UWord32 readData)
{
	UWord8 type = readData >> 30;

	if (type)
	{
		Model::selfTestMflHigh.fillFaultPacketInfo(readData);
	}
	else
	{
		testStatus.packetId = type;
		testStatus.faultBitStatus = (readData >> 6) & 0x00000003;
		testStatus.periodicBitStatus = (readData >> 4) & 0x00000003;
		testStatus.functionalBitStatus = (readData >> 2) & 0x00000003;
		testStatus.modularBitStatus = readData & 0x00000003;
	}

	return type;
}
