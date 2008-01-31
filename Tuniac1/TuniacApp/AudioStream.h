#pragma once

#include "AudioOutput.h"
//#include "DirectSoundOutput.h"

#include "IAudioSource.h"
#include "AudioPacketizer.h"

#include "IPlaylist.h"

#include "CriticalSection.h"

#define FADE_NONE		0
#define FADE_FADEIN		1
#define FADE_FADEOUT	2

class CAudioStream : public IAudioCallback
{
public:
	IAudioSource		*				m_pSource;

	CAudioOutput						m_Output;
	//CDirectSoundOutput					m_Output;
	CAudioPacketizer					m_Packetizer;

	CCriticalSection					m_Lock;

	unsigned long						m_SamplesOut;

	float								fVolumeScale;			//0.0 - 1.0 affects total volume output!
	float								fVolume;
	float								fVolumeChange;

	unsigned long						m_PlayState;
	unsigned long						m_FadeState;

	bool								m_bIsFinished;
	bool								m_bMixNotify;
	bool								m_bFinishNotify;

	IPlaylistEntry				*		m_pEntry;
	bool								m_bEntryPlayed;

	unsigned long						m_Channels;

public:
	CAudioStream(IAudioSource * pSource, IPlaylistEntry * pEntry);
	~CAudioStream(void);

public:
	bool GetBuffer(float * pAudioBuffer, unsigned long NumSamples);

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
};
