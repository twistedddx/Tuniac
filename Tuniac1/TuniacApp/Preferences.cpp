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
#include "resource.h"
#include "preferences.h"

#define PREFERENCES_KEY			TEXT("Software\\Tuniac")

#define MAINWINDOWPOS			TEXT("MainWindowPos")
#define MAINWINDOWMAXIMIZED		TEXT("MainWindowMaximized")
#define MAINWINDOWMINIMIZED		TEXT("MainWindowMinimized")
#define SOURCEVIEWDIVIDERX		TEXT("SourceViewDividerX")
#define ACTIVETHEME				TEXT("ActiveTheme")

#define TRAYICONMODE			TEXT("TrayIconMode")
#define MINIMIZEONCLOSE			TEXT("MinimizeOnClose")
#define ALWAYSONTOP				TEXT("AlwaysOnTop")
#define PAUSEONLOCK				TEXT("PauseOnLock")
#define PAUSEONSCREENSAVE		TEXT("PauseOnScreensave")
#define SHOWALBUMART			TEXT("ShowAlbumArt")
#define ARTONSELECTION			TEXT("ArtOnSelection")
#define FOLLOWCURRENTSONG		TEXT("FollowCurrentSong")
#define SMARTSORTING			TEXT("SmartSorting")

#define WINDOWFORMATSTRING		TEXT("WindowFormatString")
#define PLUGINFORMATSTRING		TEXT("PluginFormatString")
#define LISTFORMATSTRING		TEXT("ListFormatString")

#define CROSSFADETIME			TEXT("CrossFadeTime")
#define CROSSFADEENABLED		TEXT("CrossFadeEnabled")

#define AUDIOBUFFERING			TEXT("AudioBuffering")
#define AUDIOGAIN				TEXT("AudioReplayGain")
#define AUDIOGAINALBUM			TEXT("AudioReplayGainAlbum")

#define VOLUME 					TEXT("Volume")
#define EQENABLED				TEXT("EQEnabled")
#define EQLOW					TEXT("EQLow")
#define EQMID					TEXT("EQMid")
#define EQHIGH					TEXT("EQHigh")
#define AMPGAIN					TEXT("AmpGain")

#define DATEADDED 				TEXT("DateAdded")
#define PLAYLISTSORTING			TEXT("PlaylistSorting")
#define AUTOADDPLAYLIST			TEXT("AutoAddPlaylist")
#define AUTOSOFTPAUSE			TEXT("AutoSoftPause")

#define SHUFFLEPLAY				TEXT("ShufflePlay")
#define REPEATMODE				TEXT("RepeatMode")

#define VISUALFPS				TEXT("VisualFPS")
#define CURRENTVISUAL			TEXT("CurrentVisual")
#define SHOWVISART				TEXT("ShowVisArt")

#define PLAYLISTVIEWNUMCOLS		TEXT("PlaylistViewNumCols")
#define PLAYLISTVIEWCOLIDS		TEXT("PlaylistViewColIDs")
#define PLAYLISTVIEWCOLWIDTHS	TEXT("PlaylistViewColWidths")

#define FILEASSOCTYPE			TEXT("FileAssocType")

#define HISTORYLISTSIZE			TEXT("HistoryListSize")
#define FUTURELISTSIZE			TEXT("FutureListSize")


#define FORMATSTRING_HELP	TEXT("\
@U\tURL\r\n\
@F\tFilename\r\n\
@X\tExtension\r\n\
@x\tEntension (without leading .)\r\n\
@K\tKind\r\n\
@S\tSize\r\n\
@A\tArtist\r\n\
@L\tAlbum\r\n\
@T\tTitle\r\n\
@#\tTrack\r\n\
@G\tGenre\r\n\
@Y\tYear\r\n\
@I\tPlay Time\r\n\
@i\tPlay Time (short form)\r\n\
@D\tDate Added\r\n\
@E\tFile Creation Date\r\n\
@P\tLast Played Date\r\n\
@B\tBitrate\r\n\
@M\tSample Rate\r\n\
@N\tChannels\r\n\
@C\tComment\r\n\
@Z\tPlay Count\r\n\
@R\tRating\r\n\
@!\tPlay State\
")


#define HISTORY_MAX		(40)
#define FUTURE_MAX		(40)

LRESULT CALLBACK CPreferences::GeneralProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPreferences * pPrefs = (CPreferences *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
				pPrefs = (CPreferences *)lParam;

				SendDlgItemMessage(hDlg, IDC_GENERAL_FOLLOWCURRENTSONG, BM_SETCHECK, pPrefs->m_bFollowCurrentSong ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_GENERAL_SMARTSORTING, BM_SETCHECK, pPrefs->m_bSmartSorting ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_GENERAL_AUTOSOFTPAUSE, BM_SETCHECK, pPrefs->m_bAutoSoftPause ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_GENERAL_PAUSEONLOCK, BM_SETCHECK, pPrefs->m_bPauseOnLock ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_GENERAL_PAUSEONSCREENSAVE, BM_SETCHECK, pPrefs->m_bPauseOnScreensave ? BST_CHECKED : BST_UNCHECKED, 0);
				
				SendDlgItemMessage(hDlg, IDC_GENERAL_TASKBAR_NORMAL, BM_SETCHECK, pPrefs->m_eTrayIconMode == TrayIconNever ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_GENERAL_TASKBAR_MINIMIZE, BM_SETCHECK, pPrefs->m_eTrayIconMode == TrayIconMinimize ? BST_CHECKED : BST_UNCHECKED, 0);

				SendDlgItemMessage(hDlg, IDC_GENERAL_MINIMIZEONCLOSE, BM_SETCHECK, pPrefs->m_bMinimizeOnClose ? BST_CHECKED : BST_UNCHECKED, 0);


				TCHAR szSize[8];
				wnsprintf(szSize, 8, TEXT("%i"), pPrefs->m_iHistoryListSize);
				SendDlgItemMessage(hDlg, IDC_GENERAL_HISTORYCOUNT, WM_SETTEXT, 0, (LPARAM)szSize);
				SendDlgItemMessage(hDlg, IDC_GENERAL_HISTORYCOUNT_SPINNER, UDM_SETRANGE, 0, (LPARAM)MAKELONG(HISTORY_MAX, 2));
				SendDlgItemMessage(hDlg, IDC_GENERAL_HISTORYCOUNT_SPINNER, UDM_SETBUDDY, (WPARAM)GetDlgItem(hDlg, IDC_GENERAL_HISTORYCOUNT), 0);
 
				wnsprintf(szSize, 8, TEXT("%i"), pPrefs->m_iFutureListSize);
				SendDlgItemMessage(hDlg, IDC_GENERAL_FUTURECOUNT, WM_SETTEXT, 0, (LPARAM)szSize);
				SendDlgItemMessage(hDlg, IDC_GENERAL_FUTURECOUNT_SPINNER, UDM_SETRANGE, 0, (LPARAM)MAKELONG(FUTURE_MAX, 2));
				SendDlgItemMessage(hDlg, IDC_GENERAL_FUTURECOUNT_SPINNER, UDM_SETBUDDY, (WPARAM)GetDlgItem(hDlg, IDC_GENERAL_FUTURECOUNT), 0);

				SendDlgItemMessage(hDlg, IDC_GENERAL_SHOWALBUMART, BM_SETCHECK, pPrefs->m_bShowAlbumArt ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_GENERAL_ARTONSELECTION, BM_SETCHECK, pPrefs->m_bArtOnSelection ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_GENERAL_SHOWVISART, BM_SETCHECK, pPrefs->m_bShowVisArt ? BST_CHECKED : BST_UNCHECKED, 0);

				EnableWindow(GetDlgItem(hDlg, IDC_GENERAL_SHOWVISART), pPrefs->m_bShowAlbumArt ? TRUE : FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_GENERAL_ARTONSELECTION), pPrefs->m_bShowAlbumArt ? TRUE : FALSE);
			}
			break;

		case WM_COMMAND:
			{
				switch (LOWORD(wParam)) 
				{
					case IDC_GENERAL_FOLLOWCURRENTSONG:
						{
							int State = SendDlgItemMessage(hDlg, IDC_GENERAL_FOLLOWCURRENTSONG, BM_GETCHECK, 0, 0);
							pPrefs->m_bFollowCurrentSong = State == BST_UNCHECKED ? FALSE : TRUE;
						}
						break;

					case IDC_GENERAL_SMARTSORTING:
						{
							int State = SendDlgItemMessage(hDlg, IDC_GENERAL_SMARTSORTING, BM_GETCHECK, 0, 0);
							pPrefs->m_bSmartSorting = State == BST_UNCHECKED ? FALSE : TRUE;
						}
						break;


					case IDC_GENERAL_AUTOSOFTPAUSE:
						{
							int State = SendDlgItemMessage(hDlg, IDC_GENERAL_AUTOSOFTPAUSE, BM_GETCHECK, 0, 0);
							pPrefs->m_bAutoSoftPause = State == BST_UNCHECKED ? FALSE : TRUE;
						}
						break;

					case IDC_GENERAL_PAUSEONLOCK:
						{
							int State = SendDlgItemMessage(hDlg, IDC_GENERAL_PAUSEONLOCK, BM_GETCHECK, 0, 0);
							pPrefs->m_bPauseOnLock = State == BST_UNCHECKED ? FALSE : TRUE;
						}
						break;

					case IDC_GENERAL_PAUSEONSCREENSAVE:
						{
							int State = SendDlgItemMessage(hDlg, IDC_GENERAL_PAUSEONSCREENSAVE, BM_GETCHECK, 0, 0);
							pPrefs->m_bPauseOnScreensave = State == BST_UNCHECKED ? FALSE : TRUE;
						}
						break;

					case IDC_GENERAL_TASKBAR_NORMAL:
						{
							pPrefs->m_eTrayIconMode = TrayIconNever;
							tuniacApp.m_Taskbar.Hide();
						}
						break;

					case IDC_GENERAL_TASKBAR_MINIMIZE:
						{
							pPrefs->m_eTrayIconMode = TrayIconMinimize;
							tuniacApp.m_Taskbar.Show();
						}
						break;

					case IDC_GENERAL_MINIMIZEONCLOSE:
						{
							int State = SendDlgItemMessage(hDlg, IDC_GENERAL_MINIMIZEONCLOSE, BM_GETCHECK, 0, 0);
							pPrefs->m_bMinimizeOnClose = State == BST_UNCHECKED ? FALSE : TRUE;
						}
						break;

					case IDC_GENERAL_HISTORYCOUNT:
						{
							TCHAR szSize[4];
							SendDlgItemMessage(hDlg, IDC_GENERAL_HISTORYCOUNT, WM_GETTEXT, 8, (LPARAM)szSize);
							int iSize = _wtoi(szSize);
							if(iSize < 2)
								iSize = 2;
							if(iSize > HISTORY_MAX)
								iSize = HISTORY_MAX;
							pPrefs->m_iHistoryListSize = iSize;
						}
						break;

					case IDC_GENERAL_FUTURECOUNT_SPINNER:
						{
							MessageBox(NULL, L"hit", L"", 0);
						}
						break;

					case IDC_GENERAL_FUTURECOUNT:
						{
							TCHAR szSize[4];
							SendDlgItemMessage(hDlg, IDC_GENERAL_FUTURECOUNT, WM_GETTEXT, 8, (LPARAM)szSize);
							int iSize = _wtoi(szSize);
							if(iSize < 2)
								iSize = 2;
							if(iSize > FUTURE_MAX)
								iSize = FUTURE_MAX;
							pPrefs->m_iFutureListSize = iSize;
						}
						break;
					
					case IDC_GENERAL_SHOWALBUMART:
						{
							int State = SendDlgItemMessage(hDlg, IDC_GENERAL_SHOWALBUMART, BM_GETCHECK, 0, 0);
							pPrefs->m_bShowAlbumArt = State == BST_UNCHECKED ? FALSE : TRUE;

							if(pPrefs->m_bShowAlbumArt && pPrefs->m_bShowVisArt)
								tuniacApp.m_VisualWindow->Show();
							else
								if(wcscmp(tuniacApp.GetActiveScreenName(), L"Visuals") != 0)
									tuniacApp.m_VisualWindow->Hide();

							tuniacApp.m_SourceSelectorWindow->ToggleAlbumArt(pPrefs->m_bShowAlbumArt);

							EnableWindow(GetDlgItem(hDlg, IDC_GENERAL_SHOWVISART), pPrefs->m_bShowAlbumArt ? TRUE : FALSE);
							EnableWindow(GetDlgItem(hDlg, IDC_GENERAL_ARTONSELECTION), pPrefs->m_bShowAlbumArt ? TRUE : FALSE);
						}
						break;

					case IDC_GENERAL_ARTONSELECTION:
						{
							int State = SendDlgItemMessage(hDlg, IDC_GENERAL_ARTONSELECTION, BM_GETCHECK, 0, 0);
							pPrefs->m_bArtOnSelection = State == BST_UNCHECKED ? FALSE : TRUE;
						}
						break;

					case IDC_GENERAL_SHOWVISART:
						{
							int State = SendDlgItemMessage(hDlg, IDC_GENERAL_SHOWVISART, BM_GETCHECK, 0, 0);
							pPrefs->m_bShowVisArt = State == BST_UNCHECKED ? FALSE : TRUE;
							tuniacApp.m_SourceSelectorWindow->ToggleAlbumArt(pPrefs->m_bShowAlbumArt);
							if(pPrefs->m_bShowVisArt)
								tuniacApp.m_VisualWindow->Show();
							else
								tuniacApp.m_VisualWindow->Hide();
						}
						break;

				}
			}
			break;

		default:
			return false;
			break;
	}

	return true;
}

