#pragma once
#include "IAudioSource.h"

class CMpegAudioDecoderPlugin :
	public IAudioSourceSupplier
{
protected:
	IAudioSourceHelper	*	m_pHelper;
public:
	CMpegAudioDecoderPlugin(void);
	~CMpegAudioDecoderPlugin(void);

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

	virtual IAudioSource *			CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO);
};
