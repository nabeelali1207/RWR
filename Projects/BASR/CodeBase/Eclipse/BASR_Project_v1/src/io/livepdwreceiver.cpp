/*
 * livepdwreceiver.cpp
 *
 *  Created on: Nov 11, 2019
 *      Author: gmd
 */

#include "livepdwreceiver.h"

void LivePdwReceiver::initialize(bool recordingEnabled, std::string fileName)
{
	recordPdws = recordingEnabled;
	if (recordPdws == true)
	{
		dataDumpFile.openFile(fileName);
		string fileName2, fileName3, fileName4;
		fileName2 = "/media/sda1/output/livePdws_ant2.hex";
		fileName3 = "/media/sda1/output/livePdws_ant3.hex";
		fileName4 = "/media/sda1/output/livePdws_ant4.hex";
		dataDumpFile2.openFile(fileName2);
		dataDumpFile3.openFile(fileName3);
		dataDumpFile4.openFile(fileName4);

	}
	MemoryMapAddressClass::kernelLowPdwBuffer[0].initializeKernelMemory(0,
			PDW_BUFFER_SIZE, procNameBuffer1);
	MemoryMapAddressClass::kernelLowPdwBuffer[1].initializeKernelMemory(0,
			PDW_BUFFER_SIZE, procNameBuffer2);
	MemoryMapAddressClass::kernelLowPdwBuffer[2].initializeKernelMemory(0,
			PDW_BUFFER_SIZE, procNameBuffer3);
	MemoryMapAddressClass::kernelLowPdwBuffer[3].initializeKernelMemory(0,
			PDW_BUFFER_SIZE, procNameBuffer4);

	MemoryMapAddressClass::kernelLowPollVector.initializeKernelMemory(2,
			DATA_BUFFER_SIZE, procNamePollVector);

	localBuffer = (UWord8*) malloc(1 << 16);
	localBufferSize = 0;

	spsc_queue.reset();

	memcpyBytes = 0;
	memcpyItrs = 0;
	memcpyTimeBenchmark = 0;
	parseTimeBenchmark = 0;
	enqueTimeBenchmark = 0;
	pdwsProcessed = 0;
	lastValidBuffer = 3;

	resetLastBuffer();
	//resetPdwBufferCounts(0);
	tempPdwsssss.resize(0);
	tempPdwsssss.reserve(512);
	toaOffsetFromBootTime = 0;
}

void LivePdwReceiver::extractPdwsFromBuffer(UWord8 *buffer, UWord32 size)
{
	UWord32 pdwCount = size / PDW_SIZE;
//	UWord32 validPdwCount = 0;
	PdwSmall32 *receivedPdw;

	const UWord32 MAX_PDWS_TO_Read = SPSC_CAPACITY;
	UWord32 readStartPoint = 0;
	uint32_t capcityAvailable = spsc_queue.write_available();

	if (pdwCount > capcityAvailable)
	{
		//system becomes slow to respond if there are too many pdws
		//here we will discard pdws if too many data is available
		readStartPoint = pdwCount - min(MAX_PDWS_TO_Read, capcityAvailable);
//		cout << "System Capacity Reached \n";
	}
	pdwsDiscardedSinceLastPrint += readStartPoint;
	pdwsProcessedSinceLastPrint += (pdwCount - readStartPoint);

//	if (recordPdws == true && readStartPoint != pdwCount)
//	{
//		dataDumpFile.writeDelimitedDataToDisk(
//				buffer + (readStartPoint * PDW_SIZE),
//				size - (readStartPoint * PDW_SIZE));
//	}

	receivedPdw = (PdwSmall32*) &buffer[readStartPoint * PDW_SIZE];

	spsc_queue.push(receivedPdw, (pdwCount - readStartPoint));
}

