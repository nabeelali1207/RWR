/*
 * logginghighdelegate.h
 *
 *  Created on: Sep 25, 2019
 *      Author: hassan
 */

#ifndef LOGGINGHIGHDELEGATE_H_
#define LOGGINGHIGHDELEGATE_H_

#include "../common/common.h"
#include "../vo/trackdefaultclass.h"
#include "../vo/libraryclass.h"
#include "../vo/dataanalysisclass.h"
#include "../model/model.h"
#include <time.h>
#include "../utility/sha256.h"
#include "../utility/aes.h"
#include "../utility/consthigh.h"
#include <string.h>
#include <sys/stat.h>
#include "../io/fileoutputflightrecordhigh.h"

void logWriteLowTracks(vector<TrackDefaultClass> &tracksE,
		vector<TrackDefaultClass> &tracksG, vector<TrackDefaultClass> &tracksI,
		vector<TrackDefaultClass> &tracksJ, ThreatLibrary *inputLibrary,
		LibraryMetaClass *libraryMeta, string flighRecordPath);

void logWriteHighTracks(vector<TrackDefaultClass> &tracksE,
		vector<TrackDefaultClass> &tracksG, vector<TrackDefaultClass> &tracksI,
		vector<TrackDefaultClass> &tracksJ, ThreatLibrary *inputLibrary,
		LibraryMetaClass *libraryMeta, string flighRecordPath);

void singleFlightRecordCopy(FlightRecord *fr, TrackDefaultClass &track);

void singleLowEmitterCopy(FlightRecord *fr, TrackDefaultClass &track);

void initFlightRecordStructure();

#ifdef LOGGINGHIGHDELEGATE_CPP_

#else
//extern if required
#endif
//End of conditional exclusion

#endif // LOGGINGHIGHDELEGATE_H_

