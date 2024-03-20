/*
 * trackrfclass.h
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#ifndef TRACKRFCLASS_H_
#define TRACKRFCLASS_H_

#include "../common/common.h"
#include "../utility/constlow.h"
#include "../utility/consthigh.h"
#include "../vo/limitrfclass.h"
#include "../vo/pdwdefaultclass.h"
#include "../io/serialbuffer.h"
#include "../delegate/analysistrackrflowdelegate.h"
#include <boost/circular_buffer.hpp>

using namespace boost;

class TrackRfClass {
public:

	//low variables
	circular_buffer<LimitRfClass> rfCircular;//need first and last rf in seq
	uint16_t lastAnalysisSpotCount;
	vector<LimitRfClass> rfFirst;//stringing usage
	//rfLast stringing usage, can be gotten from rfCircular

	uint32_t rfAmplitude;
	BandCodeEnum bandCode;//perhaps change this to enum
	uint16_t rfTrackingPossible;
	RfTypeEnum curRfType;
	uint16_t rfValue;
	uint64_t timeToaRfAnl;

	//high variables
	circular_buffer<LimitRfClass> accumulateRf;//lib match usage
	uint16_t accumulateRfCount;//lib match usage
	uint16_t accumulateFixedRfCount;
	uint16_t accumulateAgileRfCount;
	uint16_t accumulateSwitchRfCount;
	uint16_t accumulateJumpRfCount;
	uint16_t accumulateComplexRfCount;
	vector<AnalysisTrackRfLowClass> rfTrackingObj;
	uint8_t getBandEnumeration();
	void AllocateRfTrackingObject();

	uint8_t getBandCodeLegacy();
	LimitRfClass getRfMinSpot();
	LimitRfClass getRfMaxSpot();
	LimitRfClass getRfMinSpotLow();
	LimitRfClass getRfMaxSpotLow();
	TrackRfClass();
	~TrackRfClass();
	void serialize(SerialBufferClass & buffer);
};


#endif /* TRACKRFCLASS_H_ */
