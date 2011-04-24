/*
 *  generaldefs.h
 *
 *  Created by Tony Million on 22/09/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

// this is a bit of a general purpose playing field leveler 
// some things are defined for unix and a lot of things are defined for windows

#pragma once

//#define DEBUG


#include <map>
#include <vector>
#include <string>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "stdint.h"

#include <memory.h>

#include <fcntl.h>
#include <sys/stat.h>

typedef	std::string							String;


typedef std::vector<String>					StringArray;
typedef std::vector<String>::iterator		StringIterator;

typedef std::vector<int32_t>				IntArray;
typedef std::vector<int64_t>				Int64Array;

typedef std::vector<uint32_t>				UnsignedIntArray;
typedef std::vector<uint64_t>				UnsignedInt64Array;

#ifdef DEBUG

#define DEBUG_DESTROY

#ifdef _MSC_VER

#define DLog(...) { char tstr[4096]; sprintf(tstr, __VA_ARGS__); OutputDebugStringA(tstr); }

#else
	#define DLog(...) fprintf(stderr, __VA_ARGS__);
#endif

#else

#define DLog(...)
#endif

// Some platform specific stuff goes here


#ifdef _MSC_VER

// extra windows includes for general file and socket stuff
#include <windows.h>
#include <winsock.h>

#include <string.h>
#include <io.h>
#include <share.h>

// these aren't defined - thanks microsluts
// TODO: make a generic tuniac header file which sorts all this mess out please!
#define ssize_t SSIZE_T
#define strncasecmp _strnicmp
#define strcasecmp _stricmp

#else


#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define closesocket(s) close(s);
#define Sleep(ms) usleep(ms*1000);

#endif

/// define min and max here if they're not already thanks
#ifndef NOMINMAX
#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif
#endif  /* NOMINMAX */




