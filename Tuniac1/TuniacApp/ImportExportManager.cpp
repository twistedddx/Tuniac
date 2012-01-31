/*
	Copyright (C) 2003-2008 Tony Million

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation is required.

	2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.
*/

#include "stdafx.h"
#include "ImportExportManager.h"
#include "LibraryEntry.h"

CImportExportManager::CImportExportManager(void)
{
}

CImportExportManager::~CImportExportManager(void)
{
}


bool			CImportExportManager::Initialize(void)
{
	WIN32_FIND_DATA		w32fd;
	HANDLE				hFind;
	TCHAR				szBase[MAX_PATH];
	TCHAR				szSearchFilename[MAX_PATH];

	GetModuleFileName(NULL, szBase, MAX_PATH);
	PathRemoveFileSpec(szBase);
	PathAddBackslash(szBase);
	StrCat(szBase, TEXT("importexport"));
	PathAddBackslash(szBase);
	
	StrCpy(szSearchFilename, szBase);
	StrCat(szSearchFilename, TEXT("*.dll"));

	hFind = FindFirstFile(szSearchFilename, &w32fd); 
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			if(StrCmp(w32fd.cFileName, TEXT(".")) == 0 || StrCmp(w32fd.cFileName, TEXT("..")) == 0 )
				continue;

			TCHAR szFilename[MAX_PATH];
			StrCpy(szFilename, szBase);
			StrCat(szFilename, w32fd.cFileName);

			HINSTANCE hDLL = LoadLibrary(szFilename);
			if(hDLL)
			{
				bool bUsed = false;
				PluginEntry PE;
				PE.pImporter = NULL;
				PE.pExporter = NULL;


				GetTuniacImportPluginVersionFunc pGTIPVF = (GetTuniacImportPluginVersionFunc)GetProcAddress(hDLL, "GetTuniacImportPluginVersion");
				if(pGTIPVF == NULL || pGTIPVF() != ITUNIACIMPORTPLUGIN_VERSION)
				{
					if(pGTIPVF != NULL)
					{
						TCHAR szError[512];
						_snwprintf(szError, 512, TEXT("Incompatable import plugin found: %s\n\nThis Plugin must be updated before you can use it."), w32fd.cFileName);
						MessageBox(tuniacApp.getMainWindow(), szError, TEXT("Error"), MB_OK | MB_ICONWARNING);
					}
				}
				else
				{

					CreateTuniacImportPluginFunc pCTIPF = (CreateTuniacImportPluginFunc)GetProcAddress(hDLL, "CreateTuniacImportPlugin");
					if(pCTIPF)
					{
						ITuniacImportPlugin * pImportPlugin = pCTIPF();

						if(pImportPlugin)
						{
							PE.pImporter = pImportPlugin;
							bUsed = true;
						}
					}
				}

				GetTuniacExportPluginVersionFunc pGTEPVF = (GetTuniacExportPluginVersionFunc)GetProcAddress(hDLL, "GetTuniacExportPluginVersion");
				if(pGTEPVF == NULL || pGTEPVF() != ITUNIACEXPORTPLUGIN_VERSION)
				{
					if(pGTEPVF != NULL)
					{
						TCHAR szError[512];
						_snwprintf(szError, 512, TEXT("Incompatable export plugin found: %s\n\nThis Plugin must be updated before you can use it."), w32fd.cFileName);
						MessageBox(tuniacApp.getMainWindow(), szError, TEXT("Error"), MB_OK | MB_ICONWARNING);
					}
				}
				else
				{
					CreateTuniacExportPluginFunc pCTEPF = (CreateTuniacExportPluginFunc)GetProcAddress(hDLL, "CreateTuniacExportPlugin");
					if(pCTEPF)
					{
						ITuniacExportPlugin * pExportPlugin = pCTEPF();

						if(pExportPlugin)
						{
							PE.pExporter = pExportPlugin;
							bUsed = true;
							for(unsigned long i = 0; i < PE.pExporter->GetNumExtensions(); i++)
							{
								ExportExtension EE;
								EE.szExt = pExportPlugin->SupportedExtension(i);
								if(EE.szExt == NULL) continue;
								EE.pExporter = pExportPlugin;
								m_ExportExtensions.AddTail(EE);
							}
						}
					}
				}

				if(bUsed)
				{
					PE.hDLL = hDLL;
					m_PluginArray.AddTail(PE);
				}
				else
				{
					FreeLibrary(hDLL);
				}

			}

		} while(FindNextFile( hFind, &w32fd));

		FindClose(hFind); 
	}

	return true;
}