LRESULT CALLBACK CPreferences::FormattingProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPreferences * pPrefs = (CPreferences *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
				pPrefs = (CPreferences *)lParam;

				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, CB_RESETCONTENT, 0, 0);
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, WM_SETTEXT, 0, (LPARAM)pPrefs->m_szWindowFormatString);
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@T - @A [Tuniac]"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@!: @T - @A [Tuniac]"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("(@I) @T - @A [Tuniac]"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@T - @L - @A [Tuniac]"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@U [Tuniac]"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@F - @I [Tuniac]"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("Tuniac - @!: @F (@I)"));

				SendDlgItemMessage(hDlg, IDC_FORMATTING_PLUGINFORMAT, CB_RESETCONTENT, 0, 0);
				SendDlgItemMessage(hDlg, IDC_FORMATTING_PLUGINFORMAT, WM_SETTEXT, 0, (LPARAM)pPrefs->m_szPluginFormatString);
				SendDlgItemMessage(hDlg, IDC_FORMATTING_PLUGINFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@T - @A"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_PLUGINFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@F"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_PLUGINFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("[@#][@T - @A][@L - @Y][@I|@B|@S]"));

				SendDlgItemMessage(hDlg, IDC_FORMATTING_LISTFORMAT, CB_RESETCONTENT, 0, 0);
				SendDlgItemMessage(hDlg, IDC_FORMATTING_LISTFORMAT, WM_SETTEXT, 0, (LPARAM)pPrefs->m_szListFormatString);
				SendDlgItemMessage(hDlg, IDC_FORMATTING_LISTFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@T - @A"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_LISTFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@F"));


				pPrefs->m_hTextFormatToolTip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
												WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
												CW_USEDEFAULT, CW_USEDEFAULT,
												CW_USEDEFAULT, CW_USEDEFAULT,
												GetDlgItem(hDlg, IDC_FORMATTING_FORMATSTRING_HELP),
												NULL, tuniacApp.getMainInstance(),
												NULL);
				SetWindowPos(pPrefs->m_hTextFormatToolTip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
				TOOLINFO ti;
				ti.cbSize = sizeof(ti);
				GetClientRect(GetDlgItem(hDlg, IDC_FORMATTING_FORMATSTRING_HELP), &ti.rect);
				ti.hinst = tuniacApp.getMainInstance();
				ti.lpszText = FORMATSTRING_HELP;
				ti.hwnd = GetDlgItem(hDlg, IDC_FORMATTING_FORMATSTRING_HELP);
				ti.uFlags = TTF_SUBCLASS;
				SendMessage(pPrefs->m_hTextFormatToolTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
				SendMessage(pPrefs->m_hTextFormatToolTip, TTM_SETMAXTIPWIDTH, 0, 300);
				SendMessage(pPrefs->m_hTextFormatToolTip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 600000);
				SendMessage(pPrefs->m_hTextFormatToolTip, TTM_SETDELAYTIME, TTDT_INITIAL, 0);
				SendMessage(pPrefs->m_hTextFormatToolTip, TTM_SETDELAYTIME, TTDT_RESHOW, 0);
			}
			break;

		case WM_COMMAND:
			{
				switch (LOWORD(wParam)) 
				{

					case IDC_FORMATTING_WINDOWTITLEFORMAT:
						{
							SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, WM_GETTEXT, 256, (LPARAM)pPrefs->m_szWindowFormatString);
						}
						break;
					case IDC_FORMATTING_PLUGINFORMAT:
						{
							SendDlgItemMessage(hDlg, IDC_FORMATTING_PLUGINFORMAT, WM_GETTEXT, 256, (LPARAM)pPrefs->m_szPluginFormatString);
						}
						break;

					case IDC_FORMATTING_LISTFORMAT:
						{
							SendDlgItemMessage(hDlg, IDC_FORMATTING_LISTFORMAT, WM_GETTEXT, 256, (LPARAM)pPrefs->m_szListFormatString);
						}
						break;

					case IDC_FORMATTING_FORMATSTRING_HELP:
						{
							SendMessage(pPrefs->m_hTextFormatToolTip, TTM_POP, 0, 0);
							MessageBox(hDlg, FORMATSTRING_HELP, TEXT("Help"), MB_OK | MB_ICONINFORMATION);
						}
						break;

				}
			}
			break;

		default:
			return false;
			break;
	}

	return true;

}

LRESULT CALLBACK CPreferences::PluginsProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPreferences * pPrefs = (CPreferences *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
				pPrefs = (CPreferences *)lParam;

				HWND hListView = GetDlgItem(hDlg, IDC_PLUGINS_LIST);
				ListView_SetExtendedListViewStyle(hListView, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

				LVCOLUMN	lvC;
				lvC.mask	= LVCF_WIDTH | LVCF_TEXT;
				lvC.cx		= 305;
				lvC.pszText	= TEXT("");
				ListView_InsertColumn(hListView, 0, &lvC);

				LVITEM item;
				item.mask = LVIF_TEXT;
				item.iSubItem = 0;

				for(unsigned int i = 0; i < tuniacApp.m_PluginManager.GetNumPlugins(); i++)
				{
					PluginEntry * pPE = tuniacApp.m_PluginManager.GetPluginAtIndex(i);
					if(pPE == NULL) break;

					TCHAR szItem[128];
					wnsprintf(szItem, 128, TEXT("%s (%s)"), pPE->szName, pPE->szDllFile);

					item.pszText = szItem;
					item.iItem = i;
					ListView_InsertItem(hListView, &item);
				}
				SendMessage(hDlg, WM_USER, 0, 0);

			}
			break;

		case WM_USER:
			{
				HWND hList = GetDlgItem(hDlg, IDC_PLUGINS_LIST);

				for(unsigned int i = 0; i < tuniacApp.m_PluginManager.GetNumPlugins(); i++)
				{
					ListView_SetCheckState(hList, i, tuniacApp.m_PluginManager.IsPluginEnabled(i) ? TRUE : FALSE);
				}
			}
			break;

		case WM_NOTIFY:
			{
				UINT idCtrl = wParam;
				LPNMHDR lpNotify = (LPNMHDR)lParam;

				switch(lpNotify->code)
				{
					case LVN_ITEMCHANGED:
						{
							LPNMLISTVIEW lpView = (LPNMLISTVIEW)lpNotify;
							int iSel = ListView_GetNextItem(lpView->hdr.hwndFrom, -1, LVNI_SELECTED);
							if(iSel >= 0)
							{
								PluginEntry * pPE = tuniacApp.m_PluginManager.GetPluginAtIndex(iSel);

								if(pPE != NULL && pPE->ulFlags & PLUGINFLAGS_ABOUT)
								{
									EnableWindow(GetDlgItem(hDlg, IDC_PLUGINS_ABOUT), TRUE);
								}
								else
								{
									EnableWindow(GetDlgItem(hDlg, IDC_PLUGINS_ABOUT), FALSE);
								}

								if(pPE != NULL && pPE->ulFlags & PLUGINFLAGS_CONFIG)
								{
									EnableWindow(GetDlgItem(hDlg, IDC_PLUGINS_CONFIGURE), TRUE);
								}
								else
								{
									EnableWindow(GetDlgItem(hDlg, IDC_PLUGINS_CONFIGURE), FALSE);
								}


							}
							else
							{
								EnableWindow(GetDlgItem(hDlg, IDC_PLUGINS_ABOUT), FALSE);
								EnableWindow(GetDlgItem(hDlg, IDC_PLUGINS_CONFIGURE), FALSE);
							}
							
							if(lpView->iItem == -1) break;

							if((lpView->uChanged & LVIF_STATE) && lpView->uOldState && (lpView->uNewState & LVIS_STATEIMAGEMASK))
							{
								bool bChecked = ListView_GetCheckState(lpView->hdr.hwndFrom, lpView->iItem);
								if(bChecked != tuniacApp.m_PluginManager.IsPluginEnabled(lpView->iItem))
								{
									tuniacApp.m_PluginManager.EnablePlugin(lpView->iItem, bChecked);
									SendMessage(hDlg, WM_USER, 0, 0);
								}
							}

						}
						break;
				}
			}
			break;

		case WM_COMMAND:
			{
				WORD wCmdID = LOWORD(wParam);
				HWND hList = GetDlgItem(hDlg, IDC_PLUGINS_LIST);

				switch(wCmdID)
				{
					case IDC_PLUGINS_ABOUT:
						{
							int iSel = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
							if(iSel < 0) break;
							PluginEntry * pPE = tuniacApp.m_PluginManager.GetPluginAtIndex(iSel);
							if(pPE == NULL || !(pPE->ulFlags & PLUGINFLAGS_ABOUT)) break;

							if(pPE->pPlugin == NULL)
							{
								if(IDYES == MessageBox(hDlg, TEXT("Plugin must be loaded to do this.\n\nEnable plugin now?"), TEXT(""), MB_YESNO | MB_ICONINFORMATION))
								{
									tuniacApp.m_PluginManager.EnablePlugin(iSel, TRUE);
									SendMessage(hDlg, WM_USER, 0, 0);
								}
							}
							if(pPE->pPlugin != NULL)
								pPE->pPlugin->About(hDlg);
						}
						break;

					case IDC_PLUGINS_CONFIGURE:
						{
							int iSel = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
							if(iSel < 0) break;
							PluginEntry * pPE = tuniacApp.m_PluginManager.GetPluginAtIndex(iSel);
							if(pPE == NULL || !(pPE->ulFlags & PLUGINFLAGS_CONFIG)) break;

							if(pPE->pPlugin == NULL)
							{
								if(IDOK == MessageBox(hDlg, TEXT("Plugin must be loaded to do this.\n\nEnable plugin now?"), TEXT(""), MB_YESNO | MB_ICONINFORMATION))
								{
									tuniacApp.m_PluginManager.EnablePlugin(iSel, TRUE);
									SendMessage(hDlg, WM_USER, 0, 0);
								}
							}
							if(pPE->pPlugin != NULL)
								pPE->pPlugin->Configure(hDlg);
						}
						break;

					case IDC_PLUGINS_ENABLEALL:
					case IDC_PLUGINS_DISABLEALL:
						{
							for(unsigned int i = 0; i < tuniacApp.m_PluginManager.GetNumPlugins(); i++)
							{
								tuniacApp.m_PluginManager.EnablePlugin(i, wCmdID == IDC_PLUGINS_ENABLEALL);
							}
							SendMessage(hDlg, WM_USER, 0, 0);
						}
						break;
				}	
			}
			break;
		default:
			return false;
			break;
	}

	return true;
}

