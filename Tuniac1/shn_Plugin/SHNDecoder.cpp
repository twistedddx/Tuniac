#include "StdAfx.h"
#include "shndecoder.h"


struct ShnPlayUtilFileStream {
	ShnPlayStream vtbl;
	FILE * file;
};

typedef struct ShnPlayUtilFileStream ShnPlayUtilFileStream;

int ShnPlayUtilFileStream_Read(ShnPlayStream * stream, void * buffer, int bytes, int * bytes_read)
{
	ShnPlayUtilFileStream * _this;
	int nBytesRead;

	if (stream == 0 || buffer == 0) return 0;

	_this = (ShnPlayUtilFileStream *)stream;

	nBytesRead = fread(buffer, 1, bytes, _this->file);
	// was reading successful?
	if (bytes_read)
		*bytes_read = nBytesRead;

	if (nBytesRead == bytes || feof(_this->file))
		return 1;

	return 0;
}

int ShnPlayUtilFileStream_Seek(ShnPlayStream * stream, int position)
{
	ShnPlayUtilFileStream * _this;

	if (stream == 0) return 0;

	_this = (ShnPlayUtilFileStream *)stream;

	return !fseek(_this->file, position, SEEK_SET);
}

int ShnPlayUtilFileStream_CanSeek(ShnPlayStream * stream)
{
	return 1;
}

int ShnPlayUtilFileStream_GetLength(ShnPlayStream * stream)
{
	ShnPlayUtilFileStream * _this;
	int nSavedPosition;
	int nSize;

	if (stream == 0) return 0;

	_this = (ShnPlayUtilFileStream *)stream;

	// save current position
	nSavedPosition = ftell(_this->file);
	// move to the end of file
	fseek(_this->file, 0, SEEK_END);
	// get the size
	nSize = ftell(_this->file);
	// restore previous position
	fseek(_this->file, nSavedPosition, SEEK_SET);
	// return size
	return nSize;
}

int ShnPlayUtilFileStream_GetPosition(ShnPlayStream * stream)
{
	ShnPlayUtilFileStream * _this;
	int nPosition;

	if (stream == 0) return 0;

	_this = (ShnPlayUtilFileStream *)stream;

	nPosition = ftell(_this->file);
	
	if(nPosition >= 0)
		return nPosition;

	return -1;
}


int ShnPlayUtil_OpenFileStreamA(ShnPlayStream ** pstream, const char * filename)
{
	FILE * file;
	ShnPlayUtilFileStream * _this;

	if (pstream == 0 || filename == 0) return 0;

	*pstream = 0;

	file = fopen(filename, "rb");
	if (file == 0) return 0;

	_this = (ShnPlayUtilFileStream *)malloc(sizeof(ShnPlayUtilFileStream));
	_this->file = file;
	
	_this->vtbl.Read        = ShnPlayUtilFileStream_Read;
	_this->vtbl.Seek        = ShnPlayUtilFileStream_Seek;
	_this->vtbl.CanSeek     = ShnPlayUtilFileStream_CanSeek;
	_this->vtbl.GetLength   = ShnPlayUtilFileStream_GetLength;
	_this->vtbl.GetPosition = ShnPlayUtilFileStream_GetPosition;

	*pstream = (ShnPlayStream *)_this;

	return 1;
}

void ShnPlayUtil_CloseFileStream(ShnPlayStream * stream)
{
	ShnPlayUtilFileStream * _this = (ShnPlayUtilFileStream *)stream;
	if (_this->file)
		fclose(_this->file);

	free(_this);
}

CshnDecoder::CshnDecoder(void)
{
}

CshnDecoder::~CshnDecoder(void)
{
}

bool CshnDecoder::Open(LPTSTR szSource)
{
	char tempname[_MAX_PATH]; 	 
	WideCharToMultiByte(CP_UTF8, 0, szSource, -1, tempname, _MAX_PATH, 0, 0);

	if (ShnPlayUtil_OpenFileStreamA(&stream, tempname))
	{
		if (ShnPlay_OpenStream(&state, stream, 0))
		{

			if (ShnPlay_GetInfo(state, &info))
			{
				buffer = new char [1024*(info.bits_per_sample/8)*info.channels];
				m_Buffer = new float [1024*(info.bits_per_sample/8)*info.channels];
				ulLength = (__int64)info.sample_count * 1000 / info.sample_rate;
				ulByterate = (int)(ShnPlayUtilFileStream_GetLength(stream) / (ulLength / 1000));
			}
			else
				return false;
				//ShnPlay_ErrorMessage(state)
		}
		else
		{
			return false;
		}
	}

	//info.bits_per_sample
	return(true);
}

bool CshnDecoder::Close()
{
	delete [] buffer;
	buffer = NULL;
	delete [] m_Buffer;
	m_Buffer = NULL;
	ShnPlay_Close(state);
	ShnPlayUtil_CloseFileStream(stream);
	return(true);
}

void		CshnDecoder::Destroy(void)
{
	Close();
	delete this;
}

bool		CshnDecoder::GetFormat(unsigned long * SampleRate, unsigned long * Channels)
{
	*SampleRate = info.sample_rate;
	*Channels	= info.channels;

	return(true);
}

bool		CshnDecoder::GetLength(unsigned long * MS)
{
	*MS = ulLength;
	return true;
}

bool		CshnDecoder::SetPosition(unsigned long * MS)
{

//	unsigned long ByteOffset = ((float)*MS / 1000.0f) * ((unsigned long)ulByterate);
//	ByteOffset -= ByteOffset % info.block_align;

//	ShnPlayUtilFileStream_Seek(stream, ByteOffset);

	return false;
}

bool		CshnDecoder::SetState(unsigned long State)
{
	return(true);
}

bool		CshnDecoder::GetBuffer(float ** ppBuffer, unsigned long * NumSamples)
{
	*NumSamples =0;

	int samples_read = 0;
	int rv = ShnPlay_Read(state, buffer, 1024, &samples_read);

	if (rv)
	{
		short	* pData		= (short *)buffer;
		float	* pBuffer	= m_Buffer;

		for(unsigned long x=0; x<samples_read*info.channels;x++)
		{
			*pBuffer = (*pData) / 32767.0f;
			pData++;
			pBuffer++;
		}
		*ppBuffer = m_Buffer;

		*NumSamples = samples_read*info.channels;
	}
	else
	{
		return false;
		//ShnPlay_ErrorMessage(state)
	}

	return(true);
}