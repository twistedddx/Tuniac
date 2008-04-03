#pragma once

#include "iaudiosource.h"

#include ".\OptimFROG\OptimFROG.h"

class COFRDecoder :
	public IAudioSource
{
protected:
	char		*	buffer;
	float		*	m_Buffer;
    void* decoderInstance;
    OptimFROG_Info iInfo;

	float			m_divider;

public:
	COFRDecoder(void);
	~COFRDecoder(void);

;
	bool Open(LPTSTR szSource);
	bool Close();

public:
	void		Destroy(void);
	bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels);
	bool		GetLength(unsigned long * MS);
	bool		SetPosition(unsigned long * MS);
	bool		SetState(unsigned long State);
	bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples);
};