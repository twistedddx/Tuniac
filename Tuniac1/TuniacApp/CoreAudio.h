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
/*
	Modification and addition to Tuniac originally written by Tony Million
	Copyright (C) 2003-2012 Brett Hoyle
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
#define NOTIFY_COREAUDIO_PLAYBACKFAILED		4
#define NOTIFY_COREAUDIO_PLAYEDPOINTREACHED	5
class CCoreAudio : 
	public CSingleton<CCoreAudio>,
	public IAudioSourceHelper,
	public IXAudio2EngineCallback,
	public IMMNotificationClient
{
protected:

	LONG								_cRef;
	IMMDeviceEnumerator		*			m_pEnumerator;

	Array<CAudioStream *, 2>			m_Streams;
	Array<IAudioSourceSupplier *, 2>	m_AudioSources;

	CCriticalSection					m_Lock;

	IXAudio2				*			m_pXAudio;
    IXAudio2MasteringVoice	*			m_pMasteringVoice;

	unsigned long						m_ulCrossfadeTimeMS;
	unsigned long						m_BufferSizeMS;

	int									m_iVolPercent;

	bool								m_bEQEnabled;
	float								m_fEQLow;
	float								m_fEQMid;
	float								m_fEQHigh;

	float								m_fAmpGain;

	float					*			fMixBuffer;
	bool								m_bReplayGainEnabled;
	bool								m_bUseAlbumGain;

public:
	CCoreAudio(void);
	~CCoreAudio(void);

	bool				Startup(void);
	bool				Shutdown(void);
	
	bool				SetSource(LPTSTR szSource, float *fReplayGainAlbum, float *fReplayGainTrack, bool bResetAudio);

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

	unsigned long		GetVisData(float * ToHere, unsigned long ulNumSamples);

	unsigned long		GetNumPlugins(void);
	IAudioSourceSupplier * GetPluginAtIndex(unsigned long ulIndex);

	void				CheckOldStreams(void);

	int					GetVolumePercent();
	void				SetVolumePercent(int iVolPercent);

	void				EnableEQ(bool bEnabled);
	float				GetEQLowGain(void);
	float				GetEQMidGain(void);
	float				GetEQHighGain(void);
	void				SetEQGain(float fEQLow, float fEQMid, float fEQHigh);
	float				GetAmpGain();
	void				SetAmpGain(float fPercent);

	void				EnableReplayGain(bool bEnable);
	void				ReplayGainUseAlbumGain(bool bAlbumGain);

	void				SetCrossfadeTime(unsigned long ulMS);
	void				SetAudioBufferSize(unsigned long ulMS) { m_BufferSizeMS = ulMS; }
	
// IAudioSourceHelper inherited methods
public:
	void				UpdateMetaData(LPTSTR m_URL, void * pNewData, unsigned long ulFieldID);
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

//IMMNotificationClient inherited methods
public:
	STDMETHOD_(HRESULT, QueryInterface) (THIS_ REFIID riid, __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject) { return S_OK; };
	STDMETHOD_(ULONG, AddRef) (THIS_ void) { return InterlockedIncrement(&_cRef); };
	STDMETHOD_(ULONG, Release) (THIS_ void) {
										ULONG ulRef = InterlockedDecrement(&_cRef);
										if (0 == ulRef)
										{
											delete this;
										}
										return ulRef;
									};

	STDMETHOD_(HRESULT, OnDeviceStateChanged) (THIS_ LPCWSTR pwstrDeviceId, DWORD dwNewState) { return S_OK; };
	STDMETHOD_(HRESULT, OnDeviceAdded) (THIS_ LPCWSTR pwstrDeviceId) { return S_OK; };
	STDMETHOD_(HRESULT, OnDeviceRemoved) (THIS_ LPCWSTR pwstrDeviceId) { return S_OK; };
	STDMETHOD_(HRESULT, OnPropertyValueChanged) (THIS_ LPCWSTR pwstrDeviceId, const PROPERTYKEY key) { return S_OK; };
	STDMETHOD_(HRESULT, OnDefaultDeviceChanged) (THIS_ EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId);

};