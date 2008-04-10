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
#include ".\preferences.h"

#define PREFERENCES_KEY			TEXT("Software\\Tuniac")

#define MAINWINDOWPOS			TEXT("MainWindowPos")
#define MAINWINDOWMAXIMIZED		TEXT("MainWindowMaximized")
#define SOURCEVIEWDIVIDERX		TEXT("SourceViewDividerX")
#define ACTIVETHEME				TEXT("ActiveTheme")

#define TRAYICONMODE			TEXT("TrayIconMode")
#define MINIMIZEONCLOSE			TEXT("MinimizeOnClose")
#define ALWAYSONTOP				TEXT("AlwaysOnTop")
#define PAUSEONLOCK				TEXT("PauseOnLock")
#define PAUSEONSCREENSAVE		TEXT("PauseOnScreensave")
#define FOLLOWCURRENTSONG		TEXT("FollowCurrentSong")
#define SMARTSORTING			TEXT("SmartSorting")

#define WINDOWFORMATSTRING		TEXT("WindowFormatString")
#define PLUGINFORMATSTRING		TEXT("PluginFormatString")
#define LISTFORMATSTRING		TEXT("ListFormatString")

#define CROSSFADETIME			TEXT("CrossFadeTime")
#define CROSSFADEENABLED		TEXT("CrossFadeEnabled")

#define AUDIOBUFFERING			TEXT("AudioBuffering")

#define VOLUME 					TEXT("Volume")

#define SHUFFLEPLAY				TEXT("ShufflePlay")
#define REPEATMODE				TEXT("RepeatMode")

