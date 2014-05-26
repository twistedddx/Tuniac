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
	Copyright (C) 2003-2012 Brett Hoyle
*/

#include "stdafx.h"
#include "medialibraryplaylistentry.h"

CMediaLibraryPlaylistEntry::CMediaLibraryPlaylistEntry(LibraryEntry * pIPE)
{
	CopyMemory(&m_LibraryEntry, pIPE, sizeof(LibraryEntry));
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
				return (void *)m_LibraryEntry.ulYear;
			}
			break;

		case FIELD_TRACKNUM:
			{
				return (void *)m_LibraryEntry.dwTrack;
			}
			break;

		case FIELD_PLAYBACKTIME:
			{
				return (void *)m_LibraryEntry.ulPlaybackTime;
			}
			break;

		case FIELD_BITRATE:
			{
				return (void *)m_LibraryEntry.ulBitRate;
			}
			break;

		case FIELD_SAMPLERATE:
			{
				return (void *)m_LibraryEntry.ulSampleRate;
			}
			break;

		case FIELD_NUMCHANNELS:
			{
				return (void *)m_LibraryEntry.ulChannels;
			}
			break;

		case FIELD_KIND:
			{
				return (void*)m_LibraryEntry.ulKind;
			}
			break;

		case FIELD_FILEEXTENSION:
			{
				if(PathIsURL(m_LibraryEntry.szURL))
					return L"";
				return (void *)PathFindExtension(m_LibraryEntry.szURL);
			}
			break;

		case FIELD_FILESIZE:
			{
				return (void*)(m_LibraryEntry.ulFilesize);
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
				return (void *)m_LibraryEntry.ulPlayCount;
			}
			break;

		case FIELD_RATING:
			{
				return (void *)m_LibraryEntry.ulRating;
			}
			break;

		case FIELD_REPLAYGAIN_TRACK_GAIN:
			{
				return & m_LibraryEntry.fReplayGain_Track_Gain;
			}
			break;

		case FIELD_REPLAYGAIN_TRACK_PEAK:
			{
				return & m_LibraryEntry.fReplayGain_Track_Peak;
			}
			break;

		case FIELD_REPLAYGAIN_ALBUM_GAIN:
			{
				return & m_LibraryEntry.fReplayGain_Album_Gain;
			}
			break;

		case FIELD_REPLAYGAIN_ALBUM_PEAK:
			{
				return & m_LibraryEntry.fReplayGain_Album_Peak;
			}
			break;

		case FIELD_AVAILABILITY:
			{
				return (void *)m_LibraryEntry.ulAvailability;
			}
			break;
		case FIELD_BPM:
			{
				return (void *)m_LibraryEntry.ulBPM;
			}
			break;

		case FIELD_ALBUMARTIST:
			{
				return m_LibraryEntry.szAlbumArtist;
			}
			break;

		case FIELD_COMPOSER:
			{
				return m_LibraryEntry.szComposer;
			}
			break;
	}

	return NULL;
}

