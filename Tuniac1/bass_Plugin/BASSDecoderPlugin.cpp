#include "StdAfx.h"
#include "bassdecoderplugin.h"
#include "BASSDecoder.h"

CBASSDecoderPlugin::CBASSDecoderPlugin(void)
{
	if (HIWORD(BASS_GetVersion())!=BASSVERSION)
	{
		MessageBox(0,L"An incorrect version of BASS.DLL was loaded",0,MB_ICONERROR);
	}
	else
	{
		exts.AddTail(std::wstring(L".mp3"));
		exts.AddTail(std::wstring(L".mp2"));
		exts.AddTail(std::wstring(L".mp1"));
		exts.AddTail(std::wstring(L".ogg"));
		exts.AddTail(std::wstring(L".wav"));
		exts.AddTail(std::wstring(L".aif"));

		exts.AddTail(std::wstring(L".mo3"));
		exts.AddTail(std::wstring(L".mod"));
		exts.AddTail(std::wstring(L".xm"));
		exts.AddTail(std::wstring(L".it"));
		exts.AddTail(std::wstring(L".s3m"));
		exts.AddTail(std::wstring(L".mtm"));
		exts.AddTail(std::wstring(L".umx"));

		TCHAR				szFilename[MAX_PATH];
		TCHAR				szFilePath[MAX_PATH];
		WIN32_FIND_DATA		w32fd;
		HANDLE				hFind;

		GetModuleFileName(NULL, szFilePath, MAX_PATH);
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

				TCHAR szURL[MAX_PATH];

				StrCpy(szURL, szFilePath);
				StrCat(szURL, w32fd.cFileName);

				char mbURL[MAX_PATH]; 	 
				WideCharToMultiByte(CP_UTF8, 0, szURL, -1, mbURL, MAX_PATH, 0, 0);

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

							char * soundFont = (char *)BASS_GetConfigPtr(BASS_CONFIG_MIDI_DEFFONT);
							if(!soundFont)
							{
								//creative soundfont not found. attempt to load local soundfont
								TCHAR szSoundFontFilename[MAX_PATH];
								StrCpy(szSoundFontFilename, szFilePath);
								StrCat(szSoundFontFilename, TEXT("*.SF2"));

								WIN32_FIND_DATA		w32fdSoundFont;
								HANDLE				hFindSoundFont;

								hFindSoundFont = FindFirstFile(szSoundFontFilename, &w32fdSoundFont); 
								if(hFindSoundFont != INVALID_HANDLE_VALUE) 
								{

									StrCpy(szSoundFontFilename, szFilePath);
									StrCat(szSoundFontFilename, w32fdSoundFont.cFileName);

									char mbSoundFontFilename[MAX_PATH]; 	 
									WideCharToMultiByte(CP_UTF8, 0, szSoundFontFilename, -1, mbSoundFontFilename, MAX_PATH, 0, 0);

									BASS_SetConfigPtr(BASS_CONFIG_MIDI_DEFFONT, mbSoundFontFilename);
									soundFont = (char *)BASS_GetConfigPtr(BASS_CONFIG_MIDI_DEFFONT);
								}
							}
							//check if we now have a sound font loaded either local or creative
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
							int iStrLen = strlen(pinfo->formats[a].exts) + 1;
							char * extstring = (char *)malloc(iStrLen);
							strncpy(extstring, pinfo->formats[a].exts, (iStrLen));
							char * pch = strtok(extstring,"*;");
							while (pch != NULL)
							{
								int iExtLen = strlen(pch)+1;
								TCHAR * newext = (TCHAR *)malloc((iExtLen) * sizeof(TCHAR));
								MultiByteToWideChar(CP_ACP, 0, pch, iExtLen, newext, iExtLen);
								exts.AddTail(std::wstring(newext));
								free(newext);
								pch = strtok(NULL, "*;");
							}
							free(extstring);
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
}

void			CBASSDecoderPlugin::Destroy(void)
{
	BASS_Free();
	BASS_PluginFree(0);
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
    MessageBox(hParent, TEXT("BASS Plugin\nUsing BASS 2.4\nCopyright (c) 1999-2010 Un4seen Developments Ltd."), GetName(), MB_OK | MB_ICONINFORMATION);
	return true;
}

bool			CBASSDecoderPlugin::Configure(HWND hParent)
{
	return(false);
}

bool			CBASSDecoderPlugin::CanHandle(LPTSTR szSource)
{
	/*
	if(!StrCmpI(TEXT(".mp3"), PathFindExtension(szSource)))
	{
		return false;
	}
	*/


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
	return exts.GetCount();
}

LPTSTR			CBASSDecoderPlugin::GetCommonExt(unsigned long ulIndex)
{
	return (LPTSTR)exts[ulIndex].c_str();
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
