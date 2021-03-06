#pragma once

#include "ITuniacImportPlugin.h"
#include "ITuniacExportPlugin.h"


class CM3U_Import :
	public ITuniacImportPlugin
{

protected:
	TCHAR			m_szBaseDir[MAX_PATH];

	FILE *			m_File;
	TCHAR			szTitle[128];

public:
	CM3U_Import(void);
	~CM3U_Import(void);

	LPTSTR			GetName(void);
	void			Destory(void);

	unsigned long	GetNumExtensions(void);
	LPTSTR			SupportedExtension(unsigned long ulExtentionNum);

	bool			CanHandle(LPTSTR szSource);

	bool			BeginImport(LPTSTR szSource);
	bool			ImportUrl(LPTSTR szDest, unsigned long iDestSize);
	bool			ImportTitle(LPTSTR szDest, unsigned long iDestSize);
	bool			EndImport(void);
};


class CM3U_Export :
	public ITuniacExportPlugin
{

protected:
	TCHAR			m_BaseDir[MAX_PATH];
	FILE *			m_File;

public:
	CM3U_Export(void);
	~CM3U_Export(void);

	LPTSTR			GetName(void);
	void			Destory(void);

	unsigned long	GetNumExtensions(void);
	LPTSTR			SupportedExtension(unsigned long ulExtentionNum);

	bool			CanHandle(LPTSTR szSource);

	bool			BeginExport(LPTSTR szSource, unsigned long ulNumItems);
	bool			ExportEntry(LibraryEntry & libraryEntry);
	bool			EndExport(void);
};
