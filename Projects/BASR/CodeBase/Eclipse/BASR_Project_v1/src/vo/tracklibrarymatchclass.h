/*
 * tracklibrarymatchclass.h
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#ifndef TRACKLIBRARYMATCHCLASS_H_
#define TRACKLIBRARYMATCHCLASS_H_

#include "../common/common.h"
#include "../vo/libraryclass.h"
#include <boost/circular_buffer.hpp>

using namespace boost;

enum class EmitterPlatformEnum {platform_unknown, platform_ground, platform_air,
	platform_ship, platform_singlecannon, platform_doublecannon};

enum class EmitterAttributeEnum {attribute_unknown, attribute_friend,
	attribute_foe, attribute_ours};

//currently not being used as we are using a legacy structure, needs fixing
class TrackLibraryMatchScoreClass {
public:
	uint16_t ID;
	float  PRIScore;
	float  PRItypeScore;
	float  RFScore;
	float  RFtypeScore;
	float  PWScore;
	float  PWtypeScore;
	float  LpiScore;
	float  Total;
	uint8_t dwellCount;
	uint8_t staggerCount;
	uint8_t slidingCount;
	uint8_t jitterCount;
	uint8_t complexCount;
	uint8_t diffToaCount;
	uint16_t emitterBatchId;
	TrackLibraryMatchScoreClass();
	~TrackLibraryMatchScoreClass();
};

class TrackLibraryResultsClass {
public:

	LibraryMatchScore highestMatchList[3];
    uint8_t highestMatchListCount;
    uint8_t thresholdScoreKnown;
    uint16_t priorityLibrary;
    uint16_t effectiveRadiatedPower;
    uint8_t countPriRequiredLibrary;
    uint8_t countPriPresentLibrary;
    uint8_t countPriRequiredKnown;
    uint32_t countCategoryConsecutiveKnown;
	uint8_t flagDisplayLibrary;
	uint8_t flagKnownLibrary;
	uint8_t flagKnownPartialLibrary;
	uint8_t flagAerialOnlyLibrary;
	uint8_t flagTrackingOnlyLibrary;
	uint8_t flagMissileOnlyLibrary;
	uint8_t flagScanningOnlyLibrary;
	uint8_t flagSamOnlyLibrary;
	uint8_t flagShipOnlyLibrary;
	uint8_t suppressEmitter;
	uint8_t cfdSuggestedId;
	uint8_t cfdAlternativeId;

	TrackLibraryResultsClass();
	void clear();
	virtual ~TrackLibraryResultsClass();
};

class TrackLibraryMatchClass {
public:
	//how to handle cd/cw correlations in this structure?
	//track library match along with candidate matches
	//do we need a circular match buffer here? between releases?

	TrackLibraryResultsClass last;
	TrackLibraryResultsClass previous;
	TrackLibraryResultsClass active;

	uint8_t flagSetUnknownLibrary;
	uint8_t flagIterAttemptMatchLibrary;
	uint8_t flagRequestLibraryMatch;
	uint8_t flagRequestResetTrackParameters;
	uint32_t countConsecutiveUnknownLibrary;
	uint32_t timeIterCountLibMatchExec;
	uint32_t timeIterLimitLibMatchExec;

	TrackLibraryMatchClass();
	virtual ~TrackLibraryMatchClass();
};


#endif /* TRACKLIBRARYMATCHCLASS_H_ */
