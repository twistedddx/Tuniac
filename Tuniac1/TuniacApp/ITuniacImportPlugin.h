#pragma once

#define ITUNIACIMPORTPLUGIN_VERSION	MAKELONG(0, 1)

class ITuniacImportPlugin {

public:
	virtual LPTSTR			GetName(void)										= 0;
	virtual void			Destory(void)										= 0;

	virtual unsigned long	GetNumExtensions(void)								= 0;
	virtual LPTSTR			SupportedExtension(unsigned long ulExtentionNum)	= 0;

	virtual bool			CanHandle(LPTSTR szSource)							= 0;

	virtual bool			BeginImport(LPTSTR szSource)						= 0;
	virtual bool			ImportUrl(LPTSTR szDest, unsigned long iDestSize)	= 0;
	virtual bool			ImportTitle(LPTSTR szDest, unsigned long iDestSize)	= 0;
	virtual bool			EndImport(void)										= 0;
};

typedef ITuniacImportPlugin * (*CreateTuniacImportPluginFunc)(void);
extern "C" __declspec(dllexport) ITuniacImportPlugin * CreateTuniacImportPlugin(void);

typedef unsigned long (*GetTuniacImportPluginVersionFunc)(void);
extern "C" __declspec(dllexport) unsigned long		GetTuniacImportPluginVersion(void);