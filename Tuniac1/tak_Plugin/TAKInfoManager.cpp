#include "stdafx.h"
#include "takinfomanager.h"
// actual class definition here

CTAKInfoManager::CTAKInfoManager(void)
{
}

CTAKInfoManager::~CTAKInfoManager(void)
{
}

void			CTAKInfoManager::Destroy(void)
{
    delete this;
}

unsigned long	CTAKInfoManager::GetNumExtensions(void)
{
    return 1;
}

LPTSTR			CTAKInfoManager::SupportedExtension(unsigned long ulExtentionNum)
{
    static LPTSTR exts[] = 
    {
        TEXT(".tak")
    };

    return exts[ulExtentionNum];
}

bool			CTAKInfoManager::CanHandle(LPTSTR szSource)
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

bool			CTAKInfoManager::GetInfo(LibraryEntry * libEnt)
{

    TtakSSDOptions				Options;
    TtakSeekableStreamDecoder	Decoder;
    TtakAPEv2Tag				TagInfo;
    Ttak_str_StreamInfo			StreamInfo;

    char tempname[MAX_PATH]; 	 
    WideCharToMultiByte(CP_UTF8, 0, libEnt->szURL, -1, tempname, MAX_PATH, 0, 0);

    Options.Cpu   = tak_Cpu_Any;

    Decoder = tak_SSD_Create_FromFile (tempname, &Options, NULL, NULL);
    if (Decoder == NULL)
        return false;
    if (tak_SSD_Valid (Decoder) != tak_True)
        return false;
    if (tak_SSD_GetStreamInfo (Decoder, &StreamInfo) != tak_res_Ok) 
        return false;

    TagInfo = tak_SSD_GetAPEv2Tag(Decoder);
    if(tak_APE_Valid(TagInfo))
    {
        unsigned int keycount = tak_APE_GetItemNum(TagInfo);
        char field_name[256];
        char field_value[256];
        int field_size;

        for(unsigned int x=0; x<keycount; x++)
        {
            tak_APE_GetItemKey(TagInfo, x, field_name, 256, &field_size);
            if(strcmp(field_name, "Album") == 0)
            {
                tak_APE_GetItemValue(TagInfo, x, field_value, 256, &field_size);
                MultiByteToWideChar(CP_UTF8, 0, field_value, field_size, libEnt->szAlbum, 128);
                continue;
            }
            if(strcmp(field_name, "Artist") == 0)
            {
                tak_APE_GetItemValue(TagInfo, x, field_value, 256, &field_size);
                MultiByteToWideChar(CP_UTF8, 0, field_value, field_size, libEnt->szArtist, 128);
                continue;
            }
            if(strcmp(field_name, "Title") == 0)
            {
                tak_APE_GetItemValue(TagInfo, x, field_value, 256, &field_size);
                MultiByteToWideChar(CP_UTF8, 0, field_value, field_size, libEnt->szTitle, 128);
                continue;
            }
            if(strcmp(field_name, "Genre") == 0)
            {
                tak_APE_GetItemValue(TagInfo, x, field_value, 256, &field_size);
                MultiByteToWideChar(CP_UTF8, 0, field_value, field_size, libEnt->szGenre, 128);
                continue;
            }
            if(strcmp(field_name, "Comment") == 0)
            {
                tak_APE_GetItemValue(TagInfo, x, field_value, 256, &field_size);
                MultiByteToWideChar(CP_UTF8, 0, field_value, field_size, libEnt->szComment, 128);
                continue;
            }
            if(strcmp(field_name, "Track") == 0)
            {
                tak_APE_GetItemValue(TagInfo, x, field_value, 256, &field_size);
                libEnt->dwTrack[0] = atoi(field_value);
                continue;
            }
            if(strcmp(field_name, "Year") == 0)
            {
                tak_APE_GetItemValue(TagInfo, x, field_value, 256, &field_size);
                libEnt->ulYear = strtoul(field_value, NULL, 10);
                continue;
            }
            
            if(strcmp(field_name, "replaygain_track_gain") == 0)
            {
                tak_APE_GetItemValue(TagInfo, x, field_value, 256, &field_size);
                libEnt->fReplayGain_Track_Gain = atof(field_value);
                continue;
            }
            if(strcmp(field_name, "replaygain_track_peak") == 0)
            {
                tak_APE_GetItemValue(TagInfo, x, field_value, 256, &field_size);
                libEnt->fReplayGain_Track_Peak = atof(field_value);
                continue;
            }
            if(strcmp(field_name, "replaygain_album_gain") == 0)
            {
                tak_APE_GetItemValue(TagInfo, x, field_value, 256, &field_size);
                libEnt->fReplayGain_Album_Gain = atof(field_value);
                continue;
            }
            if(strcmp(field_name, "replaygain_album_peak") == 0)
            {
                tak_APE_GetItemValue(TagInfo, x, field_value, 256, &field_size);
                libEnt->fReplayGain_Album_Peak = atof(field_value);
                continue;
            }
            
        }
    }

    HANDLE hFile = CreateFile(libEnt->szURL, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
                return false;

    unsigned long filesize = GetFileSize(hFile, NULL);

    CloseHandle(hFile);
    unsigned long time = StreamInfo.Sizes.SampleNum/StreamInfo.Audio.SampleRate;

    libEnt->ulBitRate			= ((filesize * 1000.0) / time) / 128;
    libEnt->ulChannels			= StreamInfo.Audio.ChannelNum;
    libEnt->ulSampleRate		= StreamInfo.Audio.SampleRate;
    libEnt->ulPlaybackTime		= time*1000;

    tak_SSD_Destroy (Decoder);

    return true;
}

bool			CTAKInfoManager::SetInfo(LibraryEntry * libEnt)
{
    return true;
}

unsigned long	CTAKInfoManager::GetNumberOfAlbumArts(void)
{
    return 0;
}

bool			CTAKInfoManager::GetAlbumArt(	unsigned long		ulImageIndex,
                                                LPVOID			*	pImageData,
                                                unsigned long	*	ulImageDataSize,
                                                LPTSTR				szMimeType,
                                                unsigned long	*	ulArtType)
{
    return false;
}

bool			CTAKInfoManager::FreeAlbumArt(LPVOID				pImageData)
{
    return false;
}
