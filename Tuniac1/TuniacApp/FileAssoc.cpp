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
/*
	Modification and addition to Tuniac originally written by Tony Million
	Copyright (C) 2003-2014 Brett Hoyle
*/

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
	ReAssociate(0);
	SHDeleteKey(HKEY_CLASSES_ROOT, TEXT("Tuniac.audio"));
	return true;
}

bool			CFileAssoc::UpdateExtensionList(void)
{
	m_ExtList.RemoveAll();
	for(unsigned long i = 0; i < CCoreAudio::Instance()->GetNumPlugins(); i++)
	{
		IAudioSourceSupplier * pPlugin = CCoreAudio::Instance()->GetPluginAtIndex(i);
		for (unsigned long j = 0; j < pPlugin->GetNumCommonExts(); j++)
		{
			m_ExtList.AddTail(std::wstring(pPlugin->GetCommonExt(j)));
		}
	}

	ITuniacImportPlugin * pImporter;
	int iImporter = 0;
	while((pImporter = tuniacApp.m_MediaLibrary.m_ImportExport.GetImporterAtIndex(iImporter)) != NULL)
	{
		for (unsigned long j = 0; j < pImporter->GetNumExtensions(); j++)
		{
			m_ExtList.AddTail(std::wstring(pImporter->SupportedExtension(j)));
		}
		iImporter++;
	}

	return true;
}

