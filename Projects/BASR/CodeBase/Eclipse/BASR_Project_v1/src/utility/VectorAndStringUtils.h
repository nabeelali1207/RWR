/*
 * VectorAndStringUtils.h
 *
 *  Created on: Jan 23, 2023
 *      Author: user
 */

#ifndef VECTORANDSTRINGUTILS_H_
#define VECTORANDSTRINGUTILS_H_

#include <vector>
#include <string>
#include <functional>

class VectorAndStringUtils
{
public:
	template<class T>
	static std::string vectorToString(const std::vector<T>&v,char delim=',');

	static bool isNumber(const std::string& s)
	{
	    return s.find_first_not_of("0123456789") == std::string::npos;
	}


	template<class T>
	static void removeDuplicates(const std::vector<T>&inVector,std::vector<T>&outVector,T leeway=0);
};


template<class T>
std::string VectorAndStringUtils::vectorToString(const std::vector<T>&v,char delim)
{
	std::string res;
	res.reserve(v.size()*10);
	for(uint32_t i=0;i<v.size();i++)
	{
		res+=to_string(v[i]);
		res+=delim;
	}
	return res;
}
template<>
std::string VectorAndStringUtils::vectorToString<std::string>(const std::vector<std::string>&v,char delim);



template<class T>
void VectorAndStringUtils::removeDuplicates(const std::vector<T>&inVector,std::vector<T>&outVector,T leeway)
{
	outVector.resize(0);
	outVector.reserve(inVector.size());
	if(inVector.size()==0)
		return;
	outVector.push_back(inVector.front());
	for(uint32_t i=1;i<inVector.size();i++)
	{
		T& last = outVector.back();

		if(max(last,inVector[i])-min(last,inVector[i])>leeway)
		{
			outVector.push_back(inVector[i]);
		}
	}
}
//template<>
//void VectorAndStringUtils::removeDuplicates<uint32_t>(const std::vector<uint32_t>&inVector,std::vector<uint32_t>&outVector,uint32_t leeway);
#endif /* VECTORANDSTRINGUTILS_H_ */
