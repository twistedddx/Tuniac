#pragma once

#include "CriticalSection.h"

#include "IAudioCallback.h"

class CAudioOutput
{
protected:
	WAVEFORMATPCMEX						m_waveFormatPCMEx;
	HWAVEOUT							m_waveHandle;
	WAVEHDR					*			m_Buffers;

#ifdef VISTAAUDIOHACK
	float					*			pVistaTempBuffer;
#endif

	float					*			m_pfAudioBuffer;
	unsigned long						m_dwBufferSize;

	bool								m_Playing;

	unsigned long						m_BufferLengthMS;
	unsigned long						m_BlockSize;
	unsigned long						m_Interval;
	unsigned long						m_NumBuffers;

	unsigned long						m_ActiveBuffer;
	unsigned long						m_QueuedBuffers;
	unsigned long						m_LastPlayedBuffer;

	__int64								m_dwSamplesOut;

	LARGE_INTEGER						m_liCountsPerSecond;
	LARGE_INTEGER						m_liLastPerformanceCount;
	unsigned long						m_dwLastTickCount;

	IAudioCallback			*			m_pCallback;

	CCriticalSection					m_AudioLock;
	HANDLE								m_hEvent;

	HANDLE								m_hThread;
	unsigned long						m_dwThreadID;
	bool								m_bThreadRun;

	static unsigned long __stdcall		ThreadStub(void * in);
	unsigned long						ThreadProc(void);

	static void CALLBACK WaveCallback(HWAVEOUT hWave, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD dw2);

	bool Open(void);
	bool Close(void);
	bool Initialize(void);
	bool Shutdown(void);

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

	unsigned long GetMaxRewind(void)
	{
		return m_NumBuffers;
	}

	unsigned long GetMSOut(void)
	{
		__int64 SamplesOut = GetSamplesOut();

		return( (unsigned long)((float)SamplesOut / ((float)m_waveFormatPCMEx.Format.nSamplesPerSec / 1000.0f)) );
	}

	__int64 GetSamplesOut(void);

	bool GetVisData(float * ToHere, unsigned long ulNumSamples);

};
