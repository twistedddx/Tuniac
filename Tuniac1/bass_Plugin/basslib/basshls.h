/*
	BASSHLS 2.4 C/C++ header file
	Copyright (c) 2015-2017 Un4seen Developments Ltd.

	See the BASSHLS.CHM file for more detailed documentation
*/

#ifndef BASSHLS_H
#define BASSHLS_H

#include "bass.h"

#if BASSVERSION!=0x204
#error conflicting BASS and BASSHLS versions
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BASSHLSDEF
#define BASSHLSDEF(f) WINAPI f
#endif

// additional sync type
#define BASS_SYNC_HLS_SEGMENT	0x10300

// additional tag type
#define BASS_TAG_HLS_EXTINF		0x14000 // segment's EXTINF tag : UTF-8 string
#define BASS_TAG_HLS_STREAMINF	0x14001 // EXT-X-STREAM-INF tag : UTF-8 string
#define BASS_TAG_HLS_DATE		0x14002 // EXT-X-PROGRAM-DATE-TIME tag : UTF-8 string

// additional BASS_StreamGetFilePosition mode
#define BASS_FILEPOS_HLS_SEGMENT	0x10000	// segment sequence number

HSTREAM BASSHLSDEF(BASS_HLS_StreamCreateFile)(BOOL mem, const void *file, QWORD offset, QWORD length, DWORD flags);
HSTREAM BASSHLSDEF(BASS_HLS_StreamCreateURL)(const char *url, DWORD flags, DOWNLOADPROC *proc, void *user);

#ifdef __cplusplus
}

#if defined(_WIN32) && !defined(NOBASSOVERLOADS)
static inline HSTREAM BASS_HLS_StreamCreateFile(BOOL mem, const WCHAR *file, QWORD offset, QWORD length, DWORD flags)
{
	return BASS_HLS_StreamCreateFile(mem, (const void*)file, offset, length, flags|BASS_UNICODE);
}

static inline HSTREAM BASS_HLS_StreamCreateURL(const WCHAR *url, DWORD flags, DOWNLOADPROC *proc, void *user)
{
	return BASS_HLS_StreamCreateURL((const char*)url, flags|BASS_UNICODE, proc, user);
}
#endif
#endif

#endif
