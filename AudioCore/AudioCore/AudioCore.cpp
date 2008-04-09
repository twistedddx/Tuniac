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
	m_AudioProducer = new CXAudioOutputProducer();
	if(!m_AudioProducer->Initialize())
		return false;

	return true;
}

bool				CAudioCore::Shutdown(void)
{
	FreeAudioSources();
	m_AudioProducer->Shutdown();
	m_AudioProducer->Destroy();
	return true;
}


bool				CAudioCore::Reset(void)
{
	return false;
}

bool				CAudioCore::LoadAudioSource(wchar_t		*	wcsPluginName)
{
	for(int x=0; x<m_vAudioSources.size(); x++)
	{
		if(_wcsicmp(wcsPluginName, m_vAudioSources[x].filename.c_str()) == 0)
		{
			return true;
		}
	}


	AudioSource			as;

	as.filename = wcsPluginName;
	as.hDLL = LoadLibrary(wcsPluginName);
	if(as.hDLL)
	{
		CreateAudioSourceSupplierFunc		pCASS;

		pCASS = (CreateAudioSourceSupplierFunc)GetProcAddress(as.hDLL, CREATEAUDIOSOURCESUPPLIERNAME);

		if(pCASS)
		{
			as.pSupplier = pCASS();
			if(as.pSupplier)
			{
				m_vAudioSources.push_back(as);
				return true;
			}
		}

		FreeLibrary(as.hDLL);
	}

	return false;
}

bool				CAudioCore::FreeAudioSources(void)
{
	Reset();

	while(m_vAudioSources.size())
	{
		m_vAudioSources[0].pSupplier->Destroy();
		FreeLibrary(m_vAudioSources[0].hDLL);

		m_vAudioSources.erase(m_vAudioSources.begin());
	}

	return true;
}

AUDIOSTREAMID		CAudioCore::RenderAudioStream(wchar_t	*	pwsFileName)
{
	IAudioOutput * pOutput = m_AudioProducer->CreateAudioOutput(44100, 2);
	return 0;
}

AUDIOSTREAMID		CAudioCore::CreateAudioStream(IAudioSource * pSource)
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

