#pragma once

#include <IAudioStream.h>
#include <IAudioSource.h>

class IAudioCoreCallback
{
public:
	// nothing here yet!
};


// stream enumerator, you implement these to get a list of all the streams in the audiocore, pStream is locked for the duration of the call
// so feel free to use it BUT BE QUICK!!!
class IAudioCoreStreamEnumerator
{
public:
	virtual bool AudioCoreEnumerateStream(AUDIOSTREAMID asidStreamID, IAudioStream * pStream) = 0;
	virtual bool AudioCoreEnumerateStreamFinished(void) = 0;
};


class IAudioCore
{
public:
	virtual void				Destroy(void)														= 0;

	virtual bool				Startup(void)														= 0;
	virtual bool				Shutdown(void)														= 0;

	//////////////////////////////////////////////////////////////////////////////////
	// Remove all streams, reset everything back to it would be at startup
	// Source plugins are not unloaded!!!!
	virtual bool				Reset(void)															= 0;

	//////////////////////////////////////////////////////////////////////////////////
	// Audio Source functions 
	// this will load a dll which exports the CreateAudioSourceSupplier function, and 
	// add the created IAudioSourceSupplier to an internal list!
	virtual bool				LoadAudioSource(wchar_t		*	wcsPluginName)						= 0;
	// Releases all resourcs (including DLLS and such from the audio engine)
	// NOTE: THIS INHERENTLY IMPLIES A Reset() - since there can be no audio sources
	// except perhaps user supplied ones, but too bad....
	virtual bool				FreeAudioSources(void)												= 0;


	//////////////////////////////////////////////////////////////////////////////////
	// Stream creation, render stream will find an input plugin to play the stream
	// CreateAudioStream allows you to pass in your own audio sample provider to do
	// your own special stream playback along side everything else
	virtual AUDIOSTREAMID		RenderAudioStream(wchar_t	*	pwsFileName)						= 0;
	virtual AUDIOSTREAMID		CreateAudioStream(IAudioSource	* pSource)							= 0;		// Supply your own audio source!

	///////////////////////////////////////////////////////////////////////
	// Destroy an audio stream based on the ID, may return false if the stream is locked
	virtual bool				DestroyAudioStream(AUDIOSTREAMID	streamID)						= 0;

	///////////////////////////////////////////////////////////////////////
	// locking an audio stream means:
	//
	// A> nothing else can lock the stream as long as you own the lock
	// B> the stream can't be destroyed out from under you
	// C> Potentially adding buffers to the stream will be stopped for the duration of your lock - SO BE QUICK!
	// ^^ that is to say you will have at least as long as the output buffer to do your work
	// it may also not affect the stream at all...
	virtual IAudioStream *		LockAudioStream(AUDIOSTREAMID	streamID)							= 0;
	virtual bool				UnlockAudioStream(IAudioStream * pAudioStream)						= 0;

	//////////////////////////////////////////////////////////////////////////////////
	// Enumeration, uses he IAudioCoreStreamEnumerator passed in to enumerate all the streams

	virtual bool				EnumerateAudioStreams(IAudioCoreStreamEnumerator * pEnumerator)		= 0;

	///////////////////////////////////////////////////////////////////////
	// affects all streams equally
	virtual void				SetGlobalVolume(float fPercent)										= 0;
	virtual float				GetGlobalVolume(void)												= 0;
};