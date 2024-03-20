/*
 * cwpreportdelegate.h
 *
 *  Created on: Jan 14, 2021
 *      Author: saad
 */

#ifndef REPORTCWHIGHDELEGATE_H_
#define REPORTCWHIGHDELEGATE_H_

#include "../common/common.h"
#include "../model/model.h"
#include "../utility/constant.h"
#include "../vo/dataanalysisclass.h"
#include "../delegate/analysislibrarydelegate.h"
#include "analysistracklowdelegate.h"

/*
 * Function Name:   reportCwReadFromCwp
 * Purpose:         This function reads CW reports from CQSI, finds all self test emitters
 *					and copy those emitters to local track variable of Data Analysis Class
 * Inputs:          DataAnalysisClass Object
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void reportCwReadFromCwp(DataAnalysisClass &analysisCwp);

/*
 * Function Name:   reportCwCopyCwpReportToAnalysisObject
 * Purpose:         This function copies received emitters to local track variable of DataAnalysisClass
 * Inputs:          DataAnalysisClass Object
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void reportCwCopyCwpReportToAnalysisObject(DataAnalysisClass &analysisCwp);

/*
 * Function Name:   reportCwCompleteIterationHigh
 * Purpose:         This function is main logic of CW processing. CW reports are read from
 * 					CSI link with CWP, are associated with old CW tracks, new CW tracks are
 * 					created for new threats, Spur CW threats are rejected, matching, distance
 * 					calculation is done and finally tracks to be expired are expired
 * Inputs:          DataAnalysisClass Object for received CW threats
 * 					DataAnalysisClass Object for current iteration
 * 					Decrypted Threat Library
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void reportCwCompleteIterationHigh(uint32_t simulatedThreats,
		DataAnalysisClass &analysisCwpHistory,
		DataAnalysisClass &analysisCwpCurrent, LibraryStruct &library,
		CfdLibrary &cfdLib);

/*
 * Function Name:   reportCwPostProcessIterHigh
 * Purpose:         All the track lifetime counters are incremented in this function
 * Inputs:          All the CW tracks
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void reportCwPostProcessIterHigh(vector<TrackDefaultClass> &tracks);

/*
 * Function Name:   reportCwExpireTracksHigh
 * Purpose:         All tracks are traversed, If a track has completed its lifetime,
 * 					it will be deleted in this function
 * Inputs:          All the CW tracks
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void reportCwExpireTracksHigh(vector<TrackDefaultClass> &tracks);

/*
 * Function Name:   reportCwAssociateTracksHigh
 * Purpose:         CW tracks having same RF as CW received tracks are associated
 * 					and all the latest values are updated to the CW track
 * Inputs:          All the CW tracks being tracked
 * 					All the CW tracks received
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void reportCwAssociateTracksHigh(vector<TrackDefaultClass> &tracksCwReported,
		vector<TrackDefaultClass> &tracksCwActual);

/*
 * Function Name:   reportCwCreateTracksHigh
 * Purpose:         New CW tracks are created for those CW threats who are not associated
 * Inputs:          All the CW tracks being tracked
 * 					All the CW tracks received
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void reportCwCreateTracksHigh(vector<TrackDefaultClass> &tracksCwReported,
		DataAnalysisClass &analysisCwp);

/*
 * Function Name:   reportCwMatchTracksHighLogic
 * Purpose:         All CW tracks are sent to be matched against CW threats from TL
 * Inputs:          All the CW tracks being tracked
 * 					Threat Library
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void reportCwMatchTracksHighLogic(vector<TrackDefaultClass> &tracksCwActual,
		LibraryStruct &library, CfdLibrary &cfdLib);

void reportCwCfdDataTracksHigh(vector<TrackDefaultClass> &tracksCwActual,
		LibraryStruct &library, CfdLibrary &cfdLib);

void reportCwMatchTrackPostProcess(TrackDefaultClass &track,
		LibraryStruct &library, CfdLibrary &cfdLib);

/*
 * Function Name:   reportCwMatchTracksHighLogic
 * Purpose:         CW threat is matched with TL CW threats based on their RF
 * Inputs:          Single CW track
 * 					Threat Library
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void reportCwMatchTrackHigh(TrackDefaultClass &track, LibraryStruct &library);

/*
 * Function Name:   reportCwCalculateDistanceHigh
 * Purpose:			Distance of each CW track is calculated here us FSPL formulae
 * Inputs:          All the CW tracks being tracked
 * 					Threat Library
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void reportCwCalculateDistanceHigh(vector<TrackDefaultClass> &tracksCwActual,
		LibraryStruct &library);

/*
 * Function Name:   reportCwTpAdctoDbm
 * Purpose:			CWP ADC counts are converted to dBms
 * Inputs:          ADC Counts
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
F64 reportCwTpAdctoDbm(F64 adcCount);

/*
 * Function Name:   reportCwDeleteSpursTracks
 * Purpose:			Spurs i.e. RF image CW tracks are found and deleted
 * 					To be called a spur, both RFs should be in different bands
 * 					and are multiples of eachother by +- 2 MHz
 * Inputs:          All the CW tracks being tracked
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void reportCwDeleteSpursTracks(vector<TrackDefaultClass> &tracksCwReported);


/*
 * Function Name:   reportCwDeleteSpursTracks
 * Purpose:			A simple averaging filter is applied on every iteration to
 * 					avoid jumps in distance
 * Inputs:          All the CW tracks being tracked
 * Outputs:         none
 * Scope:           Can only be called by any Delegate
 */
void reportCwSmoothDistanceWithAveraging(
		vector<TrackDefaultClass> &tracksCwReported);

#endif /* REPORTCWHIGHDELEGATE_H_ */
