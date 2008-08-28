#pragma once

#include "iaudiosource.h"

#include <wmsdk.h>

class CWMADecoder :
	public IAudioSource
{
protected:

	float				m_Buffer[131072];

	IWMSyncReader			*	m_ISyncReader;
	INSSBuffer				*	m_pINSSBuffer;
	WM_MEDIA_TYPE			*	m_theMediaType;
	IWMOutputMediaProps		*	m_IAudioOutputProps;
	WORD						m_iAudioStreamNumber;
	DWORD						m_theOutputsCount;
	HRESULT hr;

	int m_iAudioOutputNumber;

	unsigned long ulChannels;
	unsigned long ulSampleRate;

	QWORD				m_cnsFileDuration;
	BOOL                m_bIsSeekable;
	unsigned long		m_ulTotalTimeInMS;

public:
	CWMADecoder(void);
	~CWMADecoder(void);

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
