/*
 * dataanalysisclass.cpp
 *
 *  Created on: Oct 18, 2018
 *      Author: hassan
 */

#define DATAANALYSISCLASS_CPP_

#include "../vo/dataanalysisclass.h"

DataAnalysisClass::DataAnalysisClass()
{
	// TODO Auto-generated constructor stub
	lastReportingTime=0;
	lastReportingSystemIter=0;
	pdwDequeTimeBenchmark=0;

	RSSCRC=0xffffffff;
	ethernetThreats=0;
	failureIDs=0;
}

DataAnalysisClass::~DataAnalysisClass()
{
	// TODO Auto-generated destructor stub
}

void DataAnalysisClass::serialize(SerialBufferClass & buffer)
{

	buffer.serialize(bandAll,0);
	buffer.serialize(bandCD,0);
	buffer.serialize(bandEJ,0);
	buffer.serialize(bandK,0);

	buffer.serialize(RSSCRC,0);
	buffer.serialize(ethernetThreats,0);
	buffer.serialize(failureIDs,0);
}
