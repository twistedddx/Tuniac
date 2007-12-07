#include "stdafx.h"
#include ".\audiostream.h"

#define CopyFloat(dst, src, num) CopyMemory(dst, src, (num) * sizeof(float))

CAudioStream::CAudioStream(IAudioSource * pSource, IPlaylistEntry * pEntry)
{
	m_pEntry		= pEntry;
	m_bEntryPlayed	= false;

	m_PlayState		= STATE_UNKNOWN;
	m_FadeState		= FADE_NONE;

	m_pSource		= pSource;

	fVolume			= 1.0f;
	m_bMixNotify	= false;
	m_bIsFinished	= false;
	m_bIsLast		= false;


	m_SamplesOut	= 0;

	m_Output.SetCallback(this);

	unsigned long srate;
	pSource->GetFormat(&srate, &m_Channels);
	m_Output.SetFormat(srate, m_Channels);

	m_Packetizer.SetPacketSize(m_Output.GetBlockSize());
}

CAudioStream::~CAudioStream(void)
{
	m_pSource->Destroy();
}

bool			CAudioStream::SetVolumeScale(float scale)
{
	fVolumeScale = scale;
	return true;
}

bool			CAudioStream::GetBuffer(float * pAudioBuffer, unsigned long NumSamples)
{
	CAutoLock t(&m_Lock);

	if(m_Packetizer.IsFinished())
	{
		if(!m_bEntryPlayed)
		{
			SYSTEMTIME st;
			GetLocalTime(&st);
			m_pEntry->SetField(FIELD_DATELASTPLAYED, &st);
			tuniacApp.m_SourceSelectorWindow->UpdateView();
			m_bEntryPlayed = true;
		}
		if(!m_bMixNotify)
		{
			tuniacApp.CoreAudioMessage(NOTIFY_MIXPOINTREACHED, NULL);
			m_bMixNotify = true;
		}
		m_bIsFinished = true;
	}

	while(!m_Packetizer.IsBufferAvailable())
	{
		float *			pBuffer			= NULL;
		unsigned long	ulNumSamples	= 0;

		if(m_pSource->GetBuffer(&pBuffer, &ulNumSamples))
		{
			m_Packetizer.WriteData(pBuffer, ulNumSamples);
		}
		else
		{
			m_Packetizer.Finished();
		}
	}

	if(m_Packetizer.GetBuffer(pAudioBuffer))
	{
		// if we're crossfading
		if(m_FadeState != FADE_NONE)
		{
			for(unsigned long x=0; x<NumSamples; x+=m_Channels)
			{
				for(unsigned long chan=0; chan<m_Channels; chan++)
				{
					pAudioBuffer[x+chan]		*= fVolume;
				}
				fVolume += fVolumeChange;
				fVolume = max(0.0f, min(fVolume, 1.0f));
			}

			if((fVolume == 0.0) || (fVolume == 1.0))
				m_FadeState = FADE_NONE;
		}

		// now apply the volume scale, only if we need to
		if(fVolumeScale != 1.0)
		{
			for(unsigned long x=0; x<NumSamples; x+=m_Channels)
			{
				for(unsigned long chan=0; chan<m_Channels; chan++)
				{
					pAudioBuffer[x+chan]		*= fVolumeScale;
				}
			}
		}

		m_Packetizer.Advance();
		m_SamplesOut +=  NumSamples;

		unsigned long pos = GetPosition();
		unsigned long len = GetLength();
		unsigned long cft = tuniacApp.m_Preferences.GetCrossfadeTime() * 1000;

		if(!m_bEntryPlayed)
		{
			if(pos > (len * 0.25))
			{
				SYSTEMTIME st;
				GetLocalTime(&st);
				m_pEntry->SetField(FIELD_DATELASTPLAYED, &st);

				tuniacApp.m_SourceSelectorWindow->UpdateView();

				m_bEntryPlayed = true;
			}
		}

		if(!m_bMixNotify)
		{
			if(len > cft)
			{
				if(pos >= (len - cft))
				{
					//crossfade all bar last song covered(it gets called but wont do reset at this point)
					m_bIsLast = !tuniacApp.m_PlaylistManager.GetActivePlaylist()->CheckNext();
					tuniacApp.CoreAudioMessage(NOTIFY_MIXPOINTREACHED, NULL);
					m_bMixNotify = true;
				}
			}
			else
			{
				//len is shorter then crossfade, start crossfade already
				m_bIsLast = !tuniacApp.m_PlaylistManager.GetActivePlaylist()->CheckNext();
				tuniacApp.CoreAudioMessage(NOTIFY_MIXPOINTREACHED, NULL);
				m_bMixNotify = true;
			}
		}
		else
		{
			//CheckNext() will fail as its called when the last song has started already
			//due to crossfade, if we simply call now without check of last song, last
			//song will finish seconds in.

			//if at end of last song in playlist but crossfade was the call n seconds ago
			//used for repeatnone where this is the end of line. 
			if(pos >= len && cft != 0 && m_bIsLast)
			{
				tuniacApp.CoreAudioMessage(NOTIFY_PLAYBACKFINISHED, NULL);
			}
		}
		return true;
	}
	return false;
}

unsigned long	CAudioStream::GetState(void)
{
	return m_PlayState;
}

unsigned long	CAudioStream::GetFadeState(void)
{
	return m_FadeState;
}

bool			CAudioStream::IsFinished(void)
{
	return m_bIsFinished;
}

bool			CAudioStream::FadeIn(unsigned long ulMS)
{
	CAutoLock t(&m_Lock);

	m_FadeState = FADE_FADEIN;
	fVolumeChange =  1.0f / ((float)ulMS * (float)m_Output.GetSampleRate());
	fVolume = 0.0f;
	return true;
}

bool			CAudioStream::FadeOut(unsigned long ulMS)
{
	CAutoLock t(&m_Lock);

	m_FadeState = FADE_FADEOUT;
	fVolumeChange =  -(1.0f / ((float)ulMS * (float)m_Output.GetSampleRate()));
	fVolume  = 1.0f;
	return true;
}

bool			CAudioStream::Start(void)
{
	tuniacApp.CoreAudioMessage(NOTIFY_PLAYBACKSTARTED, NULL);
	m_PlayState = STATE_PLAYING;
	return m_Output.Start();
}

bool			CAudioStream::Stop(void)
{
	m_PlayState = STATE_STOPPED;
	return m_Output.Stop();
}

unsigned long	CAudioStream::GetLength(void)
{
	unsigned long Length;

	if(m_pSource->GetLength(&Length))
		return Length;

	return LENGTH_UNKNOWN;
}

unsigned long	CAudioStream::GetPosition(void)
{
	unsigned long MSOutput = ((float)m_SamplesOut / (((float)m_Output.GetSampleRate()/1000.0f) * (float)m_Output.GetChannels()));
	return(MSOutput);
}

bool			CAudioStream::SetPosition(unsigned long MS)
{
	CAutoLock t(&m_Lock);

	if(m_FadeState != FADE_NONE)
	{
		fVolume		= 1.0;
		m_FadeState = FADE_NONE;
	}

	unsigned long Pos = MS;

	if(m_pSource->SetPosition(&Pos))
	{
		m_SamplesOut = Pos * (((float)m_Output.GetSampleRate()/1000.0f) * (float)m_Output.GetChannels());

		m_Packetizer.Reset();
		m_Output.Reset();

		return true;
	}

	return false;
}

bool			CAudioStream::GetVisData(float * ToHere, unsigned long ulNumSamples)
{
	return m_Output.GetVisData(ToHere, ulNumSamples);
}