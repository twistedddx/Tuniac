#pragma once

typedef unsigned long AUDIOSTREAMID;

class IAudioCoreCallback
{
public:
	// nothing here yet!
};

class IAudioCore
{
public:
	virtual void				Destroy(void)									= 0;

	virtual bool				Startup(void)									= 0;
	virtual bool				Shutdown(void)									= 0;

	///////////////////////////////////////////////////////////////////////
	// Stream creation, render stream will find an input plugin to play the stream
	// CreateAudioStream allows you to pass in your own audio sample provider to do
	// your own special stream playback along side everything else
	virtual AUDIOSTREAMID		RenderAudioStream(wchar_t	*	pwsFileName)	= 0;
	virtual AUDIOSTREAMID		CreateAudioStream(void * pVoid)					= 0;		// for future expansion ATM

	///////////////////////////////////////////////////////////////////////
	// Destroy an audio stream based on the ID, may return false if the stream is locked
	virtual bool				DestroyAudioStream(AUDIOSTREAMID	streamID)	= 0;

	///////////////////////////////////////////////////////////////////////
	// locking an audio stream means:
	//
	// A> nothing else can lock the stream as long as you own the lock
	// B> the stream can't be destroyed out from under you
	// C> Potentially playback of the stream will be stopped for the duration of your lock - SO BE QUICK!
	// ^^ that is to say you will have at least as long as the output buffer to do your work
	// it may also not affect the stream at all...
	virtual void *				LockAudioStream(AUDIOSTREAMID	streamID)		= 0;
	virtual bool				UnlockAudioStream(void * pAudioStream)			= 0


	///////////////////////////////////////////////////////////////////////
	// affects all streams equally
	virtual void				SetGlobalVolume(float fPercent)					= 0;
	virtual float				GetGlobalVolume(void)							= 0;
};