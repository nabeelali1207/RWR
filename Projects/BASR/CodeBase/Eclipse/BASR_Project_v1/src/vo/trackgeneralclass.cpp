/*
 * trackgeneralclass.cpp
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#define TRACKGENERALCLASS_CPP_

#include "../vo/trackgeneralclass.h"

//default constructor
TrackGeneralClass::TrackGeneralClass()
{
	this->dspPdwBuffer.set_capacity(ConstLow::trackPdwBuffCircSize);
	this->dspIterationPulseCount = 0;
	this->dspTrackingEnabled = true;
	this->dspCandidateForStringingFlag = false;
	this->dspDeleteTrack = false;
	this->updateOffFlag = false;
	this->timeToDeclareSilent = 0;
	this->iterSysLastUpdate = 0;
	this->timeToaLastUpdate = 0;
	this->timeSysLastUpdate = 0;
	this->updateOffFlag = 0;
	this->trackType = TrackTypeEnum::track_unknown;
	this->toaFirst = 0;
	this->toaLast = 0;
	this->toaFirstOverlapAnalysis = 0;
	this->toaFirstUpdatedFlag = 0;
	this->totalStringTracksCount = 0;
	this->iterStringTracksCount = 0;
	this->timeSysLastUpdate = 0;
	this->timeSysCreation = 0;
	this->trackID = getUniqueID();
	this->dspTrackingEnabled = true;
	this->trackPulseCountTotal = 0; //stringing usage
	this->trackPulseCountIterationLast = 0; //stringing usage
	this->priorityDynamic = 0;
	this->flagDeletionHigh = 0;
	this->flagVisibleHigh = 1;
	this->flagVisiblePriority = 1;
	this->flagSuppressEmitter = 0;
	this->flagDemod = 0;
	this->flagSelfTest = 0;
	this->flagProbation = 0;
	this->processingHighFlag = 0;
	this->timeIterCountLifetime = 0;
	this->timeIterLimitLifetime = 0;
	this->timeIterCountLifetimeGlobal = 0;
	//this->timeIterVisibleSet = 0;
	this->timeIterCountDemodSupprSet = 0;
	this->timeIterLimitDemodSupprSet = 0;
	this->timeIterCountProbationSet = 0;
	this->timeIterLimitProbationSet = 0;
}

TrackGeneralClass::~TrackGeneralClass()
{
	// TODO Auto-generated destructor stub
}
uint64_t TrackGeneralClass::IDCounter=0;
uint64_t TrackGeneralClass::getUniqueID()
{
	return IDCounter++;
}
void TrackGeneralClass::serialize(SerialBufferClass & buffer)
{
	buffer.serialize(trackID, 0);
	buffer.serialize(dspTrackingEnabled, 0);
	buffer.serialize(flagDemod, 0);
	buffer.serialize(flagSelfTest, 0);
	buffer.serialize(updateOffFlag, 0);
	buffer.serialize(trackType, 0);
	buffer.serialize(toaFirst, 0);
	buffer.serialize(toaLast, 0);
	buffer.serialize(timeToaLastUpdate, 0);
	buffer.serialize(totalStringTracksCount, 0);
	buffer.serialize(iterStringTracksCount, 0);
	//buffer.serialize(timeToaCreation, 0);
	buffer.serialize(timeSysLastUpdate, 0);
	buffer.serialize(timeSysCreation, 0);
}
