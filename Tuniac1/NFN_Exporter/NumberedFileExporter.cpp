#include "StdAfx.h"
#include "NumberedFileExporter.h"
#include "shlwapi.h"

CNumberedFileExporter::CNumberedFileExporter(void)
{
}

CNumberedFileExporter::~CNumberedFileExporter(void)
{
}

LPTSTR			CNumberedFileExporter::GetName(void)
{
	return TEXT("Numbered File Exporter");
}

void			CNumberedFileExporter::Destory(void)
{
	delete this;
}


unsigned long	CNumberedFileExporter::GetNumExtensions(void)
{
	return 1;
}

LPTSTR			CNumberedFileExporter::SupportedExtension(unsigned long ulExtentionNum)
{
	return TEXT("NFN");
}


bool			CNumberedFileExporter::CanHandle(LPTSTR szSource)
{
	TCHAR * pLastDigits = szSource + (lstrlen(szSource)-3);
	if(StrStrI(pLastDigits, TEXT("NFN")))
		return true;

	return false;
}


bool			CNumberedFileExporter::BeginExport(LPTSTR szSource, unsigned long ulNumItems)
{
	StrCpyN(m_szExportFolder, szSource, (lstrlen(szSource)-2));

	if(GetFileAttributes(m_szExportFolder) == INVALID_FILE_ATTRIBUTES)
	{
		// folder doesn't exist, create it.
		CreateDirectory(m_szExportFolder, NULL);
	}

	m_ulCurrentFileIndex = 1;

	return true;
}

bool			CNumberedFileExporter::ExportEntry(LibraryEntry & libraryEntry)
{
	TCHAR			m_szTempFile[MAX_PATH];

	StrCpy(m_szTempFile, m_szExportFolder);
	PathAddBackslash(m_szTempFile);

	TCHAR tempBit[32];

	_snwprintf(tempBit, 32, TEXT("%04d - "), m_ulCurrentFileIndex);
	StrCat(m_szTempFile, tempBit);

	StrCat(m_szTempFile, PathFindFileName(libraryEntry.szURL));


	CopyFile(libraryEntry.szURL, m_szTempFile, TRUE);


	m_ulCurrentFileIndex++;
	return true;
}

bool			CNumberedFileExporter::EndExport(void)
{
	return false;
}

