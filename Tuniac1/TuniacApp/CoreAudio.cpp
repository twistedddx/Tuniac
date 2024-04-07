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
	Copyright (C) 2003-2014 Brett Hoyle
*/

#define INITGUID

#include "stdafx.h"
#include "CoreAudio.h"
#include "TuniacApp.h"
#include "resource.h"

#include "TuniacMemoryFileIO.h"
#include "TuniacStandardFileIO.h"
#include "TuniacHTTPFileIO.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#ifdef SSE
#include "SSE_Utils.h"
#define ClearFloat(src, num)  SSE_ClearFloat(src, num)
#else
#define ClearFloat(src, num)  ZeroMemory(src, (num) * sizeof(float))
#endif

// TODO: Derive this from XAudioEngineCallback to let us know if we need to restart XAudio :(
// TODO: Change AudioStream to have Initialize and Shutdown so we can return audio errors back up the chain!

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

CCoreAudio::CCoreAudio(void) : 
	m_ulCrossfadeTimeMS(6000),
	m_BufferSizeMS(500),
	m_iVolPercent(100),
	m_fAmpGain(-6.0f),
	m_pXAudio(NULL),
	_cRef(1),
	m_pEnumerator(NULL),
	m_pMasteringVoice(NULL),
	m_bReplayGainEnabled(true),
	m_bUseAlbumGain(false),
	fMixBuffer(NULL)
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

	fMixBuffer = (float*)_aligned_malloc(4096 * sizeof(float), 16);

	if(FAILED(XAudio2Create(&m_pXAudio)))
	{
		//booo we dont have an XAudio2 object!
		if (tuniacApp.m_LogWindow)
		{
			if (tuniacApp.m_LogWindow->GetLogOn())
				tuniacApp.m_LogWindow->LogMessage(TEXT("CoreAudio"), TEXT("Failed initializing XAudio 2.9"));
		}
		MessageBox(NULL, TEXT("Failed initializing XAudio 2.9.\nTry reinstalling Tuniac to fix"), TEXT("Fatal Error"), MB_OK | MB_ICONERROR);
		m_pXAudio = NULL;
		return false;
	}

	m_pXAudio->StartEngine();
	m_pXAudio->RegisterForCallbacks(this);

	if(CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&m_pEnumerator) == S_OK)
	{
		if(m_pEnumerator)
		{
			if(m_pEnumerator->RegisterEndpointNotificationCallback(this) == S_OK)
				m_pEnumerator->AddRef();
		}

	}


	HRESULT hr;

    if ( FAILED(hr = m_pXAudio->CreateMasteringVoice( &m_pMasteringVoice ) ) )
    {
		if (tuniacApp.m_LogWindow)
		{
			if (tuniacApp.m_LogWindow->GetLogOn())
				tuniacApp.m_LogWindow->LogMessage(TEXT("CoreAudio"), TEXT("Failed creating mastering voice."));
		}
		MessageBox(NULL, TEXT("Failed creating mastering voice"), TEXT("Fatal Error"), MB_OK | MB_ICONERROR);
        return false;
    }



	WIN32_FIND_DATA		w32fd;
	HANDLE				hFind;
	TCHAR				szFilename[512];

	GetModuleFileName(NULL, szFilename, 512);
	PathRemoveFileSpec(szFilename);
	PathAddBackslash(szFilename);
	StringCchCat(szFilename, MAX_PATH, TEXT("*.dll"));

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
			StringCchCat(temp, MAX_PATH, w32fd.cFileName);

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
	if(m_pEnumerator)
	{
		m_pEnumerator->UnregisterEndpointNotificationCallback(this);
		SAFE_RELEASE(m_pEnumerator);
	}

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

	if(fMixBuffer)
	{
		_aligned_free(fMixBuffer);
	}

	return true;
}

