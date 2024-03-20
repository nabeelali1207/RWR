/*
 * CustomJsonClass.h
 *
 *  Created on: Jan 25, 2023
 *      Author: gmd
 */

#ifndef BASICJSONPARSER_H_
#define BASICJSONPARSER_H_

#define USE_MAPS 1

#include <string>
#include <vector>
#include "c5j.h"


#if USE_MAPS==1
#include <map>
#endif
using std::string;
using std::vector;
class BasicJsonParser
{

	uint32_t maxElemCount;

	std::vector<cj5_token> tokensVector;

	vector<uint32_t> tempChildrenVector;

public:
	BasicJsonParser(uint32_t maxElementCount = 1000);

	bool parseFromString(std::string &message);
	bool isValidJson;
	uint32_t elementCount;





#if USE_MAPS==1
	std::map<string,string> nameToIdMap;

	bool getElemValue(string &key, string& val)
	{
		std::map<string,string>::iterator it = nameToIdMap.find(key);
		if(it!=nameToIdMap.end())
		{
			val = it->second;
			return true;
		}
		return false;
	}
	void placeElement(string& name, string& val)
	{
		nameToIdMap[name]=val;
	}
#else
	std::vector<std::pair<string,string>> nameToIdMap;

	bool getElemValue(string &key, string& val)
	{
		for(uint32_t i=0;i<nameToIdMap.size();i++)
		{
			if(nameToIdMap[i].first ==name)
			{
				val = nameToIdMap[i].second;
				return true;
			}
		}
		return false;
	}
	void placeElement(string& name, string& val)
	{
		nameToIdMap.push_back(std::pair<string,string>(name,val));
	}
#endif

};

#endif /* BASICJSONPARSER_H_ */
