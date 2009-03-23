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
	m_pHelper = pHelper;
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
	if(!PathIsURL(szSource))
	{
		for(unsigned int x=0; x<GetNumCommonExts(); x++)
		{
			if(!StrCmpI(GetCommonExt(x), PathFindExtension(szSource)))
			{
				return(true);
			}
		}
	}
	else
	{
		if(!StrCmpN(szSource, TEXT("AUDIOCD"), 7))
			return true;

		// check the correct BASS was loaded
		if (HIWORD(BASS_GetVersion())!=BASSVERSION)
		{
			MessageBox(0,L"An incorrect version of BASS.DLL was loaded",0,MB_ICONERROR);
			return false;
		}

		TCHAR				szFilename[_MAX_PATH];
		TCHAR				szFilePath[_MAX_PATH];
		WIN32_FIND_DATA		w32fd;
		HANDLE				hFind;


		GetModuleFileName(NULL, szFilePath, _MAX_PATH);
		PathRemoveFileSpec(szFilePath);
		PathAddBackslash(szFilePath);
		StrCat(szFilePath, TEXT("bass"));
		PathAddBackslash(szFilePath);
		StrCpy(szFilename, szFilePath);
		StrCat(szFilename, TEXT("bass*.dll"));


		hFind = FindFirstFile(szFilename, &w32fd); 
		if(hFind != INVALID_HANDLE_VALUE) 
		{
			do
			{
				if(StrCmp(w32fd.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0 )
					continue;

				TCHAR szURL[_MAX_PATH];

				StrCpy(szURL, szFilePath);
				StrCat(szURL, w32fd.cFileName);

				char mbURL[_MAX_PATH]; 	 
				WideCharToMultiByte(CP_UTF8, 0, szURL, -1, mbURL, _MAX_PATH, 0, 0);

				if(!BASS_PluginLoad(mbURL, 0))
				{
					int x = BASS_ErrorGetCode();
					int b = x;
				}

			} while(FindNextFile(hFind, &w32fd));

			FindClose(hFind); 
		}

		BASS_Init(0,44100,0,0,NULL);
		char mbURL[512]; 	 
		WideCharToMultiByte(CP_UTF8, 0, szSource, -1, mbURL, 512, 0, 0);
		HSTREAM decodehandle = BASS_StreamCreateURL(mbURL,0, BASS_STREAM_DECODE|BASS_SAMPLE_FLOAT, NULL, 0);
		bool bOK = false;
		if(decodehandle)
			bOK = true;

		BASS_StreamFree(decodehandle);
		BASS_Free();
		BASS_PluginFree(0);

		return bOK;
	}


	return(false);
}


unsigned long	CBASSDecoderPlugin::GetNumCommonExts(void)
{
	unsigned long count = 14;

#ifdef COMPILE32BIT
	count += 17;
#endif
	return count;
/*
	TCHAR				szFilename[_MAX_PATH];
	TCHAR				szFilePath[_MAX_PATH];
	WIN32_FIND_DATA		w32fd;
	HANDLE				hFind;

	GetModuleFileName(NULL, szFilePath, _MAX_PATH);
	PathRemoveFileSpec(szFilePath);
	PathAddBackslash(szFilePath);
	StrCat(szFilePath, TEXT("bass"));
	PathAddBackslash(szFilePath);
	StrCpy(szFilename, szFilePath);
	StrCat(szFilename, TEXT("bass*.dll"));

	hFind = FindFirstFile(szFilename, &w32fd); 
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if(StrCmp(w32fd.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0 )
				continue;

			TCHAR szURL[_MAX_PATH];

			StrCpy(szURL, szFilePath);
			StrCat(szURL, w32fd.cFileName);

			char mbURL[_MAX_PATH]; 	 
			WideCharToMultiByte(CP_UTF8, 0, szURL, -1, mbURL, _MAX_PATH, 0, 0);

			HPLUGIN plug;
			if (plug=BASS_PluginLoad(mbURL,0))
			{
				const BASS_PLUGININFO *pinfo=BASS_PluginGetInfo(plug);
				count += pinfo->formatc;
				BASS_PluginFree(0);
			}
		} while(FindNextFile(hFind, &w32fd));

		FindClose(hFind); 
	}
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

		TEXT(".cda"),
		TEXT(".ape"),
		TEXT(".mac"),
		TEXT(".flac"),
		TEXT(".fla"),
		TEXT(".oga"),
		TEXT(".wma"),
		TEXT(".wv")

#ifdef COMPILE32BIT
		,TEXT(".aac")
		,TEXT(".mp4")
		,TEXT(".m4a")
		,TEXT(".ac3")
		,TEXT(".adx")
		,TEXT(".alac")
		,TEXT(".mpc")
		,TEXT(".mpp")
		,TEXT(".mp+")
		,TEXT(".ofr")
		,TEXT(".ofs")
		,TEXT(".spx")
		,TEXT(".tta")
		,TEXT(".mid")
		,TEXT(".kidi")
		,TEXT(".rmi")
		,TEXT(".kar")

#endif
	};

	return exts[ulIndex];


/*
	TCHAR				szFilename[_MAX_PATH];
	TCHAR				szFilePath[_MAX_PATH];
	WIN32_FIND_DATA		w32fd;
	HANDLE				hFind;


	GetModuleFileName(NULL, szFilePath, _MAX_PATH);
	PathRemoveFileSpec(szFilePath);
	PathAddBackslash(szFilePath);
	StrCat(szFilePath, TEXT("bass"));
	PathAddBackslash(szFilePath);
	StrCpy(szFilename, szFilePath);
	StrCat(szFilename, TEXT("bass*.dll"));

	hFind = FindFirstFile(szFilename, &w32fd); 
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if(StrCmp(w32fd.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0 )
				continue;

			TCHAR szURL[_MAX_PATH];

			StrCpy(szURL, szFilePath);
			StrCat(szURL, w32fd.cFileName);

			char mbURL[_MAX_PATH]; 	 
			WideCharToMultiByte(CP_UTF8, 0, szURL, -1, mbURL, _MAX_PATH, 0, 0);

			HPLUGIN plug;
			if (plug=BASS_PluginLoad(mbURL,0))
			{
				const BASS_PLUGININFO *pinfo=BASS_PluginGetInfo(plug);
				int a;
				for (a=0;a<pinfo->formatc;a++)
				{
					//pinfo->formats[a].exts is "*.ext1;*.ext2;"
					const char *extstring = &pinfo->formats[a].exts[1];
					int pos = 0;

					while(strlen(extstring))
					{
						char ext[16] = {NULL};
						pos = strcspn(extstring, ";");
						if(pos >= strlen(extstring))
						{
							strncpy(ext, extstring, pos);
							break;
						}
						strncpy(ext, extstring, pos);
						extstring = &extstring[pos+1];

						LPTSTR newext = NULL;
						MultiByteToWideChar(CP_UTF8, 0, ext, strlen(ext), newext, 16);
						exts.AddTail(newext);
					}
				}
				BASS_PluginFree(0);
			}
			int x = BASS_ErrorGetCode();
			int b = x;
		} while(FindNextFile(hFind, &w32fd));

		FindClose(hFind); 
	}
	*/
}


IAudioSource *		CBASSDecoderPlugin::CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO)
{
	CBASSDecoder *	pDec = new CBASSDecoder();

	if(!pDec->Open(szSource, m_pHelper))
	{
		delete pDec;
		return(NULL);
	}

	return (IAudioSource *)pDec;
}
