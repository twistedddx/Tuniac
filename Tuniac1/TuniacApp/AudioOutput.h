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

#pragma once

#include "CriticalSection.h"
#include "IAudioCallback.h"

class CAudioOutput : public IXAudio2VoiceCallback
{
protected:
	WAVEFORMATPCMEX						m_waveFormatPCMEx;

	IXAudio2SourceVoice		*			m_pSourceVoice;
	IXAudio2				*			m_pXAudio;

	/////////////////////////////////////////////
	//	Some of this stuff needs remoing due to oldness

	unsigned long						m_ulBuffersizeMS;

	float					*			m_pfAudioBuffer;

	bool								m_bPlaying;

	unsigned long						m_BlockSize;
	unsigned long						m_BlockSizeBytes;

	unsigned long						m_Interval;

	// new for XAudio, it tells us what bufer its working on, so we can point to it, get the m_liLastPerformanceCount, compare then grab the data right out the buffer!
	unsigned __int64					m_BufferInProgress;
	unsigned long						m_ulLastTickCount;

	unsigned __int64					m_SamplesOutLastReset;

	IAudioCallback			*			m_pCallback;

	CCriticalSection					m_AudioLock;
	HANDLE								m_hEvent;

	HANDLE								m_hThread;
	unsigned long						m_dwThreadID;
	bool								m_bThreadRun;

	bool								m_bStreamFinished;

	static unsigned long __stdcall		ThreadStub(void * in);
	unsigned long						ThreadProc(void);


	bool Open(void);
	bool Close(void);
	bool Initialize(void);
	bool Shutdown(void);

	~CAudioOutput(void);


/////////////////////////////////////////////////////////////
//			XAUDIO CALLBACKS!!!!!!
public:
	STDMETHOD_(void, OnVoiceProcessingPassStart) (UINT32) {}
    STDMETHOD_(void, OnVoiceProcessingPassEnd) () {}
    STDMETHOD_(void, OnStreamEnd) () 
	{
		m_bStreamFinished = true;
	}

    STDMETHOD_(void,OnBufferStart) ( void* context) 
	{
		//QueryPerformanceCounter(&m_liLastPerformanceCount);
		m_BufferInProgress = reinterpret_cast<unsigned __int64>(context);
		m_ulLastTickCount = GetTickCount();
	}

    STDMETHOD_(void,OnBufferEnd) ( void* ) 
	{ 
		SetEvent( m_hEvent ); 
	}

    STDMETHOD_(void,OnLoopEnd) ( void* ) {}   
	STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error) {};


public:
	CAudioOutput(IXAudio2 * pXAudio, unsigned long ulBufferSize = 250);

	void Destroy();

	void SetCallback(IAudioCallback * pCallback)
	{
		m_pCallback = pCallback;
		SetEvent(m_hEvent);
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

	unsigned long GetAudioBufferMS(void)
	{
		return m_ulBuffersizeMS;
	}

	__int64 GetSamplesOut(void);

	bool GetVisData(float * ToHere, unsigned long ulNumSamples);

	bool StreamFinished(void)
	{
		return m_bStreamFinished;
	}

};
