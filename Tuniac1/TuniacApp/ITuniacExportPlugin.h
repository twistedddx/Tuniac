#pragma once

#include "LibraryEntry.h"

#define ITUNIACEXPORTPLUGIN_VERSION	MAKELONG(0, 1)

class ITuniacExportPlugin {

public:
	virtual LPTSTR			GetName(void)										= 0;
	virtual void			Destory(void)										= 0;

	virtual unsigned long	GetNumExtensions(void)								= 0;
	virtual LPTSTR			SupportedExtension(unsigned long ulExtentionNum)	= 0;

	virtual bool			CanHandle(LPTSTR szSource)							= 0;

	virtual bool			BeginExport(LPTSTR szSource)						= 0;
	virtual bool			ExportEntry(LibraryEntry & libraryEntry)			= 0;
	virtual bool			EndExport(void)										= 0;
};

typedef ITuniacExportPlugin * (*CreateTuniacExportPluginFunc)(void);
extern "C" __declspec(dllexport) ITuniacExportPlugin * CreateTuniacExportPlugin(void);

typedef unsigned long (*GetTuniacExportPluginVersionFunc)(void);
extern "C" __declspec(dllexport) unsigned long		GetTuniacExportPluginVersion(void);