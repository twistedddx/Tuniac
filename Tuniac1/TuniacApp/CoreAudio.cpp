#define INITGUID

#include "stdafx.h"
#include "CoreAudio.h"
#include "TuniacApp.h"
#include "resource.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

// TODO: Derive this from XAudioEngineCallback to let us know if we need to restart XAudio :(
// TODO: Change AudioStream to have Initialize and Shutdown so we can return audio errors back up the chain!

CCoreAudio::CCoreAudio(void) : 
	m_CrossfadeTimeMS(6000),
	m_BufferSizeMS(250),
	m_fVolume(100.0f)
{
}

CCoreAudio::~CCoreAudio(void)
{
}

bool			CCoreAudio::Startup()
{
	//////////////////////////////////////////////////////////////////////////
	//
	// FIRST INITIALIZE XAUDIO
	//
	if(FAILED(XAudio2Create(&m_pXAudio)))
	{
		//booo we dont have an XAudio2 object!
		MessageBox(NULL, TEXT("Failed initializing XAudio"), TEXT("Fatal Error"), MB_OK | MB_ICONERROR);
		m_pXAudio = NULL;
		return false;
	}

	m_pXAudio->StartEngine();
	m_pXAudio->RegisterForCallbacks(this);

	HRESULT hr;

    if ( FAILED(hr = m_pXAudio->CreateMasteringVoice( &m_pMasteringVoice ) ) )
    {
		MessageBox(NULL, TEXT("Failed creating mastering voice"), TEXT("Fatal Error"), MB_OK | MB_ICONERROR);
        return false;
    }



	WIN32_FIND_DATA		w32fd;
	HANDLE				hFind;
	TCHAR				szFilename[512];

	GetModuleFileName(NULL, szFilename, 512);
	PathRemoveFileSpec(szFilename);
	PathAddBackslash(szFilename);
	StrCat(szFilename, TEXT("*.dll"));

	hFind = FindFirstFile( szFilename, &w32fd); 
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if(StrCmp(w32fd.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0 )
				continue;

			TCHAR temp[512];

			GetModuleFileName(NULL, temp, 512);
			PathRemoveFileSpec(temp);
			PathAddBackslash(temp);
			StrCat(temp, w32fd.cFileName);

			HINSTANCE hDLL = LoadLibrary(temp);
			if(hDLL)
			{
				CreateAudioSourceSupplierFunc pCIMPF;

				pCIMPF = (CreateAudioSourceSupplierFunc)GetProcAddress(hDLL, "CreateDecoderPlugin");
				if(pCIMPF)
				{
					IAudioSourceSupplier * pPlugin = pCIMPF();

					if(pPlugin)
					{
						pPlugin->SetHelper(this);
						m_AudioSources.AddTail(pPlugin);
					}
					else
					{
						FreeLibrary(hDLL);
					}
				}
				else
				{
					FreeLibrary(hDLL);
				}
			}

		} while(FindNextFile( hFind, &w32fd));

		FindClose(hFind); 
	}

	return true;
}

bool			CCoreAudio::Shutdown(void)
{
	Reset();

	while(m_AudioSources.GetCount())
	{
		m_AudioSources[0]->Destroy();
		m_AudioSources.RemoveAt(0);
	}


    // All XAudio2 interfaces are released when the engine is destroyed, but being tidy
	if(m_pMasteringVoice)
	{
	    m_pMasteringVoice->DestroyVoice();
		m_pMasteringVoice = NULL;
	}

	if(m_pXAudio)
	{
		m_pXAudio->UnregisterForCallbacks(this);
		m_pXAudio->StopEngine();
		SAFE_RELEASE(m_pXAudio);
	}

	return true;
}

