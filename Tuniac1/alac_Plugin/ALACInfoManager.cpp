#include "stdafx.h"
#include <wchar.h>
#include "alacinfomanager.h"
// actual class definition here

CALACInfoManager::CALACInfoManager(void)
{
}

CALACInfoManager::~CALACInfoManager(void)
{
}

void			CALACInfoManager::Destroy(void)
{
	delete this;
}

unsigned long	CALACInfoManager::GetNumExtensions(void)
{
	return 1;
}

LPTSTR			CALACInfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
	static LPTSTR exts[] = 
	{
		TEXT(".alac")
	};

	return exts[ulExtentionNum];
}

bool			CALACInfoManager::CanHandle(LPTSTR szSource)
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

bool			CALACInfoManager::GetInfo(LibraryEntry * libEnt)
{

	demux_res_t demux;

    FILE *m_file = _wfopen(libEnt->szURL, TEXT("rbS"));
    if(m_file == NULL)
		return false;

	stream_t *stream = stream_create_file(m_file, 1);

	if(!qtmovie_read(stream, &demux)) {
		//ALAC: alac_decode: failed to load the QuickTime movie headers!
		return false;
	}


	HANDLE hFile = CreateFile(libEnt->szURL, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
				return false;

	unsigned long filesize = GetFileSize(hFile, NULL);

	CloseHandle(hFile);


	unsigned int i;
	unsigned long output_time = 0;
	unsigned int index = 0;
	unsigned int accum = 0;
    for (i = 0; i < demux.num_sample_byte_sizes; i++)
    {
        unsigned int thissample_duration;

	while((demux.time_to_sample[index].sample_count + accum) <= i) {
		accum += demux.time_to_sample[index].sample_count;
    		index++;
		if(index >= demux.num_time_to_samples) {
			//ALAC: get_song_length: sample %i does not have a duration
			return false;
		}
	}
    
    thissample_duration = demux.time_to_sample[index].sample_duration;
    output_time += thissample_duration;
    }
	// ( Sample_count / sample_rate ) * 1000 ---> to get time in milliseconds


	unsigned long time = (output_time / demux.sample_rate);


	libEnt->iBitRate			= ((filesize * 1000.0) / time) / 128;
	libEnt->iChannels			= demux.num_channels;
	libEnt->iSampleRate			= demux.sample_rate;
	libEnt->iPlaybackTime		= time * 1000;


	stream_destroy(stream);
	return true;
}

bool			CALACInfoManager::SetInfo(LibraryEntry * libEnt)
{
	return true;
}

unsigned long	CALACInfoManager::GetNumberOfAlbumArts(LPTSTR		szFilename)
{
	return 0;
}

bool			CALACInfoManager::GetAlbumArt(	LPTSTR				szFilename, 
												unsigned long		ulImageIndex,
												LPVOID			*	pImageData,
												unsigned long	*	ulImageDataSize,
												LPTSTR				szMimeType,
												unsigned long	*	ulArtType)
{
	return false;
}

bool			CALACInfoManager::FreeAlbumArt(LPVOID				pImageData)
{
	return false;
}
