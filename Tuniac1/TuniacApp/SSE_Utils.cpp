#include <xmmintrin.h>

void SSE_CopyFloat(float * Dest, float * Src, unsigned long len)
{
	__m128 XMM0;
	unsigned long index = 0;
	
	while(len >= 4)
	{
		XMM0 = _mm_loadu_ps(&Src[index]);
		_mm_storeu_ps(&Dest[index], XMM0);
		
		// increment pointers;
		index		+= 4;
		len			-= 4;
	}
	while(len)
	{
		Src[index] = Dest[index];			
		index++;
		len--;
	}
}

void SSE_ClearFloat(float * Array, unsigned long len)
{
	__m128 XMM0;
	int index = 0;
	
	XMM0 = _mm_setzero_ps();
	
	while(len >=4)
	{
		_mm_storeu_ps(&Array[index], XMM0);
		index += 4;
		len -= 4;
	}
	
	while(len)
	{
		Array[index] = 0.0f;
		index++;
		len--;
	}
}

void SSE_AddArrayStore(float * ArrayA, float * ArrayB, float * storage, unsigned long len)
{
	__m128 XMM0, XMM1, XMM2;
	int index = 0;		
	
	while(len >= 4)
	{
		XMM0 = _mm_loadu_ps(&ArrayA[index]);
		XMM1 = _mm_loadu_ps(&ArrayB[index]);
		
		XMM2 = _mm_add_ps(XMM0, XMM1);
		
		_mm_storeu_ps(&storage[index], XMM2);
		
		// increment pointers;
		index+=4;			
		len -= 4;
	}
	
	while(len)
	{
		storage[index] = (ArrayA[index]) + (ArrayB[index]);
		
		index++;
		len--;
	}
	
	
/*	
	int i;
	
	for(i=0; i<len; i++)
	{
		storage[i] = ArrayA[i] + ArrayB[i];
	}	
 */
}

void SSE_MulArrayStore(float * ArrayA, float * ArrayB, float * storage, unsigned long len)
{
	__m128 XMM0, XMM1, XMM2;
	
	int index = 0;

	while(len >= 4)
	{
		XMM0 = _mm_loadu_ps(&ArrayA[index]);
		XMM1 = _mm_loadu_ps(&ArrayB[index]);
		
		XMM2 = _mm_mul_ps(XMM0, XMM1);

		_mm_storeu_ps(&storage[index], XMM2);
		
		// increment pointers;
		index += 4;
		len -= 4;
	}
	
	while(len)
	{
		storage[index] = (ArrayA[index]) * (ArrayB[index]);
		
		index++;
		len--;
	}
	
	/*
	for(i=0; i<len; i++)
	{
		storage[i] = ArrayA[i] * ArrayB[i];
	}
	 */
}