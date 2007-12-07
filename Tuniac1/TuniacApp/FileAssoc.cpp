#include "stdafx.h"
#include "resource.h"
#include "FileAssoc.h"

#include "IAudioSource.h"
#include "ITuniacImportPlugin.h"

#define FILEASSOC_NUMTYPES				(3)

#define FILEASSOC_REG_FILE_OPEN			TEXT("Open")
#define FILEASSOC_REG_FILE_PLAY			TEXT("Play")
#define FILEASSOC_REG_FILE_QUEUE		TEXT("Queue")

#define FILEASSOC_REG_FOLDER_OPEN		TEXT("Tuniac: Open")
#define FILEASSOC_REG_FOLDER_PLAY		TEXT("Tuniac: Play")
#define FILEASSOC_REG_FOLDER_QUEUE		TEXT("Tuniac: Queue")

#define FILEASSOC_ACTION_OPEN			TEXT("")
#define FILEASSOC_ACTION_PLAY			TEXT("-play")
#define FILEASSOC_ACTION_QUEUE			TEXT("-playnext")


#define FILEASSOC_ARRAY_TYPES			{FILEASSOC_TYPE_OPEN, FILEASSOC_TYPE_PLAY, FILEASSOC_TYPE_QUEUE}
#define FILEASSOC_ARRAY_FILE			{FILEASSOC_REG_FILE_OPEN, FILEASSOC_REG_FILE_PLAY, FILEASSOC_REG_FILE_QUEUE}
#define FILEASSOC_ARRAY_FOLDER			{FILEASSOC_REG_FOLDER_OPEN, FILEASSOC_REG_FOLDER_PLAY, FILEASSOC_REG_FOLDER_QUEUE}
#define FILEASSOC_ARRAY_ACTIONS			{FILEASSOC_ACTION_OPEN, FILEASSOC_ACTION_PLAY, FILEASSOC_ACTION_QUEUE}


CFileAssoc::CFileAssoc(void)
{
}

CFileAssoc::~CFileAssoc(void)
{
}

bool			CFileAssoc::CleanAssociations()
{
	UpdateExtensionList();
	for(int i = 0; i < m_ExtList.GetCount(); i++)
	{
		m_ExtList[i].bAssociated = false;
	}
	ReAssociate(0, false);
	tuniacApp.m_Preferences.RecursiveDeleteReg(HKEY_CLASSES_ROOT, TEXT("Tuniac.audio"));
	return true;
}

bool			CFileAssoc::UpdateExtensionList(void)
{
	m_ExtList.RemoveAll();
	for(unsigned long i = 0; i < tuniacApp.m_CoreAudio.GetNumPlugins(); i++)
	{
		IAudioSourceSupplier * pPlugin = tuniacApp.m_CoreAudio.GetPluginAtIndex(i);
		LPTSTR szName = pPlugin->GetName();
		for (unsigned long j = 0; j < pPlugin->GetNumCommonExts(); j++)
		{
			FileAssocDetail FAD;
			FAD.szDesc = szName;
			FAD.szExt = pPlugin->GetCommonExt(j);
			m_ExtList.AddTail(FAD);
			m_ExtList[m_ExtList.GetCount() - 1].bAssociated = IsAssocInReg(m_ExtList.GetCount() - 1);
		}
	}

	ITuniacImportPlugin * pImporter;
	int iImporter = 0;
	while((pImporter = tuniacApp.m_MediaLibrary.m_ImportExport.GetImporterAtIndex(iImporter)) != NULL)
	{
		LPTSTR szName = pImporter->GetName();
		for (unsigned long j = 0; j < pImporter->GetNumExtensions(); j++)
		{
			FileAssocDetail FAD;
			FAD.szDesc = szName;
			FAD.szExt = pImporter->SupportedExtension(j);
			m_ExtList.AddTail(FAD);
			m_ExtList[m_ExtList.GetCount() - 1].bAssociated = IsAssocInReg(m_ExtList.GetCount() - 1);
		}
		iImporter++;
	}

	return true;
}

PFileAssocDetail	CFileAssoc::GetExtensionDetail(unsigned long iIndex)
{
	if(iIndex >= m_ExtList.GetCount())
		return NULL;

	PFileAssocDetail pFAD = &m_ExtList[iIndex];
	return pFAD;
}