void LivePdwReceiver::copyDataFromKernelBufferToLocalBuffer(UWord8 numBuffer)
{

//	UWord32 bytesToRead = pdwCount[numBuffer] * PDW_SIZE;
//	UWord8 *buffStartPoint = localBuffer + localBufferSize;
//
//	memcpy((void *) buffStartPoint,
//			(void *) MemoryMapAddressClass::kernelLowPdwBuffer[numBuffer].getVirtualAddressKernel(),
//			bytesToRead);
//	localBufferSize += bytesToRead;

	UWord32 pdwsToRead = pdwCount[numBuffer];
	UWord64 currBufferSize = localBufferSize;
	PdwSmall32 tempPdw;
	UWord8* tempBuff = (UWord8*) &tempPdw;

	UWord32 invalidPdwCount = 0;
	for (UWord32 i = 0; i < pdwsToRead; i++)
	{
		memcpy((void *) tempBuff,
				(void *) (MemoryMapAddressClass::kernelLowPdwBuffer[numBuffer].getVirtualAddressKernel()
						+ i * PDW_SIZE), PDW_SIZE);
		UWord8 pdwValid;

		if (recordPdws == true)
		{
			//dataDumpFile.writeDelimitedDataToDisk(tempBuff, PDW_SIZE);
			if (tempPdw.ANT == 0)
			{
				dataDumpFile.writeDelimitedDataToDisk(tempBuff, PDW_SIZE);
			}
			else if (tempPdw.ANT == 1)
			{
				dataDumpFile2.writeDelimitedDataToDisk(tempBuff, PDW_SIZE);
			}
			else if (tempPdw.ANT == 2)
			{
				dataDumpFile3.writeDelimitedDataToDisk(tempBuff, PDW_SIZE);
			}
			else
			{
				dataDumpFile4.writeDelimitedDataToDisk(tempBuff, PDW_SIZE);
			}

		}

//		pdwValid = ((tempPdw.Pa > 200) || (tempPdw.Pb > 200)
//				|| (tempPdw.Pc > 200) || (tempPdw.Pd > 200)) || (tempPdw.BC == 4);

		pdwValid = ((tempPdw.Pa > 500) || (tempPdw.Pb > 500)
				|| (tempPdw.Pc > 500) || (tempPdw.Pd > 500))
				|| (tempPdw.BC == 4) || (tempPdw.BC == 5);

		pdwValid = pdwValid && ((tempPdw.TOA & 0x0000FFFFFFFFFFFF) != 0);

		pdwValid = pdwValid
				&& ((tempPdw.ST & 1) == 1 || checkpdwValidity(tempPdw));
		uint16_t tempPb = tempPdw.Pc;
		tempPdw.Pc = tempPdw.Pa;
		tempPdw.Pa = tempPdw.Pb;
		tempPdw.Pb = tempPb;
		if(tempPdw.BC==5)
		{
			tempPb=tempPdw.Pa;
			tempPdw.Pa = tempPdw.Pd;
			tempPdw.Pd = tempPb;
		}
		pdwValid = pdwValid && (tempPdw.BC == 5 || tempPdw.RF_V == 1);

		if (pdwValid == 1)
		{
			UWord8 *buffStartPoint = localBuffer + currBufferSize;
			memcpy((void *) buffStartPoint, (void*) tempBuff, PDW_SIZE);
			currBufferSize += PDW_SIZE;
//			if (recordPdws == true)
//			{
//				//dataDumpFile.writeDelimitedDataToDisk(tempBuff, PDW_SIZE);
//				if (tempPdw.ANT == 0)
//				{
//					dataDumpFile.writeDelimitedDataToDisk(tempBuff, PDW_SIZE);
//				}
//				else if (tempPdw.ANT == 1)
//				{
//					dataDumpFile2.writeDelimitedDataToDisk(tempBuff, PDW_SIZE);
//				}
//				else if (tempPdw.ANT == 2)
//				{
//					dataDumpFile3.writeDelimitedDataToDisk(tempBuff, PDW_SIZE);
//				}
//				else
//				{
//					dataDumpFile4.writeDelimitedDataToDisk(tempBuff, PDW_SIZE);
//				}
//
//			}
		}
		else
		{
			invalidPdwCount++;
		}
	}

	localBufferSize = currBufferSize;
	invalidpdwsSinceLastPrint += invalidPdwCount;
	pdwsReceivedSinceLastPrint += pdwsToRead;

}

void LivePdwReceiver::readAvailableData()
{
	KernelPdwReader();

	if (localBufferSize > 0)
	{
		extractPdwsFromBuffer(localBuffer, localBufferSize);
		localBufferSize = 0;
	}
	else
	{
		usleep(1000);
	}
	printInfo();
}