LRESULT CALLBACK CPreferences::AudioProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPreferences * pPrefs = (CPreferences *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
				pPrefs = (CPreferences *)lParam;

				//RECT r;
				//POINT pt;
				//GetWindowRect(GetDlgItem(GetParent(hDlg), IDCANCEL), &r);
				//pt.x = r.left;
				//pt.y = r.top;
				//ScreenToClient(GetParent(hDlg), &pt);

				//ShowWindow(GetDlgItem(GetParent(hDlg), IDCANCEL), SW_HIDE);
				//SetWindowPos(GetDlgItem(GetParent(hDlg), IDOK), NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				TCHAR		tstr[256];
				wsprintf(tstr, TEXT("Crossfade for %d seconds"), pPrefs->m_iCrossfadeTime);
				SetDlgItemText(hDlg, IDC_CROSSFADE_TIME_TEXT, tstr);
				SendDlgItemMessage(hDlg, IDC_CROSSFADE_TIME_SLIDER, TBM_SETRANGE,	TRUE, MAKELONG(2, 15));
				SendDlgItemMessage(hDlg, IDC_CROSSFADE_TIME_SLIDER, TBM_SETPOS,		TRUE, pPrefs->m_iCrossfadeTime);

				EnableWindow(GetDlgItem(hDlg, IDC_CROSSFADE_TIME_TEXT), pPrefs->m_bCrossfadeEnabled ? TRUE : FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_CROSSFADE_TIME_SLIDER), pPrefs->m_bCrossfadeEnabled ? TRUE : FALSE);
				SendDlgItemMessage(hDlg, IDC_CROSSFADE_ENABLE, BM_SETCHECK, pPrefs->m_bCrossfadeEnabled ? BST_CHECKED : BST_UNCHECKED, 0);

				wsprintf(tstr, TEXT("Buffer length %d milliseconds"), pPrefs->m_iAudioBuffering);
				SetDlgItemText(hDlg, IDC_BUFFER_TIME_TEXT, tstr);
				SendDlgItemMessage(hDlg, IDC_BUFFER_TIME_SLIDER, TBM_SETRANGE,	TRUE, MAKELONG(250, 5000));
				SendDlgItemMessage(hDlg, IDC_BUFFER_TIME_SLIDER, TBM_SETPOS,	TRUE, pPrefs->m_iAudioBuffering);


				EnableWindow(GetDlgItem(hDlg, IDC_AUDIO_REPLAYGAINALBUM), pPrefs->ReplayGainEnabled() ? TRUE : FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_AUDIO_AMPGAINTEXT), pPrefs->ReplayGainEnabled() ? TRUE : FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_AUDIO_AMPGAINSLIDER), pPrefs->ReplayGainEnabled() ? TRUE : FALSE);
				SendDlgItemMessage(hDlg, IDC_AUDIO_REPLAYGAIN, BM_SETCHECK, pPrefs->ReplayGainEnabled() ? BST_CHECKED : BST_UNCHECKED, 0);

				SendDlgItemMessage(hDlg, IDC_AUDIO_REPLAYGAINALBUM, BM_SETCHECK, pPrefs->ReplayGainUseAlbumGain() ? BST_CHECKED : BST_UNCHECKED, 0);

				_snwprintf(tstr, 42, TEXT("Reduce nonreplaygain files by %1.2f db"), pPrefs->m_fAmpGain);
				SetDlgItemText(hDlg, IDC_AUDIO_AMPGAINTEXT, tstr);
				SendDlgItemMessage(hDlg, IDC_AUDIO_AMPGAINSLIDER, TBM_SETRANGE,	TRUE, MAKELONG(-150, 150));
				SendDlgItemMessage(hDlg, IDC_AUDIO_AMPGAINSLIDER, TBM_SETPOS,	TRUE, (pPrefs->m_fAmpGain*10.0f));
			}
			break;

		case WM_COMMAND:
			{
				WORD wCmdID = LOWORD(wParam);

				switch(wCmdID)
				{
					case IDC_CROSSFADE_ENABLE:
						{
							int State = SendDlgItemMessage(hDlg, IDC_CROSSFADE_ENABLE, BM_GETCHECK, 0, 0);
							pPrefs->m_bCrossfadeEnabled = State == BST_UNCHECKED ? FALSE : TRUE;
							EnableWindow(GetDlgItem(hDlg, IDC_CROSSFADE_TIME_TEXT), pPrefs->m_bCrossfadeEnabled);
							EnableWindow(GetDlgItem(hDlg, IDC_CROSSFADE_TIME_SLIDER), pPrefs->m_bCrossfadeEnabled);
						}
						break;

					case IDC_AUDIO_REPLAYGAIN:
						{
							int State = SendDlgItemMessage(hDlg, IDC_AUDIO_REPLAYGAIN, BM_GETCHECK, 0, 0);
							pPrefs->m_bReplayGain = State == BST_UNCHECKED ? FALSE : TRUE;
							EnableWindow(GetDlgItem(hDlg, IDC_AUDIO_REPLAYGAINALBUM), pPrefs->m_bReplayGain);
							EnableWindow(GetDlgItem(hDlg, IDC_AUDIO_AMPGAINTEXT), pPrefs->m_bReplayGain);
							EnableWindow(GetDlgItem(hDlg, IDC_AUDIO_AMPGAINSLIDER), pPrefs->m_bReplayGain);

							CCoreAudio::Instance()->EnableReplayGain(pPrefs->m_bReplayGain);
							CCoreAudio::Instance()->ReplayGainUseAlbumGain(pPrefs->m_bReplayGainAlbum);
						}
						break;

					case IDC_AUDIO_REPLAYGAINALBUM:
						{
							int State = SendDlgItemMessage(hDlg, IDC_AUDIO_REPLAYGAINALBUM, BM_GETCHECK, 0, 0);
							pPrefs->m_bReplayGainAlbum = State == BST_UNCHECKED ? FALSE : TRUE;

							CCoreAudio::Instance()->EnableReplayGain(pPrefs->m_bReplayGain);
							CCoreAudio::Instance()->ReplayGainUseAlbumGain(pPrefs->m_bReplayGainAlbum);
						}
						break;
				}
			}
			break;

		case WM_HSCROLL:
			{
				switch (LOWORD(wParam)) 
				{
					case TB_THUMBTRACK:
					case TB_ENDTRACK:
						{
							TCHAR		tstr[256];

							pPrefs->m_iCrossfadeTime = SendDlgItemMessage(hDlg, IDC_CROSSFADE_TIME_SLIDER, TBM_GETPOS, 0, 0); 
							wsprintf(tstr, TEXT("Crossfade for %d seconds"), pPrefs->m_iCrossfadeTime);
							SetDlgItemText(hDlg, IDC_CROSSFADE_TIME_TEXT, tstr);

							pPrefs->m_iAudioBuffering = SendDlgItemMessage(hDlg, IDC_BUFFER_TIME_SLIDER, TBM_GETPOS, 0, 0); 
							wsprintf(tstr, TEXT("Buffer length %d milliseconds"), pPrefs->m_iAudioBuffering);
							SetDlgItemText(hDlg, IDC_BUFFER_TIME_TEXT, tstr);
							CCoreAudio::Instance()->SetAudioBufferSize(pPrefs->m_iAudioBuffering);

							pPrefs->m_fAmpGain = SendDlgItemMessage(hDlg, IDC_AUDIO_AMPGAINSLIDER, TBM_GETPOS, 0, 0) / 10.0f; 
							_snwprintf(tstr, 42, TEXT("Reduce nonreplaygain files by %1.2f db"), pPrefs->m_fAmpGain);
							SetDlgItemText(hDlg, IDC_AUDIO_AMPGAINTEXT, tstr);
							CCoreAudio::Instance()->SetAmpGain(pPrefs->m_fAmpGain);
						}
						break;
				}
			}
			break;

		default:
			return false;
			break;
	}

	return true;
}

