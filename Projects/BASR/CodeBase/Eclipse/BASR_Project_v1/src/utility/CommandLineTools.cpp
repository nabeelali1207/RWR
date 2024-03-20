/*
 * CommandLineTools.cpp
 *
 *  Created on: Jan 23, 2023
 *      Author: user
 */

#include "CommandLineTools.h"

#include "dirent.h"

bool CommandLineTools::checkFolderExistence(string _path)
{
//	if (boost::filesystem::is_directory(boost::filesystem::path(_path)))
//	{
//		return true;
//	}
	return false;
}
bool CommandLineTools::checkFileExistence(string _path)
{

//	if (boost::filesystem::is_regular_file(boost::filesystem::path(_path)))
//	{
//		return true;
//	}
	return false;
}

bool CommandLineTools::createFolders(string _path)
{
	return false;
	//return boost::filesystem::create_directories(boost::filesystem::path(_path));
}

string CommandLineTools::getCurrentDirectyPath(string path)
{
//	boost::filesystem::path full_path(boost::filesystem::current_path());
//	return full_path.string();
	return "";
	/*namespace fs = boost::filesystem;
	 fs::path full_path = fs::system_complete("../asset/toolbox");*/
}

bool CommandLineTools::deleteFile(string _path)
{
	if (checkFileExistence(_path))
	{
//		if (boost::filesystem::remove(boost::filesystem::path(_path)))
//		{
//			return true;
//		}
	}
	return false;
}
vector<string> CommandLineTools::getAllFilesInDirectory(string _dirPath,
		string filter)
{
	std::vector<string> names;

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(_dirPath.c_str())) != NULL)
	{
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL)
		{
			string fileName = ent->d_name;
			if (fileName.find(filter) != string::npos)
			{
				names.push_back(fileName);
			}
		}
		closedir(dir);
	}

//	namespace fs = boost::filesystem;
//	fs::path p(_dirPath);
//	std::vector<fs::directory_entry> v;

//	if (is_directory(p))
//	{
//		copy(fs::directory_iterator(p), fs::directory_iterator(), std::back_inserter(v));
//
//		for (std::vector<fs::directory_entry>::const_iterator it = v.begin(); it != v.end(); ++it)
//		{
//			if (filter == "")
//			{
//				names.push_back((*it).path().string());
//			}
//			else if ((*it).path().string().find(filter) != string::npos)
//			{
//				names.push_back((*it).path().string());
//			}
//		}
//	}
	return names;
}

uint64_t CommandLineTools::getThreadId()
{
	return 0;
//	uint64_t tId = std::hash<std::thread::id>{}(std::this_thread::get_id());
//	return tId;
}
