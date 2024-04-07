#pragma once
#include "iaudiosource.h"
#include "Array.h"
#include <strsafe.h>
#include <cstdint>
#include <string>
#include "bass.h"
#include <mmreg.h>

class CBASSDecoderPlugin :
	public IAudioSourceSupplier
{
protected:
	IAudioSourceHelper	*	m_pHelper;
	Array<std::wstring, 12>		exts;

	HSTREAM				hStreamID;
	bool				bIsStream;
	bool				bModFile;
	bool				bMicInput;

	unsigned long		m_ActiveDevice;


public:
	CBASSDecoderPlugin(void);
	~CBASSDecoderPlugin(void);

public:
	virtual void			Destroy(void);

	virtual void			SetHelper(IAudioSourceHelper * pHelper);
	virtual LPTSTR			GetName(void);
	virtual GUID			GetPluginID(void);

	virtual unsigned long	GetFlags(void);

	virtual bool			About(HWND hParent);
	virtual bool			Configure(HWND hParent);

	virtual bool			CanHandle(LPTSTR szSource);
	virtual bool			Close(void);
	virtual	unsigned long	GetNumCommonExts(void);
	virtual	LPTSTR			GetCommonExt(unsigned long ulIndex);

	virtual IAudioSource *		CreateAudioSource(LPTSTR szSource, IAudioFileIO * pFileIO);
};