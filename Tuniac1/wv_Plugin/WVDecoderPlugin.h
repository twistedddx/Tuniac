#pragma once
#include "iaudiosource.h"

class CWVDecoderPlugin :
	public IAudioSourceSupplier
{
public:
	CWVDecoderPlugin(void);
	~CWVDecoderPlugin(void);
public:
	virtual void			Destroy(void);

	virtual void			SetHelper(IAudioSourceHelper * pHelper);
	virtual LPTSTR			GetName(void);
	virtual GUID			GetPluginID(void);

	virtual unsigned long	GetFlags(void);

	virtual bool			About(HWND hParent);
	virtual bool			Configure(HWND hParent);

	virtual bool			CanHandle(LPTSTR szSource);
	virtual	unsigned long	GetNumCommonExts(void);
	virtual	LPTSTR			GetCommonExt(unsigned long ulIndex);

	virtual IAudioSource *		CreateAudioSource(LPTSTR szSource);
};