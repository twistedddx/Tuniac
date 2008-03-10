#pragma once

class IAudioOutput
{
public:
	virtual bool SetFormat(unsigned long ulSampleRate, unsigned long Channels) = 0;

	virtual unsigned long GetBufferSize(void) = 0;
	virtual bool BufferAvailable(void) = 0;

};