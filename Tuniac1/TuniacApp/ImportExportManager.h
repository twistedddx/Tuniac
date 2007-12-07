#pragma once

#include "IPlaylist.h"

#include "ITuniacImportPlugin.h"
#include "ITuniacExportPlugin.h"


class CImportExportManager
{
protected:

	typedef struct
	{
		HINSTANCE				hDLL;
		ITuniacImportPlugin *	pImporter;
		ITuniacExportPlugin *	pExporter;
	} PluginEntry;
	Array<PluginEntry, 3>		m_PluginArray;

	typedef struct
	{
		ITuniacExportPlugin *	pExporter;
		LPTSTR					szExt;
	} ExportExtension;
	Array<ExportExtension, 3>	m_ExportExtensions;

	bool			ImportFrom(ITuniacImportPlugin * pImporter, LPTSTR szSource); 
	bool			ExportTo(ITuniacExportPlugin * pExporter, LPTSTR szSource, EntryArray & entryArray);

public:
	CImportExportManager(void);
	~CImportExportManager(void);

	bool			Initialize(void);
	bool			Shutdown(void);

	ITuniacImportPlugin *	GetImporterAtIndex(unsigned long iImporter);
	ITuniacExportPlugin *	GetExporterAtIndex(unsigned long iExporter);

	unsigned long	NumExportExtensions(void);
	LPTSTR			GetExportExtension(unsigned long ulIndex);
	void			PopuplateExportMenu(HMENU hMenu, unsigned long ulBase);

	bool			CanImport(LPTSTR szSource);
	bool			CanExport(LPTSTR szSource);

	bool			Import(LPTSTR szSource);
	
	bool			Export(EntryArray & entryArray, LPTSTR szSource);

};
