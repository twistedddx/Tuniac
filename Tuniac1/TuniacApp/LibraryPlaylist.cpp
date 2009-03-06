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
#include ".\libraryplaylist.h"

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

unsigned long		CLibraryPlaylist::GetIDAtRealIndex(unsigned long ulIndex)
{
	return m_PlaylistArray[ulIndex].pEntry->GetEntryID();
}

bool				CLibraryPlaylist::AddEntryToPlaylist(IPlaylistEntry * lpPLE)
{
	return false;
}

bool				CLibraryPlaylist::DeleteItemArray(IndexArray &	indexArray)
{
#define STRINGSIZE	128*1024*sizeof(TCHAR)

	bool			bRemoveFromDisk		= false;
	bool			bIsURL				= false;
	bool			bAskOnce			= false;
	TCHAR		*	szFilename;

	szFilename = (LPTSTR)malloc(STRINGSIZE);
	ZeroMemory(szFilename, STRINGSIZE);

	LPTSTR			szCopyPtr			= szFilename;

	int loop = 0;

	if(GetKeyState(VK_SHIFT) < 0 && MessageBox(tuniacApp.getMainWindow(), TEXT("Would you like to remove the selected items from the harddrive?"), TEXT("Delete Items From Library"), MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)
	{
		bRemoveFromDisk = true;
	}

	for(unsigned long x=0; x<indexArray.GetCount(); x++)
	{
		indexArray[x] = NormalFilteredIndexToRealIndex(indexArray[x]);
	}

	while(indexArray.GetCount())
	{
		bIsURL = false;
		for(unsigned long x=1; x<indexArray.GetCount(); x++)
		{
			if(indexArray[0] < indexArray[x])
				indexArray[x]--;
		}

		unsigned long ulRealIndex;
		ulRealIndex = indexArray[0];
		IPlaylistEntry * pEntry = m_PlaylistArray[ulRealIndex].pEntry;

		if(m_ActiveRealIndex != INVALID_PLAYLIST_INDEX)
		{
			if(ulRealIndex < m_ActiveRealIndex)
			{
				m_ActiveRealIndex--;
			}
			else if(ulRealIndex == m_ActiveRealIndex)
			{
				if((bRemoveFromDisk) && (tuniacApp.m_PlaylistManager.GetActivePlaylist() == this))
				{
					CCoreAudio::Instance()->Reset();
				}
				m_ActiveRealIndex = INVALID_PLAYLIST_INDEX;
			}
		}

		if(PathIsURL((LPTSTR)pEntry->GetField(FIELD_URL)))
		{
			bIsURL = true;
		}

		if((bRemoveFromDisk) && (!bIsURL))
		{
			StrCpy(szCopyPtr, (LPTSTR)pEntry->GetField(FIELD_URL));
			szCopyPtr = &szCopyPtr[lstrlen(szCopyPtr)+1];
		}

		for(unsigned long list = 0; list < tuniacApp.m_PlaylistManager.m_StandardPlaylists.GetCount(); list++)
		{
			tuniacApp.m_PlaylistManager.m_StandardPlaylists[list]->DeleteAllItemsWhereIDEquals(pEntry->GetEntryID());
		}

		m_PlaylistArray.RemoveAt(ulRealIndex);

		tuniacApp.m_MediaLibrary.RemoveItem(pEntry);
		indexArray.RemoveAt(0);
		loop ++;
	}

	if(bRemoveFromDisk)
	{
		SHFILEOPSTRUCT				op;

		op.hwnd						= tuniacApp.getMainWindow();
		op.wFunc					= FO_DELETE;
		op.pFrom					= szFilename;
		op.pTo						= NULL;
		op.fFlags					= FOF_ALLOWUNDO | FOF_WANTNUKEWARNING | FOF_NOCONFIRMATION;
		op.fAnyOperationsAborted	= true;
		op.hNameMappings			= NULL;
		op.lpszProgressTitle		= NULL;

		::SHFileOperation(&op);
	}

	free(szFilename);
	ApplyFilter();

	return true;
}


bool				CLibraryPlaylist::RebuildPlaylist(void)
{
	for(unsigned long x=0; x<tuniacApp.m_MediaLibrary.GetCount(); x++)
	{
		bool bFound = false;
		IPlaylistEntry * t = tuniacApp.m_MediaLibrary.GetItemByIndex(x);

		for(unsigned long y = 0; y<m_PlaylistArray.GetCount(); y++)
		{
			if(t->GetEntryID() == m_PlaylistArray[y].pEntry->GetEntryID())
			{
				bFound = true;
				break;
			}
		}

		if(!bFound)
		{
			PlaylistEntry PE;

			PE.bFiltered	= false;
			PE.pEntry		= t;

			m_PlaylistArray.AddTail(PE);
			m_SortType		= SORTED_UNSORTED;
		}
	}

	return true;
}
