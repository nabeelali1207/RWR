/*
 * cwpcqsilink.cpp
 *
 *  Created on: Jan 13, 2021
 *      Author: saad
 */
#define CWPCQSILINK_CPP_

#include "cwpcqsilink.h"

//This function is used to initialize CWP CQSI link
void CwpCqsiLink::initialize()
{
	if (ConstHigh::cwReportsLogOn == 1)
	{
		cwHexDumpFile.openFile(ConstHigh::cwHexDumpFileName);
	}

	MemoryMapAddressClass::cwpCqsiReg.initialize(
			(UWord8*) MemoryMapAddressClass::CWP_CQSI_BASE_REGISTER);
	CwpCqsiLink::sendWord(CwpCqsiLink::CWP_CONTROL_OFFSET, 1);
	CwpCqsiLink::sendWord(CwpCqsiLink::CWP_CONTROL_OFFSET, 0);
}

//This function cleans up internal data structures of the
//class i.e. cwReportRecord and cwRecieveBuffer
void CwpCqsiLink::clean()
{
	memset(&cwReportRecord, 0, sizeof(CwReportRecord));
	memset(cwReportRecieveBuffer, 0, 128 * sizeof(UWord32));
}

//This function sends the word to address offset on CWP CQSI link
void CwpCqsiLink::sendWord(UWord32 address, UWord32 word)
{
	MemoryMapAddressClass::cwpCqsiReg.writeDataToReg(address, word);
}

//This function receives the data from address offset on CWP CQSI link
UWord32 CwpCqsiLink::recvWord(UWord32 address)
{
	UWord32 val = MemoryMapAddressClass::cwpCqsiReg.readDataFromReg(address);
	return val;
}

//This function is the main loop of CW report reading and extraction
//It starts reading data till data stops coming or 127 data packets of 4 bytes each
// have received.Afterwards, it calls the extractor and extracts data from those bytes
void CwpCqsiLink::readDataFromCwpCqsi()
{

	Word32 iterator = 0;
	UWord32 status;
	UWord32 recievedData;
	if ((ConstHigh::cwReportsOn == 1) && (ConstHigh::simOnActualHwFlag == 1))
	{
		//checking if any data has been sen by CWP
		status = CwpCqsiLink::recvWord(CwpCqsiLink::CWP_STATUS_OFFSET);
		//loop till data is not finished or 127 bytes are reached
		while ((status & 0x00000002) == 0)
		{
			//fetching data
			recievedData = CwpCqsiLink::recvWord(CwpCqsiLink::CWP_RX_OFFSET);
			//keeping the data in buffer array
			cwReportRecieveBuffer[iterator] = recievedData;
//            printf("Data Recieved: %.8x \n",recievedData);
			//A check to avoid any misalignment from CWP
			//We have ensured CWP won't miss data inbetween but can miss initial
			//bytes hence this logic was applied
			if ((cwReportRecieveBuffer[0] & 0xFFFFFFFF) == 0xbeeffeed)
			{
				iterator++;
			}
			status = CwpCqsiLink::recvWord(CwpCqsiLink::CWP_STATUS_OFFSET);
			if ((status & 0x00000002) == 2)
			{
				usleep(1);
				status = CwpCqsiLink::recvWord(CwpCqsiLink::CWP_STATUS_OFFSET);
			}
			if (iterator > 127)
			{
				break;
			}
		}
//		if (iterator > 0) {
//			printf("Bytes: %d\n", iterator);
//		}
		//4 data packets of 4 bytes each make a single CW report
		if (iterator >= 4)
		{
			//CW Hex dump logic
			if (ConstHigh::cwReportsLogOn == 1)
			{
				cwHexDumpFile.writeDelimitedDataToDisk(
						(uint8_t*) &cwReportRecieveBuffer[0], (iterator * 4));
				cwHexDumpFile.flushToDisk();
			}
			//extraction logic
			extractCwReportsLogic(iterator);
			iterator = 0;
		}
	}
}

