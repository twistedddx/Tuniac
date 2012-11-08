#include "StdAfx.h"
#include "bassdecoder.h"

IAudioSourceHelper *m_pHelper;

CBASSDecoder::CBASSDecoder(void)
{
}

CBASSDecoder::~CBASSDecoder(void)
{
}

void DoMeta(DWORD handle, void *user)
{
	char *icy=(char *)BASS_ChannelGetTags(handle,BASS_TAG_ICY);
	TCHAR szArtist[128];
	TCHAR szGenre[128];
	TCHAR szTitle[128];
	int iBitrate;
	if(icy)
	{ // got ICY metadata
		for(;*icy;icy+=strlen(icy)+1)
		{
			if (!strnicmp(icy,"icy-name:",9))
			{
				MultiByteToWideChar(CP_ACP, 0, icy+9, -1, szTitle, 128);
				m_pHelper->UpdateMetaData((LPTSTR)user, szTitle, FIELD_TITLE);
			}

			if (!strnicmp(icy,"icy-genre:",10))
			{
				MultiByteToWideChar(CP_ACP, 0, icy+10, -1, szGenre, 128);
				m_pHelper->UpdateMetaData((LPTSTR)user, szGenre, FIELD_GENRE);
			}
			if (!strnicmp(icy,"icy-br:",7))
			{
				iBitrate = (atoi(icy+7))*1000;
				m_pHelper->UpdateMetaData((LPTSTR)user, (void *)iBitrate, FIELD_BITRATE);
			}
			/* these should already been known
			ice-samplerate:
			ice-channels:
			ice-bitrate:
			*/
		}
	}
	char *meta=(char *)BASS_ChannelGetTags(handle,BASS_TAG_META);
	if(meta)
	{ // got Shoutcast metadata StreamTitle='title';StreamUrl='http://www.website.com';
		char *title=strstr(meta,"StreamTitle='");
		char *url=strstr(meta,"StreamUrl='");
		if(title)
		{
			title=strdup(title+13);
			strchr(title,';')[-1]=0;
			MultiByteToWideChar(CP_ACP, 0, title, -1, szArtist, 128);
			m_pHelper->UpdateMetaData((LPTSTR)user, szArtist, FIELD_ARTIST);
		}
		if(url && szArtist == NULL)
		{
			url=strdup(url+11);
			strchr(url,';')[-1]=0;
			MultiByteToWideChar(CP_ACP, 0, url, -1, szTitle, 128);
			m_pHelper->UpdateMetaData((LPTSTR)user, szTitle, FIELD_TITLE);
		}
	}
	else
	{
		meta=(char *)BASS_ChannelGetTags(handle,BASS_TAG_OGG);
		if(meta)
		{ // got Icecast/OGG tags
			const char *artist=NULL,*title=NULL,*p=meta;
			for(;*p;p+=strlen(p)+1)
			{
				if (!strnicmp(p,"artist=",7)) // found the artist
					artist=p+7;
				if (!strnicmp(p,"title=",6)) // found the title
					title=p+6;
			}
			if(artist && szTitle == NULL)
			{
				char text[100];
				_snprintf(text,sizeof(text),"%s - %s",artist,title);
				MultiByteToWideChar(CP_ACP, 0, text, -1, szArtist, 128);
				m_pHelper->UpdateMetaData((LPTSTR)user, szArtist, FIELD_ARTIST);
			}
			else if(title  && szTitle == NULL)
			{
				MultiByteToWideChar(CP_ACP, 0, title, -1, szArtist, 128);
				m_pHelper->UpdateMetaData((LPTSTR)user, szArtist, FIELD_ARTIST);
			}
		}
    }
	/*
	char *meta=(char *)BASS_ChannelGetTags(handle,BASS_TAG_WMA_META);
	if(meta)
	{ // got WMA tag

	}
	*/
}

void CALLBACK MetaSync(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	DoMeta(channel, user);
}

bool CBASSDecoder::Open(LPTSTR szSource, IAudioSourceHelper * pHelper)
{
	bModFile = false;
	bIsStream = PathIsURL(szSource);

	if(!bIsStream)
	{
		if(!(decodehandle = BASS_StreamCreateFile(FALSE, szSource, 0, 0, BASS_STREAM_DECODE|BASS_UNICODE|BASS_SAMPLE_FLOAT)))
		{
			if(decodehandle = BASS_MusicLoad(FALSE, szSource, 0, 0, BASS_MUSIC_DECODE|BASS_UNICODE|BASS_SAMPLE_FLOAT|BASS_MUSIC_RAMP|BASS_MUSIC_PRESCAN, 0))
				bModFile = true;
		}
	}
	else
	{
		if(StrCmpN(szSource, TEXT("AUDIOCD"), 7) == 0)
		{
			bIsStream = false;
			char cDrive;
			int iTrack;
			swscanf_s(szSource, TEXT("AUDIOCD:%c:%d"), &cDrive, sizeof(char), &iTrack);
			_snwprintf(szSource, 128, TEXT("%C:\\Track%02i.cda"), cDrive, iTrack);
			decodehandle = BASS_StreamCreateFile(FALSE, szSource, 0, 0, BASS_STREAM_DECODE|BASS_UNICODE|BASS_SAMPLE_FLOAT);
		}
		else
		{
			char mbURL[512]; 	 
			WideCharToMultiByte(CP_UTF8, 0, szSource, -1, mbURL, 512, 0, 0);
			decodehandle = BASS_StreamCreateURL(mbURL,0, BASS_STREAM_DECODE|BASS_SAMPLE_FLOAT|BASS_STREAM_BLOCK, NULL, 0);

			if(decodehandle)
			{
				DoMeta(decodehandle, szSource);
				BASS_ChannelSetSync(decodehandle,BASS_SYNC_META,0,&MetaSync, szSource); // Shoutcast
				BASS_ChannelSetSync(decodehandle,BASS_SYNC_OGG_CHANGE,0,&MetaSync, szSource); // Icecast/OGG
				//BASS_ChannelSetSync(decodehandle,BASS_SYNC_WMA_CHANGE,0,&MetaSync, szSource); // WMA
			}
		}
	}

	if(!decodehandle)
		return false;

	m_pHelper = pHelper;

	BASS_ChannelGetInfo(decodehandle,&info);

	dTime = LENGTH_UNKNOWN;
	long long len = BASS_ChannelGetLength(decodehandle,BASS_POS_BYTE);
	if(len > 0)
		dTime = BASS_ChannelBytes2Seconds(decodehandle, len) * 1000;

	if(bIsStream) //update unknowns for streams
	{
		m_pHelper->UpdateMetaData(szSource, (void *)(int)dTime, FIELD_PLAYBACKTIME);
		m_pHelper->UpdateMetaData(szSource, (void *)(int)info.chans, FIELD_NUMCHANNELS);
		m_pHelper->UpdateMetaData(szSource, (void *)(int)info.freq, FIELD_SAMPLERATE);
	}

	m_Buffer = (float*)_aligned_malloc(BUFFERSIZE, 16);

	return(true);
}

bool CBASSDecoder::Close()
{
	if(bModFile)
		BASS_MusicFree(decodehandle);
	else
		BASS_StreamFree(decodehandle);

	if(m_Buffer)
	{
		_aligned_free(m_Buffer);
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

	if(readBytes != BUFFERSIZE)
		ZeroMemory(&m_Buffer[numSamples], BUFFERSIZE-readBytes);

	*ppBuffer = m_Buffer;

	*NumSamples = numSamples;

	return(true);
}