#include "StdAfx.h"
#include "XAudioOutputProducer.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif


CXAudioOutputProducer::CXAudioOutputProducer(void)
{
}

CXAudioOutputProducer::~CXAudioOutputProducer(void)
{
}

void CXAudioOutputProducer::Destroy(void)
{
	this->Shutdown();
	delete this;
}

bool CXAudioOutputProducer::Initialize(void)
{
	//////////////////////////////////////////////////////////////////////////
	//
	// FIRST INITIALIZE XAUDIO
	//
	if(FAILED(XAudio2Create(&m_pXAudio)))
	{
		//booo we dont have an XAudio2 object!
		//if (IDYES == MessageBox(NULL, TEXT("Failed initializing XAudio.\nThis is most likely because you have not installed DirectX March 2008 redist which is required.\nDo you want to be directed to the Microsoft download?"), TEXT("Fatal Error"), MB_YESNO | MB_ICONERROR))
		//{
			///ShellExecute(NULL, NULL, TEXT("http://download.microsoft.com/download/6/4/c/64c3d3d6-c5da-47eb-9db4-86e45b9f579e/directx_mar2008_redist.exe"), NULL, NULL, SW_SHOW);
		//}
		m_pXAudio = NULL;
		return false;
	}

	m_pXAudio->StartEngine();
	//m_pXAudio->RegisterForCallbacks(this);

	HRESULT hr;

    if ( FAILED(hr = m_pXAudio->CreateMasteringVoice( &m_pMasteringVoice ) ) )
    {
		MessageBox(NULL, TEXT("Failed creating mastering voice"), TEXT("Fatal Error"), MB_OK | MB_ICONERROR);
        return false;
    }

	return true;
}

bool CXAudioOutputProducer::Shutdown(void)
{

    // All XAudio2 interfaces are released when the engine is destroyed, but being tidy
	if(m_pMasteringVoice)
	{
	    m_pMasteringVoice->DestroyVoice();
		m_pMasteringVoice = NULL;
	}

	if(m_pXAudio)
	{
		//m_pXAudio->UnregisterForCallbacks(this);
		m_pXAudio->StopEngine();
		SAFE_RELEASE(m_pXAudio);
	}

	return true;
}

IAudioOutput * CXAudioOutputProducer::CreateAudioOutput(unsigned long ulSampleRate, unsigned long ulChannels)
{
	CXAudioOutput * pOutput = new CXAudioOutput(m_pXAudio);

	if(pOutput->Initialize(ulSampleRate, ulChannels))
	{
		// yay
		return pOutput;
	}

	pOutput->Destroy();
	return NULL;
}