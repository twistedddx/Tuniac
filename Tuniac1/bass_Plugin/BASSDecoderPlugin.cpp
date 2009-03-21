#include "StdAfx.h"
#include "bassdecoderplugin.h"
#include "BASSDecoder.h"

CBASSDecoderPlugin::CBASSDecoderPlugin(void)
{
}

CBASSDecoderPlugin::~CBASSDecoderPlugin(void)
{
}

void			CBASSDecoderPlugin::Destroy(void)
{
	delete this;
}

void			CBASSDecoderPlugin::SetHelper(IAudioSourceHelper * pHelper)
{
}

LPTSTR			CBASSDecoderPlugin::GetName(void)
{
	return(TEXT("BASS Decoder"));
}

GUID			CBASSDecoderPlugin::GetPluginID(void)
{
	static const GUID mpcPluginGUID = {0x65d0eaf3, 0x21f8, 0x4db9, {0xb1, 0x61, 0x28, 0x5d, 0xb6, 0xe1, 0xe2, 0x66} };
	return(mpcPluginGUID);
}

unsigned long	CBASSDecoderPlugin::GetFlags(void)
{
	return FLAGS_ABOUT;
}

bool			CBASSDecoderPlugin::About(HWND hParent)
{
    MessageBox(hParent, TEXT("BASS Plugin \nUsing Bass lib "), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CBASSDecoderPlugin::Configure(HWND hParent)
{
	return(false);
}

bool			CBASSDecoderPlugin::CanHandle(LPTSTR szSource)
{
	for(unsigned int x=0; x<GetNumCommonExts(); x++)
	{
		if(!StrCmpI(GetCommonExt(x), PathFindExtension(szSource)))
		{
			return(true);
		}
	}

	return(false);

/*
	TCHAR				szFilename[512];
	WIN32_FIND_DATA		w32fd;
	HANDLE				hFind;

	GetModuleFileName(NULL, szFilename, 512);
	PathRemoveFileSpec(szFilename);
	PathAddBackslash(szFilename);
	StrCat(szFilename, TEXT("bass*.dll"));

	hFind = FindFirstFile(szFilename, &w32fd); 
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if(StrCmp(w32fd.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0 )
				continue;

			char tempname[_MAX_PATH]; 	 
			WideCharToMultiByte(CP_UTF8, 0, w32fd.cFileName, -1, tempname, _MAX_PATH, 0, 0);

			HPLUGIN plug;
			if (plug=BASS_PluginLoad(tempname,0))
			{
				const BASS_PLUGININFO *pinfo=BASS_PluginGetInfo(plug);
				int a;
				for (a=0;a<pinfo->formatc;a++)
				{
					//from here is wrong pinfo->formats[a].exts is "*.ext1;*.ext2;"
					LPTSTR supportedext = &pinfo->formats[a].exts[lstrlen(szSource)-4];
					if(!lstrcmpi(supportedext, fileext))
					{
						return(true);
					}
				}
			}
		} while(FindNextFile(hFind, &w32fd));

		FindClose(hFind); 
	}
*/
}


unsigned long	CBASSDecoderPlugin::GetNumCommonExts(void)
{
	unsigned long count = 10;

#ifdef COMPILE32BIT
	count += 8;
#endif

	return count;
/*
	TCHAR				szFilename[512];
	WIN32_FIND_DATA		w32fd;
	HANDLE				hFind;

	GetModuleFileName(NULL, szFilename, 512);
	PathRemoveFileSpec(szFilename);
	PathAddBackslash(szFilename);
	StrCat(szFilename, TEXT("bass*.dll"));

	hFind = FindFirstFile(szFilename, &w32fd); 
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if(StrCmp(w32fd.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0 )
				continue;

			char tempname[_MAX_PATH]; 	 
			WideCharToMultiByte(CP_UTF8, 0, w32fd.cFileName, -1, tempname, _MAX_PATH, 0, 0);

			HPLUGIN plug;
			if (plug=BASS_PluginLoad(tempname,0))
			{
				BASS_PLUGININFO *pinfo=BASS_PluginGetInfo(plug);
				count += pinfo->formatc;
			}
		} while(FindNextFile(hFind, &w32fd));

		FindClose(hFind); 
	}

	return count;
*/
}

LPTSTR			CBASSDecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	static LPTSTR exts[] = 
	{
		TEXT(".mp3"),
		TEXT(".mp2"),
		TEXT(".mp1"),
		TEXT(".ogg"),
		TEXT(".wav"),
		TEXT(".aif"),
		TEXT(".ape"),
		TEXT(".flac"),
		TEXT(".wma"),
		TEXT(".wv")

#ifdef COMPILE32BIT
		,TEXT(".aac")
		,TEXT(".ac3")
		,TEXT(".alac")
		,TEXT(".mpc")
		,TEXT(".ofr")
		,TEXT(".spx")
		,TEXT(".tta")
		,TEXT(".midi")
#endif
	};

	return exts[ulIndex];
}


IAudioSource *		CBASSDecoderPlugin::CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO)
{
	CBASSDecoder *	pDec = new CBASSDecoder();

	if(!pDec->Open(szSource))
	{
		delete pDec;
		return(NULL);
	}

	return (IAudioSource *)pDec;
}