bool			CImportExportManager::Shutdown(void)
{
	while(m_PluginArray.GetCount())
	{
		if(m_PluginArray[0].pImporter)
			m_PluginArray[0].pImporter->Destory();

		if(m_PluginArray[0].pExporter)
			m_PluginArray[0].pExporter->Destory();

		FreeLibrary(m_PluginArray[0].hDLL);
		m_PluginArray.RemoveAt(0);
	}
	return true;
}

ITuniacImportPlugin *	CImportExportManager::GetImporterAtIndex(unsigned long iImporter)
{
	int iCount = 0;
	for(unsigned long i = 0; i < m_PluginArray.GetCount(); i++)
	{
		if(m_PluginArray[i].pImporter == NULL)
			continue;
		if(iCount == iImporter)
			return m_PluginArray[i].pImporter;
		iCount++;
	}
	return NULL;
}

ITuniacExportPlugin *	CImportExportManager::GetExporterAtIndex(unsigned long iExporter)
{
	int iCount = 0;
	for(unsigned long i = 0; i < m_PluginArray.GetCount(); i++)
	{
		if(m_PluginArray[i].pExporter == NULL)
			continue;
		if(iCount == iExporter)
			return m_PluginArray[i].pExporter;
		iCount++;
	}
	return NULL;
}

unsigned long	CImportExportManager::NumExportExtensions(void)
{
	return m_ExportExtensions.GetCount();
}

LPTSTR			CImportExportManager::GetExportExtension(unsigned long ulIndex)
{
	if(ulIndex >= m_ExportExtensions.GetCount())
		return NULL;
	return m_ExportExtensions[ulIndex].szExt;
}

void			CImportExportManager::PopuplateExportMenu(HMENU hMenu, unsigned long ulBase)
{
	while(GetMenuItemCount(hMenu))
	{
		DeleteMenu(hMenu, 0, MF_BYPOSITION);
	}

	TCHAR szItem[128];
	for(unsigned long i = 0; i < m_ExportExtensions.GetCount(); i++)
	{
		_snwprintf(szItem, 128, TEXT("%s - %s"), m_ExportExtensions[i].pExporter->GetName(), m_ExportExtensions[i].szExt);
		AppendMenu(hMenu, MF_STRING, ulBase + i, szItem);
	}
}

bool			CImportExportManager::CanImport(LPTSTR szSource)
{
	for(unsigned long i = 0; i < m_PluginArray.GetCount(); i++)
	{
		if(m_PluginArray[i].pImporter == NULL)
			continue;
		if(m_PluginArray[i].pImporter->CanHandle(szSource))
			return true;
	}
	return false;
}

bool			CImportExportManager::CanExport(LPTSTR szSource)
{
	for(unsigned long i = 0; i < m_PluginArray.GetCount(); i++)
	{
		if(m_PluginArray[i].pExporter == NULL)
			continue;
		if(m_PluginArray[i].pExporter->CanHandle(szSource))
			return true;
	}
	return false;
}

bool			CImportExportManager::Import(LPTSTR szSource)
{
	for(unsigned long i = 0; i < m_PluginArray.GetCount(); i++)
	{
		if(m_PluginArray[i].pImporter == NULL)
			continue;

		if(m_PluginArray[i].pImporter->CanHandle(szSource))
			return ImportFrom(m_PluginArray[i].pImporter, szSource);
	}
	return false;
}

bool			CImportExportManager::ImportFrom(ITuniacImportPlugin * pImporter, LPTSTR szSource)
{
	bool bOK = false;
	bool bStreamList = false;
	bool bAutoAddPlaylists = tuniacApp.m_Preferences.GetAutoAddPlaylists();
	bool bAddSingleStream = tuniacApp.m_Preferences.GetAddSingleStream();

	CMediaLibraryPlaylistEntry * pStreamEntry;
	TCHAR szFilename[MAX_PATH] = L"";;

	if(pImporter->BeginImport(szSource) && pImporter->ImportUrl(szFilename, MAX_PATH))
	{
		bOK = true;
		IPlaylistEntry	*	pPE = NULL;
		EntryArray playlistEntries;

		do
		{
			tuniacApp.m_MediaLibrary.AddItem(szFilename);
			pPE = tuniacApp.m_MediaLibrary.GetEntryByURL(szFilename);

			if(pPE)
			{
				if(bAutoAddPlaylists)
					playlistEntries.AddTail(pPE);

				bStreamList = PathIsURL(szFilename);
				if(bStreamList) //auto fill details for streams
				{
					TCHAR szTitle[128] = L"";
					if(pImporter->ImportTitle(szTitle, 128))
					{
						if(wcscmp((LPTSTR)pPE->GetField(FIELD_TITLE), L"") == 0)
							pPE->SetField(FIELD_TITLE, szTitle);
						if(wcscmp((LPTSTR)pPE->GetField(FIELD_COMMENT), L"") == 0)
							pPE->SetField(FIELD_COMMENT, szTitle);
					}
					else
					{
						if(wcscmp((LPTSTR)pPE->GetField(FIELD_TITLE), L"") == 0)
							pPE->SetField(FIELD_TITLE, L"[Unknown Stream]");
					}
					if(bAddSingleStream) //pls and m3u contain numerous mirrors of the same stream normally. Normal behaviour is to only add the first
						break;
				}
			}
		} while(pImporter->ImportUrl(szFilename, MAX_PATH));

		if(bAutoAddPlaylists && !bAddSingleStream) //create a Tuniac playlist of this playlist file except when single adding streams
		{
			TCHAR	szFileTitle[128];
			GetFileTitle(szSource, szFileTitle, 128);
			tuniacApp.m_PlaylistManager.CreateNewStandardPlaylistWithIDs(szFileTitle, playlistEntries);
			tuniacApp.m_SourceSelectorWindow->UpdateList();
		}
		pImporter->EndImport();
	}
	return bOK;
}

