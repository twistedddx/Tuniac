#pragma once

#include "IAudioSource.h"
#include "AudioStream.h"

#include "IPlaylist.h"

#include "Singleton.h"

#define NOTIFY_MIXPOINTREACHED			0
#define NOTIFY_PLAYBACKFINISHED			1
#define NOTIFY_PLAYBACKSTARTED			2

class CCoreAudio : 
	public CSingleton<CCoreAudio>,
	public IAudioSourceHelper
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
	void				SetVolumeScale(float scale);

public:
	void				UpdateStreamTitle(IAudioSource * pSource, LPTSTR szTitle, unsigned long ulFieldID);
	void				LogConsoleMessage(LPTSTR szModuleName, LPTSTR szMessage);
};