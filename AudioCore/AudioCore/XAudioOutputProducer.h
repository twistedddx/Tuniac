#pragma once

#include <IAudioOutput.h>
#include <XAudio2.h>

class CXAudioOutputProducer
{
protected:
	IXAudio2				*			m_pXAudio;
    IXAudio2MasteringVoice	*			m_pMasteringVoice;

public:
	CXAudioOutputProducer(void);
	~CXAudioOutputProducer(void);

	bool Initialize(void);
	bool Shutdown(void);

	IAudioOutput * CreateAudioOutput(void);
};