//This function simply calls extraction logic on buffer of data received from CWP
//Extracted data is stored on cwReportRecord structure
void CwpCqsiLink::extractCwReportsLogic(UWord32 count)
{

	Word32 check = 0;
	UWord32 countOfFullCw = 0, i = 0;
	UWord32 numberOfCw = count / 4;
	if (numberOfCw > 20)
	{
		numberOfCw = 20;
	}
	//printf("\n ---------- START OF CW REPORT ---------- \n");
	for (i = 0; i < numberOfCw; i++)
	{
		check = extractCWreports(&cwReportRecord.cwReportArray[i], i);
		if (check == 1)
		{
//            findCwSelfTestEmitters(&cwReportRecord.cwReportArray[i]);
		}
		countOfFullCw = countOfFullCw + check;
		//printf("\n\n ---------- --------------- ---------- \n");
	}
	//printf("\n ---------- END OF CW REPORT ---------- \n");
	if (countOfFullCw > 20)
	{
		countOfFullCw = 20;
	}
	cwReportRecord.numberOfCWPs = countOfFullCw;

}

//This function fills cwReport structure after extracting information from data buffer
//After extracting all essential information from buffer, it calculates AoA of emitter
Word8 CwpCqsiLink::extractCWreports(CwReports *cwReports, UWord8 iterator)
{
	F64 dummyfrequency;
	//check for delimeter sanctity
	if ((cwReportRecieveBuffer[(iterator * 4) + 0] & CWP_DELIMETER_MASK)
			!= 0xbeeffeed)
	{
		return 0;
	}
	cwReports->description = (cwReportRecieveBuffer[(iterator * 4) + 3]
			& CWP_DESCRIPTION_MASK);
	//description 1 or 3 means report is not to be processed
	if ((cwReports->description == 3) || (cwReports->description == 1))
	{
		return 0;
	}
	else
	{
		//extraction of information starts
		dummyfrequency = (cwReportRecieveBuffer[(iterator * 4) + 3]
				& CWP_FREQUENCY_MASK) >> 16;
		cwReports->invalid = ((cwReportRecieveBuffer[(iterator * 4) + 3]
				& CWP_INVALID_MASK) >> 3);
		cwReports->bandCode = ((cwReportRecieveBuffer[(iterator * 4) + 3]
				& CWP_BAND_CODE_MASK) >> 4);
		cwReports->selfTestStatus = ((cwReportRecieveBuffer[(iterator * 4) + 3]
				& CWP_SELF_TEST_MASK) >> 7);
		cwReports->frequency = round(((dummyfrequency) * 1600) / 4096);
		cwReports->LO = ((cwReportRecieveBuffer[(iterator * 4) + 3] & CWP_LO)
				>> 8);
		cwReports->saturationStatusAntenna1 = ((cwReportRecieveBuffer[(iterator
				* 4) + 3] & CWP_SAT_STATUS_ANT1) >> 12);
		cwReports->saturationStatusAntenna2 = ((cwReportRecieveBuffer[(iterator
				* 4) + 3] & CWP_SAT_STATUS_ANT2) >> 13);
		cwReports->saturationStatusAntenna3 = ((cwReportRecieveBuffer[(iterator
				* 4) + 3] & CWP_SAT_STATUS_ANT3) >> 14);
		cwReports->saturationStatusAntenna4 = ((cwReportRecieveBuffer[(iterator
				* 4) + 3] & CWP_SAT_STATUS_ANT4) >> 15);
		cwReports->amplitudeC = (cwReportRecieveBuffer[(iterator * 4) + 2]
				& CWP_AMP_A_MASK);
		cwReports->amplitudeA = ((cwReportRecieveBuffer[(iterator * 4) + 2]
				& CWP_AMP_B_MASK) >> 16);
		cwReports->amplitudeB = (cwReportRecieveBuffer[(iterator * 4) + 1]
				& CWP_AMP_C_MASK);
		cwReports->amplitudeD = ((cwReportRecieveBuffer[(iterator * 4) + 1]
				& CWP_AMP_D_MASK) >> 16);

		if (cwReports->frequency == 0)
		{
			printf("\nRF Invalid \n\n");
			return 0;
		}
		if (cwReports->frequency > 2380 && cwReports->frequency < 2500)
		{
			printf("Discarded CW track RF:%d\n", cwReports->frequency);
			return 0;
		}
//		printf("Amp A %d B %d C %d D %d \n", cwReports->amplitudeA,
//				cwReports->amplitudeB, cwReports->amplitudeC,
//				cwReports->amplitudeD);
//		printf("Band: %d  \n", cwReports->bandCode);
//		printf("\nRF: %d  \n\n", cwReports->frequency);

		cwReports->gainStageB = ((cwReportRecieveBuffer[(iterator * 4) + 2]
				& CWP_GAIN_A) >> 13);
		cwReports->gainStageC = ((cwReportRecieveBuffer[(iterator * 4) + 2]
				& CWP_GAIN_B) >> 29);
		cwReports->gainStageA = ((cwReportRecieveBuffer[(iterator * 4) + 1]
				& CWP_GAIN_C) >> 13);
		cwReports->gainStageD = ((cwReportRecieveBuffer[(iterator * 4) + 1]
				& CWP_GAIN_D) >> 29);
		cwReports->gainFst = ((cwReportRecieveBuffer[(iterator * 4) + 3]
				& CWP_FST_GAIN_MASK) >> 14);
		cwReports->delimeter = (cwReportRecieveBuffer[(iterator * 4) + 0]
				& CWP_DELIMETER_MASK);
		//printf("\nGain FST: %d  \n\n", cwReports->gainFst);
		cwReports->trackUpdate = 1;
		//All RF Board ADC counts are converted to TP/VDM ADC counts as
		//theeta table is implemented in TP ADC counts
		convertAllCwpAmplitudesFromRfADCToTpADC(cwReports);
//				printf("Amp A %d B %d C %d D %d \n", cwReports->amplitudeA,
//						cwReports->amplitudeB, cwReports->amplitudeC,
//						cwReports->amplitudeD);
//				printf("Band: %d  \n", cwReports->bandCode);
//				printf("\nRF: %d  \n\n", cwReports->frequency);
//					printf("Power In dBm, A:%04d, B:%04d, C:%04d, D:%04d\n\n", cwReports->amplitudeAInDbm, cwReports->amplitudeBInDbm, cwReports->amplitudeCInDbm,
//							cwReports->amplitudeDInDbm);
		//Finding AoA by looking up theeta table
		findAngleOfArrival(cwReports);

		return 1;
	}
}

