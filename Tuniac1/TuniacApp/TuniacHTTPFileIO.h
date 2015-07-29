#pragma once
#include "iaudiosource.h"

#include <cstdint>
#include "ringbuffer.h"
#include "tuniacthread.h"

class CTuniacHTTPFileIO :
	public IAudioFileIO
{
protected:
	bool			m_ThreadRun;
	bool			m_ThreadEnded;
	CTuniacThread	m_streamThread;
	
	int				m_Socket;
	
	char			Address[2048];
	char			Port[8];
	uint16_t		uPort;
	char			Request[2048];
	
	char			contenttype[1024];
	
	bool			m_bMetaData;
	uint32_t		m_DataInterval;
	
	class ringBuffer<uint8_t>		m_rbuf;

	bool			Close(void);

	static int		threadstub(void * pData);
	int				thread(void);

public:
	CTuniacHTTPFileIO(void);
	~CTuniacHTTPFileIO(void);

	bool Open(LPTSTR szFilename);

public:

	virtual void Destroy(void);

	virtual bool Read(unsigned __int64 numberofbytes, void * pData, unsigned __int64 * pBytesRead = NULL);
	virtual bool Seek(unsigned __int64 Offset, bool bFromEnd = false);

	virtual void Tell(unsigned __int64 * pCurrentOffset);
	virtual void Size(unsigned __int64 * pSize);

	virtual bool IsEOF(void);

};
