#pragma once
#include "iaudiooutput.h"
#include <XAudio2.h>

class CXAudioOutputProducer;

class CXAudioOutput :
	public IAudioOutput,
	public IXAudio2VoiceCallback
{
	friend CXAudioOutputProducer;
protected:

	IXAudio2 * m_pXAudio;

	bool Initialize(unsigned long ulSampleRate, unsigned long ulChannels);

public:
	CXAudioOutput(IXAudio2 * pXAudio);
	~CXAudioOutput(void);

public:
	void Destroy(void);

	unsigned long GetState(void);

	bool Start();
	bool Stop();

	bool Reset();

	unsigned long SamplesPlayedSinceLastReset(void);

	unsigned long GetIdealBlockSize(void);

	bool BufferAvailable(void);
	bool WriteBuffer(float * pfBuffer, unsigned long ulNumSamples);
	bool EndOfStream(void);

/////////////////////////////////////////////////////////////
//			XAUDIO CALLBACKS!!!!!!
public:
	STDMETHOD_(void, OnVoiceProcessingPassStart) () {}
    STDMETHOD_(void, OnVoiceProcessingPassEnd) () {}
    STDMETHOD_(void,OnStreamEnd) () 
	{
	}

    STDMETHOD_(void,OnBufferStart) ( void* context) 
	{
		//QueryPerformanceCounter(&m_liLastPerformanceCount);
		//m_BufferInProgress = reinterpret_cast<unsigned __int64>(context);
		//m_ulLastTickCount = GetTickCount();
	}

    STDMETHOD_(void,OnBufferEnd) ( void* ) 
	{ 
		//SetEvent( m_hEvent ); 
	}

    STDMETHOD_(void,OnLoopEnd) ( void* ) {}   
	STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error) {};
};
