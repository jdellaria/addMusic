//============================================================================
// Name        : addMusic.cpp
// Author      : Jon Dellaria
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <DLog.h>
#include <Directory.h>
#include <File.h>

#include "audioTags.h"
#include "configurationFile.h"
#include "musicDB.h"
DLog myLog;

using namespace std;
void doRecurseDirectory(char const * text, int x);

int newAlbum = 1;
long lAlbum = 0;
long lArtist = 0;
int gotCoverJPG = 0;
string destinationDir;
string jpgThumbName = "none";
string coverName = "none";

musicDB myDB;

int isAppleDropping(char* const fileName);
int isMP3(char* const fileName);
int isJPG(char* const fileName);
int addSongToDB(char* const fileName, char* const thumbName, char* const grouping);

void deleteDirectoryDo(char const* directoyEntry, int directoyEntryType);

int main(int argc, char *argv[])
{
	Directory myDirectory;
	audioTags myTags;
	configurationFile myConfig;
	string message;
	char intbuffer[20];

	cout << "Add Music to Library" << endl;

	myLog.logValue = logInformation;
	myLog.logFileName = "/home/jdellaria/Desktop/addMusic/Release/logAddMusic.txt";
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
//		myTags.Recurse((char* const )myConfig.fileLines[0].c_str(), (char* const )myConfig.fileLines[1].c_str(), "Album");
	}
	else
	{
		std::cout << "Config file must contain 2 lines. Source and destination directories." << std::endl;
	}

//		message = MODULE_NAME;
//		message.append(__func__);
//		message.append(title);
//		myLog.print(logWarning, message);
//		myLog.print(logWarning, message);

	destinationDir =  myConfig.fileLines[1];
	myDirectory.Recurse((char* const )myConfig.fileLines[0].c_str(), doRecurseDirectory);

	if(myLog.numberOfErrors == 0)
	{
		printf("No Errors\n");

		myDB.CommitSongsToLibrary();

		myDB.RemoveSongsFromPreSongLibrary();
		message = "No Errors. Removing files and directories in: ";
		message.append(destinationDir);
		myLog.print(logInformation, message);
		myDirectory.Recurse((char* const )myConfig.fileLines[0].c_str(), deleteDirectoryDo);
	}
	else
	{
		message = "There were ";
		sprintf(intbuffer,"%d", myLog.numberOfErrors);
		message.append(intbuffer);
		message.append(" errors found.");
		myLog.print(logWarning, message);
	}


	myConfig.clearLines();
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


