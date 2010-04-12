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

#pragma once

#include "AudioOutput.h"

#include "IAudioSource.h"
#include "AudioPacketizer.h"

#include "IPlaylist.h"

#include "CriticalSection.h"

#define FADE_NONE		0
#define FADE_FADEIN		1
#define FADE_FADEOUT	2

	typedef struct
	{
	  // Filter #1 (Low band)

	  float  lf;       // Frequency
	  float  f1p0;     // Poles ...
	  float  f1p1;     
	  float  f1p2;
	  float  f1p3;

	  // Filter #2 (High band)

	  float  hf;       // Frequency
	  float  f2p0;     // Poles ...
	  float  f2p1;
	  float  f2p2;
	  float  f2p3;

	  // Sample history buffer

	  float  sdm1;     // Sample data minus 1
	  float  sdm2;     //                   2
	  float  sdm3;     //                   3

	  // Gain Controls

	  float  lg;       // low  gain
	  float  mg;       // mid  gain
	  float  hg;       // high gain

	} EQSTATE;  


class CAudioStream : public IAudioCallback
{
protected:
	bool			m_bServiceThreadRun;

	static DWORD WINAPI serviceThreadStub(void * pData);
	DWORD serviceThread(void);
	int ServiceStream(void);			// new so audio playback thread can produce a new buffer while its not doing anything else!!!


private:
	~CAudioStream(void);
	bool Shutdown(void);

	
public:
	IAudioSource		*				m_pSource;
	CAudioOutput		*				m_Output;
	LPTSTR								szURL;

	CAudioPacketizer					m_Packetizer;

	CCriticalSection					m_Lock;

	unsigned long						m_ulLastSeekMS;

	float								fAmpGain;

	float								fReplayGainTrack;
	float								fReplayGainAlbum;

	bool								bTrackHasGain;
	bool								bAlbumHasGain;

	bool								bReplayGain;
	bool								bUseAlbumGain;

	float								fVolumeScale;			//0.0 - 1.0 affects total volume output!
	float								fVolume;
	float								fVolumeChange;

	int									m_PlayState;
	int									m_FadeState;

	bool								m_bIsFinished;
	bool								m_bEntryPlayed;
	bool								m_bMixNotify;
	bool								m_bFinishNotify;
	bool								m_bIsSeeking;

	unsigned long						m_Channels;

	unsigned long						m_ulCrossfadeTimeMS;
	
	HANDLE								m_hServiceThread;
	
	float 				*				pBuffer;
 	unsigned long						ulNumSamples;

	[EQSTATE							m_EQS[2];
	

public:
	CAudioStream();

	bool Initialize(IAudioSource * pSource, CAudioOutput * pOutput, LPTSTR szSource);

	void Destroy();

public:
	int GetBuffer(float * pAudioBuffer, unsigned long NumSamples);

public:

	unsigned long	GetLength(void);
	unsigned long	GetPosition(void);
	bool			SetPosition(unsigned long MS);

	unsigned long	GetState(void);
	unsigned long	GetFadeState(void);

	bool			SetVolumeScale(float scale);
	bool			SetAmpGain(float scale);

	void			EnableReplayGain(bool bEnable = true);
	bool			SetReplayGainScale(float trackscale, float albumscale);
	void			UseAlbumGain(bool bUse);

	bool			IsFinished(void);

	bool			FadeIn(unsigned long ulMS);
	bool			FadeOut(unsigned long ulMS);

	bool			Start(void);
	bool			Stop(void);

	unsigned long	GetVisData(float * ToHere, unsigned long ulNumSamples);

	void			SetCrossfadePoint(unsigned long ulCrossfadeTimeMS) { m_ulCrossfadeTimeMS = ulCrossfadeTimeMS; }
};
