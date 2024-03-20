/*
 * analysisdbscandelegate.cpp
 *
 *  Created on: Nov 16, 2018
 *      Author: afzal
 */
#define ANALYSISDBSCANDELEGATE_CPP_

#include "../delegate/analysisdbscandelegate.h"

void DBSCAN::formatInputDataForClustering(vector<PdwDefaultClass>& clusterPdws)
{
	featuresWeightsMatrix.resize(featuresCount);
	for (uint32_t i = 0; i < featuresCount; i++)
	{
		featuresWeightsMatrix[i] = 1.0;
	}
	featuresWeightsMatrix[4] = 4;
	inputSamplesCount = clusterPdws.size();
	clusterDataMatrix.resize(inputSamplesCount, featuresCount);
	distanceMatrix.resize(inputSamplesCount, inputSamplesCount);

	for (uint32_t i = 0; i < inputSamplesCount; i++)
	{
		clusterDataMatrix(i, 0) = ((double) clusterPdws[i].powerA)
				/ ConstLow::asClusterPowerMax;
		clusterDataMatrix(i, 1) = ((double) clusterPdws[i].powerB)
				/ ConstLow::asClusterPowerMax;
		clusterDataMatrix(i, 2) = ((double) clusterPdws[i].powerC)
				/ ConstLow::asClusterPowerMax;
		clusterDataMatrix(i, 3) = ((double) clusterPdws[i].powerD)
				/ ConstLow::asClusterPowerMax;
//				cout<< clusterDataMatrix(i,0) << ","<<clusterDataMatrix(i,1) << endl;
//				clusterData(i,4) = (-1.0 + clusterPdws[i].pulseWidth * (2.0) / ConstLow::asClusterPulseWidthMax);
//
		clusterDataMatrix(i, 4) = (-1.0
				+ clusterPdws[i].frequency * (2.0) / ConstLow::asClusterRfMax);
//		clusterData(i,4) = (-1.0 + clusterPdws[i].frequency * (2.0) / ConstLow::asClusterRfMax);
	}
}
void DBSCAN::calculateDistanceMatrix()
{

	for (uint32_t i = 0; i < inputSamplesCount; i++)
	{
		for (uint32_t j = 0; j < inputSamplesCount; j++)
		{
			double tempVal = 0;

			tempVal += featuresWeightsMatrix[0]*pow(
					abs(clusterDataMatrix(i, 0) - clusterDataMatrix(j, 0)), 2);
			tempVal += featuresWeightsMatrix[1]*pow(
					abs(clusterDataMatrix(i, 1) - clusterDataMatrix(j, 1)), 2);
			tempVal += featuresWeightsMatrix[2]*pow(
					abs(clusterDataMatrix(i, 2) - clusterDataMatrix(j, 2)), 2);
			tempVal += featuresWeightsMatrix[3]*pow(
					abs(clusterDataMatrix(i, 3) - clusterDataMatrix(j, 3)), 2);
			tempVal += featuresWeightsMatrix[4]*pow(
					abs(clusterDataMatrix(i, 4) - clusterDataMatrix(j, 4)), 2);
			tempVal = sqrt(tempVal);
			distanceMatrix(i, j) = tempVal;
			distanceMatrix(j, i) = tempVal;
		}
	}
}
void DBSCAN::calculateDistanceMatrix2()
{

	for (uint32_t i = 0; i < inputSamplesCount; i++)
	{
		for (uint32_t j = 0; j < inputSamplesCount; j++)
		{
			double tempVal = 0;

			tempVal += abs(clusterDataMatrix(i, 0) - clusterDataMatrix(j, 0));
			tempVal += abs(clusterDataMatrix(i, 1) - clusterDataMatrix(j, 1));
			tempVal += abs(clusterDataMatrix(i, 2) - clusterDataMatrix(j, 2));
			tempVal += abs(clusterDataMatrix(i, 3) - clusterDataMatrix(j, 3));
//			tempVal = sqrt(tempVal);
			distanceMatrix(i, j) = tempVal;
			distanceMatrix(j, i) = tempVal;
		}
	}
}
void DBSCAN::findNeighboursOfNode(uint32_t nodeIndex,
		std::vector<uint16_t> &vector)
{
	for (uint32_t i = 0; i < inputSamplesCount; i++)
	{
		if (i == nodeIndex)
			continue;
		if (distanceMatrix(nodeIndex, i) <= epsilon)
		{
			vector.push_back(i);
		}
	}
}

