/*
 * configurationFile.h
 *
 *  Created on: Sep 10, 2009
 *      Author: jdellaria
 */

#ifndef CONFIGURATIONFILE_H_
#define CONFIGURATIONFILE_H_
#include <string>
#include <string.h>
#include <vector>



class configurationFile {
public:
	configurationFile();
	virtual ~configurationFile();

	void help(char *);
	int getLines(char *);
	int clearLines();

	std::vector<std::string> fileLines;
	std::string fileName;

};

#endif /* CONFIGURATIONFILE_H_ */
