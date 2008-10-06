#pragma once
#include "iaudiosource.h"

class CTuniacMemoryFileIO :
	public IAudioFileIO
{
protected:
	HANDLE		m_hFile;
	HANDLE		m_hMappingObject;

	BYTE	*	m_pMemory;

	__int64		m_ullOffset;
	__int64		m_ullFileSize;

	bool		m_bEOF;


public:
	CTuniacMemoryFileIO(void);
	~CTuniacMemoryFileIO(void);

	bool Open(LPTSTR szFilename);

public:

	virtual void Destroy(void);

	virtual bool Read(unsigned __int64 numberofbytes, void * pData, unsigned __int64 * pBytesRead = NULL);
	virtual bool Seek(unsigned __int64 Offset, bool bFromEnd = false);

	virtual void Tell(unsigned __int64 * pCurrentOffset);
	virtual void Size(unsigned __int64 * pSize);

	virtual bool IsEOF(void);

};
