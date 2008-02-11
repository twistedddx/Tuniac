#pragma once

#include "CriticalSection.h"

#include "IAudioCallback.h"
#include <xaudio2.h>



class CAudioOutput : public IXAudio2VoiceCallback
{
protected:
	WAVEFORMATPCMEX						m_waveFormatPCMEx;

	IXAudio2				*			m_pXAudio;
	IXAudio2SourceVoice		*			m_pSourceVoice;
    IXAudio2MasteringVoice	*			m_pMasteringVoice;



	/////////////////////////////////////////////
	//	Some of this stuff needs remoing due to oldness


	float					*			m_pfAudioBuffer;

	bool								m_bPlaying;

	unsigned long						m_BlockSize;
	unsigned long						m_BlockSizeBytes;

	unsigned long						m_Interval;

	// new for XAudio, it tells us what bufer its working on, so we can point to it, get the m_liLastPerformanceCount, compare then grab the data right out the buffer!
	unsigned __int64					m_BufferInProgress;
	unsigned long						m_ulLastTickCount;

	IAudioCallback			*			m_pCallback;

	CCriticalSection					m_AudioLock;
	HANDLE								m_hEvent;

	HANDLE								m_hThread;
	unsigned long						m_dwThreadID;
	bool								m_bThreadRun;

	static unsigned long __stdcall		ThreadStub(void * in);
	unsigned long						ThreadProc(void);


	bool Open(void);
	bool Close(void);
	bool Initialize(void);
	bool Shutdown(void);

/////////////////////////////////////////////////////////////
//			XAUDIO CALLBACKS!!!!!!
public:
	STDMETHOD_(void, OnVoiceProcessingPassStart) () {}
    STDMETHOD_(void, OnVoiceProcessingPassEnd) () {}
    STDMETHOD_(void,OnStreamEnd) () {}
    STDMETHOD_(void,OnBufferStart) ( void* context) 
	{
		//QueryPerformanceCounter(&m_liLastPerformanceCount);
		m_BufferInProgress = reinterpret_cast<unsigned __int64>(context);
		m_ulLastTickCount = GetTickCount();
	}
    STDMETHOD_(void,OnBufferEnd) ( void* ) { SetEvent( m_hEvent ); }
    STDMETHOD_(void,OnLoopEnd) ( void* ) {}   


public:
	CAudioOutput(void);
	~CAudioOutput(void);



	void SetCallback(IAudioCallback * pCallback)
	{
		m_pCallback = pCallback;
	}

	bool SetFormat(unsigned long SampleRate, unsigned long Channels);

	// actual controls
	bool Start(void);
	bool Stop(void);
	bool Reset(void);

	unsigned long GetSampleRate(void)
	{
		return m_waveFormatPCMEx.Format.nSamplesPerSec;
	}

	unsigned long GetChannels(void)
	{
		return m_waveFormatPCMEx.Format.nChannels;
	}

	unsigned long GetBlockSize(void)
	{
		return m_BlockSize;
	}

	unsigned long GetInterval(void)
	{
		return m_Interval;
	}

	unsigned long GetMSOut(void)
	{
		__int64 SamplesOut = GetSamplesOut();

		return( (unsigned long)((float)SamplesOut / ((float)m_waveFormatPCMEx.Format.nSamplesPerSec / 1000.0f)) );
	}

	__int64 GetSamplesOut(void);

	bool GetVisData(float * ToHere, unsigned long ulNumSamples);

};

/*
	HWAVEOUT							m_waveHandle;
	WAVEHDR					*			m_Buffers;

	static void CALLBACK WaveCallback(HWAVEOUT hWave, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD dw2);

*/