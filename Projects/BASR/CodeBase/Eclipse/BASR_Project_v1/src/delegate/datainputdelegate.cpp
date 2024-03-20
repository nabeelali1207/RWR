/*
 * manageinputdelegate.cpp
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#define DATAINPUTDELEGATE_CPP_

#include "../delegate/datainputdelegate.h"
#include "../delegate/datascaledelegate.h"
#include <algorithm>

void inputReadPdwSimulationFile(queue<PdwDefaultClass>& inputPdwList)
{

	if (ConstLow::simIterationTypeFlag
			== LowIterationTypeFlags::simulationModeDisabled)
	{
		return;
	}
	if (Constant::configureToReadPdwsFromFileFlag == 0)
	{
		return;
	}
//	readPdwAllMatFileOfBasr(ConstLow::matInFileName.c_str(),
//			ConstLow::csvOutFileName.c_str(), inputPdwList,
//			ConstLow::simOutRawPdwFileFlag);
	readAllPdwsBasrHexFile(ConstLow::matInFileName.c_str(), inputPdwList);
}

void readAllPdwsBasrHexFile(const char *fileName,
		std::queue<PdwDefaultClass> &inputPdwList)
{
	inputPdwList = queue<PdwDefaultClass>();
	ifstream fileStream;
	fileStream.open(fileName, std::ifstream::in | std::ifstream::binary);
	bool validFileOpen;
	validFileOpen = fileStream.is_open();
	if (validFileOpen)
	{
		std::cout << "PDWS File: " << fileName << " Opened For Writing!"
				<< endl;
	}
	else
	{
		std::cout << "Unable To open: " << fileName << " for Writing!" << endl;
		exit(-1);
	}
	fileStream.seekg(0, std::ifstream::end);
	uint32_t fileSize = fileStream.tellg();
	fileStream.seekg(std::ifstream::beg);

	uint8_t *dataPtr = (uint8_t *) malloc(fileSize);
	fileStream.read((char*) dataPtr, fileSize);

	PdwSmall32 *pdws = (PdwSmall32*) dataPtr;

	uint32_t pdwCount = fileSize / sizeof(PdwSmall32);
	cout << "Pdw count" << pdwCount << endl;
	for (uint32_t i = 0; i < pdwCount; i++)
	{
		PdwDefaultClass tempPdw;
		tempPdw.ParseFromPdwSmall32(pdws[i]);

		scaleSinglePdwLiveData(tempPdw);
		if (tempPdw.frequencyValidFlag == 0)
			continue;
//		if (tempPdw.frequency < 7000 || tempPdw.frequency> 7200)
//			continue;
		inputPdwList.push(tempPdw);

//		printf("TOA: %llu PA:%d PB:%d PC:%d PD:%d PW:%d RF:%d RF_V:%d\n",
//				tempPdw.timeOfArrivalStart, tempPdw.powerA, tempPdw.powerB,
//				tempPdw.powerC, tempPdw.powerD, tempPdw.pulseWidth,
//				tempPdw.frequency, tempPdw.frequencyValidFlag);
//		cout << endl;
	}
	free(dataPtr);

}

void readAllPdwsBasrHexFile(const char *fileName, vector<PdwSmall32> &inputPdws)
{
	inputPdws.resize(0);
	ifstream fileStream;
	fileStream.open(fileName, std::ifstream::in | std::ifstream::binary);
	bool validFileOpen;
	validFileOpen = fileStream.is_open();
	if (validFileOpen)
	{
		std::cout << "PDWS File: " << fileName << " Opened For reading!"
				<< endl;
	}
	else
	{
		std::cout << "Unable To open: " << fileName << " for reading!" << endl;
		exit(-1);
	}
	fileStream.seekg(0, std::ifstream::end);
	uint32_t fileSize = fileStream.tellg();
	fileStream.seekg(std::ifstream::beg);

	uint8_t *dataPtr = (uint8_t *) malloc(fileSize);
	fileStream.read((char*) dataPtr, fileSize);

	PdwSmall32 *pdws = (PdwSmall32*) dataPtr;

	uint32_t pdwCount = fileSize / sizeof(PdwSmall32);
	cout << "Pdw count" << pdwCount << endl;
	inputPdws.reserve(pdwCount);
	for (uint32_t i = 0; i < pdwCount; i++)
	{
		PdwDefaultClass tempPdw;
		tempPdw.ParseFromPdwSmall32(pdws[i]);

		scaleSinglePdwLiveData(tempPdw);
		if (tempPdw.frequencyValidFlag == 0)
			continue;
		inputPdws.push_back(pdws[i]);

//		printf("TOA: %llu PA:%d PB:%d PC:%d PD:%d PW:%d RF:%d RF_V:%d\n",
//				tempPdw.timeOfArrivalStart, tempPdw.powerA, tempPdw.powerB,
//				tempPdw.powerC, tempPdw.powerD, tempPdw.pulseWidth,
//				tempPdw.frequency, tempPdw.frequencyValidFlag);
//		cout << endl;
	}
	free(dataPtr);

}

void inputInsertPdwBandBuffer(PdwDefaultClass &pdwObj,
		DataAnalysisClass &analysisObj)
{

	if (pdwObj.band == BandCodeEnum::band_cd)
	{
		bool powerThreshold = scaleApplyPowerThreshold(pdwObj,
				analysisObj.bandCD.powerThresholdSingle,
				analysisObj.bandCD.powerThresholdCombined);

		pdwObj.powerThresholdFlag = powerThreshold;
		analysisObj.bandCD.currentPdws.push_back(pdwObj);
	}
	else if (pdwObj.band == BandCodeEnum::band_k)
	{
		bool powerThreshold = scaleApplyPowerThreshold(pdwObj,
				analysisObj.bandK.powerThresholdSingle,
				analysisObj.bandK.powerThresholdCombined);

		pdwObj.powerThresholdFlag = powerThreshold;
		analysisObj.bandK.currentPdws.push_back(pdwObj);
	}
	else
	{
		switch (pdwObj.band)
		{
		case BandCodeEnum::band_e:
		case BandCodeEnum::band_g:
		case BandCodeEnum::band_i:
		case BandCodeEnum::band_j:
		case BandCodeEnum::band_ej:
		{
			if (pdwObj.frequencyValidFlag == 1)
			{
				bool powerThreshold = scaleApplyPowerThreshold(pdwObj,
						analysisObj.bandEJ.powerThresholdSingle,
						analysisObj.bandEJ.powerThresholdCombined);

				pdwObj.powerThresholdFlag = powerThreshold;
				analysisObj.bandEJ.currentPdws.push_back(pdwObj);
			}
			break;
		}
		default:
			break;
		}
	}
}

void inputGetPdwSimIteration(uint64_t startTime, uint64_t endTime,
		uint64_t mainThreadIterCount, uint8_t &dataFinishedFlag,
		queue<PdwDefaultClass>& bandInputPdwBuffer,
		DataAnalysisClass &analysisObj)
{
	PdwDefaultClass tempPdw; //temporary data holder

	if (ConstLow::simIterationTypeFlag
			== LowIterationTypeFlags::simulationModeDisabled)
	{
		return;
	}
	if (Constant::configureToReadPdwsFromFileFlag == 0)
	{
		return;
	}

//	startTime = startTime * ConstLow::toaCountInMicros; //this was previously calculated outside
//	endTime = endTime * ConstLow::toaCountInMicros; //this was previously calculated outside
	if (bandInputPdwBuffer.empty() == false)
	{
		int64_t ignoredPdws = 0; //just for a log

		while (true)
		{
			tempPdw = bandInputPdwBuffer.front();

			if (tempPdw.timeOfArrivalStart < startTime)
			{
				bandInputPdwBuffer.pop();
				ignoredPdws++;
			}
			else if (tempPdw.timeOfArrivalStart <= endTime)
			{
				bandInputPdwBuffer.pop();
				inputInsertPdwBandBuffer(tempPdw, analysisObj);
			}
			else
			{
				break;
			}
			if (bandInputPdwBuffer.empty() == true)
			{
				dataFinishedFlag = 1;
				break;
			}
		}
	}
	else
	{
		dataFinishedFlag = 1;
	}
	if (ConstLow::logOverallFlag == true)
	{
		cout << "PDW count EJ: " << analysisObj.bandEJ.currentPdws.size()
				<< " CD: " << analysisObj.bandCD.currentPdws.size() << " K: "
				<< analysisObj.bandK.currentPdws.size() << endl;
		cout << "System Iteration :" << mainThreadIterCount
				<< " Remaining PDWs : " << bandInputPdwBuffer.size() << endl;
	}
}
void inputGetPdwLiveIteration(uint8_t &dataFinishedFlag,
		LivePdwReceiver & pdwReceiver, DataAnalysisClass &analysisObj)
{

//	uint8_t arr[16]={0xaa,0xbb,0xcc,0xdd,0xee,0xff,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0xa5,0xb5,0xc5,0xd5};
//	PdwDefaultClass pdw;
//	pdwReceiver.extractPdwFromRawData(arr,pdw);

	if (ConstLow::simIterationTypeFlag
			!= LowIterationTypeFlags::simulationModeDisabled)
	{
		return;
	}
//	uint64_t readStartTime = Utility::getSystemTime();
	vector<PdwSmall32> & recPdws = pdwReceiver.getReceivedPdws();
//	uint64_t readEndTime = Utility::getSystemTime();
//	if (recPdws.size() > 0)
//		analysisObj.pdwDequeTimeBenchmark += (readEndTime - readStartTime);

	PdwDefaultClass convertedPdw;
	for (UWord16 i = 0; i < recPdws.size(); i++)
	{
		convertedPdw.ParseFromPdwSmall32(recPdws[i]);
		scaleSinglePdwLiveData(convertedPdw);
//		if(convertedPdw.intraPulse != IntraPulseType::UMOP)
//		{
//
//			cout << "LPI "<< (int)convertedPdw.intraPulse<<endl;
//		}
		inputInsertPdwBandBuffer(convertedPdw, analysisObj);
	}
	if (recPdws.size() == 0)
	{
		usleep(10);
	}
//	if(recPdws.size()>0)
//	{
//		if(recPdws.back().timeOfArrivalStart > 300*100000000l)
//		{
//			cout << "400 seconds passed\nExiting the DSP program"<<endl;
//			exit(0);
//		}
//	}

//	if (recPdws.size() > 0)
//	{
//		cout << "PDW count E: " << analysisObj.bandE.currentPdws.size()
//				<< " G: " << analysisObj.bandG.currentPdws.size() << " I: "
//				<< analysisObj.bandI.currentPdws.size() << " J: "
//				<< analysisObj.bandJ.currentPdws.size() << endl;
//	}

}

void inputGetPdwLiveIteration(uint8_t &dataFinishedFlag,
		LivePdwReceiver & pdwReceiver, PlaybackPdwProvider& playbackData,
		DataAnalysisClass &analysisObj)
{

//	if (ConstLow::simIterationTypeFlag
//			!= LowIterationTypeFlags::simulationModeDisabled)
//	{
//		return;
//	}
	vector<PdwSmall32> & recPdws = pdwReceiver.getReceivedPdws();
	vector<PdwSmall32> & recPdws2 = playbackData.getReceivedPdws();

	if (recPdws.size() == 0 && recPdws2.size() == 0)
	{
		usleep(10);
		return;
	}

	vector<PdwSmall32>& recPdwsMerged = recPdws2;
//	vector<PdwSmall32>& recPdwsMerged = pdwReceiver.tempPdwsssss;
//	recPdwsMerged.resize(recPdws.size() + recPdws2.size());
//	std::merge(recPdws.begin(), recPdws.end(), recPdws2.begin(), recPdws2.end(),
//			recPdwsMerged.begin(), PdwSmall32::toaComparison);

	PdwDefaultClass convertedPdw;
	for (uint32_t i = 0; i < recPdwsMerged.size(); i++)
	{
		convertedPdw.ParseFromPdwSmall32(recPdwsMerged[i]);
		scaleSinglePdwLiveData(convertedPdw);
		inputInsertPdwBandBuffer(convertedPdw, analysisObj);
	}

//	if (recPdws2.size() > 0)
//	{
//		usleep(20000);
//		return;
//	}
}
void inputClearIterationBandPdws(vector<PdwDefaultClass> &currentPdws)
{

	currentPdws.resize(0);
}

//review use of toa time reference variable for expiry of pdw
void inputExpireIterationBandPdws(vector<PdwDefaultClass> &currentPdws,
		uint64_t &timeToaReference)
{
	//we delete pulses that haven't been clustered for given
	//constant time use toa iteration reference time (first
	//or last) to implement this feature
	uint64_t toaDifference = 0;

	std::vector<PdwDefaultClass>::iterator it;
	it = currentPdws.begin();
	while (it != currentPdws.end())
	{

		//take time difference only if this pdw is old
		if (it->timeOfArrivalStart < timeToaReference)
		{
			toaDifference = abs(
					int64_t(timeToaReference - it->timeOfArrivalStart));
		}
		else
		{
			toaDifference = 0;
		}
		//if the pdw is older than 3 seconds, we delete it
		if (toaDifference > ConstLow::toaCountInSecond)
		{
			cout << "exp: " << ((uint16_t) it->bandRaw) << endl;
			it = currentPdws.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void inputCleanPdwSimData(queue<PdwDefaultClass>& inputPdwList)
{

	uint64_t deletionToaLower = 0;
	uint64_t deletionToaUpper = 0;
	uint64_t lowerBound = 0;
	uint64_t upperBound = 0;
	uint64_t currentIndex = 0;
	uint64_t diffToa = 0;
	vector<PdwDefaultClass> pdws;
	pdws.resize(inputPdwList.size());
	for (uint32_t i = 0; i < pdws.size(); i++)
	{
		pdws[i] = inputPdwList.front();
		inputPdwList.pop();
	}
	inputPdwList = queue<PdwDefaultClass>();

	if (ConstLow::simIterationTypeFlag
			== LowIterationTypeFlags::simulationModeDisabled)
	{
		return;
	}
	std::sort(pdws.begin(), pdws.end(), PdwDefaultClass::cmpToaStart);
	if (ConstLow::cleanPdwSimInputFlag == 0)
	{
		return;
	}
	if (pdws.size() == 0)
	{
		return;
	}
	LogLowClass::logCleanInputPdwStart();

	uint32_t lowerIndex = 0;
	uint32_t upperIndex = pdws.size();

	for (uint32_t i = 1; i < (pdws.size() / 2); i++)
	{
		uint64_t diffToa = pdws[i].timeOfArrivalStart
				- pdws[i - 1].timeOfArrivalStart;
		if (diffToa > ConstLow::toaCountInTenMin)
		{
			lowerIndex = i;
		}
	}
	uint32_t startPoint = (lowerIndex == 0 ? (pdws.size() / 2) : lowerIndex);
	uint32_t endPoint = pdws.size();

	for (uint32_t i = startPoint; i < endPoint; i++)
	{
		uint64_t diffToa = pdws[i].timeOfArrivalStart
				- pdws[i - 1].timeOfArrivalStart;
		if (diffToa > ConstLow::toaCountInTenMin)
		{
			upperIndex = i;
			break;
		}
	}

	for (uint32_t i = lowerIndex; i < upperIndex; i++)
	{
		inputPdwList.push(pdws[i]);
	}
	//erase the actual elements
	//testing purposes
	//deletionToaLower = 45875035529;
	//end of testing
	LogLowClass::logEraseBoundPdw(pdws[lowerIndex].timeOfArrivalStart,
			pdws[upperIndex].timeOfArrivalStart);

}

void inputLimitPdwSimData(queue<PdwDefaultClass>& inputPdwList)
{
	uint64_t toaLowerLimit = 0;
	uint64_t toaUpperLimit = 0;

	if (ConstLow::simTimeLimitFlag == 0)
	{
		return;
	}
	if (inputPdwList.size() == 0)
	{
		return;
	}
	//find lower limit toa
	if (ConstLow::simTimeStartToa > 0)
	{
		toaLowerLimit = ConstLow::simTimeStartToa;
	}
	else if (ConstLow::simTimeStartOffsetMs > 0)
	{
		toaLowerLimit = inputPdwList.front().timeOfArrivalStart
				+ (ConstLow::toaCountInMs * ConstLow::simTimeStartOffsetMs);
	}
	else
	{
		toaLowerLimit = inputPdwList.front().timeOfArrivalStart;
	}

	//find upper limit toa
	if (ConstLow::simTimeEndToa > 0)
	{
		toaUpperLimit = ConstLow::simTimeEndToa;
	}
	else if (ConstLow::simTimeEndOffsetMs > 0)
	{
		toaUpperLimit = toaLowerLimit
				+ (ConstLow::toaCountInMs * ConstLow::simTimeEndOffsetMs);
	}
	else
	{
		toaUpperLimit = inputPdwList.back().timeOfArrivalStart;
	}

	vector<PdwDefaultClass> pdws;
	pdws.resize(inputPdwList.size());
	for (uint32_t i = 0; i < pdws.size(); i++)
	{
		pdws[i] = inputPdwList.front();
		inputPdwList.pop();
	}
	inputPdwList = queue<PdwDefaultClass>();

	vector<PdwDefaultClass>::iterator it = pdws.begin();
	while (it != pdws.end())
	{
		if (((it->timeOfArrivalStart < toaLowerLimit) && (toaLowerLimit != 0))
				|| ((it->timeOfArrivalStart > toaUpperLimit)
						&& (toaUpperLimit != 0)))
		{

		}
		else
		{
			inputPdwList.push(*it);
		}
		it++;
	}

	LogLowClass::logLimitToaStart(toaLowerLimit,
			inputPdwList.front().timeOfArrivalStart);
	LogLowClass::logLimitToaEnd(toaUpperLimit,
			inputPdwList.back().timeOfArrivalStart);
	LogLowClass::logLimitToaTotal(inputPdwList.size());
}

void inputMountHardwareStorage()
{
	if (ConstLow::simOnActualHwFlag == 1)
	{
		mountUsbDrive();
	}
}

