#pragma once

class IAudioOutput
{
	bool SetFormat(unsigned long SampleRate, unsigned long Channels);

	// actual controls
	bool Start(void);
	bool Stop(void);
	bool Reset(void);

	unsigned long GetSampleRate(void);
	unsigned long GetChannels(void);

	unsigned long GetBlockSize(void);
	bool GetVisData(float * ToHere, unsigned long ulNumSamples);
};