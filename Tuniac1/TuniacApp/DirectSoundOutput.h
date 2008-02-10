#pragma once

#include "CriticalSection.h"

#include "IAudioCallback.h"
//#include <dsound.h>

class CDirectSoundOutput
{
protected:
	WAVEFORMATPCMEX						m_waveFormatPCMEx;

	IAudioCallback			*			m_pCallback;


public:
	CDirectSoundOutput(void);
	~CDirectSoundOutput(void);

	void SetCallback(IAudioCallback * pCallback)
	{
		m_pCallback = pCallback;
	}

	unsigned long GetSampleRate(void)
	{
		return m_waveFormatPCMEx.Format.nSamplesPerSec;
	}

	unsigned long GetChannels(void)
	{
		return m_waveFormatPCMEx.Format.nChannels;
	}

	unsigned long GetBlockSize()
	{
		return 4096;
	}

public:
	bool SetFormat(unsigned long SampleRate, unsigned long Channels);

	// actual controls
	bool Start(void);
	bool Stop(void);
	bool Reset(void);

	bool GetVisData(float * ToHere, unsigned long ulNumSamples);
};
