#pragma once

typedef unsigned long AUDIOSTREAMID;


class IAudioStream
{
public:
	virtual AUDIOSTREAMID	GetStreamID(void)				= 0;

	virtual bool			Play(void)						= 0;
	virtual bool			Stop(void)						= 0;

	virtual unsigned long	GetLength(void)					= 0;
	virtual unsigned long	GetPosition(void)				= 0;
	virtual bool			SetPosition(unsigned long MS)	= 0;
};