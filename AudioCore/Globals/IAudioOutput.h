#pragma once

class IAudioOutput
{
public:
	virtual void Destroy(void)													= 0;

	virtual bool SetFormat(unsigned long ulSampleRate, unsigned long Channels)	= 0;
	virtual bool Start()														= 0;
	virtual bool Stop()															= 0;
	virtual bool Reset()														= 0;

	virtual unsigned long SamplesPlayedSinceLastReset(void)						= 0;

	virtual unsigned long GetBufferLengthMS(void)								= 0;

	virtual unsigned long GetIdealBlockSize(void)								= 0;

	virtual bool BufferAvailable(void)											= 0;
	virtual bool WriteBuffer(float * pfBuffer, unsigned long ulNumSamples)		= 0;
};

class IAudioOutputProducer
{
public:
	bool Initialize(void);
	bool Shutdown(void);

	IAudioOutput * CreateAudioOutput(void);
};