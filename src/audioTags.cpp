/*
 * audioTags.cpp
 *
 *  Created on: May 12, 2019
 *      Author: jdellaria
 */


#include <stdio.h>
#include <stdlib.h>
#include <iostream>


#include <cstdlib>
#include <sys/dir.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#include <fileref.h>
#include <tag.h>
#include <Directory.h>
#include <File.h>
//#include <DLog.h>
#include <string>
#include <regex.h>
#include <errno.h>


#include <taglib/fileref.h>
#include <taglib/flacfile.h>
#include <taglib/apetag.h>
#include <taglib/mpcfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v1genres.h>
#include <taglib/mpegfile.h>
#include <taglib/oggfile.h>
#include <taglib/taglib.h>
#include <taglib/tag.h>
#include <taglib/vorbisfile.h>
#include <taglib/xiphcomment.h>

//extern DLog myLog;

#include <LinuxCommand.h>
//#include <tstring.h>
#include "audioTags.h"

#include "musicDB.h"
#define MODULE_NAME "audioTags.cpp";

using namespace std;

audioTags::audioTags() {
	// TODO Auto-generated constructor stub

}

audioTags::~audioTags() {
	// TODO Auto-generated destructor stub
}


int audioTags::get(const char *fileName)
{

	TagLib::FileRef f(fileName);
	File myCoverImageFile;
	size_t found;
	string sourceFile;
	string directoryName;
	string albumImageName;
	string message;

	TagLib::String TagLibComposers;
	TagLib::ID3v2::FrameList lstID3v2;

	TagLib::String TagLibAlbumArtists;

	sourceFile = fileName;
	location = fileName;
//	cout << "audioTags: get fileName: "  << fileName << endl;
	if(!f.isNull() && f.tag())
	{

		TagLib::Tag *tag = f.tag();

		title = tag->title().toCString();
		artist = tag->artist().toCString();
		album = tag->album().toCString();
		year = tag->year();
		track = tag->track();
		genre = tag->genre().toCString();
		location = fileName;
		found = sourceFile.find_last_of("/\\"); // Get the folder name that the file resides in
		directoryName = sourceFile.substr(0,found+1);
		albumImageName = directoryName + "cover.jpg";
		if(myCoverImageFile.exist(albumImageName.c_str()) == 0)
		{
			getImage(fileName, albumImageName.c_str());
//			cout <<"Writing Album cover for "  << albumImageName << endl;
		}
	}
	if(!f.isNull() && f.audioProperties())
	{
		TagLib::AudioProperties *properties = f.audioProperties();

		bitrate = properties->bitrate();
		sampleRate = properties->sampleRate();
		length = properties->length();
	}

	TagLib::MPEG::File mpegFile(fileName, true);

    // Composer.
    lstID3v2 = mpegFile.ID3v2Tag()->frameListMap()["TCOM"];
    if (!lstID3v2.isEmpty()) {
        for (TagLib::ID3v2::FrameList::ConstIterator it = lstID3v2.begin(); it != lstID3v2.end(); ++it) {
            if (!TagLibComposers.isEmpty()) {
            	TagLibComposers += ", ";
            }
            TagLibComposers += (*it)->toString();
        }
   		composers = TagLibComposers.toCString();
 //  		cout << "composers is: " << composers  << endl;
    }

    // Album Artist.
    lstID3v2 = mpegFile.ID3v2Tag()->frameListMap()["TPE2"];
    if (!lstID3v2.isEmpty()) {
        for (TagLib::ID3v2::FrameList::ConstIterator it = lstID3v2.begin(); it != lstID3v2.end(); ++it) {
            if (!TagLibAlbumArtists.isEmpty()) {
            	TagLibAlbumArtists += ", ";
            }
            TagLibAlbumArtists += (*it)->toString();
        }
        albumArtists = TagLibAlbumArtists.toCString();
 //       cout << "albumArtists: " << albumArtists  << endl;
    }
	return 0;
}


int audioTags::set(const char *fileName)
{

	TagLib::FileRef f(fileName);

	cout << "id3v2Tags: get fileName: "  << fileName << endl;
	if(!f.isNull() && f.tag())
	{

		TagLib::Tag *tag = f.tag();

		tag->setTitle(title);
		tag->setArtist(artist);
		tag->setAlbum(album);
		tag->setYear(year);
		tag->setTrack(track);
		tag->setGenre(genre);
		tag->setComment("");
		f.save();
	}
	return 0;
}


void audioTags::getImage(const char * mp3FileName, const char * imageFileName)
{
	LinuxCommand lCommand;
	char const *darg[7]={"ffmpeg", "-i",  NULL,  NULL, NULL};
	darg[2] = mp3FileName;
	darg[3] = imageFileName;
	char returnBuffer[100];
	lCommand.Execute(darg,returnBuffer,100);
}

