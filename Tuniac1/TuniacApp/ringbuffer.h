// A very simple non locking one reader one writer ringbuffer  (reader and writer can be different threads)
// onus is on the reader to call canRead/canWrite before actually commiting to the write operation and polling
// using their own wait mechanism 
// This is compeltely in the public domain 

#pragma once

template <typename T>
class ringBuffer
{
protected:
	T *			m_buffer;
	uint32_t	m_buffersize;
	//uint32_t	m_buffersizebytes;		// undeeded! possibly could be used for memcpy of data
	
	uint32_t	m_writeindex;
	uint32_t	m_readindex;
	
	uint32_t	m_elementstored;
	
public:
	
	ringBuffer(uint32_t startingsize = 8192)
	{
		m_buffer = 0L;
		
		setBufSize(startingsize);
	}
	
	bool canWrite(uint32_t len)
	{
		if((m_elementstored + len) > m_buffersize)
			return false;
	
		return true;
	}
	
	bool write(T* type, uint32_t len)
	{
		for(uint32_t x=0; x<len; x++)
		{
			m_buffer[m_writeindex] = type[x];
			m_writeindex++;
			//m_writeindex = m_writeindex % (m_buffersize);
			if(m_writeindex == m_buffersize)
				m_writeindex = 0;
		}
		
		m_elementstored+=len;
		if(m_elementstored > m_buffersize)
		{
			//DLog("SHIT: m_elementstored > m_buffersize\n");
		}

		return true;
	}
	
	bool canRead(uint32_t len)
	{
		if(len > m_elementstored)
			return false;
	
		return true;
	}
	
	bool read(T* type, uint32_t len)
	{
		for(uint32_t x=0; x<len; x++)
		{
			type[x] = m_buffer[m_readindex];
			m_readindex++;
			//m_readindex = m_readindex % (m_buffersize);
			if(m_readindex == m_buffersize)
				m_readindex = 0;
		}
		m_elementstored-=len;			

		if(m_elementstored < 0)
		{
			//DLog("SHIT: m_elementstored < 0\n");
		}
		
		return true;
	}
	
	uint32_t getAvailable(void)
	{
		// returns the number of elements in the buffer!
		return m_elementstored;
	}
	
	uint32_t getFree(void)
	{
		// returns the number of elements in the buffer!
		return m_buffersize - m_elementstored;
	}
	
	
	void flush(void)
	{
		m_writeindex	= 0;		
		m_readindex		= 0;
		m_elementstored = 0;
	}
	
	void setBufSize(uint32_t size)
	{
		if(m_buffer)
		{
			delete [] m_buffer;
			m_buffer = 0L;
		}
				
		m_buffer			= new T[size+1];
		m_buffersize		= size;
		//m_buffersizebytes	= size * sizeof(T);
	}
};