bool	CMediaLibraryPlaylistEntry::SetField(unsigned long ulFieldID, void * pNewData)
{
	switch(ulFieldID)
	{
		case FIELD_URL:
			{
				StringCchCopy(m_LibraryEntry.szURL, MAX_PATH, (LPTSTR)pNewData);
			}
			break;

		case FIELD_TITLE:
			{
				StringCchCopy(m_LibraryEntry.szTitle, 128, (LPTSTR)pNewData);
			}
			break;
			
		case FIELD_ARTIST:
			{
				StringCchCopy(m_LibraryEntry.szArtist, 128, (LPTSTR)pNewData);
			}
			break;

		case FIELD_ALBUM:
			{
				StringCchCopy(m_LibraryEntry.szAlbum, 128, (LPTSTR)pNewData);
			}
			break;

		case FIELD_COMMENT:
			{
				StringCchCopy(m_LibraryEntry.szComment, 128, (LPTSTR)pNewData);
			}
			break;

		case FIELD_GENRE:
			{
				StringCchCopy(m_LibraryEntry.szGenre, 128, (LPTSTR)pNewData);
			}
			break;

		case FIELD_DATELASTPLAYED:
			{
				// pNewData is a pointer to a SYSTEMTIME
				LPSYSTEMTIME lpST = (LPSYSTEMTIME)pNewData;
				CopyMemory(&m_LibraryEntry.stLastPlayed, lpST, sizeof(SYSTEMTIME));
			}
			break;

		case FIELD_DATEADDED:
			{
				// pNewData is a pointer to a SYSTEMTIME
				LPSYSTEMTIME lpST = (LPSYSTEMTIME)pNewData;
				CopyMemory(&m_LibraryEntry.stDateAdded, lpST, sizeof(SYSTEMTIME));
			}
			break;

		case FIELD_DATEFILECREATION:
			{
				// pNewData is a pointer to a SYSTEMTIME
				LPSYSTEMTIME lpST = (LPSYSTEMTIME)pNewData;
				CopyMemory(&m_LibraryEntry.stFileCreationDate, lpST, sizeof(SYSTEMTIME));
			}
			break;

//always string
		case FIELD_YEAR:
			{
				if(StrCmpI((LPTSTR)pNewData, L"") == 0)
					pNewData = L"0";
				m_LibraryEntry.ulYear = wcstoul((LPTSTR)pNewData, NULL, 10);
			}
			break;

		case FIELD_PLAYCOUNT:
			{
				if(StrCmpI((LPTSTR)pNewData, L"") == 0)
					pNewData = L"0";
				m_LibraryEntry.ulPlayCount = wcstoul((LPTSTR)pNewData, NULL, 10);
			}
			break;

		case FIELD_TRACKNUM:
			{
				bool bOK = false;
				unsigned short dwNewTrack[2];
				if(StrChr((LPTSTR)pNewData, '/') != NULL)
				{
					bOK = 2 == swscanf_s((LPTSTR)pNewData, TEXT("%hu/%hu"), &dwNewTrack[0], &dwNewTrack[1]);
					if(bOK)
						m_LibraryEntry.dwTrack[1] = dwNewTrack[1];
					else
						return false;
				}
				else
				{
					if(StrCmpI((LPTSTR)pNewData, L"") == 0)
						dwNewTrack[0] = 0;
					else
						dwNewTrack[0] = _wtoi((LPTSTR)pNewData);
					m_LibraryEntry.dwTrack[1] = 0;
				}

				m_LibraryEntry.dwTrack[0] = dwNewTrack[0];
			}
			break;

		case FIELD_NUMCHANNELS:
			{
				if(StrCmpI((LPTSTR)pNewData, L"") == 0)
					pNewData = L"0";
				m_LibraryEntry.ulChannels = wcstoul((LPTSTR)pNewData, NULL, 10);
			}
			break;

		case FIELD_BITRATE:
			{
				if(StrCmpI((LPTSTR)pNewData, L"") == 0)
					pNewData = L"0";
				m_LibraryEntry.ulBitRate = wcstoul((LPTSTR)pNewData, NULL, 10);
			}
			break;

		case FIELD_SAMPLERATE:
			{
				if(StrCmpI((LPTSTR)pNewData, L"") == 0)
					pNewData = L"0";
				m_LibraryEntry.ulSampleRate = wcstoul((LPTSTR)pNewData, NULL, 10);
			}
			break;

		case FIELD_PLAYBACKTIME:
			{
				if(StrCmpI((LPTSTR)pNewData, L"") == 0)
					pNewData = L"0";
				m_LibraryEntry.ulPlaybackTime = wcstoul((LPTSTR)pNewData, NULL, 10);
			}
			break;

		case FIELD_RATING:
			{
				if(StrCmpI((LPTSTR)pNewData, L"") == 0)
					pNewData = L"0";
				m_LibraryEntry.ulRating = wcstoul((LPTSTR)pNewData, NULL, 10);
			}
			break;

		case FIELD_KIND:
			{
				if(StrCmpI((LPTSTR)pNewData, L"") == 0)
					pNewData = L"0";
				m_LibraryEntry.ulKind = wcstoul((LPTSTR)pNewData, NULL, 10);
			}
			break;

		case FIELD_AVAILABILITY:
			{
				if(StrCmpI((LPTSTR)pNewData, L"") == 0)
					pNewData = L"0";
				m_LibraryEntry.ulAvailability = wcstoul((LPTSTR)pNewData, NULL, 10);
			}
			break;

		case FIELD_FILESIZE:
			{
				if(StrCmpI((LPTSTR)pNewData, L"") == 0)
					pNewData = L"0";
				m_LibraryEntry.ulFilesize = wcstoul((LPTSTR)pNewData, NULL, 10);
			}
			break;

		case FIELD_BPM:
			{
				if(StrCmpI((LPTSTR)pNewData, L"") == 0)
					pNewData = L"0";
				m_LibraryEntry.ulBPM = wcstoul((LPTSTR)pNewData, NULL, 10);
			}
			break;

		case FIELD_ALBUMARTIST:
			{
				StringCchCopy(m_LibraryEntry.szAlbumArtist, 128, (LPTSTR)pNewData);
			}
			break;

		case FIELD_COMPOSER:
			{
				StringCchCopy(m_LibraryEntry.szComposer, 128, (LPTSTR)pNewData);
			}
			break;

		default:
			return false;
	}

	m_bDirty = true;

	return true;
}

