#ifndef STREAM_H
#define STREAM_H
#include <stdio.h>
/* stream.h */

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

typedef struct stream_tTAG stream_t;

void stream_read(stream_t *stream, size_t len, void *buf);

int32_t stream_read_int32(stream_t *stream);
uint32_t stream_read_uint32(stream_t *stream);

int16_t stream_read_int16(stream_t *stream);
uint16_t stream_read_uint16(stream_t *stream);

int8_t stream_read_int8(stream_t *stream);
uint8_t stream_read_uint8(stream_t *stream);

void stream_skip(stream_t *stream, size_t skip);

int stream_eof(stream_t *stream);

long stream_tell(stream_t *stream);
int stream_setpos(stream_t *stream, long pos);

stream_t *stream_create_file(FILE *file,
                             int bigendian);
void stream_destroy(stream_t *stream);

#endif /* STREAM_H */

