/*
 * pdwsequenceclass.cpp
 *
 *  Created on: Feb 21, 2019
 *      Author: hassan
 */

#define PDWSEQUENCECLASS_CPP_

#include "../vo/pdwsequenceclass.h"


//default constructor
PdwSequenceClass::PdwSequenceClass() {
	this->startIndex = 0;
	this->midIndex = 0;
	this->endIndex = 0;
	this->startIndexPdwIndex = 0;
	this->depth = 0;
	this->sequenceCount = 0;
}

PdwSequenceClass::~PdwSequenceClass() {
	// TODO Auto-generated destructor stub
}


bool ToaDiffSequenceClass::cmpSequencePriority(
		const ToaDiffSequenceClass & a,
		const ToaDiffSequenceClass & b)
{
	return a.repititionCounter < b.repititionCounter;
}

bool PdwSequenceClass::cmpSequencePriority(const PdwSequenceClass & a, const PdwSequenceClass & b)
{
	return a.sequenceCount<b.sequenceCount;
}


