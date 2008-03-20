#include "stdafx.h"
#include ".\tuniacpluginmanager.h"

CTuniacPluginManager::CTuniacPluginManager(void)
{
}

CTuniacPluginManager::~CTuniacPluginManager(void)
{
}

bool CTuniacPluginManager::Initialize(void)
{
	String			baseFolder;
	StringArray		dllFileNameArray;

	g_tuniacApp.getHelper().GetTuniacRunFolder(baseFolder);
	g_tuniacApp.getHelper().GetAllFilesInFolderWithExtension(baseFolder, TEXT("dll"), dllFileNameArray);

	if(dllFileNameArray.size())
	{
		for(int x=0; x<dllFileNameArray.size();x++)
		{
			HINSTANCE hDLL = LoadLibrary(dllFileNameArray.at(x).c_str());
			if(hDLL)
			{
				if(GetProcAddress(hDLL, "CreateTuniacPlugin"))
				{
					hDLLArray.push_back(hDLL);
				}
				else
				{
					FreeLibrary(hDLL);
				}
			}
		}
	}

	if(hDLLArray.size())
	{
		CreateTuniacPluginFunc	pCTPF;

		for(int x=0; x<hDLLArray.size();x++)
		{
			pCTPF = (CreateTuniacPluginFunc)GetProcAddress(hDLLArray.at(x), "CreateTuniacPlugin");
			if(pCTPF)
			{
				int InterfaceNumber;

				// extract all TuniacInfoHandlers
				InterfaceNumber = 0;
				while(true)
				{
					ITuniacInfoHandler * temp = (ITuniacInfoHandler*)pCTPF(GUID_TUNIACINFOHANDLER, InterfaceNumber);
					if(temp)
					{
						InfoHandlerArray.push_back(temp);
					}
					else
					{
						break;
					}

					InterfaceNumber++;
				}

				// extract all PluginWindows
				InterfaceNumber = 0;
				while(true)
				{
					ITuniacPluginWindow * temp = (ITuniacPluginWindow*)pCTPF(GUID_TUNIACPLUGINWINDOW, InterfaceNumber);
					if(temp)
					{
						PluginWindowArray.push_back(temp);
					}
					else
					{
						break;
					}

					InterfaceNumber++;
				}

			}
		}

		return true;
	}

	return false;
}

bool CTuniacPluginManager::Shutdown(void)
{
	while(PluginWindowArray.empty() == false)
	{
		PluginWindowArray.back()->Destroy();
		PluginWindowArray.pop_back();
	}

	while(InfoHandlerArray.empty() == false)
	{
		InfoHandlerArray.back()->Destroy();
		InfoHandlerArray.pop_back();
	}

	while(hDLLArray.empty() == false)
	{
		FreeLibrary(hDLLArray.back());
		hDLLArray.pop_back();
	}

	return true;
}

unsigned long CTuniacPluginManager::GetPluginCount(GUID InterfaceID)
{
	if(InterfaceID == GUID_TUNIACINFOHANDLER)
	{
		return (unsigned long)InfoHandlerArray.size();
	}
	else if(InterfaceID == GUID_TUNIACPLUGINWINDOW)
	{
		return (unsigned long)PluginWindowArray.size();
	}

	return 0;
}

void * CTuniacPluginManager::GetPluginAtIndex(GUID InterfaceID, unsigned long Index)
{
	if(InterfaceID == GUID_TUNIACINFOHANDLER)
	{
		return InfoHandlerArray.at(Index);
	}
	else if(InterfaceID == GUID_TUNIACPLUGINWINDOW)
	{
		return PluginWindowArray.at(Index);
	}

	return NULL;
}
