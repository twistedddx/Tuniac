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
    MessageBox(hParent, TEXT("BASS Plugin\nUsing BASS 2.4\nCopyright (c) 1999-2019 Un4seen Developments Ltd."), GetName(), MB_OK | MB_ICONINFORMATION);
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

	// find out what file format was actually found by BASS.
	if (hStreamID)
	{
		BASS_CHANNELINFO bassChannelInfo;
		BASS_ChannelGetInfo(hStreamID, &bassChannelInfo);


		// bass basic
		if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_OGG)
			m_pHelper->UpdateMetaData(szSource, L"ogg", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_MP1)
			m_pHelper->UpdateMetaData(szSource, L"mp1", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_MP2)
			m_pHelper->UpdateMetaData(szSource, L"mp2", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_MP3)
			m_pHelper->UpdateMetaData(szSource, L"mp3", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_AIFF)
			m_pHelper->UpdateMetaData(szSource, L"aiff", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_WAV_PCM)
			m_pHelper->UpdateMetaData(szSource, L"wav", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_WAV_FLOAT)
			m_pHelper->UpdateMetaData(szSource, L"wav-float", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_WAV)
			m_pHelper->UpdateMetaData(szSource, L"wav", FIELD_FILETYPE);

		// bass plugins
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_AAC)
			m_pHelper->UpdateMetaData(szSource, L"aac", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_MP4)
			m_pHelper->UpdateMetaData(szSource, L"mp4", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_AC3)
			m_pHelper->UpdateMetaData(szSource, L"ac3", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_ADX)
			m_pHelper->UpdateMetaData(szSource, L"adx", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_APE)
			m_pHelper->UpdateMetaData(szSource, L"ape", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_MPC)
			m_pHelper->UpdateMetaData(szSource, L"mpc", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_OFR)
			m_pHelper->UpdateMetaData(szSource, L"ofr", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_SPX)
			m_pHelper->UpdateMetaData(szSource, L"spx", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_TTA)
			m_pHelper->UpdateMetaData(szSource, L"tta", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_ALAC)
			m_pHelper->UpdateMetaData(szSource, L"alac", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_CD)
			m_pHelper->UpdateMetaData(szSource, L"audio-cd", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_DSD)
			m_pHelper->UpdateMetaData(szSource, L"dsd", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_FLAC)
			m_pHelper->UpdateMetaData(szSource, L"flac", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_FLAC_OGG)
			m_pHelper->UpdateMetaData(szSource, L"flac-ogg", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_MIDI)
			m_pHelper->UpdateMetaData(szSource, L"midi", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_OPUS)
			m_pHelper->UpdateMetaData(szSource, L"opus", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_WMA)
			m_pHelper->UpdateMetaData(szSource, L"wma", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_WMA_MP3)
			m_pHelper->UpdateMetaData(szSource, L"wma-mp3", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_WV)
			m_pHelper->UpdateMetaData(szSource, L"wv", FIELD_FILETYPE);

		// bass other
		else if (bassChannelInfo.ctype == BASS_CTYPE_MUSIC_MOD)
			m_pHelper->UpdateMetaData(szSource, L"mod", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_MUSIC_MTM)
			m_pHelper->UpdateMetaData(szSource, L"mtm", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_MUSIC_S3M)
			m_pHelper->UpdateMetaData(szSource, L"s3m", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_MUSIC_XM)
			m_pHelper->UpdateMetaData(szSource, L"xm", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_MUSIC_IT)
			m_pHelper->UpdateMetaData(szSource, L"it", FIELD_FILETYPE);
		else if (bassChannelInfo.ctype == BASS_CTYPE_MUSIC_MO3)
			m_pHelper->UpdateMetaData(szSource, L"mo3", FIELD_FILETYPE);

		/* disabled
		// bass media foundation
		else if (bassChannelInfo.ctype == BASS_CTYPE_STREAM_MF)
		{
			const WAVEFORMATEX *wf = (WAVEFORMATEX*)BASS_ChannelGetTags(hStreamID, BASS_TAG_WAVEFORMAT);
			if (wf->wFormatTag == 0x1600)
				m_pHelper->UpdateMetaData(szSource, L"adts mf", FIELD_FILETYPE);
			else if (wf->wFormatTag == 0x1602)
				m_pHelper->UpdateMetaData(szSource, L"mpeg4 mf", FIELD_FILETYPE);
			else if (wf->wFormatTag == 0x1610)
				m_pHelper->UpdateMetaData(szSource, L"he-aac mf", FIELD_FILETYPE);
			else if (wf->wFormatTag == 0x0161)
				m_pHelper->UpdateMetaData(szSource, L"wma mf", FIELD_FILETYPE);
			else if (wf->wFormatTag == 0x0162)
				m_pHelper->UpdateMetaData(szSource, L"wma-pro mf", FIELD_FILETYPE);
			else if (wf->wFormatTag == 0x0163) 
				m_pHelper->UpdateMetaData(szSource, L"wma-lossless mf", FIELD_FILETYPE);
			else
				m_pHelper->UpdateMetaData(szSource, L"other mf", FIELD_FILETYPE);
		}
		*/
		else
			m_pHelper->UpdateMetaData(szSource, PathFindExtension(szSource)+1, FIELD_FILETYPE);

	}
	
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
