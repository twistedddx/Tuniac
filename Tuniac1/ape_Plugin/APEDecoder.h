#pragma once
#include "IAudioSource.h"
#include "../ape_SDK/All.h"
#include "../ape_SDK/MACLib.h"

#define MACFILTERPACKETSIZE 512 // in blocks (MAClib.h defines block as (n*sample), where n = number of channels)
#define QUANTFACTOR 4.656613428e-10F // used for 24-bit int -> 32-bit float conversion


class CAPEDecoder :
	public IAudioSource
{
protected:
	IAPEDecompress	*	MACDecompressor;

	char			*	pRawData;
	float			*	m_Buffer;
	int nChannels;
	int nBitsPerSample;
	int nBytesPerSample;
	int nSampleRate;
	int nBlockAlign;
	int nBlocksRetrieved;
	int result;

public:
	CAPEDecoder(void);
	~CAPEDecoder(void);

	bool		Open(LPTSTR szSource);
	bool		Close();

public:
	void		Destroy(void);
	bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels);
	bool		GetLength(unsigned long * MS);
	bool		SetPosition(unsigned long * MS);
	bool		SetState(unsigned long State);
	bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples);
};
