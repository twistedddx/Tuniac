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
#include ".\medialibraryplaylistentry.h"

CMediaLibraryPlaylistEntry::CMediaLibraryPlaylistEntry(LibraryEntry * pEntry)
{
	CopyMemory(&m_LibraryEntry, pEntry, sizeof(LibraryEntry));
	m_bDirty = false;
}

CMediaLibraryPlaylistEntry::~CMediaLibraryPlaylistEntry(void)
{
}

bool CMediaLibraryPlaylistEntry::IsDirty(void)
{
	return m_bDirty;
}

void CMediaLibraryPlaylistEntry::SetEntryID(unsigned long ulEntryID)
{
	m_ulEntryID = ulEntryID;
}

unsigned long CMediaLibraryPlaylistEntry::GetEntryID(void)
{
	return m_ulEntryID;
}

void *	CMediaLibraryPlaylistEntry::GetField(unsigned long ulFieldID)
{
	switch(ulFieldID)
	{
		case FIELD_KIND:
			{
				return (void*)(m_LibraryEntry.dwKind);
			}
			break;

		case FIELD_URL:
			{
				return m_LibraryEntry.szURL;
			}
			break;

		case FIELD_FILENAME:
			{
				return PathFindFileName(m_LibraryEntry.szURL);
			}
			break;

		case FIELD_FILESIZE:
			{
				return (void*)(m_LibraryEntry.dwFilesize);
			}
			break;

		case FIELD_DATEADDED:
			{
				return & m_LibraryEntry.stDateAdded;
			}
			break;

		case FIELD_DATEFILECREATION:
			{
				return & m_LibraryEntry.stFileCreationDate;
			}
			break;

		case FIELD_DATELASTPLAYED:
			{
				return & m_LibraryEntry.stLastPlayed;
			}
			break;

		case FIELD_PLAYCOUNT:
			{
				return (void *)m_LibraryEntry.dwPlayCount;
			}
			break;

		case FIELD_RATING:
			{
				return (void *)m_LibraryEntry.dwRating;
			}
			break;

		case FIELD_TITLE:
			{
				return m_LibraryEntry.szTitle;
			}
			break;
			
		case FIELD_ARTIST:
			{
				return m_LibraryEntry.szArtist;
			}
			break;

		case FIELD_ALBUM:
			{
				return m_LibraryEntry.szAlbum;
			}
			break;

		case FIELD_COMMENT:
			{
				return m_LibraryEntry.szComment;
			}
			break;

		case FIELD_GENRE:
			{
				return m_LibraryEntry.szGenre;
			}
			break;

		case FIELD_YEAR:
			{
				return (void *)m_LibraryEntry.iYear;
			}
			break;

		case FIELD_TRACKNUM:
			{
				return & m_LibraryEntry.dwTrack;
			}
			break;

		case FIELD_PLAYBACKTIME:
			{
				return (void *)m_LibraryEntry.iPlaybackTime;
			}
			break;

		case FIELD_BITRATE:
			{
				return (void *)m_LibraryEntry.iBitRate;
			}
			break;

		case FIELD_SAMPLERATE:
			{
				return (void *)m_LibraryEntry.iSampleRate;
			}
			break;

		case FIELD_NUMCHANNELS:
			{
				return (void *)m_LibraryEntry.iChannels;
			}
			break;

		case FIELD_FILEEXTENSION:
			{
				if(PathIsURL(m_LibraryEntry.szURL))
					return L"";
				return (void *)PathFindExtension(m_LibraryEntry.szURL);
			}
			break;
	}

	return NULL;
}

