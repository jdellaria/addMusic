/*
 * mp3tags.cpp
 *
 *  Created on: Dec 26, 2008
 *      Author: jdellaria
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>

#include <cstdlib>
#include <sys/dir.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#include "mp3tags.h"
#include <fileref.h>
#include <tag.h>
#include "musicdb.h"
#include <Directory.h>
#include <File.h>
#include <DLog.h>
#include <string>
#include <regex.h>
#include <errno.h>

extern DLog myLog;

using namespace std;
mp3Tags::mp3Tags()
{
}

mp3Tags::~mp3Tags()
{
}

int newAlbum = 1;
long lAlbum = 0;
long lArtist = 0;
int gotCoverJPG = 0;
string destinationDir;
string jpgThumbName = "none";
string jpgName = "none";

void recurseDo(char const* directoyEntry, int  directoyEntryType);
void deleteDirectoryDo(char const* directoyEntry, int directoyEntryType);
int isMP3(char* const fileName);
int isJPG(char* const fileName);
int get(char* const fileName, char* const grouping);
musicDB myDB;

int mp3Tags::Recurse(char const* startDirectory, char const* destinationDirectory, char const* grouping)
{
	Directory myDirectory;
	string recurseDirectory ;
	string message;
	char intbuffer[50];

	newAlbum = 1;
	destinationDir = destinationDirectory;
	myDirectory.Recurse(startDirectory, recurseDo);

	if(myLog.numberOfErrors == 0)
	{
		printf("No Errors\n");

		myDB.CommitSongsToLibrary();
		cout << "recurse: myDB.CommitSongsToLibrary()"  << endl;
		myDB.RemoveSongsFromPreSongLibrary();
		myDirectory.Recurse(startDirectory, deleteDirectoryDo);
	}
	else
	{
		message = "There were ";
		sprintf(intbuffer,"%d", myLog.numberOfErrors);
		message.append(intbuffer);
		message.append(" errors found.");
		myLog.print(logWarning, message);
	}

}

void deleteDirectoryDo(char const* directoyEntry, int directoyEntryType)
{
	Directory myDirectory;
	string message;

	if (directoyEntryType == DIRECTORYENTRYTYPE_DIR)
	{
		message = "Deleting directory ";
		message.append(directoyEntry);
		myLog.print(logWarning, message);
		myDirectory.Remove(directoyEntry);
	}
}

void recurseDo(char const* directoyEntry, int directoyEntryType)
{
	File myFile;
	string fileName;
	string destinationFileName = "none";
	size_t found;
	string sourceFile;
	string tempString;
	string AlbumName;
	string ArtistName;
	string message;
	int returnValue;

//	string startDirectory ="/home/jdellaria/Desktop/Jon/";
	char albumIDString[10];

	sourceFile = directoyEntry;

	cout << "recurseDo: "  << endl;
//	printf("%s - Directory Entry Type = %d\n",  directoyEntry,  directoyEntryType);
	if (directoyEntryType == DIRECTORYENTRYTYPE_DIR)
	{
		newAlbum = 1; // New directory means new Album
		gotCoverJPG = 0;
		cout << "recurseDo: directoyEntryType == DIRECTORYENTRYTYPE_DIR"  << endl;
//		printf("New Album!!! %s\n", directoyEntry);
	}
	if (directoyEntryType == DIRECTORYENTRYTYPE_REG)
	{
		found = sourceFile.find_last_of("/\\");
		fileName = sourceFile.substr(found+1);
		tempString = sourceFile.substr(0,found);

		found = tempString.find_last_of("/\\");
		AlbumName = tempString.substr(found+1);
		tempString = tempString.substr(0,found);

		found = tempString.find_last_of("/\\");
		ArtistName = tempString.substr(found+1);
		tempString = tempString.substr(0,found);
		destinationFileName = destinationDir + ArtistName + "/" + AlbumName + "/" + fileName;

		cout << "Artist: " << ArtistName << " - Album: " << AlbumName << endl;
		printf("%s\n%s\n\n", directoyEntry,destinationFileName.c_str());
		returnValue = myFile.copy(sourceFile,destinationFileName);

		if (returnValue == 0) // if success
		{
			message = "Copying file:";
			message.append(sourceFile);
			message.append(" to -> ");
			message.append(destinationFileName);
			myLog.print(logInformation, message);
		}
		else
		{
			message = "Error copying file:";
			message.append(myFile.errorMessageBuffer);
			message.append(": ");
			message.append(sourceFile);
			message.append(" to -> ");
			message.append(destinationFileName);
			myLog.print(logError, message);
		}

		printf("%s-->%s\n", directoyEntry,destinationFileName.c_str());
		printf("-----%s - returned %d errno %d\n\n",destinationFileName.c_str(),returnValue,errno);

		if (isMP3((char* const)destinationFileName.c_str()))
		{
			cout << "recurseDo: isMP3"  << endl;
			get ((char* const)destinationFileName.c_str(), (char* const)"Album");
			sprintf(albumIDString, "%ld", lAlbum);
			jpgThumbName = destinationDir + ArtistName + "/" + AlbumName + "/" + albumIDString + ".w300.thumb";
			myDB.setThumbLocation ((char* const)jpgThumbName.c_str());
			myDB.updateAlbumCover();
			cout << "updateAlbumCover: " << jpgThumbName  << endl;
		}
		if (isJPG((char* const)destinationFileName.c_str()))
		{
			if (gotCoverJPG == 0)
			{
				jpgName = sourceFile;
			}
			if (newAlbum == 0) //make sure that we have album id's
			{
				returnValue = myFile.copy(jpgName,jpgThumbName);

				if (returnValue == 0) // if success
				{
					message = "Copying file:";
					message.append(jpgName);
					message.append(" to -> ");
					message.append(jpgThumbName);
					myLog.print(logInformation, message);
				}
				else
				{
					message = "Error copying file:";
					message.append(myFile.errorMessageBuffer);
					message.append(": ");
					message.append(jpgName);
					message.append(" to -> ");
					message.append(jpgThumbName);
					myLog.print(logError, message);
				}
			}
			if (strcasecmp (fileName.c_str(),"cover.jpg") == 0)
			{
				printf("got Cover.jpg");
				gotCoverJPG = 1;
			}
			if (strcasecmp (fileName.c_str(),"cover.jpeg") == 0)
			{
				printf("got Cover.jpeg");
				gotCoverJPG = 1;
			}
			printf("thumb %s\n\n", jpgThumbName.c_str());
		}
	}
}

int get(char* const fileName,char* const grouping)
{

	TagLib::FileRef f(fileName);

	cout << "recurseDo: get fileName: "  << fileName << endl;
	if(!f.isNull() && f.tag())
	{

		TagLib::Tag *tag = f.tag();

		myDB.setSongName ( tag->title());
		cout << "Song Name: " << tag->title()  << endl;
		myDB.setArtist (tag->artist());
		cout << "artist(): " << tag->artist()  << endl;
		myDB.setAlbum (tag->album());
		cout << "tag->album() " << tag->album()  << endl;
		myDB.setArtist (tag->artist());
		cout << "tag->artist() " << tag->artist()  << endl;
		myDB.setSongYear (tag->year());
		cout << "tag->year(): " << tag->year()  << endl;
		myDB.setTrackNumber (tag->track());
		cout << "tag->track(): " << tag->track()  << endl;
		myDB.setGenre (tag->genre());
		cout << "tag->genre(): " << tag->genre()  << endl;
		myDB.setLocation (fileName);


		if (strcmp ("Album", grouping) == 0)
		{
			if (newAlbum == 1)
			{
				newAlbum = 0;
				lAlbum = myDB.addAlbum();
				myDB.setAlbumId(lAlbum);
				cout << "addAlbum: "  << endl;

			}
			lArtist = myDB.addArtist();
			myDB.setArtistId(lArtist);

		}
		else
		{
			myDB.setArtistId(0);
			myDB.setAlbumId(0);
		}
		myDB.setGrouping(grouping);

	}

	if(!f.isNull() && f.audioProperties())
	{
		TagLib::AudioProperties *properties = f.audioProperties();

		myDB.setBitRate (properties->bitrate());
		myDB.setSampleRate (properties->sampleRate());
		myDB.setSongTime (properties->length());
	}
	myDB.addSongToPreSongLibrary();
	return 0;
}

int isMP3(char* const fileName)
{
	if (strcasecmp (&fileName[strlen(fileName)-3],"MP3") == 0)
	{
		return (1);
	}
	return(0);
}


int isJPG(char* const fileName)
{
	if (strcasecmp (&fileName[strlen(fileName)-3],"JPG") == 0)
	{
		return (1);
	}
	if (strcasecmp (&fileName[strlen(fileName)-4],"JPEG") == 0)
	{
		return (1);
	}
	return(0);
}
