/*
 * trackpwclass.h
 *
 *  Created on: Dec 26, 2018
 *      Author: hassan
 */

#ifndef TRACKPWCLASS_H_
#define TRACKPWCLASS_H_

#include "../common/common.h"
#include "../utility/constlow.h"
#include "../utility/consthigh.h"
#include "../vo/limitpwclass.h"
#include <boost/circular_buffer.hpp>

#include "../delegate/analysistrackpwlowdelegate.h"
#include "../io/serialbuffer.h"
using namespace boost;

class TrackPwClass {
public:

	//low variables
	circular_buffer<LimitPwClass> pwCircular;//need first and last pw in seq
	vector<LimitPwClass> pwFirst;//stringing usage
	//pwLast stringing usage, can be gotten from pwCircular

	uint16_t lastAnalysisSpotCount;
	int8_t pwAnalysisDoneOnPriBuffer;
	PwTypeEnum curPwType;

	vector<AnalysisTrackPwLowClass> pwTrackingObj;

	uint64_t timeToaPwAnl;


	//high variables
	circular_buffer<LimitPwClass> accumulatePw;
	uint16_t accumulatePwCount;
	TrackPwClass();

	LimitPwClass getPwMinSpot();
	LimitPwClass getPwMaxSpot();
	void AllocatePwTrackingObject();
	~TrackPwClass();
	void serialize(SerialBufferClass & buffer);
};


#endif /* TRACKPWCLASS_H_ */
