/*
 *  synthesisfilter.h
 *  audioenginetest
 *
 *  Created by Tony Million on 02/12/2009.
 *  Copyright 2009 Tony Million. All rights reserved.
 *
 */

#pragma once

class synthesisfilter
{
protected:
	float A16[16][16], A8[8][8];       /* DCT matrix         */
	float G16[16][16], G8[8][8];       /* Output butterfly   */
	float H16[16][16], H8[8][8];       /* Scaling            */
	float B16[16][16], B8[8][8];       /* B = G * DCT * H    */
	
	float DTable[512];
	
	float V[1024];
	int Vpointer;
	
	static const float D[512];
	
	void MultiplyMatrix16(float in1[16][16], float in2[16][16], float out[16][16])
	{
		int i,j,z;
		
		for(i = 0; i < 16; i++) 
		{
			for(j = 0; j < 16; j++) 
			{
				out[i][j] = 0.0;
				for(z = 0; z < 16; z++)
					out[i][j] += in1[i][z] * in2[z][j];
			}
		}
	}
	
	void MultiplyMatrix8(float in1[8][8], float in2[8][8], float out[8][8])
	{
		int i,j,z;
		
		for(i = 0; i < 8; i++) 
		{
			for(j = 0; j < 8; j++) 
			{
				out[i][j] = 0.0;
				for(z = 0; z < 8; z++)
					out[i][j] += in1[i][z] * in2[z][j];
			}
		}
	}
	
	void IDCT(float *in, float *out);
	void Window(float *S, int step);
	
	
public:
	void Reset(void);
	synthesisfilter();
	bool PerformSynthesis(float *InputFreq, float *ToHere, int step);
};
