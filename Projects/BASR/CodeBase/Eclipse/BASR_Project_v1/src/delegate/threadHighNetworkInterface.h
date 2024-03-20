/*
 * threadHighNetworkInterface.h
 *
 *  Created on: Jan 27, 2023
 *      Author: gmd
 */

#ifndef THREADHIGHNETWORKINTERFACE_H_
#define THREADHIGHNETWORKINTERFACE_H_


#include "../io/highNetworkInterface.h"

#include "../model/model.h"

void * threadHighNetworkInterface(void *);
void highNetworkInterfacePeriodUpdateState();

void sendDummyMessagesToHighNetworkInterface();

#endif /* THREADHIGHNETWORKINTERFACE_H_ */
