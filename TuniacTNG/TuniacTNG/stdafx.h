// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
#include <commctrl.h>
#include <Commdlg.h>

#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shobjidl.h>




#include <map>
#include <vector>

#include "stdstring.h"




typedef CStdStringW						String;	

typedef std::vector<String>				StringArray;

typedef std::vector<long>				IntArray;
typedef std::vector<__int64>			Int64Array;

typedef std::vector<unsigned long>		UnsignedIntArray;
typedef std::vector<unsigned __int64>	UnsignedInt64Array;
