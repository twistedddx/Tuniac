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

#include "IAudioSource.h"
#include "AudioStream.h"

#include "IPlaylist.h"

#include "Singleton.h"

#define NOTIFY_COREAUDIO_MIXPOINTREACHED	0
#define NOTIFY_COREAUDIO_PLAYBACKFINISHED	1
#define NOTIFY_COREAUDIO_PLAYBACKSTARTED	2
#define NOTIFY_COREAUDIO_RESET				3
#define NOTIFY_COREAUDIO_TRANSITIONTO		4
#define NOTIFY_COREAUDIO_PLAYBACKFAILED		5

class CCoreAudio : 
	public CSingleton<CCoreAudio>,
	public IAudioSourceHelper,
	public IXAudio2EngineCallback
{
protected:

	typedef struct
	{
		HINSTANCE						hDLL;
		IAudioSourceSupplier *			pAudioSourceSupplier;
	} AudioSourceSupplierEntry;

	Array<CAudioStream *, 2>			m_Streams;
	Array<IAudioSourceSupplier *, 2>	m_AudioSources;

	CCriticalSection					m_Lock;

	IXAudio2				*			m_pXAudio;
    IXAudio2MasteringVoice	*			m_pMasteringVoice;

	unsigned long						m_CrossfadeTimeMS;
	unsigned long						m_BufferSizeMS;

	float								m_fVolume;
	float								m_fAmpGain;
	bool								m_bReplayGainEnabled;
	bool								m_bUseAlbumGain;

public:
	CCoreAudio(void);
	~CCoreAudio(void);

	bool				Startup(void);
	bool				Shutdown(void);
	
	bool				TransitionTo(IPlaylistEntry * pEntry);
	bool				SetSource(IPlaylistEntry * pEntry);

	bool				Reset(void);
	bool				Play(void);
	bool				Stop(void);
	bool				StopLast(void);

	unsigned long		GetState(void);

	bool				SetPosition(unsigned long MS);
	unsigned long		GetPosition(void);

	unsigned long		GetLength(void);

	unsigned long		GetChannels(void);
	unsigned long		GetSampleRate(void);

	bool				GetVisData(float * ToHere, unsigned long ulNumSamples);

	unsigned long		GetNumPlugins(void);
	IAudioSourceSupplier * GetPluginAtIndex(unsigned long ulIndex);

	void				CheckOldStreams(void);

	float				GetVolumePercent();
	void				SetVolumePercent(float fPercent);

	float				GetAmpGain();
	void				SetAmpGain(float fPercent);

	void				EnableReplayGain(bool bEnable);
	void				ReplayGainUseAlbumGain(bool bAlbumGain);

	void				SetCrossfadeTime(unsigned long ulMS);
	void				SetAudioBufferSize(unsigned long ulMS) { m_BufferSizeMS = ulMS; }
	
// IAudioSourceHelper inherited methods
public:
	void				UpdateStreamTitle(LPTSTR m_URL, LPTSTR szTitle, unsigned long ulFieldID);
	void				LogConsoleMessage(LPTSTR szModuleName, LPTSTR szMessage);

//IXAudio2EngineCallback inherited methods
public:
	// Called by XAudio2 just before an audio processing pass begins.
	STDMETHOD_(void, OnProcessingPassStart) (THIS) { };

    // Called just after an audio processing pass ends.
	STDMETHOD_(void, OnProcessingPassEnd) (THIS) { };

    // Called in the event of a critical system error which requires XAudio2
    // to be closed down and restarted.  The error code is given in Error.
    STDMETHOD_(void, OnCriticalError) (THIS_ HRESULT Error);
};