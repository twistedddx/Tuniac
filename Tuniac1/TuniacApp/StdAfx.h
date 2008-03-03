#pragma once

#define _WIN32_IE 0x0600
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