#pragma once

#include "IAudioSource.h"
#include "AudioStream.h"

#include "IPlaylist.h"

#include "Singleton.h"

#define NOTIFY_MIXPOINTREACHED			0
#define NOTIFY_PLAYBACKFINISHED			1
#define NOTIFY_PLAYBACKSTARTED			2
#define NOTIFY_COREAUDIORESET			3

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

	void				SetCrossfadeTime(unsigned long ulMS);
	void				SetAudioBufferSize(unsigned long ulMS) { m_BufferSizeMS = ulMS; }
	
// IAudioSourceHelper inherited methods
public:
	void				UpdateStreamTitle(IAudioSource * pSource, LPTSTR szTitle, unsigned long ulFieldID);
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