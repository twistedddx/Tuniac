#pragma once
#include "stdafx.h"
#include "iaudiosource.h"

#include "m4aAudioSource.h"

class Cm4aAudioSourceSupplier :
	public IAudioSourceSupplier
{
protected:
	IAudioSourceHelper		*		pHelper;
public:
	Cm4aAudioSourceSupplier(void);
	~Cm4aAudioSourceSupplier(void);

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

	IAudioSource *	CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO);
};
