#include "stdafx.h"
#include ".\audiostream.h"

#define CopyFloat(dst, src, num) CopyMemory(dst, src, (num) * sizeof(float))

CAudioStream::CAudioStream(IAudioSource * pSource, IPlaylistEntry * pEntry, IXAudio2 * pXAudio) :
	m_Output(pXAudio)
{
	m_pEntry		= pEntry;
	m_bEntryPlayed	= false;

	m_PlayState		= STATE_UNKNOWN;
	m_FadeState		= FADE_NONE;

	m_pSource		= pSource;

	fVolume			= 1.0f;
	m_bMixNotify	= false;
	m_bFinishNotify	= false;
	m_bIsFinished	= false;


	m_SamplesOut	= 0;

	m_CrossfadeTimeMS = 0;


	unsigned long srate;
	pSource->GetFormat(&srate, &m_Channels);
	m_Output.SetFormat(srate, m_Channels);

	m_Packetizer.SetPacketSize(m_Output.GetBlockSize());

	m_Output.SetCallback(this);
}

CAudioStream::~CAudioStream(void)
{
	m_Output.Shutdown();

	m_pSource->Destroy();
}

bool CAudioStream::Initialize(void)
{
	return true;
}

bool CAudioStream::Shutdown(void)
{
	return true;
}

bool			CAudioStream::SetVolumeScale(float scale)
{
	fVolumeScale = scale;
	return true;
}

bool			CAudioStream::ServiceStream(void)
{
	if(m_Packetizer.IsFinished())
	{
		return false;
	}

	if(!m_Packetizer.IsBufferAvailable())
	{
		float *			pBuffer			= NULL;
		unsigned long	ulNumSamples	= 0;

		//get buffer
		CAutoLock t(&m_Lock);
		if(m_pSource->GetBuffer(&pBuffer, &ulNumSamples))
		{
			m_Packetizer.WriteData(pBuffer, ulNumSamples);
			return true;
		}

		// there are no more buffers to get!!
		m_Packetizer.Finished();
	}

	return false;
}

bool			CAudioStream::GetBuffer(float * pAudioBuffer, unsigned long NumSamples)
{
	CAutoLock t(&m_Lock);

	while(ServiceStream())
	{
	}

	if(m_Packetizer.AnyMoreBuffer())
	{
		if(m_Packetizer.GetBuffer(pAudioBuffer))
		{
			m_SamplesOut +=  NumSamples;

			if(m_FadeState != FADE_NONE)
			{
				for(unsigned long x=0; x<NumSamples; x+=m_Channels)
				{
					for(unsigned long chan=0; chan<m_Channels; chan++)
					{
						// apply the crossfade
						pAudioBuffer[x+chan]		*= fVolume;

						// and apply the volume
						pAudioBuffer[x+chan]		*= fVolumeScale;
					}
					fVolume += fVolumeChange;
					fVolume = max(0.0f, min(fVolume, 1.0f));
				}
			}

			//check if coreaudio has not been notified yet and if we are crossfading
			if(!m_bMixNotify)
			{
				//length longer then crossfade time
				if((GetLength() - GetPosition()) < m_CrossfadeTimeMS)
				{
					m_bMixNotify = true;
					tuniacApp.CoreAudioMessage(NOTIFY_MIXPOINTREACHED, NULL);
				}
			}
			return true;
		}
	}

	if(m_Packetizer.IsFinished())
	{
		if(!m_Packetizer.AnyMoreBuffer())
		{
			if(!m_bFinishNotify)
			{
				m_bFinishNotify = true;
				m_bIsFinished = true;
				tuniacApp.CoreAudioMessage(NOTIFY_PLAYBACKFINISHED, NULL);
			}
		}
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
	//CAutoLock t(&m_Lock);

	m_FadeState = FADE_FADEIN;
	fVolumeChange =  1.0f / ((float)ulMS * (float)m_Output.GetSampleRate());
	fVolume = 0.0f;
	return true;
}

bool			CAudioStream::FadeOut(unsigned long ulMS)
{
	//CAutoLock t(&m_Lock);

	m_FadeState = FADE_FADEOUT;
	fVolumeChange =  (-1.0f / ((float)ulMS * (float)m_Output.GetSampleRate()));
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
	unsigned long MSOutput = (m_SamplesOut / ((m_Output.GetSampleRate()/1000) * m_Output.GetChannels()));

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

	m_Output.Stop();
	if(m_pSource->SetPosition(&Pos))
	{
		m_SamplesOut = Pos * ((m_Output.GetSampleRate()/1000) * m_Output.GetChannels());

		m_Packetizer.Reset();
		m_Output.Reset();

		if(m_PlayState == STATE_PLAYING)
			m_Output.Start();


		return true;
	}

	return false;
}

bool			CAudioStream::GetVisData(float * ToHere, unsigned long ulNumSamples)
{
	return m_Output.GetVisData(ToHere, ulNumSamples);
}



/*
	//if no buffer available
	while(!m_Packetizer.IsBufferAvailable())
	{
		float *			pBuffer			= NULL;
		unsigned long	ulNumSamples	= 0;

		//get buffer
		if(m_pSource->GetBuffer(&pBuffer, &ulNumSamples))
		{
			m_Packetizer.WriteData(pBuffer, ulNumSamples);
		}
		//can not buffer(end of song?)
		else
		{	
			if(!m_bFinishNotify)
			{
				m_bIsFinished = true;
				m_Packetizer.Finished();
				tuniacApp.CoreAudioMessage(NOTIFY_PLAYBACKFINISHED, NULL);
			}
		}
	}
*/


/*
	if(m_Packetizer.GetBuffer(pAudioBuffer))
	{
		
		//crossfading code
		if(m_FadeState != FADE_NONE)
		{
			
			if(m_FadeState == FADE_FADEIN)
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
			}
			else if(m_FadeState == FADE_FADEOUT)
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
			}


//			if( fVolume >= 1.0 )
//			{
//				m_FadeState = FADE_NONE;
//			}
			//crossfade completed, we want to stop this song!
			if( fVolume <= 0.0 )
			{
				m_FadeState = FADE_NONE;
			}
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
		m_SamplesOut +=  NumSamples;

		//set time file was played after 1/4 is played
		if(!m_bEntryPlayed)
		{
			if(GetPosition() > (GetLength() * 0.25))
			{
				SYSTEMTIME st;
				GetLocalTime(&st);
				m_pEntry->SetField(FIELD_DATELASTPLAYED, &st);

				tuniacApp.m_SourceSelectorWindow->UpdateView();

				m_bEntryPlayed = true;
			}
		}

		unsigned long cft = tuniacApp.m_Preferences.GetCrossfadeTime() * 1000;

		//check if coreaudio has not been notified yet and if we are crossfading
		if(!m_bMixNotify && cft)
		{
			//length longer then crossfade time
			if(GetLength() > cft)
			{
				//position greaten or equal to length minus crossfade (standard point to crossfade)
				if(GetPosition() >= (GetLength() - cft))
				{
					tuniacApp.CoreAudioMessage(NOTIFY_MIXPOINTREACHED, NULL);
					m_bMixNotify = true;
				}
			}
			//length shorten than crossfade time
			else
			{
				tuniacApp.CoreAudioMessage(NOTIFY_MIXPOINTREACHED, NULL);
				m_bMixNotify = true;
			}
		}

		return true;

	}
*/