#define VISUALFPS				TEXT("VisualFPS")

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

				SendDlgItemMessage(hDlg, IDC_GENERAL_FOLLOWCURRENTSONG, BM_SETCHECK, pPrefs->m_FollowCurrentSong ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_GENERAL_SMARTSORTING, BM_SETCHECK, pPrefs->m_SmartSorting ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_GENERAL_PAUSEONLOCK, BM_SETCHECK, pPrefs->m_PauseOnLock ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_GENERAL_PAUSEONSCREENSAVE, BM_SETCHECK, pPrefs->m_PauseOnScreensave ? BST_CHECKED : BST_UNCHECKED, 0);
				
				SendDlgItemMessage(hDlg, IDC_GENERAL_TASKBAR_NORMAL, BM_SETCHECK, pPrefs->m_eTrayIconMode == TrayIconNever ? BST_CHECKED : BST_UNCHECKED, 0);
				SendDlgItemMessage(hDlg, IDC_GENERAL_TASKBAR_MINIMIZE, BM_SETCHECK, pPrefs->m_eTrayIconMode == TrayIconMinimize ? BST_CHECKED : BST_UNCHECKED, 0);

				SendDlgItemMessage(hDlg, IDC_GENERAL_MINIMIZEONCLOSE, BM_SETCHECK, pPrefs->m_MinimizeOnClose ? BST_CHECKED : BST_UNCHECKED, 0);


				TCHAR szSize[8];
				wnsprintf(szSize, 8, TEXT("%i"), pPrefs->m_HistoryListSize);
				SendDlgItemMessage(hDlg, IDC_GENERAL_HISTORYCOUNT, WM_SETTEXT, 0, (LPARAM)szSize);
				SendDlgItemMessage(hDlg, IDC_GENERAL_HISTORYCOUNT_SPINNER, UDM_SETRANGE, 0, (LPARAM)MAKELONG(HISTORY_MAX, 2));
				SendDlgItemMessage(hDlg, IDC_GENERAL_HISTORYCOUNT_SPINNER, UDM_SETBUDDY, (WPARAM)GetDlgItem(hDlg, IDC_GENERAL_HISTORYCOUNT), 0);
 
				wnsprintf(szSize, 8, TEXT("%i"), pPrefs->m_FutureListSize);
				SendDlgItemMessage(hDlg, IDC_GENERAL_FUTURECOUNT, WM_SETTEXT, 0, (LPARAM)szSize);
				SendDlgItemMessage(hDlg, IDC_GENERAL_FUTURECOUNT_SPINNER, UDM_SETRANGE, 0, (LPARAM)MAKELONG(FUTURE_MAX, 2));
				SendDlgItemMessage(hDlg, IDC_GENERAL_FUTURECOUNT_SPINNER, UDM_SETBUDDY, (WPARAM)GetDlgItem(hDlg, IDC_GENERAL_FUTURECOUNT), 0);


			}
			break;

		case WM_COMMAND:
			{
				switch (LOWORD(wParam)) 
				{
					case IDC_GENERAL_FOLLOWCURRENTSONG:
						{
							int State = SendDlgItemMessage(hDlg, IDC_GENERAL_FOLLOWCURRENTSONG, BM_GETCHECK, 0, 0);
							pPrefs->m_FollowCurrentSong = State == BST_UNCHECKED ? FALSE : TRUE;
						}
						break;

					case IDC_GENERAL_SMARTSORTING:
						{
							int State = SendDlgItemMessage(hDlg, IDC_GENERAL_SMARTSORTING, BM_GETCHECK, 0, 0);
							pPrefs->m_SmartSorting = State == BST_UNCHECKED ? FALSE : TRUE;
						}
						break;

					case IDC_GENERAL_PAUSEONLOCK:
						{
							int State = SendDlgItemMessage(hDlg, IDC_GENERAL_PAUSEONLOCK, BM_GETCHECK, 0, 0);
							pPrefs->m_PauseOnLock = State == BST_UNCHECKED ? FALSE : TRUE;
						}
						break;

						case IDC_GENERAL_PAUSEONSCREENSAVE:
						{
							int State = SendDlgItemMessage(hDlg, IDC_GENERAL_PAUSEONSCREENSAVE, BM_GETCHECK, 0, 0);
							pPrefs->m_PauseOnScreensave = State == BST_UNCHECKED ? FALSE : TRUE;
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
							pPrefs->m_MinimizeOnClose = State == BST_UNCHECKED ? FALSE : TRUE;
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
							pPrefs->m_HistoryListSize = iSize;
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
							pPrefs->m_FutureListSize = iSize;
						}
						break;

				}
			}
			break;

		default:
			return FALSE;
			break;
	}

	return TRUE;
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
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, WM_SETTEXT, 0, (LPARAM)pPrefs->m_WindowFormatString);
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@T - @A [Tuniac]"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@!: @T - @A [Tuniac]"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("(@I) @T - @A [Tuniac]"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@T - @L - @A [Tuniac]"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@U [Tuniac]"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@F - @I [Tuniac]"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("Tuniac - @!: @F (@I)"));

				SendDlgItemMessage(hDlg, IDC_FORMATTING_PLUGINFORMAT, CB_RESETCONTENT, 0, 0);
				SendDlgItemMessage(hDlg, IDC_FORMATTING_PLUGINFORMAT, WM_SETTEXT, 0, (LPARAM)pPrefs->m_PluginFormatString);
				SendDlgItemMessage(hDlg, IDC_FORMATTING_PLUGINFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@T - @A"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_PLUGINFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("@F"));
				SendDlgItemMessage(hDlg, IDC_FORMATTING_PLUGINFORMAT, CB_ADDSTRING, 0, (LPARAM)TEXT("[@#][@T - @A][@L - @Y][@I|@B|@S]"));

				SendDlgItemMessage(hDlg, IDC_FORMATTING_LISTFORMAT, CB_RESETCONTENT, 0, 0);
				SendDlgItemMessage(hDlg, IDC_FORMATTING_LISTFORMAT, WM_SETTEXT, 0, (LPARAM)pPrefs->m_ListFormatString);
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
							SendDlgItemMessage(hDlg, IDC_FORMATTING_WINDOWTITLEFORMAT, WM_GETTEXT, 256, (LPARAM)pPrefs->m_WindowFormatString);
						}
						break;
					case IDC_FORMATTING_PLUGINFORMAT:
						{
							SendDlgItemMessage(hDlg, IDC_FORMATTING_PLUGINFORMAT, WM_GETTEXT, 256, (LPARAM)pPrefs->m_PluginFormatString);
						}
						break;

					case IDC_FORMATTING_LISTFORMAT:
						{
							SendDlgItemMessage(hDlg, IDC_FORMATTING_LISTFORMAT, WM_GETTEXT, 256, (LPARAM)pPrefs->m_ListFormatString);
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
			return FALSE;
			break;
	}

	return TRUE;

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
								if(IDOK == MessageBox(hDlg, TEXT("Plugin must be loaded to do this.\n\nEnable plugin now?"), TEXT(""), MB_YESNO | MB_ICONINFORMATION))
								{
									tuniacApp.m_PluginManager.EnablePlugin(iSel, true);
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
									tuniacApp.m_PluginManager.EnablePlugin(iSel, true);
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
			return FALSE;
			break;
	}

	return TRUE;
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

				RECT r;
				POINT pt;
				GetWindowRect(GetDlgItem(GetParent(hDlg), IDCANCEL), &r);
				pt.x = r.left;
				pt.y = r.top;
				ScreenToClient(GetParent(hDlg), &pt);

				ShowWindow(GetDlgItem(GetParent(hDlg), IDCANCEL), SW_HIDE);
				SetWindowPos(GetDlgItem(GetParent(hDlg), IDOK), NULL, pt.x, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

				TCHAR		tstr[256];
				wsprintf(tstr, TEXT("Crossfade for %d seconds"), pPrefs->m_CrossfadeTime);
				SetDlgItemText(hDlg, IDC_CROSSFADE_TIME_TEXT, tstr);
				SendDlgItemMessage(hDlg, IDC_CROSSFADE_TIME_SLIDER, TBM_SETRANGE,	TRUE, MAKELONG(2, 15));
				SendDlgItemMessage(hDlg, IDC_CROSSFADE_TIME_SLIDER, TBM_SETPOS,		TRUE, pPrefs->m_CrossfadeTime);

				if(pPrefs->m_CrossfadeEnabled == 0)
				{
					EnableWindow(GetDlgItem(hDlg, IDC_CROSSFADE_TIME_TEXT), FALSE);
					EnableWindow(GetDlgItem(hDlg, IDC_CROSSFADE_TIME_SLIDER), FALSE);
					SendDlgItemMessage(hDlg, IDC_CROSSFADE_ENABLE, BM_SETCHECK, BST_UNCHECKED, 0);
				}
				else
				{
					EnableWindow(GetDlgItem(hDlg, IDC_CROSSFADE_TIME_TEXT), TRUE);
					EnableWindow(GetDlgItem(hDlg, IDC_CROSSFADE_TIME_SLIDER), TRUE);
					SendDlgItemMessage(hDlg, IDC_CROSSFADE_ENABLE, BM_SETCHECK, BST_CHECKED, 0);
				}

				wsprintf(tstr, TEXT("Buffer size %d milliseconds"), pPrefs->m_AudioBuffering);
				SetDlgItemText(hDlg, IDC_BUFFER_TIME_TEXT, tstr);
				SendDlgItemMessage(hDlg, IDC_BUFFER_TIME_SLIDER, TBM_SETRANGE,	TRUE, MAKELONG(250, 5000));
				SendDlgItemMessage(hDlg, IDC_BUFFER_TIME_SLIDER, TBM_SETPOS,	TRUE, pPrefs->m_AudioBuffering);
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
			
							if(State == BST_UNCHECKED)
							{
								EnableWindow(GetDlgItem(hDlg, IDC_CROSSFADE_TIME_TEXT), FALSE);
								EnableWindow(GetDlgItem(hDlg, IDC_CROSSFADE_TIME_SLIDER), FALSE);
								pPrefs->m_CrossfadeEnabled = 0;
							}
							else
							{
								EnableWindow(GetDlgItem(hDlg, IDC_CROSSFADE_TIME_TEXT), TRUE);
								EnableWindow(GetDlgItem(hDlg, IDC_CROSSFADE_TIME_SLIDER), TRUE);
								pPrefs->m_CrossfadeEnabled = 1;
							}
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

							pPrefs->m_CrossfadeTime = SendDlgItemMessage(hDlg, IDC_CROSSFADE_TIME_SLIDER, TBM_GETPOS, 0, 0); 
							wsprintf(tstr, TEXT("Crossfade for %d seconds"), pPrefs->m_CrossfadeTime);
							SetDlgItemText(hDlg, IDC_CROSSFADE_TIME_TEXT, tstr);

							pPrefs->m_AudioBuffering = SendDlgItemMessage(hDlg, IDC_BUFFER_TIME_SLIDER, TBM_GETPOS, 0, 0); 
							wsprintf(tstr, TEXT("Buffer length %d milliseconds"), pPrefs->m_AudioBuffering);
							SetDlgItemText(hDlg, IDC_BUFFER_TIME_TEXT, tstr);
						}
						break;
				}
			}
			break;

		default:
			return FALSE;
			break;
	}

	return TRUE;
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
			return FALSE;
			break;
	}

	return TRUE;
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
			}
			break;

		case WM_COMMAND:
			{
				WORD wCmdID = LOWORD(wParam);

				switch(wCmdID)
				{

					case IDC_REBUILD_ML:
						{
							//do update
							for(unsigned long ulMLIndex = 0; ulMLIndex < tuniacApp.m_MediaLibrary.GetCount(); ulMLIndex++)
							{
								if(!tuniacApp.m_MediaLibrary.UpdateMLIndex(ulMLIndex))
								{
									IPlaylistEntry * pEntry = tuniacApp.m_MediaLibrary.GetItemByIndex(ulMLIndex);
									unsigned long ulEntryID = pEntry->GetEntryID();
									for(unsigned long list = 0; list < tuniacApp.m_PlaylistManager.GetNumPlaylists(); list++)
									{
										IPlaylist * pPlaylist = tuniacApp.m_PlaylistManager.GetPlaylistAtIndex(list);
										IPlaylistEX * pPlaylistEX = (IPlaylistEX *)pPlaylist;
										pPlaylistEX->DeleteAllItemsWhereIDEquals(ulEntryID);
									}
									tuniacApp.m_MediaLibrary.RemoveItem(pEntry);
									tuniacApp.m_PlaylistManager.m_LibraryPlaylist.RebuildPlaylist();
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
			return FALSE;
			break;
	}

	return TRUE;
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

				SendDlgItemMessage(hDlg, IDC_VISUALPREFS_FPS, TBM_SETRANGE, TRUE, MAKELONG(25, 100));
				SendDlgItemMessage(hDlg, IDC_VISUALPREFS_FPS, TBM_SETPOS,	TRUE, pPrefs->m_VisualFPS);

				TCHAR	tstr[32];
				wsprintf(tstr, TEXT("%d FPS"), pPrefs->m_VisualFPS);
				SetDlgItemText(hDlg, IDC_VISUALPREFS_FPSDISPLAY, tstr);
			}
			break;

		case WM_HSCROLL:
			{
				switch (LOWORD(wParam)) 
				{
					case TB_THUMBTRACK:
					case TB_ENDTRACK:
						{
							pPrefs->m_VisualFPS = SendDlgItemMessage(hDlg, IDC_VISUALPREFS_FPS, TBM_GETPOS, 0, 0);

							TCHAR	tstr[32];
							wsprintf(tstr, TEXT("%d FPS"), pPrefs->m_VisualFPS);
							SetDlgItemText(hDlg, IDC_VISUALPREFS_FPSDISPLAY, tstr);
						}
						break;
				}
			}
			break;

		default:
			return FALSE;
			break;
	}

	return TRUE;
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

				SendMessage(hDlg, WM_COMMAND, IDC_FILEASSOC_RESET, 0);

			}
			break;

		case WM_COMMAND:
			{
				WORD wCmdID = LOWORD(wParam);

				switch(wCmdID)
				{

					case IDC_FILEASSOC_ASSOCNOW:
						{
							bool bSel;
							for (int i = 1; i <= pPrefs->m_FileAssoc.GetExtensionCount(); i++)
							{
								bSel = SendDlgItemMessage(hDlg, IDC_FILEASSOC_LIST, LB_GETSEL, i, 0) > 0;
								PFileAssocDetail pFAD = pPrefs->m_FileAssoc.GetExtensionDetail(i - 1);
								pFAD->bAssociated = bSel;
							}
							bSel = SendDlgItemMessage(hDlg, IDC_FILEASSOC_LIST, LB_GETSEL, 0, 0) > 0;
							pPrefs->m_FileAssoc.ReAssociate(pPrefs->m_FileAssocType, bSel);

							int iSel = SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPEDEFAULT, CB_GETCURSEL, 0, 0);
							int iType = iSel == 1 ? FILEASSOC_TYPE_PLAY : (iSel == 2 ? FILEASSOC_TYPE_QUEUE : FILEASSOC_TYPE_OPEN);
							pPrefs->m_FileAssoc.SetDefaultType(iType);

							MessageBox(hDlg, TEXT("File associations have been applied."), TEXT("Tuniac"), MB_OK | MB_ICONINFORMATION);
						}
						break;

					case IDC_FILEASSOC_RESET:
						{
							SendDlgItemMessage(hDlg, IDC_FILEASSOC_LIST, LB_RESETCONTENT, 0, 0);

							// folder association
							SendDlgItemMessage(hDlg, IDC_FILEASSOC_LIST, LB_INSERTSTRING, -1, (LPARAM)TEXT("*** Add folders to Tuniac"));
							SendDlgItemMessage(hDlg, IDC_FILEASSOC_LIST, LB_SETSEL, pPrefs->m_FileAssoc.IsFoldersAssociated() ? TRUE : FALSE, 0);
							
							// file association
							for(int x = 0; x < pPrefs->m_FileAssoc.GetExtensionCount(); x++)
							{
								PFileAssocDetail pFAD = pPrefs->m_FileAssoc.GetExtensionDetail(x);
								TCHAR szDetail[128];
								wnsprintf(szDetail, 128, TEXT("%s - %s"), pFAD->szExt, pFAD->szDesc);
								SendDlgItemMessage(hDlg, IDC_FILEASSOC_LIST, LB_INSERTSTRING, -1, (LPARAM)szDetail);
								SendDlgItemMessage(hDlg, IDC_FILEASSOC_LIST, LB_SETSEL, pFAD->bAssociated ? TRUE : FALSE, x + 1);
							}

							SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_OPEN, BM_SETCHECK, pPrefs->m_FileAssocType & FILEASSOC_TYPE_OPEN ? BST_CHECKED : BST_UNCHECKED, 0);
							SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_PLAY, BM_SETCHECK, pPrefs->m_FileAssocType & FILEASSOC_TYPE_PLAY ? BST_CHECKED : BST_UNCHECKED, 0);
							SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_QUEUE, BM_SETCHECK, pPrefs->m_FileAssocType & FILEASSOC_TYPE_QUEUE ? BST_CHECKED : BST_UNCHECKED, 0);

							SendMessage(hDlg, WM_COMMAND, MAKELONG(IDC_FILEASSOC_TYPEDEFAULT_UPDATE, 0), 0);

						}
						break;

					case IDC_FILEASSOC_SELECTALL:
						{
							SendDlgItemMessage(hDlg, IDC_FILEASSOC_LIST, LB_SETSEL, TRUE, -1);
						}
						break;

					case IDC_FILEASSOC_DESELECTALL:
						{
							SendDlgItemMessage(hDlg, IDC_FILEASSOC_LIST, LB_SETSEL, FALSE, -1);
						}
						break;

					case IDC_FILEASSOC_TYPE_OPEN:
						{
							int State = SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPE_OPEN, BM_GETCHECK, 0, 0);
							if(State == BST_CHECKED)
							{
								pPrefs->m_FileAssocType |= FILEASSOC_TYPE_OPEN;
							}
							else
							{
								pPrefs->m_FileAssocType &= ~FILEASSOC_TYPE_OPEN;
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
								pPrefs->m_FileAssocType |= FILEASSOC_TYPE_PLAY;
							}
							else
							{
								pPrefs->m_FileAssocType &= ~FILEASSOC_TYPE_PLAY;
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
								pPrefs->m_FileAssocType |= FILEASSOC_TYPE_QUEUE;
							}
							else
							{
								pPrefs->m_FileAssocType &= ~FILEASSOC_TYPE_QUEUE;
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

							if(pPrefs->m_FileAssocType & FILEASSOC_TYPE_OPEN)
							{
								SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPEDEFAULT, CB_ADDSTRING, 0, (LPARAM)TEXT("Open"));
								iCount++;
							}
							if(pPrefs->m_FileAssocType & FILEASSOC_TYPE_PLAY)
							{
								SendDlgItemMessage(hDlg, IDC_FILEASSOC_TYPEDEFAULT, CB_ADDSTRING, 0, (LPARAM)TEXT("Play"));
								if(iType == FILEASSOC_TYPE_PLAY)
									iSel = iCount;
								iCount++;
							}
							if(pPrefs->m_FileAssocType & FILEASSOC_TYPE_QUEUE)
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
			return FALSE;
			break;
	}

	return TRUE;
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

}

