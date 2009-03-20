#pragma once

#include "IAudioSource.h"

#include "FrameSplitter.h"
#include "Layer3Decoder.h"

#define BUFFERSIZE (2304 * sizeof(float))

class CMP3Decoder :
	public IAudioSource
{
protected:
	CFrameSplitter				m_Splitter;
	Frame						m_Frame;
	IMPEGDecoder		*		m_pDecoder;

	HANDLE						m_hFile;

	unsigned long				m_LastLayer;

	unsigned long				m_SampleRate;
	unsigned long				m_NumChannels;

	unsigned long				m_ID3v2Length;
	unsigned long				m_StreamLengthMS;
	unsigned long				m_NumFrames;
	unsigned long				m_StreamDataLength;
	unsigned long				m_VbrScale;
	unsigned char				m_TOC[100];

	float					*	m_SampleBuffer;


	bool						m_bXingValid;

	IAudioSourceHelper		*	m_pHelper;

	unsigned long	GetID3HeaderLength(unsigned char * buffer);
	bool			GetXingHeader(unsigned char * XingBuffer);
	bool			GetStreamData(void);

public:
	CMP3Decoder(IAudioSourceHelper * pHelper = NULL);
	~CMP3Decoder(void);

	bool		Open(LPTSTR szFilename);

public:
	void		Destroy(void);


	bool		GetLength(unsigned long * MS);
	bool		SetPosition(unsigned long * MS);

	bool		SetState(unsigned long State);

	bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels);
	bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples);

};