// TODO: Make core audio just take in a filename, we shouldn't be reliant on IPlaylistEntries for any of this!!!!!
bool			CCoreAudio::TransitionTo(IPlaylistEntry * pEntry)
{
	{
		CAutoLock	t(&m_Lock);
		CheckOldStreams();
	}
	
	LPTSTR szSource = (LPTSTR)pEntry->GetField(FIELD_URL);

	for(unsigned long x=0; x<m_AudioSources.GetCount(); x++)
	{

		if(m_AudioSources[x]->CanHandle(szSource))
		{
			CAudioStream * pStream;
			CAudioOutput * pOutput;
			IAudioSource * pSource = m_AudioSources[x]->CreateAudioSource(szSource);
			if(pSource)
			{
				pStream = new CAudioStream();
				pOutput = new CAudioOutput(m_pXAudio, m_BufferSizeMS);

				if(pStream->Initialize(pSource, pOutput))
				{
					CAutoLock	t(&m_Lock);

					bool bShoudStart = false;
					if(m_Streams.GetCount())
					{
						for(int ttt=0; ttt<m_Streams.GetCount(); ttt++)
						{
							m_Streams[ttt]->FadeOut(m_CrossfadeTimeMS);
						}
						pStream->FadeIn(m_CrossfadeTimeMS);
						bShoudStart = true;
					}

					float scale = m_fVolume / 100.0f;

					pStream->SetVolumeScale(scale);
					pStream->SetCrossfadePoint(m_CrossfadeTimeMS);
					m_Streams.AddTail(pStream);

					if(bShoudStart)
						pStream->Start();
					return true;
				}
				else
				{
					pStream->Destroy();
					break;
				}
			}
		}
	}

	return false;
}

bool			CCoreAudio::SetSource(IPlaylistEntry * pEntry)
{
	Reset();
	if(pEntry)
		return(TransitionTo(pEntry));

	return true;
}

bool			CCoreAudio::Reset(void)
{
	CAutoLock	t(&m_Lock);

	while(m_Streams.GetCount())
	{
		m_Streams[0]->Destroy();
		m_Streams.RemoveAt(0);
	}
	return true;
}

bool			CCoreAudio::Play(void)
{
	CAutoLock	t(&m_Lock);

	CheckOldStreams();

	if(m_Streams.GetCount() == 0)
		return false;

	for(unsigned long x=0; x<m_Streams.GetCount(); x++)
	{
		m_Streams[x]->Start();
	}
	return true;
}

bool			CCoreAudio::Stop(void)
{
	CAutoLock	t(&m_Lock);

	CheckOldStreams();

	for(unsigned long x=0; x<m_Streams.GetCount(); x++)
	{
		m_Streams[x]->Stop();
	}

	return true;
}

//only stop the last stream. Useful for softpause feature
bool			CCoreAudio::StopLast(void)
{
	CAutoLock	t(&m_Lock);

	CheckOldStreams();

	m_Streams[m_Streams.GetCount() - 1]->Stop();

	return true;
}

unsigned long	CCoreAudio::GetState(void)
{
	CAutoLock	t(&m_Lock);

	if(m_Streams.GetCount())
	{
		return(m_Streams[m_Streams.GetCount()-1]->GetState());
	}

	return 0;
}

bool			CCoreAudio::SetPosition(unsigned long MS)
{
	CAutoLock	t(&m_Lock);

	for(unsigned long x=0; x<m_Streams.GetCount(); x++)
	{
		if(m_Streams[x]->GetFadeState() == FADE_FADEOUT)
		{
			m_Streams[x]->Destroy();
			m_Streams.RemoveAt(x);

			x-=1;
		}
	}

	CheckOldStreams();

	if(m_Streams.GetCount())
	{
		return(m_Streams[m_Streams.GetCount()-1]->SetPosition(MS));
	}

	return false;
}

unsigned long	CCoreAudio::GetPosition(void)
{
	CAutoLock	t(&m_Lock);

	if(m_Streams.GetCount())
	{
		return(m_Streams[m_Streams.GetCount()-1]->GetPosition());
	}

	return 0;
}

unsigned long	CCoreAudio::GetLength(void)
{
	CAutoLock	t(&m_Lock);

	if(m_Streams.GetCount())
	{
		return(m_Streams[m_Streams.GetCount()-1]->GetLength());
	}

	return LENGTH_UNKNOWN;
}

