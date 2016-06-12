#pragma once

#include "ITuniacExportPlugin.h"
#include <cstdio>
#include "shlwapi.h"
#include <strsafe.h>

class CtextOutput_Export :
	public ITuniacExportPlugin
{

protected:
	TCHAR			m_PreviousArtist[128];
	TCHAR			m_PreviousAlbum[128];
	FILE *			m_File;
	unsigned long	m_Current;

public:
	CtextOutput_Export(void);
	~CtextOutput_Export(void);

	LPTSTR			GetName(void);
	void			Destory(void);

	unsigned long	GetNumExtensions(void);
	LPTSTR			SupportedExtension(unsigned long ulExtentionNum);

	bool			CanHandle(LPTSTR szSource);

	bool			BeginExport(LPTSTR szSource, unsigned long ulNumItems);
	bool			ExportEntry(LibraryEntry & libraryEntry);
	bool			EndExport(void);
};
