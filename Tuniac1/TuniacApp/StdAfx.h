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
#pragma pack(16)
#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>

//#include <tmschema.h>

#include <Commdlg.h>
#include <commctrl.h>

// windows shell stuff
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shobjidl.h>

// other windows stuff
#include <dbt.h>

#include <mmsystem.h>
#include <mmreg.h>
#include <ks.h>
#include <ksmedia.h>
#include <xaudio2.h>

#include <stdio.h>
#include <memory.h>

//#include <math.h>
//#include <winsock2.h>

#include "Array.h"
#include "TuniacApp.h"
#include "Tuniac-Meta.h"
#include "Random.h"

extern CTuniacApp		tuniacApp;
extern CRandom			g_Rand;

// utility macros
#define TESTFLAG(dwField, dwBits)   (((dwField)&(dwBits)) ? TRUE : FALSE)

#define RECTWIDTH(rc)               ((rc).right - (rc).left)
#define RECTHEIGHT(rc)              ((rc).bottom - (rc).top)