CPreferences::~CPreferences(void)
{
}

bool CPreferences::DefaultPreferences(void)
{
	SetRect(&m_MainWindowRect, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT);
	m_MainWindowMaximized = false;

	m_eTrayIconMode				= TrayIconMinimize;
	m_MinimizeOnClose			= FALSE;
	m_AlwaysOnTop				= FALSE;
	m_PauseOnLock				= FALSE;
	m_PauseOnScreensave			= FALSE;
	m_FollowCurrentSong			= TRUE;
	m_SmartSorting				= TRUE;
	wnsprintf(m_WindowFormatString, 256, TEXT("@T - @A [Tuniac]"));
	wnsprintf(m_PluginFormatString, 256, TEXT("@T - @A"));
	wnsprintf(m_ListFormatString, 256, TEXT("@T - @A"));

	m_SourceViewDividerX		= 150;

	m_CrossfadeEnabled			= TRUE;
	m_CrossfadeTime				= 6;

	m_AudioBuffering			= 500;

	m_Volume					= 100.0;

	m_ShuffleState				= FALSE;

	m_PlaylistViewNumColumns		= 4;
	m_PlaylistViewColumnIDs[0]		= FIELD_TITLE;
	m_PlaylistViewColumnWidths[0]	= 300;
	m_PlaylistViewColumnIDs[1]		= FIELD_ARTIST;
	m_PlaylistViewColumnWidths[1]	= 200;
	m_PlaylistViewColumnIDs[2]		= FIELD_ALBUM;
	m_PlaylistViewColumnWidths[2]	= 200;
	m_PlaylistViewColumnIDs[3]		= FIELD_PLAYBACKTIME;
	m_PlaylistViewColumnWidths[3]	= 70;

	m_Theme[0] = L'\0';

	m_RepeatMode					= RepeatNone;

	m_VisualFPS						= 35;
	
	m_FileAssocType					= FILEASSOC_TYPE_OPEN;

	m_HistoryListSize = 10;
	m_FutureListSize = 10;

	return true;
}

