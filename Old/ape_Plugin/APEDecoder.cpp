#include "StdAfx.h"
#include "APEdecoder.h"


CAPEDecoder::CAPEDecoder(void) :
	pRawData(NULL)
{
}

CAPEDecoder::~CAPEDecoder(void)
{
}

bool CAPEDecoder::Open(LPTSTR szSource)
{
	int nRetVal;
	MACDecompressor = CreateIAPEDecompress(szSource, &nRetVal);
	if (nRetVal != ERROR_SUCCESS)
	{
		return false;
	}
	nChannels		= MACDecompressor->GetInfo(APE_INFO_CHANNELS);
	nBitsPerSample	= MACDecompressor->GetInfo(APE_INFO_BITS_PER_SAMPLE);
	nBytesPerSample	= MACDecompressor->GetInfo(APE_INFO_BYTES_PER_SAMPLE);
	nSampleRate		= MACDecompressor->GetInfo(APE_INFO_SAMPLE_RATE);
	nBlockAlign		= MACDecompressor->GetInfo(APE_INFO_BLOCK_ALIGN);
	
	pRawData = new char [4096 * nBlockAlign];
	m_Buffer = new float [4096 * nBlockAlign];

	return true;
}

void		CAPEDecoder::Destroy(void)
{
	delete MACDecompressor;

	if(pRawData)
	{
		delete [] pRawData;
		pRawData = NULL;
	}
	if(m_Buffer)
	{
		delete [] m_Buffer;
		m_Buffer = NULL;
	}
	delete this;
}

bool		CAPEDecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate = (unsigned long)nSampleRate;
	*Channels	= (unsigned long)nChannels;

	return true;
}

bool		CAPEDecoder::GetLength(unsigned long * MS)
{
	*MS = (unsigned long)MACDecompressor->GetInfo(APE_DECOMPRESS_LENGTH_MS);
	return true;
}

bool		CAPEDecoder::SetPosition(unsigned long * MS)
{
	int seekBlocks, totalBlocks, result;

	seekBlocks = ((unsigned long)*MS/1000) * nSampleRate;
	totalBlocks = MACDecompressor->GetInfo(APE_DECOMPRESS_TOTAL_BLOCKS) - 1024;

	if (seekBlocks > totalBlocks)
		seekBlocks = totalBlocks;
	else if (seekBlocks <= 0)
		seekBlocks = 0;

	result = MACDecompressor->Seek(seekBlocks);
	if( result != ERROR_SUCCESS )
		return false;


	return true;
}

bool		CAPEDecoder::SetState(unsigned long State)
{
	return true;
}

bool		CAPEDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	result = MACDecompressor->GetData (pRawData, 4096, &nBlocksRetrieved);
	if((nBlocksRetrieved == 0) || (result != ERROR_SUCCESS))
		return false;

	if(nBytesPerSample == 2)  //16bit
	{
		short * pData = (short *)pRawData;
		float * pBuffer = m_Buffer;
		
		for(int x=0; x<nBlocksRetrieved*nChannels; x++)
		{
			*pBuffer = (*pData) / 32767.0f;	
			pData ++;
			pBuffer++;
		}
	}
	else if (nBytesPerSample == 3) //24bit
	{
		char * pData = (char *)pRawData;
		float *pBuffer = m_Buffer;
		int sourceIndex = 0;
		int padding = nBlockAlign - nChannels * (nBitsPerSample >> 3);
		int32 workingValue;

		for ( unsigned int i = 0; i < 1024; i++ ) {
			for ( unsigned int j = 0; j < nChannels; j++ ) {
				workingValue = 0;
				workingValue |= ((int32)pRawData[sourceIndex++]) << 8 & 0xFF00;
				workingValue |= ((int32)pRawData[sourceIndex++]) << 16 & 0xFF0000;
				workingValue |= ((int32)pRawData[sourceIndex++]) << 24 & 0xFF000000;

				*pBuffer = ((float)workingValue) * QUANTFACTOR;

				pBuffer ++;
				pData += 3;
			}
			sourceIndex += padding;
		}
	}
	else if(nBytesPerSample == 4)  //32bit?
	{
		int * pData = (int *)pRawData;
		float * pBuffer = m_Buffer;
		
		float divider = (float)((1<<nBitsPerSample)-1);
		
		for(int x=0; x<nBlocksRetrieved*nChannels;x++)
		{
			*pBuffer = (*pData) / divider;	
			pData ++;
			pBuffer++;
		}
	}

	*ppBuffer = m_Buffer;
	*NumSamples = nBlocksRetrieved * nChannels;
	return true;
}