//uint8_t LivePdwReceiver::checkpdwValidity(PdwSmall32& pdw)
//{
//	const uint16_t powerThresholdE = 1980;
//	const uint16_t powerThresholdG = 1960;
//	const uint16_t powerThresholdI = 1880;
//	const uint16_t powerThresholdJ = 1920;
//
//	uint8_t pdwValid = 1;
//	uint16_t px = max(pdw.Pa, pdw.Pc);
//	uint16_t py = max(pdw.Pb, pdw.Pd);
//	uint16_t vdmPower = max(px, py);
//
//	uint8_t attenuationFlag = (pdw.LPI & 0x4) >> 2;
////	uint64_t toa = pdw.TOA & 0x0000FFFFFFFFFFFF;
////	uint64_t rfAmp = (pdw.TOA >> 48) & 0xffff;
//
//	uint16_t powerTreshold = 0;
//	switch (pdw.BC)
//	{
//	case 0:
//		powerTreshold = powerThresholdE;
//		break;
//	case 1:
//		powerTreshold = powerThresholdG;
//		break;
//	case 2:
//		powerTreshold = powerThresholdI;
//		break;
//	case 3:
//		powerTreshold = powerThresholdJ;
//		break;
//	default:
//		powerTreshold = 5000;
//	}
//	if (vdmPower < powerTreshold)
//	{
//		pdwValid = 1;
//	}
//	else
//	{
//		if (attenuationFlag == 1)
//		{
//			pdwValid = 1;
//		}
//		else
//		{
//			pdwValid = 0;
//		}
//	}
//
//	return pdwValid;
////	const uint16_t highPowerThreshold1 = 1950;
////	const uint16_t highPowerThreshold2 = 1950;
////	const uint16_t highPowerThresholdBandI1 = 2200;
////	const uint16_t highPowerThresholdBandI2 = 2100;
////	if (pdw.BC == 2)
////	{
////		if (vdmPower > highPowerThresholdBandI2)
////		{
////			if (attenuationFlag == 1)
////			{
////				pdwValid = 1;
////			}
////			else
////			{
////				pdwValid = 0;
////			}
////		}
////		else if (vdmPower < highPowerThresholdBandI1)
////		{
////			if (attenuationFlag == 1)
////			{
////				pdwValid = 0;
////			}
////			else
////			{
////				pdwValid = 1;
////			}
////		}
////		else
////		{
////			pdwValid = 1;
////		}
////	}
////	else if (vdmPower > highPowerThreshold2)
////	{
////		if (attenuationFlag == 1)
////		{
////			pdwValid = 1;
////
////		}
////		else
////		{
////			pdwValid = 0;
////		}
////	}
////	else if (vdmPower < highPowerThreshold2)
////	{
////		pdwValid = 1;
//////		if (attenuationFlag == 1)
//////		{
//////			pdwValid = 0;
//////		}
//////		else
//////		{
//////			pdwValid = 1;
//////		}
////	}
////	else
////	{
////		pdwValid = 1;
////	}
//}
uint8_t LivePdwReceiver::checkpdwValidity(PdwSmall32& pdw)
{

	uint8_t pdwValid = 1;
//	return pdwValid;
	uint16_t px = max(pdw.Pa, pdw.Pc);
	uint16_t py = max(pdw.Pb, pdw.Pd);
	uint16_t freq = pdw.RF;
	uint16_t vdmPower = max(px, py);

	uint8_t bandCode = pdw.BC;
	uint8_t attenuationFlag = (pdw.LPI & 0x4) >> 2;
//	uint64_t toa = pdw.TOA & 0x0000FFFFFFFFFFFF;
	uint64_t rfAmp = (pdw.TOA >> 48) & 0xffff;
	rfAmp = (bandCode == 4 ? pdw.Pa : rfAmp);

	uint16_t powerTresholdHigh = 2550;
	uint16_t powerTresholdHigh_Band_I_J = 2500;
	uint16_t powerTresholdLow = 1950;
	uint16_t powerTresholdIntermediate = 1990;

	uint8_t Band_G_Check_Edges = ((freq >= 6000) && (freq <= 6100))
			|| ((freq <= 10000) && (freq >= 9900));
	uint8_t Band_I_Check_Edges = ((freq >= 10000) && (freq <= 10100))
			|| ((freq <= 14000) && (freq >= 13900));
	uint8_t Band_J_Check_Edges = ((freq >= 14000) && (freq <= 14100))
			|| ((freq <= 18000) && (freq >= 17900));

	uint8_t Band_G_Check_Center = (freq > 6100) && (freq <= 10000);
	uint8_t Band_I_Check_Center = (freq > 10100) && (freq <= 13900);
	uint8_t Band_J_Check_Center = (freq > 14100) && (freq <= 17900);

//	if(bandCode == 4)
//	{
//		if ( ((freq < 1610) && (freq > 1590)) || (freq > 2000) || (freq < 500) ){
//			pdwValid = 0;
//		}
//		else {
//			pdwValid = 1;
//		}
//	}
//	else if ((bandCode == 0) || (bandCode == 1)) {
//		if (vdmPower < powerTresholdLow) {
//			pdwValid = 1;
//		} else if (vdmPower < powerTresholdHigh) {
//			if (rfAmp > 550) {
//				pdwValid = 1;
//			} else {
//				pdwValid = 0;
//			}
//		} else {
//			if (attenuationFlag == 1) {
//				if (bandCode == 0) {
//					if (rfAmp > 300) {
//						pdwValid = 1;
//					} else {
//						pdwValid = 0;
//					}
//				} else {
//					if (((freq >= 6000) && (freq <= 6100))
//							|| ((freq >= 9900) && (freq <= 10000))) {
//						if (rfAmp > 300) {
//							pdwValid = 1;
//						} else {
//							pdwValid = 0;
//						}
//
//					} else {
//						if (rfAmp > 750) {
//							pdwValid = 1;
//						} else {
//							pdwValid = 0;
//						}
//					}
//				}
//			} else {
//				pdwValid = 0;
//			}
//		}
//	} else	// Other than Band E and G
//	{
//
//		if (vdmPower < powerTresholdLow) {
//			pdwValid = 1;
//		} else {
//			if ((vdmPower > powerTresholdIntermediate)
//					&& (vdmPower < powerTresholdHigh_Band_I_J)) {
//				if (rfAmp > 730) {
//					pdwValid = 1;
//				} else {
//					pdwValid = 0;
//				}
//			} else if ((vdmPower > powerTresholdLow)
//					&& (vdmPower pdwValid< powerTresholdIntermediate)) {
//				if (rfAmp > 450) {
//					pdwValid = 1;
//				} else {
//					pdwValid = 0;
//				}
//
//			} else {
//				if (attenuationFlag == 1) {
//					if (bandCode == 3) {
//						if (((freq >= 14000) && (freq <= 14100))
//								|| ((freq >= 17900) && (freq <= 18000))) {
//							if (rfAmp > 300) {
//								pdwValid = 1;
//							} else {
//								pdwValid = 0;
//							}
//
//						} else {
//							if (rfAmp > 500) {
//								pdwValid = 1;
//							} else {
//								pdwValid = 0;
//							}
//						}
//					} else {
//						if (((freq >= 10000) && (freq <= 10100))
//								|| ((freq >= 13900) && (freq <= 14000))) {
//							if (rfAmp > 300) {
//								pdwValid = 1;
//							} else {
//								pdwValid = 0;
//							}
//
//						} else {
//							if (rfAmp > 813) {
//								pdwValid = 1;
//							} else {
//								pdwValid = 0;
//							}
//						}
//					}
//
//				} else {
//					pdwValid = 0;
//				}
//
//			}
//		}
//	}
//	return pdwValid;

	if (bandCode == 4)
	{
		if (((freq < 1610) && (freq > 1590)) || (freq > 2000) || (freq < 500))
		{
			pdwValid = 0;
		}
		else
		{
			pdwValid = 1;
		}
	}
	else
	{
		if ((vdmPower > (1900 + 400)))
		{
			if (attenuationFlag == 1)
			{
				if ((vdmPower < (2100 + 400)))
				{
					pdwValid = 1;
				}
				else
				{
					if ((Band_G_Check_Center) || (Band_I_Check_Center)
							|| (Band_J_Check_Center))
					{
						if (rfAmp > 700)
						{
							pdwValid = 1;
						}
						else
						{
							pdwValid = 0;
						}
					}
					else if ((Band_G_Check_Edges) || (Band_I_Check_Edges)
							|| (Band_J_Check_Edges))
					{
						if (rfAmp > 300)
						{
							pdwValid = 1;
						}
						else
						{
							pdwValid = 0;
						}
					}
					else
					{
						pdwValid = 1;
					}
				}
			}
			else if(pdw.PW < 30)
			{
				pdwValid = 1;
			}
			else
			{
				pdwValid = 0;
			}
		}
		else if ((vdmPower > (1500 + 400)) && (vdmPower < (1900 + 400)))
		{
			if (attenuationFlag == 0)
			{
				if ((Band_G_Check_Center) || (Band_I_Check_Center)
						|| (Band_J_Check_Center))
				{
					if (rfAmp > 600)
					{
						pdwValid = 1;
					}
					else
					{
						pdwValid = 0;
					}
				}
				else if ((Band_G_Check_Edges) || (Band_I_Check_Edges)
						|| (Band_J_Check_Edges))
				{
					if (rfAmp > 400)
					{
						pdwValid = 1;
					}
					else
					{
						pdwValid = 0;
					}
				}
				else
				{
					pdwValid = 1;
				}
			}
			else
			{
				pdwValid = 1;
			}
		}
		else if (vdmPower > (1350 + 400))
		{
			if (rfAmp > 120)
			{
				pdwValid = 1;
			}
			else
			{
				pdwValid = 0;
			}
		}
		else
		{
			pdwValid = 1;
		}
	}
	return pdwValid;
}
void LivePdwReceiver::printInfo()
{
	if ((Utility::getSystemTime() - lastPrintoutTime) > 5 * 1000000)
	{
		cout
				<< "Pdw counts (processed,discardedDueToTimeout,invalidPdws,total): ("
				<< pdwsProcessedSinceLastPrint << ","
				<< pdwsDiscardedSinceLastPrint << ","
				<< invalidpdwsSinceLastPrint << ","
				<< pdwsReceivedSinceLastPrint << ")" << endl;
		pdwsProcessedSinceLastPrint = 0;
		pdwsDiscardedSinceLastPrint = 0;
		pdwsReceivedSinceLastPrint = 0;
		invalidpdwsSinceLastPrint = 0;
		lastPrintoutTime = Utility::getSystemTime();
	}
}

