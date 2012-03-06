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

// Playlist 0 is our main Library playlist


#include "stdafx.h"
#include "libraryplaylist.h"

CLibraryPlaylist::CLibraryPlaylist(void)
{
	StrCpy(m_szPlaylistName, TEXT("Media Library"));
}

CLibraryPlaylist::~CLibraryPlaylist(void)
{
}

unsigned long		CLibraryPlaylist::GetPlaylistType(void)
{
	return PLAYLIST_TYPE_MEDIALIBRARY;
}

unsigned long		CLibraryPlaylist::GetFlags(void)
{
	return PLAYLIST_FLAGS_CANRENAME | PLAYLIST_FLAGS_EXTENDED | PLAYLISTEX_FLAGS_CANFILTER | PLAYLISTEX_FLAGS_CANSORT | PLAYLISTEX_FLAGS_CANDELETE;
}

unsigned long		CLibraryPlaylist::GetRealCount(void)
{
	return m_PlaylistArray.GetCount();
}

unsigned long		CLibraryPlaylist::GetEntryIDAtRealIndex(unsigned long ulIndex)
{
	if(m_PlaylistArray[ulIndex].pIPE)
		return m_PlaylistArray[ulIndex].pIPE->GetEntryID();
	
	return INVALID_PLAYLIST_INDEX;
}

bool				CLibraryPlaylist::AddEntryToPlaylist(IPlaylistEntry * lpPLE)
{
	return false;
}

bool				CLibraryPlaylist::DeleteNormalFilteredIndexArray(IndexArray &	indexArray)
{
	bool			bRemoveFromDisk		= false;
	TCHAR			szURL[MAX_PATH + 1];

	//user wants to remove from disk
	if(GetKeyState(VK_SHIFT) < 0 && MessageBox(tuniacApp.getMainWindow(), TEXT("Would you like to remove the selected items from the harddrive?"), TEXT("Delete Items From Library"), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)
		bRemoveFromDisk = true;

	//swap to real index
	for(unsigned long i=0; i<indexArray.GetCount(); i++)
	{
		indexArray[i] = NormalFilteredIndexToRealIndex(indexArray[i]);
	}

	//remove them all
	while(indexArray.GetCount())
	{
		//move active song
		if(m_ActiveRealIndex != INVALID_PLAYLIST_INDEX)
		{
			if(indexArray[0] < m_ActiveRealIndex)
			{
				m_ActiveRealIndex--;
			}
			else if(indexArray[0] == m_ActiveRealIndex)
			{
				if(bRemoveFromDisk && (tuniacApp.m_PlaylistManager.GetActivePlaylist() == this))
					CCoreAudio::Instance()->Reset();

				m_ActiveRealIndex = INVALID_PLAYLIST_INDEX;
			}
		}

		unsigned long ulEntryID = m_PlaylistArray[indexArray[0]].pIPE->GetEntryID();
		if(bRemoveFromDisk)
		{
			ZeroMemory(szURL, (MAX_PATH + 1));
			StrCpy(szURL, (LPTSTR)m_PlaylistArray[indexArray[0]].pIPE->GetField(FIELD_URL));
		}

		//remove from all playlists
		for(unsigned long list = 0; list < tuniacApp.m_PlaylistManager.m_StandardPlaylists.GetCount(); list++)
		{
			tuniacApp.m_PlaylistManager.m_StandardPlaylists[list]->DeleteAllItemsWhereIDEquals(ulEntryID);
		}

		//remove from active playlist
		m_PlaylistArray.RemoveAt(indexArray[0]);

		//remove from ML
		tuniacApp.m_MediaLibrary.RemoveEntryID(ulEntryID);

		//remove from disk
		if(bRemoveFromDisk)
		{
			if(!PathIsURL(szURL))
			{
				SHFILEOPSTRUCT				op;

				op.hwnd						= tuniacApp.getMainWindow();
				op.wFunc					= FO_DELETE;
				op.pFrom					= szURL;
				op.pTo						= NULL;
				op.fFlags					= FOF_ALLOWUNDO | FOF_WANTNUKEWARNING | FOF_NOCONFIRMATION;
				op.fAnyOperationsAborted	= true;
				op.hNameMappings			= NULL;
				op.lpszProgressTitle		= NULL;

				::SHFileOperation(&op);
			}
		}

		//move indexes to compensate for now deleted files
		for(unsigned long i=1; i<indexArray.GetCount(); i++)
		{
			if(indexArray[0] < indexArray[i])
				indexArray[i]--;
		}

		indexArray.RemoveAt(0);
	}

	//refresh playlists and view
	RebuildPlaylistArrays();
	tuniacApp.RebuildFutureMenu();
	tuniacApp.m_SourceSelectorWindow->UpdateView();

	return true;
}


bool				CLibraryPlaylist::RebuildPlaylist(void)
{
	for(unsigned long x=0; x<tuniacApp.m_MediaLibrary.GetCount(); x++)
	{
		bool bFound = false;
		IPlaylistEntry * t = tuniacApp.m_MediaLibrary.GetEntryByIndex(x);

		for(unsigned long y = 0; y<m_PlaylistArray.GetCount(); y++)
		{
			if(t->GetEntryID() == m_PlaylistArray[y].pIPE->GetEntryID())
			{
				bFound = true;
				break;
			}
		}

		if(!bFound)
		{
			PlaylistEntry PE;

			PE.bFiltered	= false;
			PE.pIPE		= t;

			m_PlaylistArray.AddTail(PE);
			m_SortType		= SORTED_UNSORTED;
		}
	}

	return true;
}
