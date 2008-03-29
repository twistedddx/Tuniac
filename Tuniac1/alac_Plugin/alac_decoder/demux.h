#ifndef DEMUX_H
#define DEMUX_H

#if defined(_WIN32) && !defined(__MINGW32__)
typedef unsigned __int64 uint64_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int8 uint8_t;
typedef __int64 int64_t;
typedef __int32 int32_t;
typedef __int16 int16_t;
typedef __int8  int8_t;
typedef float float32_t;
#else
#include <inttypes.h>
#endif
#include "stream.h"

typedef uint32_t fourcc_t;

typedef struct
{
    int format_read;

    uint16_t num_channels;
    uint16_t sample_size;
    uint32_t sample_rate;
    fourcc_t format;
    void *buf;

    struct {
        uint32_t sample_count;
        uint32_t sample_duration;
    } *time_to_sample;
    uint32_t num_time_to_samples;

    uint32_t *sample_byte_size;
    uint32_t num_sample_byte_sizes;

    uint32_t codecdata_len;
    void *codecdata;

    uint32_t mdat_len;
#if 0
    void *mdat;
#endif
} demux_res_t;

int qtmovie_read(stream_t *stream, demux_res_t *demux_res);

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) ( \
    ( (int32_t)(char)(ch0) << 24 ) | \
    ( (int32_t)(char)(ch1) << 16 ) | \
    ( (int32_t)(char)(ch2) << 8 ) | \
    ( (int32_t)(char)(ch3) ) )
#endif

#ifndef SLPITFOURCC
/* splits it into ch0, ch1, ch2, ch3 - use for printf's */
#define SPLITFOURCC(code) \
    (char)((int32_t)code >> 24), \
    (char)((int32_t)code >> 16), \
    (char)((int32_t)code >> 8), \
    (char)code
#endif

#endif /* DEMUX_H */

