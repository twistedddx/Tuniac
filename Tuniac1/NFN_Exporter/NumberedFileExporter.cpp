#include "StdAfx.h"
#include "NumberedFileExporter.h"


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
	TCHAR * pLastDigits = szSource + (wcsnlen_s(szSource, MAX_PATH)-3);
	if(StrStrI(pLastDigits, TEXT("NFN")))
		return true;

	return false;
}


bool			CNumberedFileExporter::BeginExport(LPTSTR szSource, unsigned long ulNumItems)
{
	if (IDNO == MessageBox(NULL, TEXT("Warning: NFN_Exporter plugin creates a copy of the selected files. Eg it duplicates the files! \n\nDo you want to continue?"), TEXT("NFN_Exporter"), MB_YESNO | MB_ICONWARNING))
		return false;

	StringCchCopy(m_szExportFolder, MAX_PATH, szSource);

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

	StringCchCopy(m_szTempFile, MAX_PATH, m_szExportFolder);
	PathAddBackslash(m_szTempFile);

	TCHAR tempBit[32];

	StringCchPrintf(tempBit, 32, TEXT("%04u - "), m_ulCurrentFileIndex);
	StringCchCat(m_szTempFile, MAX_PATH, tempBit);

	StringCchCat(m_szTempFile, MAX_PATH, PathFindFileName(libraryEntry.szURL));


	CopyFile(libraryEntry.szURL, m_szTempFile, TRUE);


	m_ulCurrentFileIndex++;
	return true;
}

bool			CNumberedFileExporter::EndExport(void)
{
	return false;
}

