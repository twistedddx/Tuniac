/*
	Copyright (C) 2003-2008 Tony Million

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation is required.

	2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.
*/

#include "stdafx.h"
#include ".\audiostream.h"

#define CopyFloat(dst, src, num) CopyMemory(dst, src, (num) * sizeof(float))

CAudioStream::CAudioStream()
{
	m_bEntryPlayed	= false;

	m_PlayState		= STATE_UNKNOWN;
	m_FadeState		= FADE_NONE;

	fReplayGainTrack	= 1.0f;
	fReplayGainAlbum	= 1.0f;

	fVolume			= 1.0f;
	m_bMixNotify	= false;
	m_bFinishNotify	= false;
	m_bIsFinished	= false;

	bTrackHasGain	= false;


	m_ulLastSeekMS	= 0;

	m_CrossfadeTimeMS = 0;

	m_Output	= NULL;
}

CAudioStream::~CAudioStream(void)
{

}

bool CAudioStream::Initialize(IAudioSource * pSource, CAudioOutput * pOutput)
{
	unsigned long srate;
	m_pSource		= pSource;
	m_Output		= pOutput;


	if(m_pSource->GetFormat(&srate, &m_Channels))
	{
		if(!pOutput->SetFormat(srate, m_Channels))
			return false;
	}
	else
		return false;

	m_Packetizer.SetPacketSize(m_Output->GetBlockSize());
	m_Output->SetCallback(this);

	fAmpGain = 1.99526231496888f;

	return true;
}

bool CAudioStream::Shutdown(void)
{
	Stop(); 

	if(m_Output)
	{
		m_Output->Destroy();
		m_Output = NULL;
	}

	if(m_pSource)
	{
		m_pSource->Destroy();
	}

	return true;
}

void CAudioStream::Destroy()
{
	Shutdown();
	delete this;
}

bool			CAudioStream::SetReplayGainScale(float trackscale, float albumscale)
{
	//float fReplayGainScale = pow(10, fReplayGain / 20);

	if(trackscale == 0.0 )
		bTrackHasGain = false;
	else
		bTrackHasGain = true;

	fReplayGainTrack = pow(10, trackscale / 20);;
	fReplayGainAlbum = pow(10, albumscale / 20);;

	return true;
}

void			CAudioStream::EnableReplayGain(bool bEnable)
{
	bReplayGain = bEnable;
}

void			CAudioStream::UseAlbumGain(bool bUse)
{
	bUseAlbumGain = bUse;
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

	unsigned long ulSongLength = GetLength();

	while(ServiceStream())
	{
	}

	if(m_Packetizer.AnyMoreBuffer())
	{
		if(m_Packetizer.GetBuffer(pAudioBuffer))
		{
			if(m_FadeState != FADE_NONE)
			{
				for(unsigned long x=0; x<NumSamples; x+=m_Channels)
				{
					for(unsigned long chan=0; chan<m_Channels; chan++)
					{
						// is replaygain set?
						if(bReplayGain && bTrackHasGain)
						{
							// +6db replaygain files
							pAudioBuffer[x+chan]		*= fAmpGain;

							// replaygain
							if(bUseAlbumGain)
								pAudioBuffer[x+chan]		*= fReplayGainAlbum;
							else
								pAudioBuffer[x+chan]		*= fReplayGainTrack;
						}

						// apply the crossfade
						pAudioBuffer[x+chan]		*= fVolume;

						// and apply the volume
						pAudioBuffer[x+chan]		*= fVolumeScale;
					}
					fVolume += fVolumeChange;
					fVolume = max(0.0f, min(fVolume, 1.0f));
				}
			}
			else
			{
				for(unsigned long x=0; x<NumSamples; x+=m_Channels)
				{
					for(unsigned long chan=0; chan<m_Channels; chan++)
					{
						// is replaygain set?
						if(bReplayGain && bTrackHasGain)
						{
							// +6db replaygain files
							pAudioBuffer[x+chan]		*= fAmpGain;

							// replaygain
							if(bUseAlbumGain)
								pAudioBuffer[x+chan]		*= fReplayGainAlbum;
							else
								pAudioBuffer[x+chan]		*= fReplayGainTrack;
						}

						// and apply the volume
						pAudioBuffer[x+chan]		*= fVolumeScale;
					}
				}
			}

			//check if coreaudio has not been notified yet and if we are crossfading
			if(ulSongLength != LENGTH_UNKNOWN)
			{			
				if(!m_bMixNotify)
				{
					//length longer then crossfade time

					if((ulSongLength - (GetPosition() + m_Output->GetAudioBufferMS())) < m_CrossfadeTimeMS)
					{
						m_bMixNotify = true;
						tuniacApp.CoreAudioMessage(NOTIFY_MIXPOINTREACHED, NULL);
					}
				}
			}

			// TODO: if we have played more than 'x' percent of a song send a last played notification back to our controller
			// could be a LastFM thing there!!

			return true;
		}
	}
	else
	{
		if(m_Packetizer.IsFinished())
		{
			if(m_Output->StreamFinished())
			{
				if(!m_bFinishNotify)
				{
					m_bFinishNotify = true;
					m_bIsFinished = true;
					tuniacApp.CoreAudioMessage(NOTIFY_PLAYBACKFINISHED, NULL);
				}
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
	fVolumeChange =  1.0f / ((float)ulMS * ((float)m_Output->GetSampleRate() / 1000.0f) );
	fVolume = 0.0f;
	return true;
}

bool			CAudioStream::FadeOut(unsigned long ulMS)
{
	//CAutoLock t(&m_Lock);

	m_FadeState = FADE_FADEOUT;
	fVolumeChange =  (-1.0f / ((float)ulMS * ((float)m_Output->GetSampleRate() / 1000.0f)  )   );
	fVolume  = 1.0f;
	return true;
}

bool			CAudioStream::Start(void)
{
	tuniacApp.CoreAudioMessage(NOTIFY_PLAYBACKSTARTED, NULL);
	m_PlayState = STATE_PLAYING;
	return m_Output->Start();
}

bool			CAudioStream::Stop(void)
{
	m_PlayState = STATE_STOPPED;
	return m_Output->Stop();
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
	unsigned long MSOutput = (m_Output->GetSamplesOut() / (m_Output->GetSampleRate()/1000));

	return(MSOutput + m_ulLastSeekMS);
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

	bool bReturn = false;

	m_Output->Stop();
	if(m_pSource->SetPosition(&Pos))
	{
		m_Packetizer.Reset();
		m_Output->Reset();
		m_ulLastSeekMS = Pos;

		bReturn =  true;
	}

	if(m_PlayState == STATE_PLAYING)
		m_Output->Start();

	return bReturn;
}

bool			CAudioStream::GetVisData(float * ToHere, unsigned long ulNumSamples)
{
	if(m_Output == NULL)
		return false;

	return m_Output->GetVisData(ToHere, ulNumSamples);
}