//This function converts all the RF board/CWP ADC counts to dBms
//Afterwards this converts dBms to TP/VDM ADC counts
void CwpCqsiLink::convertAllCwpAmplitudesFromRfADCToTpADC(CwReports *cw)
{

	Word16 aDbm = 0, bDbm = 0, cDbm = 0, dDbm = 0, localPx = 0, localPy = 0;
	Word16 difference = 0, pxCheck = 0, pyCheck = 0, powerChanged = 0,
			dummy = 0;
	//converts all CWP ADC counts to dBms
//	aDbm = rfAdcToDbm(cw->amplitudeA, cw->gainFst, cw->frequency,
//			cw->bandCode);
//	bDbm = rfAdcToDbm(cw->amplitudeB, cw->gainFst, cw->frequency,
//			cw->bandCode);
//	cDbm = rfAdcToDbm(cw->amplitudeC, cw->gainFst, cw->frequency,
//			cw->bandCode);
//	dDbm = rfAdcToDbm(cw->amplitudeD, cw->gainFst, cw->frequency,
//			cw->bandCode);
	if (cw->amplitudeA < 5)
	{
		aDbm = -80;
	}
	else
	{
		aDbm = cwPowerLookup(cw->amplitudeA, cw->gainFst, cw->frequency,
				cw->bandCode);
	}
	if (cw->amplitudeB < 5)
	{
		bDbm = -80;
	}
	else
	{
		bDbm = cwPowerLookup(cw->amplitudeB, cw->gainFst, cw->frequency,
				cw->bandCode);
	}
	if (cw->amplitudeC < 5)
	{
		cDbm = -80;
	}
	else
	{
		cDbm = cwPowerLookup(cw->amplitudeC, cw->gainFst, cw->frequency,
				cw->bandCode);
	}
	if (cw->amplitudeD < 5)
	{
		dDbm = -80;
	}
	else
	{
		dDbm = cwPowerLookup(cw->amplitudeD, cw->gainFst, cw->frequency,
				cw->bandCode);
	}
	//    printf("ADBM:%d\n",aDbm);
	//    printf("BDBM:%d\n",bDbm);
	//    printf("CDBM:%d\n",cDbm);
	//    printf("DDBM:%d\n",dDbm);
//	printf("(RF: %04d, GAin: %01d\n", cw->frequency,cw->gainFst);
//	printf("Power In Adc, A:%04d, B:%04d, C:%04d, D:%04d\n", cw->amplitudeA, cw->amplitudeB, cw->amplitudeC,
//			cw->amplitudeD);
//	printf("Power In dBm, A:%04d, B:%04d, C:%04d, D:%04d\n", aDbm, bDbm, cDbm,
//			dDbm);

	//normalizing px and py to -60 as sensitivity of CWP is -70 while VDM has -60 dBm
	if (aDbm > cDbm)
	{
		localPx = aDbm;
		pxCheck = 1;
	}
	else
	{
		localPx = cDbm;
		pxCheck = 2;
	}
	if (bDbm > dDbm)
	{
		localPy = bDbm;
		pyCheck = 1;
	}
	else
	{
		localPy = dDbm;
		pyCheck = 2;
	}
	if ((localPx < -60) && (localPy < -60))
	{
		if (localPx < -70)
		{
			if (localPy >= -70)
			{
				localPy = -60;
				powerChanged = 1;
			}
		}
		else if (localPy < -70)
		{
			localPx = -60;
			powerChanged = 1;
		}
		else
		{
			if (localPx > localPy)
			{
				dummy = localPy;
			}
			else
			{
				dummy = localPx;
			}
			difference = -60 - dummy;
			localPx += difference;
			localPy += difference;
			powerChanged = 1;
		}
	}
	if (powerChanged == 1)
	{
		if (pxCheck == 1)
		{
			aDbm = localPx;
		}
		else
		{
			cDbm = localPx;
		}
		if (pyCheck == 1)
		{
			bDbm = localPy;
		}
		else
		{
			dDbm = localPy;
		}
	}
	cw->amplitudeAInDbm = aDbm;
	cw->amplitudeBInDbm = bDbm;
	cw->amplitudeCInDbm = cDbm;
	cw->amplitudeDInDbm = dDbm;
	//converting dBm to TP/VDM ADC counts
	cw->amplitudeA = dbmToTpAdc(aDbm);
	cw->amplitudeB = dbmToTpAdc(bDbm);
	cw->amplitudeC = dbmToTpAdc(cDbm);
	cw->amplitudeD = dbmToTpAdc(dDbm);
}

