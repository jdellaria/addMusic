/*
 * musicdb.h
 *
 *  Created on: Dec 26, 2008
 *      Author: jdellaria
 */

#ifndef MUSICDB_H_
#define MUSICDB_H_
#include <mysql/mysql.h>
#include <string>
#include <iostream>
#include <tag.h>

class musicDB{
public:
	char	name[255];
	char	artist[150];
	char	composer[150];
	char	album[255];
	char	grouping[100];
	char	genre[100];
	int	trackNumber;
	int	songYear;
	int	bitRate;
	int	sampleRate;
	int	songTime;
	char	dateModified[30];
	char	dateAdded[30];
	char	location[255];
	int	artistId;
	int	albumId;
	char	thumblocation[255];

	MYSQL	dbaseConnection;

		musicDB();
		~musicDB();
	MYSQL	OpenConnection();
	void	CloseConnection();
	long	addAlbum();
	long	updateAlbumCover();
	long	addSongToPreSongLibrary();
	long	addSongWithTimes( char * SQLStmt, MYSQL_TIME* dateModified, MYSQL_TIME* dateAdded);
	long	CommitSongsToLibrary();
	long	RemoveSongsFromPreSongLibrary();

	void	setArtist(TagLib::String artistName);
	void	setArtistId(long id);
	void	setAlbum( TagLib::String albumName);
	void	setAlbumId(long id);
	void	setSongName(TagLib::String songName);
	void	setGrouping(TagLib::String groupingName);
	void	setGenre(TagLib::String genreName);
	void	setTrackNumber(int track);
	void	setSongYear(int year);
	void	setBitRate(int brate);
	void	setSampleRate(int srate);
	void	setSongTime(int stime);
/*	void	setDateModified(char *date);
	void	setDateAdded(char *date);*/
	void	setLocation(char* where);
	void	setThumbLocation(char* where);
//	void	setLocation(TagLib::String where);
	long	addArtist();
	long	getArtistID();

};

#endif /* MUSICDB_H_ */
