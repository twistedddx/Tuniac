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

#define INITGUID

#include "stdafx.h"
#include "CoreAudio.h"
#include "TuniacApp.h"
#include "resource.h"

#include "TuniacMemoryFileIO.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

// TODO: Derive this from XAudioEngineCallback to let us know if we need to restart XAudio :(
// TODO: Change AudioStream to have Initialize and Shutdown so we can return audio errors back up the chain!

CCoreAudio::CCoreAudio(void) : 
	m_CrossfadeTimeMS(6000),
	m_BufferSizeMS(250),
	m_fVolume(100.0f),
	m_fAmpGain(-6.0f),
	m_pXAudio(NULL),
	m_pMasteringVoice(NULL),
	m_bReplayGainEnabled(true),
	m_bUseAlbumGain(false)

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
		if (IDYES == MessageBox(NULL, TEXT("Failed initializing XAudio 2.2.\nThis is most likely because you have not installed DirectX August 2008 redist which is required.\n Simply reinstall Tuniac to fix or do you want to be directed to the Microsoft download?"), TEXT("Fatal Error"), MB_YESNO | MB_ICONERROR))
		{
			ShellExecute(NULL, NULL, TEXT("http://download.microsoft.com/download/5/c/8/5c8b7216-bbc2-4215-8aa5-9dfef9cdb3df/directx_aug2008_redist.exe"), NULL, NULL, SW_SHOW);
		}
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
	float *fReplayGainAlbum = (float *)pEntry->GetField(FIELD_REPLAYGAIN_ALBUM_GAIN);
	float *fReplayGainTrack = (float *)pEntry->GetField(FIELD_REPLAYGAIN_TRACK_GAIN);

	for(unsigned long x=0; x<m_AudioSources.GetCount(); x++)
	{

		if(m_AudioSources[x]->CanHandle(szSource))
		{
			CAudioStream * pStream;
			CAudioOutput * pOutput;

			IAudioFileIO * pFileIO = NULL;

			if(m_AudioSources[x]->GetFlags() & FLAGS_PROVIDEFILEIO)
			{
				CTuniacMemoryFileIO * pIO = new CTuniacMemoryFileIO();

				if(pIO->Open(szSource))
					pFileIO = pIO;


				if(!pFileIO)
					break;
			}

			IAudioSource * pSource = m_AudioSources[x]->CreateAudioSource(szSource, pFileIO);
			if(pSource)
			{
				pStream = new CAudioStream();
				pOutput = new CAudioOutput(m_pXAudio, m_BufferSizeMS);

				if(pStream->Initialize(pSource, pOutput, szSource))
				{
					CAutoLock	t(&m_Lock);

					bool bShoudStart = false;
					for(int ttt=0; ttt<m_Streams.GetCount(); ttt++)
					{
						m_Streams[ttt]->FadeOut(m_CrossfadeTimeMS);
					pStream->FadeIn(m_CrossfadeTimeMS);
					bShoudStart = true;
					}

					pStream->SetVolumeScale(m_fVolume);
					pStream->SetAmpGain(m_fAmpGain);
					pStream->SetCrossfadePoint(m_CrossfadeTimeMS);

					pStream->EnableReplayGain(m_bReplayGainEnabled);
					pStream->UseAlbumGain(m_bUseAlbumGain);
					pStream->SetReplayGainScale(*fReplayGainTrack, *fReplayGainAlbum);

					m_Streams.AddTail(pStream);

					if(bShoudStart)
						pStream->Start();

					tuniacApp.CoreAudioMessage(NOTIFY_COREAUDIO_TRANSITIONTO, NULL);

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
	tuniacApp.CoreAudioMessage(NOTIFY_COREAUDIO_PLAYBACKFAILED, NULL);

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
/*
void CCoreAudio::SetReplayGain(float fReplayGain)
{
	bool bReplayGain = false;
	if(fReplayGain != 0.0f)
		bReplayGain = true;


	if(m_Streams.GetCount())
		m_Streams[m_Streams.GetCount()-1]->SetReplayGainScale(fReplayGainScale, bReplayGain);
}
*/

void CCoreAudio::SetVolumePercent(float fVolume)
{
	m_fVolume = fVolume;

	for(unsigned long x=0; x<m_Streams.GetCount(); x++)
	{
		m_Streams[x]->SetVolumeScale(m_fVolume);
	}
}

float CCoreAudio::GetAmpGain()
{
	return m_fAmpGain;
}

void CCoreAudio::SetAmpGain(float fAmpGain)
{
	m_fAmpGain = fAmpGain;

	for(unsigned long x=0; x<m_Streams.GetCount(); x++)
	{
		m_Streams[x]->SetAmpGain(m_fAmpGain);
	}
}

void CCoreAudio::EnableReplayGain(bool bEnable)
{
	m_bReplayGainEnabled = bEnable;
	for(unsigned long x=0; x<m_Streams.GetCount(); x++)
	{
		m_Streams[x]->EnableReplayGain(m_bReplayGainEnabled);
	}
}

void CCoreAudio::ReplayGainUseAlbumGain(bool bAlbumGain)
{
	m_bUseAlbumGain = bAlbumGain;

	for(unsigned long x=0; x<m_Streams.GetCount(); x++)
	{
		m_Streams[x]->UseAlbumGain(m_bUseAlbumGain);
	}
}


void	CCoreAudio::UpdateStreamTitle(LPTSTR szSource, LPTSTR szTitle, unsigned long ulFieldID)
{
	tuniacApp.UpdateStreamTitle(szSource, szTitle, ulFieldID);
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
				tuniacApp.CoreAudioMessage(NOTIFY_COREAUDIO_RESET, 0);
			}
			break;


	}
}
