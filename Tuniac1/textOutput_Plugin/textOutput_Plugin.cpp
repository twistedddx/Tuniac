#include "stdafx.h"
#include "textOutput_Plugin.h"


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


extern "C" __declspec(dllexport) ITuniacExportPlugin * CreateTuniacExportPlugin(void)
{
	ITuniacExportPlugin * pExporter = new CtextOutput_Export;
	return(pExporter);
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacExportPluginVersion(void)
{
	return ITUNIACEXPORTPLUGIN_VERSION;
}


CtextOutput_Export::CtextOutput_Export(void)
{
}

CtextOutput_Export::~CtextOutput_Export(void)
{
}

LPTSTR			CtextOutput_Export::GetName(void)
{
	return(TEXT("Text Output Playlist Exporter"));
}

void			CtextOutput_Export::Destory(void)
{
	delete this;
}

unsigned long	CtextOutput_Export::GetNumExtensions(void)
{
	return 1;
}

LPTSTR			CtextOutput_Export::SupportedExtension(unsigned long ulExtentionNum)
{
	if(ulExtentionNum == 0)
		return TEXT(".txt");
	return NULL;
}

bool			CtextOutput_Export::CanHandle(LPTSTR szSource)
{
	if(PathIsURL(szSource))
		return false;

	if(StrStrI(PathFindExtension(szSource), TEXT("TXT")))
		return true;

	return false;
}

bool			CtextOutput_Export::BeginExport(LPTSTR szSource, unsigned long ulNumItems)
{
	if (!CanHandle(szSource))
		return false;

	m_File = _wfopen(szSource, TEXT("w"));

	if (m_File == NULL)
		return false;
}

bool			CtextOutput_Export::ExportEntry(LibraryEntry & libraryEntry)
{
	if (m_File == NULL)
		return false;

	TCHAR szData[512];

	if (lstrcmpi(m_PreviousAlbum, libraryEntry.szAlbum) != 0)
	{
		m_Current++;
		StringCchPrintf(szData, 512, TEXT("%u: %s - %s - %u"), m_Current, libraryEntry.szArtist, libraryEntry.szAlbum, libraryEntry.ulYear);
		StringCchCopy(m_PreviousAlbum, MAX_PATH, libraryEntry.szAlbum);
		StringCchCopy(m_PreviousArtist, MAX_PATH, libraryEntry.szArtist);
		fputws(szData, m_File);
		fputws(TEXT("\n"), m_File);
	}
	else if (lstrcmpi(m_PreviousArtist, libraryEntry.szArtist) != 0)
	{
		m_Current++;
		StringCchPrintf(szData, 512, TEXT("%u: %s - %s - %u"), m_Current, libraryEntry.szArtist, libraryEntry.szAlbum, libraryEntry.ulYear);
		StringCchCopy(m_PreviousAlbum, MAX_PATH, libraryEntry.szAlbum);
		StringCchCopy(m_PreviousArtist, MAX_PATH, libraryEntry.szArtist);
		fputws(szData, m_File);
		fputws(TEXT("\n"), m_File);
	}

	return true;
}

bool			CtextOutput_Export::EndExport(void)
{
	if (m_File == NULL)
		return false;

	fclose(m_File);
	m_File = NULL;
	return true;
}
