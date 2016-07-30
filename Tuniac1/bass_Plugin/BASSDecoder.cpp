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
	//attempt to put station details in "artist" and track details in "title" at least.

	char *icy=(char *)BASS_ChannelGetTags(handle,BASS_TAG_ICY);
	TCHAR szArtist[128];
	TCHAR szGenre[128];
	TCHAR szTitle[128];
	TCHAR szAlbum[128];
	TCHAR szComment[128];
	int iBitrate;
	if(icy)
	{ // got ICY metadata
		for(;*icy;icy+=strnlen_s(icy,2048)+1)
		{
			//station name
			if (!strnicmp(icy,"icy-name:",9))
			{
				MultiByteToWideChar(CP_ACP, 0, icy+9, -1, szArtist, 128);
				if(m_pHelper)
					m_pHelper->UpdateMetaData((LPTSTR)user, szArtist, FIELD_ARTIST);
			}

			//station Genre
			if (!strnicmp(icy,"icy-genre:",10))
			{
				MultiByteToWideChar(CP_ACP, 0, icy+10, -1, szGenre, 128);
				if(m_pHelper)
					m_pHelper->UpdateMetaData((LPTSTR)user, szGenre, FIELD_GENRE);
			}

			//Station bitrate
			if (!strnicmp(icy,"icy-br:",7))
			{
				iBitrate = (atoi(icy+7))*1000;
				if(m_pHelper)
					m_pHelper->UpdateMetaData((LPTSTR)user, (unsigned long)iBitrate, FIELD_BITRATE);
			}
			/* these should already been known during open
			ice-samplerate:
			ice-channels:
			*/
		}
	}
	char *meta=(char *)BASS_ChannelGetTags(handle,BASS_TAG_META);
	if(meta)
	{ // got Shoutcast metadata StreamTitle='title';StreamUrl='http://www.website.com';
		char *title=strstr(meta,"StreamTitle='");
		char *url=strstr(meta,"StreamUrl='");

		// track details, normally Artist - Title
		if(title)
		{
			title=strdup(title+13);
			strchr(title,';')[-1]=0;
			MultiByteToWideChar(CP_ACP, 0, title, -1, szTitle, 128);
			if(m_pHelper)
				m_pHelper->UpdateMetaData((LPTSTR)user, szTitle, FIELD_TITLE);
		}
		/* //should already be filled during import
		if(url && szTitle == NULL)
		{
			url=strdup(url+11);
			strchr(url,';')[-1]=0;
			MultiByteToWideChar(CP_ACP, 0, url, -1, szTitle, 128);
			if(m_pHelper)
				m_pHelper->UpdateMetaData((LPTSTR)user, szTitle, FIELD_ARTIST);
		}
		*/
	}
	else
	{
		meta=(char *)BASS_ChannelGetTags(handle,BASS_TAG_OGG);
		if(meta)
		{ // got Icecast/OGG tags
			const char *artist = NULL;
			const char *title = NULL;
			const char *album = NULL;
			const char *comment = NULL;
			const char *p = meta;
			for(;*p;p+=strnlen_s(p,2048)+1)
			{
				if (!strnicmp(p,"artist=",7)) // found the artist
					artist=p+7;
				if (!strnicmp(p,"title=",6)) // found the title
					title=p+6;
				if (!strnicmp(p, "album=", 6)) // found the album
					album=p+6;
				if (!strnicmp(p, "comment=", 8)) // found the comment
					comment = p + 8;
				//server="Icecast-2.3.3"
				//encoder=
			}
			if (strnlen_s(title, 1) && strnlen_s(artist, 1))
			{
				MultiByteToWideChar(CP_ACP, 0, title, -1, szTitle, 128);
				MultiByteToWideChar(CP_UTF8, 0, artist, -1, szArtist, 128);

				TCHAR szText[128];
				_snwprintf(szText, sizeof(szText), L"%s - %s", szArtist, szTitle);

				if (m_pHelper)
					m_pHelper->UpdateMetaData((LPTSTR)user, szText, FIELD_TITLE);
			}
			else if(strnlen_s(title, 1))
			{
				MultiByteToWideChar(CP_UTF8, 0, title, -1, szTitle, 128);
				if(m_pHelper)
					m_pHelper->UpdateMetaData((LPTSTR)user, szTitle, FIELD_TITLE);
			}
			if(strnlen_s(album, 2))
			{
				MultiByteToWideChar(CP_UTF8, 0, album, -1, szAlbum, 128);
				if(m_pHelper)
					m_pHelper->UpdateMetaData((LPTSTR)user, szAlbum, FIELD_ALBUM);
			}
			if (strnlen_s(comment, 1))
			{
				MultiByteToWideChar(CP_UTF8, 0, comment, -1, szComment, 128);
				if (m_pHelper)
					m_pHelper->UpdateMetaData((LPTSTR)user, szComment, FIELD_COMMENT);
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

/*bool CBASSDecoder::Open(LPTSTR szSource, IAudioSourceHelper * pHelper)
{
	bModFile = false;
	bIsStream = PathIsURL(szSource);

	if(!bIsStream)
	{
		if(!(decodehandle = BASS_StreamCreateFile(FALSE, szSource, 0, 0, BASS_STREAM_DECODE|BASS_ASYNCFILE|BASS_UNICODE|BASS_SAMPLE_FLOAT)))
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
			wchar_t cDrive;
			int iTrack;
			swscanf_s(szSource, TEXT("AUDIOCD:%c:%d"), &cDrive, sizeof(char), &iTrack);
			StringCchPrintf(szSource, 128, TEXT("%C:\\Track%02i.cda"), cDrive, iTrack);
			decodehandle = BASS_StreamCreateFile(FALSE, szSource, 0, 0, BASS_STREAM_DECODE|BASS_ASYNCFILE|BASS_UNICODE|BASS_SAMPLE_FLOAT);
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
*/
bool CBASSDecoder::Open(LPTSTR szSource, IAudioSourceHelper * pHelper, HSTREAM decodehandle, bool bModFile, bool bIsStream)
{
	m_bModFile = bModFile;
	m_bIsStream = bIsStream;
	m_decodehandle = decodehandle;

	m_pHelper = pHelper;

	if(m_bIsStream)
	{
		DoMeta(m_decodehandle, szSource);
		BASS_ChannelSetSync(m_decodehandle,BASS_SYNC_META,0,&MetaSync, szSource); // Shoutcast
		BASS_ChannelSetSync(m_decodehandle,BASS_SYNC_OGG_CHANGE,0,&MetaSync, szSource); // Icecast/OGG
	}

	BASS_ChannelGetInfo(m_decodehandle,&info);

	if(m_bIsStream)
		dTime = LENGTH_STREAM;
	else
		dTime = LENGTH_UNKNOWN;

	long long len = BASS_ChannelGetLength(m_decodehandle,BASS_POS_BYTE);
	if(len > 0)
		dTime = BASS_ChannelBytes2Seconds(m_decodehandle, len) * 1000;

	if(bIsStream) //update unknowns for streams
	{
		m_pHelper->UpdateMetaData(szSource, (unsigned long)dTime, FIELD_PLAYBACKTIME);
		m_pHelper->UpdateMetaData(szSource, (unsigned long)info.chans, FIELD_NUMCHANNELS);
		m_pHelper->UpdateMetaData(szSource, (unsigned long)info.freq, FIELD_SAMPLERATE);
	}

	m_Buffer = (float*)_aligned_malloc(BUFFERSIZE, 16);

	return(true);
}

bool CBASSDecoder::Close()
{
	if(m_bModFile)
		BASS_MusicFree(m_decodehandle);
	else
		BASS_StreamFree(m_decodehandle);

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
	DWORD pos = BASS_ChannelSeconds2Bytes(m_decodehandle,(*MS/1000));
	BASS_ChannelSetPosition(m_decodehandle, pos, BASS_POS_BYTE);
	return(true);
}

bool		CBASSDecoder::SetState(unsigned long State)
{
	/*
	//BASS_Start and BASS_Pause relate to when BASS outputs to soundcard itself
	//we want to handle these for streams as they timeout while paused.
	if(State == STATE_PLAYING)
		BASS_Start();

	if(State == STATE_STOPPED)
		BASS_Pause();
	*/
	return(true);

}

bool		CBASSDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	if(BASS_ChannelIsActive(m_decodehandle) == BASS_ACTIVE_STOPPED)
		return false;

	DWORD readBytes = BASS_ChannelGetData(m_decodehandle, m_Buffer, BUFFERSIZE);
	unsigned long numSamples = readBytes / sizeof(float);

	if(readBytes != BUFFERSIZE)
		ZeroMemory(&m_Buffer[numSamples], BUFFERSIZE-readBytes);

	*ppBuffer = m_Buffer;

	*NumSamples = numSamples;

	return(true);
}