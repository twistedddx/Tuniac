#include "StdAfx.h"
#include "AudioCore.h"

#include <map>

std::map<AUDIOSTREAMID, void*>		m_thingMap;

CAudioCore::CAudioCore(void)
{
}

CAudioCore::~CAudioCore(void)
{
}

void				CAudioCore::Destroy(void)
{
}

bool				CAudioCore::Startup(void)
{
	if(!m_AudioProducer.Initialize())
		return false;

	return true;
}

bool				CAudioCore::Shutdown(void)
{
	m_AudioProducer.Shutdown();
	return true;
}


bool				CAudioCore::Reset(void)
{
	return false;
}


AUDIOSTREAMID		CAudioCore::RenderAudioStream(wchar_t	*	pwsFileName)
{
	IAudioOutput * pOutput = m_AudioProducer.CreateAudioOutput();
	return 0;
}

AUDIOSTREAMID		CAudioCore::CreateAudioStream(void * pVoid)
{
	return 0;
}


bool				CAudioCore::DestroyAudioStream(AUDIOSTREAMID	streamID)
{
	return false;
}


IAudioStream *		CAudioCore::LockAudioStream(AUDIOSTREAMID	streamID)
{
	return NULL;
}

bool				CAudioCore::UnlockAudioStream(IAudioStream * pAudioStream)
{
	return false;
}


bool				CAudioCore::EnumerateAudioStreams(IAudioCoreStreamEnumerator * pEnumerator)
{
	return false;
}


void				CAudioCore::SetGlobalVolume(float fPercent)
{
}

float				CAudioCore::GetGlobalVolume(void)
{
	return 100.0;
}

