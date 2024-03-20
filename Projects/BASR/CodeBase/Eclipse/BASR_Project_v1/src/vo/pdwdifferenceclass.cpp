/*
 * pdwdifferenceclass.cpp
 *
 *  Created on: Feb 19, 2019
 *      Author: hassan
 */

#define PDWDIFFERENCECLASS_CPP_

#include "../vo/pdwdifferenceclass.h"


bool PdwDifferenceClass::cmpDiffToa(const PdwDifferenceClass & a,
		const PdwDifferenceClass & b) {
	return a.diffToa < b.diffToa;
}


//default constructor
PdwDifferenceClass::PdwDifferenceClass() {

	this->pdwIndex = 0;
	this->diffToa = -1;
	this->diffToaCompare = -1;
	//this->timeOfArrivalStart = 0;
}

PdwDifferenceClass::~PdwDifferenceClass() {
	// TODO Auto-generated destructor stub
}