bool CPreferences::LoadPreferences(void)
{
	HKEY		hTuniacPrefKey;

	DefaultPreferences();

	if(RegOpenKeyEx(	HKEY_CURRENT_USER,
						PREFERENCES_KEY,
						0,
						KEY_QUERY_VALUE,
						&hTuniacPrefKey) == ERROR_SUCCESS)
	{
		unsigned long Size;
		unsigned long Type;

		Size = sizeof(RECT);
		Type = REG_BINARY;
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

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							MAINWINDOWMAXIMIZED,
							NULL,
							&Type,
							(LPBYTE)&m_MainWindowMaximized,
							&Size);


		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							SOURCEVIEWDIVIDERX,
							NULL,
							&Type,
							(LPBYTE)&m_SourceViewDividerX,
							&Size);

		Size = 128 * sizeof(WCHAR);
		Type = REG_SZ;
		RegQueryValueEx(	hTuniacPrefKey,
							ACTIVETHEME,
							NULL,
							&Type,
							(LPBYTE)&m_Theme,
							&Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							TRAYICONMODE,
							NULL,
							&Type,
							(LPBYTE)&m_eTrayIconMode,
							&Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							MINIMIZEONCLOSE,
							NULL,
							&Type,
							(LPBYTE)&m_MinimizeOnClose,
							&Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							ALWAYSONTOP,
							NULL,
							&Type,
							(LPBYTE)&m_AlwaysOnTop,
							&Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							PAUSEONLOCK,
							NULL,
							&Type,
							(LPBYTE)&m_PauseOnLock,
							&Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							PAUSEONSCREENSAVE,
							NULL,
							&Type,
							(LPBYTE)&m_PauseOnScreensave,
							&Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							FOLLOWCURRENTSONG,
							NULL,
							&Type,
							(LPBYTE)&m_FollowCurrentSong,
							&Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							SMARTSORTING,
							NULL,
							&Type,
							(LPBYTE)&m_SmartSorting,
							&Size);

		Size = 256 * sizeof(WCHAR);
		Type = REG_SZ;
		RegQueryValueEx(	hTuniacPrefKey,
							WINDOWFORMATSTRING,
							NULL,
							&Type,
							(LPBYTE)&m_WindowFormatString,
							&Size);

		Size = 256 * sizeof(WCHAR);
		Type = REG_SZ;
		RegQueryValueEx(	hTuniacPrefKey,
							PLUGINFORMATSTRING,
							NULL,
							&Type,
							(LPBYTE)&m_PluginFormatString,
							&Size);

		Size = 256 * sizeof(WCHAR);
		Type = REG_SZ;
		RegQueryValueEx(	hTuniacPrefKey,
							LISTFORMATSTRING,
							NULL,
							&Type,
							(LPBYTE)&m_ListFormatString,
							&Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							CROSSFADETIME,
							NULL,
							&Type,
							(LPBYTE)&m_CrossfadeTime,
							&Size);

		if(m_CrossfadeTime > 15)
			m_CrossfadeTime = 15;
		if(m_CrossfadeTime < 0)
			m_CrossfadeTime = 0;

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							AUDIOBUFFERING,
							NULL,
							&Type,
							(LPBYTE)&m_AudioBuffering,
							&Size);

		if(m_AudioBuffering > 5000)
			m_AudioBuffering = 5000;
		if(m_AudioBuffering < 250)
			m_AudioBuffering = 250;

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							CROSSFADEENABLED,
							NULL,
							&Type,
							(LPBYTE)&m_CrossfadeEnabled,
							&Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							VOLUME,
							NULL,
							&Type,
							(LPBYTE)&m_Volume,
							&Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							SHUFFLEPLAY,
							NULL,
							&Type,
							(LPBYTE)&m_ShuffleState,
							&Size);


		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							REPEATMODE,
							NULL,
							&Type,
							(LPBYTE)&m_RepeatMode,
							&Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							VISUALFPS,
							NULL,
							&Type,
							(LPBYTE)&m_VisualFPS,
							&Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							PLAYLISTVIEWNUMCOLS,
							NULL,
							&Type,
							(LPBYTE)&m_PlaylistViewNumColumns,
							&Size);	

		Size = sizeof(int) * m_PlaylistViewNumColumns;
		Type = REG_BINARY;
		RegQueryValueEx(	hTuniacPrefKey,
							PLAYLISTVIEWCOLIDS,
							NULL,
							&Type,
							(LPBYTE)m_PlaylistViewColumnIDs,
							&Size);	

		Size = sizeof(int) * m_PlaylistViewNumColumns;
		Type = REG_BINARY;
		RegQueryValueEx(	hTuniacPrefKey,
							PLAYLISTVIEWCOLWIDTHS,
							NULL,
							&Type,
							(LPBYTE)m_PlaylistViewColumnWidths,
							&Size);	

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							FILEASSOCTYPE,
							NULL,
							&Type,
							(LPBYTE)&m_FileAssocType,
							&Size);	

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							HISTORYLISTSIZE,
							NULL,
							&Type,
							(LPBYTE)&m_HistoryListSize,
							&Size);	

		Size = sizeof(int);
		Type = REG_DWORD;
		RegQueryValueEx(	hTuniacPrefKey,
							FUTURELISTSIZE,
							NULL,
							&Type,
							(LPBYTE)&m_FutureListSize,
							&Size);	

		RegCloseKey(hTuniacPrefKey);

		return true;
	}

	return false;
}

