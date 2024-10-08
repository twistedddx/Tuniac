/*
	BASSOPUS 2.4 C/C++ header file
	Copyright (c) 2012-2024 Un4seen Developments Ltd.

	See the BASSOPUS.CHM file for more detailed documentation
*/

#ifndef BASSOPUS_H
#define BASSOPUS_H

#include "bass.h"

#if BASSVERSION!=0x204
#error conflicting BASS and BASSOPUS versions
#endif

#ifdef __OBJC__
typedef int BOOL32;
#define BOOL BOOL32 // override objc's BOOL
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BASSOPUSDEF
#define BASSOPUSDEF(f) WINAPI f
#endif

// BASS_CHANNELINFO type
#define BASS_CTYPE_STREAM_OPUS		0x11200

// Additional attributes
#define BASS_ATTRIB_OPUS_ORIGFREQ	0x13000
#define BASS_ATTRIB_OPUS_GAIN		0x13001

typedef struct {
	BYTE version;
	BYTE channels;
	WORD preskip;
	DWORD inputrate;
	short gain;
	BYTE mapping;
	BYTE streams;
	BYTE coupled;
	BYTE chanmap[255];
} BASS_OPUS_HEAD;

#define BASS_STREAMPROC_OPUS_LOSS	0x40000000

HSTREAM BASSOPUSDEF(BASS_OPUS_StreamCreate)(const BASS_OPUS_HEAD *head, DWORD flags, STREAMPROC *proc, void *user);
HSTREAM BASSOPUSDEF(BASS_OPUS_StreamCreateFile)(BOOL mem, const void *file, QWORD offset, QWORD length, DWORD flags);
HSTREAM BASSOPUSDEF(BASS_OPUS_StreamCreateURL)(const char *url, DWORD offset, DWORD flags, DOWNLOADPROC *proc, void *user);
HSTREAM BASSOPUSDEF(BASS_OPUS_StreamCreateFileUser)(DWORD system, DWORD flags, const BASS_FILEPROCS *procs, void *user);
DWORD BASSOPUSDEF(BASS_OPUS_StreamPutData)(HSTREAM handle, const void *buffer, DWORD length);

#ifdef __cplusplus
}

#if defined(_WIN32) && !defined(NOBASSOVERLOADS)
static inline HSTREAM BASS_OPUS_StreamCreateFile(BOOL mem, const WCHAR *file, QWORD offset, QWORD length, DWORD flags)
{
	return BASS_OPUS_StreamCreateFile(mem, (const void*)file, offset, length, flags | BASS_UNICODE);
}

static inline HSTREAM BASS_OPUS_StreamCreateURL(const WCHAR *url, DWORD offset, DWORD flags, DOWNLOADPROC *proc, void *user)
{
	return BASS_OPUS_StreamCreateURL((const char*)url, offset, flags | BASS_UNICODE, proc, user);
}
#endif
#endif

#ifdef __OBJC__
#undef BOOL
#endif

#endif
