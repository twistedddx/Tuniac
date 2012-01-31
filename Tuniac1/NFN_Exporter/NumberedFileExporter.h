#pragma once

#include "ITuniacExportPlugin.h"
#include <stdio.h>

class CNumberedFileExporter :
	public ITuniacExportPlugin
{
protected:

	TCHAR			m_szExportFolder[MAX_PATH];

	unsigned long	m_ulCurrentFileIndex;

public:
	CNumberedFileExporter(void);
	~CNumberedFileExporter(void);

public:
	LPTSTR			GetName(void);
	void			Destory(void);

	unsigned long	GetNumExtensions(void);
	LPTSTR			SupportedExtension(unsigned long ulExtentionNum);

	bool			CanHandle(LPTSTR szSource);

	bool			BeginExport(LPTSTR szSource, unsigned long ulNumItems);
	bool			ExportEntry(LibraryEntry & libraryEntry);
	bool			EndExport(void);
};
