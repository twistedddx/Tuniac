////////////////////////////////////////////////////////////
//
//				EDIT THIS ONE YOU BIG BALD FUCK!
//
//

#include "stdafx.h"
#include "DUMMYinfomanager.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}


extern "C" __declspec(dllexport) IInfoManager * CreateInfoManagerPlugin(void)
{
	IInfoManager * pInfo = new CDUMMYInfoManager;

	return(pInfo);
}

extern "C" __declspec(dllexport) unsigned long		GetTuniacInfoManagerVersion(void)
{
	return ITUNIACINFOMANAGER_VERSION;
}


// actual class definition here

CDUMMYInfoManager::CDUMMYInfoManager(void)
{
}

CDUMMYInfoManager::~CDUMMYInfoManager(void)
{
}

void			CDUMMYInfoManager::Destroy(void)
{
	delete this;
}

unsigned long	CDUMMYInfoManager::GetNumExtensions(void)
{
	return 1;
}

LPTSTR			CDUMMYInfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
	return TEXT("txt");
}

bool			CDUMMYInfoManager::CanHandle(LPTSTR szSource)
{
	if(StrStrI(PathFindExtension(szSource), TEXT("txt")))
		return true;
	return false;
}

bool			CDUMMYInfoManager::GetInfo(LibraryEntry * libEnt)
{
	TCHAR	szFileTitle[128];
	GetFileTitle(libEnt->szURL, szFileTitle, 128);
	StrCpy(libEnt->szTitle, szFileTitle);

	return true;
}

bool			CDUMMYInfoManager::SetInfo(LibraryEntry * libEnt)
{
	return true;
}