#include "StdAfx.h"

#include "wavdecoder.h"

CWAVDecoder::CWAVDecoder(void)
{
}

CWAVDecoder::~CWAVDecoder(void)
{
}

bool CWAVDecoder::Open(LPTSTR szSource)
{
	hWav = mmioOpen( szSource, NULL, MMIO_ALLOCBUF | MMIO_READ );

	if (hWav==NULL)
	{
		Close();
		return false;
	}

	parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if (mmioDescend(hWav, &parent, NULL, MMIO_FINDRIFF))
	{
		Close();
		return false;
	}

	child.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hWav, &child,  &parent, 0))
	{
		Close();
		return false;
	}

	if (mmioRead(hWav, (char*)&wfmex, sizeof(WAVEFORMATEX)) != sizeof(WAVEFORMATEX))
	{
		Close();
		return false;
	}

	if (mmioAscend(hWav, &child, 0))
	{
		Close();
		return false;
	}

	child.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hWav, &child, &parent, MMIO_FINDCHUNK))
	{
		Close();
		return false;
	}

	ulChunkSize = child.cksize;
	ulLenthMS = 1000 * ((unsigned long)ulChunkSize / (unsigned long)wfmex.nAvgBytesPerSec);
	if(ulLenthMS < 1)
		ulLenthMS = LENGTH_UNKNOWN;

	//work out divider for unsigned integer signed: (float)(1<<wfmex.wBitsPerSample)-1;
	//m_divider = pow(2, (wfmex.wBitsPerSample-1))-1;
	m_divider = (float)(1<<(wfmex.wBitsPerSample-1))-1;

	/*
	if(wfmex.wBitsPerSample == 8)
		m_divider = 128.0f;
	else if(wfmex.wBitsPerSample == 16)
		m_divider = 32767.0f;
	else if(wfmex.wBitsPerSample == 24)
		m_divider = 8388608.0f;
	else if(wfmex.wBitsPerSample == 32)
		m_divider = 2147483648.0;
	*/


	return true;
}

bool CWAVDecoder::Close()
{
	mmioClose(hWav, 0);
	return true;
}

void		CWAVDecoder::Destroy(void)
{
	Close();
	delete this;
}

bool		CWAVDecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate = wfmex.nSamplesPerSec;
	*Channels	= wfmex.nChannels;

	return true;
}

bool		CWAVDecoder::GetLength(unsigned long * MS)
{
	*MS = ulLenthMS;
	return true;
}

bool		CWAVDecoder::SetPosition(unsigned long * MS)
{

	unsigned long ByteOffset = ((float)*MS / 1000.0f) * (unsigned long)wfmex.nAvgBytesPerSec;
	ByteOffset -= ByteOffset % wfmex.nBlockAlign;

	//seek in to where we want
	mmioSeek(hWav, parent.dwDataOffset+sizeof(FOURCC)+ByteOffset, SEEK_SET);
	return true;
}

bool		CWAVDecoder::SetState(unsigned long State)
{
	return true;
}

bool		CWAVDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	*NumSamples = 0;

	char			buffer[BUF_SIZE] = {0};
	Read = mmioRead(hWav, (char*)buffer, BUF_SIZE);
	if(Read > 0)
	{
 		short * pData = (short*)buffer;
		float * pBuffer = m_Buffer;
		for(int x=0; x<Read/2; x++)
		{
			*pBuffer = (*pData) / m_divider;	
			pData ++;
			pBuffer++;
		}

		/*
		for(int x=0; x<(Read/2); x++)
		{
			m_Buffer[x] = pData[x] / m_divider;
		}
		*/
		*ppBuffer = m_Buffer;

		*NumSamples = Read/2;

	}
	else
	{
		Close();
		return false;
	}

	return true;
}
