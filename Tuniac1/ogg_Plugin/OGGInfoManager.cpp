////////////////////////////////////////////////////////////
//
//				EDIT THIS ONE YOU BIG BALD FUCK!
//
//
#include "StdAfx.h"
#include "vorbis\vorbisfile.h"
#include "ogginfomanager.h"

// actual class definition here

COGGInfoManager::COGGInfoManager(void)
{
}

COGGInfoManager::~COGGInfoManager(void)
{
}

void			COGGInfoManager::Destroy(void)
{
	delete this;
}

unsigned long	COGGInfoManager::GetNumExtensions(void)
{
	return 1;
}

LPTSTR			COGGInfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
	return TEXT("OGG");
}

bool			COGGInfoManager::CanHandle(LPTSTR szSource)
{
	if(StrStrI(PathFindExtension(szSource), TEXT("OGG")))
		return true;
	return false;
}

bool			COGGInfoManager::GetInfo(LibraryEntry * libEnt)
{

	f = _wfopen(libEnt->szURL, TEXT("rbS"));

	if(f == NULL)
		return false;

	if(ov_open(f, &oggFile, NULL, 0)<0){
		return false;
	}

 // Get some information about the OGG file

	TCHAR tstr[256];
	char	*t;

	if ((tInfo = ov_comment(&oggFile, -1))) {
		if ((t = vorbis_comment_query(tInfo, "title", 0)))
			_snwprintf(libEnt->szTitle, 128, L"%S", t);

		if ((t = vorbis_comment_query(tInfo, "artist", 0)))
			_snwprintf(libEnt->szArtist, 128, L"%S", t);

		if ((t = vorbis_comment_query(tInfo, "album", 0)))
			_snwprintf(libEnt->szAlbum, 128, L"%S", t);

		if ((t = vorbis_comment_query(tInfo, "comment", 0)))
			_snwprintf(libEnt->szComment, 128, L"%S", t);

		if ((t = vorbis_comment_query(tInfo, "genre", 0)))
			_snwprintf(libEnt->szGenre, 128, L"%S", t);

		if ((t = vorbis_comment_query(tInfo, "date", 0)))
			libEnt->iYear = atoi(t);

		if ((t = vorbis_comment_query(tInfo, "tracknumber", 0)))
			libEnt->dwTrack[0] = atoi(t);
	}

	pInfo = ov_info(&oggFile, -1);
	double time = ov_time_total(&oggFile, 0);
    
	libEnt->iBitRate			= pInfo->bitrate_nominal;
	libEnt->iChannels			= pInfo->channels;
	libEnt->iSampleRate			= pInfo->rate;
	libEnt->iPlaybackTime		= (unsigned long)(time * 1000.0);

	ov_clear(&oggFile);

	return true;
}

bool			COGGInfoManager::SetInfo(LibraryEntry * libEnt)
{
	return true;
}