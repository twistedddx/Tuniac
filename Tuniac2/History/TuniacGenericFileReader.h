#pragma once
#include "ituniacfilereader.h"

class CTuniacGenericFileReader :
	public ITuniacFileReader
{
protected:
	HANDLE			m_hFile;

public:
	CTuniacGenericFileReader(void);
	~CTuniacGenericFileReader(void);

public:
	void			Destroy(void);

	LPTSTR			GetName(void);
	GUID			GetPluginID(void);		// use guidgen to make a custom guid to return

	unsigned long	GetFlags(void);

	void			SetHelper(ITuniacPluginHelper * pHelper);

public:
	bool			CanHandle(LPTSTR szFilename, unsigned long * Merit);

	bool			Open(LPTSTR szFilename);
	bool			Close(void);

	bool			Read(LPVOID pData, unsigned long ulBytesToRead, unsigned long * ulBytesRead = NULL);
	bool			SetPosition(unsigned long ulNewPosition, unsigned long * ulOldPosition);

	bool			GetLength(unsigned long * ulLength);

};