LRESULT CALLBACK CPreferences::EQProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPreferences * pPrefs = (CPreferences *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{

				SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
				pPrefs = (CPreferences *)lParam;

				SendDlgItemMessage(hDlg, IDC_EQLOW_SLIDER, TBM_SETRANGE,	TRUE, MAKELONG(0, 2000));
				SendDlgItemMessage(hDlg, IDC_EQLOW_SLIDER, TBM_SETPOS,		TRUE, (pPrefs->m_fEQLow * 1000.0f));

				SendDlgItemMessage(hDlg, IDC_EQMID_SLIDER, TBM_SETRANGE,	TRUE, MAKELONG(0, 2000));
				SendDlgItemMessage(hDlg, IDC_EQMID_SLIDER, TBM_SETPOS,		TRUE, (pPrefs->m_fEQMid * 1000.0f));

				SendDlgItemMessage(hDlg, IDC_EQHIGH_SLIDER, TBM_SETRANGE,	TRUE, MAKELONG(0, 2000));
				SendDlgItemMessage(hDlg, IDC_EQHIGH_SLIDER, TBM_SETPOS,		TRUE, (pPrefs->m_fEQHigh * 1000.0f));

				SendDlgItemMessage(hDlg, IDC_EQ_ENABLE, BM_SETCHECK, pPrefs->m_bEQEnabled ? BST_CHECKED : BST_UNCHECKED, 0);

			}
			break;

		case WM_COMMAND:
			{
				WORD wCmdID = LOWORD(wParam);

				switch(wCmdID)
				{
					case IDC_EQRESET_BUTTON:
						{
							SendDlgItemMessage(hDlg, IDC_EQLOW_SLIDER, TBM_SETPOS,		TRUE, 1000);
							SendDlgItemMessage(hDlg, IDC_EQMID_SLIDER, TBM_SETPOS,		TRUE, 1000);
							SendDlgItemMessage(hDlg, IDC_EQHIGH_SLIDER, TBM_SETPOS,		TRUE, 1000);
							pPrefs->m_fEQLow = 1.0f; 
							pPrefs->m_fEQMid = 1.0f; 
							pPrefs->m_fEQHigh = 1.0f; 
							CCoreAudio::Instance()->SetEQGain(pPrefs->m_fEQLow, pPrefs->m_fEQMid, pPrefs->m_fEQHigh);
						}
						break;

					case IDC_EQ_ENABLE:
						{
							int State = SendDlgItemMessage(hDlg, IDC_EQ_ENABLE, BM_GETCHECK, 0, 0);
							pPrefs->m_bEQEnabled = State == BST_UNCHECKED ? FALSE : TRUE;
							CCoreAudio::Instance()->EnableEQ(State);
						}
						break;
				}

			}
			break;

		case WM_HSCROLL:
			{
				switch (LOWORD(wParam)) 
				{
					case TB_THUMBTRACK:
					case TB_ENDTRACK:
						{
							pPrefs->m_fEQLow = ((float)SendDlgItemMessage(hDlg, IDC_EQLOW_SLIDER, TBM_GETPOS, 0, 0) / 1000.0f); 
							pPrefs->m_fEQMid = ((float)SendDlgItemMessage(hDlg, IDC_EQMID_SLIDER, TBM_GETPOS, 0, 0) / 1000.0f); 
							pPrefs->m_fEQHigh = ((float)SendDlgItemMessage(hDlg, IDC_EQHIGH_SLIDER, TBM_GETPOS, 0, 0) / 1000.0f); 
							CCoreAudio::Instance()->SetEQGain(pPrefs->m_fEQLow, pPrefs->m_fEQMid, pPrefs->m_fEQHigh);
						}
						break;
				}
			}
			break;

		default:
			return false;
			break;
	}

	return true;
}

LRESULT CALLBACK CPreferences::CoreAudioProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPreferences * pPrefs = (CPreferences *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
				pPrefs = (CPreferences *)lParam;

				SendDlgItemMessage(hDlg, IDC_AUDIOPLUGINS_LIST, LB_RESETCONTENT, 0, 0);

				for(unsigned long x=0; x<CCoreAudio::Instance()->GetNumPlugins(); x++)
				{
					SendDlgItemMessage(hDlg, IDC_AUDIOPLUGINS_LIST, LB_INSERTSTRING, -1, (LPARAM)CCoreAudio::Instance()->GetPluginAtIndex(x)->GetName());
				}
			}
			break;

		case WM_COMMAND:
			{
				WORD wCmdID = LOWORD(wParam);

				switch(wCmdID)
				{
					case IDC_AUDIOPLUGIN_ABOUT:
						{
							unsigned long sel = SendDlgItemMessage(hDlg, IDC_AUDIOPLUGINS_LIST, LB_GETCURSEL, 0, 0);
							CCoreAudio::Instance()->GetPluginAtIndex(sel)->About(hDlg);
						}
						break;

					case IDC_AUDIOPLUGIN_CONFIGURE:
						{
							unsigned long sel = SendDlgItemMessage(hDlg, IDC_AUDIOPLUGINS_LIST, LB_GETCURSEL, 0, 0);
							CCoreAudio::Instance()->GetPluginAtIndex(sel)->Configure(hDlg);
						}
						break;

					case IDC_AUDIOPLUGINS_LIST:
						{
							unsigned long sel = SendDlgItemMessage(hDlg, IDC_AUDIOPLUGINS_LIST, LB_GETCURSEL, 0, 0);

							if(sel == LB_ERR)
							{
								EnableWindow(GetDlgItem(hDlg, IDC_AUDIOPLUGIN_ABOUT), FALSE);
								EnableWindow(GetDlgItem(hDlg, IDC_AUDIOPLUGIN_CONFIGURE), FALSE);
							}
							else
							{
								if(CCoreAudio::Instance()->GetPluginAtIndex(sel)->GetFlags() & FLAGS_ABOUT)
								{
									EnableWindow(GetDlgItem(hDlg, IDC_AUDIOPLUGIN_ABOUT), TRUE);
								}
								else
								{
									EnableWindow(GetDlgItem(hDlg, IDC_AUDIOPLUGIN_ABOUT), FALSE);
								}

								if(CCoreAudio::Instance()->GetPluginAtIndex(sel)->GetFlags() & FLAGS_CONFIGURE)
								{
									EnableWindow(GetDlgItem(hDlg, IDC_AUDIOPLUGIN_CONFIGURE), TRUE);
								}
								else
								{
									EnableWindow(GetDlgItem(hDlg, IDC_AUDIOPLUGIN_CONFIGURE), FALSE);
								}
							}

						}
						break;
				}
			}
			break;

		default:
			return false;
			break;
	}

	return true;
}

LRESULT CALLBACK CPreferences::LibraryProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPreferences * pPrefs = (CPreferences *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
				pPrefs = (CPreferences *)lParam;

				SendDlgItemMessage(hDlg, IDC_FILETIMETODATEADDED, BM_SETCHECK, pPrefs->m_bSetDateAddedToFileCreationTime ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_LIBRARY_PLAYLISTSORTING, BM_SETCHECK, pPrefs->m_bPlaylistSorting ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_LIBRARY_AUTOADDPLAYLIST, BM_SETCHECK, pPrefs->m_bAutoAddPlaylist ? BST_CHECKED : BST_UNCHECKED, 0);
			}
			break;

		case WM_COMMAND:
			{
				WORD wCmdID = LOWORD(wParam);

				switch(wCmdID)
				{
					case IDC_LIBRARY_FILETIMETODATEADDED:
						{
							int State = SendDlgItemMessage(hDlg, IDC_LIBRARY_FILETIMETODATEADDED, BM_GETCHECK, 0, 0);
							pPrefs->m_bSetDateAddedToFileCreationTime = State == BST_UNCHECKED ? FALSE : TRUE;
						}
						break;

					case IDC_LIBRARY_PLAYLISTSORTING:
						{
							int State = SendDlgItemMessage(hDlg, IDC_LIBRARY_PLAYLISTSORTING, BM_GETCHECK, 0, 0);
							pPrefs->m_bPlaylistSorting = State == BST_UNCHECKED ? FALSE : TRUE;
						}
						break;


					case IDC_LIBRARY_AUTOADDPLAYLIST:
						{
							int State = SendDlgItemMessage(hDlg, IDC_LIBRARY_AUTOADDPLAYLIST, BM_GETCHECK, 0, 0);
							pPrefs->m_bAutoAddPlaylist = State == BST_UNCHECKED ? FALSE : TRUE;
						}
						break;

					case IDC_REBUILD_ML:
						{
							//do update
							for(unsigned long ulMLIndex = 0; ulMLIndex < tuniacApp.m_MediaLibrary.GetCount(); ulMLIndex++)
							{
								if(!tuniacApp.m_MediaLibrary.UpdateMLIndex(ulMLIndex))
								{
									IPlaylistEntry * pIPE = tuniacApp.m_MediaLibrary.GetItemByIndex(ulMLIndex);
									unsigned long ulEntryID = pIPE->GetEntryID();
									for(unsigned long list = 0; list < tuniacApp.m_PlaylistManager.GetNumPlaylists(); list++)
									{
										IPlaylist * pPlaylist = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(list);
										IPlaylistEX * pPlaylistEX = (IPlaylistEX *)pPlaylist;
										pPlaylistEX->DeleteAllItemsWhereIDEquals(ulEntryID);
									}
									tuniacApp.m_MediaLibrary.RemoveItem(pIPE);
									tuniacApp.m_PlaylistManager.m_LibraryPlaylist.RebuildPlaylist();
									tuniacApp.m_PlaylistManager.m_LibraryPlaylist.ApplyFilter();
								}
							}

							for(unsigned long ulRealIndex = 0; ulRealIndex < tuniacApp.m_PlaylistManager.m_LibraryPlaylist.GetRealCount(); ulRealIndex++)
							{
								tuniacApp.m_PlaylistManager.m_LibraryPlaylist.UpdateIndex(ulRealIndex);
							}



							for(unsigned long list = 0; list < tuniacApp.m_PlaylistManager.m_StandardPlaylists.GetCount(); list++)
							{
								for(unsigned long ulRealIndex = 0; ulRealIndex < tuniacApp.m_PlaylistManager.m_StandardPlaylists[list]->GetRealCount(); ulRealIndex++)
								{
									tuniacApp.m_PlaylistManager.m_StandardPlaylists[list]->UpdateIndex(ulRealIndex);
								}
							}
							tuniacApp.m_SourceSelectorWindow->UpdateView();
						}
						break;

				}
			}
			break;

		default:
			return false;
			break;
	}

	return true;
}


LRESULT CALLBACK CPreferences::VisualsProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPreferences * pPrefs = (CPreferences *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
				pPrefs = (CPreferences *)lParam;

				SendDlgItemMessage(hDlg, IDC_VISUAL_FPS, TBM_SETRANGE, TRUE, MAKELONG(25, 100));
				SendDlgItemMessage(hDlg, IDC_VISUAL_FPS, TBM_SETPOS,	TRUE, pPrefs->m_iVisualFPS);

				TCHAR	tstr[32];
				wsprintf(tstr, TEXT("%d FPS"), pPrefs->m_iVisualFPS);
				SetDlgItemText(hDlg, IDC_VISUAL_FPSDISPLAY, tstr);
			}
			break;

		case WM_HSCROLL:
			{
				switch (LOWORD(wParam)) 
				{
					case TB_THUMBTRACK:
					case TB_ENDTRACK:
						{
							pPrefs->m_iVisualFPS = SendDlgItemMessage(hDlg, IDC_VISUAL_FPS, TBM_GETPOS, 0, 0);

							TCHAR	tstr[32];
							wsprintf(tstr, TEXT("%d FPS"), pPrefs->m_iVisualFPS);
							SetDlgItemText(hDlg, IDC_VISUAL_FPSDISPLAY, tstr);
						}
						break;
				}
			}
			break;

		default:
			return false;
			break;
	}

	return true;
}

