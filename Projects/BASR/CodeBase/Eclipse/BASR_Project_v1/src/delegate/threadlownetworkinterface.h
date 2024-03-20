/*
 * threadlownetworkinterface.h
 *
 *  Created on: Jan 23, 2023
 *      Author: gmd
 */

#ifndef THREADLOWNETWORKINTERFACE_H_
#define THREADLOWNETWORKINTERFACE_H_

#include "../io/lowNetworkInterface.h"

#include "../model/model.h"

void * threadLowNetworkInterface(void *);
void lowNetworkInterfacePeriodicUpdateState();

bool loadPdwsForPlayback();

void sendDummyMessagesToLowNetworkInterface();
#endif /* THREADLOWNETWORKINTERFACE_H_ */
