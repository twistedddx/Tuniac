#pragma once

#include "MP3StreamerDecoder.h"

class CMP3StreamerDecoderPlugin :
	public IAudioSourceSupplier
{
protected:
	IAudioSourceHelper *	m_pHelper;

public:
	CMP3StreamerDecoderPlugin(void);
	~CMP3StreamerDecoderPlugin(void);

public:
	virtual void					Destroy(void);

	virtual void					SetHelper(IAudioSourceHelper * pHelper);
	virtual LPTSTR					GetName(void);
	virtual GUID					GetPluginID(void);

	virtual unsigned long			GetFlags(void);

	virtual bool					About(HWND hParent);
	virtual bool					Configure(HWND hParent);

	virtual bool					CanHandle(LPTSTR szSource);
	virtual	unsigned long			GetNumCommonExts(void);
	virtual	LPTSTR					GetCommonExt(unsigned long ulIndex);

	virtual IAudioSource *			CreateAudioSource(LPTSTR szSource);

};
