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

#pragma pack(16)
#pragma once

#define ITUNIACINFOMANAGER_VERSION		MAKELONG(0, 3)

//24-4-2 0.4 add GetDevice GetDevices

#define UNKNOWN_DEVICE		(-1)

#include "LibraryEntry.h"

typedef struct
{
	TCHAR devicename[128];
	unsigned long samplerate;
	unsigned long channels;
	TCHAR type[128];
} InfoDevice;

class IInfoManager
{
public:
	virtual void			Destroy(void)										= 0;

	virtual unsigned long	GetNumExtensions(void)								= 0;
	virtual LPTSTR			SupportedExtension(unsigned long ulExtentionNum)	= 0;

	virtual bool			CanHandle(LPTSTR szSource)							= 0;
	virtual bool			GetInfo(LibraryEntry * libEnt)						= 0;
	virtual bool			SetInfo(LibraryEntry * libEnt)						= 0;

	virtual unsigned long	GetNumberOfAlbumArts(void)							= 0;
	virtual bool			GetAlbumArt(unsigned long		ulImageIndex,
										LPVOID			*	pImageData,
										unsigned long	*	ulImageDataSize,
										LPTSTR				szMimeType,
										unsigned long	*	ulArtType)			= 0;
	virtual bool			FreeAlbumArt(LPVOID				pImageData)			= 0;
};

typedef IInfoManager * (*CreateInfoManagerPluginFunc)(void);
extern "C" __declspec(dllexport) IInfoManager * CreateInfoManagerPlugin(void);

typedef unsigned long (*GetTuniacInfoManagerVersionFunc)(void);
extern "C" __declspec(dllexport) unsigned long GetTuniacInfoManagerVersion(void);
