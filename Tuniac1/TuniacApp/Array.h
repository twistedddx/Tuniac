// Array.h: interface for the Array class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _ARRAY_H__
#define _ARRAY_H__

#include "new"

//static inline void *__cdecl operator new(size_t, void *_P) {return (_P); }
//static inline void __cdecl operator delete(void *, void *) {return; }


template<class TYPE, int BLOCKSIZE> 
class Array  
{
protected:
	unsigned long m_nCount;
	unsigned long m_nAllocated;
	unsigned long m_nBlockSize;
	TYPE	*m_pItems;
public:
	Array() { m_nCount = m_nAllocated = 0;
			  m_pItems = NULL;
			  m_nBlockSize = BLOCKSIZE; };

	virtual ~Array()
	{
		if( m_pItems )
		{
			VirtualFree(m_pItems, 0, MEM_RELEASE);
			m_pItems = 0;
		}
	};

	__inline TYPE& operator[]( int index )
	{
		//if(index > m_nAllocated)
			//ASSERT(0);

		return m_pItems[index];
	};

	bool Grow()
	{
		if( m_nAllocated )
		{
			{
				TYPE *temp;
				temp  = (TYPE*) VirtualAlloc(NULL, (m_nAllocated + m_nBlockSize)*sizeof(TYPE), MEM_COMMIT, PAGE_READWRITE);

				if( temp )
				{
					CopyMemory( temp, m_pItems, m_nAllocated * sizeof(TYPE) );
					VirtualFree(m_pItems, 0, MEM_RELEASE);
				}
				else 
					return false;

				m_pItems = temp;
			}
		}
		else
		{
			m_pItems =  (TYPE*) VirtualAlloc(NULL, (m_nBlockSize)*sizeof(TYPE), MEM_COMMIT, PAGE_READWRITE);
		}

		m_nAllocated += m_nBlockSize;
		return true;
	};
	
	bool RemoveTail()
	{
		if( m_nCount )
		{
			m_nCount--;
			m_pItems[m_nCount].~TYPE();
			return true;
		}
		return false;
	};

	__inline unsigned long GetCount()
	{
		return m_nCount;
	};

	int Find( TYPE& item, int nStartFrom = 0 )
	{
		for( ; nStartFrom < (int)m_nCount; nStartFrom++ )
		{
			if( m_pItems[nStartFrom] == item )
				return nStartFrom;
		}
		
		return -1;
	};

	bool InsertBefore(int index, TYPE& item )
	{
		if( m_nCount == m_nAllocated )
			if( !Grow() )
				return false;

		MoveMemory( &m_pItems[index+1], &m_pItems[index], sizeof(TYPE) * (m_nCount - index) );
		::new( (void*) &m_pItems[index] ) TYPE;
		m_pItems[index] = item;
		m_nCount++;
		return true;
	};
	
	bool InsertAfter(int index, TYPE& item )
	{
		return InsertBefore( index+1 , item );
	};

	void RemoveBlock( int start, int end )
	{
		for( int i=start;i<=end;i++ )
			m_pItems[i].~TYPE();

		MoveMemory( &m_pItems[start], &m_pItems[end+1], sizeof(TYPE) * (m_nCount - (end + 1) ) );
		m_nCount -= (end-start+1);
	};

	bool CycleToStart( int index )
	{
		if( index == 0 )
			return true;

		TYPE *temp;
		temp =  (TYPE*) VirtualAlloc(NULL, (index)*sizeof(TYPE), MEM_COMMIT, PAGE_READWRITE);
		if( temp )
		{
			CopyMemory( temp, m_pItems, (index) * sizeof(TYPE) );
			CopyMemory( m_pItems, &m_pItems[index], (m_nCount - index) * sizeof(TYPE) );
			CopyMemory( &m_pItems[m_nCount - index], temp, (index) * sizeof(TYPE) );
			VirtualFree(temp, 0, MEM_RELEASE);
			return true;
		}
		return false;

	}

	void RemoveAt( int index )
	{
		RemoveBlock( index, index );
	};

	void RemoveAll()
	{
		if( m_nCount )
			RemoveBlock( 0, m_nCount-1 );
	};


	bool AddTail( TYPE & item )
	{ 
		if( m_nCount == m_nAllocated )
			if( !Grow() )
				return false;

		// construct
		::new( (void*) &m_pItems[m_nCount] ) TYPE;
		m_pItems[m_nCount++] = item;
		return true;
	};
	
	bool AddHead( TYPE & item )
	{
		return InsertBefore( 0, item );
	};
};

#endif 
