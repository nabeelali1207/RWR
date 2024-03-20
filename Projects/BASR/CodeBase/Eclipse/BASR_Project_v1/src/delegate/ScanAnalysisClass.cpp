/*
 * analysisscanlowdelegate.cpp
 *
 *  Created on: Sep 11, 2020
 *      Author: tehseen
 */

#include "ScanAnalysisClass.h"

ScanAnalysisClass::ScanAnalysisClass()
{
	// TODO Auto-generated constructor stub
	this->scanPdwBuffer.set_capacity(10000);
	this->pulseCount = 0;
	this->currMode = EmitterMode::unknown;
	this->buffersincelastReport = 0;
	this->bufferMaxPower=0;
}

ScanAnalysisClass::~ScanAnalysisClass()
{
	// TODO Auto-generated destructor stub
}

void ScanAnalysisClass::FillScanBuffer(PdwDefaultClass &pdw)
{
	scanPdwBuffer.push_back(pdw);
	pulseCount++;
}


void ScanAnalysisClass::SetMaxPower(uint16_t maxPower)
{
	if(buffersincelastReport>0)
	{
		bufferMaxPower=max(bufferMaxPower,maxPower);
	}
	else
	{
		bufferMaxPower=maxPower;
	}
}

void ScanAnalysisClass::ScanAnalysis(uint8_t dominantAntenna)
{
	int32_t bufferSize = scanPdwBuffer.size();
	uint32_t startIndex = max(0, bufferSize - pulseCount);
	uint32_t endIndex = bufferSize;
	uint16_t powers[4];
	int8_t numberOfCycles = 0;
	int32_t numberOfValidLvls = 0;
	vector<float> probOfValidLvl(10);
//	int16_t ScanPulses[4096];

	int8_t lastPulseLvl = -1, currPulseLvl;
	powerLvlsPattern.resize(0);
	scanHistogram.clearHistogram();

	for (uint32_t i = startIndex, j = 0; i < endIndex; i++, j++)
	{
		PdwDefaultClass &pdw = scanPdwBuffer[i];
		powers[0] = pdw.powerA;
		powers[1] = pdw.powerB;
		powers[2] = pdw.powerC;
		powers[3] = pdw.powerD;
		float normalizedPower = round(
				((float) powers[dominantAntenna] / bufferMaxPower)
						* ScanAnalysisHistogram::SCAN_HIST_SIZE) - 1;

		currPulseLvl = normalizedPower;
//		ScanPulses[j] = powers[dominantAntenna];
		if (currPulseLvl < ScanAnalysisHistogram::SCAN_HIST_SIZE
				&& currPulseLvl > -1)
		{
			scanHistogram.addToHistogram(currPulseLvl);
			if (currPulseLvl != lastPulseLvl)
			{
				powerLvlsPattern.push_back(currPulseLvl);

			}
			lastPulseLvl = currPulseLvl;
		}
	}
	scanHistogram.findLevels();
	int32_t numberOfLvls = scanHistogram.getNumberOfLevelsFound();
	int16_t levelsCountPerCycle[10];
	int8_t levelsRepetitionCount = 0;
	if (numberOfLvls >= 5)
	{
		for (uint32_t i = 0; i < powerLvlsPattern.size(); i++)
		{
			if (powerLvlsPattern[i] == 9)
			{
				if (numberOfCycles == 0)
				{
					numberOfCycles++;
				}
				else if (levelsRepetitionCount >= 3)
				{
					numberOfCycles++;
				}
				memset(levelsCountPerCycle, 0, sizeof(levelsCountPerCycle));
				levelsRepetitionCount =0;
			}
			levelsCountPerCycle[powerLvlsPattern[i]]++;
			if (levelsCountPerCycle[powerLvlsPattern[i]] == 2)
				levelsRepetitionCount++;
		}
	}
	if (numberOfCycles > 0 && numberOfLvls >= 5)
	{
		CreateConcMatrix();

		/* Find probability of a valid level and find no. of valid levels based on their probability */
		for (uint16_t rowIndex = 0; rowIndex < numberOfLvls; rowIndex++) //first row and column in concurrence matrix is for unknown value
		{
			uint16_t levelValue = scanHistogram.levelsFound[rowIndex].levelValue;
			if (levelValue == 0)
			{
				probOfValidLvl[levelValue] =
						(float) ConcMatrix[levelValue][levelValue + 1]
								/ numberOfCycles;
			}
			else if (levelValue == 9)
			{
				probOfValidLvl[levelValue] =
						(float) ConcMatrix[levelValue][levelValue - 1]
								/ numberOfCycles;
			}
			else
			{
				probOfValidLvl[levelValue] =
						(float) (ConcMatrix[levelValue][levelValue + 1]
								+ ConcMatrix[levelValue][levelValue - 1])
								/ (2 * numberOfCycles);
			}
			if (probOfValidLvl[levelValue] > 0.7 && probOfValidLvl[levelValue] <= 2)
				numberOfValidLvls++;
		}
	}

	if (numberOfValidLvls >= 3)
		currMode = EmitterMode::scan;
	else if(numberOfLvls < 5)
	{
		if(pulseCount > 100)
			currMode = EmitterMode::track;
		else
			currMode = EmitterMode::unknown;
	}
	else currMode = EmitterMode::unknown;

	buffersincelastReport++;
	if (buffersincelastReport >= 2)
	{
		ClearData();
		buffersincelastReport =0;
	}
}
void ScanAnalysisClass::CreateConcMatrix()
{
	memset(ConcMatrix, 0, sizeof(ConcMatrix)); // 2D matrix
	for (uint16_t i = 0; i < powerLvlsPattern.size() - 1; i++) // scan the whole pri buffer
	{
		ConcMatrix[powerLvlsPattern[i]][powerLvlsPattern[i + 1]]++; // add one to label to cater -1 i.e. unknown label
	}
}
EmitterMode ScanAnalysisClass::GetEmitterMode()
{
	return currMode;
}
EmitterMode ScanAnalysisClass::DetectScanType()
{
	return currMode;
}

