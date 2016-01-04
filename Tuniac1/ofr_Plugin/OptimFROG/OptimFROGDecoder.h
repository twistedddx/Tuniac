/* Copyright (C) 1996-2015 Florin Ghido, all rights reserved. */
/* This file is part of OptimFROG SDK, see OptimFROG.h for details. */


// OptimFROGDecoder.h contains a thin C++ wrapper around the
// C-style interface provided in the OptimFROG.h file.


#ifndef OPTIMFROGDECODER_H_INCLUDED
#define OPTIMFROGDECODER_H_INCLUDED


#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "OptimFROG.h"


#if defined(CFG_MSCVER_WIN32_MIX86) || defined(CFG_BORLANDC_WIN32_MIX86) || defined(CFG_MSCVER_WIN64_MX64)
#define OptimFROGDecoder_strcasecmp stricmp
#else
// for strcasecmp, POSIX compliant
#include <strings.h>
#define OptimFROGDecoder_strcasecmp strcasecmp
#endif


class OptimFROGDecoder
{
public:

    OptimFROGDecoder()
    {
        init();
    }

    ~OptimFROGDecoder()
    {
        freeFileDetails();
        OptimFROG_destroyInstance(decoderInstance);
        decoderInstance = C_NULL;
        isOpened = C_FALSE;
    }


    condition_t openExt(ReadInterface* rInt, void* readerInstance,
                        condition_t readTags = C_FALSE, condition_t max16bit = C_FALSE)
    {
        if (decoderInstance == C_NULL)
        {
            decoderInstance = OptimFROG_createInstance();
            if (decoderInstance == C_NULL)
            {
                return C_FALSE;
            }
        }

        isOpened = OptimFROG_openExt(decoderInstance, rInt, readerInstance, readTags);
        if (!isOpened)
        {
            return C_FALSE;
        }
        isMax16bit = max16bit;

        OptimFROG_Info tInfo;
        OptimFROG_getInfo(decoderInstance, &tInfo);
        copyInfo(tInfo);
        if (readTags)
        {
            OptimFROG_Tags tTags;
            OptimFROG_getTags(decoderInstance, &tTags);
            copyTags(tTags);
            OptimFROG_freeTags(&tTags);
        }

        outputBytesPerSample = bitspersample / 8;
        if (isMax16bit && (bitspersample > 16))
        {
            outputBytesPerSample = 16 / 8;
        }

        return isOpened;
    }


    condition_t open(char* fileName, condition_t readTags = C_FALSE, condition_t max16bit = C_FALSE)
    {
        if (decoderInstance == C_NULL)
        {
            decoderInstance = OptimFROG_createInstance();
            if (decoderInstance == C_NULL)
            {
                return C_FALSE;
            }
        }

        isOpened = OptimFROG_open(decoderInstance, fileName, readTags);
        if (!isOpened)
        {
            return C_FALSE;
        }
        isMax16bit = max16bit;

        OptimFROG_Info tInfo;
        OptimFROG_getInfo(decoderInstance, &tInfo);
        copyInfo(tInfo);
        if (readTags)
        {
            OptimFROG_Tags tTags;
            OptimFROG_getTags(decoderInstance, &tTags);
            copyTags(tTags);
            OptimFROG_freeTags(&tTags);
        }

        outputBytesPerSample = bitspersample / 8;
        if (isMax16bit && (bitspersample > 16))
        {
            outputBytesPerSample = 16 / 8;
        }

        return isOpened;
    }


    condition_t close()
    {
        freeFileDetails();
        condition_t result = OptimFROG_close(decoderInstance);
        isOpened = C_FALSE;
        return result;
    }


    sInt32_t readHead(void* headData, uInt32_t maxSize)
    {
        assert(isOpened);
        return OptimFROG_readHead(decoderInstance, headData, maxSize);
    }

    sInt32_t readTail(void* tailData, uInt32_t maxSize)
    {
        assert(isOpened);
        return OptimFROG_readTail(decoderInstance, tailData, maxSize);
    }


    condition_t seekable()
    {
        assert(isOpened);
        return OptimFROG_seekable(decoderInstance);
    }

    condition_t seekPointPos(sInt64_t pointPos)
    {
        assert(isOpened);
        return OptimFROG_seekPoint(decoderInstance, pointPos);
    }

    condition_t seekTimeMillis(sInt64_t milliseconds)
    {
        assert(isOpened);
        return OptimFROG_seekTime(decoderInstance, milliseconds);
    }

    condition_t seekTimeDouble(Float64_t seconds)
    {
        assert(isOpened);
        sInt64_t pointPos = (sInt64_t) (seconds * samplerate);
        return OptimFROG_seekPoint(decoderInstance, pointPos);
    }

    condition_t seekBytePos(sInt64_t bytePos)
    {
        assert(isOpened);
        sInt64_t pointPos = bytePos / (outputBytesPerSample * channels);
        return OptimFROG_seekPoint(decoderInstance, pointPos);
    }


    sInt64_t getPointPos()
    {
        assert(isOpened);
        return OptimFROG_getPos(decoderInstance);
    }

