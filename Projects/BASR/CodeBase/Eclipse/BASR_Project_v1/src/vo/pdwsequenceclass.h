/*
 * pdwsequenceclass.h
 *
 *  Created on: Feb 21, 2019
 *      Author: hassan
 */

#ifndef PDWSEQUENCECLASS_H_
#define PDWSEQUENCECLASS_H_

#include "../common/common.h"

class PdwSequenceClass {
public:

	//diffToaSorted indices
	uint16_t startIndex;
	uint16_t midIndex;
	uint16_t endIndex;
	uint16_t startIndexPdwIndex;
	uint16_t depth;
	uint16_t sequenceCount;

	//int64_t diffToaCompare;

	static bool cmpSequencePriority(const PdwSequenceClass & a, const PdwSequenceClass & b);

	PdwSequenceClass();
	virtual ~PdwSequenceClass();
};
class ToaDiffSequenceClass
{
public:
	uint16_t firstPdwIndex;
	uint16_t repititionCounter;
	uint16_t consectiveSeqOccurance;
	uint16_t sequenceID;
	int64_t valAvg;
	ToaDiffSequenceClass(void):firstPdwIndex(0),repititionCounter(0),consectiveSeqOccurance(0),sequenceID(0),valAvg(0)
	{

	}
	static bool cmpSequencePriority(const ToaDiffSequenceClass & a, const ToaDiffSequenceClass & b);

};
class PriSequenceFrame
{
public:
	vector<int64_t> 	sequenceValues;
	vector<uint16_t> 	sequenceRepetition;
};
class PriSequence
{
public:
	vector<PriSequenceFrame> priFrames;

};




#endif /* PDWSEQUENCECLASS_H_ */