bool			CFileAssoc::AssociateExtension(unsigned long ulIndex)
{
	if(!tuniacApp.getSavePrefs())
		return false;

	if(ulIndex >= m_ExtList.GetCount())
		return false;

	HKEY hTuniacAssocKey;

	if(RegCreateKey(	HKEY_LOCAL_MACHINE,
						L"SOFTWARE\\Tuniac\\Capabilities\\FileAssociations",
						&hTuniacAssocKey) == ERROR_SUCCESS)
	{

		RegSetValueEx(	hTuniacAssocKey, 
						(LPTSTR)m_ExtList[ulIndex].c_str(), 
						0,
						REG_SZ,
						(LPBYTE)L"Tuniac.audio", 
						(12 + 1) * sizeof(TCHAR));
		RegCloseKey(hTuniacAssocKey);
	}

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
			StringCchCopy(szDefault, 128, szTypeList[i]);
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
		int Size = (wcsnlen_s(szDefault, 128) + 1) * sizeof(TCHAR);
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

bool			CFileAssoc::ReAssociate(int iTypes)
{
	int		iTypeList[FILEASSOC_NUMTYPES]		= FILEASSOC_ARRAY_TYPES;
	LPTSTR	szTypeList[FILEASSOC_NUMTYPES]		= FILEASSOC_ARRAY_FILE;
	LPTSTR	szActionList[FILEASSOC_NUMTYPES]	= FILEASSOC_ARRAY_ACTIONS;

	HKEY hTuniacAssocKey;
	TCHAR szTmp[512];
	unsigned long Size;

	if(RegCreateKey(	HKEY_CLASSES_ROOT,
						TEXT("Tuniac.audio"),
						&hTuniacAssocKey) == ERROR_SUCCESS)
	{
		StringCchCopy(szTmp, 512, TEXT("Tuniac Audio File"));
		Size = (wcsnlen_s(szTmp, 512) + 1) * sizeof(TCHAR);
		RegSetValueEx(	hTuniacAssocKey,
						NULL,
						NULL,
						REG_SZ,
						(LPBYTE)&szTmp,
						Size);
		RegCloseKey(hTuniacAssocKey);
	}
	else
	{
		return false;
	}
	
	if(RegCreateKey(	HKEY_CLASSES_ROOT,
						TEXT("Tuniac.audio\\DefaultIcon"),
						&hTuniacAssocKey) == ERROR_SUCCESS)
	{
		GetModuleFileName(NULL, szTmp, 512);
		StringCchPrintf(szTmp, 512, TEXT("%s,0"), szTmp);
		Size = (wcsnlen_s(szTmp, 512) + 1) * sizeof(TCHAR);
		RegSetValueEx(	hTuniacAssocKey,
						NULL,
						NULL,
						REG_SZ,
						(LPBYTE)&szTmp,
						Size);
		RegCloseKey(hTuniacAssocKey);
	}

	for(int i = 0; i < FILEASSOC_NUMTYPES; i++)
	{
		if(iTypes & iTypeList[i])
		{

			StringCchPrintf(szTmp, 512, TEXT("Tuniac.audio\\Shell\\%s"), szTypeList[i]);
			if(RegCreateKey(	HKEY_CLASSES_ROOT,
								szTmp,
								&hTuniacAssocKey) == ERROR_SUCCESS)
			{
				StringCchPrintf(szTmp, 512, TEXT("&%s"), szTypeList[i]);
				Size = (wcsnlen_s(szTmp, 512) + 1) * sizeof(TCHAR);
				RegSetValueEx(	hTuniacAssocKey,
								NULL,
								NULL,
								REG_SZ,
								(LPBYTE)&szTmp,
								Size);
				RegCloseKey(hTuniacAssocKey);

				StringCchPrintf(szTmp, 512, TEXT("Tuniac.audio\\Shell\\%s\\command"), szTypeList[i]);
				if(RegCreateKey(	HKEY_CLASSES_ROOT,
									szTmp,
									&hTuniacAssocKey) == ERROR_SUCCESS)
				{
					GetModuleFileName(NULL, szTmp, 512);
					StringCchPrintf(szTmp, 512, TEXT("%s %s \"%%1\""), szTmp, szActionList[i]);
					Size = (wcsnlen_s(szTmp, 512) + 1) * sizeof(TCHAR);
					RegSetValueEx(	hTuniacAssocKey,
									NULL,
									NULL,
									REG_SZ,
									(LPBYTE)&szTmp,
									Size);
					RegCloseKey(hTuniacAssocKey);
				}
			}
		}
		else
		{
			StringCchPrintf(szTmp, 512, TEXT("Tuniac.audio\\Shell\\%s\\command"), szTypeList[i]);
			if(RegDeleteKey(	HKEY_CLASSES_ROOT,
								szTmp) == ERROR_SUCCESS)
			{

				StringCchPrintf(szTmp, 512, TEXT("Tuniac.audio\\Shell\\%s"), szTypeList[i]);
				RegDeleteKey(	HKEY_CLASSES_ROOT,
								szTmp);

			}

		}
	}

	if(RegCreateKey(	HKEY_LOCAL_MACHINE,
						L"SOFTWARE\\Tuniac",
						&hTuniacAssocKey) == ERROR_SUCCESS)
	{
		if(RegCreateKey(	HKEY_LOCAL_MACHINE,
							L"SOFTWARE\\Tuniac\\Capabilities",
							&hTuniacAssocKey) == ERROR_SUCCESS)
		{
			RegSetValueEx(	hTuniacAssocKey, 
							L"ApplicationDescription", 
							0,
							REG_SZ,
							(LPBYTE)L"Tuniac media player", 
							(19 + 1) * sizeof(TCHAR));

			RegSetValueEx(	hTuniacAssocKey, 
							L"ApplicationName", 
							0,
							REG_SZ,
							(LPBYTE)L"Tuniac", 
							(6 + 1) * sizeof(TCHAR));


			RegCloseKey(hTuniacAssocKey);
		}
	}

	if(RegCreateKey(	HKEY_LOCAL_MACHINE,
						L"SOFTWARE\\RegisteredApplications",
						&hTuniacAssocKey) == ERROR_SUCCESS)
	{

		RegSetValueEx(	hTuniacAssocKey, 
						L"Tuniac", 
						0,
						REG_SZ,
						(LPBYTE)L"Software\\Tuniac\\Capabilities", 
						(28 + 1) * sizeof(TCHAR));
		RegCloseKey(hTuniacAssocKey);
	}

	UpdateExtensionList();

	for(unsigned long i = 0; i < m_ExtList.GetCount(); i++)
	{
		AssociateExtension(i);
	}

	// must be called to let windows know
	SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
	return true;
}
