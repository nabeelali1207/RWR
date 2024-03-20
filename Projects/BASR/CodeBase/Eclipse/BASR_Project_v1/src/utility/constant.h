/*
 * constant.h
 *
 *  Created on: Jan 1, 2019
 *      Author: hassan
 */

#ifndef CONSTANT_H_
#define CONSTANT_H_

#include "../common/common.h"

//notes
//constants may need to be defined using the const keyword to avoid dynamic
//..allocation problems when using these values to initialize container sizes
//end of notes

class Constant
{
public:

	//System Constants
	static uint8_t configureToRunOnHwFlag;
	static uint8_t configureToReadPdwsFromFileFlag;
	static uint8_t configureForTwoBandProcessingFlag;
	static uint8_t systemdelegateSelectLowHigh;

	static uint8_t simParametersByScript;
	static uint8_t systemBandCountOperate;
	static uint8_t systemBandCountInput;
	static uint8_t systemVersionOperate;
	static uint8_t systemVersionInput;
	static uint64_t systemOneToaClockNs;
	static uint64_t systemOnePwClockNs;
	static uint64_t systemOneAdcPowerMax;
	static uint64_t systemTwoToaClockNs;
	static uint64_t systemTwoPwClockNs;
	static uint64_t systemTwoAdcPowerMax;

	static string parameterFileName;

	Constant();
	virtual ~Constant();
};

//Definitions
#ifdef CONSTANT_CPP_

//Allocation for static variables of model
Constant constant;

//System Constants
uint8_t Constant::configureToRunOnHwFlag = 0;
uint8_t Constant::configureToReadPdwsFromFileFlag = 0;
uint8_t Constant::configureForTwoBandProcessingFlag = 0;
uint8_t Constant::systemdelegateSelectLowHigh = 1;
uint8_t Constant::simParametersByScript=0;
uint8_t Constant::systemBandCountOperate = 4;
uint8_t Constant::systemBandCountInput = 2;
uint8_t Constant::systemVersionOperate = 2;
uint8_t Constant::systemVersionInput = 1;
uint64_t Constant::systemOneToaClockNs = 50;
uint64_t Constant::systemOnePwClockNs = 25;
uint64_t Constant::systemOneAdcPowerMax = 256;
uint64_t Constant::systemTwoToaClockNs = 10;
uint64_t Constant::systemTwoPwClockNs = 10;
uint64_t Constant::systemTwoAdcPowerMax = 4096;

string Constant::parameterFileName = "configs.cfg";

#else
//Global
#endif
//End

#endif /* CONSTANT_H_ */