void doRecurseDirectory(char const * directoyEntry, int directoyEntryType)
{
	string message;
	audioTags myTags;

	File myFile;
	string fileName;
	string destinationFileName = "none";
	size_t found;
	string sourceFile;
	string tempString;
	string AlbumName;
	string ArtistName;
	char albumIDString[10];

	int returnValue;

	sourceFile = directoyEntry;

	if (directoyEntryType == DIRECTORYENTRYTYPE_DIR)
	{
		newAlbum = 1; // New directory means new Album
		gotCoverJPG = 0;
//		cout << "recurseDo: directoyEntryType == DIRECTORYENTRYTYPE_DIR"  << endl;
//		printf("New Album!!! %s\n", directoyEntry);
	}

	if(directoyEntryType == DIRECTORYENTRYTYPE_REG) // if it is a file and a duplicate... then remove
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

//		cout << "Artist: " << ArtistName << " - Album: " << AlbumName << endl;
//		printf("%s\n%s\n\n", directoyEntry,destinationFileName.c_str());

		message.clear();
		message.append(directoyEntry);
		myLog.print(logWarning, message);


		if (!isAppleDropping((char* const)fileName.c_str())) // Apple leaves files thatnbegin with ._ and we need to forget about them.
		{

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
				message = "Error copying file: ";
				message.append(myFile.errorMessageBuffer);
				message.append(": ");
				message.append(sourceFile);
				message.append(" to -> ");
				message.append(destinationFileName);
				myLog.print(logError, message);
			}
	// See if this file is the cover file. if it is... then we can copy over the thumb image as well when we have the album ID
			if (strcasecmp (fileName.c_str(),"cover.jpg") == 0)
			{
				gotCoverJPG = 1;
				coverName = sourceFile;
			}
			if (strcasecmp (fileName.c_str(),"cover.jpeg") == 0)
			{
				gotCoverJPG = 1;
				coverName = sourceFile;
			}

			if (isMP3((char* const)sourceFile.c_str()))
			{
				myTags.get((char*)sourceFile.c_str());
				myDB.setAlbum(myTags.album);
				myDB.setArtist(myTags.artist);
				myDB.setComposer(myTags.composers);
				myDB.setAlbumArtists(myTags.albumArtists);
				myDB.setSongName(myTags.title);
				myDB.setGenre(myTags.genre);
				myDB.setTrackNumber(myTags.track);
				myDB.setSongYear(myTags.year);
				myDB.setBitRate(myTags.bitrate);
				myDB.setSampleRate(myTags.sampleRate);
				myDB.setSongTime(myTags.length);
				myDB.setLocation ((char*)destinationFileName.c_str());
				myDB.setGrouping("Album");

				if (newAlbum == 1)
				{
					newAlbum = 0;
					lAlbum = myDB.addAlbum();
					sprintf(albumIDString, "%ld", lAlbum);
					jpgThumbName = destinationDir + ArtistName + "/" + AlbumName + "/" + albumIDString + ".w300.thumb";
					myDB.setThumbLocation ((char*)jpgThumbName.c_str());
					myDB.setAlbumId(lAlbum);
					myDB.updateAlbumCover();
					lArtist = myDB.addArtist();
					myDB.setArtistId(lArtist);
		//			cout << "updateAlbumCover: " << jpgThumbName  << endl;
				}


				myDB.setAlbumId(lAlbum);
				myDB.setArtistId(lArtist);
				myDB.addSongToPreSongLibrary();

//				addSongToDB((char* const)destinationFileName.c_str(), (char* const)jpgThumbName.c_str(), (char* const)"Album");

//				cout << "updateAlbumCover: " << jpgThumbName  << endl;

			}
//				printf("thumb %s\n\n", jpgThumbName.c_str());
			if( (gotCoverJPG == 1) && (newAlbum == 0))
			{
				returnValue = myFile.copy(coverName,jpgThumbName);
				gotCoverJPG = 0; // Set got cover back to 0 so that we dont to copy it again

				if (returnValue == 0) // if success
				{
					message = "Copying file:";
					message.append(coverName);
					message.append(" to -> ");
					message.append(jpgThumbName);
					myLog.print(logInformation, message);
				}
				else
				{
					message = "Error copying file: ";
					message.append(myFile.errorMessageBuffer);
					message.append(": ");
					message.append(coverName);
					message.append(" to -> ");
					message.append(jpgThumbName);
					myLog.print(logError, message);
				}
			}

		}
	}
}

#ifdef NOTNEEDED

int addSongToDB(char* const fileName, char* const thumbName, char* const grouping)
{
	audioTags myTags;

	myTags.get(fileName);
	myDB.setAlbum(myTags.album);
	myDB.setArtist(myTags.artist);
	myDB.setSongName(myTags.title);
	myDB.setGenre(myTags.genre);
	myDB.setTrackNumber(myTags.track);
	myDB.setSongYear(myTags.year);
	myDB.setBitRate(myTags.bitrate);
	myDB.setSampleRate(myTags.sampleRate);
	myDB.setSongTime(myTags.length);
	myDB.setLocation (fileName);
	myDB.setGrouping(grouping);

	if (strcmp ("Album", grouping) != 0) // is this is not an album.. set album and artist ID to 0
	{
		lAlbum = 0;
		lArtist = 0;
	}

	myDB.setAlbumId(lAlbum);
	myDB.setArtistId(lArtist);
	myDB.addSongToPreSongLibrary();

	return 0;
}
#endif

int isMP3(char* const fileName)
{
	if (strcasecmp (&fileName[strlen(fileName)-3],"MP3") == 0)
	{
		return (1);
	}
	return(0);
}


int isAppleDropping(char* const fileName)
{
	if (strstr (&fileName[0],"._") != 0) //strstr returns a pointer to the string if it is found. Otherwise a 0 is returned.
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
