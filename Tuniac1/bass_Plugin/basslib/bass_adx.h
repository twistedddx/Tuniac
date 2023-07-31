//Zavyalov Alexey Alexandrovich (RUS) (Завьялов Алексей Александрович)
//for more information please write on e-mail _ff_alex_ff_@mail.ru (RUS) (Все вопросы писать на e-mail _ff_alex_ff_@mail.ru)

#ifndef BASSADX_H
#define BASSADX_H

#include "bass.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BASSADXDEF
#define BASSADXDEF(f) WINAPI f
#endif

typedef struct
{
 BOOL  LoopEnabled;
 QWORD SampleStart;
 QWORD ByteStart;
 QWORD SampleEnd;
 QWORD ByteEnd;
} TADX_LoopStruct, *PADX_LoopStruct;

// ADX Loop TAG
#define BASS_TAG_ADX_LOOP 	0x12000	// ADX loop structure

// BASS_CHANNELINFO type
#define BASS_CTYPE_STREAM_ADX	0x1F000 // ADX stream type (not splitted on version 3 or 4 or 5)


HSTREAM BASSADXDEF(BASS_ADX_StreamCreateFile)(BOOL mem, const void *file, QWORD offset, QWORD length, DWORD flags);
HSTREAM BASSADXDEF(BASS_ADX_StreamCreateURL)(const char *url, DWORD offset, DWORD flags, DOWNLOADPROC *proc, void *user);
HSTREAM BASSADXDEF(BASS_ADX_StreamCreateFileUser)(DWORD system, DWORD flags, BASS_FILEPROCS *procs, void *user);

#ifdef __cplusplus
}
#endif

#endif

 