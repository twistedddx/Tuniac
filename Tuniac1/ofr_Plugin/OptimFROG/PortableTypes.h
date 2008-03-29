/* Copyright (C) 1996-2006 Florin Ghido, all rights reserved. */


/* PortableTypes.h */


#ifndef PORTABLETYPES_H_INCLUDED
#define PORTABLETYPES_H_INCLUDED


#include "SystemID.h"
#include <stddef.h>


#if defined(CFG_MSCVER_WIN32_MIX86) || defined(CFG_BORLANDC_WIN32_MIX86)

typedef signed char sInt8_t;
typedef unsigned char uInt8_t;
typedef signed short int sInt16_t;
typedef unsigned short int uInt16_t;
typedef signed int sInt32_t;
typedef unsigned int uInt32_t;
typedef signed __int64 sInt64_t;
typedef unsigned __int64 uInt64_t;
typedef unsigned char condition_t;
#define C_TRUE 1
#define C_FALSE 0
#define C_NULL NULL
typedef float Float32_t;
typedef double Float64_t;

#elif defined(CFG_GNUC_MINGW32_I386) || defined(CFG_GNUC_CYGWIN_I386) || defined(CFG_GNUC_DJGPP_I386) || defined(CFG_GNUC_LINUX_I386) || defined(CFG_GNUC_MACH_PPC)

typedef signed char sInt8_t;
typedef unsigned char uInt8_t;
typedef signed short int sInt16_t;
typedef unsigned short int uInt16_t;
typedef signed int sInt32_t;
typedef unsigned int uInt32_t;
typedef signed long long int sInt64_t;
typedef unsigned long long int uInt64_t;
typedef unsigned char condition_t;
#define C_TRUE 1
#define C_FALSE 0
#define C_NULL NULL
typedef float Float32_t;
typedef double Float64_t;

#elif defined(CFG_GNUC_LINUX_X8664)

typedef signed char sInt8_t;
typedef unsigned char uInt8_t;
typedef signed short int sInt16_t;
typedef unsigned short int uInt16_t;
typedef signed int sInt32_t;
typedef unsigned int uInt32_t;
typedef signed long int sInt64_t;
typedef unsigned long int uInt64_t;
typedef unsigned char condition_t;
#define C_TRUE 1
#define C_FALSE 0
#define C_NULL NULL
typedef float Float32_t;
typedef double Float64_t;

#else

#include "PortableTypes.inc"

#endif


#endif
