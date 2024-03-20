/*
 * systemstorage.h
 *
 *  Created on: Nov 2, 2018
 *      Author: hassan
 */

#ifndef SYSTEMSTORAGE_H_
#define SYSTEMSTORAGE_H_

#include "../common/common.h"

void mountUsbDrive();
void mountSsdDrive();
void copyActualTlToEmmc();
void copyDummyTlFromEmmc();
void formatTlPartition();
void formatFrPartition();

//All definitions
#ifdef SYSTEMSTORAGE_CPP_
//All file scope variables
#else
//Global variables using extern
#endif
//End of conditional exclusion

#endif /* SYSTEMSTORAGE_H_ */