//This function converts amplitude in CWP ADC counts to dBms using fixed formula
//and lookup tables for power correction given by team CWP
Word16 CwpCqsiLink::rfAdcToDbm(Word16 amplitude, UWord8 gain, UWord16 rf,
		UWord8 band)
{
	float vpeak = 0, power = 0, correctedPower = 0;
	if (amplitude <= 0)
	{
		return -500;
	}
	if (gain > 3)
	{
		gain = 3;
	}
	vpeak = (((amplitude * 2) * 0.8) / 4096);
	power = (20 * log10((vpeak / 0.31622))); //hack -7
	if ((gain == 3) && (band == 0))
	{
		power = power + 0;
	}
	//else if((gain == 1) && (band == 0)){
	//	power  = power - 2;
	//}
	else
	{
		power = power - 5;
	}
	printf("Power:%f\n", power);
	//    correctedPower = power - 10 * (5 - gain);
	correctedPower = cwPowerLookup(power, gain, rf, band);
	printf("Corr_Pow:%f\n", correctedPower);
	return (Word16) correctedPower;
}

//This function converts dBm to TP ADC counts
Word16 CwpCqsiLink::dbmToTpAdc(Word16 correctedPowerInDbm)
{
	float dlva = 0, finalPower = 0;
	dlva = (0.0539 * correctedPowerInDbm) + 3.8;
	if (dlva > 5)
	{
		dlva = 5;
	}
	else if (dlva < 0)
	{
		dlva = 0;
	}
	finalPower = (dlva * 0.37) * 2048;
	return (Word16) finalPower;
}
Word16 CwpCqsiLink::tpAdcToDbm(Word16 dbm)
{
	float finalPower = 0;
	finalPower = dbm;
	finalPower = finalPower / 2048.0 / 0.37;
	finalPower = (finalPower - 3.8) / 0.0539;
	return (Word16) finalPower;
}
//This function returns corrected power in dBm based on correction lookup tables
//provided by team CWP.
Word16 CwpCqsiLink::cwPowerLookup(float power, UWord8 gain, UWord16 rf,
		UWord8 band)
{

//	F32 origPow=power;
	Word16 lookedupPower = 0;
	F32 powerDiff = 0, minDiff = 300, fftBin = 0, diffFft = 0;
	UWord16 index = 0, rfIndex = 0, powerIndex = 0;

	if (gain > 3)
	{
		gain = 3;
	}

//	float lookedUpValue=0;

	// Band 0
	if (band == 0)
	{
		fftBin = (float) (abs(abs(4000 - rf) - 1600) * 4096) / 1600.0;
		diffFft = abs(round(fftBin) - fftBin);
		if (diffFft > 0.2)
		{
			power = (float) power * (1.0 / (1.0 - diffFft));
		}
//		printf("FFT Bin Power:%f\n\n", power);

		rfIndex = rf - 2000;

		if (rfIndex == 4000)
		{
			rfIndex--;
		}
		// GS0
		if (gain == 0)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB0Gs0) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB0Gs0[rfIndex][index]);
				if (powerDiff < minDiff)
				{
//					lookedUpValue=cwReceivedPowerB0Gs0[rfIndex][index];
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB0Gs0[powerIndex];
		}
		else if (gain == 1)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB0Gs1) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB0Gs1[rfIndex][index]);
				if (powerDiff < minDiff)
				{
//					lookedUpValue=cwReceivedPowerB0Gs1[rfIndex][index];
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB0Gs1[powerIndex];
		}
		else if (gain == 2)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB0Gs2) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB0Gs2[rfIndex][index]);
				if (powerDiff < minDiff)
				{
//					lookedUpValue=cwReceivedPowerB0Gs2[rfIndex][index];
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB0Gs2[powerIndex];
		}
		else if (gain == 3)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB0Gs3) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB0Gs3[rfIndex][index]);
				if (powerDiff < minDiff)
				{
//					lookedUpValue=cwReceivedPowerB0Gs3[rfIndex][index];
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB0Gs3[powerIndex];
		}
	}
	else if (band == 1)
	{
		fftBin = (float) (abs(abs(4000 - abs(12000 - rf)) - 1600) * 4096)
				/ 1600.0;
		diffFft = abs(round(fftBin) - fftBin);
		if (diffFft > 0.2)
		{
			power = (float) power * (1.0 / (1.0 - diffFft));
		}
//		printf("FFT Bin Power:%f\n\n", power);

		rfIndex = rf - 6000;

		if (rfIndex == 4000)
		{
			rfIndex--;
		}
		// GS0
		if (gain == 0)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB1Gs0) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB1Gs0[rfIndex][index]);
				if (powerDiff < minDiff)
				{
//					lookedUpValue=cwReceivedPowerB1Gs0[rfIndex][index];
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB1Gs0[powerIndex];
		}
		else if (gain == 1)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB1Gs1) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB1Gs1[rfIndex][index]);
				if (powerDiff < minDiff)
				{
//					lookedUpValue=cwReceivedPowerB1Gs1[rfIndex][index];
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB1Gs1[powerIndex];
		}
		else if (gain == 2)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB1Gs2) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB1Gs2[rfIndex][index]);
				if (powerDiff < minDiff)
				{
//					lookedUpValue=cwReceivedPowerB1Gs2[rfIndex][index];
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB1Gs2[powerIndex];
		}
		else if (gain == 3)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB1Gs3) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB1Gs3[rfIndex][index]);
				if (powerDiff < minDiff)
				{
//					lookedUpValue=cwReceivedPowerB1Gs3[rfIndex][index];
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB1Gs3[powerIndex];
		}
	}
	else if (band == 2)
	{
		fftBin = (float) (abs(abs(4000 - abs(16000 - rf)) - 1600) * 4096)
				/ 1600.0;
		diffFft = abs(round(fftBin) - fftBin);
		if (diffFft > 0.2)
		{
			power = (float) power * (1.0 / (1.0 - diffFft));
		}
//		printf("FFT Bin Power:%f\n\n", power);

		rfIndex = rf - 10000;

		if (rfIndex == 4000)
		{
			rfIndex--;
		}
		// GS0
		if (gain == 0)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB2Gs0) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB2Gs0[rfIndex][index]);
				if (powerDiff < minDiff)
				{
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB2Gs0[powerIndex];
		}
		else if (gain == 1)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB2Gs1) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB2Gs1[rfIndex][index]);
				if (powerDiff < minDiff)
				{
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB2Gs1[powerIndex];
		}
		else if (gain == 2)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB2Gs2) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB2Gs2[rfIndex][index]);
				if (powerDiff < minDiff)
				{
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB2Gs2[powerIndex];
		}
		else if (gain == 3)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB2Gs3) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB2Gs3[rfIndex][index]);
				if (powerDiff < minDiff)
				{
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB2Gs3[powerIndex];
		}
	}
	else if (band == 3)
	{
		fftBin = (float) (abs(abs(4000 - abs(12000 - rf)) - 1600) * 4096)
				/ 1600.0;
		diffFft = abs(round(fftBin) - fftBin);
		if (diffFft > 0.2)
		{
			power = (float) power * (1.0 / (1.0 - diffFft));
		}
//		printf("FFT Bin Power:%f\n\n", power);

		rfIndex = rf - 14000;

		if (rfIndex == 4000)
		{
			rfIndex--;
		}
		// GS0
		if (gain == 0)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB3Gs0) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB3Gs0[rfIndex][index]);
				if (powerDiff < minDiff)
				{
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB3Gs0[powerIndex];
		}
		else if (gain == 1)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB3Gs1) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB3Gs1[rfIndex][index]);
				if (powerDiff < minDiff)
				{
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB3Gs1[powerIndex];
		}
		else if (gain == 2)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB3Gs2) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB3Gs2[rfIndex][index]);
				if (powerDiff < minDiff)
				{
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB3Gs2[powerIndex];
		}
		else if (gain == 3)
		{
			for (index = 0;
					index < sizeof(cwCorrectedPowerB3Gs3) / sizeof(Word16);
					index++)
			{
				powerDiff = abs(power - cwReceivedPowerB3Gs3[rfIndex][index]);
				if (powerDiff < minDiff)
				{
					minDiff = powerDiff;
					powerIndex = index;
				}
			}

			lookedupPower = cwCorrectedPowerB3Gs3[powerIndex];
		}
	}
	else if (band == 4)
	{
		rfIndex = rf - 500;

		if (rfIndex == 2000)
		{
			rfIndex--;
		}
		for (index = 0; index < sizeof(cwCorrectedPowerB4) / sizeof(Word16);
				index++)
		{
			powerDiff = abs(power - cwReceivedPowerB4[rfIndex][index]);
			if (powerDiff < minDiff)
			{
				minDiff = powerDiff;
				powerIndex = index;
			}
		}

		lookedupPower = cwCorrectedPowerB4[powerIndex];
	}
