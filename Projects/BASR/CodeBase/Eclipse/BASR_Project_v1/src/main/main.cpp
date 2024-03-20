//============================================================================
// Name        : main.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#define MAIN_CPP_

#include "../main/main.h"


int main(int argumentCount, char **argumentValue)
{
	Constant::configureToRunOnHwFlag = 0;
	Constant::configureToReadPdwsFromFileFlag = 1;

	//Utility::setDate("2021-04-07 15:02:24");
	if (setupSystemConstantsFromCommandLine(argumentCount, argumentValue)
			== false)
	{
		std::cout << "Invalid Inputs Specs \n Exiting" << std::endl;
		return -1;
	}

	Utility::printTime();
	std::cout << std::endl;
	if (Constant::simParametersByScript == 0)
	{
		Constant::systemdelegateSelectLowHigh = 1;
	}
	if (Constant::systemdelegateSelectLowHigh == 0)
	{
		std::cout << "Program Started in DSP Mode" << std::endl;
		initializeLowDelegate();
		terminateLowDelegate();
		std::cout << "Program Ended in DSP Mode" << std::endl;
	}
	if (Constant::systemdelegateSelectLowHigh == 1)
	{
		//reserve
		//cout << "Constant::simParametersByScript: " << (uint16_t)Constant::simParametersByScript << endl;
		//cout << "Constant::systemdelegateSelectLowHigh: " << (uint16_t)Constant::systemdelegateSelectLowHigh << endl;
		//cout << "ConstLow::simIterationTypeFlag: " << (uint16_t)ConstLow::simIterationTypeFlag << endl;
		//cout << "ConstHigh::simOnActualHwFlag: " << (uint16_t)ConstHigh::simOnActualHwFlag << endl;
		//cout << "ConstHigh::readingFromLvds: " << (uint16_t)ConstHigh::readingFromLvds << endl;
		//cout << "ConstHigh::dspReportFileName: " << ConstHigh::dspReportFileName << endl;
		//cout << "ConstHigh::dspReportFileName.length(): "<< ConstHigh::dspReportFileName.length() << endl;
		//end of reserve

		std::cout << "Program Started in CPU Mode" << std::endl;
		initializeHighDelegate();
		terminateHighDelegate();
		std::cout << "Program Ended in CPU Mode" << std::endl;
	}
	return 0;
}


void updateSystemRunConfigs()
{

}
