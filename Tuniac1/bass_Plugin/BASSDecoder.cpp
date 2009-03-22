#include "StdAfx.h"
#include "bassdecoder.h"

CBASSDecoder::CBASSDecoder(void)
{
}

CBASSDecoder::~CBASSDecoder(void)
{
}

bool CBASSDecoder::Open(LPTSTR szSource)
{
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

	bIsStream = false;
	BASS_Init(0,44100,0,0,NULL);
	if(!PathIsURL(szSource))
	{
		decodehandle = BASS_StreamCreateFile(FALSE, szSource, 0, 0, BASS_STREAM_DECODE|BASS_UNICODE|BASS_SAMPLE_FLOAT);
	}
	else
	{
		if(!StrCmpN(szSource, TEXT("AUDIOCD"), 7))
		{
			char cDrive;
			int iTrack;
			swscanf_s(szSource, TEXT("AUDIOCD:%c:%d"), &cDrive, sizeof(char), &iTrack);
			wsprintf(szSource, TEXT("%C:\\Track%02i.cda"), cDrive, iTrack);
			decodehandle = BASS_StreamCreateFile(FALSE, szSource, 0, 0, BASS_STREAM_DECODE|BASS_UNICODE|BASS_SAMPLE_FLOAT);
		}
		else
		{
			char mbURL[512]; 	 
			WideCharToMultiByte(CP_UTF8, 0, szSource, -1, mbURL, 512, 0, 0);
			decodehandle = BASS_StreamCreateURL(mbURL,0, BASS_STREAM_DECODE|BASS_SAMPLE_FLOAT, NULL, 0);
			bIsStream = true;
		}
	}

	if(!decodehandle)
		return false;

	BASS_ChannelGetInfo(decodehandle,&info);

	dTime = LENGTH_UNKNOWN;
	if(!bIsStream)
		dTime = BASS_ChannelBytes2Seconds(decodehandle,BASS_ChannelGetLength(decodehandle,BASS_POS_BYTE)) * 1000;

	m_Buffer = (float *)VirtualAlloc(NULL, BUFFERSIZE, MEM_COMMIT, PAGE_READWRITE);
	VirtualLock(m_Buffer, BUFFERSIZE);

	return(true);
}

bool CBASSDecoder::Close()
{
	BASS_StreamFree(decodehandle);
	BASS_Free();
	BASS_PluginFree(0);

	if(m_Buffer)
	{
		VirtualUnlock(m_Buffer, BUFFERSIZE);
		VirtualFree(m_Buffer, 0, MEM_RELEASE);
	}

	return(true);
}

void		CBASSDecoder::Destroy(void)
{
	Close();
	delete this;
}

bool		CBASSDecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate = info.freq;
	*Channels	= info.chans;

	return(true);
}

bool		CBASSDecoder::GetLength(unsigned long * MS)
{
	*MS = dTime;
	return(true);
}

bool		CBASSDecoder::SetPosition(unsigned long * MS)
{
	DWORD pos = BASS_ChannelSeconds2Bytes(decodehandle,(*MS/1000));
	BASS_ChannelSetPosition(decodehandle, pos, BASS_POS_BYTE);
	return(true);
}

bool		CBASSDecoder::SetState(unsigned long State)
{
	return(true);
}

bool		CBASSDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	if(BASS_ChannelIsActive(decodehandle) == BASS_ACTIVE_STOPPED)
		return false;

	DWORD readBytes = BASS_ChannelGetData(decodehandle, m_Buffer, BUFFERSIZE);

	unsigned long numSamples = readBytes / sizeof(float);

	*ppBuffer = m_Buffer;

	*NumSamples = numSamples;

	return(true);
}