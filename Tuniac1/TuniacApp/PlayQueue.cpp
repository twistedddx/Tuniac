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

IPlaylistEntry *	CPlayQueue::GetItemAtIndex(unsigned long ulIndex)
{
	if (ulIndex > m_Queue.GetCount())
		return NULL;
	return m_Queue[ulIndex];
}

bool				CPlayQueue::Prepend(IPlaylistEntry * pEntry)
{
	if(pEntry)
		m_Queue.AddHead(pEntry);
	return true;
}

bool				CPlayQueue::Append(IPlaylistEntry * pEntry)
{
	if(pEntry)
		m_Queue.AddTail(pEntry);
	return true;
}

bool				CPlayQueue::Remove(int iIndex)
{
	if(m_Queue.GetCount() == 0)
		return NULL;

	m_Queue.RemoveAt(iIndex);
	return true;
}

bool				CPlayQueue::RemoveItem(IPlaylistEntry * pEntry)
{
	if(pEntry == NULL)
		return false;

	for(unsigned long i = 0; i < m_Queue.GetCount(); i++)
	{
		if(m_Queue[i] == pEntry)
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
