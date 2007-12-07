#pragma once

class IAudioCallback
{
public:
	virtual bool GetBuffer(float * pAudioBuffer, unsigned long NumSamples) = 0;
};