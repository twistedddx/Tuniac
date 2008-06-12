#include "StdAfx.h"
#include "XAudioOutput.h"

CXAudioOutput::CXAudioOutput(IXAudio2 * pXAudio)
{
}

CXAudioOutput::~CXAudioOutput(void)
{
}

bool CXAudioOutput::Initialize(unsigned long ulSampleRate, unsigned long ulChannels)
{
	return false;
}

void CXAudioOutput::Destroy(void)
{
}

unsigned long CXAudioOutput::GetState(void)
{
	return 0;
}

bool CXAudioOutput::Start()
{
	return false;
}

bool CXAudioOutput::Stop()
{
	return false;
}

bool CXAudioOutput::Reset()
{
	return false;
}


unsigned long CXAudioOutput::SamplesPlayedSinceLastReset(void)
{
	return 0;
}


unsigned long CXAudioOutput::GetIdealBlockSize(void)
{
	return 0;
}


bool CXAudioOutput::BufferAvailable(void)
{
	return false;
}

bool CXAudioOutput::WriteBuffer(float * pfBuffer, unsigned long ulNumSamples)
{
	return false;
}

bool CXAudioOutput::EndOfStream(void)
{

	return false;
}