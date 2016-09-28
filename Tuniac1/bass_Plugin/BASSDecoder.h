#pragma once

#include "iaudiosource.h"
#include <cstdio>
#include <strsafe.h>

#include "bass.h"
#include "bass_aac.h"
#include "bass_ac3.h"
#include "bass_adx.h"
#include "bass_ape.h"
#include "bass_mpc.h"
#include "bass_ofr.h"
#include "bass_spx.h"
#include "bass_tta.h"
#include "bassalac.h"
#include "basscd.h"
#include "bassdsd.h"
#include "bassflac.h"
#include "bassmidi.h"
#include "bassopus.h"
#include "basswma.h"
#include "basswv.h"
#include "basshls.h"

#define BUFFERSIZE (1024 * sizeof(float))

class CBASSDecoder :
	public IAudioSource
{
protected:

	FILE			*	m_file;

	float			*	m_Buffer;
	unsigned long		numSamples;
	DWORD				readBytes;

	bool				m_bIsStream;
	double				dTime;
	bool				m_bModFile;
	BASS_CHANNELINFO	info;
	HSTREAM				m_decodehandle;

public:
	CBASSDecoder(void);
	~CBASSDecoder(void);

	bool Open(LPTSTR szSource, IAudioSourceHelper * pHelper, HSTREAM decodeHandle, bool bModfile, bool bIsStream);
	//bool Open(LPTSTR szSource, IAudioSourceHelper * pHelper);
	bool Close();

public:
	void		Destroy(void);
	bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels);
	bool		GetLength(unsigned long * MS);
	bool		SetPosition(unsigned long * MS);
	bool		SetState(unsigned long State);
	bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples);
};
