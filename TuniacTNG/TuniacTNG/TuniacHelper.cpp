#include "stdafx.h"
#include ".\tuniachelper.h"

CTuniacHelper::CTuniacHelper(void)
{
}

CTuniacHelper::~CTuniacHelper(void)
{
}

bool CTuniacHelper::GetTuniacRunFolder(String & toHere)
{
	TCHAR				szURL[MAX_PATH];

	if(GetModuleFileName(NULL, szURL, 512))
	{
		PathRemoveFileSpec(szURL);
		PathAddBackslash(szURL);

		toHere.clear();
		toHere = szURL;

		return true;
	}

	return false;
}

bool CTuniacHelper::GetFolderContents(String folder, StringArray & tohere, bool recurse)
{
	WIN32_FIND_DATA		w32fd;
	HANDLE				hFind;

	String searchPath = folder;
	if(!searchPath.Right(1).Equals(String("\\")))
		searchPath += TEXT("\\");

	searchPath += TEXT("*.*");

	hFind = FindFirstFile( searchPath.c_str(), &w32fd); 
	if(hFind != INVALID_HANDLE_VALUE) 
	{
		do
		{
			String filename = w32fd.cFileName;

			if( (filename.Equals(String(TEXT(".")))) || (filename.Equals(String(TEXT("..")))) )
				continue;

			String filePath = folder;

			if(!filePath.Right(1).Equals(String(TEXT("\\"))))
				filePath += TEXT("\\");

			filePath += w32fd.cFileName;

			if(PathIsFolder(filePath))
			{
				if(recurse)
					GetFolderContents(	filePath, tohere, recurse );
			}
			else
			{
				tohere.push_back(filePath);
			}
		} while(FindNextFile(hFind, &w32fd));

		FindClose(hFind); 
	}


	return true;
}

bool CTuniacHelper::GetAllFilesInFolderWithExtension(String folder, String Extension, StringArray & toHere)
{
	StringArray tempArray;
	

	if(GetFolderContents(folder, tempArray, false))
	{
		toHere.clear();

		for(unsigned int i=0; i<tempArray.size(); i++)
		{
			if(tempArray.at(i).Right((int)Extension.length()).ToUpper() == Extension.ToUpper())
			{
				toHere.push_back(tempArray.at(i));
			}
		}

		return true;
	}

	return false;
}

bool CTuniacHelper::PathIsFolder(String & path)
{
	if(GetFileAttributes(path.c_str()) & FILE_ATTRIBUTE_DIRECTORY)
		return true;

	return false;
}


bool CTuniacHelper::FormatSystemTime(String & toHere, SYSTEMTIME & st)
{
//    sprintf(zBuf, "%04d-%02d-%02d %02d:%02d:%02d",x.Y, x.M, x.D, x.h, x.m, (int)(x.s));


	toHere.erase();
	toHere.Format(TEXT("%04d-%02d-%02d %02d:%02d:%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	return true;
}
