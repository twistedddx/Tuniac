#pragma once

#include "IAudioCore.h"
#include "XAudioOutputProducer.h"

#include <IAudioSource.h>
#include <vector>

class CAudioCore :
	public IAudioCore
{
protected:

	typedef struct _AudioSource_
	{
		IAudioSourceSupplier		*			pSupplier;
		HINSTANCE								hDLL;
		std::wstring							filename;
	} AudioSource;

	std::vector<AudioSource>					m_vAudioSources;
	CXAudioOutputProducer						m_AudioProducer;

public:
	CAudioCore(void);
	~CAudioCore(void);

public:
	void				Destroy(void);

	bool				Startup(void);
	bool				Shutdown(void);

	bool				Reset(void);

	bool				LoadAudioSource(wchar_t		*	wcsPluginName);
	bool				FreeAudioSources(void);

	AUDIOSTREAMID		RenderAudioStream(wchar_t	*	pwsFileName);
	AUDIOSTREAMID		CreateAudioStream(IAudioSource * pSource);

	bool				DestroyAudioStream(AUDIOSTREAMID	streamID);

	IAudioStream *		LockAudioStream(AUDIOSTREAMID	streamID);
	bool				UnlockAudioStream(IAudioStream * pAudioStream);

	bool				EnumerateAudioStreams(IAudioCoreStreamEnumerator * pEnumerator);

	void				SetGlobalVolume(float fPercent);
	float				GetGlobalVolume(void);
};
