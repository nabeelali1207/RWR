/*
 * inputmatlab.h
 *
 *  Created on: Oct 12, 2018
 *      Author: hassan
 */

#ifndef FILEINPUTMATLAB_H_
#define FILEINPUTMATLAB_H_

//#include <matio.h>
#include "../common/common.h"
#include "../vo/pdwdefaultclass.h"
#include <queue>
void readPdwAllMatFileOfBasr(const char *matInFileName, const char *csvOutFileName,
		list<PdwDefaultClass> &inputPdwList, uint8_t writeOutputFlag);

void readAllPdwsBasrHexFile(const char *fileName,std::queue<PdwDefaultClass> &inputPdwList);
void readAllPdwsBasrHexFile(const char *fileName,vector<PdwSmall32> &inputPdws);
#ifdef FILEINPUTMATLAB_CPP_

#else

#endif

#endif /* FILEINPUTMATLAB_H_ */
