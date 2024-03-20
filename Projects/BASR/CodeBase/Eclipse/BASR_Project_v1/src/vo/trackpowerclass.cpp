/*
 * trackpowerclass.cpp
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#define TRACKPOWERCLASS_CPP_

#include "../vo/trackpowerclass.h"

//default constructor
TrackPowerClass::TrackPowerClass()
{
	arpFirst = 0;
	arpLast = 0;
	lastPa = 0;
	lastPb = 0;
	lastPc = 0;
	lastPd = 0;
	pulseCountSinceLastPowerAnalysis = 0;
	pulseCountSinceLastScanAnalysis = 0;
	pulseCountSinceLastGroundIlluminationFlagsUpdate = 0;
	quadrantCodeMaxPxPy = 0;
	avgPA = 0;
	avgPB = 0;
	avgPC = 0;
	avgPD = 0;
	stdPA = 0;
	stdPB = 0;
	stdPC = 0;
	stdPD = 0;
	pulseCountSinceLastReport = 0;
	curMinStartPower = 0;
	curMinEndPower = 0;
	curMaxPower = 0;
	toaMinStartPower = 0;
	toaMinEndPower = 0;
	pxAntennaFirst = 0;
	pxAntennaLast = 0;
	pyAntennaFirst = 0;
	pyAntennaLast = 0;
	maxPowerToa = 0;
	pxValueFirst = 0;
	pxValueLast = 0;
	pxAntenna = 0;
	pyAntenna = 0;
	pxMax = 0;
	pyMax = 0;
	sumPxPyMax = 0;
	pyValueFirst = 0;
	pyValueLast = 0;
	pxMaxQuadrant[0] = 0;
	pxMaxQuadrant[1] = 0;
	pxMaxQuadrant[2] = 0;
	pxMaxQuadrant[3] = 0;
	pyMaxQuadrant[0] = 0;
	pyMaxQuadrant[1] = 0;
	pyMaxQuadrant[2] = 0;
	pyMaxQuadrant[3] = 0;
	quadrantCodeMaxPxPy = 0;
	quadrantCodeFirst = 0;
	quadrantCodeLast = 0;
	quadrantCodeMax = 0;
	primaryAntennaFirst = 0;
	primaryAntennaLast = 0;
	dominantAntenna = 0;
	secondaryAntenna = 0;
	countQuadrantCodesIter[0] = 0;
	countQuadrantCodesIter[1] = 0;
	countQuadrantCodesIter[2] = 0;
	countQuadrantCodesIter[3] = 0;
	groundIlluminationFlag = 0;
	groundIlluminationAxis = GroundIlluminationEnum::no_axis;
	flagScanAnalysisRequested = 0;
	countDominantAntenna[0] = 0;
	countDominantAntenna[1] = 0;
	countDominantAntenna[2] = 0;
	countDominantAntenna[3] = 0;

	countSecDominantAntenna[0] = 0;
	countSecDominantAntenna[1] = 0;
	countSecDominantAntenna[2] = 0;
	countSecDominantAntenna[3] = 0;

	scanTypeFirst = EmitterArpEnum::scan_unknown;
	scanTypeLast = EmitterArpEnum::scan_unknown;
	scanIlluminationTimeFirst = 0;
	scanIlluminationTimeLast = 0;
	timeToaScanAnalysisLast = 0;
	scanPulseCountFirst = 0;
	scanPulseCountLast = 0;
	flagTrackingModeHigh = 0;
	flagScanningModeHigh =0;
	flagMissileLaunchHigh=0;
	countTrackingModeIterHigh = 0;
	countTrackingModeLowIterHigh = 0;
	countScanningModeIterHigh=0;
	countScanningModeLowIterHigh=0;
	flagResetQuadrantCodes = 0;
	illuminationInfo.set_capacity(10);
	scanAnalysisHistory.set_capacity(6);
}

TrackPowerClass::~TrackPowerClass()
{
	// TODO Auto-generated destructor stub
}
void TrackPowerClass::serialize(SerialBufferClass &buffer)
{
	buffer.serialize(lastPa, 0);
	buffer.serialize(lastPb, 0);
	buffer.serialize(lastPc, 0);
	buffer.serialize(lastPd, 0);

	// per N pulses variables N=10 for now
	buffer.serialize(pulseCountSinceLastPowerAnalysis, 0);
	buffer.serialize(avgPA, 0);
	buffer.serialize(avgPB, 0);
	buffer.serialize(avgPC, 0);
	buffer.serialize(avgPD, 0);

	buffer.serialize(stdPA, 0);
	buffer.serialize(stdPB, 0);
	buffer.serialize(stdPC, 0);
	buffer.serialize(stdPD, 0);

	// per reporting period variables
	buffer.serialize(pulseCountSinceLastReport, 0);
	buffer.serialize(curMinStartPower, 0);
	buffer.serialize(curMinEndPower, 0);
	buffer.serialize(curMaxPower, 0);
	buffer.serialize(toaMinStartPower, 0);
	buffer.serialize(toaMinEndPower, 0);
	buffer.serialize(pxAntennaFirst, 0);
	buffer.serialize(pxAntennaLast, 0);
	buffer.serialize(pyAntennaFirst, 0);
	buffer.serialize(pyAntennaLast, 0);
	buffer.serialize(maxPowerToa, 0);
	buffer.serialize(pxValueFirst, 0);
	buffer.serialize(pxValueLast, 0);
	buffer.serialize(quadrantCodeMaxPxPy, 0);
	buffer.serialize(pxMax, 0);
	buffer.serialize(pyMax, 0);
	buffer.serialize(pyValueFirst, 0);
	buffer.serialize(pyValueLast, 0);
	buffer.serialize(quadrantCodeFirst, 0);
	buffer.serialize(quadrantCodeLast, 0);
	buffer.serialize(primaryAntennaFirst, 0);
	buffer.serialize(primaryAntennaLast, 0);
	buffer.serialize(groundIlluminationFlag, 0);
	buffer.serialize(groundIlluminationAxis, 0);
	buffer.serialize(countQuadrantCodesIter[0], 0);
	buffer.serialize(countQuadrantCodesIter[1], 0);
	buffer.serialize(countQuadrantCodesIter[2], 0);
	buffer.serialize(countQuadrantCodesIter[3], 0);
	buffer.serialize(scanTypeLast, 0);
	buffer.serialize(scanIlluminationTimeLast, 0);
	buffer.serialize(arpLast, 0); //?
	buffer.serialize(timeToaScanAnalysisLast, 0);
	buffer.serialize(illuminationInfo,0);
}

void TrackPowerClass::copyParametersFromStringedEmitter(TrackPowerClass &other)
{

	weightsVector[0] = other.weightsVector[0];
	weightsVector[1] = other.weightsVector[1];
	weightsVector[2] = other.weightsVector[2];
	weightsVector[3] = other.weightsVector[3];
	lastPa = other.lastPa;
	lastPb = other.lastPb;
	lastPc = other.lastPc;
	lastPd = other.lastPd;
	pulseCountSinceLastPowerAnalysis = other.pulseCountSinceLastPowerAnalysis;
	pulseCountSinceLastScanAnalysis += other.pulseCountSinceLastScanAnalysis;

	pulseCountSinceLastGroundIlluminationFlagsUpdate +=
			other.pulseCountSinceLastGroundIlluminationFlagsUpdate;

	avgPA = other.avgPA;
	stdPA = other.stdPA;
	avgPB = other.avgPB;
	stdPB = other.stdPB;
	avgPC = other.avgPC;
	stdPC = other.stdPC;
	avgPD = other.avgPD;
	stdPD = other.stdPD;
	pulseCountSinceLastReport += other.pulseCountSinceLastReport;
	curMinStartPower = other.curMinStartPower;
	curMinEndPower = other.curMinEndPower;
	curMaxPower = other.curMaxPower;
	toaMinStartPower = other.toaMinStartPower;
	toaMinEndPower = other.toaMinEndPower;

	pxAntennaLast = other.pxAntennaLast;
	pyAntennaLast = other.pyAntennaLast;
	pxValueLast = other.pxValueLast;

	if ((other.pxMax + other.pyMax) > (pxMax + pyMax))
	{
		pxMax = other.pxMax;
		pyMax = other.pyMax;
		quadrantCodeMaxPxPy = other.quadrantCodeMaxPxPy;
	}

	pyValueLast = other.pyValueLast;
	quadrantCodeLast = other.quadrantCodeLast;

	countQuadrantCodesIter[0] += other.countQuadrantCodesIter[0];
	countQuadrantCodesIter[1] += other.countQuadrantCodesIter[1];
	countQuadrantCodesIter[2] += other.countQuadrantCodesIter[2];
	countQuadrantCodesIter[3] += other.countQuadrantCodesIter[3];

	primaryAntennaLast = other.primaryAntennaLast;
	dominantAntenna = other.dominantAntenna;
	secondaryAntenna = other.secondaryAntenna;
	pxAntenna = other.pxAntenna;
	pyAntenna = other.pyAntenna;

	if (other.groundIlluminationFlag == true)
	{
		groundIlluminationFlag = other.groundIlluminationFlag;
		groundIlluminationAxis = other.groundIlluminationAxis;
	}
	countDominantAntenna[0] += other.countDominantAntenna[0];
	countDominantAntenna[1] += other.countDominantAntenna[1];
	countDominantAntenna[2] += other.countDominantAntenna[2];
	countDominantAntenna[3] += other.countDominantAntenna[3];

	countSecDominantAntenna[0] += other.countSecDominantAntenna[0];
	countSecDominantAntenna[1] += other.countSecDominantAntenna[1];
	countSecDominantAntenna[2] += other.countSecDominantAntenna[2];
	countSecDominantAntenna[3] += other.countSecDominantAntenna[3];

	scanIlluminationTimeFirst = other.scanIlluminationTimeFirst;
	scanIlluminationTimeLast = other.scanIlluminationTimeLast;

	scanPulseCountLast = other.scanPulseCountLast;
	arpLast = other.arpLast;
	//flagTrackingLowLast = other.flagTrackingLowLast;
	//flagTrackingModeHigh = other.flagTrackingModeHigh;
}
