#pragma once

void SSE_CopyFloat(float * Src, float * Dest, int len);
void SSE_ClearFloat(float * Array, int len);

void SSE_AddArrayStore(float * ArrayA, float * ArrayB, float * storage, int len);
void SSE_MulArrayStore(float * ArrayA, float * ArrayB, float * storage, int len);
