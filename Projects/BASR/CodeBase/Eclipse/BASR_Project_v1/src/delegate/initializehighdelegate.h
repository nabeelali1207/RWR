/*
 * initializehighdelegate.h
 *
 *  Created on: Sep 25, 2019
 *      Author: hassan
 */

#ifndef INITIALIZEHIGHDELEGATE_H_
#define INITIALIZEHIGHDELEGATE_H_

#include "../common/common.h"
#include "../model/model.h"
#include "../delegate/datascaledelegate.h"
#include "../delegate/timedelegate.h"
#include "../delegate/datainputdelegate.h"
#include "../delegate/dataoutputdelegate.h"
#include "../delegate/analysislibrarydelegate.h"
#include "../delegate/hardwaredelegate.h"
#include "../vo/loghighclass.h"
#include "../vo/libraryclass.h"
#include "simulationhighdelegate.h"
#include "threadmainhighdelegate.h"
#include "threadHighNetworkInterface.h"
#include "../utility/threadutility.h"
void initializeHighDelegate();



#ifdef INITIALIZEHIGHDELEGATE_CPP_
//global variables
pthread_t cpuMainThread;
pthread_t cpuNetworkInterfaceThread;

pthread_attr_t cpuMainThreadAttrib;
pthread_attr_t cpuNetworkInterfaceThreadAttrib;

cpu_set_t cpuMainThreadCpuSet;
cpu_set_t cpuNetworkInterfaceThreadCpuSet;

#else
//externs
#endif

#endif /* INITIALIZEHIGHDELEGATE_H_ */
