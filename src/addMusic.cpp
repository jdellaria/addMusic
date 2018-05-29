//============================================================================
// Name        : addMusic.cpp
// Author      : Jon Dellaria
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <DLog.h>
#include "configurationFile.h"
#include "mp3tags.h"

DLog myLog;

using namespace std;

int main(int argc, char *argv[])
{
	cout << "Add Music to Library" << endl;
	mp3Tags myTags;
	configurationFile myConfig;

	myLog.logValue = logInformation;
	myLog.logFileName = "/home/jdellaria/workspace/addMusic/Release/log.txt";
	myLog.printFile = true;
	myLog.printScreen = true;
	myLog.printTime = true;
	if (argc >= 2) // if there is an argument, then assume it is the configuration file
	{
		myConfig.getLines(argv[1]);
	}
	else //otherwise assume there is a file in the default with the name "config.conf"
	{
		myConfig.getLines("./config.conf");
	}
	if (myConfig.fileLines.size() == 2)
	{
		std::cout << "Source Directory - " << myConfig.fileLines[0] << std::endl;
		std::cout << "Destination Directory - " << myConfig.fileLines[1] << std::endl;
//		myTags.Recurse((char* const )"/Shared/New Albums/", (char* const )"/RAID/Music/Album Music/", "Album");
		myTags.Recurse((char* const )myConfig.fileLines[0].c_str(), (char* const )myConfig.fileLines[1].c_str(), "Album");
	}
	else
	{
		std::cout << "Config file must contain 2 lines. Source and destination directories." << std::endl;
	}
	myConfig.clearLines();
}
