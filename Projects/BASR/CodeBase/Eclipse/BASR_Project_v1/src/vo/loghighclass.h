/*
 * lowhighclass.h
 *
 *  Created on: Sep 26, 2019
 *      Author: hassan
 */

#ifndef LOGHIGHCLASS_H_
#define LOGHIGHCLASS_H_

#include "../common/common.h"

class LogHighClass
{
public:

	LogHighClass();
	virtual ~LogHighClass();
};

//All definitions
#ifdef LOGHIGHCLASS_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion

#endif /* LOGHIGHCLASS_H_ */
