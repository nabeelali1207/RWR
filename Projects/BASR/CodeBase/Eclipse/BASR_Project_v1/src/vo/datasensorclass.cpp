/*
 * datasensorclass.cpp
 *
 *  Created on: Jan 1, 2019
 *      Author: hassan
 */

#define DATASENSORCLASS_CPP_

#include "../vo/datasensorclass.h"

DataSensorClass::DataSensorClass()
{
	// TODO Auto-generated constructor stub
	//band = BandCodeEnum::band_uncomputed;
	this->timeToaReference = 0;
	this->toaOfLastProcessedPdw = 0;
	this->trackToaLastMax = 0;
	this->powerThresholdSingle = 0;
	this->powerThresholdCombined = 0;
	this->powerThresholdClustering = 0;

	newPulsesInClusteringBufferCount = 0;
	stringableTrackCount = 0;

	this->compareHighHigh.resize(0, 0);
	this->compareHighLow.resize(0, 0);
	this->compareLowLow.resize(0, 0);
}

DataSensorClass::~DataSensorClass()
{
	// TODO Auto-generated destructor stub
}

void DataSensorClass::serialize(SerialBufferClass & buffer)
{
	//buffer.serialize(band);
	buffer.serialize(tracks, 0);

	buffer.serialize(timeToaReference);

}

