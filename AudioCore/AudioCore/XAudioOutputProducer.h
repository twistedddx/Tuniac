#pragma once

#include <IAudioOutput.h>
#include <XAudio2.h>

#include "XAudioOutput.h"


#include <vector>

class CXAudioOutputProducer : public IAudioOutputProducer
{
protected:
	IXAudio2				*			m_pXAudio;
    IXAudio2MasteringVoice	*			m_pMasteringVoice;

	~CXAudioOutputProducer(void);

public:
	CXAudioOutputProducer(void);

	void Destroy(void);
	bool Initialize(void);
	bool Shutdown(void);

	IAudioOutput * CreateAudioOutput(unsigned long ulSampleRate, unsigned long Channels);
};
