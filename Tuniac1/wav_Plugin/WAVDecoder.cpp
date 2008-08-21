#include "StdAfx.h"

#include "wavdecoder.h"

bool CWAVDecoder::FOURCC_EQUAL(char* X, char* Y)
{
	if(X[0]==Y[0] && X[1]==Y[1] && X[2]==Y[2] && X[3]==Y[3])
		return true;

	return false;
}

CWAVDecoder::CWAVDecoder(void)
{
}

CWAVDecoder::~CWAVDecoder(void)
{
}

bool CWAVDecoder::Open(LPTSTR szSource)
{
    m_file = _wfopen(szSource,TEXT("rbS"));
    if (m_file)
    {
		wstat = fread(&wav,sizeof(WAV_HDR),1,m_file);

		//check for RIFF header
		if(!FOURCC_EQUAL(wav.rID, "RIFF"))
			return false;

		//check for WAVE header
		if(!FOURCC_EQUAL(wav.wID, "WAVE"))
			return false;

		//check for fmt header
		if(!FOURCC_EQUAL(wav.fId, "fmt "))
			return false;

		m_bFloatMode = 0;
		if(wav.wFormatTag == 3)
			m_bFloatMode = 1;
		else if(wav.wFormatTag!=1)
			//unsupported format
			return false;

		rmore = wav.pcm_header_len - (sizeof(WAV_HDR) - 20);
		wstat = fseek(m_file,rmore,SEEK_CUR);
		if(wstat!=0)
			//cant seek
			return false;
	}


	// read chunks until a 'data' chunk is found
	sflag = 1;
	while(sflag!=0){

		// check attempts before it gets out of hand
		if(sflag>10)
			//too many chunks
			return false;

		 // read chunk header
		wstat = fread(&chk,sizeof(CHUNK_HDR),(size_t)1,m_file);
		if(wstat!=1)
			//cant read chunk
			return false;

		// check chunk type
			if(FOURCC_EQUAL(chk.dId, "data"))
				break;
	 
		// skip over chunk
		sflag++;
		wstat = fseek(m_file,chk.dLen,SEEK_CUR);
		if(wstat!=0)
			//cant seek
			return false;
	}

	//store where we are for seeking later
	fgetpos(m_file, &fpos );

	if(wav.nBitsPerSample == 8)
	{
		m_divider = 128.0f;
	}
	else if(wav.nBitsPerSample == 12)
	{
		m_divider = 4095.0f;
	}
	else if(wav.nBitsPerSample == 16)
	{
		m_divider = 32767.0f;
	}
	else if(wav.nBitsPerSample == 20)
	{
		m_divider = 1048575.0f;
	}
	else if(wav.nBitsPerSample == 24)
	{
		m_divider = 8388608.0f;
	}
	else if(wav.nBitsPerSample == 32)
	{
		m_divider = 2147483648.0;
	}


	return(true);
}

bool CWAVDecoder::Close()
{
	return(true);
}

void		CWAVDecoder::Destroy(void)
{
	fclose(m_file);
	Close();
	delete this;
}

bool		CWAVDecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate = wav.nSamplesPerSec;
	*Channels	= wav.nChannels;

	return(true);
}

bool		CWAVDecoder::GetLength(unsigned long * MS)
{
	*MS = GetFileSize(m_file, NULL) / (unsigned long)wav.nAvgBytesPerSec;
	return(true);
}

bool		CWAVDecoder::SetPosition(unsigned long * MS)
{
	unsigned long ByteOffset = (float)(*MS / 1000) * (unsigned long)wav.nAvgBytesPerSec;

	//setpos to the start of the data block we found on open
	fsetpos(m_file, &fpos );
	//seek in to where we want
	fseek(m_file,ByteOffset,SEEK_CUR);
	return(true);
}

bool		CWAVDecoder::SetState(unsigned long State)
{
	return(true);
}

bool		CWAVDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	BYTE			buffer[BUF_SIZE];

	/* read signal data */
	wstat = fread(buffer,BUF_SIZE,1,m_file);
	if(wstat!=1)
		//cant read chunk
		return false;

	if(m_bFloatMode)
	{
		*ppBuffer = (float*)buffer;
	}
	else
	{

		short * pData = (short*)buffer;

		for(int x=0; x<(BUF_SIZE/2); x++)
		{
			m_Buffer[x] = (float)pData[x] / m_divider;
		}

		*ppBuffer	= m_Buffer;

	}
	*NumSamples = (BUF_SIZE/2);

	return(true);
}