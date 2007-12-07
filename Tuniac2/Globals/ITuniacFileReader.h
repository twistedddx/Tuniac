#pragma once

#include "ITuniacPlugin.h"

class ITuniacFileReader : public ITuniacPlugin
{
public:
	virtual bool CanHandle(LPTSTR szFilename, unsigned long * Merit) = 0;

	virtual bool Open(LPTSTR szFilename) = 0;
	virtual bool Close(void) = 0;

	virtual bool Read(LPVOID pData, unsigned long ulBytesToRead, unsigned long * ulBytesRead = NULL) = 0;
	virtual bool SetPosition(unsigned long ulNewPosition, unsigned long * ulOldPosition) = 0;

	virtual bool GetLength(unsigned long * ulLength) = 0;
};