bool CPreferences::SavePreferences(void)
{
	HKEY		hTuniacPrefKey;

	if(RegCreateKey(	HKEY_CURRENT_USER,
						PREFERENCES_KEY,
						&hTuniacPrefKey) == ERROR_SUCCESS)
	{
		unsigned long	Size;
		unsigned long	Type;

		Size = sizeof(RECT);
		Type = REG_BINARY;
		RegSetValueEx(	hTuniacPrefKey, 
						MAINWINDOWPOS, 
						0,
						Type,
						(LPBYTE)&m_MainWindowRect, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						MAINWINDOWMAXIMIZED, 
						0,
						Type,
						(LPBYTE)&m_MainWindowMaximized, 
						Size);

		Size = (wcslen(m_Theme) + 1) * sizeof(TCHAR);
		Type = REG_SZ;
		RegSetValueEx(	hTuniacPrefKey,
						ACTIVETHEME,
						NULL,
						Type,
						(LPBYTE)&m_Theme,
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						TRAYICONMODE, 
						0,
						Type,
						(LPBYTE)&m_eTrayIconMode, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						MINIMIZEONCLOSE, 
						0,
						Type,
						(LPBYTE)&m_MinimizeOnClose, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						ALWAYSONTOP, 
						0,
						Type,
						(LPBYTE)&m_AlwaysOnTop, 
						Size);
		
		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						PAUSEONLOCK, 
						0,
						Type,
						(LPBYTE)&m_PauseOnLock, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						PAUSEONSCREENSAVE, 
						0,
						Type,
						(LPBYTE)&m_PauseOnScreensave, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						FOLLOWCURRENTSONG, 
						0,
						Type,
						(LPBYTE)&m_FollowCurrentSong, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						SMARTSORTING, 
						0,
						Type,
						(LPBYTE)&m_SmartSorting, 
						Size);

		Size = (wcslen(m_WindowFormatString) + 1) * sizeof(TCHAR);
		Type = REG_SZ;
		RegSetValueEx(	hTuniacPrefKey,
						WINDOWFORMATSTRING,
						NULL,
						Type,
						(LPBYTE)&m_WindowFormatString,
						Size);

		Size = (wcslen(m_PluginFormatString) + 1) * sizeof(TCHAR);
		Type = REG_SZ;
		RegSetValueEx(	hTuniacPrefKey,
						PLUGINFORMATSTRING,
						NULL,
						Type,
						(LPBYTE)&m_PluginFormatString,
						Size);

		Size = (wcslen(m_ListFormatString) + 1) * sizeof(TCHAR);
		Type = REG_SZ;
		RegSetValueEx(	hTuniacPrefKey,
						LISTFORMATSTRING,
						NULL,
						Type,
						(LPBYTE)&m_ListFormatString,
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						SOURCEVIEWDIVIDERX, 
						0,
						Type,
						(LPBYTE)&m_SourceViewDividerX, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						CROSSFADETIME, 
						0,
						Type,
						(LPBYTE)&m_CrossfadeTime, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						AUDIOBUFFERING, 
						0,
						Type,
						(LPBYTE)&m_AudioBuffering, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						CROSSFADEENABLED, 
						0,
						Type,
						(LPBYTE)&m_CrossfadeEnabled, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						VOLUME, 
						0,
						Type,
						(LPBYTE)&m_Volume, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						SHUFFLEPLAY, 
						0,
						Type,
						(LPBYTE)&m_ShuffleState, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						REPEATMODE, 
						0,
						Type,
						(LPBYTE)&m_RepeatMode, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						VISUALFPS, 
						0,
						Type,
						(LPBYTE)&m_VisualFPS, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						PLAYLISTVIEWNUMCOLS, 
						0,
						Type,
						(LPBYTE)&m_PlaylistViewNumColumns, 
						Size);

		Size = sizeof(int) * m_PlaylistViewNumColumns;
		Type = REG_BINARY;
		RegSetValueEx(	hTuniacPrefKey, 
						PLAYLISTVIEWCOLIDS, 
						0,
						Type,
						(LPBYTE)m_PlaylistViewColumnIDs, 
						Size);

		Size = sizeof(int) * m_PlaylistViewNumColumns;
		Type = REG_BINARY;
		RegSetValueEx(	hTuniacPrefKey, 
						PLAYLISTVIEWCOLWIDTHS, 
						0,
						Type,
						(LPBYTE)m_PlaylistViewColumnWidths, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						FILEASSOCTYPE, 
						0,
						Type,
						(LPBYTE)&m_FileAssocType, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						HISTORYLISTSIZE, 
						0,
						Type,
						(LPBYTE)&m_HistoryListSize, 
						Size);

		Size = sizeof(int);
		Type = REG_DWORD;
		RegSetValueEx(	hTuniacPrefKey, 
						FUTURELISTSIZE, 
						0,
						Type,
						(LPBYTE)&m_FutureListSize, 
						Size);


		RegCloseKey(hTuniacPrefKey);
	}

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
			return FALSE;
			break;
	}

	return TRUE;

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



