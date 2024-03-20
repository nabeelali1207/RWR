/*
 * hardwaredelegate.h
 *
 *  Created on: Jan 1, 2019
 *      Author: test
 */

#ifndef HARDWAREDELEGATE_H_
#define HARDWAREDELEGATE_H_

#include "../io/systemstorage.h"
#include "../utility/constlow.h"
#include "../utility/consthigh.h"
#include "../model/model.h"

void hardwareMountUsbDrive();
void hardwareMountSsdDrive();

//All definitions
#ifdef HARDWAREDELEGATE_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion


#endif /* HARDWAREDELEGATE_H_ */