unsigned long CCoreAudio::GetChannels(void)
{
	CAutoLock	t(&m_Lock);

	if(m_Streams.GetCount())
	{
		return(m_Streams[m_Streams.GetCount()-1]->m_Output->GetChannels());
	}

	return -1;
}

unsigned long CCoreAudio::GetSampleRate(void)
{
	CAutoLock	t(&m_Lock);

	if(m_Streams.GetCount())
	{
		return(m_Streams[m_Streams.GetCount()-1]->m_Output->GetSampleRate());
	}

	return -1;
}


bool			CCoreAudio::GetVisData(float * ToHere, unsigned long ulNumSamples)
{
	CAutoLock	t(&m_Lock);

	if(m_Streams.GetCount() == 0)
	{
		return false;
	}
	else if(m_Streams.GetCount() == 1)
	{
		return(m_Streams[0]->GetVisData(ToHere, ulNumSamples));
	}
	else
	{
		// multiple streams... MIX THEM!!!
		float TBuffer[4096];

		ZeroMemory(ToHere, ulNumSamples * sizeof(float));

		for(unsigned long x=0; x<m_Streams.GetCount(); x++)
		{
			if(m_Streams[x]->GetVisData(TBuffer, ulNumSamples))
			{
				for(unsigned long samp = 0; samp < ulNumSamples; samp++)
				{
					ToHere[samp] += TBuffer[samp] / m_Streams.GetCount();
				}
			}
		}

		return true;
	}

	return false;
}


unsigned long CCoreAudio::GetNumPlugins(void)
{
	return m_AudioSources.GetCount();
}

IAudioSourceSupplier * CCoreAudio::GetPluginAtIndex(unsigned long ulIndex)
{
	if(ulIndex < m_AudioSources.GetCount())
	{
		return m_AudioSources[ulIndex];
	}

	return NULL;
}


void CCoreAudio::CheckOldStreams(void)
{
	for(unsigned long x=0; x<m_Streams.GetCount(); x++)
	{
		if(m_Streams[x]->IsFinished())
		{
			m_Streams[x]->Destroy();
			m_Streams.RemoveAt(x);

			x-=1;
		}	
	}
}


float CCoreAudio::GetVolumePercent()
{
	return m_fVolume;
}


void CCoreAudio::SetVolumePercent(float fVolume)
{
	if(fVolume < 0.0)
	{
		fVolume = 0.0;
	}
	if(fVolume > 100.0)
	{
		fVolume = 100.0;
	}

	m_fVolume = fVolume;

	for(unsigned long x=0; x<m_Streams.GetCount(); x++)
	{
		m_Streams[x]->SetVolumeScale(m_fVolume / 100.0f);
	}
}

void	CCoreAudio::UpdateStreamTitle(IAudioSource * pSource, LPTSTR szTitle, unsigned long ulFieldID)
{
	if(pSource == NULL)
		return;
	
	for(unsigned long i = 0; i < m_Streams.GetCount(); i++)
	{
		/*
		if(m_Streams[0]->m_pSource == pSource)
		{
			m_Streams[0]->m_pEntry->SetField(ulFieldID, szTitle);
			tuniacApp.m_SourceSelectorWindow->UpdateView();
			tuniacApp.m_PluginManager.PostMessage(PLUGINNOTIFY_SONGINFOCHANGE, NULL, NULL);
			break;
		}
		*/
	}
}

void CCoreAudio::LogConsoleMessage(LPTSTR szModuleName, LPTSTR szMessage)
{
}

void CCoreAudio::SetCrossfadeTime(unsigned long ulMS) 
{ 
	m_CrossfadeTimeMS = ulMS; 

	for(unsigned long x=0; x<m_Streams.GetCount(); x++)
	{
		m_Streams[x]->SetCrossfadePoint(ulMS);
	}
}



void CCoreAudio::OnCriticalError(HRESULT Error)
{
	switch(Error)
	{
		case XAUDIO2_E_XMA_DECODER_ERROR:
			{
				int x=0;
			}
			break;

		default:
		case XAUDIO2_E_DEVICE_INVALIDATED:
			{
				tuniacApp.CoreAudioMessage(NOTIFY_COREAUDIORESET, 0);
			}
			break;


	}
}