bool			CFileAssoc::IsAssocInReg(unsigned long ulIndex)
{
	if(ulIndex >= m_ExtList.GetCount())
		return false;

	PFileAssocDetail pFAD = &m_ExtList[ulIndex];

	HKEY hClassesKey;

	if(RegOpenKeyEx(	HKEY_CLASSES_ROOT,
						pFAD->szExt,
						0,
						KEY_QUERY_VALUE,
						&hClassesKey) != ERROR_SUCCESS)
	{
		return false;
	}

	TCHAR szTmp[265];
	unsigned long Size = 256 * sizeof(TCHAR);
	unsigned long Type = REG_SZ;

	if(RegQueryValueEx(	hClassesKey,
						NULL,
						NULL,
						&Type,
						(LPBYTE)&szTmp,
						&Size) == ERROR_SUCCESS)
	{

		RegCloseKey(hClassesKey);
		if(StrCmpNI(szTmp, TEXT("Tuniac.audio"), wcslen(TEXT("Tuniac.audio"))) == 0) {
			return true;
		}

	}

	return false;

}

bool			CFileAssoc::IsFoldersAssociated(void)
{
	LPTSTR	szTypeList[FILEASSOC_NUMTYPES]		= FILEASSOC_ARRAY_FOLDER;
	HKEY hClassesKey;
	TCHAR szTmp[512];

	for(int i = 0; i < FILEASSOC_NUMTYPES; i++)
	{
		wnsprintf(szTmp, 512, TEXT("Folder\\shell\\%s"), szTypeList[i]);
		if(RegOpenKeyEx(	HKEY_CLASSES_ROOT,
							szTmp,
							0,
							KEY_QUERY_VALUE,
							&hClassesKey) == ERROR_SUCCESS)
		{
			RegCloseKey(hClassesKey);
			return true;
		}
	}
	return false;
}

bool			CFileAssoc::AssociateFolders(int iTypes)
{
	int		iTypeList[FILEASSOC_NUMTYPES]		= FILEASSOC_ARRAY_TYPES;
	LPTSTR	szTypeList[FILEASSOC_NUMTYPES]		= FILEASSOC_ARRAY_FOLDER;
	LPTSTR	szActionList[FILEASSOC_NUMTYPES]	= FILEASSOC_ARRAY_ACTIONS;

	HKEY	hClassesKey;
	TCHAR	szTmp[512];

	for(int i = 0; i < FILEASSOC_NUMTYPES; i++)
	{
		if(iTypes & iTypeList[i])
		{
			wnsprintf(szTmp, 512, TEXT("Folder\\shell\\%s\\command"), szTypeList[i]);
			if(RegCreateKey(	HKEY_CLASSES_ROOT,
								szTmp,
								&hClassesKey) != ERROR_SUCCESS)
			{
				return false;
			}

			GetModuleFileName(NULL, szTmp, 512);
			wnsprintf(szTmp, 512, TEXT("%s %s \"%%1\""), szTmp, szActionList[i]);
			unsigned long Size = (wcslen(szTmp) + 1) * sizeof(TCHAR);
			RegSetValueEx(	hClassesKey,
							NULL,
							NULL,
							REG_SZ,
							(LPBYTE)&szTmp,
							Size);
	
			RegCloseKey(hClassesKey);
		}
		else
		{
			wnsprintf(szTmp, 512, TEXT("Folder\\shell\\%s\\command"), szTypeList[i]);
			if(RegDeleteKey(	HKEY_CLASSES_ROOT,
								szTmp) == ERROR_SUCCESS)
			{

				wnsprintf(szTmp, 512, TEXT("Folder\\shell\\%s"), szTypeList[i]);
				RegDeleteKey(	HKEY_CLASSES_ROOT,
								szTmp);

			}
		}

	}
	return true;
}