void	CPreferences::SetSourceViewDividerX(int x)
{
	m_SourceViewDividerX = x;
}

int		CPreferences::GetSourceViewDividerX(void)
{
	return(m_SourceViewDividerX);
}

void	CPreferences::SetMainWindowRect(RECT * lpRect)
{
	CopyRect(&m_MainWindowRect, lpRect);
}

RECT *	CPreferences::GetMainWindowRect(void)
{
	return &m_MainWindowRect;
}

void	CPreferences::SetMainWindowMaximized(bool bMaximized)
{
	m_MainWindowMaximized = bMaximized;
}

bool	CPreferences::GetMainWindowMaximized(void)
{
	return m_MainWindowMaximized;
}

bool	CPreferences::CrossfadingEnabled(void)
{
	if(m_CrossfadeEnabled)
		return true;

	return false;
}

int CPreferences::GetCrossfadeTime(void)
{
	return m_CrossfadeTime;
}

void CPreferences::SetCrossfadeTime(int time)
{
	m_CrossfadeTime = time;
}

int CPreferences::GetAudioBuffering(void)
{
	return m_AudioBuffering;

}

float	CPreferences::GetVolumePercent(void)
{
	return m_Volume;
}

void	CPreferences::SetVolumePercent(float fPercent)
{
	m_Volume = fPercent;
}

