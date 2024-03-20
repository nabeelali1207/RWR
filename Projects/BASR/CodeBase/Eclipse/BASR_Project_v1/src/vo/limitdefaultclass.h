/*
 * limitdefaultclass.h
 *
 *  Created on: Dec 30, 2018
 *      Author: test
 */

#ifndef LIMITDEFAULTCLASS_H_
#define LIMITDEFAULTCLASS_H_

#include "../common/common.h"


enum class LimitTypeEnum {type_default, type_pri, type_rf, type_pw};
enum class LimitRangeEnum {range_discrete, range_continuous};

class LimitDefaultClass {
public:
	LimitDefaultClass();
	~LimitDefaultClass();
};


#endif /* LIMITDEFAULTCLASS_H_ */
