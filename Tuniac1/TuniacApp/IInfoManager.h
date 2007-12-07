#pragma once

#define ITUNIACINFOMANAGER_VERSION		MAKELONG(0, 1)

#include "LibraryEntry.h"

class IInfoManager
{
public:
	virtual void			Destroy(void)										= 0;

	virtual unsigned long	GetNumExtensions(void)								= 0;
	virtual LPTSTR			SupportedExtension(unsigned long ulExtentionNum)	= 0;

	virtual bool			CanHandle(LPTSTR szSource)							= 0;
	virtual bool			GetInfo(LibraryEntry * libEnt)						= 0;
	virtual bool			SetInfo(LibraryEntry * libEnt)						= 0;
};

typedef IInfoManager * (*CreateInfoManagerPluginFunc)(void);
extern "C" __declspec(dllexport) IInfoManager * CreateInfoManagerPlugin(void);

typedef unsigned long (*GetTuniacInfoManagerVersionFunc)(void);
extern "C" __declspec(dllexport) unsigned long GetTuniacInfoManagerVersion(void);
