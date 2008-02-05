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
	TCHAR				szBase[512];
	TCHAR				szSearchFilename[512];

	GetModuleFileName(NULL, szBase, 512);
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

			TCHAR szFilename[512];
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
						wnsprintf(szError, 512, TEXT("Incompatable import plugin found: %s\n\nThis Plugin must be updated before you can use it."), w32fd.cFileName);
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
						wnsprintf(szError, 512, TEXT("Incompatable export plugin found: %s\n\nThis Plugin must be updated before you can use it."), w32fd.cFileName);
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
		wnsprintf(szItem, 128, TEXT("%s - %s"), m_ExportExtensions[i].pExporter->GetName(), m_ExportExtensions[i].szExt);
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
	
	bool bOK = false, bStreamList = false;
	CMediaLibraryPlaylistEntry * pStreamEntry;
	TCHAR szFilename[512];

	if(pImporter->BeginImport(szSource) && pImporter->ImportUrl(szFilename, 512))
	{
		bOK = true;
		bStreamList = PathIsURL(szFilename) == TRUE;
		if(bStreamList)
		{

			tuniacApp.m_MediaLibrary.AddItem(szFilename);
			pStreamEntry = tuniacApp.m_MediaLibrary.GetItemByIndex(tuniacApp.m_MediaLibrary.GetCount() - 1);

			TCHAR szTitle[128];
			if(pImporter->ImportTitle(szTitle, 128))
			{
				if(wcslen(pStreamEntry->GetLibraryEntry()->szArtist) == 0)
					StrCpyN(pStreamEntry->GetLibraryEntry()->szArtist, szTitle, 128);
				if(wcslen(pStreamEntry->GetLibraryEntry()->szComment) == 0)
				StrCpyN(pStreamEntry->GetLibraryEntry()->szComment, szTitle, 128);
			}
			else
			{
				if(wcslen(pStreamEntry->GetLibraryEntry()->szArtist) == 0)
					StrCpy(pStreamEntry->GetLibraryEntry()->szArtist, TEXT("[Unknown Stream]"));
			}


			if(pStreamEntry != NULL && StrCmpI(pStreamEntry->GetLibraryEntry()->szURL, szFilename) == 0)
			{
				/*
				for(int i = 1; i < LIBENTRY_MAX_URLS; i++)
				{
					if(!pImporter->ImportUrl(pStreamEntry->GetLibraryEntry()->szURL[i], 512))
						break;
				}
				*/
			} else {
				bOK = false;
			}
		}
		else
		{
			do
			{
				tuniacApp.m_MediaLibrary.AddItem(szFilename);
			} while(pImporter->ImportUrl(szFilename, 512));
		}
		pImporter->EndImport();
	}
	return bOK;
}

bool			CImportExportManager::Export(EntryArray & entryArray, LPTSTR szSource)
{
	TCHAR szFilename[512];
	ZeroMemory(&szFilename, sizeof(TCHAR) * 512);
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
			
			wnsprintf(szFilterText, 128, TEXT("%s (*%s)"), m_ExportExtensions[i].pExporter->GetName(), m_ExportExtensions[i].szExt);
			wnsprintf(szFilterExt, 16, TEXT("*%s"), m_ExportExtensions[i].szExt);
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
		ofn.nMaxFile = 512;
		ofn.lpstrFilter = szFilters;
		ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
		ofn.hwndOwner = tuniacApp.getMainWindow();

		if(GetSaveFileName(&ofn) == 0)
			return false;


		StrCpyN(szFilename, ofn.lpstrFile, 512);

		if(ofn.nFilterIndex >= 1 && ofn.nFilterIndex <= m_ExportExtensions.GetCount())
		{
			if(!CanExport(szFilename))
				StrCatN(szFilename, m_ExportExtensions[ofn.nFilterIndex - 1].szExt, 512);
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
	if(pExporter->BeginExport(szSource))
	{
		LibraryEntry LE;
		CMediaLibraryPlaylistEntry * pEntry;

		for(unsigned long i = 0; i < entryArray.GetCount(); i++)
		{
			pEntry = tuniacApp.m_MediaLibrary.GetItemByID(entryArray[i]->GetEntryID());
			if(pEntry != NULL)
			{
				CopyMemory(&LE, pEntry->GetLibraryEntry(), sizeof(LE));
				pExporter->ExportEntry(LE);
			}
		}

		pExporter->EndExport();
		return true;
	}
	return false;
}