ScanAnalysisClass::ScanAnalysisClass(const ScanAnalysisClass &other)
{
	(*this) = other;
}

void ScanAnalysisClass::operator =(const ScanAnalysisClass &other)
{
	currMode = other.currMode;
	scanHistogram = other.scanHistogram;
	scanPdwBuffer = other.scanPdwBuffer;
	pulseCount = other.pulseCount;
	powerLvlsPattern = other.powerLvlsPattern;
	buffersincelastReport = other.buffersincelastReport;
	bufferMaxPower = other.bufferMaxPower;
	memcpy(ConcMatrix, other.ConcMatrix, sizeof(ConcMatrix));
}

void ScanAnalysisClass::ClearData()
{
	pulseCount = 0;
	bufferMaxPower=0;
	//scanHistogram.clearHistogram();
	//ConcMatrix.resize(0);
}







void IlluminationCycleInfo::takeUnion(IlluminationCycleInfo&other)
{
	//if(cycleDuration<other.cycleDuration)
	{
		uint32_t totalPulseCount = cyclePulseCount + other.cyclePulseCount;
		uint32_t oldPercent = (128*cyclePulseCount)/(uint32_t)totalPulseCount;
		uint32_t newPercent = (128*other.cyclePulseCount)/(uint32_t)totalPulseCount;
		uint32_t newMean = (oldPercent*meanPower)+(newPercent*other.meanPower);
		uint32_t newVar = (oldPercent*variance)+(newPercent*other.variance);
		uint32_t newK = (oldPercent*kurtosis)+(newPercent*other.kurtosis);
		meanPower = newMean/128;
		variance = newVar/128;
		kurtosis = newK/128;

		endPower = other.endPower;

	}
	cycleDuration = max(cycleStartTime+cycleDuration,other.cycleStartTime+other.cycleDuration);
	cycleStartTime = min(cycleStartTime,other.cycleStartTime);
	cycleDuration = cycleDuration - cycleStartTime;
	cyclePulseCount +=other.cyclePulseCount;
	if(minPower>other.minPower)
	{
		minPower = other.minPower;
		minPowerTime = other.minPowerTime;
	}
	if(maxPower<other.maxPower)
	{
		maxPower = other.maxPower;
		maxPowerTime = other.maxPowerTime;
	}

}
void IlluminationCycleInfo::takeUnion(circular_buffer<IlluminationCycleInfo> &first,circular_buffer<IlluminationCycleInfo>&other)
{
	for(uint32_t i=0;i<other.size();i++)
	{
		IlluminationCycleInfo& curr=other[i];
		uint64_t mp1 =curr.cycleStartTime + curr.cycleDuration/2;
		bool merged=false;
		for(uint32_t j=first.size();j>0;j--)
		{
			IlluminationCycleInfo& old=first[j-1];
			uint64_t range =curr.cycleDuration/2 + old.cycleDuration/2 + 200000;
			uint64_t mp2 =old.cycleStartTime + old.cycleDuration/2;
			if((max(mp1,mp2)-min(mp1,mp2)) < range)
			{// to check if 2 ranges overlap
				old.takeUnion(curr);
				merged =true;
				break;
			}
		}
		if(merged==false)
		{
			first.push_back(curr);
		}
	}

}


ScanAnalysisClassV2::ScanAnalysisClassV2()
{
	illuminationCycleInfo.reserve(10);

	accumulatedPowerCount = 0;
	accumulatedPower = 0;
	accumulatedPowerIndex=0;
}