bool			CFileAssoc::AssociateExtension(unsigned long ulIndex)
{
	if(ulIndex >= m_ExtList.GetCount())
		return false;

	PFileAssocDetail pFAD = &m_ExtList[ulIndex];
	HKEY hClassesKey;

	if(RegCreateKey(	HKEY_CLASSES_ROOT,
						pFAD->szExt,
						&hClassesKey) != ERROR_SUCCESS)
	{
		return false;
	}

	TCHAR szTmp[512];
	unsigned long Size = 512 * sizeof(TCHAR);
	unsigned long Type = REG_SZ;

	if(pFAD->bAssociated)
	{

		RegQueryValueEx(	hClassesKey,
							NULL,
							NULL,
							&Type,
							(LPBYTE)&szTmp,
							&Size);

		if(wcslen(szTmp) > 0 && StrCmpNI(szTmp, TEXT("Tuniac.audio"), wcslen(TEXT("Tuniac.audio"))) != 0)
		{
			RegSetValueEx(	hClassesKey,
							TEXT("Tuniac_Previous"),
							NULL,
							REG_SZ,
							(LPBYTE)&szTmp,
							Size);
		}

		Size = (wcslen(TEXT("Tuniac.audio")) + 1) * sizeof(TCHAR);
		wnsprintf(szTmp, 512, TEXT("Tuniac.audio"));
		RegSetValueEx(	hClassesKey,
						NULL,
						NULL,
						REG_SZ,
						(LPBYTE)&szTmp,
						Size);

	}
	else
	{
		if (RegQueryValueEx(	hClassesKey,
								TEXT("Tuniac_Previous"),
								NULL,
								&Type,
								(LPBYTE)&szTmp,
								&Size) != ERROR_SUCCESS)
		{
			wnsprintf(szTmp, 512, TEXT(""));
		}

		Size = (wcslen(szTmp) + 1) * sizeof(TCHAR);
		RegSetValueEx(	hClassesKey,
						NULL,
						NULL,
						REG_SZ,
						(LPBYTE)&szTmp,
						Size);

		RegDeleteValue(	hClassesKey,
						TEXT("Tuniac_Previous"));

	}
	RegCloseKey(hClassesKey);

	return true;

}

int				CFileAssoc::GetExtensionCount(void)
{
	return m_ExtList.GetCount();
}

int				CFileAssoc::GetDefaultType(void)
{
	HKEY hClassesKey;
	int		iTypeList[FILEASSOC_NUMTYPES]		= FILEASSOC_ARRAY_TYPES;
	LPTSTR	szTypeList[FILEASSOC_NUMTYPES]		= FILEASSOC_ARRAY_FILE;

	if(RegOpenKey(	HKEY_CLASSES_ROOT,
						TEXT("Tuniac.audio\\Shell"),
						&hClassesKey) != ERROR_SUCCESS)
	{
		return iTypeList[0];
	}

	TCHAR szDefault[128];
	unsigned long Size = 128 * sizeof(TCHAR);
	unsigned long Type = REG_SZ;


	if(RegQueryValueEx(	hClassesKey,
						NULL,
						NULL,
						&Type,
						(LPBYTE)&szDefault,
						&Size) != ERROR_SUCCESS)
	{
		return iTypeList[0];
	}
	RegCloseKey(hClassesKey);

	for(int i = 0; i < FILEASSOC_NUMTYPES; i++)
	{
		if(StrCmpI(szDefault, szTypeList[i]) == 0) {
			return iTypeList[i];
		}
	}

	return iTypeList[0];

}

bool			CFileAssoc::SetDefaultType(int iType)
{
	HKEY hClassesKey;
	int		iTypeList[FILEASSOC_NUMTYPES]		= FILEASSOC_ARRAY_TYPES;
	LPTSTR	szTypeList[FILEASSOC_NUMTYPES]		= FILEASSOC_ARRAY_FILE;
	TCHAR	szDefault[128];
	bool bFound = false;

    	for(int i = 0; i < FILEASSOC_NUMTYPES; i++)
	{
		if(iType == iTypeList[i])
		{
			wnsprintf(szDefault, 128, TEXT("%s"), szTypeList[i]);
			bFound = true;
			break;
		}
	}
	if(!bFound)
		return false;

	if(RegCreateKey(	HKEY_CLASSES_ROOT,
						TEXT("Tuniac.audio\\Shell"),
						&hClassesKey) == ERROR_SUCCESS)
	{
		int Size = (wcslen(szDefault) + 1) * sizeof(TCHAR);
		RegSetValueEx(	hClassesKey,
						NULL,
						NULL,
						REG_SZ,
						(LPBYTE)&szDefault,
						Size);
		RegCloseKey(hClassesKey);
	} else {
		return false;
	}

	return true;
}

