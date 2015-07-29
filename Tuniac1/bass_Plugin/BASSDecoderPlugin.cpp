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
		//disable media foundation. Only adds video format support and has at least once picked a png as MPEG
		BASS_SetConfig(BASS_CONFIG_MF_DISABLE, 1);


		// formats inside standard bass.dll
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

		// fine and load additional plugins in ./bass/bass*.dll folder
		TCHAR				szFilename[MAX_PATH];
		TCHAR				szFilePath[MAX_PATH];
		WIN32_FIND_DATA		w32fd;
		HANDLE				hFind;

		GetModuleFileName(NULL, szFilePath, MAX_PATH);
		PathRemoveFileSpec(szFilePath);
		PathAddBackslash(szFilePath);
		StringCchCat(szFilePath, MAX_PATH, TEXT("bass"));
		PathAddBackslash(szFilePath);
		StringCchCopy(szFilename, MAX_PATH, szFilePath);
		StringCchCat(szFilename, MAX_PATH, TEXT("bass*.dll"));

		hFind = FindFirstFile(szFilename, &w32fd); 
		if(hFind != INVALID_HANDLE_VALUE) 
		{
			do
			{
				if(StrCmp(w32fd.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0 )
					continue;

				TCHAR szURL[MAX_PATH];

				StringCchCopy(szURL, MAX_PATH, szFilePath);
				StringCchCat(szURL, MAX_PATH, w32fd.cFileName);

				char mbURL[MAX_PATH]; 	 
				WideCharToMultiByte(CP_UTF8, 0, szURL, -1, mbURL, MAX_PATH, 0, 0);

				HPLUGIN plug;
				if(plug = BASS_PluginLoad(mbURL, 0))
				{
					bool bAddPlugin = true;
					int stringLen = wcsnlen_s(szURL, MAX_PATH);
					if(stringLen >= 12)
					{
						LPTSTR	t = &szURL[stringLen-12];
						if(StrCmpI(TEXT("bassmidi.dll"), t) == 0)
						{

							char * soundFont = (char *)BASS_GetConfigPtr(BASS_CONFIG_MIDI_DEFFONT);
							if(!soundFont)
							{
								//creative soundfont not found. attempt to load local soundfont
								TCHAR szSoundFontFilename[MAX_PATH];
								StringCchCopy(szSoundFontFilename, MAX_PATH, szFilePath);
								StringCchCat(szSoundFontFilename, MAX_PATH, TEXT("*.SF2"));

								WIN32_FIND_DATA		w32fdSoundFont;
								HANDLE				hFindSoundFont;

								hFindSoundFont = FindFirstFile(szSoundFontFilename, &w32fdSoundFont); 
								if(hFindSoundFont != INVALID_HANDLE_VALUE) 
								{

									StringCchCopy(szSoundFontFilename, MAX_PATH, szFilePath);
									StringCchCat(szSoundFontFilename, MAX_PATH, w32fdSoundFont.cFileName);

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
						//find and add plugins extensions for CanHandle()
						const BASS_PLUGININFO *pinfo=BASS_PluginGetInfo(plug);
						for (int a=0;a<pinfo->formatc;a++)
						{
							int iStrLen = strnlen_s(pinfo->formats[a].exts,256)+1;
							char * extstring = (char *)malloc(iStrLen);
							strcpy_s(extstring, iStrLen, pinfo->formats[a].exts);
							char * pch = strtok(extstring, "*;");
							while (pch != NULL)
							{
								int iExtLen = strnlen_s(pch,16)+1;
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
	// do we want Tony's mp3 decoder?
	if(!StrCmpI(TEXT(".mp3"), PathFindExtension(szSource)))
	{
		return false;
	}
	*/

	bModFile = false;
	bIsStream = PathIsURL(szSource);

	if (!bIsStream)
	{
		hStreamID = BASS_StreamCreateFile(FALSE, szSource, 0, 0, BASS_STREAM_DECODE | BASS_ASYNCFILE | BASS_UNICODE | BASS_SAMPLE_FLOAT);
		if (!hStreamID)
		{
			//failed typical audio formats, test for mod etc.
			hStreamID = BASS_MusicLoad(FALSE, szSource, 0, 0, BASS_MUSIC_DECODE | BASS_UNICODE | BASS_SAMPLE_FLOAT | BASS_MUSIC_RAMP | BASS_MUSIC_PRESCAN, 0);
			if (hStreamID)
				bModFile = true;
		}


		/* if we support media foundation, perhaps reject file if type is MPEG due to high false positives.
			BASS internal codecs should have already detected MPEG if file was actually valid

		else
		{
				BASS_CHANNELINFO ci;
				BASS_ChannelGetInfo(testHandle, &ci);
				if (ci.ctype == BASS_CTYPE_STREAM_MF)
				{
					const WAVEFORMATEX *wf = (const WAVEFORMATEX*)BASS_ChannelGetTags(testHandle, BASS_TAG_WAVEFORMAT);
					if (wf->wFormatTag == WAVE_FORMAT_MPEG)
					{
						BASS_StreamFree(testHandle);
						testHandle = 0;
					}
				}
		}
		*/
	}
	else
	{
		if (StrCmpN(szSource, TEXT("AUDIOCD"), 7) == 0)
		{
			bIsStream = false;
			wchar_t cDrive;
			int iTrack;
			swscanf_s(szSource, TEXT("AUDIOCD:%c:%d"), &cDrive, (int)sizeof(char), &iTrack);
			StringCchPrintf(szSource, 128, TEXT("%C:\\Track%02i.cda"), cDrive, iTrack);
			hStreamID = BASS_StreamCreateFile(FALSE, szSource, 0, 0, BASS_STREAM_DECODE | BASS_ASYNCFILE | BASS_UNICODE | BASS_SAMPLE_FLOAT);
		}
		else
		{
			char mbURL[512];
			WideCharToMultiByte(CP_UTF8, 0, szSource, -1, mbURL, 512, 0, 0);
			hStreamID = BASS_StreamCreateURL(mbURL, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_STREAM_BLOCK, NULL, 0);
		}
	}

	/* find out what file format was actually found by BASS.
	if (hStreamID)
	{
		BASS_CHANNELINFO bassChannelInfo;
		BASS_ChannelGetInfo(hStreamID, &bassChannelInfo);
		DWORD x = bassChannelInfo.ctype;
		DWORD y = bassChannelInfo.plugin;

		const BASS_PLUGININFO * pluginInfo = BASS_PluginGetInfo(bassChannelInfo.plugin);
		int a;
		for (a = 0; a < pluginInfo->formatc; a++) {
			const char * formatName = pluginInfo->formats[a].name; // return it's name   
		}


		if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_OGG)
			LPTSTR test= L"Ogg Vorbis";
		if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_MP1)
			LPTSTR test = L"MPEG layer 1";
		if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_MP2)
			LPTSTR test = L"MPEG layer 2";
		if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_MP3)
			LPTSTR test = L"MPEG layer 3";
		if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_AIFF)
			LPTSTR test = L"Audio IFF";
		if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_WAV_PCM)
			LPTSTR test = L"PCM WAVE";
		if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_WAV_FLOAT)
			LPTSTR test = L"Floating-point WAVE";
		if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_MF)
		{
			const WAVEFORMATEX *wf = (WAVEFORMATEX*)BASS_ChannelGetTags(hStreamID, BASS_TAG_WAVEFORMAT);
			if (wf->wFormatTag == 0x1610)
				LPTSTR test = L"Advanced Audio Coding";
			if (wf->wFormatTag == 0x0161 || wf->wFormatTag == 0x0162 || wf->wFormatTag == 0x0163)
				LPTSTR test = L"Windows Media Audio";
		}
		if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_WAV)
			LPTSTR test = L"WAVE";
	}
	*/

	return hStreamID;
}

bool			CBASSDecoderPlugin::Close(void)
{
	if (hStreamID)
	{
		if(bModFile)
			BASS_MusicFree(hStreamID);
		else
			BASS_StreamFree(hStreamID);
	}

	return true;
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

	if (!pDec->Open(szSource, m_pHelper, hStreamID, bModFile, bIsStream))
	{
		delete pDec;
		return(NULL);
	}

	return (IAudioSource *)pDec;
}