    sInt64_t getTimeMillis()
    {
        assert(isOpened);
        sInt64_t pointPos = OptimFROG_getPos(decoderInstance);
        return (sInt64_t) (((Float64_t) pointPos / samplerate) * 1000.0);
    }

    Float64_t getTimeDouble()
    {
        assert(isOpened);
        sInt64_t pointPos = OptimFROG_getPos(decoderInstance);
        return (Float64_t) pointPos / samplerate;
    }

    sInt64_t getBytePos()
    {
        assert(isOpened);
        sInt64_t pointPos = OptimFROG_getPos(decoderInstance);
        return pointPos * (outputBytesPerSample * channels);
    }


    condition_t recoverableErrors()
    {
        assert(isOpened);
        return OptimFROG_recoverableErrors(decoderInstance);
    }


    sInt32_t readPoints(void* data, uInt32_t noPoints)
    {
        assert(isOpened);
        return OptimFROG_read(decoderInstance, data, noPoints, isMax16bit);
    }

    sInt32_t readBytes(void* data, uInt32_t noBytes)
    {
        assert(isOpened);
        uInt32_t noPoints = noBytes / (outputBytesPerSample * channels);
        sInt32_t pointsRead = OptimFROG_read(decoderInstance, data, noPoints, isMax16bit);
        if (pointsRead < 0)
        {
            return pointsRead;
        }
        return pointsRead * (outputBytesPerSample * channels);
    }


    static uInt32_t getVersion()
    {
        return OptimFROG_getVersion();
    }


    static sInt32_t decodeFile(char* sourceFile, char* destinationFile,
                               OptimFROG_callBack callBack = C_NULL, void* callBackParam = C_NULL)
    {
        return OptimFROG_decodeFile(sourceFile, destinationFile, callBack, callBackParam);
    }


    sInt32_t getFileDetails(char* sourceFile, condition_t readTags = C_FALSE)
    {
        sInt32_t result = OptimFROG_NoError;
        init();

        if (readTags)
        {
            OptimFROG_Info tInfo;
            OptimFROG_Tags tTags;
            result = OptimFROG_infoFile(sourceFile, &tInfo, &tTags);
            if (result == OptimFROG_NoError)
            {
                copyInfo(tInfo);
                copyTags(tTags);
                OptimFROG_freeTags(&tTags);
            }
        }
        else
        {
            OptimFROG_Info tInfo;
            result = OptimFROG_infoFile(sourceFile, &tInfo, C_NULL);
            if (result == OptimFROG_NoError)
            {
                copyInfo(tInfo);
            }
        }

        if (result == OptimFROG_NoError)
        {
            outputBytesPerSample = bitspersample / 8;
        }

        return result;
    }

    const char* findTag(const char* key)
    {
        for (uInt32_t i = 0; i < keyCount; ++i)
        {
            if (OptimFROGDecoder_strcasecmp(key, keys[i]) == 0)
            {
                return values[i];
            }
        }

        return C_NULL;
    }


    // OptimFROG_Info
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

    // OptimFROG_Tags
    uInt32_t keyCount;
    char* keys[64];
    char* values[64];

    // miscellaneous info
    condition_t isOpened;
    condition_t isMax16bit;
    uInt32_t outputBytesPerSample;

private:

    void copyInfo(OptimFROG_Info& info)
    {
        channels = info.channels;
        samplerate = info.samplerate;
        bitspersample = info.bitspersample;
        bitrate = info.bitrate;

        version = info.version;
        method = info.method;
        speedup = info.speedup;

        noPoints = info.noPoints;
        originalSize = info.originalSize;
        compressedSize = info.compressedSize;
        length_ms = info.length_ms;

        sampleType = info.sampleType;
        channelConfig = info.channelConfig;
    }

    void copyTags(OptimFROG_Tags& tags)
    {
        keyCount = tags.keyCount;
        for (uInt32_t i = 0; i < keyCount; ++i)
        {
            keys[i] = strdup(tags.keys[i]);
            values[i] = strdup(tags.values[i]);
        }
    }


    void init()
    {
        // OptimFROG_Info
        channels = 0;
        samplerate = 0;
        bitspersample = 0;
        bitrate = 0;

        version = 0;
        method = C_NULL;
        speedup = C_NULL;

        noPoints = 0;
        originalSize = 0;
        compressedSize = 0;
        length_ms = 0;

        sampleType = C_NULL;
        channelConfig = C_NULL;

        // OptimFROG_Tags
        keyCount = 0;
        for (uInt32_t i = 0; i < 64; ++i)
        {
            keys[i] = C_NULL;
            values[i] = C_NULL;
        }

        // miscellaneous info
        isOpened = C_FALSE;
        isMax16bit = C_FALSE;
        outputBytesPerSample = 0;

        // OptimFROG decoder handle
        decoderInstance = C_NULL;
    }

    void freeFileDetails()
    {
        for (uInt32_t i = 0; i < keyCount; ++i)
        {
            free(keys[i]);
            keys[i] = C_NULL;
            free(values[i]);
            values[i] = C_NULL;
        }
        keyCount = 0;
    }


    // OptimFROG decoder handle
    void* decoderInstance;
};


#endif
