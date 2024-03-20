/*
 * threatprocessorselftestdelegate.h
 *
 *  Created on: Feb 23, 2021
 *      Author: saad
 */

#ifndef THREATPROCESSORSELFTESTDELEGATE_H_
#define THREATPROCESSORSELFTESTDELEGATE_H_

#include "../delegate/selftesthighdelegate.h"
#include "../vo/datasensorclass.h"

/*
 * Function Name:   fillPacketInfofindTpSelfTestEmitters
 * Purpose:			This function goes through all the EJ emitters and extracts
 * 					parameters of self test emitters, if self test emitters are
 * 					according to VDM specied criterion, self test is passed
 * Inputs:          SelfTestControllerClass
 * 					DataSensorClass of Band EJ
 * Outputs:         Pulsed self test results
 * Scope:           Can only be called by any Delegate
 */
void fillPacketInfofindTpSelfTestEmitters(
		SelfTestHighDelegate &stDelegate, DataSensorClass &bandEJ);

#endif /* DELEGATE_THREATPROCESSORSELFTESTDELEGATE_H_ */