bool	CMediaLibraryPlaylistEntry::SetField(unsigned long ulFieldID, void * pNewData)
{
	switch(ulFieldID)
	{
		case FIELD_DATELASTPLAYED:
			{
				// pNewData is a pointer to a SYSTEMTIME

				LPSYSTEMTIME lpST = (LPSYSTEMTIME)pNewData;
				CopyMemory(&m_LibraryEntry.stLastPlayed, lpST, sizeof(SYSTEMTIME));
				m_LibraryEntry.dwPlayCount++;
			}
			break;

		case FIELD_RATING:
			{
				m_LibraryEntry.dwRating = (int)pNewData;
			}
			break;

		case FIELD_FILENAME:
			{
				StrCpyN(PathFindFileName(m_LibraryEntry.szURL), (LPTSTR)pNewData, 128);
			}
			break;

		case FIELD_TITLE:
			{
				StrCpyN(m_LibraryEntry.szTitle, (LPTSTR)pNewData, 128);
			}
			break;
			
		case FIELD_ARTIST:
			{
				StrCpyN(m_LibraryEntry.szArtist, (LPTSTR)pNewData, 128);
			}
			break;

		case FIELD_ALBUM:
			{
				StrCpyN(m_LibraryEntry.szAlbum, (LPTSTR)pNewData, 128);
			}
			break;

		case FIELD_COMMENT:
			{
				StrCpyN(m_LibraryEntry.szComment, (LPTSTR)pNewData, 128);
			}
			break;

		case FIELD_GENRE:
			{
				StrCpyN(m_LibraryEntry.szGenre, (LPTSTR)pNewData, 128);
			}
			break;

		case FIELD_YEAR:
			{
				if(1 != swscanf_s((LPTSTR)pNewData, TEXT("%d"), &m_LibraryEntry.iYear))
                    return false;
			}
			break;

		case FIELD_TRACKNUM:
			{
				bool bOK = false;
				unsigned short dwNewTrack[1];
				if(StrChr((LPTSTR)pNewData, '/') != NULL)
				{
					bOK = 2 == swscanf_s((LPTSTR)pNewData, TEXT("%u/%u"), &dwNewTrack[0], &dwNewTrack[1]);
					if(bOK)
						m_LibraryEntry.dwTrack[1] = dwNewTrack[1];
				}
				else
				{
					bOK = 1 == swscanf_s((LPTSTR)pNewData, TEXT("%u"), &dwNewTrack[0]);
					if(bOK)
						m_LibraryEntry.dwTrack[1] = 0;
				}
				if(!bOK)
					return false;
				m_LibraryEntry.dwTrack[0] = dwNewTrack[0];
			}
			break;

		default:
			return false;
	}

	m_bDirty = true;

	return true;
}