uint32_t DBSCAN::assignClusterIdToDirectlyReachablePoints(int16_t clusterId,
		int32_t corePointIndex,
		std::vector<uint16_t> & _directlyReachablePoints)
{
	uint32_t pointCountInCluster = 1;
	labels[corePointIndex] = clusterId;
	for (uint32_t i = 0; i < _directlyReachablePoints.size(); i++)
	{
		uint32_t minorNodeIndex = _directlyReachablePoints[i];
		if (visitedFlag[minorNodeIndex] == 0)
		{
			visitedFlag[minorNodeIndex] = 1;
			labels[minorNodeIndex] = clusterId;
			pointCountInCluster++;
			reachablePoints.resize(0);
			findNeighboursOfNode(minorNodeIndex, reachablePoints);
			if (reachablePoints.size() > minPointsToDeclareCorePoint)
			{
				for (uint32_t j = 0; j < reachablePoints.size(); j++)
				{
					if (visitedFlag[reachablePoints[j]] == 0)
					{
						_directlyReachablePoints.push_back(reachablePoints[j]);
					}
				}
			}
		}
		else if (labels[minorNodeIndex] == -1)
		{//a spot that has been already visited but no label was assigned before to it
			labels[minorNodeIndex]=clusterId;
			pointCountInCluster++;
		}

	}
	return pointCountInCluster;
}
uint32_t DBSCAN::clusterFormattedData()
{
	directlyReachablePoints.resize(0);
	directlyReachablePoints.reserve(distanceMatrix.size1());
	reachablePoints.resize(0);
	reachablePoints.reserve(distanceMatrix.size1());

	labels.resize(distanceMatrix.size1());
	visitedFlag.resize(distanceMatrix.size1());

	int16_t clusterId = 0;

	for (uint32_t i = 0; i < distanceMatrix.size1(); i++)
	{
		labels[i] = -1;
		visitedFlag[i] = 0;
	}

	for (uint32_t currMajorNodeIndex = 0;
			currMajorNodeIndex < distanceMatrix.size1(); currMajorNodeIndex++)
	{
		if (visitedFlag[currMajorNodeIndex] == 0)
		{
			visitedFlag[currMajorNodeIndex] = 1;

			directlyReachablePoints.resize(0);
			findNeighboursOfNode(currMajorNodeIndex, directlyReachablePoints);

			if (directlyReachablePoints.size() > minPointsToDeclareCorePoint)
			{
				uint32_t pulseCountInCluster =
						assignClusterIdToDirectlyReachablePoints(clusterId,
								currMajorNodeIndex, directlyReachablePoints);

				if (pulseCountInCluster >= minPulsesToDeclareCluster)
				{
					clusterId++;
				}
				else
				{
					removeClusterWithId(clusterId);
				}
			}

		}
	}
	return clusterId;
}
uint32_t DBSCAN::clusterInputPdws(vector<PdwDefaultClass>& inputPdws,uint64_t *benchmarkPtr)
{
	benchmarkPtr[0]=Utility::getSystemTime();
	formatInputDataForClustering(inputPdws);
	benchmarkPtr[1]=Utility::getSystemTime();

	calculateDistanceMatrix();
	benchmarkPtr[2]=Utility::getSystemTime();

	uint32_t clusterCount = clusterFormattedData();
	benchmarkPtr[3]=Utility::getSystemTime();

	return clusterCount;
}
std::vector<int16_t> & DBSCAN::getLabels()
{
	return labels;
}
void DBSCAN::removeClusterWithId(uint16_t id)
{
	for (uint32_t i = 0; i < labels.size(); i++)
	{
		if (labels[i] == id)
			labels[i] = -1;
	}
}