void LivePdwReceiver::resetLastBuffer()
{

	if (Utility::getSystemTime() - timeOfLastBufferCorrection < 500 * 1000)
	{
		return;
	}
	else
	{
		timeOfLastBufferCorrection = Utility::getSystemTime();
		cout
				<< "Trying to recover from vdm error condition by reseting nextExpectedBuffer value"
				<< endl;
	}
	MemoryMapAddressClass::kernelLowPollVector.readDataFromRegKernel(3,
			dataAvailableStatus);
	if (dataAvailableStatus & 1)
	{
		lastValidBuffer = 3;
	}
	else if (dataAvailableStatus & 2)
	{
		lastValidBuffer = 0;
	}
	else if (dataAvailableStatus & 4)
	{
		lastValidBuffer = 1;
	}
	else if (dataAvailableStatus & 8)
	{
		lastValidBuffer = 2;
	}
}

vector<PdwSmall32>& LivePdwReceiver::getReceivedPdws()
{

	uint32_t availableCount = spsc_queue.read_available();
	uint32_t pdwsToRead = min((uint32_t) 128, availableCount);

	pdwsSinceLastRead.resize(pdwsToRead);

	if (availableCount > 0)
	{
		spsc_queue.pop(&pdwsSinceLastRead[0], pdwsToRead);
	}
	return pdwsSinceLastRead;
}