LRESULT CALLBACK CPreferences::FileAssocProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CPreferences * pPrefs = (CPreferences *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)lParam);
				pPrefs = (CPreferences *)lParam;

				SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_OPEN, BM_SETCHECK, pPrefs->m_iFileAssocType & FILEASSOC_TYPE_OPEN ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_PLAY, BM_SETCHECK, pPrefs->m_iFileAssocType & FILEASSOC_TYPE_PLAY ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_QUEUE, BM_SETCHECK, pPrefs->m_iFileAssocType & FILEASSOC_TYPE_QUEUE ? BST_CHECKED : BST_UNCHECKED, 0);

				SendMessage(hDlg, WM_COMMAND, MAKELONG(IDC_FILEASSOC_TYPEDEFAULT_UPDATE, 0), 0);
			}
			break;

		case WM_COMMAND:
			{
				WORD wCmdID = LOWORD(wParam);

				switch(wCmdID)
				{
					case IDC_FILEASSOC_REGASSOC:
						{
							pPrefs->m_FileAssoc.ReAssociate(pPrefs->m_iFileAssocType);

							int iSel = SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPEDEFAULT, CB_GETCURSEL, 0, 0);
							int iType = iSel == 1 ? FILEASSOC_TYPE_PLAY : (iSel == 2 ? FILEASSOC_TYPE_QUEUE : FILEASSOC_TYPE_OPEN);
							pPrefs->m_FileAssoc.SetDefaultType(iType);

							MessageBox(hDlg, TEXT("File associations have been applied."), TEXT("Tuniac"), MB_OK | MB_ICONINFORMATION);
						}
						break;

					case IDC_FILEASSOC_SETDEFAULT:
						{
							OSVERSIONINFO osvi;
							ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
							osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

							GetVersionEx(&osvi);

							if(osvi.dwMajorVersion >= 6)
							{
								HRESULT res;
								IApplicationAssociationRegistrationUI *aarui;
								res = CoCreateInstance(
									CLSID_ApplicationAssociationRegistrationUI,
									0,
									CLSCTX_INPROC_SERVER,
									IID_IApplicationAssociationRegistrationUI,
									(LPVOID*)&aarui);

								if (SUCCEEDED(res) && aarui != 0)
								{
									// We can call the Vista-style UI for registrations
									aarui->LaunchAdvancedAssociationUI(L"Tuniac");
									aarui->Release();
								}
							}
						}
						break;

					case IDC_FILEASSOC_TYPE_OPEN:
						{
							int State = SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_OPEN, BM_GETCHECK, 0, 0);
							if(State == BST_CHECKED)
							{
								pPrefs->m_iFileAssocType |= FILEASSOC_TYPE_OPEN;
							}
							else
							{
								pPrefs->m_iFileAssocType &= ~FILEASSOC_TYPE_OPEN;
								if(SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_PLAY, BM_GETCHECK, 0, 0) == BST_UNCHECKED
									&& SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_QUEUE, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
								{
									SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_OPEN, BM_SETCHECK, BST_CHECKED, 0);
									SendMessage(hDlg, WM_COMMAND, MAKELONG(IDC_FILEASSOC_TYPE_OPEN, 0), 0);
								}
							}
							SendMessage(hDlg, WM_COMMAND, MAKELONG(IDC_FILEASSOC_TYPEDEFAULT_UPDATE, 0), 0);
						}
						break;

					case IDC_FILEASSOC_TYPE_PLAY:
						{
							int State = SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_PLAY, BM_GETCHECK, 0, 0);
							if(State == BST_CHECKED)
							{
								pPrefs->m_iFileAssocType |= FILEASSOC_TYPE_PLAY;
							}
							else
							{
								pPrefs->m_iFileAssocType &= ~FILEASSOC_TYPE_PLAY;
								if(SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_OPEN, BM_GETCHECK, 0, 0) == BST_UNCHECKED
									&& SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_QUEUE, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
								{
									SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_OPEN, BM_SETCHECK, BST_CHECKED, 0);
									SendMessage(hDlg, WM_COMMAND, MAKELONG(IDC_FILEASSOC_TYPE_OPEN, 0), 0);
								}
							}
							SendMessage(hDlg, WM_COMMAND, MAKELONG(IDC_FILEASSOC_TYPEDEFAULT_UPDATE, 0), 0);
						}
						break;

					case IDC_FILEASSOC_TYPE_QUEUE:
						{
							int State = SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_QUEUE, BM_GETCHECK, 0, 0);
							if(State == BST_CHECKED)
							{
								pPrefs->m_iFileAssocType |= FILEASSOC_TYPE_QUEUE;
							}
							else
							{
								pPrefs->m_iFileAssocType &= ~FILEASSOC_TYPE_QUEUE;
								if(SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_OPEN, BM_GETCHECK, 0, 0) == BST_UNCHECKED
									&& SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_QUEUE, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
								{
									SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_OPEN, BM_SETCHECK, BST_CHECKED, 0);
									SendMessage(hDlg, WM_COMMAND, MAKELONG(IDC_FILEASSOC_TYPE_OPEN, 0), 0);
								}
							}
							SendMessage(hDlg, WM_COMMAND, MAKELONG(IDC_FILEASSOC_TYPEDEFAULT_UPDATE, 0), 0);
						}
						break;

					case IDC_FILEASSOC_TYPEDEFAULT_UPDATE:
						{
							SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPEDEFAULT, CB_RESETCONTENT, 0, 0);
							int iType = pPrefs->m_FileAssoc.GetDefaultType();
							int iSel = 0, iCount = 0;

							if(pPrefs->m_iFileAssocType & FILEASSOC_TYPE_OPEN)
							{
								SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPEDEFAULT, CB_ADDSTRING, 0, (LPARAM)TEXT("Open"));
								iCount++;
							}
							if(pPrefs->m_iFileAssocType & FILEASSOC_TYPE_PLAY)
							{
								SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPEDEFAULT, CB_ADDSTRING, 0, (LPARAM)TEXT("Play"));
								if(iType == FILEASSOC_TYPE_PLAY)
									iSel = iCount;
								iCount++;
							}
							if(pPrefs->m_iFileAssocType & FILEASSOC_TYPE_QUEUE)
							{
								SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPEDEFAULT, CB_ADDSTRING, 0, (LPARAM)TEXT("Queue"));
								if(iType == FILEASSOC_TYPE_QUEUE)
									iSel = iCount;
								iCount++;
							}

							SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPEDEFAULT, CB_SETCURSEL, iSel, 0);

						}
						break;

				}
			}
			break;

		default:
			return false;
			break;
	}

	return true;
}

DLGTEMPLATE * LockDlgRes(unsigned long ulRes)
{
	HRSRC hrsrc = FindResource(NULL, MAKEINTRESOURCE(ulRes), RT_DIALOG); 
	HGLOBAL hglb = LoadResource(tuniacApp.getMainInstance(), hrsrc); 
	return (DLGTEMPLATE *) LockResource(hglb); 
}

CPreferences::CPreferences(void)
{
	// root pages
	m_Pages[0].pszName = TEXT("General");
	m_Pages[0].pDialogFunc = (DLGPROC)&GeneralProc;
	m_Pages[0].iParent = -1;
	m_Pages[0].pTemplate = LockDlgRes(IDD_PREFERENCES_GENERAL);

	m_Pages[1].pszName = TEXT("Plugins");
	m_Pages[1].pDialogFunc = (DLGPROC)&PluginsProc;
	m_Pages[1].iParent = -1;
	m_Pages[1].pTemplate = LockDlgRes(IDD_PREFERENCES_PLUGINS);

	m_Pages[2].pszName = TEXT("Audio");
	m_Pages[2].pDialogFunc = (DLGPROC)&AudioProc;
	m_Pages[2].iParent = -1;
	m_Pages[2].pTemplate = LockDlgRes(IDD_PREFERENCES_AUDIO);

	// general
	m_Pages[3].pszName = TEXT("Formatting");
	m_Pages[3].pDialogFunc = (DLGPROC)&FormattingProc;
	m_Pages[3].iParent = 0;
	m_Pages[3].pTemplate = LockDlgRes(IDD_PREFERENCES_FORMATTING);

	m_Pages[4].pszName = TEXT("Media Library");
	m_Pages[4].pDialogFunc = (DLGPROC)&LibraryProc;
	m_Pages[4].iParent = 0;
	m_Pages[4].pTemplate = LockDlgRes(IDD_PREFERENCES_MEDIALIBRARY);

	m_Pages[5].pszName = TEXT("File Assoc");
	m_Pages[5].pDialogFunc = (DLGPROC)&FileAssocProc;
	m_Pages[5].iParent = 0;
	m_Pages[5].pTemplate = LockDlgRes(IDD_PREFERENCES_FILEASSOC);

	// plugins
	m_Pages[6].pszName = TEXT("Audio");
	m_Pages[6].pDialogFunc = (DLGPROC)&CoreAudioProc;
	m_Pages[6].iParent = 1;
	m_Pages[6].pTemplate = LockDlgRes(IDD_PREFERENCES_COREAUDIO);

	//m_Pages[8].pszName = TEXT("Import/Export");
	//m_Pages[8].pDialogFunc = (DLGPROC)&ImportExportProc;
	//m_Pages[8].iParent = 1;
	//m_Pages[8].pTemplate = LockDlgRes(IDD_PREFERENCES_IMPORTEXPORT);

	m_Pages[7].pszName = TEXT("Visuals");
	m_Pages[7].pDialogFunc = (DLGPROC)&VisualsProc;
	m_Pages[7].iParent = 1;
	m_Pages[7].pTemplate = LockDlgRes(IDD_PREFERENCES_VISUALS);

	m_Pages[8].pszName = TEXT("EQ");
	m_Pages[8].pDialogFunc = (DLGPROC)&EQProc;
	m_Pages[8].iParent = 2;
	m_Pages[8].pTemplate = LockDlgRes(IDD_PREFERENCES_EQ);


}

CPreferences::~CPreferences(void)
{
}

bool CPreferences::DefaultPreferences(void)
{
	SetRect(&m_MainWindowRect, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT);

	m_iSourceViewDividerX		= 150;
	m_eTrayIconMode				= TrayIconMinimize;
	//m_Theme[0] = L'\0';

	m_bMainWindowMaximized		= FALSE;
	m_bMainWindowMinimized		= FALSE;
	m_bMinimizeOnClose			= FALSE;
	m_bAlwaysOnTop				= FALSE;
	m_bPauseOnLock				= FALSE;
	m_bPauseOnScreensave		= FALSE;
	m_bShowAlbumArt				= TRUE;
	m_bArtOnSelection			= FALSE;
	m_bFollowCurrentSong		= TRUE;
	m_bSmartSorting				= TRUE;

	m_bSetDateAddedToFileCreationTime = FALSE;
	m_bPlaylistSorting			= FALSE;
	m_bAutoAddPlaylist			= TRUE;
	m_bAutoSoftPause			= FALSE;

	wnsprintf(m_szWindowFormatString, 256, TEXT("@T - @A [Tuniac]"));
	wnsprintf(m_szPluginFormatString, 256, TEXT("@T - @A"));
	wnsprintf(m_szListFormatString, 256, TEXT("@T - @A"));

	m_bCrossfadeEnabled			= TRUE;
	m_iCrossfadeTime			= 6;

	m_iAudioBuffering			= 500;
	m_bReplayGain				= FALSE;
	m_bReplayGainAlbum			= FALSE;

	m_fVolume					= 100.0f;

	m_bEQEnabled				= FALSE;
	m_fEQLow					= 1.0f;
	m_fEQMid					= 1.0f;
	m_fEQHigh					= 1.0f;
	m_fAmpGain					= -6.0f;

	m_iPlaylistViewNumColumns		= 5;
	m_PlaylistViewColumnIDs[0]		= FIELD_TITLE;
	m_PlaylistViewColumnWidths[0]	= 300;
	m_PlaylistViewColumnIDs[1]		= FIELD_ARTIST;
	m_PlaylistViewColumnWidths[1]	= 200;
	m_PlaylistViewColumnIDs[2]		= FIELD_ALBUM;
	m_PlaylistViewColumnWidths[2]	= 200;
	m_PlaylistViewColumnIDs[3]		= FIELD_TRACKNUM;
	m_PlaylistViewColumnWidths[3]	= 70;
	m_PlaylistViewColumnIDs[4]		= FIELD_PLAYBACKTIME;
	m_PlaylistViewColumnWidths[4]	= 70;

	m_eRepeatMode				= RepeatNone;
	m_bShuffleState				= FALSE;

	m_iVisualFPS				= 60;
	m_iCurrentVisual			= 0;
	m_bShowVisArt				= 0;

	m_iFileAssocType			= FILEASSOC_TYPE_OPEN;

	m_iHistoryListSize = 10;
	m_iFutureListSize = 10;


	return true;
}