ScanAnalysisClassV2::ScanAnalysisClassV2(const ScanAnalysisClassV2& other)
{
	*this=other;
}

void ScanAnalysisClassV2::operator=(const ScanAnalysisClassV2&other)
{
	currMode = other.currMode;
	memcpy(powerLvls, other.powerLvls, sizeof(powerLvls));
	toaOffset = other.toaOffset;
	accumulatedPower = other.accumulatedPower;
	accumulatedPowerCount = other.accumulatedPowerCount;
	accumulatedPowerIndex = other.accumulatedPowerIndex;
}

void ScanAnalysisClassV2::addFirstPdw(PdwDefaultClass &pdw)
{
	clearAll();
	uint64_t toaMask = -1;
	toaMask = toaMask ^ ((1<<TIME_DOWN_SHIFT)-1);
	toaOffset = pdw.timeOfArrivalStart & (toaMask);
	accumulatedPower = 0;
	accumulatedPowerCount = 0;
	accumulatedPowerIndex = 0;

	addNextPdw(pdw);
}
void ScanAnalysisClassV2::addNextPdw(PdwDefaultClass &pdw)
{
	uint64_t myBinIndex = (pdw.timeOfArrivalStart-toaOffset)>>TIME_DOWN_SHIFT;

	if(myBinIndex<accumulatedPowerIndex)
		return;
	else if(myBinIndex==accumulatedPowerIndex)
	{
		accumulatedPowerCount++;
		accumulatedPower=accumulatedPower + (uint32_t)((pdw.powerX>>1) + (pdw.powerY>>1));
	}
	else if (myBinIndex<(BUFF_ENTRY_COUNT-50))
	{//index number changes

		uint32_t avgPowerOfBin = (accumulatedPowerCount==0?0:accumulatedPower/accumulatedPowerCount);
		powerLvls[accumulatedPowerIndex].power= (avgPowerOfBin>>POWER_DOWN_SHIFT); //maps the range [0,4096) to [0,256)
		powerLvls[accumulatedPowerIndex].pulseCount = (accumulatedPowerCount>255?255:accumulatedPowerCount);
		if((myBinIndex-accumulatedPowerIndex)<10)
		{
			for(uint32_t i=accumulatedPowerIndex+1;i<myBinIndex;i++)
			{
				powerLvls[i].power=powerLvls[accumulatedPowerIndex].power;
				powerLvls[i].pulseCount=0;
			}
		}
		else
		{
			for(uint32_t i=accumulatedPowerIndex+1;i<myBinIndex;i++)
			{
				powerLvls[i].power=0;
				powerLvls[i].pulseCount=0;
			}
		}

		accumulatedPower = (uint32_t)((pdw.powerX>>1) + (pdw.powerY>>1));
		accumulatedPowerCount = 1;
		accumulatedPowerIndex = myBinIndex;
		powerLvls[accumulatedPowerIndex].power= 0; //maps the range [0,4096) to [0,256)
		powerLvls[accumulatedPowerIndex].pulseCount = 0;
	}
	else if (myBinIndex>=BUFF_ENTRY_COUNT)
	{
		makeSpaceForNewIncomingData();
	}
}
void ScanAnalysisClassV2::performScanAnalysis()
{
	//int i=0;

	//detect illumination times
	//detect connected pieces of power recv

	illuminationCycleInfo.resize(0);

	uint64_t startTime = toaOffset;
	uint32_t validIndexCount = (accumulatedPowerIndex<BUFF_ENTRY_COUNT?accumulatedPowerIndex:0);

	int16_t illuminationStartPoint=-1;
	uint32_t accPulseCount =0;
	IlluminationCycleInfo tempInfo1;
	for(uint32_t i=0;i<validIndexCount;i++)
	{
		uint8_t currPower = powerLvls[i].power;
		if(currPower>20)
		{
			if(illuminationStartPoint==-1)
			{//find start point where power crosses the threshold
				illuminationStartPoint = i;
				accPulseCount = powerLvls[i].pulseCount;
				continue;
			}
			accPulseCount += powerLvls[i].pulseCount;
		}
		else if(illuminationStartPoint!=-1  && (i-illuminationStartPoint)>3)
		{//find end point where power falls below the threshold
			int16_t illuminationEndPoint=i-1;
			tempInfo1.cycleStartTime = illuminationStartPoint;
			tempInfo1.cycleDuration=illuminationEndPoint;
			tempInfo1.cyclePulseCount = accPulseCount;
			if(tempInfo1.cyclePulseCount>8)
			{
				illuminationCycleInfo.push_back(tempInfo1);
			}
			//reset and search ahead
			illuminationStartPoint = -1;
		}
	}

	if(illuminationStartPoint!=-1 && (validIndexCount-illuminationStartPoint)>3 ) //last iteration repeated of above loop
	{//find end point where power falls below the threshold
		int16_t illuminationEndPoint=validIndexCount;
		tempInfo1.cycleStartTime = illuminationStartPoint;
		tempInfo1.cycleDuration=illuminationEndPoint;
		tempInfo1.cyclePulseCount = accPulseCount;
		if(tempInfo1.cyclePulseCount>8)
		{
			illuminationCycleInfo.push_back(tempInfo1);
		}
	}

	for(uint32_t i=0;i<illuminationCycleInfo.size();i++)
	{

		IlluminationCycleInfo& tempInfo=illuminationCycleInfo[i];
		tempInfo.variance =0;
		uint32_t sp=tempInfo.cycleStartTime;
		uint32_t ep=tempInfo.cycleDuration;
		uint32_t dur=(ep-sp);
		tempInfo.cycleStartTime = startTime + (sp<<TIME_DOWN_SHIFT);
		tempInfo.cycleDuration = dur<<TIME_DOWN_SHIFT;
		uint16_t illuminationMaxPower=powerLvls[sp].power;
		uint16_t illuminationMinPower=powerLvls[sp].power;
		uint32_t minPowerTime = sp;
		uint32_t maxPowerTime = sp;
		uint32_t peakPowerDuration = 0;

		uint32_t accumPower = 0;
		for(uint32_t j=sp;j<ep;j++)
		{
			uint8_t currPower = powerLvls[j].power;
			accumPower += currPower;
			if(illuminationMinPower>currPower)
			{
				illuminationMinPower = currPower;
				minPowerTime = j;
			}
			if(illuminationMaxPower<currPower)
			{
				illuminationMaxPower = currPower;
				maxPowerTime = j;
			}
		}
		for(uint32_t j=sp;j<ep;j++)
		{
			uint8_t currPower = powerLvls[j].power;

			if(abs(illuminationMaxPower-currPower) < 8)
			{
				peakPowerDuration++;
			}
		}
		peakPowerDuration = 100*peakPowerDuration/(ep-sp);
		uint32_t mean = (accumPower<<POWER_DOWN_SHIFT) / dur;
		tempInfo.startPower = powerLvls[sp].power<<POWER_DOWN_SHIFT;
		tempInfo.endPower = powerLvls[ep-1].power<<POWER_DOWN_SHIFT;
		tempInfo.maxPower = illuminationMaxPower<<POWER_DOWN_SHIFT;
		tempInfo.minPower = illuminationMinPower<<POWER_DOWN_SHIFT;
		tempInfo.minPowerTime = (minPowerTime<<TIME_DOWN_SHIFT);
		tempInfo.maxPowerTime = (maxPowerTime<<TIME_DOWN_SHIFT);
		tempInfo.meanPower = mean;
		tempInfo.peakPowerDuration = peakPowerDuration;


		int16_t mean2 = mean>>POWER_DOWN_SHIFT;
		uint64_t secondMomentAcc = 0;
		uint64_t fourthMomentAcc = 0;
		for(uint32_t j=sp;j<ep;j++)
		{
			int16_t currPower = powerLvls[j].power;
			int32_t diff = abs(currPower-mean2);
			uint32_t moment2= (diff*diff);
			uint32_t moment4= (moment2*moment2);
			secondMomentAcc+=moment2;
			fourthMomentAcc+=moment4;
		}
		secondMomentAcc=(secondMomentAcc)/dur;
		tempInfo.variance = secondMomentAcc;
		fourthMomentAcc=(100*fourthMomentAcc)/dur;
		uint64_t k = (secondMomentAcc==0?0:fourthMomentAcc/(secondMomentAcc*secondMomentAcc));
		tempInfo.kurtosis = k;

	}


	makeSpaceForNewIncomingData();
}

void ScanAnalysisClassV2::clearAll()
{
	memset(powerLvls,0,sizeof(powerLvls));
}
void ScanAnalysisClassV2::makeSpaceForNewIncomingData()
{
	if(accumulatedPowerIndex<250)
		return;
	uint32_t binsCountToRetain = 140;

	uint32_t spaceToFreeUp = accumulatedPowerIndex-binsCountToRetain;
	uint32_t sp = spaceToFreeUp;
	memcpy(&powerLvls[0],&powerLvls[sp],sizeof(PowerHistBin)*(binsCountToRetain+1));
	uint64_t toaOffsetIncrement = (spaceToFreeUp << TIME_DOWN_SHIFT);
	toaOffset +=toaOffsetIncrement;
	accumulatedPowerIndex = accumulatedPowerIndex - spaceToFreeUp;
}
