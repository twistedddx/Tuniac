#ifndef __MEDIASCIENCE_TRANSFORM_H__
#define __MEDIASCIENCE_TRANSFORM_H__

#define BLOCKSIZE 512

#include <math.h>

typedef struct Complex
{
	float r;
	float i;
}Complex;


short RepositionTable[BLOCKSIZE];

void CalcRepositionTable()
{	
	
	int k;
	int i;
	int j = 0;
    for(i=1;i <= BLOCKSIZE-2 ; i++) 
	{
		k = BLOCKSIZE/2;
		while( k <= j ) 
		{
			j = j-k;
			k = k/2;
		}
		j = j+k;
		RepositionTable[i] = j;
	}
	RepositionTable[0] = 0;
	RepositionTable[BLOCKSIZE-1] = BLOCKSIZE-1;
}

void __inline FastFFT(Complex *Result)
{
	int i, j, k;
	float Phase;
	Complex CurrentPhase, DeltaPhase, TempValue;

    for(i = 4; i < BLOCKSIZE; i<<=1)
	{
		CurrentPhase.r = 1.0f;
		CurrentPhase.i = 0.0f;

		Phase = -3.14159265f / i;

		DeltaPhase.r = sin(Phase);
		DeltaPhase.i = cos(Phase);

		for(j = 0; j < i; j++)
		{
			for(k = j; k < BLOCKSIZE; k += 2*i)
			{
				TempValue.r = CurrentPhase.r*Result[k+i].r - CurrentPhase.i*Result[k+i].i;
				TempValue.i = CurrentPhase.r*Result[k+i].i + CurrentPhase.i*Result[k+i].r;

				Result[k+i].r = Result[k].r - TempValue.r;
				Result[k+i].i =  Result[k].i - TempValue.i;

				Result[k].r += TempValue.r;
				Result[k].i += TempValue.i;
			}
			
			TempValue.r = CurrentPhase.r*DeltaPhase.r - CurrentPhase.i*DeltaPhase.i;
			TempValue.i = CurrentPhase.r*DeltaPhase.i + CurrentPhase.i*DeltaPhase.r;

			CurrentPhase.r = TempValue.r;
			CurrentPhase.i = TempValue.i;
		}
	}
}

void __inline FastFullFFT(Complex *Result, Complex *Samples)
{
	int i,j;
	
	for(i = 0; i < BLOCKSIZE; i++)
	{
		j = RepositionTable[i];
		Result[j].r = Samples[i].r;
		Result[j].i = 0.0f;
	}
	FastFFT( Result );
}


void __inline FastFFT_M8(unsigned char* pDest, signed char *pSamples)
{
	static	Complex Result[BLOCKSIZE];
	int		i;

	short	*pRT		= RepositionTable;
	Complex	*pResult	= Result;

	for(i = 0; i < (BLOCKSIZE>>3); i++)
	{
		Result[pRT[0]].r = ((float)pSamples[0]) * 0.0625f;
		Result[pRT[0]].i = 0.0f;
		Result[pRT[1]].r = ((float)pSamples[1]) * 0.0625f;
		Result[pRT[1]].i = 0.0f;
		Result[pRT[2]].r = ((float)pSamples[2]) * 0.0625f;
		Result[pRT[2]].i = 0.0f;
		Result[pRT[3]].r = ((float)pSamples[3]) * 0.0625f;
		Result[pRT[3]].i = 0.0f;
		Result[pRT[4]].r = ((float)pSamples[4]) * 0.0625f;
		Result[pRT[4]].i = 0.0f;
		Result[pRT[5]].r = ((float)pSamples[5]) * 0.0625f;
		Result[pRT[5]].i = 0.0f;
		Result[pRT[6]].r = ((float)pSamples[6]) * 0.0625f;
		Result[pRT[6]].i = 0.0f;
		Result[pRT[7]].r = ((float)pSamples[7]) * 0.0625f;
		Result[pRT[7]].i = 0.0f;

		pRT			+= 8;
		pSamples	+= 8;
	}
	FastFFT( Result );

	long tempbuffer;
	for(i = 0; i < (BLOCKSIZE>>3); i++)
	{
		tempbuffer = pResult[0].i;
		if( tempbuffer < 0 )
			tempbuffer = -tempbuffer;
		pDest[0]  = tempbuffer;

		tempbuffer = pResult[1].i;
		if( tempbuffer < 0 )
			tempbuffer = -tempbuffer;
		pDest[1]  = tempbuffer;

		tempbuffer = pResult[2].i;
		if( tempbuffer < 0 )
			tempbuffer = -tempbuffer;
		pDest[2]  = tempbuffer;

		tempbuffer = pResult[3].i;
		if( tempbuffer < 0 )
			tempbuffer = -tempbuffer;
		pDest[3]  = tempbuffer;

		
		pDest	+= 4;
		pResult	+= 4;
	}
}

#endif