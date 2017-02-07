#pragma once
#include "iaudiosource.h"

#include <shnplay.h>


class CshnDecoder :
	public IAudioSource
{
protected:

	float				* m_Buffer;
	char				* buffer;
	FILE				* file;

	ShnPlayStream vtbl;
	ShnPlayInfo info;
	ShnPlayStream * stream;
	ShnPlay * state;

	unsigned long ulLength;
	unsigned long ulByterate;


public:
	CshnDecoder(void);
	~CshnDecoder(void);

	bool Open(LPTSTR szSource);

public:
	void		Destroy(void);
	bool		GetFormat(unsigned long * SampleRate, unsigned long * Channels);
	bool		GetLength(unsigned long * MS);
	bool		SetPosition(unsigned long * MS);
	bool		SetState(unsigned long State);
	bool		GetBuffer(float ** ppBuffer, unsigned long * NumSamples);
};
