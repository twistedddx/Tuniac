////////////////////////////////////////////////////////////
//
//				EDIT THIS ONE YOU BIG BALD FUCK!
//
//

#include "stdafx.h"
#include <atlbase.h>
#include "STDinfomanager.h"
#include "tag.h"
#include "tfile.h"
#include "fileref.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


extern "C" __declspec(dllexport) IInfoManager * CreateInfoManagerPlugin(void)
{
	IInfoManager * pInfo = new CSTDInfoManager;

	return(pInfo);
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacInfoManagerVersion(void)
{
	return ITUNIACINFOMANAGER_VERSION;
}


// actual class definition here

CSTDInfoManager::CSTDInfoManager(void)
{
}

CSTDInfoManager::~CSTDInfoManager(void)
{
}

void			CSTDInfoManager::Destroy(void)
{
	delete this;
}

unsigned long	CSTDInfoManager::GetNumExtensions(void)
{
	return 10;
}

LPTSTR			CSTDInfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
	static LPTSTR exts[] = 
	{
		TEXT(".flac"),
		TEXT(".mp3"),
		TEXT(".ogg"),
		TEXT(".mpc"),
		TEXT(".wv"),
		TEXT(".m4a"),
		TEXT(".m4p"),
		TEXT(".mp4"),
		TEXT(".tta"),
		TEXT(".spx")
	};

	return exts[ulExtentionNum];
}

bool			CSTDInfoManager::CanHandle(LPTSTR szSource)
{

	if(StrStrI(PathFindExtension(szSource), TEXT("FLAC")))
		return true;
	if(StrStrI(PathFindExtension(szSource), TEXT("MP3")))
		return true;
	if(StrStrI(PathFindExtension(szSource), TEXT("OGG")))
		return true;
	if(StrStrI(PathFindExtension(szSource), TEXT("MPC")))
		return true;
	if(StrStrI(PathFindExtension(szSource), TEXT("WV")))
		return true;
	if(StrStrI(PathFindExtension(szSource), TEXT("M4A")))
		return true;
	if(StrStrI(PathFindExtension(szSource), TEXT("M4P")))
		return true;
	if(StrStrI(PathFindExtension(szSource), TEXT("MP4")))
		return true;
	if(StrStrI(PathFindExtension(szSource), TEXT("TTA")))
		return true;
	if(StrStrI(PathFindExtension(szSource), TEXT("SPX")))
		return true;

	return false;
}

bool			CSTDInfoManager::GetInfo(LibraryEntry * libEnt)
{
	USES_CONVERSION;
	TagLib::File		*		m_File;
	m_File = TagLib::FileRef::create(libEnt->szURL, 1, TagLib::AudioProperties::Fast);

	if(m_File)
	{
		libEnt->iSampleRate = m_File->audioProperties()->sampleRate();
		libEnt->iChannels = m_File->audioProperties()->channels();
		libEnt->iBitRate = m_File->audioProperties()->bitrate() * 1024;
		libEnt->iPlaybackTime = m_File->audioProperties()->length() * 1000;

		libEnt->iYear = m_File->tag()->year();
		libEnt->dwTrack[0] = m_File->tag()->track();

		swprintf(libEnt->szTitle, 128, L"%s", m_File->tag()->title().toWString().c_str());

		swprintf(libEnt->szArtist, 128, L"%s", m_File->tag()->artist().toWString().c_str());

		swprintf(libEnt->szAlbum, 128, L"%s", m_File->tag()->album().toWString().c_str());

		swprintf(libEnt->szGenre, 128, L"%s", m_File->tag()->genre().toWString().c_str());

		swprintf(libEnt->szComment, 128, L"%s", m_File->tag()->comment().toWString().c_str());
	}
	delete m_File;
	return true;
}

bool			CSTDInfoManager::SetInfo(LibraryEntry * libEnt)
{
	return true;
}