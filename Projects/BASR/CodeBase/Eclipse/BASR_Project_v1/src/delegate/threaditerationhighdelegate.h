/*
 * threaditerationhighdelegate.h
 *
 *  Created on: Sep 26, 2019
 *      Author: hassan
 */

#ifndef THREADITERATIONHIGHDELEGATE_H_
#define THREADITERATIONHIGHDELEGATE_H_

#include "../common/common.h"
#include "../vo/datasensorclass.h"
#include "../model/model.h"
#include "../delegate/analysistrackhighdelegate.h"
#include "../delegate/analysistrackmergehighdelegate.h"

//start of case notes

//case: GroundTest_50001to60000pages
//Debug iteration: 23
//An emitter reported from low side with slight overlap was not merged at high side.

//Case: 8thMay201376001to77000pages
//There is a multi staggered scan that comes in the
//Debug iterations: 1, 39, 41, 44...
//But it does not appear in high side flight record.

//case: 130719_Flt2of3_IBand_10F11_1578_1875secs
//Debug iterations: 478 and 688.
//multiple emitters for Dwell type emitter are created at low side due to AOA
//jump but not being strung by high side.

//end of case notes

//new notes
//some logic needed to surpress multiple unknows from same angle along with fixed and large pri i-band scans
//lib  fidelity across merge, flight record and surpression logic and outercode and reset consecutive known
//..count flag
//accumulate logic for organic and string still requires refinement
//rename libmatch.display to libmatch.current and correct usage in files
//tighten stringing criteria for emitters with wide angle difference, non i-band emitters can be limited
//..in thier use of wide angle based stringing
//avoid track creation for ground illumination tracks if possible
//aoa smoothing
//review jitter and complex logic commonality
//ensure library match logic for difftoa tracks as well
//update merge variables keep/destroy logic after changes
//for basr and aems port, band specific decisions would be a problem
//look at track length in multiples of 200ms rather than update count in stringing decisions
//evaluate consequences and benefits of negative stringing at dsp
//distance calculation and history based smoothing, compare distance logic with basr
//our consecutive update flag for high allows counts through mergers?
//compute or use stt flag
//compute type/platform flags for unknown and with library
//remove duplicate pri from low during organic update logic between dwell and stagger (no fixed) ?
//..previous logic doesn't allow organic updates for 5 iterations if duplicate pris with no fixed flag
//..previous logic attempts to remove duplicate types from low if both are present in low
//adjust library match according to stt flag
//adjust library match for single pri emitters
//adjust frequency and placement of library matches
//adjust symbology according to emitter types and stt modes
//review high track merging parameters
//check flight record overwrites over long duration
//dsp failure should be presented after multiple reports are missed
//receiver related adjustment and self tests
//do not run self test (cbit) for emitters if present in last 10 mins
//maintain unknown track history for optimal numeric assignment
//adjust library scores and report any problem in tl software chain
//handle uncertain pri merger defination more in line with pup-1
//program control panel macros for track supression of scans
//color code threats based on band or scan type
//handle cd/cw tracks including surpression logic
//handle cfd logic
//consolidate old pup-1 notes with these

//convert stagger pri types of double and triplets into dwell spots
//finalize difftoa pri tracks only if they have been consistently been updated for a while??
//fix pw merge function done
//remove redundant pri from low tracks at low or while copying to accummulate at high done
//low side pw and rf types need to be fixed, with respect to type and reported spots/range
//a new exclusion rule is needed for both string and merge that relies on at least pri or pw

void threadCompleteIterationHigh(DataSensorClass &low,
		DataSensorClass &lowNext, DataSensorClass &high);//need to revise input

//functions, includes and defines
#ifdef THREADITERATIONHIGHDELEGATE_CPP_
//global variables
#else
//externs
#endif
//end of conditional exclusion

#endif /* THREADITERATIONHIGHDELEGATE_H_ */
