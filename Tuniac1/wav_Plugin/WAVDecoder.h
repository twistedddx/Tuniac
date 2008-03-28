#pragma once

#include "iaudiosource.h"
#include <stdio.h>

typedef struct{
	char rID[4];            // 'RIFF'
	long int rLen;
  
	char wID[4];            // 'WAVE'
  
	char fId[4];            // 'fmt '
	long int pcm_header_len;   // varies...
	short int wFormatTag;
	short int nChannels;      // 1,2 for stereo data is (l,r) pairs
	long int nSamplesPerSec;
	long int nAvgBytesPerSec;
	short int nBlockAlign;      
	short int nBitsPerSample;
}   WAV_HDR;

	// header of wav file
typedef struct{
	char dId[4];            // 'data' or 'fact'
	long int dLen;
	//   unsigned char *data;
}   CHUNK_HDR;

class CWAVDecoder :
	public IAudioSource
{
protected:
	FILE			*	m_file;
	float				m_Buffer[4096];
	float				m_divider;
	long				wbuff[4096];

	bool m_bFloatMode;
	WAV_HDR wav;
	CHUNK_HDR chk;
	unsigned int wstat;
	unsigned int rmore;
	int sflag;

	unsigned int wbuff_len;

public:
	CWAVDecoder(void);
	~CWAVDecoder(void);

	bool FOURCC_EQUAL(char* X, char* Y);
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