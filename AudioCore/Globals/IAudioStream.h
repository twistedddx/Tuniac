#pragma once

typedef unsigned long AUDIOSTREAMID;


class IAudioStream
{
public:
	virtual bool	Play(void) = 0;
	virtual bool	Stop(void) = 0;
};