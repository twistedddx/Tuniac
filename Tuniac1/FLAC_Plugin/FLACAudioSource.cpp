#include "StdAfx.h"
#include "FLACAudioSource.h"

CFLACAudioSource::CFLACAudioSource(void)
{
}

CFLACAudioSource::~CFLACAudioSource(void)
{
}

bool		CFLACAudioSource::Open(LPTSTR szStream)
{
	return false;
}

void		CFLACAudioSource::Destroy(void)
{
	delete this;
}

bool		CFLACAudioSource::GetLength(unsigned long * MS)
{
	return false;
}

bool		CFLACAudioSource::SetPosition(unsigned long * MS)
{
	return false;
}

bool		CFLACAudioSource::SetState(unsigned long State)
{
	return false;
}

bool		CFLACAudioSource::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	return false;
}

bool		CFLACAudioSource::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	return false;
}