bool			CFileAssoc::ReAssociate(int iTypes, bool bAssocFolders)
{
	int		iTypeList[FILEASSOC_NUMTYPES]		= FILEASSOC_ARRAY_TYPES;
	LPTSTR	szTypeList[FILEASSOC_NUMTYPES]		= FILEASSOC_ARRAY_FILE;
	LPTSTR	szActionList[FILEASSOC_NUMTYPES]	= FILEASSOC_ARRAY_ACTIONS;
	HKEY hClassesKey;
	TCHAR szTmp[512];
	unsigned long Size;

	if(RegCreateKey(	HKEY_CLASSES_ROOT,
						TEXT("Tuniac.audio"),
						&hClassesKey) == ERROR_SUCCESS)
	{
		wnsprintf(szTmp, 512, TEXT("Tuniac Audio File"));
		Size = (wcslen(szTmp) + 1) * sizeof(TCHAR);
		RegSetValueEx(	hClassesKey,
						NULL,
						NULL,
						REG_SZ,
						(LPBYTE)&szTmp,
						Size);
		RegCloseKey(hClassesKey);
	} else {
		return false;
	}
	
	if(RegCreateKey(	HKEY_CLASSES_ROOT,
						TEXT("Tuniac.audio\\DefaultIcon"),
						&hClassesKey) == ERROR_SUCCESS)
	{
		GetModuleFileName(NULL, szTmp, 512);
		wnsprintf(szTmp, 512, TEXT("%s,0"), szTmp);
		Size = (wcslen(szTmp) + 1) * sizeof(TCHAR);
		RegSetValueEx(	hClassesKey,
						NULL,
						NULL,
						REG_SZ,
						(LPBYTE)&szTmp,
						Size);
		RegCloseKey(hClassesKey);
	}

	for(int i = 0; i < FILEASSOC_NUMTYPES; i++)
	{
		if(iTypes & iTypeList[i])
		{

			wnsprintf(szTmp, 512, TEXT("Tuniac.audio\\Shell\\%s"), szTypeList[i]);
			if(RegCreateKey(	HKEY_CLASSES_ROOT,
								szTmp,
								&hClassesKey) == ERROR_SUCCESS)
			{
				wnsprintf(szTmp, 512, TEXT("&%s"), szTypeList[i]);
				Size = (wcslen(szTmp) + 1) * sizeof(TCHAR);
				RegSetValueEx(	hClassesKey,
								NULL,
								NULL,
								REG_SZ,
								(LPBYTE)&szTmp,
								Size);
				RegCloseKey(hClassesKey);

				wnsprintf(szTmp, 512, TEXT("Tuniac.audio\\Shell\\%s\\command"), szTypeList[i]);
				if(RegCreateKey(	HKEY_CLASSES_ROOT,
									szTmp,
									&hClassesKey) == ERROR_SUCCESS)
				{
					GetModuleFileName(NULL, szTmp, 512);
					wnsprintf(szTmp, 512, TEXT("%s %s \"%%1\""), szTmp, szActionList[i]);
					Size = (wcslen(szTmp) + 1) * sizeof(TCHAR);
					RegSetValueEx(	hClassesKey,
									NULL,
									NULL,
									REG_SZ,
									(LPBYTE)&szTmp,
									Size);
					RegCloseKey(hClassesKey);
				}
			}
		}
		else
		{
			wnsprintf(szTmp, 512, TEXT("Tuniac.audio\\Shell\\%s\\command"), szTypeList[i]);
			if(RegDeleteKey(	HKEY_CLASSES_ROOT,
								szTmp) == ERROR_SUCCESS)
			{

				wnsprintf(szTmp, 512, TEXT("Tuniac.audio\\Shell\\%s"), szTypeList[i]);
				RegDeleteKey(	HKEY_CLASSES_ROOT,
								szTmp);

			}

		}
	}

	AssociateFolders(bAssocFolders ? iTypes : 0);

	for(unsigned long i = 0; i < m_ExtList.GetCount(); i++)
	{
		AssociateExtension(i);
	}

	// must be called to let windows know
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	return true;
}