//	printf("PowIn:%d, RfInd:%d\n", powerIndex, rfIndex);
//	printf("ADC, Power, Rf, GainStage,minBin,Value,minValue, LookupPower (%04f,%04f,%04d,%01d,%02d,%03.1f,%03.1f, %02d)\n",
//				origPow,round(power),rf, gain,powerIndex,lookedUpValue,minDiff,lookedupPower);
	return lookedupPower;

}

//This function first finds quadrant of emitter and then finds AoA of emitter
void CwpCqsiLink::findAngleOfArrival(CwReports *cw)
{
	//UWord32 delta;
	CwpCqsiLink::findQuadrant(cw);
	//delta = abs(cw->px - cw->py);
//	cw->angleOfArrival = 360
//			- CwpCqsiLink::angleOfArrivalCalculation(cw->px, cw->py,
//					cw->quadrant);

	cw->angleOfArrival = doaCalculateAngle(cw->px, cw->py, cw->quadrant, 2,
			Constant::systemBandCountOperate);
}

//This function first finds max powers across x and y and then finds the quadrant of emitter
void CwpCqsiLink::findQuadrant(CwReports *cw)
{

	CwpCqsiLink::findPxPy(cw);
	if ((cw->px == cw->amplitudeA) && (cw->py == cw->amplitudeB))
	{
		cw->quadrant = 0;
	}
	else if ((cw->px == cw->amplitudeA) && (cw->py == cw->amplitudeD))
	{
		cw->quadrant = 2;
	}
	else if ((cw->px == cw->amplitudeC) && (cw->py == cw->amplitudeD))
	{
		cw->quadrant = 3;
	}
	else if ((cw->px == cw->amplitudeC) && (cw->py == cw->amplitudeB))
	{
		cw->quadrant = 1;
	}
	else
	{
		cw->quadrant = 5;
	}
}

