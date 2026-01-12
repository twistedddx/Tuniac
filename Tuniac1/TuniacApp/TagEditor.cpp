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
#include "tageditor.h"

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
/*
				TCHAR szFieldData[256];

				pPE->GetTextRepresentation(FIELD_PLAYBACKTIME, szFieldData, 256);
				SendDlgItemMessage(hDlg, IDC_PLAYTIME_TEXT, WM_SETTEXT, 0, (WPARAM)szFieldData);

				pPE->GetTextRepresentation(FIELD_SAMPLERATE, szFieldData, 256);
				SendDlgItemMessage(hDlg, IDC_SAMPLERATE_TEXT, WM_SETTEXT, 0, (WPARAM)szFieldData);

				pPE->GetTextRepresentation(FIELD_NUMCHANNELS, szFieldData, 256);
				SendDlgItemMessage(hDlg, IDC_CHANNELS_TEXT, WM_SETTEXT, 0, (WPARAM)szFieldData);

				pPE->GetTextRepresentation(FIELD_BITRATE, szFieldData, 256);
				SendDlgItemMessage(hDlg, IDC_BITRATE_TEXT, WM_SETTEXT, 0, (WPARAM)szFieldData);
*/
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