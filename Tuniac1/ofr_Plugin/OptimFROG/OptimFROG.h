/* Copyright (C) 1996-2016 Florin Ghido, all rights reserved. */
/* OptimFROG SDK v1.600 for OptimFROG Lossless/DualStream Audio Codec */
/* Visit http://LosslessAudio.org/ for updates and more information. */
/* @OptimFROG is also on Twitter. E-mail: florin.ghido@gmail.com */
/* SDK and Libraries are free for non-commercial and commercial use. */


/* OptimFROG.h contains a C/C++ interface to DLL/SO/DYLIB Libraries for */
/* working with OptimFROG Lossless (OFR) and DualStream (OFS) files. */


#ifndef OPTIMFROG_H_INCLUDED
#define OPTIMFROG_H_INCLUDED


#include "PortableTypes.h"


#if defined(CFG_MSCVER_WIN32_MIX86) || defined(CFG_BORLANDC_WIN32_MIX86) || defined(CFG_MSCVER_WIN64_MX64)
#pragma pack(push, 1)
#endif


typedef struct
{
    condition_t (*close)(void* instance);
    sInt32_t (*read)(void* instance, void* destBuffer, uInt32_t count);
    condition_t (*eof)(void* instance);
    condition_t (*seekable)(void* instance);
    sInt64_t (*length)(void* instance);
    sInt64_t (*getPos)(void* instance);
    condition_t (*seek)(void* instance, sInt64_t pos);
}
#if defined(CFG_ATTRIBUTE_PACKED)
__attribute__((packed))
#endif
ReadInterface;


typedef struct
{
    uInt32_t keyCount;
    char* keys[64];
    char* values[64];
}
#if defined(CFG_ATTRIBUTE_PACKED)
__attribute__((packed))
#endif
OptimFROG_Tags;


typedef struct
{
    uInt32_t channels;
    uInt32_t samplerate;
    uInt32_t bitspersample;
    uInt32_t bitrate;

    uInt32_t version;
    const char* method;
    const char* speedup;

    sInt64_t noPoints;
    sInt64_t originalSize;
    sInt64_t compressedSize;
    sInt64_t length_ms;

    const char* sampleType;
    const char* channelConfig;
}
#if defined(CFG_ATTRIBUTE_PACKED)
__attribute__((packed))
#endif
OptimFROG_Info;


#if defined(CFG_MSCVER_WIN32_MIX86) || defined(CFG_BORLANDC_WIN32_MIX86) || defined(CFG_MSCVER_WIN64_MX64)
#pragma pack(pop)
#endif


typedef void (*OptimFROG_callBack)(void* callBackParam, Float64_t percentage);


#if defined(CFG_MSCVER_WIN32_MIX86) || defined(CFG_BORLANDC_WIN32_MIX86) || defined(CFG_GNUC_MINGW32_I386) || defined(CFG_GNUC_CYGWIN_I386) || defined(CFG_MSCVER_WIN64_MX64)
#define IMPORT_SYMBOL __declspec(dllimport)
#else
#define IMPORT_SYMBOL
#endif


#if defined(__cplusplus)
extern "C"
{
#define DEFAULT_FALSE = C_FALSE
#define DEFAULT_NULL = C_NULL
#else
#define DEFAULT_FALSE
#define DEFAULT_NULL
#endif

IMPORT_SYMBOL uInt32_t OptimFROG_getVersion(void);

IMPORT_SYMBOL void* OptimFROG_createInstance(void);
IMPORT_SYMBOL void OptimFROG_destroyInstance(void* decoderInstance);

IMPORT_SYMBOL condition_t OptimFROG_openExt(void* decoderInstance, ReadInterface* rInt, void* readerInstance, condition_t readTags DEFAULT_FALSE);

IMPORT_SYMBOL condition_t OptimFROG_open(void* decoderInstance, char* fileName, condition_t readTags DEFAULT_FALSE);
IMPORT_SYMBOL condition_t OptimFROG_close(void* decoderInstance);

IMPORT_SYMBOL sInt32_t OptimFROG_readHead(void* decoderInstance, void* headData, uInt32_t maxSize);
IMPORT_SYMBOL sInt32_t OptimFROG_readTail(void* decoderInstance, void* tailData, uInt32_t maxSize);

IMPORT_SYMBOL condition_t OptimFROG_seekable(void* decoderInstance);
IMPORT_SYMBOL condition_t OptimFROG_seekPoint(void* decoderInstance, sInt64_t point);
IMPORT_SYMBOL condition_t OptimFROG_seekTime(void* decoderInstance, sInt64_t milliseconds);
IMPORT_SYMBOL sInt64_t OptimFROG_getPos(void* decoderInstance);
IMPORT_SYMBOL condition_t OptimFROG_recoverableErrors(void* decoderInstance);

IMPORT_SYMBOL sInt32_t OptimFROG_read(void* decoderInstance, void* data, uInt32_t noPoints, condition_t max16bit DEFAULT_FALSE);

IMPORT_SYMBOL condition_t OptimFROG_getInfo(void* decoderInstance, OptimFROG_Info* info);
IMPORT_SYMBOL condition_t OptimFROG_getTags(void* decoderInstance, OptimFROG_Tags* tags);
IMPORT_SYMBOL void OptimFROG_freeTags(OptimFROG_Tags* tags);

IMPORT_SYMBOL sInt32_t OptimFROG_decodeFile(char* sourceFile, char* destinationFile, OptimFROG_callBack callBack DEFAULT_NULL, void* callBackParam DEFAULT_NULL);
IMPORT_SYMBOL sInt32_t OptimFROG_infoFile(char* sourceFile, OptimFROG_Info* info, OptimFROG_Tags* tags DEFAULT_NULL);

#if defined(__cplusplus)
}
#endif


#define OptimFROG_NoError 0
#define OptimFROG_MemoryError 1
#define OptimFROG_OpenError 2
#define OptimFROG_WriteError 3
#define OptimFROG_FatalError 4
#define OptimFROG_RecoverableError 5


#endif
