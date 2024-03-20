/*
 * datasensorclass.h
 *
 *  Created on: Jan 1, 2019
 *      Author: hassan
 */

#ifndef DATASENSORCLASS_H_
#define DATASENSORCLASS_H_

#include "../common/common.h"
#include "../vo/dataenvironmentclass.h"
#include "../vo/trackdefaultclass.h"
#include "../vo/trackcompareclass.h"
#include "../vo/pdwdefaultclass.h"
#include "../io/serialbuffer.h"
#include "../delegate/analysisdbscandelegate.h"
class DataSensorClass {
public:
	//BandCodeEnum band;
	//create tracks and candidate tracks structures
	vector <PdwDefaultClass> currentPdws;
	vector <PdwDefaultClass> clusterPdws;
	vector <TrackDefaultClass> clusterTracks;
	//rollbuffer needed? or we just accumulate un-tracked pdw into clusterbuff
	list <PdwDefaultClass> rollPdws;
	vector <TrackDefaultClass> tracks;
	uint64_t timeToaReference;//maybe redundant
	uint64_t toaOfLastProcessedPdw;

	//low side variables
	uint16_t powerThresholdSingle;
	uint16_t powerThresholdCombined;
	uint16_t powerThresholdClustering;
	DBSCAN dbScanObj;
	uint32_t newPulsesInClusteringBufferCount;
	uint32_t stringableTrackCount;
	//stringing structures, set default size pending in constructor
	vector <DataEnvironmentClass> environmentParameterHistory;
	vector <DataEnvironmentClass> environmentRangeHistory;
	vector <uint16_t> tracksIndexSorted;//sorted by first toa
	boost::numeric::ublas::matrix<TrackCompareClass> compareHighHigh;
	boost::numeric::ublas::matrix<TrackCompareClass> compareHighLow;
	boost::numeric::ublas::matrix<TrackCompareClass> compareLowLow;
	//potential stringing variables
	uint64_t trackToaLastMax;

	DataSensorClass();
	virtual ~DataSensorClass();
	void serialize(SerialBufferClass & buffer);
};

#endif /* DATASENSORCLASS_H_ */