bool	CMediaLibraryPlaylistEntry::SetField(unsigned long ulFieldID, unsigned long pNewData)
{
	switch(ulFieldID)
	{
		case FIELD_PLAYCOUNT:
			{
				m_LibraryEntry.ulPlayCount = pNewData;
			}
			break;
		case FIELD_YEAR:
			{
				m_LibraryEntry.ulYear = pNewData;
			}
			break;
		case FIELD_TRACKNUM:
			{
				m_LibraryEntry.dwTrack[0] = pNewData;
			}
			break;
		case FIELD_NUMCHANNELS:
			{
				m_LibraryEntry.ulChannels = pNewData;
			}
			break;
		case FIELD_BITRATE:
			{
				m_LibraryEntry.ulBitRate = pNewData;
			}
			break;
		case FIELD_SAMPLERATE:
			{
				m_LibraryEntry.ulSampleRate = pNewData;
			}
			break;
		case FIELD_PLAYBACKTIME:
			{
				m_LibraryEntry.ulPlaybackTime = pNewData;
			}
			break;
		case FIELD_RATING:
			{
				m_LibraryEntry.ulRating = pNewData;
			}
			break;
		case FIELD_AVAILABILITY:
			{
				m_LibraryEntry.ulAvailability = pNewData;
			}
			break;
		case FIELD_KIND:
			{
				m_LibraryEntry.ulKind = pNewData;
			}
			break;
		case FIELD_FILESIZE:
			{
				m_LibraryEntry.ulFilesize = pNewData;
			}
			break;
		case FIELD_BPM:
			{
				m_LibraryEntry.ulBPM = pNewData;
			}
			break;

		default:
			return false;
	}

	m_bDirty = true;

	return true;
}

