#pragma once

#include "iaudiosource.h"

#include <wmsdk.h>


#ifndef SAFE_RELEASE
#define SAFE_RELEASE( x )  \
    if ( NULL != x )       \
    {                      \
        x->Release( );     \
        x = NULL;          \
    }

#endif

#ifndef SAFE_ARRAYDELETE
#define SAFE_ARRAYDELETE( x )  \
    if( NULL != x )        \
    {                      \
        delete [] x;       \
        x = NULL;          \
    }
#endif

class CWMADecoder :
	public IAudioSource
{
protected:

	float				m_Buffer[131072];
	DWORD				m_audioBufferLength;

	IWMSyncReader			*	m_ISyncReader;
	INSSBuffer				*	m_pINSSBuffer;
	IWMOutputMediaProps		*	m_IAudioOutputProps;
	WM_MEDIA_TYPE			*	m_theMediaType;
	WORD						m_iAudioStreamNumber;
	DWORD						m_theOutputsCount;
	HRESULT hr;

	int m_iAudioOutputNumber;

	unsigned long ulChannels;
	unsigned long ulSampleRate;
	unsigned long m_qwTotalTimeInMS;

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