void LivePdwReceiver::KernelPdwReader()
{
	vdmError = false;

	MemoryMapAddressClass::kernelLowPollVector.readDataFromRegKernel(3,
			dataAvailableStatus);

	dataPresent[0] = dataAvailableStatus & 1;
	dataPresent[1] = dataAvailableStatus & 2;
	dataPresent[2] = dataAvailableStatus & 4;
	dataPresent[3] = dataAvailableStatus & 8;

	if (dataPresent[0])
	{
		MemoryMapAddressClass::kernelLowPollVector.readDataFromRegKernel(7,
				pdwCount[0]);
	}
	if (dataPresent[1])
	{
		MemoryMapAddressClass::kernelLowPollVector.readDataFromRegKernel(11,
				pdwCount[1]);
	}
	if (dataPresent[2])
	{
		MemoryMapAddressClass::kernelLowPollVector.readDataFromRegKernel(15,
				pdwCount[2]);
	}
	if (dataPresent[3])
	{
		MemoryMapAddressClass::kernelLowPollVector.readDataFromRegKernel(19,
				pdwCount[3]);
	}

	uint8_t currBufferIndex = lastValidBuffer;
	uint64_t invalidBufferFlags = 0;

	for (uint8_t j = 0; j < 4; j++)
	{
		currBufferIndex = (currBufferIndex + 1) % 4;
		UWord8 anyDataAvailable = (dataPresent[0]) || (dataPresent[1])
				|| (dataPresent[2]) || (dataPresent[3]);
		if (anyDataAvailable == 1)
		{
			bool orderValid = KernelBufferOrderValidChecker(currBufferIndex);

			if (orderValid == true)
			{
				bool countValid = KernelBufferDataValidChecker(currBufferIndex);
				if (countValid)
				{
					copyDataFromKernelBufferToLocalBuffer(currBufferIndex);
				}
				else
				{
					invalidBufferFlags = (invalidBufferFlags
							| (1 << (currBufferIndex)));
				}

				lastValidBuffer = currBufferIndex;
				pdwCount[currBufferIndex] = 0;
				dataPresent[currBufferIndex] = 0;//clear the data flag without copying if count invalid

			}
			else
			{
				vdmError = true;
				cout << "LivePdws: vdm error\n";
				resetLastBuffer();
				break;
			}
		}
		else
		{
			break;
		}
	}

	if (vdmError || invalidBufferFlags != 0)
	{
		cout << "Data indication error \t\t";
		cout << "DataAvailableFlag: " << dataAvailableStatus
				<< " bufferPdwCounts(b0,b1,b2,b3): (" << pdwCount[0] << " , "
				<< pdwCount[1] << " , " << pdwCount[2] << " , " << pdwCount[3]
				<< ")";
		cout << " last buffer: " << (uint32_t) lastValidBuffer << "\n";
	}
	else if (dataAvailableStatus != 0)
	{
		resetPdwBufferCounts(invalidBufferFlags);
	}
}

