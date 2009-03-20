#pragma once

void SSE_CopyFloat(float * Src, float * Dest, unsigned long len);
void SSE_ClearFloat(float * Array, unsigned long len);

void SSE_AddArrayStore(float * ArrayA, float * ArrayB, float * storage, unsigned long len);
void SSE_MulArrayStore(float * ArrayA, float * ArrayB, float * storage, unsigned long len);