bool CPreferences::LoadPreferences(void)
{
	HKEY		hTuniacPrefKey;

	DefaultPreferences();

	if(RegOpenKeyEx(HKEY_CURRENT_USER, PREFERENCES_KEY, 0, KEY_QUERY_VALUE, &hTuniacPrefKey) != ERROR_SUCCESS)
		return false;

	DWORD Size = NULL;
	DWORD Type = NULL;

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						MAINWINDOWMAXIMIZED,
						NULL,
						&Type,
						(LPBYTE)&m_bMainWindowMaximized,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						MAINWINDOWMINIMIZED,
						NULL,
						&Type,
						(LPBYTE)&m_bMainWindowMinimized,
						&Size);

	Size = sizeof(int);
	RegQueryValueEx(	hTuniacPrefKey,
						SOURCEVIEWDIVIDERX,
						NULL,
						&Type,
						(LPBYTE)&m_iSourceViewDividerX,
						&Size);

	/*
	Size = 128 * sizeof(WCHAR);
	Type = REG_SZ;
	RegQueryValueEx(	hTuniacPrefKey,
						ACTIVETHEME,
						NULL,
						&Type,
						(LPBYTE)&m_Theme,
						&Size);
	*/

	Size = sizeof(int);
	RegQueryValueEx(	hTuniacPrefKey,
						TRAYICONMODE,
						NULL,
						&Type,
						(LPBYTE)&m_eTrayIconMode,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						MINIMIZEONCLOSE,
						NULL,
						&Type,
						(LPBYTE)&m_bMinimizeOnClose,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						ALWAYSONTOP,
						NULL,
						&Type,
						(LPBYTE)&m_bAlwaysOnTop,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						PAUSEONLOCK,
						NULL,
						&Type,
						(LPBYTE)&m_bPauseOnLock,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						PAUSEONSCREENSAVE,
						NULL,
						&Type,
						(LPBYTE)&m_bPauseOnScreensave,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						SHOWALBUMART,
						NULL,
						&Type,
						(LPBYTE)&m_bShowAlbumArt,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						ARTONSELECTION,
						NULL,
						&Type,
						(LPBYTE)&m_bArtOnSelection,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						FOLLOWCURRENTSONG,
						NULL,
						&Type,
						(LPBYTE)&m_bFollowCurrentSong,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						SMARTSORTING,
						NULL,
						&Type,
						(LPBYTE)&m_bSmartSorting,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						CROSSFADEENABLED,
						NULL,
						&Type,
						(LPBYTE)&m_bCrossfadeEnabled,
						&Size);

	Size = sizeof(int);
	RegQueryValueEx(	hTuniacPrefKey,
						CROSSFADETIME,
						NULL,
						&Type,
						(LPBYTE)&m_iCrossfadeTime,
						&Size);

	Size = sizeof(int);
	RegQueryValueEx(	hTuniacPrefKey,
						AUDIOBUFFERING,
						NULL,
						&Type,
						(LPBYTE)&m_iAudioBuffering,
						&Size);

	Size = sizeof(int);
	RegQueryValueEx(	hTuniacPrefKey,
						FILEASSOCTYPE,
						NULL,
						&Type,
						(LPBYTE)&m_iFileAssocType,
						&Size);	

	Size = sizeof(int);
	RegQueryValueEx(	hTuniacPrefKey,
						HISTORYLISTSIZE,
						NULL,
						&Type,
						(LPBYTE)&m_iHistoryListSize,
						&Size);	

	Size = sizeof(int);
	RegQueryValueEx(	hTuniacPrefKey,
						FUTURELISTSIZE,
						NULL,
						&Type,
						(LPBYTE)&m_iFutureListSize,
						&Size);	

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						DATEADDED,
						NULL,
						&Type,
						(LPBYTE)&m_bSetDateAddedToFileCreationTime,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						PLAYLISTSORTING,
						NULL,
						&Type,
						(LPBYTE)&m_bPlaylistSorting,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						AUTOADDPLAYLIST,
						NULL,
						&Type,
						(LPBYTE)&m_bAutoAddPlaylist,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						AUTOSOFTPAUSE,
						NULL,
						&Type,
						(LPBYTE)&m_bAutoSoftPause,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						SHUFFLEPLAY,
						NULL,
						&Type,
						(LPBYTE)&m_bShuffleState,
						&Size);

	Size = sizeof(int);
	RegQueryValueEx(	hTuniacPrefKey,
						REPEATMODE,
						NULL,
						&Type,
						(LPBYTE)&m_eRepeatMode,
						&Size);

	Size = sizeof(int);
	RegQueryValueEx(	hTuniacPrefKey,
						CURRENTVISUAL,
						NULL,
						&Type,
						(LPBYTE)&m_iCurrentVisual,
						&Size);

	Size = sizeof(int);
	RegQueryValueEx(	hTuniacPrefKey,
						VISUALFPS,
						NULL,
						&Type,
						(LPBYTE)&m_iVisualFPS,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						SHOWVISART,
						NULL,
						&Type,
						(LPBYTE)&m_bShowVisArt,
						&Size);

	Size = sizeof(float);
	RegQueryValueEx(	hTuniacPrefKey,
						VOLUME,
						NULL,
						&Type,
						(LPBYTE)&m_fVolume,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						EQENABLED,
						NULL,
						&Type,
						(LPBYTE)&m_bEQEnabled,
						&Size);

	Size = sizeof(float);
	RegQueryValueEx(	hTuniacPrefKey,
						EQLOW,
						NULL,
						&Type,
						(LPBYTE)&m_fEQLow,
						&Size);

	Size = sizeof(float);
	RegQueryValueEx(	hTuniacPrefKey,
						EQMID,
						NULL,
						&Type,
						(LPBYTE)&m_fEQMid,
						&Size);

	Size = sizeof(float);
	RegQueryValueEx(	hTuniacPrefKey,
						EQHIGH,
						NULL,
						&Type,
						(LPBYTE)&m_fEQHigh,
						&Size);

	Size = sizeof(float);
	RegQueryValueEx(	hTuniacPrefKey,
						AMPGAIN,
						NULL,
						&Type,
						(LPBYTE)&m_fAmpGain,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						AUDIOGAIN,
						NULL,
						&Type,
						(LPBYTE)&m_bReplayGain,
						&Size);

	Size = sizeof(BOOL);
	RegQueryValueEx(	hTuniacPrefKey,
						AUDIOGAINALBUM,
						NULL,
						&Type,
						(LPBYTE)&m_bReplayGainAlbum,
						&Size);

	Size = sizeof(int);
	RegQueryValueEx(	hTuniacPrefKey,
						PLAYLISTVIEWNUMCOLS,
						NULL,
						&Type,
						(LPBYTE)&m_iPlaylistViewNumColumns,
						&Size);	

	Size = sizeof(int) * m_iPlaylistViewNumColumns;
	RegQueryValueEx(	hTuniacPrefKey,
						PLAYLISTVIEWCOLIDS,
						NULL,
						&Type,
						(LPBYTE)m_PlaylistViewColumnIDs,
						&Size);	

	Size = sizeof(int) * m_iPlaylistViewNumColumns;
	RegQueryValueEx(	hTuniacPrefKey,
						PLAYLISTVIEWCOLWIDTHS,
						NULL,
						&Type,
						(LPBYTE)m_PlaylistViewColumnWidths,
						&Size);	

	Size = 256 * sizeof(WCHAR);
	RegQueryValueEx(	hTuniacPrefKey,
						WINDOWFORMATSTRING,
						NULL,
						&Type,
						(LPBYTE)&m_szWindowFormatString,
						&Size);

	Size = 256 * sizeof(WCHAR);
	RegQueryValueEx(	hTuniacPrefKey,
						PLUGINFORMATSTRING,
						NULL,
						&Type,
						(LPBYTE)&m_szPluginFormatString,
						&Size);

	Size = 256 * sizeof(WCHAR);
	RegQueryValueEx(	hTuniacPrefKey,
						LISTFORMATSTRING,
						NULL,
						&Type,
						(LPBYTE)&m_szListFormatString,
						&Size);

	Size = sizeof(RECT);
	RegQueryValueEx(	hTuniacPrefKey,
						MAINWINDOWPOS,
						NULL,
						&Type,
						(LPBYTE)&m_MainWindowRect,
						&Size);

	if(m_MainWindowRect.left < 0)
		m_MainWindowRect.left = CW_USEDEFAULT;

	if(m_MainWindowRect.top < 0)
		m_MainWindowRect.top = CW_USEDEFAULT;

	if(m_MainWindowRect.right < 500)
		m_MainWindowRect.right = CW_USEDEFAULT;

	if(m_MainWindowRect.bottom < 500)
		m_MainWindowRect.bottom = CW_USEDEFAULT;

	RegCloseKey(hTuniacPrefKey);

	return true;
}

