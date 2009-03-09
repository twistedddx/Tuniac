#include <xmmintrin.h>

void SSE_CopyFloat(float * Dest, float * Src, unsigned long len)
{
	__m128 a;
	unsigned long index = 0;
	
	while(len >= 4)
	{
		a = _mm_load_ps(&Src[index]);
		_mm_store_ps(&Dest[index], a);
		
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

void SSE_ClearFloat(float * Array, int len)
{
	__m128	m;
	int index = 0;
	
	m = _mm_setzero_ps();
	
	while(len >=4)
	{
		_mm_store_ps(&Array[index], m);
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

void SSE_AddArrayStore(float * ArrayA, float * ArrayB, float * storage, int len)
{
	__m128 a, b, c;
	int index = 0;		
	
	while(len >= 4)
	{
		a = _mm_load_ps(&ArrayA[index]);
		b = _mm_load_ps(&ArrayB[index]);
		
		c = _mm_add_ps(a, b);
		
		_mm_store_ps(&storage[index], c);
		
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

void SSE_MulArrayStore(float * ArrayA, float * ArrayB, float * storage, int len)
{
	__m128 a, b, c;
	
	int index = 0;

	while(len >= 4)
	{
		a = _mm_load_ps(&ArrayA[index]);
		b = _mm_load_ps(&ArrayB[index]);
		
		c = _mm_mul_ps(a, b);

		_mm_store_ps(&storage[index], c);
		
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