/*
 * mp3tags.h
 *
 *  Created on: Dec 26, 2008
 *      Author: jdellaria
 */

#ifndef MP3TAGS_H_
#define MP3TAGS_H_

class mp3Tags{
public:
	mp3Tags();
	~mp3Tags();
//	int get(char *fileName, char *grouping);
	int Recurse(char const* startDirectory, char const* destinationDirectory, char const* grouping);
//	void recurseDo(char* const directoyEntry, int directoyEntryType);
//	int isMP3(char* const fileName);
};

#endif /* MP3TAGS_H_ */