bool CPreferences::SavePreferences(void)
{
	HKEY		hTuniacPrefKey;

	if(RegCreateKey(HKEY_CURRENT_USER, PREFERENCES_KEY, &hTuniacPrefKey) != ERROR_SUCCESS)
		return false;

	RegSetValueEx(	hTuniacPrefKey, 
					MAINWINDOWMAXIMIZED, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bMainWindowMaximized, 
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					MAINWINDOWMINIMIZED, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bMainWindowMinimized, 
					sizeof(BOOL));


	/*
	Size = (wcslen(m_Theme) + 1) * sizeof(TCHAR);
	Type = REG_SZ;
	RegSetValueEx(	hTuniacPrefKey,
					ACTIVETHEME,
					NULL,
					Type,
					(LPBYTE)&m_Theme,
					Size);
	*/

	RegSetValueEx(	hTuniacPrefKey, 
					TRAYICONMODE, 
					0,
					REG_DWORD,
					(LPBYTE)&m_eTrayIconMode, 
					sizeof(int));

	RegSetValueEx(	hTuniacPrefKey, 
					MINIMIZEONCLOSE, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bMinimizeOnClose, 
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					ALWAYSONTOP, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bAlwaysOnTop, 
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					PAUSEONLOCK, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bPauseOnLock, 
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					PAUSEONSCREENSAVE, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bPauseOnScreensave, 
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					SHOWALBUMART, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bShowAlbumArt, 
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					ARTONSELECTION, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bArtOnSelection, 
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					FOLLOWCURRENTSONG, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bFollowCurrentSong, 
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					SMARTSORTING, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bSmartSorting, 
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					SOURCEVIEWDIVIDERX, 
					0,
					REG_DWORD,
					(LPBYTE)&m_iSourceViewDividerX, 
					sizeof(int));

	RegSetValueEx(	hTuniacPrefKey, 
					CROSSFADEENABLED, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bCrossfadeEnabled, 
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					CROSSFADETIME, 
					0,
					REG_DWORD,
					(LPBYTE)&m_iCrossfadeTime, 
					sizeof(int));

	RegSetValueEx(	hTuniacPrefKey, 
					AUDIOBUFFERING, 
					0,
					REG_DWORD,
					(LPBYTE)&m_iAudioBuffering, 
					sizeof(int));

	RegSetValueEx(	hTuniacPrefKey, 
					VOLUME, 
					0,
					REG_DWORD,
					(LPBYTE)&m_fVolume, 
					sizeof(float));

	RegSetValueEx(	hTuniacPrefKey, 
					EQENABLED, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bEQEnabled, 
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					EQLOW, 
					0,
					REG_DWORD,
					(LPBYTE)&m_fEQLow, 
					sizeof(float));

	RegSetValueEx(	hTuniacPrefKey, 
					EQMID, 
					0,
					REG_DWORD,
					(LPBYTE)&m_fEQMid, 
					sizeof(float));
	
	RegSetValueEx(	hTuniacPrefKey, 
					EQHIGH, 
					0,
					REG_DWORD,
					(LPBYTE)&m_fEQHigh, 
					sizeof(float));

	RegSetValueEx(	hTuniacPrefKey, 
					AMPGAIN, 
					0,
					REG_DWORD,
					(LPBYTE)&m_fAmpGain, 
					sizeof(float));

	RegSetValueEx(	hTuniacPrefKey, 
					AUDIOGAIN, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bReplayGain, 
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					AUDIOGAINALBUM, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bReplayGainAlbum, 
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					SHUFFLEPLAY, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bShuffleState, 
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					REPEATMODE, 
					0,
					REG_DWORD,
					(LPBYTE)&m_eRepeatMode, 
					sizeof(int));

	RegSetValueEx(	hTuniacPrefKey,
					CURRENTVISUAL,
					NULL,
					REG_DWORD,
					(LPBYTE)&m_iCurrentVisual,
					sizeof(int));

	RegSetValueEx(	hTuniacPrefKey, 
					VISUALFPS, 
					0,
					REG_DWORD,
					(LPBYTE)&m_iVisualFPS, 
					sizeof(int));

	RegSetValueEx(	hTuniacPrefKey, 
					SHOWVISART, 
					0,
					REG_DWORD,
					(LPBYTE)&m_bShowVisArt, 
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					FILEASSOCTYPE, 
					0,
					REG_DWORD,
					(LPBYTE)&m_iFileAssocType, 
					sizeof(int));

	RegSetValueEx(	hTuniacPrefKey, 
					HISTORYLISTSIZE, 
					0,
					REG_DWORD,
					(LPBYTE)&m_iHistoryListSize, 
					sizeof(int));

	RegSetValueEx(	hTuniacPrefKey, 
					FUTURELISTSIZE, 
					0,
					REG_DWORD,
					(LPBYTE)&m_iFutureListSize, 
					sizeof(int));

	RegSetValueEx(	hTuniacPrefKey,
					DATEADDED,
					NULL,
					REG_DWORD,
					(LPBYTE)&m_bSetDateAddedToFileCreationTime,
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey,
					PLAYLISTSORTING,
					NULL,
					REG_DWORD,
					(LPBYTE)&m_bPlaylistSorting,
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey,
					AUTOADDPLAYLIST,
					NULL,
					REG_DWORD,
					(LPBYTE)&m_bAutoAddPlaylist,
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey,
					AUTOSOFTPAUSE,
					NULL,
					REG_DWORD,
					(LPBYTE)&m_bAutoSoftPause,
					sizeof(BOOL));

	RegSetValueEx(	hTuniacPrefKey, 
					PLAYLISTVIEWNUMCOLS, 
					0,
					REG_DWORD,
					(LPBYTE)&m_iPlaylistViewNumColumns, 
					sizeof(int));

	RegSetValueEx(	hTuniacPrefKey, 
					PLAYLISTVIEWCOLIDS, 
					0,
					REG_BINARY,
					(LPBYTE)m_PlaylistViewColumnIDs, 
					sizeof(int) * m_iPlaylistViewNumColumns);

	RegSetValueEx(	hTuniacPrefKey, 
					PLAYLISTVIEWCOLWIDTHS, 
					0,
					REG_BINARY,
					(LPBYTE)m_PlaylistViewColumnWidths, 
					sizeof(int) * m_iPlaylistViewNumColumns);

	RegSetValueEx(	hTuniacPrefKey,
					WINDOWFORMATSTRING,
					NULL,
					REG_SZ,
					(LPBYTE)&m_szWindowFormatString,
					(wcslen(m_szWindowFormatString) + 1) * sizeof(TCHAR));

	RegSetValueEx(	hTuniacPrefKey,
					PLUGINFORMATSTRING,
					NULL,
					REG_SZ,
					(LPBYTE)&m_szPluginFormatString,
					(wcslen(m_szPluginFormatString) + 1) * sizeof(TCHAR));

	RegSetValueEx(	hTuniacPrefKey,
					LISTFORMATSTRING,
					NULL,
					REG_SZ,
					(LPBYTE)&m_szListFormatString,
					(wcslen(m_szListFormatString) + 1) * sizeof(TCHAR));

	RegSetValueEx(	hTuniacPrefKey, 
					MAINWINDOWPOS, 
					0,
					REG_BINARY,
					(LPBYTE)&m_MainWindowRect, 
					sizeof(RECT));

	RegCloseKey(hTuniacPrefKey);

	return true;
}

void CPreferences::CleanPreferences(void)
{
	LONG lResult;
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_CURRENT_USER, PREFERENCES_KEY, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		lResult = SHDeleteKey(HKEY_CURRENT_USER, PREFERENCES_KEY);
	}
   RegCloseKey(hKey);
}

bool CPreferences::ShowPreferences(HWND hParentWnd, unsigned int iStartPage)
{
	if(iStartPage >= (sizeof(m_Pages) / sizeof(PrefPage)))
		iStartPage = 0;
	
	m_StartPage = FindNthTreeLeaf(iStartPage, -1);
	if(m_StartPage < 0)
		m_StartPage = 0;

	DialogBoxParam(tuniacApp.getMainInstance(), MAKEINTRESOURCE(IDD_PREFERENCES), hParentWnd, (DLGPROC)WndProcStub, (DWORD_PTR)this);

	return true;
}

void			CPreferences::BuildTree(HWND hTree, int iPage)
{
	TVITEMEX tvi; 
	TVINSERTSTRUCT tvins; 

	tvi.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE; 
	tvi.state = m_Pages[iPage].iParent == -1 ? TVIS_BOLD | TVIS_EXPANDED : 0;
	tvi.stateMask = TVIS_BOLD | TVIS_EXPANDED;
	tvins.hInsertAfter = TVI_LAST; 
	tvins.hParent = m_Pages[iPage].iParent == -1 ? TVI_ROOT : m_Pages[m_Pages[iPage].iParent].hTreeItem; 


	tvi.pszText = m_Pages[iPage].pszName;
	tvi.lParam = (LPARAM)iPage; 
	tvins.itemex = tvi; 
	m_Pages[iPage].hTreeItem = TreeView_InsertItem(hTree, &tvins); 


	for(int i = 0; i < sizeof(m_Pages) / sizeof(PrefPage); i++)
	{
		if(m_Pages[i].iParent == iPage)
			BuildTree(hTree, i);
	}

}

int CPreferences::FindNthTreeLeaf(int i, int iParent)
{
	static int c = -1;
	if(c == -1)
		c = i;

	for(int a = 0; a < (sizeof(m_Pages) / sizeof(PrefPage)); a++)
	{
		if(m_Pages[a].iParent == iParent)
		{
			c--;
			if(c < 0)
				return a;

			int b = FindNthTreeLeaf(c, a);
			if(b != -1)
				return b;
		}
	}
	return -1;
}

LRESULT CALLBACK CPreferences::WndProcStub(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_INITDIALOG)
		SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);

	CPreferences * pPrefs = (CPreferences *)(LONG_PTR)GetWindowLongPtr(hDlg, GWLP_USERDATA);
	return(pPrefs->WndProc(hDlg, uMsg, wParam, lParam));
}

LRESULT CALLBACK CPreferences::WndProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				HWND hParent = GetParent(hDlg);
				HWND hTree = GetDlgItem(hDlg, IDC_PREFERENCES_TREE);
				RECT rcDlg, rcParent, rcTree;

				GetWindowRect(hDlg, &rcDlg);
				GetWindowRect(hParent, &rcParent);
				GetWindowRect(hTree, &rcTree);
			        
				int iWidth  = rcDlg.right  - rcDlg.left;
				int iHeight = rcDlg.bottom - rcDlg.top;

				int x = ((rcParent.right - rcParent.left) -  iWidth) / 2 + rcParent.left;
				int y = ((rcParent.bottom - rcParent.top) - iHeight) / 2 + rcParent.top;

				int iScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
				int iScreenHeight = GetSystemMetrics(SM_CYSCREEN);
			    
				if(x < 0) x = 0;
				if(y < 0) y = 0;
				if(x + iWidth  > iScreenWidth)  x = iScreenWidth  - iWidth;
				if(y + iHeight > iScreenHeight) y = iScreenHeight - iHeight;

				MoveWindow(hDlg, x, y, iWidth, iHeight, FALSE);


				m_PagePos.top = 0;
				m_PagePos.left = rcTree.right - rcTree.left + 12;

				m_PagePos.right = rcDlg.right - rcDlg.left - m_PagePos.left - 10;
				m_PagePos.bottom = rcDlg.bottom - rcDlg.top - 62;

				int iItemHeight = TreeView_GetItemHeight(hTree);
				TreeView_SetItemHeight(hTree, iItemHeight * 1.2);

				for(int i = 0; i < sizeof(m_Pages) / sizeof(PrefPage); i++)
				{
					if(m_Pages[i].iParent == -1)
						BuildTree(hTree, i);
				}

				if(m_StartPage > 0)
					TreeView_SelectItem(hTree, m_Pages[m_StartPage].hTreeItem);

			}
			break;

		case WM_CLOSE:
			{
				EndDialog(hDlg, 0);
			}
			break;

		case WM_NOTIFY:
			{
				UINT idCtrl = wParam;
				LPNMHDR lpNotify = (LPNMHDR)lParam;

				switch(lpNotify->code)
				{
					case TVN_SELCHANGED:
						{
							LPNMTREEVIEW pTree = (LPNMTREEVIEW)lParam;
							unsigned int iPage = pTree->itemNew.lParam;

							if(iPage >= sizeof(m_Pages) / sizeof(PrefPage))
								break;

							if(m_hPage != NULL)
								DestroyWindow(m_hPage);
							
							m_hPage = CreateDialogIndirectParam(tuniacApp.getMainInstance(), m_Pages[iPage].pTemplate, hDlg, m_Pages[iPage].pDialogFunc, (LPARAM)this);
							SetWindowPos(m_hPage, HWND_TOP, m_PagePos.left, m_PagePos.top, m_PagePos.right, m_PagePos.bottom, SWP_SHOWWINDOW);
							EnableWindow(m_hPage, TRUE);

							// for bug when prefs dialog created and show, tree would get focus but not show it
							SetFocus(pTree->hdr.hwndFrom); 

						}
						break;

				}
			}
			break;

		case WM_COMMAND:
			{
				WORD wCmdID = LOWORD(wParam);

				switch(wCmdID)
				{
					case IDC_OK:
						{
							SavePreferences();
							EndDialog(hDlg, 0);
						}
						break;
				}

			}
			break;

		default:
			return false;
			break;
	}

	return true;

}

