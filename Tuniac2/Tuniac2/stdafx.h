// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <Commdlg.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shobjidl.h>

// TODO: reference additional headers your program requires here
#include <string>
#include <iostream>
#include <stdexcept>
#include <map>
#include <vector>
using namespace std;

#include "StdString.h"


typedef CStdStringW				String;	
typedef std::vector<String>		StringArray;
typedef std::vector<int>		IntArray;

#include "resource.h"
#include "TuniacApp.h"
extern CTuniacApp		g_tuniacApp;
