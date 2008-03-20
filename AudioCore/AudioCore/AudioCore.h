#pragma once

#include "IAudioCore.h"
#include "XAudioOutputProducer.h"

class CAudioCore :
	public IAudioCore
{
protected:
	CXAudioOutputProducer	m_AudioProducer;

public:
	CAudioCore(void);
	~CAudioCore(void);

public:
	void				Destroy(void);

	bool				Startup(void);
	bool				Shutdown(void);

	bool				Reset(void);

	AUDIOSTREAMID		RenderAudioStream(wchar_t	*	pwsFileName);
	AUDIOSTREAMID		CreateAudioStream(void * pVoid);

	bool				DestroyAudioStream(AUDIOSTREAMID	streamID);

	IAudioStream *		LockAudioStream(AUDIOSTREAMID	streamID);
	bool				UnlockAudioStream(IAudioStream * pAudioStream);

	bool				EnumerateAudioStreams(IAudioCoreStreamEnumerator * pEnumerator);

	void				SetGlobalVolume(float fPercent);
	float				GetGlobalVolume(void);
};
