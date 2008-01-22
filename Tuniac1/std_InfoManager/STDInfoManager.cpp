////////////////////////////////////////////////////////////
//
//				EDIT THIS ONE YOU BIG BALD FUCK!
//
//

#include "stdafx.h"
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
	TagLib::File		*		m_File;
	char ConvertBuffer[512];
	WideCharToMultiByte(CP_ACP, 0, libEnt->szURL, 512, ConvertBuffer, 512, NULL, NULL);
	m_File = TagLib::FileRef::create(ConvertBuffer, 1, TagLib::AudioProperties::Accurate);

	if(m_File)
	{
		double time = m_File->audioProperties()->length();
		double bitrate = m_File->audioProperties()->bitrate();
		libEnt->iSampleRate = m_File->audioProperties()->sampleRate();
		libEnt->iChannels = m_File->audioProperties()->channels();
		libEnt->iBitRate = (unsigned long)(bitrate * 1024.0);
		libEnt->iPlaybackTime = (unsigned long)(time * 1000.0);

		swprintf(libEnt->szTitle, L"%S", m_File->tag()->title().toCString(true));
		swprintf(libEnt->szArtist, L"%S", m_File->tag()->artist().toCString(true));
		swprintf(libEnt->szAlbum, L"%S", m_File->tag()->album().toCString(true));
		libEnt->iYear = m_File->tag()->year();
		libEnt->dwTrack[0] = m_File->tag()->track();
		swprintf(libEnt->szGenre, L"%S", m_File->tag()->genre().toCString(true));
		swprintf(libEnt->szComment, L"%S", m_File->tag()->comment().toCString(true));

	}
	delete m_File;
	return true;
}

bool			CSTDInfoManager::SetInfo(LibraryEntry * libEnt)
{
	return true;
}