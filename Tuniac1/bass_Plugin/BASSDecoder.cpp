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

			if(!BASS_PluginLoad(tempname, 0))
			{
				int x = BASS_ErrorGetCode();
				int b = x;
			}

		} while(FindNextFile(hFind, &w32fd));

		FindClose(hFind); 
	}

	BASS_Init(0,44100,0,0,NULL);

	decodehandle = BASS_StreamCreateFile(FALSE, szSource, 0, 0, BASS_STREAM_DECODE|BASS_UNICODE|BASS_SAMPLE_FLOAT);
	if(!decodehandle)
	{
		int x = BASS_ErrorGetCode();
		return false;
	}

	BASS_ChannelGetInfo(decodehandle,&info);

	qwBytes = BASS_ChannelGetLength(decodehandle,BASS_POS_BYTE);
	dTime = BASS_ChannelBytes2Seconds(decodehandle,qwBytes) * 1000;
	qwBytePos = 0;

	return(true);
}

bool CBASSDecoder::Close()
{
	BASS_StreamFree(decodehandle);
	BASS_Free();
	BASS_PluginFree(0);
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
	qwBytePos = pos;
	return(true);
}

bool		CBASSDecoder::SetState(unsigned long State)
{
	return(true);
}

bool		CBASSDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	if(qwBytePos >= qwBytes)
		return false;

	DWORD readBytes = BASS_ChannelGetData(decodehandle, m_Buffer, 4096);
	if(!readBytes)
		return false;

	qwBytePos += readBytes;

	unsigned long numSamples = readBytes / 4;

	*ppBuffer = m_Buffer;

	*NumSamples = numSamples;

	return(true);
}