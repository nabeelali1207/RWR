/*
 * trackdefaultclass.h
 *
 *  Created on: Nov 20, 2018
 *      Author: hassan
 */

#ifndef TRACKDEFAULTCLASS_H_
#define TRACKDEFAULTCLASS_H_

#include "../common/common.h"
#include "../vo/trackangleclass.h"
#include "../vo/trackaudioclass.h"
#include "../vo/trackcfdclass.h"
#include "../vo/trackgeneralclass.h"
#include "../vo/trackhistoryclass.h"
#include "../vo/trackintrapulseclass.h"
#include "../vo/trackjammingclass.h"
#include "../vo/tracklibrarymatchclass.h"
#include "../vo/trackplatformclass.h"
#include "../vo/trackpowerclass.h"
#include "../vo/trackpriclass.h"
#include "../vo/trackpwclass.h"
#include "../vo/trackrangeclass.h"
#include "../vo/trackreleaseclass.h"
#include "../vo/trackrfclass.h"
#include "../io/serialbuffer.h"

class TrackDefaultClass
{

public:

	//general notes
	//pri, pw, rf difftoa, scan, arp, intrapulse containers, lib match (erp
	//cfd, audio program), ac attitude. Ranges low high, circular buffers etc??
	//decide which analysis actions would be one time, routine etc?
	//decide overall clustering, tracking, merging, visibility sequence?
	//decide stringing/merge timing
	//we may need to add sequence structures for pri, rf, pw, intrapulse
	//..combined sequences
	//parameters to decide track visibility for distant/offhand scans:
	//..strong lib match, perslistence, pri number and type.
	//make multiple timer functions to update time of all the routine tasks
	//such as merging, release, report, record, toa analysis (proper) etc
	//number of missed pulses flag (based on last pri) to determine expiry
	//end of general notes

	TrackAngleClass angle;
	TrackAudioClass audio;
	TrackCfdClass cfd;
	TrackGeneralClass general;
	TrackHistoryClass history;
	TrackIntraPulseClass intrapulse;
	TrackJammingClass jamming;
	TrackLibraryMatchClass libmatch;
	TrackPlatformClass platform;
	TrackPowerClass power;
	TrackPriClass pri;
	TrackPwClass pw;
	TrackRangeClass range;
	TrackReleaseClass release;
	TrackRfClass rf;
	circular_buffer<TrackDefaultClass> last;

	TrackDefaultClass();
	~TrackDefaultClass();
	void serialize(SerialBufferClass &buffer);
	static bool compareTrackToas(const TrackDefaultClass &first,
			const TrackDefaultClass &second);
};

#endif /* TRACKDEFAULTCLASS_H_ */
