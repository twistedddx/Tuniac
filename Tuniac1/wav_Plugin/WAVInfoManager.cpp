#include "stdafx.h"
#include "wavinfomanager.h"

// actual class definition here

CWAVInfoManager::CWAVInfoManager(void)
{
}

CWAVInfoManager::~CWAVInfoManager(void)
{
}

void			CWAVInfoManager::Destroy(void)
{
	delete this;
}

unsigned long	CWAVInfoManager::GetNumExtensions(void)
{
	return 1;
}

LPTSTR			CWAVInfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
	static LPTSTR exts[] = 
	{
		TEXT(".wav")
	};

	return exts[ulExtentionNum];
}

bool			CWAVInfoManager::CanHandle(LPTSTR szSource)
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

bool			CWAVInfoManager::GetInfo(LibraryEntry * libEnt)
{
	HMMIO hWav;
	WAVEFORMATEX wfmex;
	MMCKINFO child, parent;

	unsigned long ulChunkSize;
	unsigned long ulLenthMS;

	hWav = mmioOpen( libEnt->szURL, NULL, MMIO_ALLOCBUF | MMIO_READ );

	if (hWav==NULL)
		return false;

	parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if (mmioDescend(hWav, &parent, NULL, MMIO_FINDRIFF))
		return false;

	child.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(hWav, &child,  &parent, 0))
		return false;

	if (mmioRead(hWav, (char*)&wfmex, sizeof(WAVEFORMATEX)) != sizeof(WAVEFORMATEX))
		return false;

	if (mmioAscend(hWav, &child, 0))
		return false;

	child.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(hWav, &child, &parent, MMIO_FINDCHUNK))
		return false;

	ulChunkSize = child.cksize;
	ulLenthMS = 1000 * ((unsigned long)ulChunkSize / (unsigned long)wfmex.nAvgBytesPerSec);
	if(ulLenthMS < 1)
		ulLenthMS = NULL;

	libEnt->iBitRate			= wfmex.nAvgBytesPerSec;
	libEnt->iChannels			= wfmex.nChannels;
	libEnt->iSampleRate			= wfmex.nSamplesPerSec;
	libEnt->iPlaybackTime		= ulLenthMS;

	mmioClose(hWav, 0);

	return true;
}

bool			CWAVInfoManager::SetInfo(LibraryEntry * libEnt)
{
	return true;
}

unsigned long	CWAVInfoManager::GetNumberOfAlbumArts(LPTSTR		szFilename)
{
	return 0;
}

bool			CWAVInfoManager::GetAlbumArt(	LPTSTR				szFilename, 
												unsigned long		ulImageIndex,
												LPVOID			*	pImageData,
												unsigned long	*	ulImageDataSize,
												LPTSTR				szMimeType,
												unsigned long	*	ulArtType)
{
	return false;
}

bool			CWAVInfoManager::FreeAlbumArt(LPVOID				pImageData)
{
	return false;
}
