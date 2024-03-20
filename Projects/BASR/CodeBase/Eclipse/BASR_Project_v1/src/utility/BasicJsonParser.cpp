/*
 * CustomJsonClass.cpp
 *
 *  Created on: Jan 25, 2023
 *      Author: user
 */


#define CJ5_IMPLEMENT
#include "c5j.h"

#include "BasicJsonParser.h"

BasicJsonParser::BasicJsonParser(uint32_t maxElementCount)
{

	maxElemCount = maxElementCount;
	isValidJson = false;
	elementCount = 0;
	tokensVector.resize(maxElemCount);
	tempChildrenVector.reserve(maxElementCount);
}

bool BasicJsonParser::parseFromString(std::string &message)
{
	cj5_result r = cj5_parse(&message[0], (int32_t) message.length(),
			&tokensVector[0], maxElemCount);

	if (r.error == CJ5_ERROR_NONE)
	{
		isValidJson = true;
		tokensVector.resize(r.num_tokens);
		nameToIdMap.clear();

		uint32_t arrayIndex = 0;
		for (uint32_t i = 0; i < (uint32_t) r.num_tokens; i++)
		{


			const char* g_token_types[] = { "CJ5_TOKEN_OBJECT", "CJ5_TOKEN_ARRAY", "CJ5_TOKEN_NUMBER",
			                                "CJ5_TOKEN_STRING", "CJ5_TOKEN_BOOL",  "CJ5_TOKEN_NULL" };
            char content[1024];
            int num = tokensVector[i].end - tokensVector[i].start;
            memcpy(content, &message[tokensVector[i].start], num);
            content[num] = '\0';
			printf("%d: { type = %s, size = %d, parent = %d,(s:%d,e:%d) content = '%s' \n", i,
			                   g_token_types[tokensVector[i].type], tokensVector[i].size, tokensVector[i].parent_id, tokensVector[i].size, tokensVector[i].end, content);


			cj5_token myToken = tokensVector[i];
			if (myToken.parent_id > 0)
			{
				cj5_token parentToken = tokensVector[myToken.parent_id];
				if (parentToken.parent_id == 0
						&& parentToken.type == CJ5_TOKEN_STRING && myToken.type != CJ5_TOKEN_ARRAY)
				{
					string newElemName(&message[parentToken.start],
							&message[parentToken.end]);
					string newElemValue(&message[myToken.start],
							&message[myToken.end]);
					placeElement(newElemName, newElemValue);

					arrayIndex = 0;
				}

				else if (parentToken.parent_id > 0)
				{
					cj5_token grandParentToken =
							tokensVector[parentToken.parent_id];
					if (grandParentToken.parent_id == 0
							&& parentToken.type == CJ5_TOKEN_ARRAY)
					{
						string arrayName(&message[grandParentToken.start],
								&message[grandParentToken.end]);
						string newElemValue(&message[myToken.start],
								&message[myToken.end]);
						string name = arrayName + "[" + std::to_string(arrayIndex) + "]";
						placeElement(name, newElemValue);
						arrayIndex++;
						string elemCount = std::to_string(arrayIndex);
						placeElement(arrayName, elemCount);
					}
					else
					{
						arrayIndex = 0;
					}
				}
				else
				{
					arrayIndex = 0;
				}

			}
		}
	}
	else
	{
		isValidJson = false;
	}
	return isValidJson;
}