bool	CMediaLibraryPlaylistEntry::GetTextRepresentation(unsigned long ulFieldID, LPTSTR szString, unsigned long ulNumChars)
{
	switch(ulFieldID)
	{
		case FIELD_KIND:
			{
				if(m_LibraryEntry.dwKind == ENTRY_KIND_URL)
					StrCpyN(szString, TEXT("URL"), ulNumChars);
				else
					StrCpyN(szString, TEXT("File"), ulNumChars);
			}
			break;

		case FIELD_URL:
			{
				StrCpyN(szString, m_LibraryEntry.szURL, ulNumChars);
			}
			break;

		case FIELD_FILENAME:
			{
				StrCpyN(szString, PathFindFileName(m_LibraryEntry.szURL), ulNumChars);
			}
			break;

		case FIELD_FILESIZE:
			{	// NOTE: optimize divides with bitshifts?? i / 1024 == i >> 10 
				if (m_LibraryEntry.dwFilesize < 1024)
				{
					wnsprintf(szString, ulNumChars, TEXT("%d bytes"), m_LibraryEntry.dwFilesize);
					break;
				}
				unsigned long ulSize = m_LibraryEntry.dwFilesize / 1024;
				if (ulSize < 1024)
				{
					wnsprintf(szString, ulNumChars, TEXT("%d Kb"), ulSize);
					break;
				}
				ulSize /= 1024;
				if (ulSize < 1024)
				{
					wnsprintf(szString, ulNumChars, TEXT("%d Mb"), ulSize);
					break;
				}
				ulSize /= 1024;
				wnsprintf(szString, ulNumChars, TEXT("%d Gb"), ulSize);
			}
			break;

		case FIELD_DATEADDED:
			{
				int x = GetTimeFormat(LOCALE_USER_DEFAULT, 0, &m_LibraryEntry.stDateAdded, NULL, szString, 100);
				StrCpy(&szString[x-1], TEXT(" "));
				GetDateFormat(LOCALE_USER_DEFAULT, 0, &m_LibraryEntry.stDateAdded, NULL, &szString[x], 100);
			}
			break;

		case FIELD_DATEFILECREATION:
			{
				int x = GetTimeFormat(LOCALE_USER_DEFAULT, 0, &m_LibraryEntry.stFileCreationDate, NULL, szString, 100);
				StrCpy(&szString[x-1], TEXT(" "));
				GetDateFormat(LOCALE_USER_DEFAULT, 0, &m_LibraryEntry.stFileCreationDate, NULL, &szString[x], 100);
			}
			break;

		case FIELD_DATELASTPLAYED:
			{
				if(m_LibraryEntry.dwPlayCount > 0)
				{
					int x = GetTimeFormat(LOCALE_USER_DEFAULT, 0, &m_LibraryEntry.stLastPlayed, NULL, szString, 100);
					StrCpy(&szString[x-1], TEXT(" "));
					GetDateFormat(LOCALE_USER_DEFAULT, 0, &m_LibraryEntry.stLastPlayed, NULL, &szString[x], 100);
				}
				else
				{
					StrCpyN(szString, TEXT("Never"), ulNumChars);
				}
			}
			break;

		case FIELD_PLAYCOUNT:
			{
				wnsprintf(szString, ulNumChars, TEXT("%d"), m_LibraryEntry.dwPlayCount);
			}
			break;

		case FIELD_RATING:
			{
				wnsprintf(szString, ulNumChars, TEXT("%d"), m_LibraryEntry.dwRating);
			}
			break;

		case FIELD_TITLE:
			{
				StrCpyN(szString, m_LibraryEntry.szTitle, ulNumChars);
			}
			break;
			
		case FIELD_ARTIST:
			{
				StrCpyN(szString, m_LibraryEntry.szArtist, ulNumChars);
			}
			break;

		case FIELD_ALBUM:
			{
				StrCpyN(szString, m_LibraryEntry.szAlbum, ulNumChars);
			}
			break;

		case FIELD_COMMENT:
			{
				StrCpyN(szString, m_LibraryEntry.szComment, ulNumChars);
			}
			break;

		case FIELD_GENRE:
			{
				StrCpyN(szString, m_LibraryEntry.szGenre, ulNumChars);
			}
			break;

		case FIELD_YEAR:
			{
				if(m_LibraryEntry.iYear > 0)
					wnsprintf(szString, ulNumChars, TEXT("%d"), m_LibraryEntry.iYear);
			}
			break;

		case FIELD_TRACKNUM:
			{
				if(m_LibraryEntry.dwTrack[1])
				{
					wnsprintf(szString, ulNumChars, TEXT("%d/%d"), m_LibraryEntry.dwTrack[0], m_LibraryEntry.dwTrack[1]);
				}
				else
				{
					wnsprintf(szString, ulNumChars, TEXT("%d"), m_LibraryEntry.dwTrack[0]);
				}
			}
			break;

		case FIELD_PLAYBACKTIME:
			{
				if(m_LibraryEntry.iPlaybackTime <= 0)
				{
					StrCpyN(szString, TEXT("Inf"), ulNumChars);
				}
				else
				{
					int time = m_LibraryEntry.iPlaybackTime / 1000;
					wnsprintf(szString, ulNumChars, TEXT("%02d:%02d:%02d"),  ((time / 60) / 60), (time / 60) % 60, time % 60 );
				}
			}
			break;

		case FIELD_BITRATE:
			{
				wnsprintf(szString, ulNumChars, TEXT("%dkbps"), m_LibraryEntry.iBitRate/1000);
			}
			break;

		case FIELD_SAMPLERATE:
			{
				wnsprintf(szString, ulNumChars, TEXT("%dHz"), m_LibraryEntry.iSampleRate);
			}
			break;

		case FIELD_NUMCHANNELS:
			{
				if(m_LibraryEntry.iChannels == 0)
					StrCpyN(szString, TEXT("Mute"), ulNumChars);
				if(m_LibraryEntry.iChannels == 1)
					StrCpyN(szString, TEXT("Mono"), ulNumChars);
				if(m_LibraryEntry.iChannels == 2)
					StrCpyN(szString, TEXT("Stereo"), ulNumChars);
				if(m_LibraryEntry.iChannels == 3)
					StrCpyN(szString, TEXT("2.1"), ulNumChars);
				if(m_LibraryEntry.iChannels == 4)
					StrCpyN(szString, TEXT("Quad"), ulNumChars);
				if(m_LibraryEntry.iChannels == 5)
					StrCpyN(szString, TEXT("5"), ulNumChars);
				if(m_LibraryEntry.iChannels == 6)
					StrCpyN(szString, TEXT("5.1"), ulNumChars);
				if(m_LibraryEntry.iChannels == 7)
					StrCpyN(szString, TEXT("6.1"), ulNumChars);
				if(m_LibraryEntry.iChannels == 8)
					StrCpyN(szString, TEXT("7.1"), ulNumChars);
				if(m_LibraryEntry.iChannels > 7)
					StrCpyN(szString, TEXT("Plenty"), ulNumChars);
			}
			break;

		case FIELD_FILEEXTENSION:
			{
				if(PathIsURL(m_LibraryEntry.szURL))
					StrCpyN(szString, TEXT(""), ulNumChars);
				else
					StrCpyN(szString, PathFindExtension(m_LibraryEntry.szURL), ulNumChars);
			}
			break;

		default:
			return false;
	}

	return true;
}