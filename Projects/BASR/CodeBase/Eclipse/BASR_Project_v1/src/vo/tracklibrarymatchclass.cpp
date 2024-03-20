/*
 * tracklibrarymatchclass.cpp
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#define TRACKLIBRARYMATCHCLASS_CPP_

#include "../vo/tracklibrarymatchclass.h"

TrackLibraryMatchScoreClass::TrackLibraryMatchScoreClass()
{
	this->ID = 0;
	this->LpiScore = 0;
	this->PRIScore = 0;
	this->PWScore = 0;
	this->PWtypeScore = 0;
	this->RFScore = 0;
	this->RFtypeScore = 0;
	this->PRItypeScore = 0;
	this->Total = 0;
	this->dwellCount = 0;
	this->staggerCount = 0;
	this->slidingCount = 0;
	this->jitterCount = 0;
	this->complexCount = 0;
	this->diffToaCount = 0;
	this->emitterBatchId = 0;
}

TrackLibraryMatchScoreClass::~TrackLibraryMatchScoreClass()
{
}

TrackLibraryResultsClass::TrackLibraryResultsClass()
{
	this->clear();
	this->thresholdScoreKnown = ConstHigh::libraryScoreDefaultThreshold;
	this->countPriRequiredKnown = ConstHigh::libraryCountPriDefaultThreshold;
}
void TrackLibraryResultsClass::clear()
{
	this->highestMatchListCount = 0;

	for (uint8_t i = 0; i < 3; i++)
	{
		this->highestMatchList[i].ID = 0;
		this->highestMatchList[i].LpiScore = 0;
		this->highestMatchList[i].PRIScore = 0;
		this->highestMatchList[i].PWScore = 0;
		this->highestMatchList[i].PWtypeScore = 0;
		this->highestMatchList[i].RFScore = 0;
		this->highestMatchList[i].RFtypeScore = 0;
		this->highestMatchList[i].PRItypeScore = 0;
		this->highestMatchList[i].Total = 0;

		this->highestMatchList[i].symbolOne = 0;
		this->highestMatchList[i].symbolTwo = 0;
		this->highestMatchList[i].symbolThree = 0;
		this->highestMatchList[i].symbolFour = 0;
		this->highestMatchList[i].symbolFive = 0;
		this->highestMatchList[i].symbolTop = 0;
		this->highestMatchList[i].symbolOuter = 0;
	}

	this->highestMatchListCount = 0;
	this->priorityLibrary = 0;
	this->effectiveRadiatedPower = 0;
	this->countPriRequiredLibrary = 0;
	this->countPriPresentLibrary = 0;
	this->countCategoryConsecutiveKnown = 0;
	this->flagDisplayLibrary = 0;
	this->flagKnownLibrary = 0;
	this->flagKnownPartialLibrary = 0;
	this->flagAerialOnlyLibrary = 0;
	this->flagShipOnlyLibrary = 0;
	this->flagTrackingOnlyLibrary = 0;
	this->flagMissileOnlyLibrary = 0;
	this->flagScanningOnlyLibrary = 0;
	this->flagSamOnlyLibrary = 0;
	this->suppressEmitter = 0;
}
TrackLibraryResultsClass::~TrackLibraryResultsClass()
{

}

//default constructor
TrackLibraryMatchClass::TrackLibraryMatchClass()
{
	this->flagRequestLibraryMatch = 0;
	this->flagSetUnknownLibrary = 0;
	this->flagIterAttemptMatchLibrary = 0;
	this->flagRequestResetTrackParameters = 0;
	this->countConsecutiveUnknownLibrary = 0;
	this->timeIterCountLibMatchExec = 0;
	this->timeIterLimitLibMatchExec = ConstHigh::timeIterEquivalent1000ms;
}

TrackLibraryMatchClass::~TrackLibraryMatchClass()
{
}
