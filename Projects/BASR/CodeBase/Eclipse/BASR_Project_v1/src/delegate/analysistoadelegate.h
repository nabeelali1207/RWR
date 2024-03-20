/*
 * analysistoadelegate.h
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#ifndef ANALYSISTOADELEGATE_H_
#define ANALYSISTOADELEGATE_H_

#include "../common/common.h"
#include "../vo/pdwdefaultclass.h"
#include "../vo/pdwdifferenceclass.h"
#include "../vo/pdwsequenceclass.h"
#include "../vo/trackdefaultclass.h"
#include "../vo/datasensorclass.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <numeric>
#include "../model/model.h"
#include "../vo/loglowclass.h"
#include "analysistrackprilowdelegate.h"
//notes
//keep track of pulses for pri analysis, onces that sink in a track not based
//on pri would still have pri analysis performed on them while those sunk based
//on pri would skip that process, along with the non ones before it. Maybe use
//a pdw level flag for such marking in the circular buffer?

//old thought
//when computing pri analysis, compute diff toas and histogram them, determine
//..center value of each histogram and assign a range for that value that would
//..be used for further sequence searches. Use appropriate data structures
//..To sequence search, take each histogrammed difftoa range and build desired
//..patterns depending on which one we are looking for i.e. stagger, jitter etc
//..we would need to cater for missing pulses when sequence searching
//When and how would we calculate last running pri of a track? pri analysis
//..may give us pris out of order, additional limited pri analysis maybe needed
//..along with storage and hashing of our existing spots
//end of notes


//All definitions
void priUpdatePriActiveSpotInfo(vector <TrackDefaultClass>& tracks, uint32_t &stringableTrackCount);
void priCopyResultFromFromTrackingObject(TrackDefaultClass& track);

#ifdef ANALYSISTOADELEGATE_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion


#endif /* ANALYSISTOADELEGATE_H_ */
