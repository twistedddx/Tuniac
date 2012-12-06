/*
	Copyright (C) 2003-2008 Tony Million

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation is required.

	2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.

	3. This notice may not be removed or altered from any source distribution.
*/
/*
	Modification and addition to Tuniac originally written by Tony Million
	Copyright (C) 2003-2012 Brett Hoyle
*/

#pragma once

class CRandom
{
protected:
  #if 0
    // define constants for MT11213A:
    // (long constants cannot be defined as enum in 16-bit compilers)
    #define MERS_N   351
    #define MERS_M   175
    #define MERS_R   19
    #define MERS_U   11
    #define MERS_S   7
    #define MERS_T   15
    #define MERS_L   17
    #define MERS_A   0xE4BD75F5
    #define MERS_B   0x655E5280
    #define MERS_C   0xFFD58000
  #else    
    // or constants for MT19937:
    #define MERS_N   624
    #define MERS_M   397
    #define MERS_R   31
    #define MERS_U   11
    #define MERS_S   7
    #define MERS_T   15
    #define MERS_L   18
    #define MERS_A   0x9908B0DF
    #define MERS_B   0x9D2C5680
    #define MERS_C   0xEFC60000
  #endif
private:
	unsigned long mt[MERS_N];            // state vector
	int mti;                             // index into mt

public:
	CRandom(void)	
	{
		RandomInit(GetTickCount());
	}

	~CRandom(void)
	{
	}

	void RandomInit(long int seed) 
	{
		// re-seed generator
		unsigned long s = (unsigned long)seed;
		for (mti = 0; mti < MERS_N; mti++) 
		{
			s = s * 29943829 - 1;
			mt[mti] = s;
		}
	}

	unsigned long BRandom() 
	{
		// generate 32 random bits
		unsigned long y;

		if (mti >= MERS_N) 
		{
			// generate MERS_N words at one time
			const unsigned long LOWER_MASK = (1LU << MERS_R) - 1; // lower MERS_R bits
			const unsigned long UPPER_MASK = -1L  << MERS_R;      // upper (32 - MERS_R) bits
			int kk, km;
			for (kk=0, km=MERS_M; kk < MERS_N-1; kk++) 
			{
				y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
				mt[kk] = mt[km] ^ (y >> 1) ^ (-(signed long)(y & 1) & MERS_A);
				if (++km >= MERS_N) 
					km = 0;
			}

			y = (mt[MERS_N-1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
			mt[MERS_N-1] = mt[MERS_M-1] ^ (y >> 1) ^ (-(signed long)(y & 1) & MERS_A);
			mti = 0;}

			y = mt[mti++];

			// Tempering (May be omitted):
			y ^=  y >> MERS_U;
			y ^= (y << MERS_S) & MERS_B;
			y ^= (y << MERS_T) & MERS_C;
			y ^=  y >> MERS_L;

			return y;
	}

  
	double Random() 
	{
		// output random float number in the interval 0 <= x < 1
		union {double f; unsigned long i[2];} convert;
		unsigned long r = BRandom(); // get 32 random bits
		// The fastest way to convert random bits to floating point is as follows:
		// Set the binary exponent of a floating point number to 1+bias and set
		// the mantissa to random bits. This will give a random number in the 
		// interval [1,2). Then subtract 1.0 to get a random number in the interval
		// [0,1). This procedure requires that we know how floating point numbers
		// are stored. The storing method is tested in function RandomInit and saved 
		// in the variable Architecture. (A PC running Windows or Linux uses 
		// LITTLE_ENDIAN architecture).

		convert.i[0] =  r << 20;
		convert.i[1] = (r >> 12) | 0x3FF00000;
		return convert.f - 1.0;

	}
  
	long IRandom(long min, long max) 
	{
		// output random integer in the interval min <= x <= max
		long r;

		r = long((max - min + 1) * Random()) + min; // multiply interval with random and truncate

		if (r > max) 
			r = max;

		if (max < min) 
			return 0x80000000;
		return r;
	}
};