//This function finds max powers across x and y axis
void CwpCqsiLink::findPxPy(CwReports *cw)
{

	if (cw->amplitudeA > cw->amplitudeC)
	{
		cw->px = cw->amplitudeA;
	}
	else
	{
		cw->px = cw->amplitudeC;
	}
	if (cw->amplitudeB > cw->amplitudeD)
	{
		cw->py = cw->amplitudeB;
	}
	else
	{
		cw->py = cw->amplitudeD;
	}
}

//This function extracts angle of arrival from lookup table based on Px, Py and quadrant of emitter
UWord16 CwpCqsiLink::angleOfArrivalCalculation(UWord16 Px, UWord16 Py,
		UWord8 QuadrantCode)
{
	UWord16 delta;
	UWord16 angleOfArrival = 0;
	delta = (UWord16) (Px - Py);
	delta = delta >> 4;
	angleOfArrival = AngleTableOneBand[(QuadrantCode << 12) | delta];
	angleOfArrival = (angleOfArrival * 360) / 256;
	//printf("AOA: %d\n",angleOfArrival);
	return angleOfArrival;
}

//This function simply finds Self Test emitters amongst report CW reports and
//fills self test results accordingly
void CwpCqsiLink::findCwSelfTestEmitters(CwReports *cwReports,
		CwSelfTestResults *cwSelfTestResults, UWord8 FstInProgress)
{
	UWord32 i = 0;
	if (FstInProgress == 1)
	{
		cwSelfTestResults->gainCtlrResult = 0x1; // Gain Cntlr not required for now
		cwSelfTestResults->cwAmpResults = 0xFFFF; // Amp test not required for now
//        printf("CW FST: %d\n",cwReportRecord.numberOfCWPs);
		cwSelfTestResults->cwDetectionResults |= 0x0002;
		cwSelfTestResults->cwDetectionResults |= 0x0010;
		cwSelfTestResults->cwDetectionResults |= 0x0020;
		cwSelfTestResults->cwDetectionResults |= 0x0080;
		cwSelfTestResults->cwDetectionResults |= 0x0100;
		for (i = 0; i < cwReportRecord.numberOfCWPs; i++)
		{
			//printf("CW FST: %d\n", cwReports[i].selfTestStatus);
			if (cwReports[i].selfTestStatus == 1)
			{
				if (cwReports[i].bandCode == 0)
				{
					if (cwReports[i].gainFst == 0)
					{
						cwSelfTestResults->cwDetectionResults |= 0x0001;
					}
//					else if (cwReports[i].gainFst == 1)
//					{
//						cwSelfTestResults->cwDetectionResults |= 0x0002;
//					}
				}
				else if (cwReports[i].bandCode == 1)
				{
					if (cwReports[i].gainFst == 0)
					{
						cwSelfTestResults->cwDetectionResults |= 0x0004;
					}
				}
				else if (cwReports[i].bandCode == 2)
				{
					if (cwReports[i].gainFst == 0 || cwReports[i].gainFst == 1)
					{
						cwSelfTestResults->cwDetectionResults |= 0x0008;
					}
//					if (cwReports[i].gainFst == 0)
//					{
//						cwSelfTestResults->cwDetectionResults |= 0x0008;
//					}
//					else if (cwReports[i].gainFst == 1)
//					{
//						cwSelfTestResults->cwDetectionResults |= 0x0010;
//					}
//                    else if(cwReports[i].gainFst == 2)
//					{
//						cwSelfTestResults->cwDetectionResults |= 0x0020;
//					}
				}
				else if (cwReports[i].bandCode == 3)
				{
					if (cwReports[i].gainFst == 0 || cwReports[i].gainFst == 1)
					{
						cwSelfTestResults->cwDetectionResults |= 0x0040;
					}
//					if (cwReports[i].gainFst == 0)
//					{
//						cwSelfTestResults->cwDetectionResults |= 0x0040;
//					}
//                    else if(cwReports[i].gainFst == 1)
//					{
//						cwSelfTestResults->cwDetectionResults |= 0x0080;
//					}
//                    else if(cwReports[i].gainFst == 2)
//					{
//						cwSelfTestResults->cwDetectionResults |= 0x0100;
//					}
				}
			}
		}
	}
}
