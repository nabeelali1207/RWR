/*
 * analysisclusterdelegate.h
 *
 *  Created on: Jan 6, 2019
 *      Author: hassan
 */

#ifndef ANALYSISCLUSTERDELEGATE_H_
#define ANALYSISCLUSTERDELEGATE_H_

#include "../common/common.h"
#include "../vo/loglowclass.h"
#include "../utility/constant.h"
#include "../utility/utility.h"
#include "../model/model.h"
#include "../io/systemtime.h"
#include "../delegate/analysisdbscandelegate.h"
#include "../delegate/analysistracklowdelegate.h"
#include "../delegate/analysispowerdelegate.h"

//notes
//Do we track pulses in next iteration that failed clustering?
//end of notes

//All definitions
void clusterAnalysis(vector<PdwDefaultClass> &clusterPdws,
		vector<TrackDefaultClass> &clusterTracks, uint32_t &newPulsesCount,
		DBSCAN &dbScanObj, uint64_t currSystemIteration,
		uint64_t *benchmarkPtr);
//we can choose to accumulate pulses that fail to cluster for a second round?
void clusterCreateTracks(vector<TrackDefaultClass> &clusterTracks,
		vector<int16_t> &clusterPdwsLabels,
		vector<PdwDefaultClass> &clusterPdws, uint64_t currSystemIteration);
void clusterCheckSelfTestFlagOfTracks(vector<TrackDefaultClass> &clusterTracks);
void clusterCheckValidClusters(vector<TrackDefaultClass> &clusterTracks,
		vector<int16_t> &clusterPdwsLabels,
		vector<PdwDefaultClass> &clusterPdws);
void clusterProcessInputPdw(list<PdwDefaultClass> &currentPdws,
		vector<PdwDefaultClass> &clusterPdws);
void clusterExcludeCriteriaPdw(list<PdwDefaultClass> &currentPdws);
void clusterRemoveClusteredPdwsFromBuffer(vector<int16_t> &clusterPdwsLabels,
		vector<PdwDefaultClass> &clusterPdws);
void clusterGetUnclusteredPulsesBack(vector<PdwDefaultClass> &clusterPdws,
		vector<PdwDefaultClass> &iterationPdws);
void clusterRemoveTrackedPulsesFromClusteringBuffer(
		vector<PdwDefaultClass> &clusterPdws,
		vector<PdwDefaultClass> &iterationPdws);

void clusterInitiateNewTracks(vector<TrackDefaultClass> &tracks,
		vector<TrackDefaultClass> &clusterTracks, uint64_t currSystemIteration,
		uint32_t &stringableTrackCount);
void clusterSortTrackPdw(vector<TrackDefaultClass> &tracks);
void clusterPopulateInputPdw(vector<PdwDefaultClass> &currentPdws,
		vector<PdwDefaultClass> &clusterPdws, uint32_t &newPulsesCount,
		uint16_t clusteringPowerThreshold);

#ifdef ANALYSISCLUSTERDELEGATE_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion

#endif /* ANALYSISCLUSTERDELEGATE_H_ */
