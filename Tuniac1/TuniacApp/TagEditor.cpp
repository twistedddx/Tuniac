#include "stdafx.h"
#include "resource.h"
#include ".\tageditor.h"

CTagEditor::CTagEditor(void)
{
}

CTagEditor::~CTagEditor(void)
{
}

LRESULT CALLBACK CTagEditor::EditorProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//wnd proc

	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
			}
			break;

		case WM_COMMAND:
			{
				WORD lID = LOWORD(wParam);
/*
				switch(lID)
				{
				}
				break;
*/
			}
			break;

		default:
			return FALSE;
			break;
		}

	return TRUE;
}

bool CTagEditor::UpdateControls()
{
	//puts the data onto the selected panel

	return false;

}

bool CTagEditor::SaveData()
{
	//saves the data back into the library

	return false;
}

bool CTagEditor::ShowEditor(HWND aParentWnd)
{
	//displays the property sheet

	PROPSHEETPAGE	lEditorPages[3];
	PROPSHEETHEADER psh;

	DWORD		lPageSize = sizeof(PROPSHEETPAGE);
	HINSTANCE	lInstance = tuniacApp.getMainInstance();

	//set up the pages
	lEditorPages[0].dwSize		= lPageSize;
    lEditorPages[0].dwFlags		= PSP_USETITLE;
	lEditorPages[0].hInstance	= lInstance;
    lEditorPages[0].pszTemplate	= MAKEINTRESOURCE(IDD_EDITOR_SONG);
    lEditorPages[0].pszIcon		= NULL;
    lEditorPages[0].pfnDlgProc	= (DLGPROC)EditorProc;
    lEditorPages[0].pszTitle	= TEXT("Song Info.");
    lEditorPages[0].lParam		= (LPARAM)this;
    lEditorPages[0].pfnCallback	= NULL;

	//set next two pages and adjust
	memcpy(&lEditorPages[1], &lEditorPages[0], lPageSize);
	memcpy(&lEditorPages[2], &lEditorPages[0], lPageSize);

	//adjust
	lEditorPages[1].pszTemplate	= MAKEINTRESOURCE(IDD_EDITOR_MISC);
	lEditorPages[1].pszTitle	= TEXT("Misc. Info.");

	lEditorPages[2].pszTemplate	= MAKEINTRESOURCE(IDD_EDITOR_FILE);
	lEditorPages[2].pszTitle	= TEXT("File Info.");

	//set up the header
	psh.dwSize		= sizeof(PROPSHEETHEADER);
    psh.dwFlags		= PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent	= aParentWnd;
	psh.hInstance	= tuniacApp.getMainInstance();
    psh.pszIcon		= 0;
    psh.pszCaption	= TEXT("Tuniac Preferences");
    psh.nPages		= 3;
    psh.nStartPage	= 0;
    psh.ppsp		= (LPCPROPSHEETPAGE) &lEditorPages;
    psh.pfnCallback = NULL;

   if(PropertySheet(&psh) == -1)
		return false;

   return true;
}