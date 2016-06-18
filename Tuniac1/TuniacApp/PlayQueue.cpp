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
#include "PlayQueue.h"

CPlayQueue::CPlayQueue(void)
{
}

CPlayQueue::~CPlayQueue(void)
{
}

unsigned long		CPlayQueue::GetCount(void)
{
	return m_Queue.GetCount();
}

unsigned long		CPlayQueue::GetEntryIDAtIndex(unsigned long ulIndex)
{
	if (ulIndex == INVALID_PLAYLIST_INDEX)
		return INVALID_PLAYLIST_INDEX;

	if (ulIndex > m_Queue.GetCount())
		return INVALID_PLAYLIST_INDEX;
	return m_Queue[ulIndex];
}

bool				CPlayQueue::Prepend(unsigned long ulEntryID)
{
	m_Queue.AddHead(ulEntryID);
	return true;
}

bool				CPlayQueue::Append(unsigned long ulEntryID)
{
	m_Queue.AddTail(ulEntryID);
	return true;
}

bool				CPlayQueue::Remove(unsigned long ulIndex)
{
	if(m_Queue.GetCount() == 0)
		return NULL;

	m_Queue.RemoveAt(ulIndex);
	return true;
}

bool				CPlayQueue::RemoveEntryID(unsigned long ulEntryID)
{
	if (ulEntryID == INVALID_PLAYLIST_INDEX)
		return false;

	for(unsigned long i = 0; i < m_Queue.GetCount(); i++)
	{
		if(m_Queue[i] == ulEntryID)
		{
			m_Queue.RemoveAt(i);
			return true;
		}
	}
	return false;
}


void				CPlayQueue::Clear(void)
{
	m_Queue.RemoveAll();
}
