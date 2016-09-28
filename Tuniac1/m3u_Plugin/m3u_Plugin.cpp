#include "stdafx.h"
#include "m3u_Plugin.h"

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

	if (ulExtentionNum == 1)
		return TEXT(".m3u8");

	return NULL;
}

bool			CM3U_Import::CanHandle(LPTSTR szSource)
{
	if(PathIsURL(szSource))
		return false;

	if (StrStrI(PathFindExtension(szSource), TEXT("M3U8")) || StrStrI(PathFindExtension(szSource), TEXT("M3U")))
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

	StringCchCopy(m_szBaseDir, MAX_PATH, szSource);
	PathRemoveFileSpec(m_szBaseDir);
	PathAddBackslash(m_szBaseDir);

	return true;
}

bool			CM3U_Import::ImportUrl(LPTSTR szDest, unsigned long iDestSize)
{
	ZeroMemory(szDest, iDestSize * sizeof(TCHAR));
	ZeroMemory(szTitle, 128 * sizeof(TCHAR));

	if(m_File == NULL)
		return false;

	if(feof(m_File) != 0)
		return false;

	while(1)
	{
		if (fgetws(szDest, iDestSize, m_File) != NULL)
		{
			if (wcsnlen_s(szDest, iDestSize) < 2)
				continue;

			if (!_wcsnicmp(szDest, L"#EXTM3U", 7))
				continue;

			if (!_wcsnicmp(szDest, L"#EXT-X-", 7))
				continue;

			if (!_wcsnicmp(szDest, L"#EXTINF:", 8))
			{
				TCHAR * szTemp = wcsstr(szDest, L",");

				if (szTemp)
				{
					int iLen = wcsnlen_s(szTemp, iDestSize);

					if (szTemp[iLen - 1] == L'\n')
						szTemp[iLen - 1] = L'\0';

					StringCchCopyN(szTitle, 128, &szTemp[1], 128);
				}
				continue;
			}

			if (szDest[wcsnlen_s(szDest, iDestSize) - 1] == L'\n')
				szDest[wcsnlen_s(szDest, iDestSize) - 1] = L'\0';

			if (!PathIsURL(szDest) && PathIsRelative(szDest))
			{
				TCHAR szTemp[MAX_PATH];
				StringCchCopyN(szTemp, 128, szDest, 128);
				StringCchPrintf(szDest, iDestSize, TEXT("%s%s"), m_szBaseDir, szTemp);
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
	if(szTitle != L"")
	{
		StringCchCopyN(szDest, iDestSize, &szTitle[0], iDestSize);
		return true;
	}

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
	
	StringCchCopy(m_BaseDir, MAX_PATH, szSource);
	PathRemoveFileSpec(m_BaseDir);
	PathAddBackslash(m_BaseDir);

	fputws(TEXT("#EXTM3U\n"), m_File);

	return true;
}

bool			CM3U_Export::ExportEntry(LibraryEntry & libraryEntry)
{
	if(m_File == NULL)
		return false;

	TCHAR szData[MAX_PATH];
	unsigned long ulTime = (libraryEntry.ulPlaybackTime == LENGTH_UNKNOWN || libraryEntry.ulPlaybackTime == LENGTH_STREAM) ? (-1) : (libraryEntry.ulPlaybackTime / 1000);
	if(wcsnlen_s(libraryEntry.szTitle, 128) > 0)
	{
		if(wcsnlen_s(libraryEntry.szArtist, 128) > 0)
			StringCchPrintf(szData, MAX_PATH, TEXT("#EXTINF:%u,%s - %s"), ulTime, libraryEntry.szArtist, libraryEntry.szTitle);
		else if(wcsnlen_s(libraryEntry.szTitle,128) > 0)
			StringCchPrintf(szData, MAX_PATH, TEXT("#EXTINF:%u,%s"), ulTime, libraryEntry.szTitle);
	}
	else
	{
		StringCchPrintf(szData, MAX_PATH, TEXT("#EXTINF:%u,"), ulTime);
	}
	fputws(szData, m_File);
	fputws(TEXT("\n"), m_File);

	LPTSTR pszBase = wcsstr(libraryEntry.szURL, m_BaseDir);
	if(pszBase == libraryEntry.szURL)
		StringCchCopy(szData, MAX_PATH, libraryEntry.szURL + wcsnlen_s(m_BaseDir, MAX_PATH));
	else
		StringCchCopy(szData, MAX_PATH, libraryEntry.szURL);

	fputws(szData, m_File);
	fputws(TEXT("\n"), m_File);


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

