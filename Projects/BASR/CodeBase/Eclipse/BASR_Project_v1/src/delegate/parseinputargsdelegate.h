/*
 * parseinputargsdelegate.h
 *
 *  Created on: Sep 28, 2022
 *      Author: gmd
 */

#ifndef PARSEINPUTARGSDELEGATE_H_
#define PARSEINPUTARGSDELEGATE_H_

#include "../model/model.h"
#include "../common/common.h"
#include "../utility/constant.h"
#include "../utility/consthigh.h"
#include "../utility/constlow.h"
#include <sstream>

//functions, includes and defines
bool setupSystemConstantsFromCommandLine(int argumentCount, char ** argumentValue);
bool readParametersFromFile(ifstream &iFile);

#endif /* PARSEINPUTARGSDELEGATE_H_ */
