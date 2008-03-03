#include "stdafx.h"
#include "m4ainfomanager.h"


#include "mp4ff.h"
#include "utils.h"

static int GetAACTrack(mp4ff_t *infile)
{
    /* find AAC track */
    int i, rc;
    int numTracks = mp4ff_total_tracks(infile);

    for (i = 0; i < numTracks; i++)
    {
        unsigned char *buff = NULL;
        int buff_size = 0;
        mp4AudioSpecificConfig mp4ASC;

        mp4ff_get_decoder_config(infile, i, &buff, (unsigned int *)&buff_size);

        if (buff)
        {
            rc = NeAACDecAudioSpecificConfig(buff, buff_size, &mp4ASC);
            free(buff);

            if (rc < 0)
                continue;
            return i;
        }
    }

    /* can't decode this */
    return -1;
}


static uint32_t read_callback(void *user_data, void *buffer, uint32_t length)
{
    return fread(buffer, 1, length, (FILE*)user_data);
}

static uint32_t seek_callback(void *user_data, uint64_t position)
{
    return fseek((FILE*)user_data, position, SEEK_SET);
}

static uint32_t write_callback(void *user_data, void *buffer, uint32_t length)
{
    return fwrite(buffer, 1, length, (FILE*)user_data);
}

static uint32_t truncate_callback(void *user_data)
{
    chsize(fileno((FILE*)user_data), ftell((FILE*)user_data));
    return 1;
}

Cm4aInfoManager::Cm4aInfoManager(void)
{
}

Cm4aInfoManager::~Cm4aInfoManager(void)
{
}

void			Cm4aInfoManager::Destroy(void)
{
	delete this;
}

unsigned long	Cm4aInfoManager::GetNumExtensions(void)
{
	return 2;
}

LPTSTR			Cm4aInfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
	static LPTSTR exts[] = 
	{
		TEXT(".m4a"),
		TEXT(".mp4")
	};

	return exts[ulExtentionNum];
}

bool			Cm4aInfoManager::CanHandle(LPTSTR szSource)
{
	if(PathIsURL(szSource))
		return(false);

	for(unsigned int x=0; x<GetNumExtensions(); x++)
	{
		if(!StrCmpI(SupportedExtension(x), PathFindExtension(szSource)))
		{
			return(true);
		}
	}

	return(false);
}

bool			Cm4aInfoManager::GetInfo(LibraryEntry * libEnt)
{
    char *pVal = NULL;

	// stuff from ID3Lib here
	char ConvertBuffer[512];

	WideCharToMultiByte(CP_ACP, 0, libEnt->szURL, 512, ConvertBuffer, 512, NULL, NULL);

	FILE *mp4File;
	mp4ff_callback_t mp4cb = {0};
	mp4ff_t *file;

	mp4File = fopen(ConvertBuffer, "rb");
	mp4cb.read = read_callback;
	mp4cb.seek = seek_callback;
	mp4cb.write = write_callback;
	mp4cb.truncate = truncate_callback;
	mp4cb.user_data = mp4File;

	// put info extraction here!

	file = mp4ff_open_read(&mp4cb);
	if (file == NULL) 
		return false;

    if(mp4ff_meta_get_artist(file, &pVal))
    {
		MultiByteToWideChar(CP_UTF8, 0, pVal, strlen(pVal), libEnt->szArtist, 128);
    }

    if(mp4ff_meta_get_title(file, &pVal))
    {
		MultiByteToWideChar(CP_UTF8, 0, pVal, strlen(pVal), libEnt->szTitle, 128);
    }

    if(mp4ff_meta_get_album(file, &pVal))
    {
		MultiByteToWideChar(CP_UTF8, 0, pVal, strlen(pVal), libEnt->szAlbum, 128);
    }

    if(mp4ff_meta_get_genre(file, &pVal))
    {
		MultiByteToWideChar(CP_UTF8, 0, pVal, strlen(pVal), libEnt->szGenre, 128);
    }

    if(mp4ff_meta_get_comment(file, &pVal))
    {
		MultiByteToWideChar(CP_UTF8, 0, pVal, strlen(pVal), libEnt->szComment, 128);
    }

    if(mp4ff_meta_get_track(file, &pVal))
    {
		libEnt->dwTrack[0] = atoi(pVal);
	}

    if(mp4ff_meta_get_totaltracks(file, &pVal))
    {
		libEnt->dwTrack[1] = atoi(pVal);
    }

    if(mp4ff_meta_get_date(file, &pVal))
    {
		libEnt->iYear = atoi(pVal);
    }


	int track;
	if ((track = GetAACTrack(file)) < 0)
	{
	}

	libEnt->iSampleRate		= mp4ff_get_sample_rate(file, track);
	libEnt->iBitRate		= mp4ff_get_avg_bitrate(file, track);
	libEnt->iChannels		= mp4ff_get_channel_count(file, track);


	unsigned long Samples	= mp4ff_get_track_duration(file, track);
	libEnt->iPlaybackTime	= (float)Samples / (float)(libEnt->iSampleRate/1000);
		
		
	mp4ff_close(file);
	fclose(mp4File);

	return true;
}

bool			Cm4aInfoManager::SetInfo(LibraryEntry * libEnt)
{
	return true;
}