bool			CImportExportManager::Export(EntryArray & entryArray, LPTSTR szSource)
{
	TCHAR szFilename[MAX_PATH];
	ZeroMemory(&szFilename, sizeof(TCHAR) * MAX_PATH);
	if(szSource == NULL)
	{

		TCHAR szFilterText[128];
		TCHAR szFilterExt[16];
		LPTSTR szFilters;
		if((szFilters = (LPTSTR)malloc(sizeof(TCHAR))) == NULL) return false;
		int nSize = 1;
		LPTSTR pDest = szFilters;
		void * pTemp;

		for(unsigned long i = 0; i < m_ExportExtensions.GetCount(); i++)
		{
			
			_snwprintf(szFilterText, 128, TEXT("%s (*%s)"), m_ExportExtensions[i].pExporter->GetName(), m_ExportExtensions[i].szExt);
			_snwprintf(szFilterExt, 16, TEXT("*%s"), m_ExportExtensions[i].szExt);
			if((pTemp = realloc((void *)szFilters, (nSize + wcslen(szFilterText) + wcslen(szFilterExt) + 2) * sizeof(TCHAR))) == NULL) break;
			szFilters = (LPTSTR)pTemp;

			pDest = szFilters + (nSize - 1);
			nSize += wcslen(szFilterText) + 1;
			memcpy((void *)pDest, (void *)szFilterText, (wcslen(szFilterText) + 1) * sizeof(TCHAR));

			pDest = szFilters + (nSize - 1);
			nSize += wcslen(szFilterExt) + 1;
			memcpy((void *)pDest, (void *)szFilterExt, (wcslen(szFilterExt) + 1) * sizeof(TCHAR));

		}
		pDest = szFilters + nSize - 1;
		*pDest = L'\0';

		OPENFILENAME ofn;
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = szFilename;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrFilter = szFilters;
		ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
		ofn.hwndOwner = tuniacApp.getMainWindow();

		if(GetSaveFileName(&ofn) == 0)
			return false;


		StrCpyN(szFilename, ofn.lpstrFile, MAX_PATH);

		if(ofn.nFilterIndex >= 1 && ofn.nFilterIndex <= m_ExportExtensions.GetCount())
		{
			if(!CanExport(szFilename))
				StrCatN(szFilename, m_ExportExtensions[ofn.nFilterIndex - 1].szExt, MAX_PATH);
		}
		szSource = szFilename;

		free((void *)szFilters);
	}

	for(unsigned long i = 0; i < m_PluginArray.GetCount(); i++)
	{
		if(m_PluginArray[i].pExporter == NULL)
			continue;
		if(m_PluginArray[i].pExporter->CanHandle(szSource))
			return ExportTo(m_PluginArray[i].pExporter, szSource, entryArray);
	}
	MessageBox(tuniacApp.getMainWindow(), TEXT("Error exporting playlist."), TEXT("Tuniac"), MB_OK | MB_ICONWARNING);
	return false;
}

bool			CImportExportManager::ExportTo(ITuniacExportPlugin * pExporter, LPTSTR szSource, EntryArray & entryArray)
{
	if(pExporter->BeginExport(szSource, entryArray.GetCount()))
	{
		LibraryEntry LE;
		CMediaLibraryPlaylistEntry * pIPE;

		for(unsigned long i = 0; i < entryArray.GetCount(); i++)
		{
			pIPE = tuniacApp.m_MediaLibrary.GetEntryByEntryID(entryArray[i]->GetEntryID());
			if(pIPE)
			{
				CopyMemory(&LE, pIPE->GetLibraryEntry(), sizeof(LE));
				pExporter->ExportEntry(LE);
			}
		}

		pExporter->EndExport();
		return true;
	}
	return false;
}
