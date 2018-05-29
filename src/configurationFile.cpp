/*
 * configurationFile.cpp
 *
 *  Created on: Sep 10, 2009
 *      Author: jdellaria
 */

#include <sys/stat.h>
#include <sys/dir.h>
#include <errno.h>
#include <dirent.h>
#include <vector>
#include <fstream>
#include <iostream>

#include "configurationFile.h"
using namespace std;



configurationFile::configurationFile()
{
	fileLines.clear();
}

configurationFile::~configurationFile()
{
	// TODO Auto-generated destructor stub
}


/*
int configurationFile::read(char *tName)
{
	for (int i = 0;i<strlen(tName);i++)
	{
		fileName.push_back(tName[i]);
	}
	std::cout << fileName + "\n";
}
*/

// Read a file into a vector

void configurationFile::help(char *program) {
	std::cout << program;
	std::cout << ": Need a filename for a parameter.\n";
}

int configurationFile::getLines(char *Name)
{
	std::string line;

	std::ifstream infile (Name, std::ios_base::in);
	if(!infile)
	{
		cout << "Cannot open file" << Name << "\n";
		return 0;
	}
	infile.sync();

	while (getline(infile, line, '\n'))
	{
		fileLines.push_back (line);
	}
	infile.close();
	return 1;
}

int configurationFile::clearLines()
{
	fileLines.clear();
}
/*
int configurationFile::getLines()
{
	std::string line;

	std::ifstream infile (fileName.c_str(), std::ios_base::in);
	infile.sync();
	while (getline(infile, line, '\n'))
	{
		fileLines.push_back (line);
	}
	infile.close();
	return 1;
}
*/