bool	CMediaLibraryPlaylistEntry::SetField(unsigned long ulFieldID, float pNewData)
{
	switch(ulFieldID)
	{
		case FIELD_REPLAYGAIN_TRACK_GAIN:
			{
				m_LibraryEntry.fReplayGain_Track_Gain = pNewData;
			}
			break;
		case FIELD_REPLAYGAIN_TRACK_PEAK:
			{
				m_LibraryEntry.fReplayGain_Track_Peak = pNewData;
			}
			break;
		case FIELD_REPLAYGAIN_ALBUM_GAIN:
			{
				m_LibraryEntry.fReplayGain_Album_Gain = pNewData;
			}
			break;
		case FIELD_REPLAYGAIN_ALBUM_PEAK:
			{
				m_LibraryEntry.fReplayGain_Album_Peak = pNewData;
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
				if(m_LibraryEntry.ulKind == ENTRY_KIND_URL)
					StringCchCopy(szString, ulNumChars, TEXT("URL"));
				else
					StringCchCopy(szString, ulNumChars, TEXT("File"));
			}
			break;

		case FIELD_URL:
			{
				StringCchCopy(szString, ulNumChars, m_LibraryEntry.szURL);
			}
			break;

		case FIELD_FILENAME:
			{
				StringCchCopy(szString, ulNumChars, PathFindFileName(m_LibraryEntry.szURL));
			}
			break;

		case FIELD_TITLE:
			{
				StringCchCopy(szString, ulNumChars, m_LibraryEntry.szTitle);
			}
			break;
			
		case FIELD_ARTIST:
			{
				StringCchCopy(szString, ulNumChars, m_LibraryEntry.szArtist);
			}
			break;

		case FIELD_ALBUM:
			{
				StringCchCopy(szString, ulNumChars, m_LibraryEntry.szAlbum);
			}
			break;

		case FIELD_COMMENT:
			{
				StringCchCopy(szString, ulNumChars, m_LibraryEntry.szComment);
			}
			break;

		case FIELD_GENRE:
			{
				StringCchCopy(szString, ulNumChars, m_LibraryEntry.szGenre);
			}
			break;

		case FIELD_YEAR:
			{
				if(m_LibraryEntry.ulYear < 1)
					StringCchCopy(szString, ulNumChars, TEXT("Unknown"));
				else
					StringCchPrintf(szString, ulNumChars, TEXT("%u"), m_LibraryEntry.ulYear);
			}
			break;

		case FIELD_PLAYCOUNT:
			{
				StringCchPrintf(szString, ulNumChars, TEXT("%u"), m_LibraryEntry.ulPlayCount);
			}
			break;

		case FIELD_TRACKNUM:
			{
				if(m_LibraryEntry.dwTrack[1])
				{
					StringCchPrintf(szString, ulNumChars, TEXT("%hu/%hu"), m_LibraryEntry.dwTrack[0], m_LibraryEntry.dwTrack[1]);
				}
				else
				{
					StringCchPrintf(szString, ulNumChars, TEXT("%hu"), m_LibraryEntry.dwTrack[0]);
				}
			}
			break;

		case FIELD_PLAYBACKTIME:
			{
				if(m_LibraryEntry.ulPlaybackTime == LENGTH_STREAM)
				{
					StringCchCopy(szString, ulNumChars, TEXT("Stream"));
				}
				else if(m_LibraryEntry.ulPlaybackTime == LENGTH_UNKNOWN)
				{
					StringCchCopy(szString, ulNumChars, TEXT("Unknown"));
				}
				else
				{
					unsigned long time = m_LibraryEntry.ulPlaybackTime / 1000;
					if(time > 3600)
						StringCchPrintf(szString, ulNumChars, TEXT("%02u:%02u:%02u"),  ((time / 60) / 60), (time / 60) % 60, time % 60 );
					else
						StringCchPrintf(szString, ulNumChars, TEXT("%02u:%02u"), (time / 60) % 60, time % 60 );
				}
			}
			break;

		case FIELD_BITRATE:
			{
				StringCchPrintf(szString, ulNumChars, TEXT("%ukbps"), m_LibraryEntry.ulBitRate/1000);
			}
			break;

		case FIELD_SAMPLERATE:
			{
				StringCchPrintf(szString, ulNumChars, TEXT("%uHz"), m_LibraryEntry.ulSampleRate);
			}
			break;

		case FIELD_NUMCHANNELS:
			{
				if(m_LibraryEntry.ulChannels == 0)
					StringCchCopy(szString, ulNumChars, TEXT("Mute"));
				if(m_LibraryEntry.ulChannels == 1)
					StringCchCopy(szString, ulNumChars, TEXT("Mono"));
				if(m_LibraryEntry.ulChannels == 2)
					StringCchCopy(szString, ulNumChars, TEXT("Stereo"));
				if(m_LibraryEntry.ulChannels == 3)
					StringCchCopy(szString, ulNumChars, TEXT("2.1"));
				if(m_LibraryEntry.ulChannels == 4)
					StringCchCopy(szString, ulNumChars, TEXT("Quad"));
				if(m_LibraryEntry.ulChannels == 5)
					StringCchCopy(szString, ulNumChars, TEXT("5"));
				if(m_LibraryEntry.ulChannels == 6)
					StringCchCopy(szString, ulNumChars, TEXT("5.1"));
				if(m_LibraryEntry.ulChannels == 7)
					StringCchCopy(szString, ulNumChars, TEXT("6.1"));
				if(m_LibraryEntry.ulChannels == 8)
					StringCchCopy(szString, ulNumChars, TEXT("7.1"));
				if(m_LibraryEntry.ulChannels > 8)
					StringCchCopy(szString, ulNumChars, TEXT("Plenty"));
			}
			break;

		case FIELD_FILEEXTENSION:
			{
				if(PathIsURL(m_LibraryEntry.szURL))
					StringCchCopy(szString, ulNumChars, TEXT(""));
				else
					StringCchCopy(szString, ulNumChars, PathFindExtension(m_LibraryEntry.szURL));
			}
			break;

		case FIELD_RATING:
			{
				StringCchPrintf(szString, ulNumChars, TEXT("%u"), m_LibraryEntry.ulRating);
			}
			break;

		case FIELD_FILESIZE:
			{	// NOTE: optimize divides with bitshifts?? i / 1024 == i >> 10 
				if (m_LibraryEntry.ulFilesize < 1024)
				{
					StringCchPrintf(szString, ulNumChars, TEXT("%u bytes"), m_LibraryEntry.ulFilesize);
					break;
				}
				unsigned long ulSize = m_LibraryEntry.ulFilesize / 1024;
				if (ulSize < 1024)
				{
					StringCchPrintf(szString, ulNumChars, TEXT("%u Kb"), ulSize);
					break;
				}
				ulSize /= 1024;
				if (ulSize < 1024)
				{
					StringCchPrintf(szString, ulNumChars, TEXT("%u Mb"), ulSize);
					break;
				}
				ulSize /= 1024;
				StringCchPrintf(szString, ulNumChars, TEXT("%u Gb"), ulSize);
			}
			break;

		case FIELD_DATEADDED:
			{
				int x = GetTimeFormat(LOCALE_USER_DEFAULT, 0, &m_LibraryEntry.stDateAdded, NULL, szString, 100);
				StringCchCopy(&szString[x-1], ulNumChars, TEXT(" "));
				GetDateFormat(LOCALE_USER_DEFAULT, 0, &m_LibraryEntry.stDateAdded, NULL, &szString[x], 100);
			}
			break;

		case FIELD_DATEFILECREATION:
			{
				int x = GetTimeFormat(LOCALE_USER_DEFAULT, 0, &m_LibraryEntry.stFileCreationDate, NULL, szString, 100);
				StringCchCopy(&szString[x-1], ulNumChars, TEXT(" "));
				GetDateFormat(LOCALE_USER_DEFAULT, 0, &m_LibraryEntry.stFileCreationDate, NULL, &szString[x], 100);
			}
			break;

		case FIELD_DATELASTPLAYED:
			{
				if(m_LibraryEntry.ulPlayCount > 0)
				{
					int x = GetTimeFormat(LOCALE_USER_DEFAULT, 0, &m_LibraryEntry.stLastPlayed, NULL, szString, 100);
					StringCchCopy(&szString[x-1], ulNumChars, TEXT(" "));
					GetDateFormat(LOCALE_USER_DEFAULT, 0, &m_LibraryEntry.stLastPlayed, NULL, &szString[x], 100);
				}
				else
				{
					StringCchCopy(szString, ulNumChars, TEXT("---"));
				}
			}
			break;

		case FIELD_REPLAYGAIN_TRACK_GAIN:
			{
				if(m_LibraryEntry.fReplayGain_Track_Gain)
					StringCchPrintf(szString, ulNumChars, TEXT("%1.2f"), m_LibraryEntry.fReplayGain_Track_Gain);
				else
					StringCchCopy(szString, ulNumChars, TEXT("---"));
			}
			break;
		case FIELD_REPLAYGAIN_TRACK_PEAK:
			{
				if(m_LibraryEntry.fReplayGain_Track_Peak)
					StringCchPrintf(szString, ulNumChars, TEXT("%1.2f"), m_LibraryEntry.fReplayGain_Track_Peak);
				else
					StringCchCopy(szString, ulNumChars, TEXT("---"));
			}
			break;
		case FIELD_REPLAYGAIN_ALBUM_GAIN:
			{
				if(m_LibraryEntry.fReplayGain_Album_Gain)
					StringCchPrintf(szString, ulNumChars, TEXT("%1.2f"), m_LibraryEntry.fReplayGain_Album_Gain);
				else
					StringCchCopy(szString, ulNumChars, TEXT("---"));
			}
			break;
		case FIELD_REPLAYGAIN_ALBUM_PEAK:
			{
				if(m_LibraryEntry.fReplayGain_Album_Peak)
					StringCchPrintf(szString, ulNumChars, TEXT("%1.2f"), m_LibraryEntry.fReplayGain_Album_Peak);
				else
					StringCchCopy(szString, ulNumChars, TEXT("---"));
			}
			break;
		case FIELD_AVAILABILITY:
			{
				if(m_LibraryEntry.ulAvailability == AVAILABLILITY_AVAILABLE)
					StringCchCopy(szString, ulNumChars, TEXT("Available"));
				else if(m_LibraryEntry.ulAvailability == AVAILABLILITY_UNAVAILABLE)
					StringCchCopy(szString, ulNumChars, TEXT("Unavailable"));
				else
					StringCchCopy(szString, ulNumChars, TEXT("Unknown"));
			}
			break;
		case FIELD_BPM:
			{
				if(m_LibraryEntry.ulBPM < 1)
					StringCchCopy(szString, ulNumChars, TEXT("---"));
				else
					StringCchPrintf(szString, ulNumChars, TEXT("%u"), m_LibraryEntry.ulBPM);
			}
			break;
		case FIELD_ALBUMARTIST:
			{
				StringCchCopy(szString, ulNumChars, m_LibraryEntry.szAlbumArtist);
			}
			break;
		case FIELD_COMPOSER:
			{
				StringCchCopy(szString, ulNumChars, m_LibraryEntry.szComposer);
			}
			break;

		default:
			return false;
	}

	return true;
}
