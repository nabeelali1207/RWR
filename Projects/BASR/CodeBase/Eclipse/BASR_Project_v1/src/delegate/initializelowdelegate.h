/*
 * initializelowdelegate.h
 *
 *  Created on: Oct 15, 2018
 *      Author: hassan
 */

#ifndef INITIALIZELOWDELEGATE_H_
#define INITIALIZELOWDELEGATE_H_

#include "../common/common.h"
#include "../model/model.h"
#include "../delegate/datascaledelegate.h"
#include "../delegate/timedelegate.h"
#include "../delegate/datainputdelegate.h"
#include "../delegate/dataoutputdelegate.h"
#include "../delegate/hardwaredelegate.h"
#include "../vo/loglowclass.h"
#include "simulationlowdelegate.h"
#include "threadmainlowdelegate.h"
#include "threadiohandlerlowdelegate.h"
#include "../utility/threadutility.h"

#include "../delegate/threadlownetworkinterface.h"
void initializeLowDelegate();

#ifdef INITIALIZELOWDELEGATE_CPP_
//global variables
pthread_t dspMainThread;
pthread_t dspPdwsThread;
pthread_t dspNetworkInterfaceThread;

pthread_attr_t dspMainThreadAttrib;
pthread_attr_t dspPdwsThreadAttrib;
pthread_attr_t dspNetworkInterfaceThreadAttrib;

cpu_set_t dspMainThreadCpuSet;
cpu_set_t dspPdwsThreadCpuSet;
cpu_set_t dspNetworkInterfaceThreadCpuSet;

#else
//externs
#endif



#endif /* INITIALIZELOWDELEGATE_H_ */
