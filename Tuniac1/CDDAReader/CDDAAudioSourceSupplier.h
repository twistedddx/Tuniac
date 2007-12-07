#pragma once
#include "iaudiosource.h"

class CCDDAAudioSourceSupplier :
	public IAudioSourceSupplier
{
public:
	CCDDAAudioSourceSupplier(void);
	~CCDDAAudioSourceSupplier(void);

public:
	void			Destroy(void);

	void			SetHelper(IAudioSourceHelper * pHelper);

	LPTSTR			GetName(void);
	GUID			GetPluginID(void);
	unsigned long	GetFlags(void);

	bool			About(HWND hParent);
	bool			Configure(HWND hParent);

	bool			CanHandle(LPTSTR szSource);
	unsigned long	GetNumCommonExts(void);
	LPTSTR			GetCommonExt(unsigned long ulIndex);

	IAudioSource *	CreateAudioSource(LPTSTR szSource);

};