int		CPreferences::GetPlaylistViewNumColumns(void)
{
	return m_PlaylistViewNumColumns;
}

void	CPreferences::SetPlaylistViewNumColumns(int iColumns)
{
	m_PlaylistViewNumColumns = iColumns;
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

LPTSTR	CPreferences::GetTheme(void)
{
	if(m_Theme[0] == L'\0')
		return NULL;
	return m_Theme;
}

bool CPreferences::GetShuffleState(void)
{
	return m_ShuffleState == TRUE ? true : false;
}

void CPreferences::SetShuffleState(bool bEnabled)
{
	m_ShuffleState = bEnabled;
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
	return m_RepeatMode;
}

void		CPreferences::SetRepeatMode(RepeatMode eMode)
{
	m_RepeatMode = eMode;
	tuniacApp.m_SourceSelectorWindow->UpdateView();
}

unsigned long	CPreferences::GetVisualFPS(void)
{
	return m_VisualFPS;
}

TrayIconMode	CPreferences::GetTrayIconMode(void)
{
	return m_eTrayIconMode;
}

void		CPreferences::SetTrayIconMode(TrayIconMode eMode)
{
	m_eTrayIconMode = eMode;
}

bool		CPreferences::GetMinimizeOnClose(void)
{
	return m_MinimizeOnClose == TRUE ? true : false;
}

bool		CPreferences::GetFollowCurrentSongMode(void)
{
	return m_FollowCurrentSong == TRUE ? true : false;
}

void		CPreferences::SetFollowCurrentSongMode(bool bEnabled)
{
	m_FollowCurrentSong = bEnabled;
}

bool		CPreferences::GetSmartSortingEnabled(void)
{
	return m_SmartSorting == TRUE;
}

bool		CPreferences::GetPauseOnLock(void)
{
	return m_PauseOnLock == TRUE ? true : false;
}

bool		CPreferences::GetPauseOnScreensave(void)
{
	return m_PauseOnScreensave == TRUE ? true : false;
}


LPTSTR		CPreferences::GetWindowFormatString(void)
{
	return m_WindowFormatString;
}

LPTSTR		CPreferences::GetPluginFormatString(void)
{
	return m_PluginFormatString;
}

LPTSTR		CPreferences::GetListFormatString(void)
{
	return m_ListFormatString;
}

bool		CPreferences::GetAlwaysOnTop(void)
{
		return m_AlwaysOnTop == TRUE ? true : false;
}

void		CPreferences::SetAlwaysOnTop(bool bEnabled)
{
	m_AlwaysOnTop = bEnabled;
}

unsigned long	CPreferences::GetHistoryListSize(void)
{
	return m_HistoryListSize;
}

int			CPreferences::GetFutureListSize(void)
{
	return m_FutureListSize;
}