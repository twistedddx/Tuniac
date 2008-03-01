#pragma once

class IAudioCallback
{
public:
	virtual bool GetBuffer(float * pAudioBuffer, unsigned long NumSamples) = 0;
	virtual bool ServiceStream(void) = 0;			// new so audio playback thread can produce a new buffer while its not doing anything else!!!

};