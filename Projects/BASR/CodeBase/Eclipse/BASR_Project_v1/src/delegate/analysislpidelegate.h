/*
 * analysislpidelegate.h
 *
 *  Created on: Aug 24, 2022
 *      Author: root
 */
#include "../common/common.h"
#include "../vo/pdwsequenceclass.h"
#include "../vo/trackdefaultclass.h"
#include "../utility/utility.h"
#include "../utility/constant.h"
#ifndef SRC_DELEGATE_ANALYSISLPIDELEGATE_H_
#define SRC_DELEGATE_ANALYSISLPIDELEGATE_H_

void lpiUpdateTrackLpiOnPulseAssociation(TrackDefaultClass &track,
			PdwDefaultClass &newPdw);
	IntraPulseType lpiAnalysisExecute(TrackDefaultClass &track);
	vector<PdwDefaultClass> lpiGetPdwsForAnalysis(TrackDefaultClass &track);

#endif /* SRC_DELEGATE_ANALYSISLPIDELEGATE_H_ */
