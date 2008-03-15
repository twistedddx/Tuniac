#pragma once

#include "AudioOutput.h"

#include "IAudioSource.h"
#include "AudioPacketizer.h"

#include "IPlaylist.h"

#include "CriticalSection.h"

#define FADE_NONE		0
#define FADE_FADEIN		1
#define FADE_FADEOUT	2

class CAudioStream : public IAudioCallback
{
private:
	~CAudioStream(void);
	bool Shutdown(void);

	
public:
	IAudioSource		*				m_pSource;
	CAudioOutput		*				m_Output;


	CAudioPacketizer					m_Packetizer;

	CCriticalSection					m_Lock;

	unsigned long						m_SamplesOut;
	unsigned long						m_ulLastSeekMS;

	float								fVolumeScale;			//0.0 - 1.0 affects total volume output!
	float								fVolume;
	float								fVolumeChange;

	unsigned long						m_PlayState;
	unsigned long						m_FadeState;

	bool								m_bIsFinished;
	bool								m_bMixNotify;
	bool								m_bFinishNotify;

	bool								m_bEntryPlayed;

	unsigned long						m_Channels;

	unsigned long						m_CrossfadeTimeMS;

public:
	CAudioStream();

	bool Initialize(IAudioSource * pSource, CAudioOutput * pOutput);

	void Destroy();

public:
	bool GetBuffer(float * pAudioBuffer, unsigned long NumSamples);
	bool ServiceStream(void);			// new so audio playback thread can produce a new buffer while its not doing anything else!!!

public:

	unsigned long	GetLength(void);
	unsigned long	GetPosition(void);
	bool			SetPosition(unsigned long MS);

	unsigned long	GetState(void);
	unsigned long	GetFadeState(void);

	bool			SetVolumeScale(float scale);

	bool			IsFinished(void);

	bool			FadeIn(unsigned long ulMS);
	bool			FadeOut(unsigned long ulMS);

	bool			Start(void);
	bool			Stop(void);

	bool			GetVisData(float * ToHere, unsigned long ulNumSamples);

	void			SetCrossfadePoint(unsigned long ulCrossfadeTimeMS) { m_CrossfadeTimeMS = ulCrossfadeTimeMS; }
};