bool LivePdwReceiver::KernelBufferOrderValidChecker(UWord8 numBuffer)
{
	// check the order of buffers
	// if data is available in any buffer but expected buffer does not contain data
	// then error exists in buffer order
	UWord8 expectedNextBuffer = (lastValidBuffer + 1) % 4;
	if (numBuffer == expectedNextBuffer && dataPresent[numBuffer] == 1)
	{
		return true;
	}
	//else
	{
		return false;
	}
}

bool LivePdwReceiver::KernelBufferDataValidChecker(UWord8 numBuffer)
{
	// buffer count is not valid if below condition is not met
	// in such a scenario we need to tell the buffer index to vdm
	bool countValid = false;

	if ((pdwCount[numBuffer] != 0) && (pdwCount[numBuffer] <= MAX_PDWS))
	{
		countValid = true;
	}
	else
	{
		countValid = false;
		printf("Curr Buff : %d Pdw Size : %d\n", (UWord32) numBuffer,
				(UWord32) pdwCount[numBuffer]);
	}

	return countValid;
}

void LivePdwReceiver::resetPdwBufferCounts(UWord64 invalidBufferMask)
{

	UWord64 temporaryZero = 0;
	if (dataAvailableStatus & 1)
	{
		MemoryMapAddressClass::kernelLowPollVector.writeDataToRegKernel(7,
				temporaryZero);
	}
	if (dataAvailableStatus & 2)
	{
		MemoryMapAddressClass::kernelLowPollVector.writeDataToRegKernel(11,
				temporaryZero);
	}
	if (dataAvailableStatus & 4)
	{
		MemoryMapAddressClass::kernelLowPollVector.writeDataToRegKernel(15,
				temporaryZero);
	}
	if (dataAvailableStatus & 8)
	{
		MemoryMapAddressClass::kernelLowPollVector.writeDataToRegKernel(19,
				temporaryZero);
	}

	MemoryMapAddressClass::kernelLowPollVector.writeDataToRegKernel(3,
			invalidBufferMask);
}