bool			CCoreAudio::SetSource(LPTSTR szSource, float *fReplayGainAlbum, float *fReplayGainTrack, bool bResetAudio, bool bOutputDisabled)
{
	//XAudio Voice not valid, try to give the audio engine a boot
	if(m_pMasteringVoice == NULL)
	{
		Shutdown();
		if(!Startup())
			return false;
	}

	if(bResetAudio)
		Reset();

	{
		CAutoLock	t(&m_Lock);
		CheckOldStreams();
	}

	for(unsigned long x=0; x<m_AudioSources.GetCount(); x++)
	{
		if(m_AudioSources[x]->CanHandle(szSource))
		{
			CAudioStream * pStream;
			CAudioOutput * pOutput;

			IAudioFileIO * pFileIO = NULL;

			if(m_AudioSources[x]->GetFlags() & FLAGS_PROVIDEMEMORYFILEIO)
			{
				CTuniacMemoryFileIO * pIO = new CTuniacMemoryFileIO();

				if(pIO->Open(szSource))
					pFileIO = pIO;


				if(!pFileIO)
					break;
			}

			if(m_AudioSources[x]->GetFlags() & FLAGS_PROVIDESTANDARDFILEIO)
			{
				CTuniacStandardFileIO * pIO = new CTuniacStandardFileIO();

				if(pIO->Open(szSource))
					pFileIO = pIO;


				if(!pFileIO)
					break;
			}

			if(m_AudioSources[x]->GetFlags() & FLAGS_PROVIDEHTTPFILEIO)
			{
				CTuniacHTTPFileIO * pIO = new CTuniacHTTPFileIO();

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
					if (tuniacApp.m_LogWindow)
					{
						if (tuniacApp.m_LogWindow->GetLogOn())
						{
							TCHAR szMessage[MAX_PATH + 20];
							StringCchPrintf(szMessage, MAX_PATH + 20, TEXT("Set source %s"), szSource);
							tuniacApp.m_LogWindow->LogMessage(TEXT("CoreAudio"), szMessage);
						}
					}

					CAutoLock	t(&m_Lock);

					if(bOutputDisabled)
						pOutput->SetOutputDisabled();

					bool bShoudStart = false;
					for(int ttt=0; ttt<m_Streams.GetCount(); ttt++)
					{
						m_Streams[ttt]->FadeOut(m_ulCrossfadeTimeMS);
						pStream->FadeIn(m_ulCrossfadeTimeMS);
						bShoudStart = true;
					}

					pStream->SetVolumeScale(m_iVolPercent);

					pStream->EnableEQ(m_bEQEnabled);
					pStream->SetEQGain(m_fEQLow, m_fEQMid, m_fEQHigh);
					pStream->SetAmpGain(m_fAmpGain);
					pStream->SetCrossfadePoint(m_ulCrossfadeTimeMS);

					pStream->EnableReplayGain(m_bReplayGainEnabled);
					pStream->UseAlbumGain(m_bUseAlbumGain);
					pStream->SetReplayGainScale(*fReplayGainTrack, *fReplayGainAlbum);

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
	tuniacApp.CoreAudioMessage(NOTIFY_COREAUDIO_PLAYBACKFAILED, NULL);
	return false;
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

	return 0;
}

unsigned long CCoreAudio::GetChannels(void)
{
	CAutoLock	t(&m_Lock);

	if(m_Streams.GetCount())
	{
		return(m_Streams[m_Streams.GetCount()-1]->m_Output->GetChannels());
	}

	return INVALID;
}

unsigned long CCoreAudio::GetSampleRate(void)
{
	CAutoLock	t(&m_Lock);

	if(m_Streams.GetCount())
	{
		return(m_Streams[m_Streams.GetCount()-1]->m_Output->GetSampleRate());
	}

	return INVALID;
}


unsigned long CCoreAudio::GetVisData(float * ToHere, unsigned long ulNumSamples)
{
	CAutoLock	t(&m_Lock);

	ClearFloat(ToHere, ulNumSamples);

	if(m_Streams.GetCount() == 0 || GetState() != STATE_PLAYING)
	{
		return 0;
	}
	else if(m_Streams.GetCount() == 1)
	{
		if(m_Streams[0]->IsFinished())
			return 0;
		else
			return(m_Streams[0]->GetVisData(ToHere, ulNumSamples));
	}
	else
	{
		// multiple streams... MIX THEM!!!
		int ulMaxSamples = 0;
		ClearFloat(fMixBuffer, ulNumSamples);

		for(unsigned long x=0; x<m_Streams.GetCount(); x++)
		{
			if(!m_Streams[x]->IsFinished())
			{
				int ulSamples = m_Streams[x]->GetVisData(fMixBuffer, ulNumSamples);
				if(ulSamples)
				{
					ulMaxSamples = max(ulSamples, ulMaxSamples);
					for(unsigned long samp = 0; samp < ulSamples; samp++)
					{
						ToHere[samp] += fMixBuffer[samp] / m_Streams.GetCount();
					}
				}
			}
		}
		return ulMaxSamples;
	}
	return 0;
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
			CAutoLock	t(&m_Lock);

			m_Streams[x]->Destroy();
			m_Streams.RemoveAt(x);

			x-=1;
		}	
	}
}


int CCoreAudio::GetVolumePercent()
{
	return m_iVolPercent;
}

void CCoreAudio::SetVolumePercent(int iVolPercent)
{
	if(iVolPercent > 100)
		iVolPercent = 100;
	if(iVolPercent < 0)
		iVolPercent = 0;

	m_iVolPercent = iVolPercent;

	for(unsigned long x=0; x<m_Streams.GetCount(); x++)
	{
		m_Streams[x]->SetVolumeScale(m_iVolPercent);
	}
}

void CCoreAudio::EnableEQ(bool bEnable)
{
	m_bEQEnabled = bEnable;
	for(unsigned long x=0; x<m_Streams.GetCount(); x++)
	{
		m_Streams[x]->EnableEQ(m_bEQEnabled);
	}
}

float CCoreAudio::GetEQLowGain()
{
	return m_fEQLow;
}

float CCoreAudio::GetEQMidGain()
{
	return m_fEQMid;
}

float CCoreAudio::GetEQHighGain()
{
	return m_fEQHigh;
}

void CCoreAudio::SetEQGain(float fEQLow, float fEQMid, float fEQHigh)
{
	m_fEQLow = fEQLow;
	m_fEQMid = fEQMid;
	m_fEQHigh = fEQHigh;
	for(unsigned long x=0; x<m_Streams.GetCount(); x++)
	{
		m_Streams[x]->SetEQGain(m_fEQLow, m_fEQMid, m_fEQHigh);
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

void	CCoreAudio::UpdateMetaData(LPTSTR szSource, void * pNewData, unsigned long ulFieldID)
{
	tuniacApp.UpdateMetaData(szSource, pNewData, ulFieldID);
}

void	CCoreAudio::UpdateMetaData(LPTSTR szSource, unsigned long pNewData, unsigned long ulFieldID)
{
	tuniacApp.UpdateMetaData(szSource, pNewData, ulFieldID);
}

void	CCoreAudio::UpdateMetaData(LPTSTR szSource, float pNewData, unsigned long ulFieldID)
{
	tuniacApp.UpdateMetaData(szSource, pNewData, ulFieldID);
}

void CCoreAudio::LogConsoleMessage(LPTSTR szModuleName, LPTSTR szMessage)
{
	if (tuniacApp.m_LogWindow)
	{
		if (tuniacApp.m_LogWindow->GetLogOn())
			tuniacApp.m_LogWindow->LogMessage(szModuleName, szMessage);
	}
}

void CCoreAudio::SetCrossfadeTime(unsigned long ulMS) 
{ 
	m_ulCrossfadeTimeMS = ulMS; 

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
				//Soundcard lost(eg unplugged)
				tuniacApp.CoreAudioMessage(NOTIFY_COREAUDIO_RESET, 0);
			}
			break;


	}
}

HRESULT CCoreAudio::OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR pwstrDeviceId)
{
	if (flow == eRender && role == eMultimedia)
		tuniacApp.CoreAudioMessage(NOTIFY_COREAUDIO_RESET, 0);

	return S_OK;
};