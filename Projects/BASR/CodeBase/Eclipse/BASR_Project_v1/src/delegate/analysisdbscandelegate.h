/*
 * analysisdbscandelegate.h
 *
 *  Created on: Nov 16, 2018
 *      Author: afzal
 */

#ifndef ANALYSISDBSCANDELEGATE_H_
#define ANALYSISDBSCANDELEGATE_H_

#include "../common/common.h"
#include "../vo/pdwdefaultclass.h"
#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/algorithm/minmax.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <vector>

#include "../utility/constlow.h"
#include "../vo/loglowclass.h"

//using namespace boost::numeric;

class DBSCAN
{
public:
	DBSCAN()
	{
		epsilon = ConstLow::asClusterDbScanEpsilon;
		minPointsToDeclareCorePoint = ConstLow::asClusterDbScanMinNeighborCount;
		featuresCount = ConstLow::asClusterDbScanDimensionCount;
		minPulsesToDeclareCluster = ConstLow::asClusterMinPulsesToDeclareTrack;
		inputSamplesCount = 0;
	}

	uint32_t clusterInputPdws(vector<PdwDefaultClass>&,uint64_t *benchmarkPtr);

	std::vector<int16_t> & getLabels();

private:
	uint32_t clusterFormattedData();
	void formatInputDataForClustering(vector<PdwDefaultClass>&);
	void calculateDistanceMatrix();
	void calculateDistanceMatrix2();
	void findNeighboursOfNode(uint32_t nodeIndex,
			std::vector<uint16_t> &vector);

	uint32_t assignClusterIdToDirectlyReachablePoints(int16_t clusterId,
			int32_t corePointIndex, std::vector<uint16_t> & reachablePoints);
	void removeClusterWithId(uint16_t id);

	double epsilon;
	uint32_t minPointsToDeclareCorePoint;
	uint32_t minPulsesToDeclareCluster;
	uint32_t featuresCount;
	uint32_t inputSamplesCount;

	boost::numeric::ublas::vector<double> featuresWeightsMatrix;
	boost::numeric::ublas::matrix<double> clusterDataMatrix;
	boost::numeric::ublas::matrix<double> distanceMatrix;
	std::vector<int16_t> labels;
	std::vector<uint8_t> visitedFlag;
	std::vector<uint16_t> directlyReachablePoints;
	std::vector<uint16_t> reachablePoints;

};

//All definitions
#ifdef ANALYSISDBSCANDELEGATE_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion

#endif /* ANALYSISDBSCANDELEGATE_H_ */
