#include "stdafx.h"
#include "pls_Plugin.h"
#include "shlwapi.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


extern "C" __declspec(dllexport) ITuniacImportPlugin * CreateTuniacImportPlugin(void)
{
	ITuniacImportPlugin * pImporter = new CPLS_Import;
	return(pImporter);
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacImportPluginVersion(void)
{
	return ITUNIACIMPORTPLUGIN_VERSION;
}


extern "C" __declspec(dllexport) ITuniacExportPlugin * CreateTuniacExportPlugin(void)
{
	ITuniacExportPlugin * pExporter = new CPLS_Export;
	return(pExporter);
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacExportPluginVersion(void)
{
	return ITUNIACEXPORTPLUGIN_VERSION;
}


CPLS_Import::CPLS_Import(void)
{
}

CPLS_Import::~CPLS_Import(void)
{
}

LPTSTR			CPLS_Import::GetName(void)
{
	return(TEXT("PLS Playlist Importer"));
}

void			CPLS_Import::Destory(void)
{
	delete this;
}

unsigned long	CPLS_Import::GetNumExtensions(void)
{
	return 1;
}

LPTSTR			CPLS_Import::SupportedExtension(unsigned long ulExtentionNum)
{
	if(ulExtentionNum == 0)
		return TEXT(".pls");
	return NULL;
}

bool			CPLS_Import::CanHandle(LPTSTR szSource)
{
	if(PathIsURL(szSource))
		return false;

	if(StrStrI(PathFindExtension(szSource), TEXT("PLS")))
		return true;

	return false;
}

bool			CPLS_Import::BeginImport(LPTSTR szSource)
{
	if(!CanHandle(szSource))
		return false;

	if(!PathFileExists(szSource))
		return false;

	m_Current = 0;
	StrNCpy(m_Source, szSource, MAX_PATH);
	StrNCpy(m_BaseDir, m_Source, MAX_PATH);
	PathRemoveFileSpec(m_BaseDir);
	PathAddBackslash(m_BaseDir);

	m_StartedImport = true;
	return true;
}

bool			CPLS_Import::ImportUrl(LPTSTR szDest, unsigned long iDestSize)
{
	ZeroMemory(szDest, iDestSize * sizeof(TCHAR));
	m_Current++;


	if(!m_StartedImport)
		return false;

	TCHAR szBuffer[MAX_PATH];
	TCHAR szVarName[32];
	_snwprintf(szVarName, 32, L"File%d", m_Current);
	GetPrivateProfileString(L"playlist", szVarName, NULL, szBuffer, MAX_PATH, m_Source);

	if(szBuffer && wcslen(szBuffer) > 2)
	{

		if(szDest[wcslen(szDest)-1] == L'\n')
			szDest[wcslen(szDest)-1] = L'\0';

		if(!PathIsURL(szBuffer) && PathIsRelative(szBuffer))
		{
			_snwprintf(szDest, iDestSize, L"%s%s", m_BaseDir, szBuffer);
		}
		else
		{
			StrNCpy(szDest, szBuffer, iDestSize);
		}
		return true;
	}
	return false;
}

bool			CPLS_Import::ImportTitle(LPTSTR szDest, unsigned long iDestSize)
{
	ZeroMemory(szDest, iDestSize * sizeof(TCHAR));

	TCHAR szVarName[32] = L"";
	_snwprintf(szVarName, 32, L"Title%d", m_Current);
	GetPrivateProfileString(L"playlist", szVarName, NULL, szDest, iDestSize, m_Source);

	if(wcslen(szDest) > 0)
		return true;

	return false;
}

bool			CPLS_Import::EndImport(void)
{
	if(!m_StartedImport)
		return false;

	m_StartedImport = false;
	return true;
}



CPLS_Export::CPLS_Export(void)
{
}

CPLS_Export::~CPLS_Export(void)
{
}

LPTSTR			CPLS_Export::GetName(void)
{
	return(TEXT("PLS Playlist Exporter"));
}

void			CPLS_Export::Destory(void)
{
	delete this;
}

unsigned long	CPLS_Export::GetNumExtensions(void)
{
	return 1;
}

LPTSTR			CPLS_Export::SupportedExtension(unsigned long ulExtentionNum)
{
	if(ulExtentionNum == 0)
		return TEXT(".pls");
	return NULL;
}

bool			CPLS_Export::CanHandle(LPTSTR szSource)
{
	if(PathIsURL(szSource))
		return false;

	if(StrStrI(PathFindExtension(szSource), TEXT("PLS")))
		return true;

	return false;
}

bool			CPLS_Export::BeginExport(LPTSTR szSource, unsigned long ulNumItems)
{
	if(!CanHandle(szSource))
		return false;

	if(PathFileExists(szSource) == TRUE && DeleteFile(szSource) == 0)
		return false;

	m_Current = 0;
	m_Version2 = true;

	StrNCpy(m_Source, szSource, MAX_PATH);
	StrNCpy(m_BaseDir, m_Source, MAX_PATH);
	PathRemoveFileSpec(m_BaseDir);
	PathAddBackslash(m_BaseDir);

	m_StartedExport = true;
	return true;
}

bool			CPLS_Export::ExportEntry(LibraryEntry & libraryEntry)
{
	if(!m_StartedExport)
		return false;
	
	TCHAR szVarName[32];
	_snwprintf(szVarName, 32, L"File%d", ++m_Current);

	TCHAR szData[MAX_PATH];
	LPTSTR pszBase = wcsstr(libraryEntry.szURL, m_BaseDir);
	if(pszBase == libraryEntry.szURL)
		StrCpyN(szData, libraryEntry.szURL + wcslen(m_BaseDir), MAX_PATH);
	else
		StrCpyN(szData, libraryEntry.szURL, MAX_PATH);

	if(WritePrivateProfileString(L"playlist", szVarName, szData, m_Source) == 0)
		return false;

	if(!m_Version2) return true;

	_snwprintf(szVarName, 32, L"Title%d", m_Current);
	if(wcslen(libraryEntry.szTitle) > 0)
	{
		if(wcslen(libraryEntry.szArtist) > 0)
			_snwprintf(szData, 512, L"%s - %s", libraryEntry.szArtist, libraryEntry.szTitle);
		else if(wcslen(libraryEntry.szTitle) > 0)
			_snwprintf(szData, 512, L"%s", libraryEntry.szTitle);
	}
	else
	{
		_snwprintf(szData, 512, TEXT(""));
	}
	if(WritePrivateProfileString(L"playlist", szVarName, szData, m_Source) == 0)
		return false;

	_snwprintf(szVarName, 32, L"Length%d", m_Current);
	_snwprintf(szData, 512, L"%d", (libraryEntry.iPlaybackTime == -1) ? (-1) : (libraryEntry.iPlaybackTime / 1000));
	if(WritePrivateProfileString(L"playlist", szVarName, szData, m_Source) == 0)
		return false;
	
	return true;
}

bool			CPLS_Export::EndExport(void)
{
	if(!m_StartedExport)
		return false;

	if(m_Version2)
	{

		TCHAR szNumEntries[32];
		_snwprintf(szNumEntries, 32, L"%d", m_Current);

		WritePrivateProfileString(L"playlist", L"NumberOfEntries", szNumEntries, m_Source);
		WritePrivateProfileString(L"playlist", L"Version", L"2", m_Source);
	}

	m_StartedExport = false;
	return true;
}
