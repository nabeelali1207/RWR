/*
 * CommandLineTools.h
 *
 *  Created on: Jan 23, 2023
 *      Author: user
 */

#ifndef COMMANDLINETOOLS_H_
#define COMMANDLINETOOLS_H_

#include <string>
#include <vector>
using std::string;
using std::vector;

class CommandLineTools
{
public:

	static string executeInCommandLine(string command);
	static bool checkFolderExistence(string path);
	static bool checkFileExistence(string path);
	static vector<string> getAllFilesInDirectory(string _dirPath, string filter);

	static string getCurrentDirectyPath(string path);
	static bool createFolders(string path);

	static bool deleteFile(string path);

	static uint64_t getThreadId();
};

#endif /* COMMANDLINETOOLS_H_ */
