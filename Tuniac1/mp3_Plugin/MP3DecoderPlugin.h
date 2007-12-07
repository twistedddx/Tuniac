#pragma once
#include "IAudioSource.h"

class CMP3DecoderPlugin :
	public IAudioSourceSupplier
{
protected:
	IAudioSourceHelper	*	m_pHelper;
public:
	CMP3DecoderPlugin(void);
	~CMP3DecoderPlugin(void);

public:
	virtual void				Destroy(void);

	virtual void				SetHelper(IAudioSourceHelper * pHelper);
	virtual LPTSTR				GetName(void);
	virtual unsigned long		GetFlags(void);
	virtual GUID				GetPluginID(void);		// use guidgen to make a custom guid to return

	virtual bool				About(HWND hParent);
	virtual bool				Configure(HWND hParent);

	virtual bool				CanHandle(LPTSTR szSource);
	virtual	unsigned long		GetNumCommonExts(void);
	virtual	LPTSTR				GetCommonExt(unsigned long ulIndex);

	virtual IAudioSource *			CreateAudioSource(LPTSTR szSource);
};
