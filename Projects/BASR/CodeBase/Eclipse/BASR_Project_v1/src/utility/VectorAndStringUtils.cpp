/*
 * VectorAndStringUtils.cpp
 *
 *  Created on: Jan 23, 2023
 *      Author: user
 */

#include "VectorAndStringUtils.h"

template<>
std::string VectorAndStringUtils::vectorToString<std::string>(const std::vector<std::string>&v,char delim)
{
	// adds quotation marks for every index
	std::string res = "\"";
	std::string dd= "\" \"";
	dd[1]=delim;
	res.reserve(v.size()*10);
	for(uint32_t i=0;i<v.size();i++)
	{
		res+=v[i];
		res+=dd;
	}
	if(v.size()>0)
	{
		res[res.length()-2]=' ';
	}
	res[res.length()-1]=' ';

	return res;
}
