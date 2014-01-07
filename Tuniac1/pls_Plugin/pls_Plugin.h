#pragma once

#include "ITuniacImportPlugin.h"
#include "ITuniacExportPlugin.h"
#include "stdio.h"
#include "shlwapi.h"
#include <strsafe.h>

class CPLS_Import :
	public ITuniacImportPlugin
{

protected:
	int				m_Current;
	TCHAR			m_Source[MAX_PATH];
	TCHAR			m_BaseDir[MAX_PATH];
	bool			m_StartedImport;

public:
	CPLS_Import(void);
	~CPLS_Import(void);

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


class CPLS_Export :
	public ITuniacExportPlugin
{

protected:
	int				m_Current;
	TCHAR			m_Source[512];
	TCHAR			m_BaseDir[512];
	bool			m_StartedExport;

	bool			m_Version2;

public:
	CPLS_Export(void);
	~CPLS_Export(void);

	LPTSTR			GetName(void);
	void			Destory(void);

	unsigned long	GetNumExtensions(void);
	LPTSTR			SupportedExtension(unsigned long ulExtentionNum);

	bool			CanHandle(LPTSTR szSource);

	bool			BeginExport(LPTSTR szSource, unsigned long ulNumItems);
	bool			ExportEntry(LibraryEntry & libraryEntry);
	bool			EndExport(void);
};
