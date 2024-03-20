/*
 * dataanalysisclass.h
 *
 *  Created on: Oct 18, 2018
 *      Author: hassan
 */

#ifndef DATAANALYSISCLASS_H_
#define DATAANALYSISCLASS_H_

#include "../common/common.h"
#include "../vo/datasensorclass.h"
#include "../vo/pdwdefaultclass.h"
#include "../io/serialbuffer.h"
#include "../io/systemtime.h"


class DataAnalysisClass {
public:
	DataSensorClass bandAll;
	
	DataSensorClass bandL;
	
	DataSensorClass bandE;
	DataSensorClass bandG;
	DataSensorClass bandI;
	DataSensorClass bandJ;
	
	DataSensorClass bandCD;
	DataSensorClass bandEG;
	DataSensorClass bandIJ;
	DataSensorClass bandEJ;
	
	DataSensorClass bandK;

	uint64_t lastReportingTime;
	uint64_t lastReportingSystemIter;
	uint64_t pdwDequeTimeBenchmark;
	uint32_t RSSCRC;
	uint32_t ethernetThreats;
	uint32_t failureIDs;

	DataAnalysisClass();
	virtual ~DataAnalysisClass();
	void serialize(SerialBufferClass & buffer);
};

#endif /* DATAANALYSISCLASS_H_ */
