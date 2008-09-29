// TuniacTNG.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TuniacTNG.h"
#include "MediaManager.h"
#include "TuniacHelper.h"

#include "TuniacApp.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' " "version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if(CTuniacApp::Instance()->Initialize(hInstance, lpCmdLine))
	{
		CTuniacApp::Instance()->Run();
	}

}

/*


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
		case WM_CREATE:
			{
				RECT r;
				GetClientRect(hWnd, &r);
			}
			break;

		case WM_COMMAND:
			{
				wmId    = LOWORD(wParam);
				wmEvent = HIWORD(wParam);
				// Parse the menu selections:
				switch (wmId)
				{
					case IDM_FILE_EXIT:
						DestroyWindow(hWnd);
						break;
					case IDM_FILE_IMPORTFILE:
						{
							ShowAddFiles(hWnd);
						}
						break;
					case IDM_FILE_IMPORTDIR:
						{
							ShowAddFolderSelector(hWnd);
						}
						break;



					default:
						return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
			break;

		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				// TODO: Add any drawing code here...
				EndPaint(hWnd, &ps);
			}
			break;

		case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			break;

		case WM_SIZE:
			{
			}
			break;


		case WM_DROPFILES:
			{
				HDROP hDrop = (HDROP)wParam;

				TCHAR szURL[MAX_PATH];

				UINT uNumFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NULL);


				StringArray		filesToAdd;

				for(UINT file = 0; file<uNumFiles; file++)
				{
					DragQueryFile(hDrop, file, szURL, MAX_PATH);

					if(CTuniacHelper::Instance()->PathIsFolder(String(szURL)))
					{
						StringArray		contents;

						CTuniacHelper::Instance()->GetFolderContents(szURL, contents, true);

						for(int x=0; x<contents.size(); x++)
						{
							filesToAdd.push_back(contents[x]);
						}
					}
					else
					{
						filesToAdd.push_back(szURL);
					}
				}
				CMediaManager::Instance()->AddFileArray(filesToAdd);

				DragFinish(hDrop);

			}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


bool ShowAddFolderSelector(HWND hWndParent)
{
	LPMALLOC lpMalloc;  // pointer to IMalloc

	if(::SHGetMalloc(&lpMalloc) == NOERROR)
	{
		TCHAR szBuffer[1024];

		BROWSEINFO browseInfo;
		LPITEMIDLIST lpItemIDList;

		browseInfo.hwndOwner		= hWndParent;
		browseInfo.pidlRoot			= NULL; 
		browseInfo.pszDisplayName	= NULL;
		browseInfo.lpszTitle		= TEXT("Select a directory...");   // passed in
		browseInfo.ulFlags			= BIF_RETURNONLYFSDIRS | BIF_USENEWUI;   // also passed in
		browseInfo.lpfn				= NULL;      // not used
		browseInfo.lParam			= 0;      // not used   

		if((lpItemIDList = ::SHBrowseForFolder(&browseInfo)) != NULL)
		{
			if(::SHGetPathFromIDList(lpItemIDList, szBuffer))
			{
				StringArray		tempArray;

				// TODO: here we should build a fully recursed array of all the files we're going to add!
				CTuniacHelper::Instance()->GetFolderContents(szBuffer, tempArray, true);

				// TODO: then add it!
				CMediaManager::Instance()->AddFileArray(tempArray);
			}

			lpMalloc->Free(lpItemIDList);
		}

		lpMalloc->Release();

		return true;
	}

	return false;
}

bool ShowAddFiles(HWND hWndParent)
{
#define OFNBUFFERSIZE		(32*1024)
	OPENFILENAME		ofn;
	TCHAR				szURLBuffer[OFNBUFFERSIZE];

	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ZeroMemory(szURLBuffer, OFNBUFFERSIZE * sizeof TCHAR);

	ofn.lStructSize			= sizeof(OPENFILENAME);
	ofn.hwndOwner			= hWndParent;
	ofn.hInstance			= (HINSTANCE)CTuniacHelper::Instance();
	ofn.lpstrFilter			= TEXT("All Files\0*.*\0");
	ofn.lpstrCustomFilter	= NULL;
	ofn.nMaxCustFilter		= 0;
	ofn.nFilterIndex		= 0;
	ofn.lpstrFile			= szURLBuffer;
	ofn.nMaxFile			= OFNBUFFERSIZE;
	ofn.lpstrFileTitle		= NULL;
	ofn.nMaxFileTitle		= 0;
	ofn.lpstrInitialDir		= NULL;
	ofn.lpstrTitle			= NULL;
	ofn.Flags				= OFN_ALLOWMULTISELECT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if(GetOpenFileName(&ofn))
	{
		StringArray		nameArray;
		if(ofn.nFileOffset < lstrlen(szURLBuffer))
		{
			nameArray.push_back(szURLBuffer);

			CMediaManager::Instance()->AddFileArray(nameArray);
		}
		else
		{
			LPTSTR	szOFNName = &szURLBuffer[ofn.nFileOffset];

			String szFilePath = szURLBuffer;
			while( lstrlen(szOFNName) != 0 )
			{
				String filename = szFilePath;
				filename += TEXT("\\");
				filename += szOFNName;

				nameArray.push_back(filename);

				szOFNName = &szOFNName[lstrlen(szOFNName) + 1];
			}

			CMediaManager::Instance()->AddFileArray(nameArray);
		}
		return true;
	}

	return false;
}
*/