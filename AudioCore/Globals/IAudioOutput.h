#pragma once

class IAudioOutput
{
public:
	virtual void Destroy(void)													= 0;

	virtual unsigned long GetState(void)										=0;

	// fairly obvious
	virtual bool Start()														= 0;
	virtual bool Stop()															= 0;

	// stops the audio device, purges all queued buffers, and puts everything back to a state of startup
	virtual bool Reset()														= 0;

	// returns the number of samples played since the last reset or since creation
	virtual unsigned long SamplesPlayedSinceLastReset(void)						= 0;

	// returns the size of a 'block' to write IN SAMPLES * CHANNELS		(always an intergral number of samples)
	virtual unsigned long GetIdealBlockSize(void)								= 0;

	// true if there is space to write GetIdealBlockSize amount of data
	virtual bool BufferAvailable(void)											= 0;

	/// Writes audio data to the stream : never more than the value returned from GetIdealBlockSize;
	virtual bool WriteBuffer(float * pfBuffer, unsigned long ulNumSamples)		= 0;
	/// this function specifies any pending audio data should be written to the stream and playback stopped one it has been played
	virtual bool EndOfStream(void)												= 0;			
};

class IAudioOutputProducer
{
public:
	virtual void Destroy(void)																		= 0;

	virtual bool Initialize(void)																	= 0;
	virtual bool Shutdown(void)																		= 0;

	virtual IAudioOutput * CreateAudioOutput(unsigned long ulSampleRate, unsigned long Channels)	= 0;
};