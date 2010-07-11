// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "stdint.h"
#include <winsock.h>

class ISource
{
public:	
	// returns the URL this source was created with!
	virtual const char	*	URL(void)		 = 0;
	virtual const char	*	MimeType(void)	 = 0;
	
	virtual bool			IsSeekable(void) = 0;
	// whence == 0 == absolute; 
	// whence == 1 == relative
	virtual bool			Seek(uint64_t offset, bool relative = false) = 0;
	virtual uint64_t		Tell(void) = 0;
	virtual uint64_t		Size(void) = 0;
	
	// QUESTION: is it better to use Peek/Dismiss rather than READ - that way a Read is a peek(n)+dismiss(n) combo
	// and the input buffer can be a ring buffer and such
	
	virtual uint64_t		Read(void * buffer, uint64_t size) = 0;
	virtual uint64_t		Peek(void * buffer, uint64_t size) = 0;
};

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <shlwapi.h>
#include <math.h>

// TODO: reference additional headers your program requires here
#include <TCHAR.H>