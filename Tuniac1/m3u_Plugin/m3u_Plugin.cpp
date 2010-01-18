#include "stdafx.h"
#include "m3u_Plugin.h"
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
	ITuniacImportPlugin * pImporter = new CM3U_Import;
	return(pImporter);
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacImportPluginVersion(void)
{
	return ITUNIACIMPORTPLUGIN_VERSION;
}


extern "C" __declspec(dllexport) ITuniacExportPlugin * CreateTuniacExportPlugin(void)
{
	ITuniacExportPlugin * pExporter = new CM3U_Export;
	return(pExporter);
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacExportPluginVersion(void)
{
	return ITUNIACEXPORTPLUGIN_VERSION;
}



CM3U_Import::CM3U_Import(void)
{
}

CM3U_Import::~CM3U_Import(void)
{
}

LPTSTR			CM3U_Import::GetName(void)
{
	return(TEXT("M3U Playlist Importer"));
}

void			CM3U_Import::Destory(void)
{
	delete this;
}

unsigned long	CM3U_Import::GetNumExtensions(void)
{
	return 1;
}

LPTSTR			CM3U_Import::SupportedExtension(unsigned long ulExtentionNum)
{
	if(ulExtentionNum == 0)
		return TEXT(".m3u");
	return NULL;
}

bool			CM3U_Import::CanHandle(LPTSTR szSource)
{
	if(PathIsURL(szSource))
		return false;

	if(StrStrI(PathFindExtension(szSource), TEXT("M3U")))
		return true;

	return false;
}

bool			CM3U_Import::BeginImport(LPTSTR szSource)
{ //TODO: handle remote files
	if(!CanHandle(szSource))
		return false;

	m_File = _wfopen(szSource, TEXT("rS"));

	if(m_File == NULL)
		return false;
	
	StrNCpy(m_BaseDir, szSource, MAX_PATH);
	PathRemoveFileSpec(m_BaseDir);
	PathAddBackslash(m_BaseDir);
	return true;
}

bool			CM3U_Import::ImportUrl(LPTSTR szDest, unsigned long iDestSize)
{
	ZeroMemory(szDest, iDestSize * sizeof(TCHAR));

	if(m_File == NULL)
		return false;

	if(feof(m_File) != 0)
		return false;

	while(1)
	{
		if(fgetws(szDest, iDestSize, m_File) != NULL)
		{
			if(szDest[0] == L'#')
				continue;
			if(wcslen(szDest) < 2)
				continue;

			if(szDest[wcslen(szDest)-1] == L'\n')
				szDest[wcslen(szDest)-1] = L'\0';

			if(!PathIsURL(szDest) && PathIsRelative(szDest))
			{
				TCHAR szTemp[MAX_PATH];
				StrNCpy(szTemp, szDest, MAX_PATH);
				wnsprintf(szDest, iDestSize, TEXT("%s%s"), m_BaseDir, szTemp);
			}
			return true;
		}
		else
		{
			break;
		}
	}

	return false;
}

bool			CM3U_Import::ImportTitle(LPTSTR szDest, unsigned long iDestSize)
{
	return false;
}

bool			CM3U_Import::EndImport(void)
{
	if(m_File == NULL)
		return false;

	fclose(m_File);
	m_File = NULL;
	return true;
}




CM3U_Export::CM3U_Export(void)
{
}

CM3U_Export::~CM3U_Export(void)
{
}

LPTSTR			CM3U_Export::GetName(void)
{
	return(TEXT("M3U Playlist Exporter"));
}

void			CM3U_Export::Destory(void)
{
	delete this;
}

unsigned long	CM3U_Export::GetNumExtensions(void)
{
	return 1;
}

LPTSTR			CM3U_Export::SupportedExtension(unsigned long ulExtentionNum)
{
	if(ulExtentionNum == 0)
		return TEXT(".m3u");
	return NULL;
}

bool			CM3U_Export::CanHandle(LPTSTR szSource)
{
	if(PathIsURL(szSource))
		return false;

	if(StrStrI(PathFindExtension(szSource), TEXT("M3U")))
		return true;

	return false;
}

bool			CM3U_Export::BeginExport(LPTSTR szSource, unsigned long ulNumItems)
{
	if(!CanHandle(szSource))
		return false;

	m_File = _wfopen(szSource, TEXT("w"));

	if(m_File == NULL)
		return false;
	
	m_ExtendedM3U = true;

	StrNCpy(m_BaseDir, szSource, MAX_PATH);
	PathRemoveFileSpec(m_BaseDir);
	PathAddBackslash(m_BaseDir);

	if(m_ExtendedM3U)
	{
		fputws(TEXT("#EXTM3U\n"), m_File);
	}

	return true;
}

bool			CM3U_Export::ExportEntry(LibraryEntry & libraryEntry)
{
	if(m_File == NULL)
		return false;

	TCHAR szData[512];
	LPTSTR pszBase = wcsstr(libraryEntry.szURL, m_BaseDir);
	if(pszBase == libraryEntry.szURL)
		wnsprintf(szData, MAX_PATH, TEXT("%s"), libraryEntry.szURL + wcslen(m_BaseDir));
	else
		wnsprintf(szData, MAX_PATH, TEXT("%s"), libraryEntry.szURL);

	fputws(szData, m_File);
	fputws(TEXT("\n"), m_File);

	if(m_ExtendedM3U)
	{
		int iTime = (libraryEntry.iPlaybackTime == -1) ? (-1) : (libraryEntry.iPlaybackTime / 1000);
		if(wcslen(libraryEntry.szTitle) > 0)
		{
			if(wcslen(libraryEntry.szArtist) > 0)
				wnsprintf(szData, 512, TEXT("#EXTINF:%d,%s - %s"), iTime, libraryEntry.szArtist, libraryEntry.szTitle);
			else if(wcslen(libraryEntry.szTitle) > 0)
				wnsprintf(szData, 512, TEXT("#EXTINF:%d,%s"), iTime, libraryEntry.szTitle);
		}
		else
		{
			wnsprintf(szData, 512, TEXT("#EXTINF:%d,"), iTime);
		}
		fputws(szData, m_File);
		fputws(TEXT("\n"), m_File);

	}

	return true;
}

bool			CM3U_Export::EndExport(void)
{
	if(m_File == NULL)
		return false;

	fclose(m_File);
	m_File = NULL;
	return true;

}

