#include "StdAfx.h"
#include "bassdecoderplugin.h"
#include "BASSDecoder.h"

CBASSDecoderPlugin::CBASSDecoderPlugin(void)
{
	count = 12;
	exts[0] = TEXT(".mp3");
	exts[1] = TEXT(".mp2");
	exts[2] = TEXT(".mp1");
	exts[3] = TEXT(".ogg");
	exts[4] = TEXT(".wav");
	exts[5] = TEXT(".aif");

	exts[6] = TEXT(".mod");
	exts[7] = TEXT(".it");
	exts[8] = TEXT(".xm");
	exts[9] = TEXT(".s3m");
	exts[10] = TEXT(".mtm");
	exts[11] = TEXT(".umx");

	if (HIWORD(BASS_GetVersion())!=BASSVERSION)
	{
		MessageBox(0,L"An incorrect version of BASS.DLL was loaded",0,MB_ICONERROR);
	}
	else
	{

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
				if(plug = BASS_PluginLoad(mbURL, 0))
				{
					bool bAddPlugin = true;
					int stringLen = lstrlen(szURL);
					if(stringLen >= 12)
					{
						LPTSTR	t = &szURL[stringLen-12];
						if(!lstrcmpi(TEXT("bassmidi.dll"), t))
						{

							TCHAR szSoundFontFilename[_MAX_PATH];
							StrCpy(szSoundFontFilename, szFilePath);
							StrCat(szSoundFontFilename, TEXT("*.SF2"));

							WIN32_FIND_DATA		w32fdSoundFont;
							HANDLE				hFindSoundFont;

							hFind = FindFirstFile(szSoundFontFilename, &w32fdSoundFont); 
							if(hFind != INVALID_HANDLE_VALUE) 
							{
								if(StrCmp(w32fdSoundFont.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0 )
									continue;

								StrCpy(szSoundFontFilename, szFilePath);
								StrCat(szSoundFontFilename, w32fdSoundFont.cFileName);

								char mbSoundFontFilename[_MAX_PATH]; 	 
								WideCharToMultiByte(CP_UTF8, 0, szSoundFontFilename, -1, mbSoundFontFilename, _MAX_PATH, 0, 0);

								BASS_SetConfigPtr(BASS_CONFIG_MIDI_DEFFONT, mbSoundFontFilename);
							}
							char * soundFont = (char *)BASS_GetConfigPtr(BASS_CONFIG_MIDI_DEFFONT);
							if(!soundFont)
							{
								bAddPlugin = false;
								BASS_PluginFree(plug);
							}
						}
					}
					if(bAddPlugin)
					{
						
						const BASS_PLUGININFO *pinfo=BASS_PluginGetInfo(plug);
						for (int a=0;a<pinfo->formatc;a++)
						{
							int iStrLen = strlen(pinfo->formats[a].exts);
							char * extstring = (char *)malloc(iStrLen+1);
							strncpy(extstring, pinfo->formats[a].exts, iStrLen);
							extstring[iStrLen] = '\0';
							char * pch = strtok(extstring,"*;");
							while (pch != NULL)
							{
								int iExtLen = strlen(pch);
								TCHAR * newext = (TCHAR *)malloc((iExtLen+1) * sizeof(TCHAR));
								MultiByteToWideChar(CP_UTF8, 0, pch, -1, newext, iExtLen+1);
								exts[count] = (LPWSTR)newext;
								count++;
								pch = strtok(NULL, "*;");
							}
						}
						
					}
				}

			} while(FindNextFile(hFind, &w32fd));

			FindClose(hFind); 
		}

		BASS_Init(0,44100,0,0,NULL);
	}
}

CBASSDecoderPlugin::~CBASSDecoderPlugin(void)
{
	BASS_Free();
	BASS_PluginFree(0);
}


bool			CBASSDecoderPlugin::DoBASSInit()
{

	return true;
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
	HSTREAM testhandle;

	bool bMod = false;
	bool bOK = false;
	if(!PathIsURL(szSource))
	{
		if(testhandle = BASS_StreamCreateFile(FALSE, szSource, 0, 0, BASS_STREAM_DECODE|BASS_UNICODE|BASS_SAMPLE_FLOAT))
		{
			bOK = true;
		}
		else if(testhandle = BASS_MusicLoad(FALSE, szSource, 0, 0, BASS_MUSIC_DECODE|BASS_UNICODE|BASS_SAMPLE_FLOAT|BASS_MUSIC_RAMP|BASS_MUSIC_PRESCAN, 0))
		{
			bMod = true;
			bOK = true;
		}
	}
	else
	{
		if(!StrCmpN(szSource, TEXT("AUDIOCD"), 7))
			return true;

		char mbURL[512]; 	 
		WideCharToMultiByte(CP_UTF8, 0, szSource, -1, mbURL, 512, 0, 0);
		if(testhandle = BASS_StreamCreateURL(mbURL,0, BASS_STREAM_DECODE|BASS_SAMPLE_FLOAT, NULL, 0))
		{
			bOK = true;
		}
	}



	if(bOK)
	{
		if(bMod)
			BASS_MusicFree(testhandle);
		else
			BASS_StreamFree(testhandle);
	}

	return bOK;
}


unsigned long	CBASSDecoderPlugin::GetNumCommonExts(void)
{
	return count;
}

LPTSTR			CBASSDecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	return exts[ulIndex];
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
