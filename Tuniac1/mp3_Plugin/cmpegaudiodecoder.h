#pragma once

#include "IAudioSource.h"

#include "mpegdecoder.h"

#define DATABUFFERSIZE	(4096)

class CMpegAudioDecoder :
	public IAudioSource
{
protected:
	IAudioFileIO *	m_pSource;
	
	uint64_t		m_datastart;		// tells us where the mp3 data starts! (this is effectively offset 0)
										// this should skip ID3 Tag and XING/LAME header too pls! :D
	
	bool		m_bXingValid;
	uint32_t	m_NumFrames;
	uint32_t	m_StreamDataLength;
	uint8_t		m_TOC[100];
	uint32_t	m_VbrScale;
	
	uint64_t	m_TotalSamples;
	uint32_t	m_TotalMS;
	
	uint8_t		m_databuffer[DATABUFFERSIZE];
	
	float		m_audiobuffer[2304];
	
	uint32_t	m_SampleRate;
	uint32_t	m_Channels;
	
	mpegdecoder		decoder;

	
	bool		AnalyseStream(void);

public:
	bool		Open(IAudioFileIO * pFileIO);

public:
	void		Destroy(void);


	bool		GetLength(unsigned long * MS);
	bool		SetPosition(unsigned long * MS);

	bool		SetState(unsigned long State);

	bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels);
	bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples);

};
