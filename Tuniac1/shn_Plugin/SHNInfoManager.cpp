#include "stdafx.h"
#include "shninfomanager.h"


struct ShnInfoUtilFileStream {
	ShnPlayStream vtbl;
	FILE * file;
};

typedef struct ShnInfoUtilFileStream ShnInfoUtilFileStream;

int ShnInfoUtilFileStream_Read(ShnPlayStream * stream, void * buffer, int bytes, int * bytes_read)
{
	ShnInfoUtilFileStream * _this;
	int nBytesRead;

	if (stream == 0 || buffer == 0) return 0;

	_this = (ShnInfoUtilFileStream *)stream;

	nBytesRead = fread(buffer, 1, bytes, _this->file);
	// was reading successful?
	if (bytes_read)
		*bytes_read = nBytesRead;

	if (nBytesRead == bytes || feof(_this->file))
		return 1;

	return 0;
}

int ShnInfoUtilFileStream_Seek(ShnPlayStream * stream, int position)
{
	ShnInfoUtilFileStream * _this;

	if (stream == 0) return 0;

	_this = (ShnInfoUtilFileStream *)stream;

	return !fseek(_this->file, position, SEEK_SET);
}

int ShnInfoUtilFileStream_CanSeek(ShnPlayStream * stream)
{
	return 1;
}

int ShnInfoUtilFileStream_GetLength(ShnPlayStream * stream)
{
	ShnInfoUtilFileStream * _this;
	int nSavedPosition;
	int nSize;

	if (stream == 0) return 0;

	_this = (ShnInfoUtilFileStream *)stream;

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

int ShnInfoUtilFileStream_GetPosition(ShnPlayStream * stream)
{
	ShnInfoUtilFileStream * _this;
	int nPosition;

	if (stream == 0) return 0;

	_this = (ShnInfoUtilFileStream *)stream;

	nPosition = ftell(_this->file);
	
	if(nPosition >= 0)
		return nPosition;

	return -1;
}


int ShnInfoUtil_OpenFileStreamA(ShnPlayStream ** pstream, const char * filename)
{
	FILE * file;
	ShnInfoUtilFileStream * _this;

	if (pstream == 0 || filename == 0) return 0;

	*pstream = 0;

	file = fopen(filename, "rb");
	if (file == 0) return 0;

	_this = (ShnInfoUtilFileStream *)malloc(sizeof(ShnInfoUtilFileStream));
	_this->file = file;
	
	_this->vtbl.Read        = ShnInfoUtilFileStream_Read;
	_this->vtbl.Seek        = ShnInfoUtilFileStream_Seek;
	_this->vtbl.CanSeek     = ShnInfoUtilFileStream_CanSeek;
	_this->vtbl.GetLength   = ShnInfoUtilFileStream_GetLength;
	_this->vtbl.GetPosition = ShnInfoUtilFileStream_GetPosition;

	*pstream = (ShnPlayStream *)_this;

	return 1;
}

void ShnInfoUtil_CloseFileStream(ShnPlayStream * stream)
{
	ShnInfoUtilFileStream * _this = (ShnInfoUtilFileStream *)stream;
	if (_this->file)
		fclose(_this->file);

	free(_this);
}



// actual class definition here

CSHNInfoManager::CSHNInfoManager(void)
{
}

CSHNInfoManager::~CSHNInfoManager(void)
{
}

void			CSHNInfoManager::Destroy(void)
{
	delete this;
}

unsigned long	CSHNInfoManager::GetNumExtensions(void)
{
	return 1;
}

LPTSTR			CSHNInfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
	static LPTSTR exts[] = 
	{
		TEXT(".shn")
	};

	return exts[ulExtentionNum];
}

bool			CSHNInfoManager::CanHandle(LPTSTR szSource)
{
	for(unsigned int x=0; x<GetNumExtensions(); x++)
	{
		if(!StrCmpI(SupportedExtension(x), PathFindExtension(szSource)))
		{
			return(true);
		}
	}

	return(false);
}

bool			CSHNInfoManager::GetInfo(LibraryEntry * libEnt)
{
	ShnPlayStream vtbl;
	ShnPlayInfo info;
	ShnPlayStream * stream;
	ShnPlay * state;

	char tempname[MAX_PATH]; 	 
	WideCharToMultiByte(CP_UTF8, 0, libEnt->szURL, -1, tempname, MAX_PATH, 0, 0);

	if (ShnInfoUtil_OpenFileStreamA(&stream, tempname))
	{
		if (ShnPlay_OpenStream(&state, stream, 0))
		{

			if (ShnPlay_GetInfo(state, &info))
			{
				StringCchCopy(libEnt->szFileType, 16, L"shn");
				libEnt->ulChannels = info.channels;
				libEnt->ulSampleRate = info.sample_rate;
				unsigned long ulLength = (__int64)info.sample_count * 1000 / info.sample_rate;
				libEnt->ulPlaybackTime		= ulLength;
				libEnt->ulBitRate = (unsigned long)(ShnInfoUtilFileStream_GetLength(stream) * 8 / (ulLength / 1000));
			}
			else
				return false;
				//ShnPlay_ErrorMessage(state)

			ShnPlay_Close(state);

		}
		else
		{
			return false;
		}
		ShnInfoUtil_CloseFileStream(stream);
	}

	return true;
}

bool			CSHNInfoManager::SetInfo(LibraryEntry * libEnt)
{
	return true;
}

unsigned long	CSHNInfoManager::GetNumberOfAlbumArts(void)
{
	return 0;
}

bool			CSHNInfoManager::GetAlbumArt(	unsigned long		ulImageIndex,
												LPVOID			*	pImageData,
												unsigned long	*	ulImageDataSize,
												LPTSTR				szMimeType,
												unsigned long	*	ulArtType)
{
	return false;
}

bool			CSHNInfoManager::FreeAlbumArt(LPVOID				pImageData)
{
	return false;
}
