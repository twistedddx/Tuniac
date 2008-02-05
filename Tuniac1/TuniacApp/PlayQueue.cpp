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
