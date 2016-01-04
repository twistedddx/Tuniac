/* Copyright (C) 1996-2015 Florin Ghido, all rights reserved. */
/* This file is part of OptimFROG SDK, see OptimFROG.h for details. */


/* SystemID.h detects the currently used build configuration. */


#ifndef SYSTEMID_H_INCLUDED
#define SYSTEMID_H_INCLUDED


#if defined(_MSC_VER) && defined(_WIN32) && defined(_M_IX86)

#define CFG_BUILD_NAME "MSCVER_WIN32_MIX86"
#define CFG_MSCVER_WIN32_MIX86
#define CFG_LITTLE_ENDIAN
#pragma warning(disable : 4514) /* unreferenced inline function has been removed */
#pragma warning(disable : 4127) /* conditional expression is constant */
#pragma warning(disable : 4290) /* C++ exception specification ignored except to indicate a function is not __declspec(nothrow) */

#elif defined(__BORLANDC__) && defined(_WIN32) && defined(_M_IX86)

#define CFG_BUILD_NAME "BORLANDC_WIN32_MIX86"
#define CFG_BORLANDC_WIN32_MIX86
#define CFG_LITTLE_ENDIAN

#elif defined(__GNUC__) && defined(__MINGW32__) && defined(__i386__)

#define CFG_BUILD_NAME "GNUC_MINGW32_I386"
#define CFG_GNUC_MINGW32_I386
#define CFG_LITTLE_ENDIAN
#define CFG_ATTRIBUTE_PACKED

#elif defined(__GNUC__) && defined(__CYGWIN__) && defined(__i386__)

#define CFG_BUILD_NAME "GNUC_CYGWIN_I386"
#define CFG_GNUC_CYGWIN_I386
#define CFG_LITTLE_ENDIAN
#define CFG_ATTRIBUTE_PACKED

#elif defined(__GNUC__) && defined(__DJGPP__) && defined(__i386__)

#define CFG_BUILD_NAME "GNUC_DJGPP_I386"
#define CFG_GNUC_DJGPP_I386
#define CFG_LITTLE_ENDIAN
#define CFG_ATTRIBUTE_PACKED

#elif defined(__GNUC__) && defined(__linux__) && defined(__i386__)

#define CFG_BUILD_NAME "GNUC_LINUX_I386"
#define CFG_GNUC_LINUX_I386
#define CFG_LITTLE_ENDIAN
#define CFG_ATTRIBUTE_PACKED

#elif defined(__GNUC__) && defined(__MACH__) && defined(__ppc__)

#define CFG_BUILD_NAME "GNUC_MACH_PPC"
#define CFG_GNUC_MACH_PPC
#define CFG_BIG_ENDIAN
#define CFG_ATTRIBUTE_PACKED

#elif defined(__GNUC__) && defined(__MACH__) && defined(__i386__)

#define CFG_BUILD_NAME "GNUC_MACH_I386"
#define CFG_GNUC_MACH_I386
#define CFG_LITTLE_ENDIAN
#define CFG_ATTRIBUTE_PACKED

#elif defined(__GNUC__) && defined(__linux__) && defined(__x86_64__)

#define CFG_BUILD_NAME "GNUC_LINUX_X8664"
#define CFG_GNUC_LINUX_X8664
#define CFG_LITTLE_ENDIAN
#define CFG_ATTRIBUTE_PACKED

#elif defined(__GNUC__) && defined(__MACH__) && defined(__x86_64__)

#define CFG_BUILD_NAME "GNUC_MACH_X8664"
#define CFG_GNUC_MACH_X8664
#define CFG_LITTLE_ENDIAN
#define CFG_ATTRIBUTE_PACKED

#elif defined(_MSC_VER) && defined(_WIN64) && defined(_M_X64)

#define CFG_BUILD_NAME "MSCVER_WIN64_MX64"
#define CFG_MSCVER_WIN64_MX64
#define CFG_LITTLE_ENDIAN
#pragma warning(disable : 4514) /* unreferenced inline function has been removed */
#pragma warning(disable : 4127) /* conditional expression is constant */
#pragma warning(disable : 4290) /* C++ exception specification ignored except to indicate a function is not __declspec(nothrow) */

#elif defined(__GNUC__) && defined(__FreeBSD__) && defined(__i386__)

#define CFG_BUILD_NAME "GNUC_FREEBSD_I386"
#define CFG_GNUC_FREEBSD_I386
#define CFG_LITTLE_ENDIAN
#define CFG_ATTRIBUTE_PACKED

#elif defined(__GNUC__) && defined(__FreeBSD__) && defined(__x86_64__)

#define CFG_BUILD_NAME "GNUC_FREEBSD_X8664"
#define CFG_GNUC_FREEBSD_X8664
#define CFG_LITTLE_ENDIAN
#define CFG_ATTRIBUTE_PACKED

#else

#error build configuration not defined

#endif


#endif