unsigned int	CPreferences::GetPreferencesPageCount(void)
{
	return sizeof(m_Pages) / sizeof(PrefPage);
}

bool	CPreferences::GetPreferencesPageName(unsigned int iPage, LPTSTR szDest, unsigned long iSize)
{
	if(iPage >= (sizeof(m_Pages) / sizeof(PrefPage)))
		return false;

	int i = FindNthTreeLeaf(iPage, -1);
	if(i == -1)
		return false;

	if(m_Pages[i].iParent == -1)
	{
		wnsprintf(szDest, iSize, TEXT("%s"), m_Pages[i].pszName);
	}
	else
	{
		int iParent = m_Pages[i].iParent;

		if(m_Pages[iParent].iParent == -1)
		{
			wnsprintf(szDest, iSize, TEXT("%s: %s"), m_Pages[iParent].pszName, m_Pages[i].pszName);
		}
		else
		{
			wnsprintf(szDest, iSize, TEXT("%s: %s: %s"), m_Pages[m_Pages[iParent].iParent].pszName, m_Pages[iParent].pszName, m_Pages[i].pszName);
		}
	}
	return true;
}

bool	CPreferences::PluginGetValue(LPCTSTR szSubKey, LPCTSTR lpValueName, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	HKEY hTuniacPrefKey;
	TCHAR szKey[512];
	wnsprintf(szKey, 512, TEXT("%s\\%s"), PREFERENCES_KEY, szSubKey);

	if(RegOpenKeyEx(	HKEY_CURRENT_USER,
						szKey,
						0,
						KEY_QUERY_VALUE,
						&hTuniacPrefKey) == ERROR_SUCCESS)
	{
		RegQueryValueEx(	hTuniacPrefKey,
							lpValueName,
							NULL,
							lpType,
							lpData,
							lpcbData);

		RegCloseKey(hTuniacPrefKey);
		return true;
	}

	return false;
}

bool	CPreferences::PluginSetValue(LPCTSTR szSubKey, LPCTSTR lpValueName, DWORD dwType, const BYTE* lpData, DWORD cbData)
{
	if(!tuniacApp.getSavePrefs())
		return false;
		
	HKEY hTuniacPrefKey;
	TCHAR szKey[512];
	wnsprintf(szKey, 512, TEXT("%s\\%s"), PREFERENCES_KEY, szSubKey);

	if(RegCreateKey(	HKEY_CURRENT_USER,
						szKey,
						&hTuniacPrefKey) == ERROR_SUCCESS)
	{

		RegSetValueEx(	hTuniacPrefKey, 
						lpValueName, 
						0,
						dwType,
						lpData, 
						cbData);
		RegCloseKey(hTuniacPrefKey);
		return true;
	}
	return false;
}



void	CPreferences::SetSourceViewDividerX(int iPixels)
{
	m_iSourceViewDividerX = iPixels;
}

int		CPreferences::GetSourceViewDividerX(void)
{
	return m_iSourceViewDividerX;
}

void	CPreferences::SetMainWindowRect(RECT * lpRect)
{
	CopyRect(&m_MainWindowRect, lpRect);
}

RECT *	CPreferences::GetMainWindowRect(void)
{
	return &m_MainWindowRect;
}

void	CPreferences::SetMainWindowMaximized(BOOL bMaximized)
{
	m_bMainWindowMaximized = bMaximized;
}

BOOL	CPreferences::GetMainWindowMaximized(void)
{
	return m_bMainWindowMaximized;
}

void	CPreferences::SetMainWindowMinimized(BOOL bMinimized)
{
	m_bMainWindowMinimized = bMinimized;
}

BOOL	CPreferences::GetMainWindowMinimized(void)
{
	return m_bMainWindowMinimized;
}

BOOL	CPreferences::CrossfadingEnabled(void)
{
	return m_bCrossfadeEnabled;
}

int CPreferences::GetCrossfadeTime(void)
{
	return m_iCrossfadeTime;
}

void CPreferences::SetCrossfadeTime(int time)
{
	m_iCrossfadeTime = time;
}

int CPreferences::GetAudioBuffering(void)
{
	return m_iAudioBuffering;
}

BOOL CPreferences::ReplayGainEnabled(void)
{
	return m_bReplayGain;
}

BOOL CPreferences::ReplayGainUseAlbumGain(void)
{
	return m_bReplayGainAlbum;
}

float	CPreferences::GetVolumePercent(void)
{
	return m_fVolume;
}

void	CPreferences::SetVolumePercent(float fPercent)
{
	m_fVolume = fPercent;
}

float		CPreferences::GetEQLowGain(void)
{
	return m_fEQLow;
}

float		CPreferences::GetEQMidGain(void)
{
	return m_fEQMid;
}

float		CPreferences::GetEQHighGain(void)
{
	return m_fEQHigh;
}

void	CPreferences::SetEQGain(float fEQLow, float fEQMid, float fEQHigh)
{
	m_fEQLow = fEQLow;
	m_fEQMid = fEQMid;
	m_fEQHigh = fEQHigh;
}

float		CPreferences::GetAmpGain(void)
{
	return m_fAmpGain;
}

void	CPreferences::SetAmpGain(float fGain)
{
	m_fAmpGain = fGain;
}

int		CPreferences::GetPlaylistViewNumColumns(void)
{
	return m_iPlaylistViewNumColumns;
}

void	CPreferences::SetPlaylistViewNumColumns(int iColumns)
{
	m_iPlaylistViewNumColumns = iColumns;
}

int		CPreferences::GetPlaylistViewColumnIDAtIndex(int index)
{
	return m_PlaylistViewColumnIDs[index];
}

void	CPreferences::SetPlaylistViewColumnIDAtIndex(int index, int ID)
{
	m_PlaylistViewColumnIDs[index] = ID;
}

int		CPreferences::GetPlaylistViewColumnWidthAtIndex(int index)
{
	return m_PlaylistViewColumnWidths[index];
}

void	CPreferences::SetPlaylistViewColumnWidthAtIndex(int index, int Width)
{
	m_PlaylistViewColumnWidths[index] = Width;
}
/*
LPTSTR	CPreferences::GetTheme(void)
{
	if(m_Theme[0] == L'\0')
		return NULL;
	return m_Theme;
}
*/
BOOL CPreferences::GetShuffleState(void)
{
	return m_bShuffleState;
}

void CPreferences::SetShuffleState(BOOL bEnabled)
{
	m_bShuffleState = bEnabled;
	tuniacApp.m_SourceSelectorWindow->UpdateView();
	if(bEnabled)
	{
		IPlaylist * pPlaylist = tuniacApp.m_PlaylistManager.GetActivePlaylist();
		IPlaylistEX * pPlaylistEX = (IPlaylistEX *)pPlaylist;
		pPlaylistEX->RebuildPlaylistArrays();
	}
}

RepeatMode	CPreferences::GetRepeatMode(void)
{
	return m_eRepeatMode;
}

void		CPreferences::SetRepeatMode(RepeatMode eMode)
{
	m_eRepeatMode = eMode;
	tuniacApp.m_SourceSelectorWindow->UpdateView();
}

int			CPreferences::GetVisualFPS(void)
{
	return m_iVisualFPS;
}

TrayIconMode	CPreferences::GetTrayIconMode(void)
{
	return m_eTrayIconMode;
}

void		CPreferences::SetTrayIconMode(TrayIconMode eMode)
{
	m_eTrayIconMode = eMode;
}

BOOL		CPreferences::GetMinimizeOnClose(void)
{
	return m_bMinimizeOnClose;
}

BOOL		CPreferences::GetFollowCurrentSongMode(void)
{
	return m_bFollowCurrentSong;
}

void		CPreferences::SetFollowCurrentSongMode(BOOL bEnabled)
{
	m_bFollowCurrentSong = bEnabled;
}

BOOL		CPreferences::GetSmartSortingEnabled(void)
{
	return m_bSmartSorting;
}

BOOL		CPreferences::GetPauseOnLock(void)
{
	return m_bPauseOnLock;
}

BOOL		CPreferences::GetPauseOnScreensave(void)
{
	return m_bPauseOnScreensave;
}

BOOL		CPreferences::GetShowAlbumArt(void)
{
	return m_bShowAlbumArt;
}

BOOL		CPreferences::GetArtOnSelection(void)
{
	return m_bArtOnSelection;
}

LPTSTR		CPreferences::GetWindowFormatString(void)
{
	return m_szWindowFormatString;
}

LPTSTR		CPreferences::GetPluginFormatString(void)
{
	return m_szPluginFormatString;
}

LPTSTR		CPreferences::GetListFormatString(void)
{
	return m_szListFormatString;
}

BOOL		CPreferences::GetAlwaysOnTop(void)
{
	return m_bAlwaysOnTop;
}

void		CPreferences::SetAlwaysOnTop(BOOL bEnabled)
{
	m_bAlwaysOnTop = bEnabled;
}

int		CPreferences::GetCurrentVisual(void)
{
	return m_iCurrentVisual;
}

void		CPreferences::SetCurrentVisual(int iVisual)
{
	m_iCurrentVisual = iVisual;
}

BOOL		CPreferences::GetShowVisArt(void)
{
	if(!GetShowAlbumArt())
		return false;

	return m_bShowVisArt;
}

int			CPreferences::GetHistoryListSize(void)
{
	return m_iHistoryListSize;
}

int			CPreferences::GetFutureListSize(void)
{
	return m_iFutureListSize;
}

BOOL		CPreferences::GetDateAddedToFileCreationTime(void)
{
	return m_bSetDateAddedToFileCreationTime;
}

BOOL		CPreferences::GetCanPlaylistsSort(void)
{
	return m_bPlaylistSorting;
}

BOOL		CPreferences::GetAutoAddPlaylists(void)
{
	return m_bAutoAddPlaylist;
}

BOOL		CPreferences::GetAutoSoftPause(void)
{
	return m_bAutoSoftPause;
}